import matplotlib.pyplot as plt

# Define el arreglo de valores "n"
valores = [1, 2, 3, 4, 6, 8, 10]

# Lee los resultados del archivo
with open("resultados.txt", "r") as file:
    # Convierte cada línea a número flotante, reemplazando comas por puntos
    resultados = [float(line.strip().replace(",", ".")) for line in file]

# Verifica que los datos coinciden
if len(resultados) != len(valores):
    raise ValueError("El número de resultados no coincide con el número de valores 'n'.")

# Crea el gráfico
plt.plot(valores, resultados, marker='o', linestyle='-', color='b', label='Resultados vs. n')

# Añade etiquetas y título
plt.xlabel("Cantidad de hilos utilizados")
plt.ylabel("Tiempo [s]")

# Activa las cuadrículas
plt.grid(True)  # O 'on' para activar, 'off' para desactivar

# Configura el estilo de las cuadrículas (opcional)
plt.grid(linestyle='--', linewidth=0.5, color='gray')

# Muestra la leyenda
plt.legend()

# Muestra el gráfico
plt.show()
