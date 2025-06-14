#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extra.h"

void mostrarMenuPrincipal() {
    limpiarPantalla();
    puts("========================================");
    puts("               FALTA EXP               ");
    puts("========================================");
  
    puts("1) INICIAR NUEVA PARTIDA");
    puts("2) AYUDA");
    puts("3) CREDITOS");
    puts("4) SALIR");
}

void mostrarMenuJuego(){
    limpiarPantalla();
    puts("========================================");
    puts("               FALTA EXP               ");
    puts("========================================");
  
    puts("1) EXPLORAR ZONAS");
    puts("2) VER ESTADO DEL JUGADOR");
    puts("3) RECOLECTAR ITEMS DE LA ZONA");
    puts("4) USAR ITEMS DEL INVENTARIO");
    puts("5) EQUIPAR/DES-EQUIPAR ITEM");
    puts("6) ATACAR A UN ENEMIGO");
    puts("7) CONSULTAR ENEMIGOS EN LA ZONA");
    puts("8) SALIR AL MENU PRINCIPAL");
}

void seleccionOpcion()
{
    char op;
    while (1)
    {
        //Se muestra el menú y se pide una opción
        mostrarMenuJuego();
        printf("INGRESE SU OPCION: ");
        scanf(" %c", &op);

        //Se realizan las acciones según la opción seleccionada
        switch (op) {
            case '1':
                //explorarZonas(); //FUNCIÓN PARA EXPLORAR LAS ZONAS
                break;
            case '2':
                //verEstado(); //FUNCIÓN PARA VER EL ESTADO DEL JUGADOR
                break;
            case '3':
                //recolectarItems(); //FUNCIÓN PARA RECOLECTAR LOS ITEMS DE LA ZONA
                break;
            case '4':
                //usarItem(); //FUNCIÓN PARA USAR LOS ITEMS
                break;
            case '5':
                //equipar_DesequiparItem(); //FUNCIÓN PARA EQUIPAR Y DESEQUIPAR ITEMS
                break;
            case '6':
                //atacar(); //FUNCIÓN PARA ATACAR A UN ENEMIGO
                return;
            case '7':
                //verEnemigos(); //FUNCIÓN PARA VER LOS ENEMIGOS Y SUS ESTADÍSTICAS 
                return;
            case '8':   
                return;
            default:
                printf("Opcion no valida.\n");
                break;
        }
        presioneTeclaParaContinuar();
    }   
}

int main(){
    char op;
    do{
        //Se muestra un menú principal y se selecciona una opción
        mostrarMenuPrincipal();
        printf("INGRESE SU OPCION: ");
        scanf(" %c", &op);

        //Se realizan las acciones según la opción seleccionada
        switch (op)
        {
        case '1':
            //Nueva partida
            seleccionOpcion();
            break;
        case '2':
            //Ayuda
            //mostrarAyuda();
            break;
        case '3':
            //Creditos
            //mostrarCreditos();
            break;
        case '4':
            //Salir
            puts("SALIENDO DEL JUEGO");
            break;
        default:
            //Muestra de mensaje en caso de seleccionar una opción no valida
            puts("OPCION NO VALIDA");
            break;
        }
        presioneTeclaParaContinuar();
    }while(op!= '4');
    return 0;
}