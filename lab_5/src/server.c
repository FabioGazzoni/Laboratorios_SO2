#include "../lib/server.h"

int count = 0;
struct _u_instance instance;
sem_t mutex;

int main(void) {
  signal(SIGINT, handler);

  //Inicializa el semáforo binario.
  sem_init(&mutex, 0, 1);

  // Initialize instance with the port number
  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error ulfius_init_instance, abort\n");
    return(1);
  }

  // Endpoint list declaration
  ulfius_add_endpoint_by_val(&instance, "POST", "/increment", NULL, 0, &callback_post_increment, NULL);
  ulfius_add_endpoint_by_val(&instance, "GET", "/imprimir", NULL, 1, &callback_get_imprimir, NULL);

  // Start the framework
  if (ulfius_start_framework(&instance) == U_OK) {
    printf("Start framework on port %d\nPlease press <enter> or CTL+C to finish service\n", instance.port);

    getchar();
  } else {
    fprintf(stderr, "Error starting framework\n");
  }
  cleanMemory();

  return 0;
}

/**
 * Callback al recibir GET /imprimir, responde con el total del contador.
 */
int callback_get_imprimir (const struct _u_request * request, struct _u_response * response, void * user_data) {
  (void) request;
  (void) user_data;

  char response_text[15];
  snprintf(response_text, sizeof(response_text), "Count: %d", count);
    
  ulfius_set_string_body_response(response, 200, response_text);
  return U_CALLBACK_CONTINUE;
}

/**
 * Callback al recibir POST /increment, incrementa el contador con el contenido del json number.
 */
int callback_post_increment(const struct _u_request * request, struct _u_response * response, void * user_data){
  (void) user_data;

  // Leer el cuerpo de la solicitud como JSON
  json_t *json_body = ulfius_get_json_body_request(request, NULL);
  if (!json_body) {
    ulfius_set_string_body_response(response, 400, "Formato JSON incorrecto");
    return U_CALLBACK_CONTINUE;
  }

  json_t *json_number = json_object_get(json_body, "number");
  if (!json_is_integer(json_number)) {
    ulfius_set_string_body_response(response, 400, "El valor proporcionado no es un número entero");
    json_decref(json_body);//Clean JSON
    return U_CALLBACK_CONTINUE;
  }

  int number = (int) json_integer_value(json_number);
  
  sem_wait(&mutex); 
  count += number;
  sem_post(&mutex);

  json_decref(json_body);//Clean JSON

  ulfius_set_empty_body_response(response, 204);

  return U_CALLBACK_CONTINUE;
}

/**
 * Se libera la memoria utilizada por ulfius
*/
static void cleanMemory(){
  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);
  sem_destroy(&mutex); 

  printf("End framework\n");
}

static void handler()
{
  cleanMemory();
  exit(EXIT_SUCCESS);
}