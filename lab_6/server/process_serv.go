package main

import (
	"net/http"

	"github.com/gin-gonic/gin"
)

type SensorData struct {
	Username    string  `json:"username"`
	TotalMemory float32 `json:"total_memory"`
	FreeMemory  float32 `json:"free_memory"`
	SwapMemory  float32 `json:"swap_memory"`
}

var sensorsData []SensorData

/**
 * Recibe los datos de un sensor (POST) y los guarda en la lista de datos de sensores
 */
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

/**
 * Responde con la lista de datos de sensores de forma {"data": sensorsData}
 */
func getSummary(c *gin.Context) {
	c.JSON(http.StatusOK, gin.H{"data": sensorsData})
}
