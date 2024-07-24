#include <stdio.h>
#include <ulfius.h>
#include <jansson.h>
#include <signal.h>
#include <semaphore.h>

#define PORT 8080

int callback_get_imprimir (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_post_increment(const struct _u_request * request, struct _u_response * response, void * user_data);
static void handler();
static void cleanMemory();