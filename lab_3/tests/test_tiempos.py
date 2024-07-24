import matplotlib.pyplot as plt
import subprocess

# Función para extraer los valores de tiempo real del archivo
def extract_real_times(filename):
    max_time = 0.0
    min_time = 10.0
    real_times = []
    with open(filename, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if line.startswith('real'):
                time = line.split('\t')[1].strip().replace('m', '').replace('s', '').replace(',', '.')
                real_times.append(float(time))

                if max_time < float(time):
                    max_time = float(time)
                elif min_time > float(time):
                    min_time = float(time)
    return real_times, max_time, min_time

# Nombre de tu archivo de datos
filename = 'times'

subprocess.run(['bash', 'tiempos.sh'])

# Extraer los valores de tiempo real del archivo
real_times, max_time, min_time = extract_real_times(filename)

subprocess.run(['rm', filename])

# Crear una lista de números de ejecución para el eje x
execution_numbers = list(range(1, len(real_times) + 1))

# Calcular la media de los tiempos reales
mean_time = sum(real_times) / len(real_times)
print("Media: ", mean_time)
print("Máximo: ", max_time)
print("Mínimo: ", min_time)


# Graficar los datos
plt.plot(execution_numbers, real_times, marker='o', linestyle='', label='Tiempo Real')
plt.axhline(y=mean_time, color='g', linestyle='-', label='Media')
plt.axhline(y=max_time, color='r', linestyle='-', label='Máximo')
plt.axhline(y=min_time, color='r', linestyle='-', label='Mínimo')
plt.xlabel('Ejecución n°')
plt.ylabel('Tiempo Real (s)')
plt.title('Tiempo Real de Ejecuciones')
plt.grid(True)
plt.show()

