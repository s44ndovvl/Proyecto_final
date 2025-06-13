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

//Estructura para las direcciones posibles
typedef struct{
    Escenarios * arriba;
    Escenarios * abajo;
    Escenarios * izquierda;
    Escenarios * derecha;
}Direccion;

//Estructura para cada escenario(nivel)
struct Escenarios{
    char id[100];
    char nombre[50];
    char descripcion[500];
    List * items_disp;
    Direccion  dir_posibles;
    char id_arriba[3];
    char id_abajo[3];
    char id_izquierda[3];
    char id_derecha[3];
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
    Escenarios *actual;
}Enemigo;

int main(){
    printf("hello world");
    return 0;
}