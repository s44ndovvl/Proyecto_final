#ifndef HashMap_h
#define HashMap_h


typedef struct HashMap HashMap;

typedef struct Pair {
     char * key;
     void * value;
} Pair;

// Función para crear un mapa
HashMap * createMap(long capacity);

// Función para insertar una key y su valor en el mapa
void insertMap(HashMap * table, char * key, void * value);

// Función para borrar una key del mapa
void eraseMap(HashMap * table, char * key);

// Función para buscar una key en el mapa
Pair * searchMap(HashMap * table, char * key);

// Función que retorna el primer elemento del mapa
Pair * firstMap(HashMap * table);

// Función que avanza al siguiente elemento del mapa
Pair * nextMap(HashMap * table);

// Función para agrandar la capacidad del mapa
void enlarge(HashMap * map);

// Función para limpiar el mapa
void cleanMap(HashMap * map);

#endif /* HashMap_h */