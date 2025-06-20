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
    int max_vida;
    int estamina;
    int max_estamina;
    int defensa;
    int max_defensa;
    int defensa_total;
    int ataque;
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
bool cicloPelea(Jugador * , List * );
void seleccionOpcion(Jugador * );

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
    //leer_escenarios(juego); // Llama a la funciÃ³n para leer los escenarios desde el archivo CSV
    //mostrarMap(juego);
    //leer_mobs(mobs); // Llama a la funciÃ³n para leer los monstruos desde el archivo CSV
    //mostrar_mobs(mobs); // Muestra el contenido del HashMap
    //asignar_mobs(juego, mobs);


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
            //cargar los csv y hacer conexiones. hacerlo en un condicional para que ocurra una sola vez
            leer_escenarios(juego); // Llama a la funciÃ³n para leer los escenarios desde el archivo CSV
            leer_mobs(mobs); // Llama a la funciÃ³n para leer los monstruos desde el archivo CSV
            asignar_mobs(juego, mobs);

            printf("Indeque el nombre del nuevo jugador:");
            scanf(" %49s", name);
            getchar();
            player = createPlayer(name, juego);
            seleccionOpcion(player);
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

/**********************************************/
/*               Mostrar menus                */
/**********************************************/


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
    puts("2) VER INVENTARIO"); //explorar-zona
    puts("3) RECOLECTAR ITEMS"); //gention-items
    puts("4) ATACAR A UN ENEMIGO"); //atacar-enemigo
    // POSIBLE guardar partida
    puts("5) SALIR AL MENU PRINCIPAL");
}

void menuOpcionesPelea()
{
    puts("========================================");
    puts("               PELEA                   ");
    puts("========================================");
    puts("1) Atacar al enemigo");
    puts("2) Usar poción");
    puts("3) Huir de la pelea");
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
    player->estamina = 15;
    player->max_estamina = 15;
    player->defensa = 100;
    player->max_defensa = 100;
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
    List * temporal = list_create();

    for (Pocion* p = list_first(player->inventario.pocion); p != NULL; p = list_next(player->inventario.pocion)){
        printf("%d) %s - %s (+%d)\n", index, p->nombre, p->efecto, p->valor);
        list_pushBack(temporal, p);
        index++;
    }
    
    printf("%d) Cancelar\n", index);
    printf("Seleccione la opcion a usar: ");
    int opcion;
    scanf("%d", &opcion);

    if (opcion >= index || opcion < 0){
        puts("No se uso ninguna pocion.");
        list_clean(temporal);
        presioneTeclaParaContinuar();
        return true;
    }

    int actual = 1;
    Pocion *seleccionada = list_first(temporal);
    while(seleccionada != NULL && actual < opcion){
        seleccionada = list_next(temporal);
        actual++;
    }

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

    List * nueva_lista = list_create();
    for (Pocion* p = list_first(player->inventario.pocion); p != NULL; p = list_next(player->inventario.pocion)){
        if (p != seleccionada){
            list_pushBack(nueva_lista, p);
        }
        else{
            free(p);
        }
    }
    list_clean(player->inventario.pocion);
    player->inventario.pocion = nueva_lista;

    //popCurrent

    list_clean(temporal);
    presioneTeclaParaContinuar();

    if(list_size(player->inventario.pocion) == 0){
        puts("Te has quedado sin pociones");
        presioneTeclaParaContinuar();
        return false;
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
        printf("Jugador: %s | Vida: %d | Estamina: %d | Ataque: %d | Defensa: %d\n",
            player->nombre, player->vida, player->estamina, player->ataque, player->defensa);
        printf("Enemigo: %s | Vida: %d | Defensa: %d\n",
            enemigo->nombre, enemigo->vida, enemigo->defensa);
        
        menuOpcionesPelea(); // Muestra el menú de opciones de pelea
        bool repetirAccion = false; // Variable para controlar si se repite la acción
        
        char opcion;
        printf("Seleccione una opción: ");
        scanf(" %c", &opcion);

        switch (opcion)
        {
        case '1':
            enemigo->vida -= (player->ataque - enemigo->defensa); // El enemigo recibe daño del jugador
            break;
        case '2':
            repetirAccion = usarPociones(player); // El jugador usa una poción
            break;
        case '3':
            /* code */
            break;
        
        default:
            puts("Opción no válida, por favor intente de nuevo.");
            repetirAccion = true; // Repite la acción si la opción no es válida
            break;
        }
        if (repetirAccion) continue; // Si se repite la acción, vuelve al inicio del bucle
        //if(opcion != '1' || opcion != '2'|| opcion != '3') continue;
        player->vida -= (int) (enemigo->ataque/(player->defensa_total * 0.01)); // El jugador recibe daño del enemigo

        // Simulación de acciones (esto debería ser reemplazado por la lógica real del juego)
        // Por ejemplo:
        // - El jugador ataca al enemigo
        // - El enemigo ataca al jugador
        // - Se verifica si alguno ha muerto
        // - Se actualizan las estadísticas

        // Simulación simple para continuar el ciclo
        if (enemigo->vida <= 0) {
            puts("El enemigo ha sido derrotado!");
            EnemigoVivo = false; // El enemigo ya no está vivo
        }
        
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
                break;
            case '2':
                //verEstado(); //FUNCIÓN PARA VER EL ESTADO DEL JUGADOR
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
                printf("Opcion no valida.\n");
                break;
        }
        presioneTeclaParaContinuar();
    } // El bucle continuará mientras el jugador esté activo
}