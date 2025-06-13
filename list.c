#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Node Node;

struct Node {
    void * data;
    Node * next;
    Node * prev;
};

struct List {
    Node * head;
    Node * tail;
    Node * current;
};

typedef List List;

Node * createNode(void * data) {
  Node * new = (Node *)malloc(sizeof(Node));
  assert(new != NULL);
  new->data = data;
  new->prev = NULL;
  new->next = NULL;
  return new;
}

List *list_create() {
  List* lista = (List*)malloc(sizeof(List));
    lista->head = NULL;
    lista->tail = NULL;
    lista->current = NULL;
    return lista;
}

void *list_first(List *L) {
  if(L == NULL || L->head == NULL) return NULL;
    L->current = L->head;
    return L->head->data;
}

void *list_last(List *L) {
  if (L == NULL || L->head == NULL) return NULL;
    L->current = L->tail;
    return L->current->data;
}

void *list_next(List *L) {
  if(L == NULL || L->current == NULL) return NULL;
    L->current = L->current->next;

    if (L->current != NULL) return L->current->data;
    return NULL;
}

void * prevList(List * L) {
  if(L == NULL || L->current == NULL) return NULL;
  L->current = L->current->prev;
  if (L->current == NULL) return NULL;
  return L->current->data;
}

void list_pushFront(List *L, void *data) {
  Node * nuevo_nodo = createNode(data);
    nuevo_nodo->data = data;
    nuevo_nodo->next = L->head;
    nuevo_nodo->prev = NULL;
    if(L->head != NULL) L->head->prev = nuevo_nodo;
    else L->tail = nuevo_nodo;
    L->head = nuevo_nodo;
}

void list_pushBack(List *L, void *data) {
  L->current = L->tail;
  list_pushCurrent(L,data);
}

void list_pushCurrent(List *L, void *data) {
    if (L == NULL) return;
    if (L->current == NULL){
      list_pushFront(L, data);
      return;
    }
    Node * nuevo_nodo = createNode(data);
    nuevo_nodo->prev = L->current;
    nuevo_nodo->next = L->current->next;
    if (L->current->next != NULL) L->current->next->prev = nuevo_nodo;
    else L->tail = nuevo_nodo;
    L->current->next = nuevo_nodo;
}

void *list_popFront(List *L) {
  L->current = L->head;
  return list_popCurrent(L);
}

void *list_popBack(List *L) {
  L->current = L->tail;
  return list_popCurrent(L);
}

void *list_popCurrent(List *L) {
  if (L == NULL || L->current == NULL) return NULL;

  void *data = L->current->data;
  Node *siguiente = L->current->next;

  if (L->current == L->head && L->current == L->tail) {
    L->head = NULL;
    L->tail = NULL;
  } else if (L->current == L->head) {
    L->head = L->current->next;
    L->head->prev = NULL;
  } else if (L->current == L->tail) {
    L->tail = L->current->prev;
    L->tail->next = NULL;
  } else {
    L->current->prev->next = L->current->next;
    L->current->next->prev = L->current->prev;
  }

  free(L->current);
  L->current = siguiente;
  return data;
}

void list_clean(List *L) {
    while (L->head != NULL) {
      list_popFront(L);
    }
}

int list_size(List *L) {
    if (L == NULL) return 0;

    int contador = 0;
    Node *nodo = L->head;
    while (nodo != NULL) {
        contador++;
        nodo = nodo->next;
    }
    return contador;
}