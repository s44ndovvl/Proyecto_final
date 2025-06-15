#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "extra.h"
#include "hashmap.h"

typedef struct Escenarios Escenarios;

typedef struct{
    char tipo[50];
    char nombre[50];
    int defensa;
    int durabilidad;
    char bufo[50];
    int valor;
    //int nivel_requerido;
}Armadura;

typedef struct{
    char nombre[50];
    int ataque;
    int durabilidad;
}Arma;

//Estructura para cada Item
typedef struct{
    char nombre[50];
    char efecto[50];
    int valor;
}Pocion;

typedef struct{
    Arma *armas;
    Armadura *armaduras;
    Pocion *pocion;
}Inventario;

//Estructura para cada escenario(nivel)
struct Escenarios{
    char id[3];
    char nombre[50];
    char leyenda[500];
    char id_arriba[3];
    char id_abajo[3];
    char id_izquierda[3];
    char id_derecha[3];
    Escenarios *arriba;
    Escenarios *abajo;
    Escenarios *izquierda;
    Escenarios *derecha;
    List *Enemigos;
    char dificultad[8];
};

//Estructura para el jugador
typedef struct{
    char nombre[50];
    int vida;
    int estamina;
    int defensa;
    int ataque;
    int experiencia;
    int nivel;
    Escenarios *actual;
    List *inventario;
}Jugador;

typedef struct{
    char nombre[50];
    char dificultad[50];
    int vida;
    int defensa;
    int ataque;
    int exp_dada;
    Inventario *item;
}Enemigo;

void leer_escenarios(HashMap * );
void mostrarMap(HashMap * );
void leer_mobs(HashMap * );

int main(){
    HashMap *juego = createMap(100); // Crea un HashMap para almacenar los escenarios
    if (juego == NULL) {
        fprintf(stderr, "Error al crear el HashMap\n");
        return 1;
    }
    leer_escenarios(juego); // Llama a la funciÃ³n para leer los escenarios desde el archivo CSV
    mostrarMap(juego); // Muestra el contenido del HashMap

    return 0;
}

void leer_escenarios(HashMap * juego){
    FILE *archivo = fopen("data/mapa.csv", "r");
    if (archivo == NULL){
        perror("Error al abrir el archivo");
        return;
    }

    char **campos;
    campos = leer_linea_csv(archivo, ',');

    while ((campos = leer_linea_csv(archivo, ',')) != NULL){
        Escenarios * escenario = (Escenarios*)malloc(sizeof(Escenarios));
        //Inicializa los punteros de direccion a NULL para evitar punteros basura
        escenario->arriba = NULL;
        escenario->abajo = NULL;
        escenario->izquierda = NULL;
        escenario->derecha = NULL;
        
        //Copia los datos del CSV a la estructura del escenario
        strcpy(escenario->id, campos[0]);
        strcpy(escenario->nombre, campos[1]);
        strcpy(escenario->leyenda, campos[2]);

        strcpy(escenario->id_arriba, campos[3]);
        strcpy(escenario->id_abajo, campos[4]);
        strcpy(escenario->id_izquierda, campos[5]);
        strcpy(escenario->id_derecha, campos[6]);

        strcpy(escenario->dificultad, campos[7]);

        insertMap(juego, escenario->id, escenario);

    }
    fclose(archivo);

    // Segunda pasada: establecer los punteros de direcciÃ³n entre escenarios
    for (Pair *pp = firstMap(juego); pp; pp = nextMap(juego)) // Recorre todos los escenarios
    {
        Escenarios *e = (Escenarios*)pp->value; // Obtiene el escenario actual

        if(atoi(e->id_abajo) != 0)e->abajo = (Escenarios*)searchMap(juego, e->id_abajo)->value; // Conecta el escenario abajo
        if(atoi(e->id_arriba) != 0)e->arriba = (Escenarios*)searchMap(juego, e->id_arriba)->value; // Conecta el escenario arriba
        if(atoi(e->id_izquierda) != 0)e->izquierda = (Escenarios*)searchMap(juego, e->id_izquierda)->value; // Conecta el escenario izquierda
        if(atoi(e->id_derecha) != 0)e->derecha = (Escenarios*)searchMap(juego, e->id_derecha)->value; // Conecta el escenario derecha

    }
}

void leer_mobs(HashMap * mobs){
    FILE *archivo = fopen("data/enemigos.csv", "r");
    if (archivo == NULL){
        perror("Error al abrir el archivo");
        return;
    }

    char **campos;
    campos = leer_linea_csv(archivo, ',');

    while ((campos = leer_linea_csv(archivo, ',')) != NULL){
        Enemigo * enemigo = (Enemigo*)malloc(sizeof(Enemigo));
        strcpy(enemigo->nombre, campos[0]);
        strcpy(enemigo->dificultad, campos[1]);
        strcpy(enemigo->vida, campos[2]);

        strcpy(escenario->id_arriba, campos[3]);
        strcpy(escenario->id_abajo, campos[4]);
        strcpy(escenario->id_izquierda, campos[5]);
        strcpy(escenario->id_derecha, campos[6]);

        strcpy(escenario->dificultad, campos[7]);

        insertMap(mobs, escenario->id, escenario);

    }
    fclose(archivo);
}

void mostrarMap(HashMap * juego){
    for (Pair *pp = firstMap(juego); pp; pp = nextMap(juego)) {
        Escenarios *e = (Escenarios*)pp->value;
        printf("ID: %s, Nombre: %s, Leyenda: %s, Dificultad: %s\n", e->id, e->nombre, e->leyenda, e->dificultad);
        printf("Arriba: %s, Abajo: %s, Izquierda: %s, Derecha: %s\n", e->id_arriba, e->id_abajo, e->id_izquierda, e->id_derecha);
    }
}