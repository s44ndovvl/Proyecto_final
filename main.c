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

        strcpy(escenario->id_arriba, campos[7]);
        strcpy(escenario->id_abajo, campos[8]);
        strcpy(escenario->id_izquierda, campos[5]);
        strcpy(escenario->id_derecha, campos[6]);
        
        //Copia los datos del CSV a la estructura del escenario
        strcpy(escenario->id, campos[0]);
        strcpy(escenario->nombre, campos[1]);
        strcpy(escenario->leyenda, campos[2]);
        
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
    for (Pair *pp = firstMap(juego); pp; pp = nextMap(juego)) // Recorre todos los escenarios
    {
        Escenarios *e = (Escenarios*)pp->value; // Obtiene el escenario actual

        if(atoi(e->id_abajo) != 0)e->abajo = (Escenarios*)searchMap(juego, e->id_abajo)->value; // Conecta el escenario abajo
        if(atoi(e->id_arriba) != 0)e->arriba = (Escenarios*)searchMap(juego, e->id_arriba)->value; // Conecta el escenario arriba
        if(atoi(e->id_izquierda) != 0)e->izquierda = (Escenarios*)searchMap(juego, e->id_izquierda)->value; // Conecta el escenario izquierda
        if(atoi(e->id_derecha) != 0)e->derecha = (Escenarios*)searchMap(juego, e->id_derecha)->value; // Conecta el escenario derecha

    }
}