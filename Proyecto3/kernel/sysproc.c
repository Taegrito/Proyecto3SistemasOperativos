#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}


uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}
uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);

  addr = myproc()->sz;

  if (n < 0 && growproc(n) < 0)
    return -1;

  if (n > 0) {
    if (addr + n < addr)
      return -1;
    if (addr + n > USYSCALL)
      return -1;
  }

  if (t == SBRK_EAGER || n < 0) {
    if (n > 0 && growproc(n) < 0)
      return -1;
  } else {
    myproc()->sz += n;
  }

  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
uint64
sys_vmprint(void)
{
  struct proc *p = myproc();

  vmprint(p->pagetable);

  return 0;
}
uint64
sys_pgaccess(void)
{
  uint64 base;
  uint64 maskaddr;
  uint64 maskbits = 0;
  uint64 end;
  int npages;
  struct proc *p = myproc();

  argaddr(0, &base);
  argint(1, &npages);
  argaddr(2, &maskaddr);

  if ((base % PGSIZE) != 0 ||
      npages < 1 ||
      npages > 64) {
    return -1;
  }

  end = base + (uint64)npages * PGSIZE;

  if (end < base || end > p->sz) {
    return -1;
  }

  for (int i = 0; i < npages; i++) {
    uint64 va = base + (uint64)i * PGSIZE;
    pte_t *pte = walk(p->pagetable, va, 0);

    if (pte == 0 || (*pte & PTE_V) == 0)
      continue;

    if ((*pte & PTE_U) == 0)
      continue;

    if (*pte & PTE_A) {
      maskbits |= (1ULL << i);
      *pte &= ~PTE_A;
    }
  }
  if (copyout(p->pagetable, p->sz, maskaddr,
              (char *)&maskbits, sizeof(maskbits)) < 0) {
    return -1;
  }

  return 0;
}
