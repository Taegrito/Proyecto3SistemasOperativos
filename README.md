# Proyecto de Sistemas Operativos

## Proyecto 3: Tablas de páginas en acción (xv6)

### Integrantes
- Ana Isabella Gómez García

### Curso
Sistemas Operativos

### Semestre
2026-2

---

# Descripción

## Parte A:
Se implementó una página de memoria compartida que permite a los procesos consultar su PID, sin tener que realizar una llamada al sistema tradicional. Se garantizó también la liberación correcta de la memoria y la protección de solo lectura.

## Parte B:
Se realizó una función que recorre e imprime de manera jerárquica las tablas de página de un proceso, mostrando las entradas válidas, sus direcciones virtuales y direcciones físicas.

## Parte C:
Se implementó una llamada al sistema que identifica las páginas de memoria que han sido accedidas mediante el bit `PTE_A`, almacena los resultados y limpia el indicador para futuras comprobaciones.

---

# Archivos modificados

- **Kernel/memlayout.h**: Se definió la dirección virtual `USYSCALL` y la estructura `struct usyscall`, que contiene el campo `pid`.
- **Kernel/proc.h**: Se agregó el campo `struct usyscall *usyscall` a la estructura `proc`, para almacenar la referencia a la página compartida de cada proceso.
- **Kernel/vm.c**: Se implementó `vmprint()` y una función auxiliar recursiva para recorrer los tres niveles de las tablas de páginas. Se imprimen las entradas válidas, sus direcciones virtuales, valores PTE y direcciones físicas.
- **Kernel/defs.h**: Se agregó la declaración de `vmprint(pagetable_t pagetable)` para permitir su utilización desde otros archivos del kernel.
- **Kernel/proc.c**: Se reservó una página física para `USYSCALL` mediante `kalloc()`, se almacenó el PID del proceso, se mapeó la página con permisos `PTE_R | PTE_U` y se agregó su liberación mediante `kfree()`.
- **Kernel/sysproc.c**: Se implementó `sys_vmprint()`, que obtiene la tabla de páginas del proceso actual y llama a `vmprint()`. Se implementó `sys_pgaccess()`, que recibe la dirección base, la cantidad de páginas y la dirección de la máscara de resultados. Recorre las páginas, verifica el bit `PTE_A`, construye la máscara y limpia el bit de acceso.
- **Kernel/syscall.h**: Se agregó el identificador `SYS_vmprint` para registrar la llamada al sistema. Se agregó el identificador `SYS_pgaccess`.
- **Kernel/syscall.c**: Se agregó la declaración de `sys_vmprint()` y su correspondiente entrada en la tabla de llamadas al sistema. Se agregó la declaración de `sys_pgaccess()` y su registro en la tabla de llamadas al sistema.
- **user/ulib.c**: Se implementó `ugetpid()`, que consulta directamente la dirección `USYSCALL` y devuelve el PID almacenado.
- **user/user.h**: Se agregó el prototipo de la función `int ugetpid(void);`. Se agregó el prototipo `int vmprint(void);`. Se agregó el prototipo `int pgaccess(void *base, int npages, uint64 *mask);`.
- **user/usys.pl**: Se agregó `entry("vmprint")` para generar el código que permite invocar la llamada desde el espacio de usuario. Se agregó `entry("pgaccess")` para generar el código de invocación de la llamada al sistema.
- **user/usys.S**: Se generó el código ensamblador correspondiente a `vmprint` mediante `user/usys.pl`. Se generó el código ensamblador correspondiente a `pgaccess`.

---

# Integración con xv6

1. Clonar xv6-riscv y hacer `git switch -c proyecto3 06aad25`.
2. Copiar los archivos entregados en sus rutas.
3. Agregar `$U/_pgtbltest` a `UPROGS` en el Makefile.
4. Compilar con `make qemu`.
5. Ejecutar `pgtbltest` y `usertests -q`.

---

# Respuestas a las preguntas

## 1. Interpretación de vmprint()
## 2. Página de guarda
## 3. Otras llamadas que se pueden acelerar
## 4. Riesgo de USYSCALL con PTE_W
## 5. pgaccess() y páginas perezosas

---

# Resultados de las pruebas

Resumen de `pgtbltest` y `usertests` (salida completa en `pruebas/`).

---

# Decisiones de diseño

- Uso de una página compartida por proceso.
- Consulta directa del PID.
- Protección de la información.
- Ciclo de vida de la memoria.
- Recorrido recursivo.
- Impresión únicamente de entradas válidas.
- Representación jerárquica.
- Separación de responsabilidades.
- Uso del bit de acceso: `PTE_A`.
- Validación de argumentos.

---

# Limitaciones conocidas

Múltiples errores a la hora de realizar las pruebas, en un momento funcionaban y luego al volver a ejecutar el código faltaban cosas.

---

# Declaración de uso de IA

Las herramientas usadas para este proyecto fueron ChatGPT, para la detección y corrección de errores y para consultas externas relacionadas con el proyecto.
