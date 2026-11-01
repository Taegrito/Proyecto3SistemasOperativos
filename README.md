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
| Págins | Contenido | Permisos principales |
|---|---|---|
| Text | Instrucciones del programa ejecutable | PTE_R y PTE_X: permite leer y ejecutar, pero no escribir.|
| Data | Variables globales y estáticas del programa| PTE_R y PTE_W: permite leer y escribir.|
| Guard|Página ubicada debajo de la pila, para detectar desbordamientos.|El programa no puyede acceder a ella a través del modo usuario|
| Stack | 	Variables locales, direcciones de retorno y datos de las funciones. |PTE_R, PTE_W y PTE_U: permite leer y escribir desde modo usuario.|
| USYSCALL | Contiene información compartida del proceso, especialmente su PID.| PTE_R y PTE_U: el programa puede leerla, pero no modificarla.|
| Trapframe | 	Guarda el contexto de ejecución del proceso durante las transiciones entre modo usuario y modo kernel.| No tiene PTE_U, ya que es una página reservada para uso del kernel. |
## 2. Página de guarda
La página de guarda no tiene el bit de PTE_u, ya que su objetivo es impedir que un programa accceda a la memoria en la que se encuentra debajo de su pila.
Si el programa intenta utilizar más espacio de la pila , terminará intentando acceder a la página de guarda.Como esta no permite el acceso desde modo usuario, se produce una excepción de memoria y el kernel puede finalizar el proceso.
## 3. Otras llamadas que se pueden acelerar
Se podrían acelerar aquellas llamadas que consultan información del proceso que se mantiene disponible y que no requiere modificaciones constantes.
## 4. Riesgo de USYSCALL con PTE_W
Si USYSCALL se mapeara con PTE_W, el programa tendría permiso para modificar su contenido desde modo usuario.

Esto representaría un riesgo porque podría alterar información que debe ser administrada exclusivamente por el kernel, como el PID del proceso. Por ejemplo, un programa podría cambiar el valor de su PID almacenado en la página compartida y obtener información incorrecta al consultar ugetpid().
## 5. pgaccess() y páginas perezosas
sbrklazy() aumenta el tamaño lógico del espacio de direcciones del proceso, pero no asigna inmediatamente memoria física para las páginas solicitadas. La memoria se asigna de manera diferida (lazy allocation), únicamente cuando el programa intenta acceder a ella.
---

# Resultados de las pruebas
* Salidas completas:
  - pgtbltest.txt
  - usertest.txt
* E pgtbltest:
  - Resultado: 11 de 11 pruebas aprobadas.
  - Se verificó `ugetpid()` en procesos normales, hijos y después de `exec()`.
  - Se comprobó la protección de solo lectura de USYSCALL.
  - Se validaron la impresión de tablas de páginas (`vmprint`) y la detección de páginas accedidas (`pgaccess`).
  - La revisión automática del formato de `vmprint()` también fue aprobada.
* usertst -q
  - Resultado: ALL TESTS PASSED.
  - Se aprobaron las pruebas de memoria, procesos, archivos, asignación perezosa y manejo de páginas.
  - No se produjo ningún kernel panic.
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
