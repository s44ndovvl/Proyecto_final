#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

//Función para recoger un item
void recoger_items(Jugador * player){
    //Mensaje si no hay items en el escenario actual
    limpiarPantalla();
    if (player->actual->items_disp == NULL || list_first(player->actual->items_disp) == NULL){
        puts("NO HAY ITEMES DISPONIBLES EN ESTE ESCENARIO :(");
        return;
    }

    while (1){
        //Se muestran lo items disponibles para recoger
        limpiarPantalla();
        puts("ITEMS DISPONIBLES PARA RECOGER");
        int i = 1;
        Item * arreglo_items[100];

        for (Item * item = list_first(player->actual->items_disp); item != NULL; item = list_next(player->actual->items_disp)){
            printf("%d) %s, %s\n", i, item->nombre, item->efecto);
            arreglo_items[i - 1] = item;
            i++;
        }
        printf("%d) RECOGER TODO\n", i);
        printf("%d) CANCELAR\n", i + 1);

        //Se pide seleccionar el item a recoger
        int opcion;
        printf("ELIGE EL NUMERO DEL ITEM A RECOGER: ");
        scanf(" %d", &opcion);

        //Mensaje para cancelar o si se selecciono una opción invalida
        if (opcion <= 0 || opcion > i){
            puts("CANCELANDO U OPCION INVALIDA");
            presioneTeclaParaContinuar();
            return;
        }

        //Se actualizan los datos del item recogido tanto en el escenario actual como en el jugador
        if (opcion == i){ //Recoger todos los items
            Item *item = list_first(player->actual->items_disp);
            
            while (item != NULL) {
            Item *siguiente = list_next(player->actual->items_disp);
            list_pushBack(player->inventario, item);
            list_popCurrent(player->actual->items_disp);
            item = siguiente;
            }

            puts("HAS RECOGIDO TODOS LOS ITEMS DEL ESCENARIO");
        }
        else{ //Recoger un item
            Item * seleccionado = arreglo_items[opcion - 1];
            list_pushBack(player->inventario, seleccionado);
        
            for (Item * item = list_first(player->actual->items_disp); item != NULL; item = list_next(player->actual->items_disp)) {
                if (item == seleccionado) {
                    list_popCurrent(player->actual->items_disp);
                    break;
                }
            }
            
            printf("RECOGISTE: %s\n", seleccionado->nombre);
        }
    }
}

int main(){
    return 0;
}