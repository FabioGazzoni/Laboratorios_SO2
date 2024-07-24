#include "../lib/common_utils.h"

int init_socket(int type)
{
    int sockfd;
    // Obengo un file descriptor creado por el socket de tipo type, SOCK_STREAM conexion full duplex con control de errores
    if (type == AF_BLUETOOTH)
    {
        if ((sockfd = socket(type, SOCK_STREAM, BTPROTO_RFCOMM)) == 0)
        {
            perror("new socket error");
            exit(EXIT_FAILURE);
        }
    }
    else if ((sockfd = socket(type, SOCK_STREAM, 0)) == 0)
    {
        perror("new socket error");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void init_socket_internet(int *fd_socket, char *port, char *ip, int is_server)
{
    struct sockaddr_in sock_addr;

    memset((char *)&sock_addr, 0, sizeof(sock_addr));
    // obtengo el puerto
    int puerto = atoi(port);
    sock_addr.sin_family = AF_INET;               // Familia de direcciones internet
    sock_addr.sin_addr.s_addr = INADDR_ANY;       // Direcion ip
    sock_addr.sin_port = htons((uint16_t)puerto); // htons pregunta si el puerto esta disponible

    if (is_server)
    {
        if (bind(*fd_socket, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0)
        {
            perror("ligadura");
            exit(EXIT_FAILURE);
        }
        // Marca el socket como preparado para recibir conexion, acepta hasta 5 usuarios por conetar a la vez
        listen(*fd_socket, 5);
    }
    else
    {
        // Se obtiene el server (direccion ip)
        struct hostent *server = gethostbyname(ip);
        bcopy((char *)server->h_addr_list[0], (char *)&sock_addr.sin_addr.s_addr, (size_t)server->h_length);

        // Establezco la conexion
        if (connect(*fd_socket, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0)
        {
            perror("conexion error");
            exit(EXIT_FAILURE);
        }
    }

    printf("Proceso: %d - socket disponible: %d\n", getpid(), ntohs(sock_addr.sin_port));
}

void init_socket_unix(int *fd_socket, char *name_fd, int is_server)
{
    struct sockaddr_un sock_addr;
    memset((char *)&sock_addr, '\0', sizeof(sock_addr));

    sock_addr.sun_family = AF_UNIX;
    // doy el nombre del archivo donde se establece la comunicacion
    strcpy(sock_addr.sun_path, name_fd);

    socklen_t servlen = (socklen_t)strlen(sock_addr.sun_path) + sizeof(sock_addr.sun_family);
    if (is_server)
    {
        // En caso de existir un fd con ese nombre lo libero para utilizarlo
        unlink(name_fd);
        // Asocia la direccion al socket
        if (bind(*fd_socket, (struct sockaddr *)&sock_addr, servlen) < 0)
        {
            perror("ligadura");
            exit(EXIT_FAILURE);
        }
        listen(*fd_socket, 5);
    }
    else
    {
        // Establezco la conexion
        if (connect(*fd_socket, (struct sockaddr *)&sock_addr, servlen) < 0)
        {
            perror("conexion error");
            exit(EXIT_FAILURE);
        }
    }

    printf("Proceso: %d - socket disponible: %s\n", getpid(), sock_addr.sun_path);
}

void init_socket_bluetooth(int *fd_socket, char *ip, int is_server)
{
    struct sockaddr_rc sock_addr;
    memset((char *)&sock_addr, '\0', sizeof(sock_addr));
    sock_addr.rc_family = AF_BLUETOOTH;
    sock_addr.rc_channel = (uint8_t)1;

    if (is_server)
    {
        sock_addr.rc_bdaddr = *BDADDR_ANY;
        // Asocia la direccion al socket
        if (bind(*fd_socket, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0)
        {
            perror("ligadura");
            exit(EXIT_FAILURE);
        }
        listen(*fd_socket, 5);
    }
    else
    {
        // Convierte una cadena de caracteres en la direccion ip
        if (str2ba(ip, &sock_addr.rc_bdaddr) != 0)
        {
            perror("ip conversion error");
            exit(EXIT_FAILURE);
        }
        // Establezco la conexion
        if (connect(*fd_socket, (struct sockaddr *)&sock_addr, (socklen_t)sizeof(sock_addr)) < 0)
        {
            perror("conexion error");
            exit(EXIT_FAILURE);
        }
    }

    printf("Proceso: %d - socket disponible: %d\n", getpid(), sock_addr.rc_channel);
}

void send_message(char *message, int sockfd, int message_flag)
{
    char final_message[TAM];
    memset(final_message, '\0', TAM);

    strcpy(
        final_message,
        set_cJSON_mananger(
            message,
            message_flag));

    if (send(sockfd, final_message, TAM, 0) == -1)
    {
        perror("send error");
        exit(EXIT_FAILURE);
    }
}

int get_message(int sockfd, int have_checksum)
{
    char final_message[TAM];
    int is_end;

    while (1)
    {
        memset(final_message, '\0', TAM);
        if (recv(sockfd, final_message, TAM, 0) < 0)
        {
            perror("lectura de socket");
            exit(EXIT_FAILURE);
        }
        if (get_jSON_manager(final_message, &is_end, have_checksum) == EXIT_FAILURE)
        {
            exit(EXIT_FAILURE);
        }

        if (!strcmp("fin", final_message))
        {
            return 1;
        }

        printf("%s", final_message);
        if (is_end)
        {
            break;
        }
        fflush(stdout);
    }
    return 0;
}

int get_jSON_manager(char *content, int *is_end, int have_checksum)
{
    // Convierto el string pasado por argumento en una variable cJSON
    cJSON *json_parse = cJSON_Parse(content);
    memset(content, '\0', TAM);
    if (json_parse == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        perror("json_parse error");
        return EXIT_FAILURE;
    }

    // Obtengo el valr de message(string)
    cJSON *json_string = cJSON_GetObjectItemCaseSensitive(json_parse, ID_SEND);
    // Verifico que el cJSON es de tipo string y que no este nulo
    if (!cJSON_IsString(json_string) && (json_string->valuestring == NULL))
    {
        strcpy(content, "parse json_string error");
    }

    // Obtengo el valor de end(int)
    cJSON *json_int_end = cJSON_GetObjectItemCaseSensitive(json_parse, ID_END);
    if (!cJSON_IsNumber(json_int_end))
    {
        perror("parse json_int_end error");
        return EXIT_FAILURE;
    }
    if (is_end != NULL)
    {
        *is_end = (int)json_int_end->valuedouble;
    }

    // Obtengo el valor de checksum(int) si es que tiene y verifico si checksum es exitoso
    if (have_checksum)
    {
        cJSON *json_checksum = cJSON_GetObjectItemCaseSensitive(json_parse, ID_CHECKSUM);
        verifly_checksum(
            json_checksum->valuestring,
            json_string->valuestring);
        if (!cJSON_IsString(json_checksum) && (json_checksum->valuestring == NULL))
        {
            strcpy(content, "parse json_checksum error");
        }
    }

    strcpy(content, json_string->valuestring);

    cJSON_Delete(json_parse);
    return 0;
}

char *set_cJSON_mananger(char *message, int message_flag)
{
    cJSON *json = cJSON_CreateObject();

    // Verifica si es final
    init_cJSON_Flag_Final(json, (double)(message_flag & (1 << 0)));

    // Verifica si tiene checksum y agrega el mensaje
    init_cJSON_Message_and_Checksum(json, message, (message_flag & (1 << 1)));

    char *result = cJSON_Print(json);
    cJSON_Delete(json);
    return result;
}

void init_cJSON_Message_and_Checksum(cJSON *json, char *message, int need_checksum)
{
    if (cJSON_AddStringToObject(json, ID_SEND, message) == NULL)
    {
        cJSON_Delete(json);
        perror("error add string message to json");
        exit(EXIT_FAILURE);
    }
    if (need_checksum)
    {
        char checksum[SHA256_DIGEST_LENGTH];
        get_string_checksum(message, checksum);
        if (cJSON_AddStringToObject(json, ID_CHECKSUM, checksum) == NULL)
        {
            cJSON_Delete(json);
            perror("error add string checksum to json");
            exit(EXIT_FAILURE);
        }
    }
}

void init_cJSON_Flag_Final(cJSON *json, double is_final)
{
    if (cJSON_AddNumberToObject(json, ID_END, (double)is_final) == NULL)
    {
        cJSON_Delete(json);
        perror("error add number end to json");
        exit(EXIT_FAILURE);
    }
}

void get_string_checksum(char *message, char *result)
{
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)message, strlen(message), digest);
    strcpy(result, (char *)digest);
}

void verifly_checksum(char *chs1, char *chs2)
{
    if (!strcmp(chs1, chs2))
    {
        printf("Error, checksum invalido.\n");
    }
}