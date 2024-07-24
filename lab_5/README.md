### Laboratorio 5 - Sistemas Operativos II
## Ingeniería en Compitación - FCEFyN - UNC - 2023
# Sistemas Embebidos

## Introducción
Los _sistemas embebidos_ suelen ser accedidos de manera remota. Existen distintas tenicas para hacerlo, una forma muy utilizada suelen ser las _RESTful APIs_. Estas, brindan una interfaz definida y robusta para la comunicación y manipulación del _sistema embebido_ de manera remota. Definidas para un esquema _Cliente-Servidor_ se utilizan en todas las verticales de la industria tecnológica, desde aplicaciones de _IoT_ hasta juegos multijugador.

## Objetivo
El objetivo del presente trabajo práctico es que el estudiante tenga un visión _end to end_ de una implementación básica de una _RESTful API_ sobre un _sistema embedido_.
El estudiante deberá implementarlo interactuando con todas las capas del procesos. Desde el _testing_ funcional (alto nivel) hasta el código en C del servicio (bajo nivel).


### Desarrollo
En esta primer etapa se pide que se implemente un framework que exponga una [REST API](https://en.wikipedia.org/wiki/Representational_state_transfer): se propone utilizar [ulfius][ulfi]. El estudiante puede seleccionar otro, justificando la selección, o implementar el propio (no recomendado).

Se pide que se implemente los siguientes endopints:
- `POST: http://localhost:8537/increment`: Incrementa un contador interno 
- `GET: http://localhost:8537/imprimir`: Devuelve el valor del contador 

Se deberá utilizar y adaptar el cliente IPv4 del Laboratorio 2 para que envien request a ambos endpoints de la REST API. Tambien se puede utilizar el comando [curl](https://curl.se/docs/manpage.html).

La validación del sistema se debe realizar mediante la creación de al menos 100 procesos que interactúen con los endpoints creados.

## Entrega
Se deberá proveer los archivos fuente, así como cualquier otro archivo asociado a la compilación, archivos de proyecto CMAKE y el código correctamente documentado, todo en el repositorio, donde le Estudiante debe demostrar avances semana a semana mediante _commits_.

También se debe entregar un informe, guia tipo _How to_, explicando paso a paso lo realizado (puede ser un _Markdown_). El informe además debe contener el diseño de la solución con una explicacion detallada de la misma. Se debe asumir que las pruebas de compilación se realizarán en un equipo que cuenta con las herramientas típicas de consola para el desarrollo de programas (Ejemplo: gcc, make), y NO se cuenta con herramientas "GUI" para la compilación de los mismos (Ej: eclipse).


### Criterios de Corrección
- Se debe compilar el código con los flags de compilación: 
    `-Wall -Pedantic -Werror -Wextra -Wconversion -std=gnu11`
- La correcta gestion de memoria.
- Dividir el código en módulos de manera juiciosa.
- Estilo de código.
- Manejo de errores
- El código no debe contener errores, ni warnings.
- El código no debe contener errores de cppcheck.


## Evaluación
El presente trabajo práctico es individual deberá entregarse antes del viernes 12 de Mayo de 2023 a las 16:55 mediante el LEV. Será corregido y luego deberá coordinar una fecha para la defensa oral del mismo.

## Referencias y ayudas
- [Ulfius HTTP Framework](https://github.com/babelouest/ulfius)


[ulfi]: https://github.com/babelouest/ulfius
