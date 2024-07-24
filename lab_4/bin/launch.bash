#!/bin/bash

# Define el arreglo con los valores de n
valores=(1 2 3 4 6 8 10)
# Archivo donde se almacenará la salida
output_file="resultados.txt"

# Asegúrate de que el archivo está vacío antes de empezar (opcional)
> "$output_file"

# Recorre cada valor en el arreglo
for n in "${valores[@]}"; do
    # Ejecuta el programa con el parámetro actual y almacena el resultado en el archivo
    ./tp_4 "$n" >> "$output_file"
done