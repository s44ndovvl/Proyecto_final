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
    bool inmunidad;
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
bool ataque(Jugador * , Enemigo * );
void reiniciarArmadura(Armadura * );
void ataqueEnemigo(Jugador * , Enemigo * );

bool huida(Jugador *, Enemigo *);
void guardar_item(Inventario * , void * );
void recoger_items_enemigo(Jugador *, Enemigo *);

bool cicloPelea(Jugador * , List * );

void seleccionOpcion(Jugador * );
//void seleccionOpcionAyuda();

void movermeDeEscenario(Jugador * );
void lvlup(Jugador * );

/**********************************************/
/*                    Main                    */
/**********************************************/

int main(){
    HashMap *juego = createMap(100); // Crea un HashMap para almacenar los escenarios
    HashMap *mobs = createMap(100); // Crea un HashMap para almacenar los monstruos
    Jugador * player = NULL;
    if (juego == NULL) {
        fprintf(stderr, "Error al crear el HashMap\n");
        return 1;
    }
    srand(time(NULL));
    leer_escenarios(juego); // Llama a la funciÃ³n para leer los escenarios desde el archivo CSV
    //mostrarMap(juego);
    leer_mobs(mobs); // Llama a la funciÃ³n para leer los monstruos desde el archivo CSV
    //mostrar_mobs(mobs); // Muestra el contenido del HashMap
    asignar_mobs(juego, mobs);

    char op;
    char name[50];
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
            printf("INDIQUE EL NOMBRE DEL NUEVO JUGADOR: ");
            scanf(" %49s", name);
            getchar();
            player = createPlayer(name, juego);
            seleccionOpcion(player);
            break;
        case '2':
            //Ayuda
            //seleccionOpcionAyuda();
            break;
        case '3':
            //Creditos
            mostrarCreditos();
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
}

void guardar_item(Inventario * inv, void * item){
    if (!item) return;

    // Comprobamos si es poción
    Pocion *poc = (Pocion *)item;
    if (strcmp(poc->efecto, "Vida") == 0 || strcmp(poc->efecto, "Inmunidad") || 
    strcmp(poc->efecto, "Escudo") || strcmp(poc->efecto, "Estamina")){
        list_pushBack(inv->pocion, poc);
    }

    // Comprobamos si es arma
    Arma * arma = (Arma *)item;
    if (arma->ataque > 0 && arma->durabilidad > 0){
        printf("Arma actual: %s (Ataque: %d, Durabilidad: %d)\n", inv->armas.nombre, inv->armas.ataque, inv->armas.durabilidad);
        printf("Nueva arma:  %s (Ataque: %d, Durabilidad: %d)\n", arma->nombre, arma->ataque, arma->durabilidad);
        printf("¿Deseas reemplazar tu arma actual? (1 = Sí, 0 = No): ");
        int decision;
        scanf("%d", &decision);
        if (decision == 1) {
            inv->armas = *arma;
            printf("Has reemplazado tu arma con éxito.\n");
        } else {
            printf("No se reemplazó el arma.\n");
        }
        return;
    }

    //Comprobamos si es armadura
    Armadura * armadura = (Armadura *)item;
    Armadura * actual = NULL;
    if(strcmp(armadura->tipo, "Casco") == 0) actual = &inv->casco;
    else if (strcmp(armadura->tipo, "Pechera") == 0) actual = &inv->pechera;
    else if (strcmp(armadura->tipo, "Guantes") == 0) actual = &inv->guantes;
    else if (strcmp(armadura->tipo, "Pantalones") == 0) actual = &inv->pantalones;
    else if (strcmp(armadura->tipo, "Botas") == 0) actual = &inv->botas;

    if (actual) {
        printf("\n[%s]\n", armadura->tipo);
        printf("Armadura actual: %s (Defensa: %d, Durabilidad: %d)\n", actual->nombre, actual->defensa, actual->durabilidad);
        printf("Nueva armadura:  %s (Defensa: %d, Durabilidad: %d)\n", armadura->nombre, armadura->defensa, armadura->durabilidad);
        printf("¿Deseas reemplazar esta armadura? (1 = Sí, 0 = No): ");
        int decision;
        scanf("%d", &decision);
        if (decision == 1) {
            *actual = *armadura;
            printf("Has reemplazado la armadura con éxito.\n");
        } else {
            printf("No se reemplazó la armadura.\n");
        }
    }
}

void recoger_items_enemigo(Jugador *player, Enemigo *enemigo) {
    limpiarPantalla();

    Inventario *loot = &enemigo->item;

    // Crear lista con los 7 ítems fijos
    void *items_fijos[7];
    int total_fijos = 0;

    // Copiar arma
    Arma *arma = malloc(sizeof(Arma));
    *arma = loot->armas;
    items_fijos[total_fijos++] = arma;

    // Copiar armaduras
    Armadura *casco = malloc(sizeof(Armadura)); 
    *casco = loot->casco; items_fijos[total_fijos++] = casco;
    Armadura *guantes = malloc(sizeof(Armadura)); 
    *guantes = loot->guantes; items_fijos[total_fijos++] = guantes;
    Armadura *pechera = malloc(sizeof(Armadura)); 
    *pechera = loot->pechera; items_fijos[total_fijos++] = pechera;
    Armadura *pantalones = malloc(sizeof(Armadura)); 
    *pantalones = loot->pantalones; items_fijos[total_fijos++] = pantalones;
    Armadura *botas = malloc(sizeof(Armadura)); 
    *botas = loot->botas; items_fijos[total_fijos++] = botas;

    // Copiar poción (si existe)
    Pocion *p = list_first(loot->pocion);
    if (p) {
        Pocion *pocion = malloc(sizeof(Pocion));
        *pocion = *p;
        items_fijos[total_fijos++] = pocion;
    }

    // Mezclar los ítems
    srand(time(NULL));
    for (int i = total_fijos - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        void *tmp = items_fijos[i];
        items_fijos[i] = items_fijos[j];
        items_fijos[j] = tmp;
    }

    // Elegir cuántos mostrar (entre 1 y 7)
    int cantidad_a_mostrar = (rand() % total_fijos) + 1;

    List *items_posibles = list_create();
    for (int i = 0; i < cantidad_a_mostrar; i++) {
        list_pushBack(items_posibles, items_fijos[i]);
    }

    while (1) {
        limpiarPantalla();
        puts("=== OBJETOS DEL ENEMIGO ===");

        int index = 1;
        void *objeto = list_first(items_posibles);
        List *temp = list_create();

        while (objeto != NULL) {
            list_pushBack(temp, objeto);

            // Mostrar con tipo según posición
            if (index == 1) {
                Arma *a = objeto;
                printf("%d) [ARMA] %s - Ataque: %d - Durabilidad: %d\n", index, a->nombre, a->ataque, a->durabilidad);
            } else if (index >= 2 && index <= 6) {
                Armadura *arm = objeto;
                printf("%d) [ARMADURA] %s - Defensa: %d - Durabilidad: %d\n", index, arm->nombre, arm->defensa, arm->durabilidad);
            } else {
                Pocion *poc = objeto;
                printf("%d) [POCIÓN] %s - %s (%d)\n", index, poc->nombre, poc->efecto, poc->valor);
            }

            index++;
            objeto = list_next(items_posibles);
        }

        printf("%d) TOMAR TODO\n", index);
        printf("%d) CANCELAR\n", index + 1);
        printf("\nSELECCIONE UNA OPCIÓN: ");
        
        int opcion;
        scanf("%d", &opcion);

        if (opcion == index + 1 || opcion <= 0) {
            puts("CANCELANDO O OPCIÓN INVÁLIDA");
            presioneTeclaParaContinuar();
            break;
        }

        if (opcion == index) {
            for (void *elemento = list_first(items_posibles); elemento != NULL; elemento = list_next(items_posibles)) {
                guardar_item(&player->inventario, elemento);
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
                guardar_item(&player->inventario, item);

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
    puts("2) VER INVENTARIO"); //explorar-zona
    puts("3) RECOLECTAR ITEMS"); //gention-items
    puts("4) ATACAR A UN ENEMIGO"); //atacar-enemigo
    // POSIBLE guardar partida
    puts("5) SALIR AL MENU PRINCIPAL");
}

void menuOpcionesPelea()
{
    puts("========================================");
    puts("              MENU DE PELEA                   ");
    puts("========================================");
    puts("1) ATACAR AL ENEMIGO");
    puts("2) USAR POCION");
    puts("3) HUIR DE LA PELEA");
    puts("=========================================");
}

/**********************************************/
/*                  Leer CSVs                 */
/**********************************************/

void leer_escenarios(HashMap * juego){
    FILE *archivo = fopen("data/Mapa1.csv", "r");
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

/**********************************************/
/*          Asignar mobs a escenarios         */
/**********************************************/

void asignar_mobs(HashMap * escenarios, HashMap * mobs){
    for (Pair * esc = firstMap(escenarios); esc != NULL; esc = nextMap(escenarios)){
        Escenarios * escenario = esc->value;

        escenario->Enemigos = list_create();

        for (Pair * par_mob = firstMap(mobs); par_mob != NULL; par_mob = nextMap(mobs)){
            Enemigo * mob = par_mob->value;

            if (strcmp(mob->dificultad, escenario->dificultad) == 0){
                list_pushBack(escenario->Enemigos, mob);
            }
        }
    }
}

Enemigo * seleccionarEnemigo(List * enemigos){
    if(enemigos == NULL){
        return NULL;
    }

    int num_enemigos = 0;
    for (Enemigo *mob = list_first(enemigos); mob != NULL; mob = list_next(enemigos)){
        num_enemigos++;
    }
    if (num_enemigos == 0) {
        return NULL; // No enemies to select, so return NULL
    }
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

            Inventario * loot = malloc(sizeof(Inventario));

            seleccionado->item.armas = mob->item.armas;
            seleccionado->item.casco = mob->item.casco;
            seleccionado->item.guantes = mob->item.guantes;
            seleccionado->item.pechera = mob->item.pechera;
            seleccionado->item.pantalones = mob->item.pantalones;
            seleccionado->item.botas = mob->item.botas;

            loot->pocion = list_create();
            for (Pocion * p = list_first(mob->item.pocion); p != NULL; p = list_next(mob->item.pocion)){
                Pocion * nueva = malloc(sizeof(Pocion));
                *nueva = *p;
                list_pushBack(loot->pocion, nueva);
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

    strcpy(player->nombre, nombre);
    Pair * inicio = firstMap(juego);
    player->vida = 100;
    player->max_vida = 100;
    player->vida_total = 0;
    player->estamina = 15;
    player->max_estamina = 15;
    player->estamina_total = 0;
    player->defensa = 10;
    player->max_defensa = 100;
    player->defensa_total = 0;
    player->ataque = 4;
    player->ataque_total = 4;
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
    list_first(player->inventario.pocion); // Reset current to head
    for (int i = 1; i < opcion; i++) {
        list_next(player->inventario.pocion); // Move current to the selected potion
    }
    Pocion *seleccionada = (Pocion *)list_next(player->inventario.pocion); // current ahora apunta a la pocion

    printf("Usaste la pocion: %s\n", seleccionada->nombre);
    if (strcmp(seleccionada->efecto, "Vida") == 0){
        player->vida += seleccionada->valor;
        if (player->vida > player->max_vida) player->vida = player->max_vida;
        printf("Tu vida actual es: %d", player->vida);
    }
    else if (strcmp(seleccionada->efecto, "Inmunidad") == 0){
        player->inmunidad = true;
        puts("¡Ahora eres inmune temporalmente!");
    }
    else if (strcmp(seleccionada->efecto, "Escudo") == 0){
        player->defensa += seleccionada->valor;
        if (player->defensa > player->max_defensa) player->defensa = player->max_defensa;
        printf("Tu defensa aumentó a: %d\n", player->defensa);
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
        player->defensa_total += player->inventario.casco.defensa;
        aplicarBufo(player, player->inventario.pechera.bufo, player->inventario.pechera.valor);
    }
    if (strcmp(player->inventario.guantes.nombre, "Sin armadura") != 0){
        player->defensa_total += player->inventario.casco.defensa;
        aplicarBufo(player, player->inventario.guantes.bufo, player->inventario.guantes.valor);
    }
    if (strcmp(player->inventario.pantalones.nombre, "Sin armadura") != 0){
        player->defensa_total += player->inventario.casco.defensa;
        aplicarBufo(player, player->inventario.pantalones.bufo, player->inventario.pantalones.valor);
    }
    if (strcmp(player->inventario.botas.nombre, "Sin armadura") != 0){
        player->defensa_total += player->inventario.casco.defensa;
        aplicarBufo(player, player->inventario.botas.bufo, player->inventario.botas.valor);
    }
}

void reiniciarArma(Arma * arma){
    strcpy(arma->nombre, "Sin arma");
    arma->ataque = 0;
    arma->durabilidad = 0;
}

bool ataque(Jugador * player, Enemigo * enemigo){
    enemigo->vida -= ((player->ataque_total * player->ataque_total) / (player->ataque_total + enemigo->defensa)); // El enemigo recibe daño del jugador
    player->inventario.armas.durabilidad -=1;
    if (player->inventario.armas.durabilidad == 0){
        printf("Tu %s se ha roto", player->inventario.armas.nombre);
        reiniciarArma(&player->inventario.armas);
    }
    if (enemigo->vida <= 0){
        puts("El enemigo ha sido derrotado!");
        player->experiencia+= enemigo->exp_dada;
        //recoger_items_enemigos(player, enemigo);
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
    int margen = (player->estamina + player->vida) / enemigo->vida; // Calcula el margen de éxito para huir
    
    int index = rand() % enemigo->vida;
    
    if (index < margen) {
        puts("¡Has logrado huir de la pelea!");
        player->estamina -= 5; // El jugador pierde estamina al intentar huir
        if (player->estamina < 0) player->estamina = 0; // Asegura que la estamina no sea negativa
        return true; // El jugador ha huido exitosamente
    }
    return false;
}

bool cicloPelea(Jugador * player, List * enemigos)
{
    Enemigo * enemigo = seleccionarEnemigo(enemigos); // Selecciona un enemigo de la lista proporcionada

    //calcularDefensaActual(player);
    if (enemigo == NULL) {
        printf("No se encontró ningún enemigo para pelear en esta zona.\n");
        presioneTeclaParaContinuar(); // Pausa para que el usuario lea el mensaje
        return true; // El jugador sigue vivo, pero no hubo pelea.
    }

    bool EnemigoVivo = true; // Variable para controlar si el enemigo está activo
    while(EnemigoVivo && player->vida > 0) {
        limpiarPantalla();
        
        calcularEstatsT(player);
        
        printf("Jugador: %s | Vida: %d | Estamina: %d | Ataque: %d | Defensa: %d\n",
            player->nombre, player->vida, player->estamina, player->ataque_total, player->defensa_total);
        printf("Enemigo: %s | Vida: %d | Defensa: %d\n",
            enemigo->nombre, enemigo->vida, enemigo->defensa);
        
        menuOpcionesPelea(); // Muestra el menú de opciones de pelea
        bool repetirAccion = false; // Variable para controlar si se repite la acción
        
        char opcion;
        printf("INGRESE SU OPCION: ");
        scanf(" %c", &opcion);

        switch (opcion)
        {
        case '1':
            EnemigoVivo = ataque(player, enemigo);
            break;
        case '2':
            repetirAccion = usarPociones(player); // El jugador usa una poción
            break;
        case '3':
            if(huida(player, enemigo)) return true;
            break;
        default:
            puts("Opción no válida, por favor intente de nuevo.");
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
                movermeDeEscenario(player);
                break;
            case '2':
                //verEstado(); //FUNCIÓN PARA VER EL INVENTARIO DEL JUGADOR
                break;
            case '3':
                //recolectarItems(); //FUNCIÓN PARA RECOLECTAR LOS ITEMS DE LA ZONA
                break;
            case '4':
                jugadorVivo = cicloPelea(player, player->actual->Enemigos); // Ciclo de pelea con los enemigos de la zona actual
                break;
            case '5':
                //equipar_DesequiparItem(); //FUNCIÓN PARA EQUIPAR Y DESEQUIPAR ITEMS
                return;
            default:
                printf("OPCION NO VALIDA.\n");
                break;
        }
        presioneTeclaParaContinuar();
    } // El bucle continuará mientras el jugador esté activo
}

void lvlup(Jugador *jugador) {
    jugador->nivel++;
    jugador->vida += 5; // Incrementa la vida al subir de nivel
    jugador->ataque += 2; // Incrementa el ataque al subir de nivel
    jugador->defensa += 1; // Incrementa la defensa al subir de nivel
    printf("¡Felicidades! Has subido al nivel %d.\n", jugador->nivel);
}

void movermeDeEscenario(Jugador *jugador)
{
    char direccion;
    int movimiento = 0;

    do {
        limpiarPantalla();
        printf("Estás en: %s\n", jugador->actual->nombre);
        printf("¿A dónde deseas moverte?\n");

        if (jugador->actual->arriba)
            printf("  Arriba (w): %s\n", jugador->actual->arriba->nombre);
        if (jugador->actual->abajo)
            printf("  Abajo (s): %s\n", jugador->actual->abajo->nombre);
        if (jugador->actual->izquierda)
            printf("  Izquierda (a): %s\n", jugador->actual->izquierda->nombre);
        if (jugador->actual->derecha)
            printf("  Derecha (d): %s\n", jugador->actual->derecha->nombre);

        printf("\nSolo se muestran las direcciones disponibles.\n");
        printf("Ingrese dirección (w/a/s/d): ");
        scanf(" %c", &direccion);

        switch (direccion) {
            case 'w':
                if (jugador->actual->arriba) {
                    jugador->actual = jugador->actual->arriba;
                    movimiento = 1;
                } else {
                    printf("No puedes moverte en esa dirección.\n");
                }
                break;
            case 'a':
                if (jugador->actual->izquierda) {
                    jugador->actual = jugador->actual->izquierda;
                    movimiento = 1;
                } else {
                    printf("No puedes moverte en esa dirección.\n");
                }
                break;
            case 's':
                if (jugador->actual->abajo) {
                    jugador->actual = jugador->actual->abajo;
                    movimiento = 1;
                } else {
                    printf("No puedes moverte en esa dirección.\n");
                }
                break;
            case 'd':
                if (jugador->actual->derecha) {
                    jugador->actual = jugador->actual->derecha;
                    movimiento = 1;
                } else {
                    printf("No puedes moverte en esa dirección.\n");
                }
                break;
            default:
                printf("Dirección inválida. Intenta nuevamente.\n");
                break;
        }

        if (!movimiento) {
            presioneTeclaParaContinuar();
        }

    } while (!movimiento);

    printf("Te has movido a: %s\n", jugador->actual->nombre);
    presioneTeclaParaContinuar();
}
