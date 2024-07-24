import concurrent.futures
import subprocess

# Función para ejecutar curl con un POST
def send_post_request():
    result = subprocess.run(
        [
            "curl",
            "-s",  # Modo silencioso para no mostrar progreso
            "-X",
            "POST",
            "-H",
            "Content-Type: application/json",
            "-d",
            '{"number": 1}',
            "http://localhost:8080/increment",
        ],
        capture_output=True,
        text=True,
    )

    if result.returncode != 0:
        print("Error en POST, ¿esta disponible el servidor?")

# Usar ThreadPoolExecutor para paralelizar la ejecución
with concurrent.futures.ThreadPoolExecutor() as executor:
    # Crear 1000 tareas para enviar el curl
    futures = [executor.submit(send_post_request) for _ in range(1000)]

    # Esperar a que todas las tareas terminen
    concurrent.futures.wait(futures)

# Enviar la solicitud GET para imprimir el resultado
get_result = subprocess.run(
    ["curl", "-s", "-X", "GET", "http://localhost:8080/imprimir"],
    capture_output=True,
    text=True,
)

if get_result.returncode == 0:
    print("Respuesta GET:", get_result.stdout)
else:
    print("Error en GET:", get_result.stderr)
