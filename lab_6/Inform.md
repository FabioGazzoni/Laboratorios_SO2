# Laboratorio n°6 - Servicio para sistemas embebidos.

## Descripción
Se dispone a levantar un servicio de API Rest para un sistema de sensores con registro de usuarios, para su uso un usuario debe crear una cuenta nueva y iniciar sesión, donde obtendrá un token de JWT con un TimeOut establecido, una vez transcurrido este tiempo se debe volver a iniciar sesión. El usuario podrá cargar un log de datos de memoria con el token proporcionado y leer el registro de todos los logs (para esta acción no es necesario el token).

## Librerías y utilidades necesarias
### Nginx
```
sudo apt install nginx
```

### Go modules
```
go get -u github.com/gin-gonic/gin
go get -u golang.org/x/crypto/ssh
go get -u github.com/golang-jwt/jwt/v5
```

## Implementación

### Servicio en Go

Por parte del servicio se dispone de dos tipos de servicios bien distinguidos, uno es el servicio de usuarios, donde se podrá crear un nuevo usuario, iniciar sesión y obtener la lista completa de usuarios existentes.

#### Servicio de usuarios

Cuando un nuevo usuario se registra el servicio recibe un JSON con los datos de usuario y contraseña para esta nueva cuenta, se guardará un registro con estos datos mas un identificador y la hora del registro.
```go
func createUser(c *gin.Context) {
	var newUser UserGETResponse
	if err := c.ShouldBindJSON(&newUser); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	//Crear usuario en el sistema
	err := newUserSO(newUser)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": err.Error()})
		return
	}

	mutex.Lock()
	id++
	mutex.Unlock()

	newUserPOST := UserData{
		ID:        id,
		Username:  newUser.Username,
		Password:  newUser.Password,
		CreatedAt: time.Now().Format("2006-01-02 15:04:05"),
	}

	users = append(users, newUserPOST)

	response := UserPOSTResponse{
		ID:        newUserPOST.ID,
		Username:  newUserPOST.Username,
		CreatedAt: newUserPOST.CreatedAt,
	}
	//Responde con usersPOST
	c.JSON(http.StatusOK, response)
}
```

Ademas de lo anterior, se crea un nuevo usuario en el SO del servidor.
```go
func newUserSO(newUser UserGETResponse) error {
	//Crear usuario en el sistema
	cmd := exec.Command("sudo", "useradd", "-m", newUser.Username, "-s", "/bin/bash")
	err := cmd.Run()
	if err != nil {
		return err
	}

	//Establecer contraseña del usuario
	cmd = exec.Command("sudo", "chpasswd")
	cmd.Stdin = strings.NewReader(newUser.Username + ":" + newUser.Password)
	err = cmd.Run()
	if err != nil {
		return err
	}

	return nil
}
```

El endpoint que responde a esta petición es `GET http://dashboard.com/api/users/createuser`

```
curl --request POST \
        --url http://dashboard.com/api/users/createuser \
        -u USER:SECRET \
        --header 'accept: application/json' \
        --header 'content-type: application/json' \
        --data '{"username": "USER", "password": "PASSWORD"}'
```


Luego para iniciar sesión se espera en un JSON el usuario y contraseña, si estos corresponden a algún usuario existente se responde con un token JWT.
```go
func login(c *gin.Context) {
	var loginUser UserGETResponse
	if err := c.ShouldBindJSON(&loginUser); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	//Verificar si el usuario existe y si la contraseña es correcta
	isUser := false
	for _, u := range users {
		if u.Username == loginUser.Username && u.Password == loginUser.Password {
			isUser = true
			break
		}
	}

	if isUser {
		//Abrir conexión SSH
		client, err := sshConnection(loginUser)
		if err != nil {
			c.JSON(http.StatusInternalServerError, gin.H{"error": err.Error()})
			return
		}

		//Crear token JWT
		token, err := newToken(loginUser.Username)

		if err != nil {
			c.JSON(http.StatusInternalServerError, gin.H{"error": err.Error()})
			return
		}

		//Cerrar conexión SSH al finalizar
		defer client.Close()
		c.JSON(http.StatusOK, gin.H{"token": token})
		return
	} else {
		c.JSON(http.StatusUnauthorized, gin.H{"message": "Login failed"})
		return
	}
}
```
El endpoint que responde a esta petición es `POST http://dashboard.com/api/users/login`

```
curl --request POST \
        --url http://dashboard.com/api/users/login \
        -u USER:SECRET \
        --header 'accept: application/json' \
        --header 'content-type: application/json' \
        --data '{"username": "USER", "password": "PASSWORD"}'
```

Por ultimo se dispone de la petición de la lista de usuarios existentes, para ello es necesario enviar el token anteriormente generado.

```go
func getUsers(c *gin.Context) {
	token := c.GetHeader("token")
	if token == "" {
		c.JSON(http.StatusUnauthorized, gin.H{"message": "Token empty"})
		return
	}

	//Validar token
	isValid, err := verifyToken(token)
	if err == nil && isValid {

		var usersResponse []UserGETResponse
		for _, u := range users {
			usersResponse = append(usersResponse, UserGETResponse{
				Username: u.Username,
				Password: u.Password,
			})
		}

		c.JSON(http.StatusOK, gin.H{"data": usersResponse})
		return
	} else {
		c.JSON(http.StatusUnauthorized, gin.H{"message": "Unauthorized token"})
		return
	}
}
```
El endpoint que responde a esta petición es `GET http://dashboard.com/api/users/listall`

```
curl --request GET \
        -H "Token: TOKEN" \
        --url http://dashboard.com/api/users/listall
```

#### Servicio de sensores

Por otra parte está el servicio de sensores, estos reciben datos del estado de la memoria de los usuarios. Para ello los usuarios envían estos datos a traves de una petición POST, es necesario que el usuario envíe su token, ademas de su nombre de usuario.

```go
func processSensorData(c *gin.Context) {
	var newSensorData SensorData
	if err := c.ShouldBindJSON(&newSensorData); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	isUser := false
	for _, user := range users {
		if user.Username == newSensorData.Username {
			isUser = true
		}
	}

	if !isUser {
		c.JSON(http.StatusUnauthorized, gin.H{"error": "Usuario no autorizado"})
		return
	}

	token := c.GetHeader("Token")
	if token == "" {
		c.JSON(http.StatusUnauthorized, gin.H{"message": "token empty"})
		return
	}

	//Validar token
	isValid, err := verifyToken(token)
	if err != nil || !isValid {

		sensorsData = append(sensorsData, newSensorData)
		c.JSON(http.StatusOK, newSensorData)
		return
	} else {
		c.JSON(http.StatusUnauthorized, gin.H{"message": "Unauthorized token"})
		return
	}

}
```

El endpoint que responde a esta petición es `POST http://sensors.com/api/process/sensordata`

```
curl --request POST \
        --url http://sensors.com/api/process/sensordata \
        -u USER:SECRET \
        --header 'accept: application/json' \
        --header 'content-type: application/json' \
        --header 'Token: Bearer TOKEN' \
        --data '{"username": "USER", "total_memory": FLOATDATA, "free_memory": FLOATDATA, "swap_memory":FLOATDATA}'
```

Para obtener la lista completa de estos logs no es necesario iniciar sesión.

```go
func getSummary(c *gin.Context) {
	c.JSON(http.StatusOK, gin.H{"data": sensorsData})
}
```

El endpoint que responde a esta petición es `GET http://sensors.com/api/process/summary`

```
curl --request GET \
        --url http://sensors.com/api/process/summary
```

#### Nginx para el redireccionamiénto
Para que los endpoints funcionen con las url ` http://sensors.com` y `http://dashboard.com` se utilizará Nginx, para ello (una vez ya instalado el framework) se configura el archivo ubicado en `/etc/nginx/sites-available/defaul` de la forma:

```
server {
    listen 80;
    server_name dashboard.com;

    location / {
        proxy_pass http://localhost:PUERTO1;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}

server {
    listen 80;
    server_name sensors.com;

    location / {
        proxy_pass http://localhost:PUERTO2;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

En este caso `PUERTO1 = 8081` y `PUERTO2 = 8082`.

Puede ocurrir que el DNS no encuentre estas direcciones por lo que se debe modificar el archivo `/etc/hosts`, agregando:

```
127.0.0.1	dashboard.com
127.0.0.1	sensors.com
```

Cabe destacar que de esta forma los demás dispositivos en la red seguirán sin encontrar estas direcciones, esta es solo una solución para el dispositivo host. Para una solución en la red local se debe modificar el DNS de la puerta de enlace.

Una vez listo se recarga el servicio con:
```
sudo systemctl reload nginx
```

#### Inicio del servicio en Systemd
Para que el servicio se ejecute por el sistema del SO se debe agregar la configuración para que se ejecute, se debe crear un archivo nuevo en `/etc/systemd/system/` de la forma `NOMBRESERVICIO.service`

```
[Unit]
Description= Servicio para lab6 SOII
After=network.target
StartLimitIntervalSec=0
[Service]
Type=simple
Restart=always
RestartSec=1
User=root
ExecStart=PATHDELEJECUTABLE
[Install]
WantedBy=multi-user.target
```

Ya que el servicio crea nuevos usuarios en el SO es necesario que se ejecute como root. El archivo que debe cargarse es el binario ya compilado, para ello donde se encuentra el main del proyecto se debe ejecutar:

```
go build
```
Se debe levantar el servicio con:
```
sudo systemctl daemon-reload
sudo systemctl start NOMBRESERVICIO.service
sudo systemctl enable sudo systemctl enable NOMBRESERVICIO.service
```

## Tests del servicio
Se realizaron una serie de tests con un script de python para verificar el correcto funcionamiento del servicio. El resultado de este test imprime correctamente los datos obtenidos.

```
-------------------- Creación de usuarios --------------------
{"id":1,"username":"user1","created_at":"2024-06-06 14:11:09"}
{"id":2,"username":"user2","created_at":"2024-06-06 14:11:09"}
{"id":3,"username":"user3","created_at":"2024-06-06 14:11:09"}
{"id":4,"username":"user4","created_at":"2024-06-06 14:11:09"}
{"id":5,"username":"user5","created_at":"2024-06-06 14:11:09"}
-------------------- Tokens de acceso --------------------
Token para user1: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MTc2OTQ0NjksInVzZXJuYW1lIjoidXNlcjEifQ.eGoc2gcuaFGdNm4JXVZlcGDNPTbO6WiXinUCQsTMr8g
Token para user2: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MTc2OTQ0NzAsInVzZXJuYW1lIjoidXNlcjIifQ.9aTSPLbfeJGXvuyiLPjqNZU6pXZhXlUu1zzscZkCThE
Token para user3: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MTc2OTQ0NzAsInVzZXJuYW1lIjoidXNlcjMifQ.-rirV5Fn5diZQbWlIVeb9mlTmAx17ofJn2WtKmb1n2I
Token para user4: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MTc2OTQ0NzAsInVzZXJuYW1lIjoidXNlcjQifQ.tv855jURnPuKso0aP9dn6GUPZrTOKBMLXLWmv5XU2_s
Token para user5: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MTc2OTQ0NzAsInVzZXJuYW1lIjoidXNlcjUifQ.eOjI445lKA3pmkAg58_fnoSGVVewYCjFxnm_aUHB_uI
-------------------- Listado de usuarios --------------------
{"data":[{"username":"user1","password":"password1"},{"username":"user2","password":"password2"},{"username":"user3","password":"password3"},{"username":"user4","password":"password4"},{"username":"user5","password":"password5"}]}
-------------------- Resumen de estadísticas --------------------
{"data":[{"username":"user1","total_memory":453.09058,"free_memory":417.2975,"swap_memory":635.9889},{"username":"user2","total_memory":1010.69,"free_memory":713.15735,"swap_memory":778.6858},{"username":"user3","total_memory":534.966,"free_memory":433.42532,"swap_memory":332.13187},{"username":"user4","total_memory":128.7141,"free_memory":45.734108,"swap_memory":855.77386},{"username":"user5","total_memory":548.391,"free_memory":281.06177,"swap_memory":306.91553}]}
```
