#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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
    Inventario inventario;
}Jugador;

typedef struct{
    char nombre[50];
    char dificultad[50];
    int vida;
    int defensa;
    int ataque;
    int exp_dada;
    Inventario item;
}Enemigo;

void mostrarMenuPrincipal();
void mostrarMenuJuego();

void seleccionOpcion();

void leer_escenarios(HashMap * );
void mostrarMap(HashMap * );
void leer_mobs(HashMap * );
void mostrar_mobs(HashMap * );

Jugador createPlayer(char * , HashMap * );
void verEstado(HashMap * );

int main(){
    HashMap *juego = createMap(100); // Crea un HashMap para almacenar los escenarios
    HashMap *mobs = createMap(100); // Crea un HashMap para almacenar los monstruos
    Jugador * player = NULL;
    if (juego == NULL) {
        fprintf(stderr, "Error al crear el HashMap\n");
        return 1;
    }
    leer_escenarios(juego); // Llama a la funciÃ³n para leer los escenarios desde el archivo CSV
    mostrarMap(juego);
    leer_mobs(mobs); // Llama a la funciÃ³n para leer los monstruos desde el archivo CSV
    mostrar_mobs(mobs); // Muestra el contenido del HashMap


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

void leer_mobs(HashMap *mobs) {
    FILE *archivo = fopen("data/enemigos.csv", "r");
    if (!archivo) {
        perror("Error al abrir data/enemigos.csv");
        return;
    }

    // 1) Saltar cabecera
    char **campos = leer_linea_csv(archivo, ',');
    (void)campos;  // ignoramos el resultado

    // 2) Leer cada línea
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Enemigo *e = malloc(sizeof(Enemigo));
        if (!e) { perror("malloc"); break; }

        // Rellenar campos básicos
        strncpy(e->nombre,     campos[0], sizeof(e->nombre)-1);
        strncpy(e->dificultad, campos[1], sizeof(e->dificultad)-1);
        e->vida = atoi(campos[2]);
        e->ataque = atoi(campos[3]);
        e->defensa = atoi(campos[4]);
        e->exp_dada = atoi(campos[5]);

        // Inicializar inventario
        // ----------------------------------------------------------------
        //  Arma
        char *tmp = strdup(campos[6]);
        char *tok = strtok(tmp, ",");
        strncpy(e->item.armas.nombre, tok, sizeof(e->item.armas.nombre)-1);
        tok = strtok(NULL, ","); e->item.armas.ataque     = atoi(tok);
        tok = strtok(NULL, ","); e->item.armas.durabilidad = atoi(tok);
        free(tmp);

        //  Armaduras (casco, guantes, pechera, pantalones, botas)
        const int idx[5] = {7,8,9,10,11};
        Armadura *pieces[5] = {
            &e->item.casco,
            &e->item.guantes,
            &e->item.pechera,
            &e->item.pantalones,
            &e->item.botas
        };
        for (int i = 0; i < 5; i++) {
            char *tmp = strdup(campos[idx[i]]);
            char *tok = strtok(tmp, ",");

            // separar tipo y nombre
            char *sp = strchr(tok, ' ');
            if (sp) {
                *sp = '\0';
                strncpy(pieces[i]->tipo,   tok,       sizeof(pieces[i]->tipo)-1);
                strncpy(pieces[i]->nombre, sp+1,     sizeof(pieces[i]->nombre)-1);
            } else {
                strncpy(pieces[i]->tipo, tok,        sizeof(pieces[i]->tipo)-1);
            }

            // leer el resto de campos
            tok = strtok(NULL, ","); pieces[i]->defensa    = atoi(tok);
            tok = strtok(NULL, ","); pieces[i]->durabilidad = atoi(tok);
            tok = strtok(NULL, ",");
            tok = strtok(NULL, ","); pieces[i]->valor      = atoi(tok);

            free(tmp);
        }

        //  Poción
        e->item.pocion = list_create();
        {
            char *tmp = strdup(campos[12]);
            char *tok = strtok(tmp, ",");
            Pocion *p = malloc(sizeof(Pocion));
            strncpy(p->nombre, tok, sizeof(p->nombre)-1);

            tok = strtok(NULL, ",");
            strncpy(p->efecto, tok, sizeof(p->efecto)-1);

            tok = strtok(NULL, ",");
            p->valor = atoi(tok);

            list_pushBack(e->item.pocion, p);
            free(tmp);
        }
        // ----------------------------------------------------------------

        // Insertar en el HashMap
        insertMap(mobs, e->nombre, e);
    }

    fclose(archivo);
}


void mostrar_mobs(HashMap *mobs) {
    Pair *pp = firstMap(mobs);
    if (!pp) {
        printf("No hay monstruos cargados.\n");
        return;
    }

    printf("=== Lista de Monstruos ===\n");
    for (; pp; pp = nextMap(mobs)) {
        Enemigo *e = (Enemigo*)pp->value;

        // Estadísticas básicas
        printf("Nombre: %s | Dif: %s | Vida: %d | Atk: %d | Def: %d | Exp: %d\n",
            e->nombre, e->dificultad, e->vida,
            e->ataque, e->defensa, e->exp_dada
        );

        // Arma
        Arma *a = &e->item.armas;
        printf("  • Arma:    %s (Atk %d, Dur %d)\n",
            a->nombre, a->ataque, a->durabilidad
        );

        // Armaduras
        Armadura *c = &e->item.casco;
        printf("  • Casco:   %s %s (Def %d, Dur %d, Val %d)\n",
            c->tipo, c->nombre, c->defensa, c->durabilidad, c->valor
        );
        Armadura *g = &e->item.guantes;
        printf("  • Guantes: %s %s (Def %d, Dur %d, Val %d)\n",
            g->tipo, g->nombre, g->defensa, g->durabilidad, g->valor
        );
        Armadura *p = &e->item.pechera;
        printf("  • Pechera: %s %s (Def %d, Dur %d, Val %d)\n",
            p->tipo, p->nombre, p->defensa, p->durabilidad, p->valor
        );
        Armadura *pa= &e->item.pantalones;
        printf("  • Pantalones: %s %s (Def %d, Dur %d, Val %d)\n",
            pa->tipo, pa->nombre, pa->defensa, pa->durabilidad, pa->valor
        );
        Armadura *b = &e->item.botas;
        printf("  • Botas:   %s %s (Def %d, Dur %d, Val %d)\n",
            b->tipo, b->nombre, b->defensa, b->durabilidad, b->valor
        );

        // Poción(es)
        if (list_size(e->item.pocion) > 0) {
            printf("  • Pociones:\n");
            for (Pocion *ppoc = list_first(e->item.pocion);
                 ppoc;
                 ppoc = list_next(e->item.pocion))
            {
                printf("      - %s (%s, Val %d)\n",
                    ppoc->nombre, ppoc->efecto, ppoc->valor
                );
            }
        }
        putchar('\n');
    }
}

void mostrarMap(HashMap * juego){
    for (Pair *pp = firstMap(juego); pp; pp = nextMap(juego)) {
        Escenarios *e = (Escenarios*)pp->value;
        printf("ID: %s, Nombre: %s, Leyenda: %s, Dificultad: %s\n", e->id, e->nombre, e->leyenda, e->dificultad);
        printf("Arriba: %s, Abajo: %s, Izquierda: %s, Derecha: %s\n", e->id_arriba, e->id_abajo, e->id_izquierda, e->id_derecha);
    }
}

Jugador * createPlayer(char nombre[], HashMap * juego){
    Jugador * player = (Jugador *)malloc(sizeof(Jugador));

    strcpy(player->nombre, nombre);
    Pair * inicio = firstMap(juego);
    player->vida = 100;
    player->estamina = 15;
    player->defensa = 100;
    player->ataque = 1;
    player->experiencia = 0;
    player->nivel = 0;
    player->actual = inicio->value;
    
    strcpy(player->inventario.armas.nombre, "Sin arma");
    player->inventario.armas.ataque = 0;
    player->inventario.armas.durabilidad = 0;

    Armadura vacia;
    strcpy(vacia.nombre, "Sin armadura");
    strcpy(vacia.tipo, "");
    vacia.defensa = 0;
    vacia.durabilidad = 0;
    strcpy(vacia.bufo, "");
    vacia.valor = 0;

    player->inventario.casco = vacia;
    player->inventario.pechera = vacia;
    player->inventario.guantes = vacia;
    player->inventario.pantalones = vacia;
    player->inventario.botas = vacia;

    player->inventario.pocion = list_create();

    return player;
}

void verEstado(HashMap * player){

}