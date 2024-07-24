#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <openssl/sha.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include "../lib/cJSON.h"

#define TAM 2048
#define TAM_SUB 1024
#define ID_SEND "MESSAGE"
#define ID_END "END"
#define ID_CHECKSUM "CHEACKSUM"

#define _MESSAGE_CLIENTE 0           // 0b00 -> no tiene checksum, no es final
#define _MESSAGE_SERVIDOR_FINAL 3    // 0b11 -> tiene checksum, es final
#define _MESSAGE_SERVIDOR_NO_FINAL 2 // 0b10 -> tiene checksum, no es final
#define _MESSAGE_SEND_ERROR 1        // 0b01 -> no tiene checksum, es final

int init_socket(int);
void init_socket_internet(int *fd_socket, char *port, char *ip, int is_server);
void init_socket_unix(int *fd_socket, char *name_fd, int is_server);
void init_socket_bluetooth(int *fd_socket, char *ip, int is_server);
void send_message(char *message, int sockfd, int message_flag);
int get_message(int sockfd, int have_checksum);
int get_jSON_manager(char *content, int *is_end, int have_checksum);
char *set_cJSON_mananger(char *message, int is_final);
void init_cJSON_Message_and_Checksum(cJSON *json, char *message, int need_checksum);
void init_cJSON_Flag_Final(cJSON *json, double is_final);
void get_string_checksum(char *message, char *checksum);
void verifly_checksum(char *chs1, char *chs2);