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
    if (argc < 2)
    {
        fprintf(stderr, "Error: %s cantidad de argumentos invalidos\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sockfd = init_socket(AF_BLUETOOTH);

    init_socket_bluetooth(&sockfd, argv[1], 0);

    execute_communication();
    return EXIT_SUCCESS;
}

void execute_communication()
{
    char buffer[TAM_SUB];

    memset(buffer, '\0', TAM_SUB);
    send_message(buffer, sockfd, _MESSAGE_CLIENTE);

    // Verificando si se escribió: fin
    if (!strcmp("fin", buffer))
    {
        printf("Finalizando ejecución\n");
        exit(EXIT_SUCCESS);
    }

    // Espero un mensaje de retorno
    if (get_message(sockfd, 1))
    {
        perror("Error por parte del servidor\n");
        exit(EXIT_FAILURE);
    }
}

static void handler()
{
    printf("\n");
    send_message("fin", sockfd, _MESSAGE_SEND_ERROR);
    exit(EXIT_SUCCESS);
}