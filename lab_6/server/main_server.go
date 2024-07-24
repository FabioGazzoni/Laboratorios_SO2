package main

import (
	"github.com/gin-gonic/gin"
)

func main() {
	// Gorutinas separadas (dos procesos)
	go func() {
		r := gin.Default()

		r.POST("/api/users/createuser", createUser)
		r.POST("/api/users/login", login)
		r.GET("/api/users/listall", getUsers)

		r.Run(":8081")
	}()

	go func() {
		r := gin.Default()

		r.POST("/api/process/sensordata", processSensorData)
		r.GET("/api/process/summary", getSummary)
		r.Run(":8082")
	}()
	// Esperar a que se cierre el programa para no cerrar las gorutinas
	select {}
}
