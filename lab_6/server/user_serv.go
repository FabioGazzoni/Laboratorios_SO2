package main

import (
	"net/http"
	"os/exec"
	"strings"
	"sync"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/golang-jwt/jwt/v5"
	"golang.org/x/crypto/ssh"
)

type UserData struct {
	ID        int    `json:"id"`
	Username  string `json:"username" binding:"required"`
	Password  string `json:"password" binding:"required"`
	CreatedAt string `json:"created_at"`
}

type UserPOSTResponse struct {
	ID        int    `json:"id"`
	Username  string `json:"username"`
	CreatedAt string `json:"created_at"`
}

type UserGETResponse struct {
	Username string `json:"username" binding:"required"`
	Password string `json:"password" binding:"required"`
}

var users []UserData
var id int = 0
var mutex sync.Mutex

/**
 * Recibe los datos de un usuario (POST) y los guarda en la lista de usuarios
 * responde con los datos del usuario creado
 */
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

/**
 * Recibe los datos de un usuario (POST) y responde con un token JWT si el usuario existe y la contraseña es correcta
 */
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

/**
 * Responde con la lista de usuarios de forma {"data": usersGET} si el usuario está autenticado con un token JWT
 */
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

/**
 * Abre una conexión SSH con el usuario y contraseña recibidos
 */
func sshConnection(user UserGETResponse) (*ssh.Client, error) {
	config := &ssh.ClientConfig{
		User: user.Username,
		Auth: []ssh.AuthMethod{
			ssh.Password(user.Password),
		},
		HostKeyCallback: ssh.InsecureIgnoreHostKey(),
	}
	return ssh.Dial("tcp", "localhost:22", config)
}

/**
 * Crea un nuevo usuario en el sistema operativo
 */
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

/**
 * Crea un nuevo token JWT
 */
func newToken(username string) (string, error) {

	token := jwt.New(jwt.SigningMethodHS256)
	claims := token.Claims.(jwt.MapClaims)
	claims["username"] = username
	claims["exp"] = time.Now().Add(time.Minute * 10).Unix()

	t, error := token.SignedString([]byte("secret"))
	if error != nil {
		return "", error
	}
	return t, nil
}

/**
 * Verifica si el token JWT es válido
 */
func verifyToken(token string) (bool, error) {
	t, err := jwt.Parse(token, func(token *jwt.Token) (interface{}, error) {
		return []byte("secret"), nil
	})
	if err != nil && !t.Valid {
		return false, err
	}
	return true, nil
}
