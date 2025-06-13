#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = strdup(key);
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    if(map->size >= map->capacity*0.75) enlarge(map);
    long posicion = hash(key, map->capacity);
    Pair * nuevoPar = createPair(key, value);
    Pair * aux = map->buckets[posicion];
    if(aux == NULL) {
        map->buckets[posicion] = nuevoPar;
        map->size++;
    } 
    else{
        while(map->buckets[posicion] != NULL && map->buckets[posicion]->key != NULL){
            if(is_equal(map->buckets[posicion]->key, key)){
                map->buckets[posicion]->value = value;
                free(nuevoPar);
                return;
            }
            posicion = (posicion + 1) % map->capacity;
        }
        map->buckets[posicion] = nuevoPar;
        map->size++;
    }

}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
    Pair** aux = map->buckets;
    long old_capacity = map->capacity;
    map->capacity *= 2;

    map->buckets = (Pair**)calloc(map->capacity, sizeof(Pair*));
    map->size = 0;

    for(long i = 0; i < old_capacity; i++){
        if (aux[i] != NULL && aux[i]->key != NULL){
            insertMap(map, aux[i]->key, aux[i]->value);
        }
    }
    free(aux);
}


HashMap * createMap(long capacity) {
    HashMap* nuevoMapa = (HashMap*)malloc(sizeof(HashMap));
    if(nuevoMapa == NULL) return NULL;
    nuevoMapa->capacity = capacity;
    nuevoMapa->size = 0;
    nuevoMapa->current = -1;

    nuevoMapa->buckets = (Pair**)malloc(sizeof(Pair*)*capacity);
    if (nuevoMapa->buckets == NULL){
        free(nuevoMapa);
        return NULL;
    }
    for (int i = 0; i < capacity; i++) {
        nuevoMapa->buckets[i] = NULL;
    }
    return nuevoMapa;
}

void eraseMap(HashMap * map,  char * key) {    
    long posicion = hash(key, map->capacity);
    Pair* aux = map->buckets[posicion];
    if(aux == NULL) return;
    while(map->buckets[posicion] != NULL && map->buckets[posicion]->key != NULL){
        if(is_equal(map->buckets[posicion]->key, key)){
            map->buckets[posicion]->key = NULL;
            map->size--;
        }
        posicion = (posicion + 1) % map->capacity;
    }
}

Pair * searchMap(HashMap * map,  char * key) {   
    long posicion = hash(key, map->capacity);
    Pair * aux = map->buckets[posicion];
    if(aux == NULL) return NULL;
    while(map->buckets[posicion] != NULL && map->buckets[posicion]->key != NULL){
            if(is_equal(map->buckets[posicion]->key, key)){
                map->current = posicion;
                return map->buckets[posicion];
            }
            posicion = (posicion + 1) % map->capacity;
    }
    return NULL;
}

Pair * firstMap(HashMap * map) {
    for (long i = 0; i < map->capacity; i++){
        if(map->buckets[i] != NULL && map->buckets[i]->key != NULL){
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {
    long aux = map->current + 1;
    while(aux < map->capacity){
        if(map->buckets[aux] != NULL && map->buckets[aux]->key != NULL){
            map->current = aux;
            return map->buckets[aux];
        }
        aux++;
    }
    
    return NULL;
}

void cleanMap(HashMap *map) {
    if (map == NULL) return;

    for (int i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL) {
            free(map->buckets[i]);
            map->buckets[i] = NULL;
        }
    }

    map->size = 0;
    map->current = -1;
}