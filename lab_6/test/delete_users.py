import subprocess
# Lista de usuarios y contraseñas
users = [
    {"username": "user1", "password": "password1"},
    {"username": "user2", "password": "password2"},
    {"username": "user3", "password": "password3"},
    {"username": "user4", "password": "password4"},
    {"username": "user5", "password": "password5"},
]

# Eliminar todos los usuarios del sistema con userdel y sudo
for user in users: 
    # Matar los procesos del usuario
    kill_command = ["sudo", "pkill", "-u", user["username"]]
    subprocess.run(kill_command)

    # Eliminar el usuario
    delete_command = ["sudo", "userdel", "-r", user["username"]]
    subprocess.run(delete_command)

subprocess.run(["sudo", "systemctl", "restart", "serviceSOII.service"])