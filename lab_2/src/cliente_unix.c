#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <zlib.h>
#include "../lib/common_utils.h"

void execute_communication();
void get_message_from_compressed();
int get_message_sizes(int *);
void read_compress();
static void handler();

int sockfd;
char path_compress[TAM_SUB];

int main(int argc, char *argv[])
{
    signal(SIGINT, handler);

    if (argc < 3)
    {
        fprintf(stderr, "Error: %s cantidad de argumentos invalidos\n", argv[0]);
        exit(0);
    }
    sockfd = init_socket(AF_UNIX);
    init_socket_unix(&sockfd, argv[1], 0);

    memset(path_compress, '\0', TAM_SUB);
    strcpy(path_compress, argv[2]);
    strcat(path_compress, ".gz");

    execute_communication();

    return 0;
}

void execute_communication()
{
    char buffer[TAM];

    while (1)
    {
        // Envio un comando
        memset(buffer, '\0', TAM);
        printf("journalctl ");
        fgets(buffer, TAM - 1, stdin);
        strtok(buffer, "\n");

        send_message(buffer, sockfd, _MESSAGE_CLIENTE);

        // Verificando si se escribió: fin
        if (!strcmp("fin", buffer))
        {
            printf("Finalizando ejecución\n");
            exit(0);
        }

        // Espero los mensajes de retorno
        get_message_from_compressed();
    }
}

void get_message_from_compressed()
{
    unlink(path_compress);
    FILE *FileComprimed = fopen(path_compress, "wb");
    if (FileComprimed == NULL)
    {
        perror("fopen error\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // obengo el tamaño del comprimido
        int buffer_tam_compress;
        if (get_message_sizes(&buffer_tam_compress))
        {
            break;
        }

        Byte binary_file[buffer_tam_compress];
        memset(binary_file, '\0',(size_t) buffer_tam_compress);

        // Recibo el binario
        recv(sockfd, binary_file, (size_t)buffer_tam_compress, 0);

        // Escribo el binario al comprimido
        fwrite(binary_file, 1, (size_t)buffer_tam_compress, FileComprimed);
    }
    fclose(FileComprimed);
    
    //Lectura del archivo contenido en el comprimido
    read_compress();
}

int get_message_sizes(int *buffer_tam_compress)
{
    char buffer[TAM];
    memset(buffer, '\0', TAM);
    int is_end = 0;

    // leo un nuevo mensaje
    if (read(sockfd, buffer, TAM) < 0)
    {
        perror("lectura de socket");
        exit(EXIT_FAILURE);
    }
    // Parceo del mensaje
    if (get_jSON_manager(buffer, &is_end, 1))
    {
        perror("get_json error\n");
        exit(EXIT_FAILURE);
    }
    // En caso de ser fin recibi un error
    if (!strcmp("fin", buffer))
    {
        perror("Error de parte del servidor\n");
        exit(EXIT_FAILURE);
    }

    if (is_end)
    {
        return 1;
    }

    // Transformo el tamaño en int
    *buffer_tam_compress = (int)strtol(buffer, NULL, 10);

    return 0;
}

void read_compress()
{
    // Abro el comprimido con gzopen para descomprimirlo
    gzFile FileComprimed = gzopen(path_compress, "rb");

    char buffer[TAM];
    memset(buffer, '\0', TAM);
    while (gzread(FileComprimed, buffer, TAM))
    {
        printf("%s", buffer);
        memset(buffer, '\0', TAM);
    }

    gzclose(FileComprimed);
}

static void handler()
{
    printf("\n");
    send_message("fin", sockfd, _MESSAGE_SEND_ERROR);
    exit(EXIT_SUCCESS);
}