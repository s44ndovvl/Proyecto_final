#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
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
    Arma armas;
    Armadura casco;
    Armadura guantes;
    Armadura pechera;
    Armadura pantalones;
    Armadura botas;
    List *pocion;
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

void recoger_items(Jugador *player, Enemigo *enemigo) {
    limpiarPantalla();

    Inventario *loot = enemigo->item;
    if (!loot) {
        puts("El enemigo no dejó objetos.");
        presioneTeclaParaContinuar();
        return;
    }

    // Se construye una lista temporal con los 7 ítems fijos
    List *items_posibles = list_create();

    // Arma
    Arma *arma = malloc(sizeof(Arma));
    *arma = loot->armas;
    list_pushBack(items_posibles, arma);

    // Armaduras
    Armadura *casco = malloc(sizeof(Armadura)); 
    *casco = loot->casco; list_pushBack(items_posibles, casco);
    Armadura *guantes = malloc(sizeof(Armadura)); 
    *guantes = loot->guantes; list_pushBack(items_posibles, guantes);
    Armadura *pechera = malloc(sizeof(Armadura)); 
    *pechera = loot->pechera; list_pushBack(items_posibles, pechera);
    Armadura *pantalones = malloc(sizeof(Armadura)); 
    *pantalones = loot->pantalones; list_pushBack(items_posibles, pantalones);
    Armadura *botas = malloc(sizeof(Armadura)); 
    *botas = loot->botas; list_pushBack(items_posibles, botas);

    // Poción
    Pocion *p = list_first(loot->pocion);
    if (p) {
        Pocion *pocion = malloc(sizeof(Pocion));
        *pocion = *p;
        list_pushBack(items_posibles, pocion);
    }

    while (1) {
        limpiarPantalla();
        puts("=== OBJETOS DEL ENEMIGO ===");

        int index = 1;
        void *objeto = list_first(items_posibles);
        List *temp = list_create();

        while (objeto != NULL) {
            list_pushBack(temp, objeto);

            // Mostrar según tipo
            if (index == 1) {
                Arma *arma = objeto;
                printf("%d) [ARMA] %s - Ataque: %d - Durabilidad: %d\n", index, arma->nombre, arma->ataque, arma->durabilidad);
            } else if (index >= 2 && index <= 6) {
                Armadura *armadura = objeto;
                printf("%d) [ARMADURA] %s - Defensa: %d - Durabilidad: %d\n", index, armadura->nombre, armadura->defensa, armadura->durabilidad);
            } else {
                Pocion *pocion = objeto;
                printf("%d) [POCIÓN] %s - %s (%d)\n", index, pocion->nombre, pocion->efecto, pocion->valor);
            }

            index++;
            objeto = list_next(items_posibles);
        }

        printf("%d) TOMAR TODO\n", index);
        printf("%d) CANCELAR\n", index + 1);
        printf("\nSELECCIONE UNA OPCION: ");
        int opcion;
        scanf("%d", &opcion);

        if (opcion == index + 1 || opcion <= 0) {
            puts("CANCELANDO U OPCION INVALIDA");
            presioneTeclaParaContinuar();
            break;
        }

        if (opcion == index) {
            for (void *elemento = list_first(items_posibles); elemento != NULL; elemento = list_next(items_posibles)) {
                list_pushBack(player->inventario, elemento);
            }
            puts("HAS RECOGIDO TODOS LOS OBJETOS");
            presioneTeclaParaContinuar();
            break;
        } else {
            int actual = 1;
            void *item = list_first(temp);
            while (item != NULL && actual < opcion) {
                item = list_next(temp);
                actual++;
            }

            if (item != NULL) {
                list_pushBack(player->inventario, item);

                // reconstruir lista sin el ítem tomado
                List *nueva = list_create();
                for (void *it = list_first(items_posibles); it != NULL; it = list_next(items_posibles)) {
                    if (it != item) list_pushBack(nueva, it);
                }

                list_clean(items_posibles);
                for (void *it = list_first(nueva); it != NULL; it = list_next(nueva)) {
                    list_pushBack(items_posibles, it);
                }

                puts("Objeto recogido con éxito.");
                presioneTeclaParaContinuar();
            }
        }
    }
}

int main(){
    return 0;
}