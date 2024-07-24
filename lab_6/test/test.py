import subprocess
import json
import random

# Lista de usuarios y contraseñas
users = [
    {"username": "user1", "password": "password1"},
    {"username": "user2", "password": "password2"},
    {"username": "user3", "password": "password3"},
    {"username": "user4", "password": "password4"},
    {"username": "user5", "password": "password5"},
]

tokens = {}

base_command_new_user = [
    "curl", "--request", "POST",
    "--url", "http://dashboard.com/api/users/createuser",
    "-u", "USER:SECRET",
    "--header", "accept: application/json",
    "--header", "content-type: application/json"
]

base_command_login = [
    "curl", "--request", "POST",
    "--url", "http://dashboard.com/api/users/login",
    "-u", "USER:SECRET",
    "--header", "accept: application/json",
    "--header", "content-type: application/json"
]

base_command_list = [
    "curl", "--request", "GET",
    "--url", "http://dashboard.com/api/users/listall"
]

base_command_process = [
    "curl", "--request", "POST",
    "--url", "http://sensors.com/api/process/sensordata",
    "-u", "USER:SECRET",
    "--header", "accept: application/json",
    "--header", "content-type: application/json"
]

base_command_summary = [
    "curl", "--request", "GET",
    "--url", "http://sensors.com/api/process/summary"
]
print("-------------------- Creación de usuarios --------------------")
# Creación de usuarios
for user in users:
    data = f'{{"username": "{user["username"]}", "password": "{user["password"]}"}}'
    command = base_command_new_user + ["--data", data]
    subprocess.run(command)
    print("")

# Obtención de tokens
for user in users:
    data = f'{{"username": "{user["username"]}", "password": "{user["password"]}"}}'
    command = base_command_login + ["--data", data]

    # Ejecutar el comando y capturar la salida
    result = subprocess.run(command, capture_output=True, text=True)
    
    # Analizar la respuesta JSON
    try:
        response = json.loads(result.stdout)
        token = response.get("token")  # Asumiendo que la respuesta JSON contiene un campo "token"
        tokens[user["username"]] = token
    except json.JSONDecodeError:
        print(f"Error decodificando la respuesta para el usuario {user['username']}")
    except KeyError:
        print(f"El token no se encontró en la respuesta para el usuario {user['username']}")

print("-------------------- Tokens de acceso --------------------")
for user, token in tokens.items():
    print(f"Token para {user}: {token}")

print("-------------------- Listado de usuarios --------------------")
# Solo un usuario ejecuta listall
primer_usuario = list(tokens.keys())[0]
authorization_header = f"Token: {tokens[primer_usuario]}"
command = base_command_list + ["-H", authorization_header]
subprocess.run(command)

# Todos los usuarios ejecutan 
for user, token in tokens.items():
    authorization_header = f"Token: Bearer {token}"

    #Random stats
    total_memory = random.uniform(0, 1024)
    free_memory = random.uniform(0, total_memory)
    swap_memory = random.uniform(0, 1024)
    stats_data = f'{{"username": "{user}", "total_memory": {total_memory}, "free_memory": {free_memory}, "swap_memory": {swap_memory}}}'
    command = base_command_process + ["--header", authorization_header, "--data", stats_data]
    #Ejecutar proceso sin imprimir el resultado por consola
    subprocess.run(command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

print("")
print("-------------------- Resumen de estadísticas --------------------")
subprocess.run(base_command_summary)