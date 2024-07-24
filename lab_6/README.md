### Lab6 Sistemas Operativos II
## Ingeniería en Compuatación - FCEFyN - UNC
# Sistemas Embebidos

## Introducción
Los _sistemas embebidos_ suelen ser accedidos de manera remota. Existen distintas técnicas para hacerlo, una forma muy utilizada suelen ser las _RESTful APIs_. Estas, brindan una interfaz definida y robusta para la comunicación y manipulación del _sistema embebido_ de manera remota. Definidas para un esquema _Cliente-Servidor_ se utilizan en todas las verticales de la industria tecnológica, desde aplicaciones de _IoT_ hasta juegos multijugador.

## Objetivo
El objetivo del presente trabajo práctico es que el estudiante tenga un visión _end to end_ de una implementación básica de una _RESTful API_ sobre un _sistema embedido_ trabajando con aplicaciones que se asemejan a la vida profesional, a través de diferentes stacks de lenguajes.
El estudiante deberá implementarlo interactuando con todas las capas del procesos. Desde el _testing_ funcional (alto nivel) hasta el código en C y Go del servicio (bajo nivel).

## Desarrollo
### Requerimientos
Para realizar el presente trabajo practico, es necesario una computadora con _kernel_ GNU/Linux, ya que usaremos [SystemD][sysD] para implementar el manejo de nuestro servicios.

### Desarrollo
Se dispone de N sensores de IoT conectados a un servidor escrito en Go el cual recibe de estos una serie de parámetros que se listan a continuación. El servidor cuenta con un _servicio de usuarios_ y el _servicio de procesamiento_.  Cada servicio deberá exponer una _REST API_ con _Media Type_ `application/json` [about mediatype] para todas sus funcionalidades y del lado de usuarios, solo permitir la operación a usuarios autentificados.
El servicio debe tener configurado un [nginx][ngnx] por delante para poder direccionar el _request_ al servicio correspondiente.

El web server deberá *solo* responder a `dashboard.com` para el servicio de usuarios y `sensors.com` para el servicio de procesamiento. Debe retornar _404 Not Found_ para cualquier otro _path_ no existente con algún mensaje a elección con formato JSON.
A modo de simplificación, usaremos sólo _HTTP_, pero aclarando que esto posee *graves problemas de seguridad*.

A continuación, detallaremos los dos servicios a crear y las funcionalidades de cada uno.

### Servicio de Usuarios
Este servicio se encargará de crear usuarios y listarlos. Estos usuarios deberán poder _logearse_ vía _SSH_ luego de su creación. Solo podrán acceder aquellos usuarios autentificados. Y las tareas de cada endpoint usando [JWT][JWT]

#### POST /api/users
Endpoints para la creación de usuario en el sistema operativo:

```C
    GET http://{{server}}/api/users/login
```

```C
    POST http://{{server}}/api/users/createuser
```
Request
```C
        curl --request POST \
            --url http:// {server}}/api/users \
            -u USER:SECRET \
            --header 'accept: application/json' \
            --header 'content-type: application/json' \
            --data '{"username": "myuser", "password": "mypassword"}' \
            --authentification jwt
```
Respuesta
```C

        {
            "id": 142,
            "username": "myuser",
            "created_at": "2019-06-22 02:19:59"
        }

```


#### GET /api/users
Endpoint para obtener todos los usuarios del sistema operativo y sus identificadores.
```C
    GET http://{{server}}/api/users/listall
```
Request
```C
    curl --request GET \
        --url http://{{server}}/api/users \
        -u USER:SECRET \
        --header 'accept: application/json' \
        --header 'content-type: application/json' \
        --authentification jwt
```
Respuesta
```C
    {
      "data": [
          {
              "user_id": 2,
              "username": "user1",
          },
          {
              "user_id": 1,
              "username": "user2"
          },
          ...
      ]
    }
```

### Servicio de procesamiento
Debe listar un _Media Type_ , `application/json`. Con la información de: procesamiento, memoria libre, swap, etc.

#### POST /processing/submit
```C
    POST http://{{server}}/processing/submit
```
Request

```C
    curl --request POST \
        --url http://{{server}}/contador/increment \
        -u USER:SECRET \
        --header 'accept: application/json' \
        --header 'content-type: application/json'
```


#### GET /processing/summary
Este endpoint permite saber el valor de todos los sensores en promedio.
```C
    GET http://{{server}}/processing/summary
```
Request
```C
    curl --request GET \
        --url http://{{server}}/contador/value \
        -u USER:SECRET \
        --header 'accept: application/json' \
        --header 'content-type: application/json'
```

Este endpoint no tiene ningún requerimiento de para logging.

## Entrega
Se deberá proveer los archivos fuente, así como cualquier otro archivo asociado a la compilación, archivos de proyecto "Makefile" y el código correctamente documentado, todo en el repositorio, donde le Estudiante debe demostrar avances semana a semana mediante _commits_.

También se debe entregar un informe, guia tipo _How to_, explicando paso a paso lo realizado (puede ser un _Markdown_). El informe además debe contener el diseño de la solución con una explicacion detallada de la misma. Se debe asumir que las pruebas de compilación se realizarán en un equipo que cuenta con las herramientas típicas de consola para el desarrollo de programas (Ejemplo: gcc, make), y NO se cuenta con herramientas "GUI" para la compilación de los mismos (Ej: eclipse).

El install del makefile deberá copiar los archivos de configuración de systemd para poder luego ser habilitados y ejecutados por linea de comando.
El script debe copiar los archivos necesarios para el servicio Nginx systemd para poder luego ser habilitados y ejecutados por linea de comando.
Los servicios deberán pasar una batería de test escritas en _postman_ provistas. TBD.

### Criterios de Corrección
- Dividir el código en módulos de manera juiciosa.
- Estilo de código.
- Manejo de errores
- El código no debe contener errores de staticcheck.


## Evaluación
El presente trabajo práctico es individual deberá entregarse antes del viernes 02 de Junio de 2023 a las 23:55 mediante el LEV.  Será corregido y luego deberá coordinar una fecha para la defensa oral del mismo.

## Referencias y ayudas
- [Systrem D ](https://systemd.io/)
- [System D en Freedesktop](https://www.freedesktop.org/wiki/Software/systemd/)
- [nginx](https://docs.nginx.com/)
- [Kore Web PLataform](https://kore.io/)

[sysD]: https://www.freedesktop.org/wiki/Software/systemd/
[ngnx]: https://docs.nginx.com/
[ulfi]: https://github.com/babelouest/ulfius
[logrotate]: https://en.wikipedia.org/wiki/Log_rotation
[mediatype]: https://en.wikipedia.org/wiki/Media_type
[JWT]: https://jwt.io