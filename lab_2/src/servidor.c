#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/sysinfo.h>
#include <netinet/in.h>
#include <errno.h>
#include <zlib.h>
#include <malloc.h>
#include "../lib/common_utils.h"

void init_server(char *, char *);
void client_manager(int);
void new_client_manager();
void journal_response(char *);
void file_response(char *);
void send_binary_message(Bytef *, size_t);
FILE *get_FILE_from_command(char *);
void memory_stats_response();
void close_communication();
static void handler();

int type;
char *unix_file_name;
int sockfd;

int main(int argc, char *argv[])
{
    signal(SIGINT, handler);
    sockfd = 0;
    if (argc < 3)
    {
        fprintf(stderr, "Uso: %s <puerto>,  <nombre_de_socket>\n", argv[0]);
        exit(1);
    }

    init_server(argv[1], argv[2]);

    int status;
    wait(&status);
    wait(&status);
    wait(&status);
    return 0;
}

void init_server(char *port, char *file)
{
    int sockfd_internet;
    int sockfd_unix;
    int sockfd_bluetooth;

    if (fork() == 0)
    {
        type = AF_INET;
        sockfd_internet = init_socket(type);
        init_socket_internet(
            &sockfd_internet,
            port,
            NULL,
            1);

        client_manager(sockfd_internet);
    }
    else if (fork() == 0)
    {
        type = AF_UNIX;
        sockfd_unix = init_socket(type);
        init_socket_unix(
            &sockfd_unix,
            file,
            1);
        unix_file_name = file;
        client_manager(sockfd_unix);
    }
    else if (fork() == 0)
    {
        type = AF_BLUETOOTH;
        sockfd_bluetooth = init_socket(type);
        init_socket_bluetooth(
            &sockfd_bluetooth,
            NULL,
            1);
        client_manager(sockfd_bluetooth);
    }
}

void client_manager(int sockfd_type)
{
    struct sockaddr_in cli_addr;
    int size_clilen = sizeof(cli_addr);
    while (1)
    { // acept devuelve un nuevo fd que se utiliza para la comunicacion de un cliente con el servidor
        sockfd = accept(sockfd_type, (struct sockaddr *)&cli_addr, (socklen_t *)&size_clilen);
        int pid = fork();

        if (pid == 0)
        { // Proceso hijo
            close(sockfd_type);
            new_client_manager();
        }
        else
        {
            printf("SERVIDOR: Nuevo servidor, que atiende el proceso hijo: %d\n", pid);
            close(sockfd);
        }
    }
}

void new_client_manager()
{
    char buffer[TAM];

    while (1)
    {
        memset(buffer, '\0', TAM);
        // leo un nuevo mensaje
        if (read(sockfd, buffer, TAM) < 0)
        {
            perror("lectura de socket");
            close_communication();
        }
        if (get_jSON_manager(buffer, NULL, 0))
        {
            close_communication();
        }
        // Verifico si es el fin de la conversacion
        if (!strcmp("fin", buffer))
        {
            printf("PROCESO %d. Como recibí 'fin', termino la ejecución.\n\n", getpid());
            close_communication();
        }
        else
        {
            // Respondo el pedido segun el tipo de cliente
            switch (type)
            {
            case AF_INET:
                journal_response(buffer);
                break;

            case AF_UNIX:
                file_response(
                    buffer);
                break;
            case AF_BLUETOOTH:
                memory_stats_response();
                close(sockfd);
                exit(0);
                break;
            }
        }
    }
    close(sockfd);
}

void journal_response(char *command)
{
    // Obtencion del file con la respuesta del comando
    FILE *FileOpen = get_FILE_from_command(command);
    char responce[TAM_SUB];
    memset(responce, '\0', TAM_SUB);

    // something verifica que se haya accedido al menos una vez al while
    int f_get_something = 0;
    while (fgets(responce, TAM_SUB, FileOpen))
    {
        f_get_something = 1;

        // Envio el mensaje
        send_message(responce, sockfd, _MESSAGE_SERVIDOR_NO_FINAL);
        memset(responce, '\0', TAM_SUB);
    }
    if (!f_get_something)
    {
        send_message("Argumento invalido, intente nuevamente\n", sockfd, _MESSAGE_SERVIDOR_NO_FINAL);
    }
    // Al enviar todo, envio un ultimo mensaje vacio con el flag indicando que termino
    send_message("", sockfd, _MESSAGE_SERVIDOR_FINAL);
    pclose(FileOpen);
}

void file_response(char *command)
{
    //Preparo el nombre del archivo temporal comprimido
    char file_path[11] = "compress\0";
    char num_socfd[2];
    sprintf(num_socfd, "%d", sockfd);
    strcat(file_path, num_socfd);

    // Obtencion del file con la respuesta del comando
    FILE *FileOpen = get_FILE_from_command(command);

    // Creacion del archivo comprimido
    gzFile FileCompress = gzopen(file_path, "wb");
    if (FileCompress == NULL)
    {
        perror("file_path error\n");
        close_communication();
    }

    // Escribo el comprimido
    size_t read_len;
    Byte buffer[TAM];
    memset(buffer, '\0', TAM);
    while ((read_len = fread(buffer, 1, TAM, FileOpen)) > 0)
    {
        if ((size_t)gzwrite(FileCompress, buffer, (unsigned int)read_len) != read_len)
        {
            printf("gzwrite error.\n");
            close_communication();
        }
        memset(buffer, '\0', TAM);
    }
    fclose(FileOpen);
    gzclose(FileCompress);

    FILE *FileOpenCompress = fopen(file_path, "rb");
    if (FileOpenCompress == NULL)
    {
        perror("fopen error\n");
        close_communication();
    }

    // Leo en binario y envio el comprimido
    while ((read_len = fread(buffer, 1, TAM, FileOpenCompress)) > 0)
    {
        send_binary_message(buffer, read_len);
        memset(buffer, '\0', TAM);
    }
    fclose(FileOpenCompress);
    send_message("", sockfd, _MESSAGE_SERVIDOR_FINAL);
    unlink(file_path);
}

void send_binary_message(Byte *buffer, size_t buffer_tam)
{
    char number_in_string[TAM_SUB];
    memset(number_in_string, '\0', TAM_SUB);
    sprintf(number_in_string, "%ld", buffer_tam);

    send_message(number_in_string, sockfd, _MESSAGE_SERVIDOR_NO_FINAL);

    if (send(sockfd, buffer, buffer_tam, 0) == -1)
    {
        perror("send error");
        close_communication();
    }
}

FILE *get_FILE_from_command(char *command)
{
    FILE *FileOpen;
    char journal_command[TAM] = "journalctl ";
    strcat(journal_command, command);
    FileOpen = popen(journal_command, "r");
    if (FileOpen == NULL)
    {
        perror("popen error\n");
        close_communication();
    }

    return FileOpen;
}

void memory_stats_response()
{
    struct sysinfo info;
    double loadavg[3];

    // Respuesta del sistema
    if (sysinfo(&info) != 0)
    {
        printf("Error sysinfo.\n");
        close_communication();
    }
    // Respuesta del load normalizado del sistema
    if (getloadavg(loadavg, 3) == -1)
    {
        printf("Error getloadavg. \n");
        close_communication();
    }
    // Transformacion de memoria libra a MB
    double mem_mb = (double)info.freeram / 1048576.0;
    char buffer[TAM_SUB];

    sprintf(buffer, "Memoria libre: %.2f Mb\n", mem_mb);
    printf("Load normalizado\n");
    sprintf(buffer + strlen(buffer), "1 minuto: %.2f\n", loadavg[0]);
    sprintf(buffer + strlen(buffer), "5 minutos: %.2f\n", loadavg[1]);
    sprintf(buffer + strlen(buffer), "15 minutos: %.2f\n", loadavg[2]);
    // Envio de respuesta
    send_message(buffer, sockfd, _MESSAGE_SERVIDOR_FINAL);
}

void close_communication()
{
    send_message("fin", sockfd, _MESSAGE_SERVIDOR_FINAL);
    exit(EXIT_FAILURE);
}

static void handler()
{
    unlink(unix_file_name);
    exit(EXIT_SUCCESS);
}