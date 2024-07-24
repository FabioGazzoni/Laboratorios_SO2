#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include "../lib/common_utils.h"

void execute_communication();
static void handler();

int sockfd;

int main(int argc, char *argv[])
{
    signal(SIGINT, handler);
    if (argc < 3)
    {
        fprintf(stderr, "Error: %s cantidad de argumentos invalidos\n", argv[0]);
        exit(0);
    }

    sockfd = init_socket(AF_INET);
    init_socket_internet(
        &sockfd,
        argv[2],
        argv[1],
        0);

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

        // Espero un mensaje de retorno
        if (get_message(sockfd, 1))
        {
            perror("Error por parte del servidor\n");
            exit (EXIT_FAILURE);
        }
    }
}

static void handler()
{
    printf("\n");
    send_message("fin", sockfd, _MESSAGE_SEND_ERROR);
    exit(EXIT_SUCCESS);
}