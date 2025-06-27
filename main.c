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

//Estructura para el jugador
typedef struct{
    char nombre[50];
    int vida;
    int max_vida;
    int vida_total;

    int estamina;
    int max_estamina;
    int estamina_total;

    int defensa;
    int max_defensa;
    int defensa_total;

    int ataque;
    int ataque_total;

    int experiencia;
    int nivel;

    Escenarios *actual;
    Inventario inventario;
    int inmunidad;
    bool ReydemonioDerrotado;
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

static Jugador *g_player   = NULL;
static HashMap *g_juego    = NULL;
static HashMap *g_mobs     = NULL;
static List *g_facil       = NULL;
static List *g_medio       = NULL;
static List *g_dificil     = NULL;

/**********************************************/
/*                    Main                    */
/**********************************************/

int main(){
    g_juego   = createMap(100);
    g_mobs    = createMap(100);
    if (!g_juego) {
        fprintf(stderr, "Error al crear el HashMap\n");
        return 1;
    }
    srand(time(NULL));

    leer_escenarios(g_juego);
    leer_mobs(g_mobs);
    asignar_mobs(g_juego, g_mobs);

    char op;
    char name[50];
    do{
        mostrarMenuPrincipal();
        printf("INGRESE SU OPCION: ");
        scanf(" %c", &op);
        
        switch (op) {
        case '1':
            printf("INDIQUE EL NOMBRE DEL NUEVO JUGADOR: ");
            scanf(" %49s", name);
            getchar();
            g_player = createPlayer(name, g_juego);
            if (g_player) seleccionOpcion(g_player);
            else puts("No se pudo crear el jugador. Revise los datos de escenarios.");
            break;
        case '2':
            seleccionOpcionAyuda();
            break;
        case '3':
            mostrarCreditos();
            break;
        case '4':
            puts("SALIENDO DEL JUEGO");
            liberarMemoria(g_player, g_juego, g_mobs, g_facil, g_medio, g_dificil);
            break;
        default:
            puts("OPCION NO VALIDA");
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
    const char* creditos[] = {
        "Falta Creatividad Studios Presenta:",
        "Falta XP",
        "",
        "Director: Eduardo Sandoval",
        "Productor: Joaquin Contreras",
        "Gestor de Calidad: Brandon Caceres",
        "Responsable de Integracion: Josue Huaiquil",
        "Coordinador: Eduardo Sandoval"
        "Creacion de Enemigos: Brandon Caceres",
        "Interface Designer: Josue Huaiquil", "",
        "Equipo de Programacion:",
        "Eduardo Sandoval",
        "Joaquin Contreras",
        "Brandon Caceres",
        "Josue Huaiquil", "",
        "Gracias por jugar Falta XP!"
    };

    int cantidad = sizeof(creditos) / sizeof(creditos[0]);

    for (int i = 0; i < cantidad; i++) {
        printf("%s\n", creditos[i]);
        esperar(1);         // Espera 1 segundos
    }
}

/**********************************************/
/*               Mostrar menus                */
/**********************************************/

void mostrarMenuAyuda() {
    limpiarPantalla();
    puts("========================================");
    puts("              MENU DE AYUDA                  ");
    puts("========================================");
  
    puts("1) SOBRE LOS MOVIMIENTOS");
    puts("2) SOBRE LOS ENEMIGOS");
    puts("3) SOBRE EL EQUIPAMENTO");
    puts("4) VOLVER AL MENU PRINCIPAL");
}

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
        strncpy(escenario->id, campos[0], sizeof(escenario->id));
        strncpy(escenario->nombre, campos[1], sizeof(escenario->nombre));
        strncpy(escenario->leyenda, campos[2], sizeof(escenario->leyenda));

        strcpy(escenario->id_izquierda, campos[3]);
        strcpy(escenario->id_derecha, campos[4]);
        strcpy(escenario->id_arriba, campos[5]);
        strcpy(escenario->id_abajo, campos[6]);

        strncpy(escenario->dificultad, campos[7], sizeof(escenario->dificultad));

        insertMap(juego, escenario->id, escenario);

    }
    fclose(archivo);

    // Segunda pasada: establecer los punteros de dirección entre escenarios
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

    list_clean(claves);
    free(claves);
}

void leer_mobs(HashMap *mobs) {

    List * facil = list_create();
    List * media = list_create();
    List * dificil = list_create();


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

        if (strcmp(e->dificultad, "Facil") == 0) list_pushBack(facil, e);
        if (strcmp(e->dificultad, "Media") == 0) list_pushBack(media, e);
        if (strcmp(e->dificultad, "Dificil") == 0) list_pushBack(dificil, e);
        if (strcmp(e->dificultad, "Boss") == 0) insertMap(mobs, "Boss", e);
        // Insertar en el HashMap
        
    }

    g_facil = facil;
    g_medio = media;
    g_dificil = dificil;

    insertMap(mobs, "facil", facil);
    insertMap(mobs, "media", media);
    insertMap(mobs, "dificil", dificil);

    fclose(archivo);
}

/**********************************************/
/*          Asignar mobs a escenarios         */
/**********************************************/

void asignar_mobs(HashMap * escenarios, HashMap * mobs){
    for (Pair * esc = firstMap(escenarios); esc != NULL; esc = nextMap(escenarios)){
        Escenarios * escenario = esc->value;

        escenario->Enemigos = list_create();

        if (strcmp(escenario->dificultad, "Facil") == 0) escenario->Enemigos = searchMap(mobs, "facil")->value;
        if (strcmp(escenario->dificultad, "Media") == 0) escenario->Enemigos = searchMap(mobs, "media")->value;
        if (strcmp(escenario->dificultad, "Dificil") == 0) escenario->Enemigos = searchMap(mobs, "dificil")->value;
        if (strcmp(escenario->dificultad, "Boss") == 0) escenario->Enemigos = searchMap(mobs, "Boss")->value;
    }
}

void copiaArmadura(Armadura * seleccionado, Armadura * item){
    strcpy(seleccionado->tipo, item->tipo);
    strcpy(seleccionado->nombre, item->nombre);
    seleccionado->defensa = item->defensa;
    seleccionado->durabilidad = item->durabilidad;
    strcpy(seleccionado->bufo, item->bufo);
    seleccionado->valor = item->valor;
}

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
    Jugador * player = (Jugador *)malloc(sizeof(Jugador));
    if (player == NULL) exit(1);

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
    player->max_defensa = 100;
    player->defensa_total = 0;
    player->ataque = 6;
    player->ataque_total = 6;
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

/**********************************************/
/*               Ciclo de Pelea               */
/**********************************************/

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
            printf("Arma actual: %s (Ataque: %d, Durabilidad: %d)\n", inv->armas.nombre, inv->armas.ataque, inv->armas.durabilidad);
            printf("Nueva arma:  %s (Ataque: %d, Durabilidad: %d)\n", arma->nombre, arma->ataque, arma->durabilidad);
            printf("Deseas reemplazar tu arma actual? (1 = SI, 0 = NO): ");
            int decision;
            scanf("%d", &decision);
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
            printf("Deseas reemplazar esta armadura? (1 = SI, 0 = NO): ");
            int decision;
            scanf("%d", &decision);
            if (decision == 1) {
                *actual = *armadura;
                printf("Has reemplazado la armadura con exito.\n");
            } else {
                printf("No se reemplazo la armadura.\n");
            }
        }
    }
}

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
        scanf("%d", &opcion);

        if (opcion == index + 1 || opcion <= 0) {
            puts("CANCELANDO O OPCION INVALIDA");
            //presioneTeclaParaContinuar();
            break;
        }

        if (opcion == index) {
            for (ItemAux *elemento = list_first(items_posibles); elemento != NULL; elemento = list_next(items_posibles)) {
                guardar_item(&player->inventario, elemento->ptr, elemento->tipo);
                free(elemento->ptr);
                free(elemento);
            }
            puts("HAS RECOGIDO TODOS LOS OBJETOS");
            //presioneTeclaParaContinuar();
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
                //presioneTeclaParaContinuar();
            }
        }
    }
}

bool usarPociones(Jugador * player){
    limpiarPantalla();
    if (list_size(player->inventario.pocion) == 0){
        puts("No tienes pociones en el inventario.");
        return true;
    }
    puts("=== POCIONES DISPONIBLES ===");

    int index = 1;
    Pocion *p_display = list_first(player->inventario.pocion);
    while (p_display != NULL) {
        printf("%d) %s - %s (+%d)\n", index, p_display->nombre, p_display->efecto, p_display->valor);
        p_display = list_next(player->inventario.pocion);
        index++;
    }
    
    printf("%d) Cancelar\n", index);
    printf("Seleccione la opcion a usar: ");
    int opcion;
    scanf("%d", &opcion);

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

    printf("Usaste la pocion: %s\n", seleccionada->nombre);
    if (strcmp(seleccionada->efecto, "Vida") == 0){
        player->vida += seleccionada->valor;
        if (player->vida > player->max_vida) player->vida = player->max_vida;
        printf("Tu vida actual es: %d", player->vida);
    }
    else if (strcmp(seleccionada->efecto, "Inmunidad") == 0){
        player->inmunidad = seleccionada->valor;
        puts("¡Ahora eres inmune temporalmente!");
    }
    else if (strcmp(seleccionada->efecto, "Escudo") == 0){
        player->defensa += seleccionada->valor;
        if (player->defensa > player->max_defensa) player->defensa = player->max_defensa;
        printf("Tu defensa aumento a: %d\n", player->defensa);
    }
    else if (strcmp(seleccionada->efecto, "Estamina") == 0){
        player->estamina += seleccionada->valor;
        if (player->estamina > player->max_estamina) player->estamina = player->max_estamina;
        printf("Tu estamina actual es: %d\n", player->estamina);
    }

    Pocion * pocion_a_liberar = (Pocion *)list_popCurrent(player->inventario.pocion);
    free(pocion_a_liberar); // Libera la memoria de la poción usada

    presioneTeclaParaContinuar();

    if(list_size(player->inventario.pocion) == 0){
        puts("Te has quedado sin pociones");
        presioneTeclaParaContinuar();
        return false;
    }
    return false;
}

void aplicarBufo(Jugador * player, const char *bufo, int valor){
    if (strcmp(bufo, "Escudo extra") == 0) {
        player->defensa_total += valor;
    } else if (strcmp(bufo, "Estamina") == 0) {
        player->estamina_total += valor;
    } else if (strcmp(bufo, "Aumento de vida") == 0) {
        player->vida_total += valor;
    } else if (strcmp(bufo, "Ataque") == 0) {
        player->ataque_total += valor;
    }
}

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

void reiniciarArma(Arma * arma){
    strcpy(arma->nombre, "Sin arma");
    arma->ataque = 0;
    arma->durabilidad = 0;
}

bool realizarAtaque(Jugador *player, Enemigo *enemigo, bool especial) {
    int multiplicador = especial ? 2 : 1;
    int costo_estamina = especial ? -5 : 1; // -5 para restar 5 en especial, 1 para sumar 1 en normal

    int dano = multiplicador * ((player->ataque_total * player->ataque_total) / (player->ataque_total + enemigo->defensa));
    enemigo->vida -= dano;
    player->inventario.armas.durabilidad -= 1;
    player->estamina += costo_estamina;

    if (player->inventario.armas.durabilidad == 0) {
        printf("Tu %s se ha roto", player->inventario.armas.nombre);
        reiniciarArma(&player->inventario.armas);
    }
    if (enemigo->vida <= 0) {
        puts("El enemigo ha sido derrotado!");
        player->experiencia+= enemigo->exp_dada;

        if(strcmp(enemigo->nombre, "Creeper") == 0)
        {
            player->vida -= 10;
            printf("El %s ha explotado!!. te ha quitado 10 de vida :(\n", enemigo->nombre);
            presioneTeclaParaContinuar();
        } 
        if (strcmp(enemigo->dificultad, "Boss") == 0) {
        victoria();
        }
        recoger_items_enemigo(player, enemigo);
        return false;
    }
    return true;
}

void reiniciarArmadura(Armadura * vacia){
    printf("Tu %s se ha roto", vacia->nombre);
    strcpy(vacia->nombre, "Sin armadura");
    strcpy(vacia->tipo, "");
    vacia->defensa = 0;
    vacia->durabilidad = 0;
    strcpy(vacia->bufo, "");
    vacia->valor = 0;
}

void perdidaDurabilidad(Armadura * aux){
    if (strcmp(aux->nombre, "Sin armadura") != 0){
        aux->durabilidad -= 1;
        if (aux->durabilidad <= 0) reiniciarArmadura(aux);
    }
}

void ataqueEnemigo(Jugador * player, Enemigo * enemigo){
    
    if(player->inmunidad) {
        puts("¡El enemigo no puede dañarte, estas inmune!");
        (player->inmunidad)--; // Desactiva la inmunidad después de un ataque
        return;
    }

    int dano = (int)((enemigo->ataque * enemigo->ataque) / (enemigo->ataque + player->defensa_total));
    player->vida -= dano;
    
    perdidaDurabilidad(&player->inventario.casco);
    perdidaDurabilidad(&player->inventario.pechera);
    perdidaDurabilidad(&player->inventario.guantes);
    perdidaDurabilidad(&player->inventario.pantalones);
    perdidaDurabilidad(&player->inventario.botas);

    printf("El enemigo %s te ha atacado y te ha hecho %d de daño.\n", enemigo->nombre, dano);
    if (player->vida <= 0) {
        puts("Has sido derrotado por el enemigo.");
    }
}

bool huida(Jugador *player, Enemigo *enemigo)
{
    if(strcmp(enemigo->dificultad, "Boss") == 0) {
        puts("No puedes huir de un Boss!");
        return false; // No se puede huir de un Boss
    }
    int prob_base = 30;

    // Modificadores según estado del jugador y dificultad del enemigo
    int mod_estamina = player->estamina - 10; // +1% por cada punto sobre 10
    int mod_vida = player->vida - enemigo->ataque; // +1% por cada punto de vida sobre el ataque enemigo

    int mod_dificultad = 0;
    if (strcmp(enemigo->dificultad, "Dificil") == 0) mod_dificultad = -15;
    else if (strcmp(enemigo->dificultad, "Media") == 0) mod_dificultad = 10;
    else if (strcmp(enemigo->dificultad, "Facil") == 0) mod_dificultad = -5;

    int prob_final = prob_base + mod_estamina / 2 + mod_vida / 2 + mod_dificultad;

    // Limitar la probabilidad entre 5% y 90%
    if (prob_final < 0) prob_final = 0;
    if (prob_final > 100) prob_final = 100;

    int index = rand() % 100;
    if (index < prob_final) {
        puts("¡Has logrado huir de la pelea!");
        player->estamina -= 5; // El jugador pierde estamina al intentar huir
        if (player->estamina < 0) player->estamina = 0; // Asegura que la estamina no sea negativa
        return true; // El jugador ha huido exitosamente
    }
    puts("No has logrado huir de la pelea, el enemigo te ataca!");
    return false;
}

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
        
        char opcion;
        printf("INGRESE SU OPCION: ");
        scanf(" %c", &opcion);

        switch (opcion)
        {
        case '1':
            EnemigoVivo = realizarAtaque(player, enemigo, NORMAL);
            break;
        case '2':
            EnemigoVivo = realizarAtaque(player, enemigo, ESPECIAL);
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

        //Se realizan las acciones según la opción seleccionada
        switch (op) {
            case '1':
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
            case '2':
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
            case '3':
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
            case '4':
                limpiarPantalla();
                printf("VOLVIENDO AL MENU PRINCIPAL.\n");
                return;
            default:
                limpiarPantalla();
                printf("OPCION NO VALIDA.\n");
                break;
        }
        presioneTeclaParaContinuar();
    } // El bucle continuará mientras el jugador esté activo
}

void lvlup(Jugador *jugador) {
    jugador->nivel++;
    jugador->max_vida += 5; // Incrementa la vida al subir de nivel
    jugador->ataque += 2; // Incrementa el ataque al subir de nivel
    jugador->max_defensa += 1; // Incrementa la defensa al subir de nivel
    printf("¡Felicidades! Has subido al nivel %d.\n", jugador->nivel);
}

bool movermeDeEscenario(Jugador *jugador)
{
    char direccion;
    int movimiento = 0;
    bool jugadorVivo = true;

    do {
        limpiarPantalla();
        
        if (!jugadorVivo) break;

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
        printf("Ingrese direccion (w/a/s/d): ");
        scanf(" %c", &direccion);

        switch (direccion) {
            case 'w':
            case 'W':
                if (jugador->actual->arriba) {
                    jugador->actual = jugador->actual->arriba;
                    movimiento = 1;
                    jugadorVivo = cicloPelea(jugador, jugador->actual->Enemigos);
                } else {
                    printf("No puedes moverte en esa direccion.\n");
                }
                break;
            case 'a':
            case 'A':
                if (jugador->actual->izquierda) {
                    jugador->actual = jugador->actual->izquierda;
                    movimiento = 1;
                    jugadorVivo = cicloPelea(jugador, jugador->actual->Enemigos);
                } else {
                    printf("No puedes moverte en esa direccion.\n");
                }
                break;
            case 's':
            case 'S':
                if (jugador->actual->abajo) {
                    jugador->actual = jugador->actual->abajo;
                    movimiento = 1;
                    jugadorVivo = cicloPelea(jugador, jugador->actual->Enemigos);
                } else {
                    printf("No puedes moverte en esa direccion.\n");
                }
                break;
            case 'd':
            case 'D':
                if (jugador->actual->derecha) {
                    jugador->actual = jugador->actual->derecha;
                    movimiento = 1;
                    jugadorVivo = cicloPelea(jugador, jugador->actual->Enemigos);
                } else {
                    printf("No puedes moverte en esa direccion.\n");
                }
                break;
            default:
                printf("Direccion invalida. Intenta nuevamente.\n");
                break;
        }

        if (!movimiento) {
            presioneTeclaParaContinuar();
        }

    } while (!movimiento);
    limpiarPantalla();
    puts("========================================");
    puts("               FALTA XP");
    puts("========================================");
    printf("Te has movido a: %s\n\n", jugador->actual->nombre);
}

void verEstado(Jugador *player)
{
    int n = 0;
    limpiarPantalla();
    puts("========================================");
    puts("           ESTADO DEL JUGADOR           ");
    puts("========================================");
    printf("Jugador: %s | Vida: %d | Estamina: %d | Ataque: %d | Defensa: %d | Nivel: %d\n",
            player->nombre, player->vida, player->estamina, player->ataque_total, player->defensa_total, 
            player->nivel);
    printf("Te encuentras en: %s\n", player->actual->nombre);
    if(strcmp(player->inventario.armas.nombre, "Sin arma") == 1) printf("[ARMA]: %s\n", player->inventario.armas.nombre);
    if(strcmp(player->inventario.casco.nombre, "Sin armadura") == 1) printf("[ARMADURA]: Casco %s\n", player->inventario.casco.nombre);
    if(strcmp(player->inventario.pechera.nombre, "Sin armadura") == 1) printf("[ARMADURA]: Pechera %s\n", player->inventario.pechera.nombre);
    if(strcmp(player->inventario.guantes.nombre, "Sin armadura") == 1) printf("[ARMADURA]: Guantes %s\n", player->inventario.guantes.nombre);
    if(strcmp(player->inventario.pantalones.nombre, "Sin armadura") == 1) printf("[ARMADURA]: Pantalon %s\n\n", player->inventario.pantalones.nombre);
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
    liberarMemoria(g_player, g_juego, g_mobs, g_facil, g_medio, g_dificil);
    exit(0);
    
}

void liberarMemoria(Jugador *player,HashMap *juego,HashMap *mobs,List *facil,List *medio,List *dificil)
{
    // 1) Inventario del jugador
    list_clean(player->inventario.pocion);
    list_free(player->inventario.pocion);
    free(player);

    // 2) Escenarios
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
    // 4) Listas de dificultad (compartidas entre escenarios)
    list_free(facil);
    list_free(medio);
    list_free(dificil);

    cleanMap(mobs);
    free(mobs);
}