#ifndef DYNARRAYS_H
#define DYNARRAYS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "constants.h"

typedef struct ChainonPtr
{
    void* tete;
    struct ChainonPtr* queue;
} ptrlist;


typedef struct
{
  uint8_t* map;
  uint8_t block_size;
  uint8_t n_blocks;
} bitmap;




typedef struct
{
  int* tab;
  size_t len;
  uint8_t capacity;
    
} intlist;


typedef struct
{
    int**tab;
    size_t len;
    uint8_t capacity;
} intptrlist;

typedef struct
{
  char** tab;
  size_t len;
  uint8_t capacity;
  
} strlist;



typedef struct
{
  intlist* tab;
  int len;
  int capacity;
  
} listlist;


typedef struct Token {
  char* debut;
  uint16_t len;
} Token;

typedef struct toklist {
  Token* tab;
  char* source_string;
  bool free_source_string;
  size_t len;
  uint8_t capacity;
} toklist;


//prototypes des fonctions

/***********************toklist*************************/
#define toksub(chaine, deb, fin)        ((Token) {.debut = chaine + deb, .len = fin - deb})
bool tokeq(Token tok, char* string);
void printToken(Token tok);
char stringize(Token tok);
void unstringize(Token tok, char sov);
char* tokdup(Token tok);
toklist toklist_create(size_t len);
void toklist_aff(toklist* list);
void toklist_append(toklist* list, Token chaine);
void toklist_destroy(toklist* list);
size_t toklist_count(toklist* list, char* chaine);
bool strlist_token_inList(strlist* list, Token chaine);
int strlist_token_index(strlist* list, Token chaine);


/**************************ptrlist**********************/
ptrlist* ptrlist_create(void);
void ptrlist_append(ptrlist* list, void* ptr);
void ptrlist_remove(ptrlist* list, void* ptr, bool error);
//int ptrlist_index(ptrlist* list, void* ptr);
//void ptrlist_aff(ptrlist* l);
//int ptrlist_len(ptrlist* l);
int ptrlist_destroy(ptrlist* l, bool freeElements, bool freeTab);
//void ptrlist_replace(ptrlist* liste, void* aRemplacer, void* nvlleValeur);
bool ptrlist_isEmpty(ptrlist* l);
void* ptrlist_pop(ptrlist* list);
//bool ptrlist_inList(ptrlist* l, void* el);
//void ptrlist_direct_remove(ptrlist* list, ptrlist* ptr, ptrlist* prev);


/*********************intptrlist**********************/
intptrlist intptrlist_create(size_t len);
void intptrlist_append(intptrlist* list, void* ptr);
int intptrlist_index(intptrlist* list, void* ptr);
void intptrlist_destroy(intptrlist* list);

/**************************intlist**********************/
intlist intlist_create(size_t len);
size_t intlist_getsize(intlist list);
//void intlist_aff(intlist* list);
void intlist_append(intlist* list,int nombre);
//void intlist_resize(intlist* list, int newLen);
void intlist_remove(intlist* list, size_t index);
int intlist_count(intlist* list, int nb);
bool intlist_inList(intlist* list, int nombre);
int intlist_index(intlist* list, int nombre);
void intlist_insert(intlist* list,int nombre, size_t index);
int intlist_max(intlist* list);

/*************************boolmap*************************/
void bitmap_init(bitmap* bm);
void bitmap_set(bitmap* bm, size_t index, bool value);
bool bitmap_get(bitmap* bm, size_t index);
void bitmap_destroy(bitmap* bm);
void bitmap_print(bitmap* bm);

/**************************strlist**********************/
void strlist_destroy(strlist* list, bool bo);
strlist* strlist_create(size_t len);
size_t strlist_getsize(strlist* list);
void strlist_aff(strlist* list);
void strlist_append(strlist* list, char* chaine);
//void strlist_resize(strlist* list, int newLen, bool freeElement);
void strlist_remove(strlist* list, size_t index, bool freeElement);
int strlist_count(strlist* list, char* chaine);
bool strlist_inList(strlist* list, char* chaine);
bool strlist_inList_sub(strlist* list, char* chaine, int debut, int fin);
int strlist_index(strlist* list, char* chaine);
//void strlist_insert(strlist* list,char* chaine, int index);




#endif