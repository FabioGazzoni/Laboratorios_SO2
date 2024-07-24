import subprocess

def eject():
    process = subprocess.run(["../bin/original"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    subprocess.run(["../bin/laboratorio3"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    
    # Captura la salida estándar y de error del proceso
    stout = process.stdout
    sterr = process.stderr
    
    # Verifica si ocurrió algún error
    if process.returncode != 0:
        print("Ocurrió un error al ejecutar el programa en C:")
        print(sterr)
    else:
        return stout

def compare(stout):
    try:
        with open("results", "r") as archivo:
            output = archivo.read()
            if stout.strip() == output.strip():
                print("Test - Ejecución exitosa")
            else:
                print("Test - Error en la comparación de outputs")
    except FileNotFoundError:
        print("El archivo especificado no se encontró.")

if __name__ == "__main__":
    stout = eject()
    if stout:
        compare(stout)

    subprocess.run(["rm", "results"])
