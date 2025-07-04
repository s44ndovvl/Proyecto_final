#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "list.h"
#include "extra.h"
#include "hashmap.h"
#define NORMAL 0
#define ESPECIAL 1

/**********************************************/
/*                 Estructuras                */
/**********************************************/

typedef struct Escenarios Escenarios;

// Estructura para las armaduras
typedef struct{
    char tipo[50];
    char nombre[50];
    int defensa;
    int durabilidad;
    char bufo[50];
    int valor;
}Armadura;

// Estructura para las armas
typedef struct{
    char nombre[50];
    int ataque;
    int durabilidad;
}Arma;

//  Estructura para cada Item
typedef struct{
    char nombre[50];
    char efecto[50];
    int valor;
}Pocion;

// Estructura para cada Item
typedef struct{
    Arma armas;
    Armadura casco;
    Armadura guantes;
    Armadura pechera;
    Armadura pantalones;
    Armadura botas;
    List *pocion;
}Inventario;

// Estructura para cada escenario (nivel)
struct Escenarios{
    char id[4];
    char nombre[50];
    char leyenda[500];
    char id_arriba[4];
    char id_abajo[4];
    char id_izquierda[4];
    char id_derecha[4];
    Escenarios *arriba;
    Escenarios *abajo;
    Escenarios *izquierda;
    Escenarios *derecha;
    List *Enemigos;
    char dificultad[8];
};

// Estructura para el jugador
typedef struct{
    char nombre[50];

    int vida;           /**********************************************/
    int max_vida;       /*                    VIDA                    */
    int vida_total;     /**********************************************/

    int estamina;       /**********************************************/
    int max_estamina;   /*                  ESTAMINA                  */
    int estamina_total; /**********************************************/

    int defensa;        /**********************************************/
    int max_defensa;    /*                   DEFENSA                  */
    int defensa_total;  /**********************************************/

    int ataque;         /**********************************************/
    int max_ataque;     /*                   ATAQUE                   */
    int ataque_total;   /**********************************************/


    int experiencia;
    int nivel;

    Escenarios *actual;
    Inventario inventario;
    int inmunidad;
    bool ReydemonioDerrotado;
}Jugador;

// Estructura para los enemigos
typedef struct{
    char nombre[50];
    char dificultad[50];
    int vida;
    int defensa;
    int ataque;
    int exp_dada;
    Inventario item;
}Enemigo;

// Estructura para los items
typedef struct {
    int tipo; // 0 = arma, 1 = armadura, 2 = pocion
    void *ptr;
} ItemAux;

/**********************************************/
/*                 Prototipado                */
/**********************************************/
void mostrarCreditos();

void mostrarMenuAyuda();
void mostrarMenuPrincipal();
void mostrarMenuJuego();
void menuOpcionesPelea();

void leer_escenarios(HashMap * );
void leer_mobs(HashMap * );
void asignar_mobs(HashMap * , HashMap * );
Enemigo * seleccionarEnemigo(List * );
Jugador * createPlayer(char * , HashMap * );

void mostrar_mobs(HashMap * );
void mostrarMap(HashMap * );

bool usarPociones(Jugador * );
void aplicarBufo(Jugador * , const char *, int );
void calcularEstatsT(Jugador * );
void reiniciarArma(Arma * );
bool realizarAtaque(Jugador * , Enemigo * , bool );
void reiniciarArmadura(Armadura * );
void ataqueEnemigo(Jugador * , Enemigo * );
void reiniciarEstasBase(Jugador * );

bool huida(Jugador *, Enemigo *);
void guardar_item(Inventario * , void * , int );
void recoger_items_enemigo(Jugador *, Enemigo *);

bool cicloPelea(Jugador * , List * );

void seleccionOpcion(Jugador * );
void seleccionOpcionAyuda();

bool movermeDeEscenario(Jugador * );
void lvlup(Jugador * );

void verEstado(Jugador * );
void victoria();
void liberarMemoria(Jugador *,HashMap *,HashMap *,List *,List *,List *);

/**********************************************/
/*       Declaracion variables globales       */
/**********************************************/

static Jugador *g_player   = NULL;
static HashMap *g_juego    = NULL;
static HashMap *g_mobs     = NULL;
static List *g_facil       = NULL;
static List *g_medio       = NULL;
static List *g_dificil     = NULL;
static List *g_boss        = NULL;

/**********************************************/
/*                    Main                    */
/**********************************************/

int main(){
    // Se inicializan los mapas de juego y enemigos 
    g_juego   = createMap(100);
    g_mobs    = createMap(100);
    if (!g_juego) {
        fprintf(stderr, "Error al crear el HashMap\n");
        return 1;
    }
    srand(time(NULL));

    // Se leen los CSV de los escenarios y mobs, y se asignan los enemigos en el mapa
    leer_escenarios(g_juego);
    leer_mobs(g_mobs);
    asignar_mobs(g_juego, g_mobs);

    char op;
    char name[50];
    do{
        mostrarMenuPrincipal();
        printf("INGRESE SU OPCION: ");
        scanf(" %c", &op);
        while (getchar() != '\n');

        switch (op) {
        case '1': // OPCION 1: INICIAR NUEVA PARTIDA
            // Se lee el nombre del jugador
            printf("INDIQUE EL NOMBRE DEL NUEVO JUGADOR: ");
            scanf(" %49s", name);
            while (getchar() != '\n');

            // Se crea el jugador
            g_player = createPlayer(name, g_juego);

            // Si se creas correctamente el jugador se avanza almenu de juego, en caso contrario
            // se muestra un mensaje de error
            if (g_player) seleccionOpcion(g_player);
            else puts("No se pudo crear el jugador. Revise los datos de escenarios.");
            break;
        case '2': // OPCION 2: AYUDA
            seleccionOpcionAyuda(); // Se abre el menu de ayuda 
            break;
        case '3': // OPCION 3: CREDITOS
            mostrarCreditos(); // Se muestran los creditos del juego
            break;
        case '4': // OPCION 4: SALIR
            // Se muestra un mensaje de salida y se libera la memoria utiizada
            limpiarPantalla();
            puts("========================================");
            puts("               FALTA XP                 ");
            puts("========================================");
            puts("SALIENDO DEL JUEGO");
            if (g_player != NULL) liberarMemoria(g_player, g_juego, g_mobs, g_facil, g_medio, g_dificil);
            break;
        default: // OPCIONES NO VALIDAS
            puts("OPCION NO VALIDA"); // Mensaje en caso de seleccionar una opcion no valida
            break;
        }
        presioneTeclaParaContinuar();
    } while(op != '4');

    return 0;
}

/**********************************************/
/*            Mostrar creditos                */
/**********************************************/

void mostrarCreditos()
{
    limpiarPantalla();

    // Se guardan todos los textos a mostrar en los creditos
    const char* creditos[] = {
        "Falta Creatividad Studios Presenta:",
        "Falta XP",
        "",
        "Director: Eduardo Sandoval",
        "Productor: Joaquin Contreras",
        "Gestor de Calidad: Brandon Caceres",
        "Responsable de Integracion: Josue Huaiquil",
        "Coordinador: Eduardo Sandoval",
        "Creacion de Enemigos: Brandon Caceres",
        "Interface Designer: Josue Huaiquil", "",
        "Equipo de Programacion:",
        "Eduardo Sandoval",
        "Joaquin Contreras",
        "Brandon Caceres",
        "Josue Huaiquil", "",
        "Gracias por jugar Falta XP!"
    };

    // Se calcula la cntidad de elementos en el arreglo de textos 
    int cantidad = sizeof(creditos) / sizeof(creditos[0]);

    // Se muestra por pantalla cada texto cada 1 segundo
    for (int i = 0; i < cantidad; i++) {
        printf("%s\n", creditos[i]);
        esperar(1);         // Espera 1 segundos
    }
}

/**********************************************/
/*               Mostrar menus                */
/**********************************************/

// Menu de ayuda    
void mostrarMenuAyuda() {
    limpiarPantalla();
    puts("========================================");
    puts("              MENU DE AYUDA             ");
    puts("========================================");
  
    puts("1) SOBRE LOS MOVIMIENTOS");
    puts("2) SOBRE LOS ENEMIGOS");
    puts("3) SOBRE EL EQUIPAMENTO");
    puts("4) VOLVER AL MENU PRINCIPAL");
}

// Menu principal
void mostrarMenuPrincipal() {
    limpiarPantalla();
    puts("========================================");
    puts("               FALTA XP                ");
    puts("========================================");
  
    puts("1) INICIAR NUEVA PARTIDA");
    puts("2) AYUDA");
    puts("3) CREDITOS");
    puts("4) SALIR");
}

// Menu del juego
void mostrarMenuJuego(){
    limpiarPantalla();
    puts("========================================");
    puts("               FALTA XP                ");
    puts("========================================");
  
    puts("1) EXPLORAR ZONAS"); //explorar-zona
    puts("2) VER ESTADO DEL JUGADOR"); //explorar-zona
    puts("3) ATACAR A UN ENEMIGO"); //atacar-enemigo
    // POSIBLE guardar partida
    puts("4) SALIR AL MENU PRINCIPAL");
}

// Menu de pelea
void menuOpcionesPelea()
{
    puts("========================================");
    puts("              MENU DE PELEA                   ");
    puts("========================================");
    puts("1) ATACAR AL ENEMIGO");
    puts("2) ATAQUE ESPECIAL");
    puts("3) USAR POCION");
    puts("4) HUIR DE LA PELEA");
    puts("=========================================");
}

/**********************************************/
/*                  Leer CSVs                 */
/**********************************************/

// Funcion para leer el CSV de escenarios
void leer_escenarios(HashMap * juego){
    // Se abre el archivo mapa.csv
    FILE *archivo = fopen("data/mapa.csv", "r");

    // En caso de que el archivo sea NULL se muestra un mensaje de error
    if (archivo == NULL){
        perror("Error al abrir el archivo");
        return;
    }

    char **campos;
    campos = leer_linea_csv(archivo, ',');

    while ((campos = leer_linea_csv(archivo, ',')) != NULL){
        // Se reserva memoria para cada escenario
        Escenarios * escenario = (Escenarios*)malloc(sizeof(Escenarios));

        //Inicializa los punteros de direccion a NULL para evitar punteros basura
        escenario->arriba = NULL;
        escenario->abajo = NULL;
        escenario->izquierda = NULL;
        escenario->derecha = NULL;
        
        //Copia los datos del CSV a la estructura del escenario
        strncpy(escenario->id, campos[0], sizeof(escenario->id));
        strncpy(escenario->nombre, campos[1], sizeof(escenario->nombre));
        strncpy(escenario->leyenda, campos[2], sizeof(escenario->leyenda));

        strcpy(escenario->id_izquierda, campos[3]);
        strcpy(escenario->id_derecha, campos[4]);
        strcpy(escenario->id_arriba, campos[5]);
        strcpy(escenario->id_abajo, campos[6]);

        strncpy(escenario->dificultad, campos[7], sizeof(escenario->dificultad));

        // Se inserta el escenario en el mapa de juego
        insertMap(juego, escenario->id, escenario);
    }
    fclose(archivo); // Se cierra el archivo

    // Segunda pasada: Establecer los punteros de dirección entre escenarios
    List * claves = list_create();
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

        if (strcmp(escenario->id_izquierda, "-1") != 0){
            Pair * aux = searchMap(juego, escenario->id_izquierda);
            if (aux != NULL) escenario->izquierda = (Escenarios*)aux->value;
        }
        if (strcmp(escenario->id_derecha, "-1") != 0){
            Pair * aux = searchMap(juego, escenario->id_derecha);
            if (aux != NULL) escenario->derecha = (Escenarios*)aux->value;
        }
        if (strcmp(escenario->id_arriba, "-1") != 0){
            Pair * aux = searchMap(juego, escenario->id_arriba);
            if (aux != NULL) escenario->arriba = (Escenarios*)aux->value;
        }
        if (strcmp(escenario->id_abajo, "-1") != 0){
            Pair * aux = searchMap(juego, escenario->id_abajo);
            if (aux != NULL) escenario->abajo = (Escenarios*)aux->value;
        }
    }

    // Se limpia la lista de claves y se libera la memoria
    list_clean(claves);
    free(claves);
}

// Funcion para leer el CSV mobs
void leer_mobs(HashMap *mobs) {
    // Se crean las listas para las distintas dificultades
    List * facil = list_create();
    List * media = list_create();
    List * dificil = list_create();
    List * boss = list_create();

    // Se abre el archivo enemigos.csv
    FILE *archivo = fopen("data/enemigos.csv", "r");

    // En caso que el archivo sea NULL se muestra un mensaje de error   
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
        // Arma
        char *tmp = strdup(campos[6]);
        char *tok = strtok(tmp, ",");
        strncpy(e->item.armas.nombre, tok, sizeof(e->item.armas.nombre)-1);
        tok = strtok(NULL, ","); e->item.armas.ataque     = atoi(tok);
        tok = strtok(NULL, ","); e->item.armas.durabilidad = atoi(tok);
        free(tmp);

        // Armaduras (casco, guantes, pechera, pantalones, botas)
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

            // Separar tipo y nombre
            char *sp = strchr(tok, ' ');
            if (sp) {
                *sp = '\0';
                strncpy(pieces[i]->tipo,   tok,       sizeof(pieces[i]->tipo)-1);
                strncpy(pieces[i]->nombre, sp+1,     sizeof(pieces[i]->nombre)-1);
            } else {
                strncpy(pieces[i]->tipo, tok,        sizeof(pieces[i]->tipo)-1);
            }

            // Leer el resto de campos
            tok = strtok(NULL, ","); pieces[i]->defensa    = atoi(tok);
            tok = strtok(NULL, ","); pieces[i]->durabilidad = atoi(tok);
            tok = strtok(NULL, ",");
            tok = strtok(NULL, ","); pieces[i]->valor      = atoi(tok);

            // Se libera memoria
            free(tmp);
        }

        // Poción
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

            // Se libera memoria
            free(tmp);
        }
        // ----------------------------------------------------------------

        // Se agrega el mob a la lista de enemigos, segun su dificultad
        if (strcmp(e->dificultad, "Facil") == 0) list_pushBack(facil, e);
        if (strcmp(e->dificultad, "Media") == 0) list_pushBack(media, e);
        if (strcmp(e->dificultad, "Dificil") == 0) list_pushBack(dificil, e);
        if (strcmp(e->dificultad, "Boss") == 0) list_pushBack(boss, e);
    }

    // Se actualizan las variables globales asignandoles las listas de mobs
    g_facil = facil;
    g_medio = media;
    g_dificil = dificil;
    g_boss = boss;

    // Se insertan las listas de mobs en el mapa
    insertMap(mobs, "facil", facil);
    insertMap(mobs, "media", media);
    insertMap(mobs, "dificil", dificil);
    insertMap(mobs, "Boss", boss);

    // Se cierra el archivo
    fclose(archivo);
}

/**********************************************/
/*          Asignar mobs a escenarios         */
/**********************************************/

// Funcion para asignar enemigos al mapa
void asignar_mobs(HashMap * escenarios, HashMap * mobs){
    for (Pair * esc = firstMap(escenarios); esc != NULL; esc = nextMap(escenarios)){
        Escenarios * escenario = esc->value;

        escenario->Enemigos = list_create();

        // Se asignan mobs al escenario segun la dificultad que tenga asignado este
        if (strcmp(escenario->dificultad, "Facil") == 0) escenario->Enemigos = searchMap(mobs, "facil")->value;
        if (strcmp(escenario->dificultad, "Media") == 0) escenario->Enemigos = searchMap(mobs, "media")->value;
        if (strcmp(escenario->dificultad, "Dificil") == 0) escenario->Enemigos = searchMap(mobs, "dificil")->value;
        if (strcmp(escenario->dificultad, "Boss") == 0) escenario->Enemigos = searchMap(mobs, "Boss")->value;
    }
}

// Funcion para copiar la armadura
void copiaArmadura(Armadura * seleccionado, Armadura * item){
    strcpy(seleccionado->tipo, item->tipo);
    strcpy(seleccionado->nombre, item->nombre);
    seleccionado->defensa = item->defensa;
    seleccionado->durabilidad = item->durabilidad;
    strcpy(seleccionado->bufo, item->bufo);
    seleccionado->valor = item->valor;
}

// Funcion para seleccionar 
Enemigo * seleccionarEnemigo(List * enemigos){
    if(enemigos == NULL){
        return NULL;
    }

    int num_enemigos = 0;
    for (Enemigo *mob = list_first(enemigos); mob != NULL; mob = list_next(enemigos)){
        num_enemigos++;
    }
    if (num_enemigos == 0) return NULL; // No enemies to select, so return NULL

    int index = rand() % num_enemigos;

    int i = 0;
    for (Enemigo *mob = list_first(enemigos); mob != NULL; mob = list_next(enemigos)){
        if (i == index){
            Enemigo * seleccionado = malloc(sizeof(Enemigo));
            strcpy(seleccionado->nombre, mob->nombre);
            strcpy(seleccionado->dificultad, mob->dificultad);

            seleccionado->vida = mob->vida;
            seleccionado->defensa = mob->defensa;
            seleccionado->ataque = mob->ataque;
            seleccionado->exp_dada = mob->exp_dada;

            strcpy(seleccionado->item.armas.nombre, mob->item.armas.nombre);
            seleccionado->item.armas.durabilidad = mob->item.armas.durabilidad;
            seleccionado->item.armas.ataque = mob->item.armas.ataque;

            copiaArmadura(&seleccionado->item.casco, &mob->item.casco);
            copiaArmadura(&seleccionado->item.pechera, &mob->item.pechera);
            copiaArmadura(&seleccionado->item.guantes, &mob->item.guantes);
            copiaArmadura(&seleccionado->item.pantalones, &mob->item.pantalones);
            copiaArmadura(&seleccionado->item.botas, &mob->item.botas);

            seleccionado->item.pocion = list_create();
            if (mob->item.pocion != NULL) {
                for (Pocion * p = list_first(mob->item.pocion); p != NULL; p = list_next(mob->item.pocion)){
                    Pocion * nueva = malloc(sizeof(Pocion));
                    *nueva = *p;
                    list_pushBack(seleccionado->item.pocion, nueva);
                }
            }

            return seleccionado;
        }
        i++;
    }
    return NULL;
}

/**********************************************/
/*              Crear al jugador              */
/**********************************************/

Jugador * createPlayer(char nombre[], HashMap * juego){
    // Se reserva memoria para el jugador
    Jugador * player = (Jugador *)malloc(sizeof(Jugador));
    if (player == NULL) exit(1);

    //
    strcpy(player->nombre, nombre);
    Pair * inicio = firstMap(juego);
    if (inicio == NULL) {
        printf("Error: No hay escenarios cargados en el juego.\n");
        free(player);
        return NULL;
    }
    player->vida = 100;
    player->max_vida = 100;
    player->vida_total = 0;
    player->estamina = 20;
    player->max_estamina = 20;
    player->estamina_total = 0;
    player->defensa = 10;
    player->max_defensa = 10;
    player->defensa_total = 0;
    player->ataque = 6;
    player->ataque_total = 6;
    player->max_ataque = 6;
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
    Pocion *p = malloc(sizeof(Pocion));
    strcpy(p->nombre, "Pocion de Vida");
    strcpy(p->efecto, "Vida");
    p->valor = 20;
    list_pushBack(player->inventario.pocion, p);

    player->inmunidad = false;

    return player;
}

/**********************************************/
/*                   Mostrar                  */
/**********************************************/

// Funcion para mostrar mobs
void mostrar_mobs(HashMap *mobs) {
    // Se carga el primer elemento del mapa de mobs
    Pair *pp = firstMap(mobs);
    if (!pp) {
        printf("No hay monstruos cargados.\n");
        return;
    }

    // Se inicia un bucle para mostrar los mobs y sus estadisticas
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

// Funcion para mostrar el mapa
void mostrarMap(HashMap * juego){
    // Bucle para imprimir cada escenario del mapa
    for (Pair *pp = firstMap(juego); pp; pp = nextMap(juego)) {
        Escenarios *e = (Escenarios*)pp->value;
        printf("ID: %s, Nombre: %s, Leyenda: %s, Dificultad: %s\n", e->id, e->nombre, e->leyenda, e->dificultad);
        printf("Arriba: %s, Abajo: %s, Izquierda: %s, Derecha: %s\n", e->id_arriba, e->id_abajo, e->id_izquierda, e->id_derecha);
    }
}

/**********************************************/
/*               Ciclo de Pelea               */
/**********************************************/

// Funcion para reiniciar las estadisticas del jugador
void reiniciarEstasBase(Jugador * player){
    // Reinicia las estadisticas base del jugador
    player->defensa = player->max_defensa;
    player->ataque = player->max_ataque;
}

// Funcion para guardar item
void guardar_item(Inventario * inv, void * item, int tipo){
    if (!item) return;

    // Comprobamos si es poción
    if (tipo == 2) { // Poción
        Pocion *poc = (Pocion *)item;
        list_pushBack(inv->pocion, poc);
        return;
    }

    // Comprobamos si es arma
    if (tipo == 0){
        Arma * arma = (Arma *)item;
        if (arma->ataque > 0 && arma->durabilidad > 0){
            printf("\n[ARMA]\n");
            printf("Arma actual: %s (Ataque: %d, Durabilidad: %d)\n", inv->armas.nombre, inv->armas.ataque, inv->armas.durabilidad);
            printf("Nueva arma:  %s (Ataque: %d, Durabilidad: %d)\n", arma->nombre, arma->ataque, arma->durabilidad);
            printf("Deseas reemplazar tu arma actual? (1 = SI, 2 = NO): ");
            int decision;
            scanf("%d", &decision);
            while (getchar() != '\n');
            if (decision == 1) {
                inv->armas = *arma;
                printf("Has reemplazado tu arma con exito.\n");
            } else {
                printf("No se reemplazo el arma.\n");
            }
            return;
        }
    }

    //Comprobamos si es armadura
    if (tipo == 1){
        Armadura * armadura = (Armadura *)item;
        Armadura * actual = NULL;
        if(strcmp(armadura->tipo, "Casco") == 0) actual = &inv->casco;
        else if (strcmp(armadura->tipo, "Pechera") == 0) actual = &inv->pechera;
        else if (strcmp(armadura->tipo, "Guantes") == 0) actual = &inv->guantes;
        else if (strcmp(armadura->tipo, "Pantalones") == 0) actual = &inv->pantalones;
        else if (strcmp(armadura->tipo, "Botas") == 0) actual = &inv->botas;

        if (actual) {
            printf("\n[%s]\n", armadura->tipo);
            printf("Armadura actual: %s %s (Defensa: %d, Durabilidad: %d)\n", actual->tipo, actual->nombre, actual->defensa, actual->durabilidad);
            printf("Nueva armadura:  %s %s (Defensa: %d, Durabilidad: %d)\n", armadura->tipo, armadura->nombre, armadura->defensa, armadura->durabilidad);
            printf("Deseas reemplazar esta armadura? (1 = SI, 2 = NO): ");
            int decision;
            scanf("%d", &decision);
            while (getchar() != '\n');
            if (decision == 1) {
                *actual = *armadura;
                printf("Has reemplazado la armadura con exito.\n");
            } else {
                printf("No se reemplazo la armadura.\n");
            }
        }
    }
}

// Funcion para recoger items del enemigo
void recoger_items_enemigo(Jugador *player, Enemigo *enemigo) {
    limpiarPantalla();

    Inventario *loot = &enemigo->item;

    // Crear lista con los 7 ítems fijos
    ItemAux items_fijos[7];
    int total_fijos = 0;

    // Copiar arma
    Arma *arma = malloc(sizeof(Arma));
    *arma = loot->armas;
    items_fijos[total_fijos].tipo = 0;
    items_fijos[total_fijos++].ptr = arma;

    // Copiar armaduras
    Armadura *casco = malloc(sizeof(Armadura)); 
    *casco = loot->casco;
    items_fijos[total_fijos].tipo = 1;
    items_fijos[total_fijos++].ptr = casco;

    Armadura *guantes = malloc(sizeof(Armadura)); 
    *guantes = loot->guantes;
    items_fijos[total_fijos].tipo = 1;
    items_fijos[total_fijos++].ptr = guantes;

    Armadura *pechera = malloc(sizeof(Armadura)); 
    *pechera = loot->pechera;
    items_fijos[total_fijos].tipo = 1;
    items_fijos[total_fijos++].ptr = pechera;

    Armadura *pantalones = malloc(sizeof(Armadura)); 
    *pantalones = loot->pantalones;
    items_fijos[total_fijos].tipo = 1;
    items_fijos[total_fijos++].ptr = pantalones;

    Armadura *botas = malloc(sizeof(Armadura)); 
    *botas = loot->botas;
    items_fijos[total_fijos].tipo = 1;
    items_fijos[total_fijos++].ptr = botas;

    // Copiar poción (si existe)
    Pocion *p = NULL;
    if (loot->pocion != NULL)
        p = list_first(loot->pocion);
    if (p) {
        Pocion *pocion = malloc(sizeof(Pocion));
        *pocion = *p;
        items_fijos[total_fijos].tipo = 2;
        items_fijos[total_fijos++].ptr = pocion;
    }

    // Mezclar los ítems
    srand(time(NULL));
    for (int i = total_fijos - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        ItemAux tmp = items_fijos[i];
        items_fijos[i] = items_fijos[j];
        items_fijos[j] = tmp;
    }

    // Elegir cuántos mostrar (entre 1 y total_fijos)
    int cantidad_a_mostrar = (rand() % total_fijos) + 1;

    List *items_posibles = list_create();
    for (int i = 0; i < cantidad_a_mostrar; i++) {
        ItemAux *nuevo = malloc(sizeof(ItemAux));
        *nuevo = items_fijos[i];
        list_pushBack(items_posibles, nuevo);
    }

    while (1) {
        if (list_first(items_posibles) == NULL){
            puts("HAS RECOGIDO TODOS LOS OBJETOS");
            break;
        }

        puts("=== OBJETOS DEL ENEMIGO ===");

        int index = 1;
        void *objeto = list_first(items_posibles);
        List *temp = list_create();
        
        while (objeto != NULL) {
            ItemAux *item = (ItemAux *)objeto;
            list_pushBack(temp, item);

            if (item->tipo == 0) {
                Arma *a = (Arma *)item->ptr;
                printf("%d) [ARMA] %s - Ataque: %d - Durabilidad: %d\n", index, a->nombre, a->ataque, a->durabilidad);
            } else if (item->tipo == 1) {
                Armadura *arm = (Armadura *)item->ptr;
                printf("%d) [ARMADURA] %s %s - Defensa: %d - Durabilidad: %d\n", index, arm->tipo, arm->nombre, arm->defensa, arm->durabilidad);
            } else if (item->tipo == 2) {
                Pocion *poc = (Pocion *)item->ptr;
                printf("%d) [POCION] %s - %s (%d)\n", index, poc->nombre, poc->efecto, poc->valor);
            }
            index++;
            objeto = list_next(items_posibles);
        }

        printf("%d) TOMAR TODO\n", index);
        printf("%d) CANCELAR\n", index + 1);
        printf("\nSELECCIONE UNA OPCION: ");
        
        int opcion;
        scanf("%1d", &opcion);
        while (getchar() != '\n');

        limpiarPantalla();
        if (opcion == index + 1 || opcion <= 0) {
            puts("CANCELANDO O OPCION INVALIDA");
            break;
        }

        if (opcion == index) {
            for (ItemAux *elemento = list_first(items_posibles); elemento != NULL; elemento = list_next(items_posibles)) {
                guardar_item(&player->inventario, elemento->ptr, elemento->tipo);
                free(elemento->ptr);
                free(elemento);
            }
            puts("HAS RECOGIDO TODOS LOS OBJETOS");
            break;
        } else {
            int actual = 1;
            ItemAux *item = list_first(temp);
            while (item != NULL && actual < opcion) {
                item = list_next(temp);
                actual++;
            }

            if (item != NULL) {
                guardar_item(&player->inventario, item->ptr, item->tipo);

                // ...dentro de recoger_items_enemigo, al tomar un ítem individual...
                List *nueva = list_create();
                for (ItemAux *it = list_first(items_posibles); it != NULL; it = list_next(items_posibles)) {
                    if (it != item) list_pushBack(nueva, it);
                    else {
                        free(it->ptr);
                        free(it);
                    }
                }
                list_free(items_posibles); // Libera la lista anterior completamente
                items_posibles = nueva;    // Usa la nueva lista

                puts("Objeto recogido con exito.");
            }
        }
    }
}

// Funcion para usar pociones
bool usarPociones(Jugador * player){
    limpiarPantalla();

    // Mostrar un mensaje en caso de no tener pociones
    if (list_size(player->inventario.pocion) == 0){
        puts("No tienes pociones en el inventario.");
        return true;
    }

    // Mostrar las pociones disponibles
    puts("=== POCIONES DISPONIBLES ===");
    int index = 1;
    Pocion *p_display = list_first(player->inventario.pocion);
    while (p_display != NULL) {
        printf("%d) %s - %s (+%d)\n", index, p_display->nombre, p_display->efecto, p_display->valor);
        p_display = list_next(player->inventario.pocion);
        index++;
    }
    printf("%d) Cancelar\n", index);

    // Seleccionar la pocion a utillizar
    printf("Seleccione la opcion a usar: ");
    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n');

    // Mostrar mensaje en caso de no usar ninguna pocion
    if (opcion <= 0 || opcion > list_size(player->inventario.pocion)) {
        puts("No se uso ninguna pocion.");
        presioneTeclaParaContinuar();
        return false; // Retorna false si no se usó ninguna poción
    }

    // Mover `current` al nodo de la poción seleccionada
    Pocion *seleccionada = (Pocion *)list_first(player->inventario.pocion);
    for (int i = 1; i < opcion; i++) {
        seleccionada = (Pocion *)list_next(player->inventario.pocion);
    }
    if (!seleccionada) {
        puts("Error al seleccionar la pocion.");
        presioneTeclaParaContinuar();
        return false;
    }

    // Mostrar la pocion usada
    printf("Usaste la pocion: %s\n", seleccionada->nombre);

    // Mostrar la vida actualizada despues de usar la pocion, si corresponde
    if (strcmp(seleccionada->efecto, "Vida") == 0){
        player->vida += seleccionada->valor;
        if (player->vida > player->max_vida) player->vida = player->max_vida;
        printf("Tu vida actual es: %d", player->vida);
    }

    // Mostrar un mensaje mostrand que el jugador tiene inmunidad, si corresponde
    else if (strcmp(seleccionada->efecto, "Inmunidad") == 0){
        player->inmunidad = seleccionada->valor;
        puts("¡Ahora eres inmune temporalmente!");
    }

    // Mostar un mensaje de la defensa actualizada, si corresponde
    else if (strcmp(seleccionada->efecto, "Escudo") == 0){
        player->defensa += seleccionada->valor;
        printf("Tu defensa aumento a: %d\n", player->defensa);
    }

    // Mostrar un mensaje de la estamina actualizada, si corresponde
    else if (strcmp(seleccionada->efecto, "Estamina") == 0){
        player->estamina += seleccionada->valor;
        if (player->estamina > player->max_estamina) player->estamina = player->max_estamina;
        printf("Tu estamina actual es: %d\n", player->estamina);
    }

    // Mostrar un mensaje del ataque actualizado, si corresponde
    else if (strcmp(seleccionada->efecto, "Ataque") == 0){
        player->ataque += seleccionada->valor;
        printf("Tu ataque actual es: %d\n", player->ataque);
    }

    // Se quita de la lista de pociones la pocion utilizada
    Pocion * pocion_a_liberar = (Pocion *)list_popCurrent(player->inventario.pocion);
    free(pocion_a_liberar); // Libera la memoria de la poción usada

    presioneTeclaParaContinuar();

    // Mostrar un mensaje en caso de quedarse sin pociones
    if(list_size(player->inventario.pocion) == 0){
        puts("Te has quedado sin pociones");
        presioneTeclaParaContinuar();
        return false;
    }
    return false;
}

// Funcion para aplicar bufo al jugador
void aplicarBufo(Jugador * player, const char *bufo, int valor){

    if (strcmp(bufo, "Escudo extra") == 0) {
        player->defensa_total += valor; // Se agrega defensa
    } else if (strcmp(bufo, "Estamina") == 0) {
        player->estamina_total += valor; // Se agrega estamina
    } else if (strcmp(bufo, "Aumento de vida") == 0) {
        player->vida_total += valor; // Se agrega vida
    } else if (strcmp(bufo, "Ataque") == 0) {
        player->ataque_total += valor; // Se agrega ataque
    }
}

// Funcion para calcular las estadisticas del jugador
void calcularEstatsT(Jugador * player){
    player->ataque_total = player->ataque;
    if (strcmp(player->inventario.armas.nombre, "Sin arma") != 0){
        player->ataque_total += player->inventario.armas.ataque;
    }

    player->defensa_total = player->defensa;
    if (strcmp(player->inventario.casco.nombre, "Sin armadura") != 0){
        player->defensa_total += player->inventario.casco.defensa;
        aplicarBufo(player, player->inventario.casco.bufo, player->inventario.casco.valor);
    }
    if (strcmp(player->inventario.pechera.nombre, "Sin armadura") != 0){
        player->defensa_total += player->inventario.pechera.defensa;
        aplicarBufo(player, player->inventario.pechera.bufo, player->inventario.pechera.valor);
    }
    if (strcmp(player->inventario.guantes.nombre, "Sin armadura") != 0){
        player->defensa_total += player->inventario.guantes.defensa;
        aplicarBufo(player, player->inventario.guantes.bufo, player->inventario.guantes.valor);
    }
    if (strcmp(player->inventario.pantalones.nombre, "Sin armadura") != 0){
        player->defensa_total += player->inventario.pantalones.defensa;
        aplicarBufo(player, player->inventario.pantalones.bufo, player->inventario.pantalones.valor);
    }
    if (strcmp(player->inventario.botas.nombre, "Sin armadura") != 0){
        player->defensa_total += player->inventario.botas.defensa;
        aplicarBufo(player, player->inventario.botas.bufo, player->inventario.botas.valor);
    }
}

// Funcion para reiniciar el arma a cero
void reiniciarArma(Arma * arma){
    strcpy(arma->nombre, "Sin arma");
    arma->ataque = 0;
    arma->durabilidad = 0;
}

// Funcion para determinar ataque
bool realizarAtaque(Jugador *player, Enemigo *enemigo, bool especial) {
    // Aqui se determina si el ataque es uno normal o es especial
    int multiplicador = especial ? 2 : 1;
    int costo_estamina = especial ? -5 : 1; // -5 para restar 5 en especial, 1 para sumar 1 en normal

    // Se calcula el daño y se le resta al enemigo
    int dano = multiplicador * ((player->ataque_total * player->ataque_total) / (player->ataque_total + enemigo->defensa));
    enemigo->vida -= dano;
    
    // Se resta la estamina utilizada
    player->estamina += costo_estamina;


    if (strcmp(player->inventario.armas.nombre, "Sin arma") != 0){
        player->inventario.armas.durabilidad -= 1;
        if (player->inventario.armas.durabilidad == 0) {
            printf("Tu %s se ha rot\n", player->inventario.armas.nombre);
            reiniciarArma(&player->inventario.armas);
            presioneTeclaParaContinuar();
        }
    }

    if (enemigo->vida <= 0) {
        // Se muestra un mensaje al derrotar al enemigo
        puts("El enemigo ha sido derrotado!");

        // Se calcula la experiencia y el nivel
        player->experiencia+= enemigo->exp_dada;
        while(player->experiencia >= 100){
            player->experiencia -= 100;
            lvlup(player);
        }

        // Si el enemigo es el creeper, este explotara al morir causando daño y mostrando un mensaje por pantalla
        if(strcmp(enemigo->nombre, "Creeper") == 0)
        {
            player->vida -= 10;
            printf("El %s ha explotado!!. te ha quitado 10 de vida :(\n", enemigo->nombre);
            presioneTeclaParaContinuar();
        } 

        // Si se derrota al boss se procede a llamar a la funcion victoria para terminar el juego
        if (strcmp(enemigo->dificultad, "Boss") == 0) {
        victoria();
        }

        // Se llama a la funcion recoger_items_enemigo() para ver que elementos solto el mob y caules
        // recogeras el jugador
        recoger_items_enemigo(player, enemigo);
        return false;
    }
    return true;
}

// Funcion para reiniciar la armadura a cero
void reiniciarArmadura(Armadura * vacia){
    printf("Tu %s %s se ha roto\n", vacia->tipo, vacia->nombre);
    presioneTeclaParaContinuar();
    strcpy(vacia->nombre, "Sin armadura");
    strcpy(vacia->tipo, "");
    vacia->defensa = 0;
    vacia->durabilidad = 0;
    strcpy(vacia->bufo, "");
    vacia->valor = 0;
}

// Funcion para calcular la perdida de durabillidad de las armas y armaduras
void perdidaDurabilidad(Armadura * aux){
    if (strcmp(aux->nombre, "Sin armadura") != 0){
        aux->durabilidad -= 1;
        if (aux->durabilidad <= 0) reiniciarArmadura(aux);
    }
}

// Funcion para generar el ataque enemigo
void ataqueEnemigo(Jugador * player, Enemigo * enemigo){
    // Si se usa la inmunidad temporal el enemigo no puede atacar
    if(player->inmunidad) {
        puts("¡El enemigo no puede dañarte, estas inmune!");
        (player->inmunidad)--; // Desactiva la inmunidad después de un ataque
        return;
    }

    // Se calcula el daño del mob y se descuenta de la vida del jugador
    int dano = (int)((enemigo->ataque * enemigo->ataque) / (enemigo->ataque + player->defensa_total));
    player->vida -= dano;
    
    // Se calcula la perdida de durabilidad de los items del jugador
    perdidaDurabilidad(&player->inventario.casco);
    perdidaDurabilidad(&player->inventario.pechera);
    perdidaDurabilidad(&player->inventario.guantes);
    perdidaDurabilidad(&player->inventario.pantalones);
    perdidaDurabilidad(&player->inventario.botas);

    // Mostrar por pantalla el daño recibido por el mob
    printf("El enemigo %s te ha atacado y te ha hecho %d de daño.\n", enemigo->nombre, dano);

    // Mostrar mensaje en caso de ser derrotado por el mob
    if (player->vida <= 0) {
        puts("Has sido derrotado por el enemigo.");
    }
}

// Funcion para determinar si el jugador huye o no
bool huida(Jugador *player, Enemigo *enemigo)
{
    // Mostrar mensaje de que no se puede huir de la batalla contra el boss
    if(strcmp(enemigo->dificultad, "Boss") == 0) {
        puts("No puedes huir de un Boss!");
        return false; // No se puede huir de un Boss
    }

    int prob_base = 30; // Probabilidad base de escapar de un combate

    // Modificadores según estado del jugador y dificultad del enemigo
    int mod_estamina = player->estamina - 10; // +1% por cada punto sobre 10
    int mod_vida = player->vida - enemigo->ataque; // +1% por cada punto de vida sobre el ataque enemigo

    int mod_dificultad = 0;
    if (strcmp(enemigo->dificultad, "Dificil") == 0) mod_dificultad = -15;
    else if (strcmp(enemigo->dificultad, "Media") == 0) mod_dificultad = 10;
    else if (strcmp(enemigo->dificultad, "Facil") == 0) mod_dificultad = -5;

    // Se calcula la probabilidad final
    int prob_final = prob_base + mod_estamina / 2 + mod_vida / 2 + mod_dificultad;

    // Limitar la probabilidad entre 5% y 90%
    if (prob_final < 0) prob_final = 0;
    if (prob_final > 100) prob_final = 100;

    int index = rand() % 100;

    // Si el jugador logra huir se muestrar un mensaje de escape
    if (index < prob_final) {
        puts("¡Has logrado huir de la pelea!");
        player->estamina -= 5; // El jugador pierde estamina al intentar huir
        if (player->estamina < 0) player->estamina = 0; // Asegura que la estamina no sea negativa
        return true; // El jugador ha huido exitosamente
    }

    // Mostrar mensaje en caso de no poder huir de la pelea
    puts("No has logrado huir de la pelea, el enemigo te ataca!");
    return false;
}

// Funcion para crear el ciclo de pelea
bool cicloPelea(Jugador * player, List * enemigos)
{
    Enemigo * enemigo = seleccionarEnemigo(enemigos); // Selecciona un enemigo de la lista proporcionada

    //calcularDefensaActual(player);
    if (enemigo == NULL) {
        printf("No se encontro ningun enemigo para pelear en esta zona.\n");
        presioneTeclaParaContinuar(); // Pausa para que el usuario lea el mensaje
        return true; // El jugador sigue vivo, pero no hubo pelea.
    }

    bool EnemigoVivo = true; // Variable para controlar si el enemigo está activo
    while(EnemigoVivo && player->vida > 0) {
        limpiarPantalla();
        
        calcularEstatsT(player);
        
        printf("Jugador: %s | Vida: %d | Estamina: %d | Ataque: %d | Defensa: %d\n",
            player->nombre, player->vida, player->estamina, player->ataque_total, player->defensa_total);
        printf("Enemigo: %s | Vida: %d | Defensa: %d | Dificultad : %s\n",
            enemigo->nombre, enemigo->vida, enemigo->defensa, enemigo->dificultad);
        
        menuOpcionesPelea(); // Muestra el menú de opciones de pelea
        bool repetirAccion = false; // Variable para controlar si se repite la acción
        
        // Se le pide una opcion al usuario
        char opcion;
        printf("INGRESE SU OPCION: ");
        scanf(" %c", &opcion);
        while (getchar() != '\n');

        switch (opcion)
        {
        case '1':
            EnemigoVivo = realizarAtaque(player, enemigo, NORMAL);
            break;
        case '2':
            if (player->estamina >= 5) EnemigoVivo = realizarAtaque(player, enemigo, ESPECIAL);
            else puts("No cuentas con suficiente estamina para hacer un ataque especial\n");
            break;
        case '3':
            repetirAccion = usarPociones(player); // El jugador usa una poción
            break;
        case '4':
            if(huida(player, enemigo)) return true;
            break;
        default:
            puts("Opcion no valida, por favor intente de nuevo.");
            repetirAccion = true; // Repite la acción si la opción no es válida
            break;
        }
        if (repetirAccion) continue; // Si se repite la acción, vuelve al inicio del bucle
        if(EnemigoVivo) ataqueEnemigo(player, enemigo); // El enemigo ataca al jugador
    }
    

    reiniciarEstasBase(player);
    calcularEstatsT(player);

    // Mostar un mensaje al perder y cambiar el estado a muerto
    if (player->vida <= 0) {
        puts("El jugador ha sido derrotado!");
        return false; // El jugador ha muerto
    }
    return true;
}

/**********************************************/
/*             Seleccionar opcion             */
/**********************************************/

void seleccionOpcion(Jugador * player)
{
    bool jugadorVivo = true; // Variable para controlar si el jugador está activo
    char op;
    while(jugadorVivo)
    {
        //Se muestra el menú y se pide una opción
        mostrarMenuJuego();
        printf("INGRESE SU OPCION: ");
        scanf(" %c", &op);
        while (getchar() != '\n');

        //Se realizan las acciones según la opción seleccionada
        switch (op) {
            case '1':
                //explorarZonas(); //FUNCIÓN PARA EXPLORAR LAS ZONAS
                jugadorVivo = movermeDeEscenario(player);
                break;
            case '2':
                verEstado(player); //FUNCIÓN PARA VER EL ESTADO E INVENTARIO DEL JUGADOR
                break;
            case '3':
                jugadorVivo = cicloPelea(player, player->actual->Enemigos); // Ciclo de pelea con los enemigos de la zona actual
                break;
            case '4':
                //equipar_DesequiparItem(); //FUNCIÓN PARA EQUIPAR Y DESEQUIPAR ITEMS
                return;
            default:
                printf("OPCION NO VALIDA.\n");
                break;
        }
        presioneTeclaParaContinuar();
    } // El bucle continuará mientras el jugador esté activo
}

// Funcion para seleccionar una opcion en el menu de ayuda
void seleccionOpcionAyuda()
{
    char op;
    int cantidad;
    while(1)
    {
        //Se muestra el menú de ayuda y se pide una opción
        mostrarMenuAyuda();
        printf("INGRESE SU OPCION: ");
        scanf(" %c", &op);
        while (getchar() != '\n');

        //Se realizan las acciones según la opción seleccionada
        switch (op) {
            case '1': // SOBRE LOS MOVIMIENTO
                limpiarPantalla();

                puts("========================================");
                puts("              MOVIMIENTOS                ");
                puts("========================================");
                
                const char* movimiento[] = {
                    "Para avanzar en tu aventura, selecciona la opcion (EXPLORAR ZONAS) desde el menu de juego.",
                    "Dependiendo de las zonas disponibles, podras moverte por el mapa utilizando las teclas:",
                    "    W - Arriba", "    A - Izquierda", "    S - Abajo", "    D - Derecha",
                    "Cada zona oculta secretos, enemigos o tesoros. Piensa bien cada paso...",
                    "en Umbrahal, el peligro acecha en cada rincon.", ""
                };

                cantidad = sizeof(movimiento) / sizeof(movimiento[0]);

                for (int i = 0; i < cantidad; i++) {
                    printf("%s\n", movimiento[i]);
                    esperar(1);         // Espera 1 segundos
                }
                break;
            case '2': // SOBRE LOS ENEMIGOS
                limpiarPantalla();
                puts("========================================");
                puts("                ENEMIGOS                ");
                puts("========================================");

                const char* enemigos[] = {
                    "En las profundidades de Umbrahal, te enfrentaras a criaturas del inframundo que buscan detenerte.",
                    "Estos enemigos varian en dificultad: FACIL, MEDIO, DIFICIL y el temible BOSS.", "",
                    "A medida que avances por los pisos de la masmorra, los enemigos se volveran mas agresivos y letales.",
                    "Cada encuentro será un reto, y solo los más preparados sobrevivirán. Los enemigos aparecen de forma",
                    "aleatoria, y su dificultad dependera del area en la que te encuentres.", "",
                    "Derrotarlos no solo sera necesario para sobrevivir, tambien puede recompensarte con items valiosos",
                    "que podrian cambiar el curso de tu destino.", ""
                };

                cantidad = sizeof(enemigos) / sizeof(enemigos[0]);

                for (int i = 0; i < cantidad; i++) {
                    printf("%s\n", enemigos[i]);
                    esperar(1);         // Espera 1 segundos
                }
                break;
            case '3': // SOBRE EL EQUIPAMENTO
                limpiarPantalla();
                puts("========================================");
                puts("                EQUIPAMENTO             ");
                puts("========================================");
                
                const char* equip[] = {
                    "En tu descenso por la Masmorra de Umbrahal, encontraras items esenciales que marcaran la diferencia",
                    "entre la vida y la muerte.", "",
                    "Las armaduras te otorgan proteccion contra los ataques del inframundo, reduciendo el daño recibido.",
                    "Las pociones restauran salud o brindan efectos temporales como inmunidad o estamina.", "",
                    "Explora bien cada escenario. El equipamiento correcto puede ser la clave para sobrevivir... o caer.", ""
                };

                cantidad = sizeof(equip) / sizeof(equip[0]);

                for (int i = 0; i < cantidad; i++) {
                    printf("%s\n", equip[i]);
                    esperar(1);         // Espera 1 segundos
                }
                break;
            case '4': // Volver al menu principal
                limpiarPantalla();
                printf("VOLVIENDO AL MENU PRINCIPAL.\n");
                return;
            default: // Opcion no valida
                limpiarPantalla();
                printf("OPCION NO VALIDA.\n");
                break;
        }
        presioneTeclaParaContinuar();
    } // El bucle continuará mientras el jugador esté activo
}

// Funcion para hacer la subida de nivel
void lvlup(Jugador *jugador) {
    jugador->nivel++; // Sube un nivel
    jugador->max_vida += 5; // Incrementa la vida al subir de nivel
    jugador->max_ataque += 2; // Incrementa el ataque al subir de nivel
    jugador->max_defensa += 1; // Incrementa la defensa al subir de nivel
    printf("¡Felicidades! Has subido al nivel %d.\n", jugador->nivel); // Mensaje al subir de nivel
}

// Funcion para hacer el movimiento entre escenarios
bool movermeDeEscenario(Jugador *jugador)
{
    char direccion;
    int movimiento = 0;
    bool jugadorVivo = true;

    do {
        limpiarPantalla();
        // Si el jugador esta muerto no se puede hacer movimiento por el mapa
        if (!jugadorVivo) break;

        // Mostrar mensaje de donde esta el jugador y hacia a donde sepuede mover
        printf("Estas en: %s\n", jugador->actual->nombre);
        printf("A donde deseas moverte?\n");
        if (jugador->actual->arriba)
            printf("- Arriba (w): %s\n", jugador->actual->arriba->nombre);
        if (jugador->actual->abajo)
            printf("- Abajo (s): %s\n", jugador->actual->abajo->nombre);
        if (jugador->actual->izquierda)
            printf("- Izquierda (a): %s\n", jugador->actual->izquierda->nombre);
        if (jugador->actual->derecha)
            printf("- Derecha (d): %s\n", jugador->actual->derecha->nombre);
        printf("\nSolo se muestran las direcciones disponibles.\n");

        // Ingresar la opcion de donde el usuario quiere moverse
        printf("Ingrese direccion (w/a/s/d): ");
        scanf(" %c", &direccion);
        while (getchar() != '\n');

        // Segun la opcion selecionada se realizael movimieto
        switch (direccion) {
            case 'w':
            case 'W':
                // Hacia arriba
                if (jugador->actual->arriba) {
                    jugador->actual = jugador->actual->arriba;
                    movimiento = 1;
                    jugadorVivo = cicloPelea(jugador, jugador->actual->Enemigos);
                } else {
                    printf("No puedes moverte en esa direccion.\n"); // Mensaje en caso de que realizar este movimiento no este disponible
                }
                break;
            case 'a':
            case 'A':
                // Hacia la izquierda
                if (jugador->actual->izquierda) {
                    jugador->actual = jugador->actual->izquierda;
                    movimiento = 1;
                    jugadorVivo = cicloPelea(jugador, jugador->actual->Enemigos);
                } else {
                    printf("No puedes moverte en esa direccion.\n"); // Mensaje en caso de que realizar este movimiento no este disponible
                }
                break;
            case 's':
            case 'S':
                // Hacia abajo
                if (jugador->actual->abajo) {
                    jugador->actual = jugador->actual->abajo;
                    movimiento = 1;
                    jugadorVivo = cicloPelea(jugador, jugador->actual->Enemigos);
                } else {
                    printf("No puedes moverte en esa direccion.\n"); // Mensaje en caso de que realizar este movimiento no este disponible
                }
                break;
            case 'd':
            case 'D':
                // Hacia la derecha
                if (jugador->actual->derecha) {
                    jugador->actual = jugador->actual->derecha;
                    movimiento = 1;
                    jugadorVivo = cicloPelea(jugador, jugador->actual->Enemigos);
                } else {
                    printf("No puedes moverte en esa direccion.\n"); // Mensaje en caso de que realizar este movimiento no este disponible
                }
                break;
            default:
                // Mensaje en caso de que se ingrese una direccion no disponible
                printf("Direccion invalida. Intenta nuevamente.\n");
                break;
        }

        if (!movimiento) {
            presioneTeclaParaContinuar();
        }

    } while (!movimiento);

    if (!jugadorVivo) return false;

    // Mostrar hacia a donde se movio el jugador
    limpiarPantalla();
    puts("========================================");
    puts("               FALTA XP");
    puts("========================================");
    printf("Te has movido a: %s\n", jugador->actual->nombre);
    printf("%s\n\n", jugador->actual->leyenda);
}

/**********************************************/
/*        Ver estadisticas del jugador        */
/**********************************************/

void verEstado(Jugador *player)
{
    int n = 0;

    // Se muestran las estadisticas del jugador
    limpiarPantalla();
    puts("========================================");
    puts("           ESTADO DEL JUGADOR           ");
    puts("========================================");
    printf("Jugador: %s | Vida: %d | Estamina: %d | Ataque: %d | Defensa: %d | Nivel: %d\n",
            player->nombre, player->vida, player->estamina, player->ataque_total, player->defensa_total, 
            player->nivel);
    printf("Te encuentras en: %s\n", player->actual->nombre);
    presioneTeclaParaContinuar();
    limpiarPantalla();

    // Se muestran el inventario del jugador
    if(strcmp(player->inventario.armas.nombre, "Sin arma") == 1) printf("[ARMA]: %s\n\n", player->inventario.armas.nombre);
    if(strcmp(player->inventario.casco.nombre, "Sin armadura") == 1) printf("[ARMADURA]: Casco %s\n", player->inventario.casco.nombre);
    if(strcmp(player->inventario.pechera.nombre, "Sin armadura") == 1) printf("[ARMADURA]: Pechera %s\n", player->inventario.pechera.nombre);
    if(strcmp(player->inventario.guantes.nombre, "Sin armadura") == 1) printf("[ARMADURA]: Guantes %s\n", player->inventario.guantes.nombre);
    if(strcmp(player->inventario.pantalones.nombre, "Sin armadura") == 1) printf("[ARMADURA]: Pantalon %s\n", player->inventario.pantalones.nombre);
    if(strcmp(player->inventario.botas.nombre, "Sin armadura") == 1) printf("[ARMADURA]: Botas %s\n\n", player->inventario.botas.nombre);
    if(list_first(player->inventario.pocion) != NULL)
    {
        printf("[POCIONES]:\n");
        void *dato = list_first(player->inventario.pocion);
        int i = 1;
        while (dato != NULL) {
            Pocion *p = (Pocion *)dato;
            printf("%d) %s | Efecto: %s | Valor: %d\n", i++, p->nombre, p->efecto, p->valor);
            dato = list_next(player->inventario.pocion);
        }
    }
    
}

/**********************************************/
/*             Mensaje de victoria            */
/**********************************************/

void victoria()
{
    limpiarPantalla();
    puts("========================================");
    puts("               VICTORIA                 ");
    puts("========================================");
    puts("¡Felicidades! Has derrotado al Rey Demonio y has completado el juego.");
    puts("Gracias por jugar a Falta Xp.");
    puts("========================================");
    presioneTeclaParaContinuar();
    mostrarCreditos();
    presioneTeclaParaContinuar();

    // Se libera la memoria del juego
    liberarMemoria(g_player, g_juego, g_mobs, g_facil, g_medio, g_dificil);
    exit(0);
    
}

/**********************************************/
/*              Liberar memoria               */
/**********************************************/

void liberarMemoria(Jugador *player,HashMap *juego,HashMap *mobs,List *facil,List *medio,List *dificil)
{
    // Se libera la memoria del inventario del jugador
    list_clean(player->inventario.pocion);
    list_free(player->inventario.pocion);
    free(player);

    // Liberamos cada Escenarios* guardado en el map
    for (Pair *par = firstMap(juego); par != NULL; par = nextMap(juego)) {
        free(par->value);  // cada Escenarios* fue malloc()
    }
    // Limpiamos y destruimos el HashMap
    cleanMap(juego); // libera internamente cada Pair
    free(juego);

    for(Enemigo *n = list_first(facil); n != NULL; n = list_next(facil)) {
        list_free(n->item.pocion);
        free(n);
    }
    for(Enemigo *n = list_first(medio); n != NULL; n = list_next(medio)) {
        list_free(n->item.pocion);
        free(n);
    }
    for(Enemigo *n = list_first(dificil); n != NULL; n = list_next(dificil)) {
        list_free(n->item.pocion);
        free(n);
    }
    // Se libera la memoria de las listas de dificultad (compartidas entre escenarios)
    list_free(facil);
    list_free(medio);
    list_free(dificil);

    cleanMap(mobs);
    free(mobs);
}