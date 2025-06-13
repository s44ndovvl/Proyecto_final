#include "extra.h"
#define MAX_LINE_LENGTH 1024
#define MAX_FIELDS 300

char **leer_linea_csv(FILE *archivo, char separador) {
    static char linea[MAX_LINE_LENGTH];
    static char *campos[MAX_FIELDS];
    int idx = 0;

    if (fgets(linea, MAX_LINE_LENGTH, archivo) == NULL) {
        return NULL;
    }

    linea[strcspn(linea, "\n")] = '\0'; // Eliminar salto de línea
    char *ptr = linea;

    while (*ptr && idx < MAX_FIELDS - 1) {
        // Saltar espacios iniciales o separadores duplicados
        while (*ptr == ' ') ptr++;

        // Si el campo está entre comillas
        if (*ptr == '"') {
            ptr++; // Saltar comilla inicial
            campos[idx++] = ptr;

            // Avanzar hasta encontrar la comilla de cierre
            while (*ptr && !(*ptr == '"' && (*(ptr + 1) == separador || *(ptr + 1) == '\0'))) {
                ptr++;
            }

            if (*ptr == '"') {
                *ptr = '\0';  // Terminar el string del campo
                ptr++;        // Saltar comilla
                if (*ptr == separador) ptr++; // Saltar separador
            }

        } else {
            // Campo sin comillas
            campos[idx++] = ptr;
            while (*ptr && *ptr != separador) ptr++;

            if (*ptr == separador) {
                *ptr = '\0';
                ptr++;
            }
        }
    }

    campos[idx] = NULL;
    return campos;
}

List *split_string(const char *str, const char *delim) {
  List *result = list_create();
  char *token = strtok((char *)str, delim);

  while (token != NULL) {
    // Eliminar espacios en blanco al inicio del token
    while (*token == ' ') {
      token++;
    }

    // Eliminar espacios en blanco al final del token
    char *end = token + strlen(token) - 1;
    while (*end == ' ' && end > token) {
      *end = '\0';
      end--;
    }

    // Copiar el token en un nuevo string
    char *new_token = strdup(token);

    // Agregar el nuevo string a la lista
    list_pushBack(result, new_token);

    // Obtener el siguiente token
    token = strtok(NULL, delim);
  }

  return result;
}

// Función para limpiar la pantalla
void limpiarPantalla() { system("cls"); }

void presioneTeclaParaContinuar() {
  puts("Presione una tecla para continuar...");
  getchar(); // Consume el '\n' del buffer de entrada
  getchar(); // Espera a que el usuario presione una tecla
}