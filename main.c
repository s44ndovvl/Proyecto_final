#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"
#include "extra.h"
#include "hashmap.h"

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
  
    puts("1) EXPLORAR ZONAS"); //explorar-zona
    puts("2) VER ESTADO DEL JUGADOR"); //explorar-zona
    puts("3) RECOLECTAR ITEMS DE LA ZONA"); //gention-items
    puts("4) USAR ITEMS DEL INVENTARIO"); //gention-items
    puts("5) EQUIPAR/DES-EQUIPAR ITEM"); //gention-items
    puts("6) ATACAR A UN ENEMIGO"); //atacar-enemigo
    puts("7) CONSULTAR ENEMIGOS EN LA ZONA"); //explorar-zona
    // POSIBLE guardar partida
    puts("8) SALIR AL MENU PRINCIPAL");
}

void seleccionOpcion()
{
    bool jugadorVivo = true; // Variable para controlar si el jugador está activo
    char op;
    while(jugadorVivo)
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
                break;
            case '7':
                //verEnemigos(); //FUNCIÓN PARA VER LOS ENEMIGOS Y SUS ESTADÍSTICAS 
                break;
            case '8':   
                return;
            default:
                printf("Opcion no valida.\n");
                break;
        }
        presioneTeclaParaContinuar();
    } // El bucle continuará mientras el jugador esté activo
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
            //cargar los csv y hacer conexiones. hacerlo en un condicional para que ocurra una sola vez
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