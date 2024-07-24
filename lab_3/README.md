Sistemas Operativos II - Laboratorio II IPC
###  Ingeniería en Compitación - FCEFyN - UNC - 2023
# Laboratorio 3 - Introducción a la programación eficiente y performante

## Introducción
Tanto para cuando estamos trabajando en un sistema embedido de recursos limitados (cores, frecuencia, memoria, etc.) o cuando estamos trabajando en software de servicios que debe poder escalar y repsonder en un tiempo determinado, como cuando estamos trabajando en computación científica, tener un software eficiente y performante es responsabilidad nuestra.

Es evidente que, para cumplir este objetivo, hay que conocer la aquitectura y recursos destino del software que vamos a escribir.

### ¿Qué entendemos por _un software eficiente_?
Intuitivamente podemos decir que un software eficiente (high performance) es uno que se ejecuta más rápido que uno no eficiente, pero esto no es suficiente, porque puede que sea más rápido, pero que ambos sean no eficientes.

Hay que entender que eficiencia no es lo mismo que perforance, pero estan relacionadas. Eficiencia se refiere al uso optimo de los recursos y el desperdicio de los mismos. Un programa eficiente realiza un buen uso de los recursos de hardware, no deja ningún recurso en estado idle, ni raliza tareas que no tiene que hacer.

Por otro lado, performance siempre se refiere a métricas. La métrica más común es la velocidad, es decir, cuan rápido es el software. Se puede utilizar la variable tiempo para medir esta métrica, pero no es la única (clocks, instrucciones, etc.).

## Desarrollo
### Problema a resolver
Dado el archibo laboratorio3.c en el presente repo, se pide que el estudiante lo analice, estudie y le realice toda tardea de optimización, eficiencia y refactorización que considere para que el software sea mas óptimo en consumo de recursos.

1 - Se le debe medir y documentar todas las optimizaciones realizadas utilizando el comando _time_ y cualquier otro analizador estático, usando como base el software tal cómo esta presentado.

2 - Comenzando desde cero, se debe medir y documentar todas las optimizaciones realizadas utilizando el el analizador dinámico [valgrind](https://valgrind.org/).

3 - Comparar ambas versiones: la propia y la realizada con valgrind:
 - a: ¿En qué se diferencian?
 - b: ¿Cuál es mejor? ¿Porqué?

4 - Debe diseñar y desarrollar un unit test que valide el software (NO usando un framework, sólo un script).

### Restricciones
- Se puede utilizar tanto GCC u otro compilador
- La compilación debe realizarse sin [optimizaciones de compilación (-O0)](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)

### Criterios de Corrección
- Se debe compilar el código con los flags de compilación: 
     -Wall -Pedantic -Werror -Wextra -Wconversion -std=gnu11
- La correcta gestion de memoria.
- Dividir el código en módulos de manera juiciosa.
- Estilo de código.
- Manejo de errores
- El código no debe contener errores, ni warnings.
- El código no debe contener errores de cppcheck.

## Entrega
La entrega se hace a travéz del repositorio de GitHub y se deberá demostrar la realizacion del mismo mediante un correcto uso. El repositorio deberá proveer los archivos fuente y cualquier otro archivo asociados a la compilación, archivos  de  proyecto  CMAKE  y  el  código correctamente documentado. No debe contener ningún archivo asociado a proyectos de un IDE y se debe asumir que el proyecto podrá ser compilado y corrido por una `tty` en una distribución de Linux con las herramientas típicas de desarrollo. También se deberá entregar un informe en formato _Markdown_ documentando cada cambio que se le realiza al código, que efecto produce sobre el mismo y porqué?
También se deberá investigar acerca de qué utilidades de profiling gratuitas existen y que brinda cada una (un capítulo del informe), optando por una para realizar las mediciones de tiempo de ejecución de la aplicación diseñada.

El informe debe contener gráficos y análisis de comparación entre la ejecución procedural y la distribuida. El informe además debe contener el diseño de la solución y la comparativa de profilers.

## Evaluación
El presente trabajo práctico es individual y deberá entregarse antes de las 14:59ART del día 20 de Abril de 2023 dejando asentado en el LEV con el archivo de ifnorme.


## Links
- [The Art of Writing Efficient Programs](https://falksangdata.no/wp-content/uploads/2022/07/Pikus_The.art_.of_.writing.efficient.programs.pdf), Fedor G. Pikus, Packt Publishing
- [Valgrind](https://valgrind.org/)

