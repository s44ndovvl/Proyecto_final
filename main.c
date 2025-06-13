#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "extra.h"
#include "hashmap.h"

typedef struct Escenarios Escenarios;

//Estructura para cada Item
typedef struct{
    char nombre[50];
    char efecto[50];
    int valor;
}Item;

//Estructura para cada escenario(nivel)
struct Escenarios{
    char id;
    char nombre[50];
    char leyenda[500];
    char id_arriba;
    char id_abajo;
    char id_izquierda;
    char id_derecha;
    List *Enemigos;
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
    int vida;
    int defensa;
    int ataque;
    int exp_dada;
    Item *item;
}Enemigo;

int main(){
    printf("hello world");
    return 0;
}

/*void leer_escenarios(HashMap * juego){
    FILE *archivo = fopen("data/graphquest.csv", "r");
    if (archivo == NULL){
        perror("Error al abrir el archivo");
        return;
    }

    char **campos;
    campos = leer_linea_csv(archivo, ',');

    while ((campos = leer_linea_csv(archivo, ',')) != NULL){
        Escenarios * escenario = (Escenarios*)malloc(sizeof(Escenarios));
        //Inicializa los punteros de direccion a NULL para evitar punteros basura
        escenario->id_arriba = campos[7];
        escenario->id_abajo = campos[8];
        escenario->id_izquierda = campos[5];
        escenario->id_derecha = campos[6];
        
        //Copia los datos del CSV a la estructura del escenario
        strncpy(escenario->id, campos[0], sizeof(escenario->id));
        strncpy(escenario->nombre, campos[1], sizeof(escenario->nombre));
        strncpy(escenario->leyenda, campos[2], sizeof(escenario->leyenda));
        
        List * items = split_string(campos[3], ";");
        List * lista_items = list_create();

        

        strcpy(escenario->id_arriba, campos[4]);
        strcpy(escenario->id_abajo, campos[5]);
        strcpy(escenario->id_izquierda, campos[6]);
        strcpy(escenario->id_derecha, campos[7]);

        insertMap(juego, escenario->id, escenario);

    }
    fclose(archivo);

    // Segunda pasada: establecer los punteros de direcciÃ³n entre escenarios
    /*List * claves = list_create();
    Pair * par = firstMap(juego);
    while (par != NULL) {
        list_pushBack(claves, par->key);
        par = nextMap(juego);
    }

    // Ahora recorremos esa lista para validar enlaces sin afectar el iterador de juego
    for (char * clave = list_first(claves); clave != NULL; clave = list_next(claves)) {
        Pair * pEscenario = searchMap(juego, clave);
        if (!pEscenario) continue;

        Escenarios * escenario = (Escenarios*)pEscenario->value;

        if (strcmp(escenario->id_arriba, "-1") != 0){
            Pair * aux = searchMap(juego, escenario->id_arriba);
            if (aux != NULL) escenario->dir_posibles.arriba = (Escenarios*)aux->value;
        }
        if (strcmp(escenario->id_abajo, "-1") != 0){
            Pair * aux = searchMap(juego, escenario->id_abajo);
            if (aux != NULL) escenario->dir_posibles.abajo = (Escenarios*)aux->value;
        }
        if (strcmp(escenario->id_izquierda, "-1") != 0){
            Pair * aux = searchMap(juego, escenario->id_izquierda);
            if (aux != NULL) escenario->dir_posibles.izquierda = (Escenarios*)aux->value;
        }
        if (strcmp(escenario->id_derecha, "-1") != 0){
            Pair * aux = searchMap(juego, escenario->id_derecha);
            if (aux != NULL) escenario->dir_posibles.derecha = (Escenarios*)aux->value;
        }
    }

    list_clean(claves);
    free(claves);    
}*/