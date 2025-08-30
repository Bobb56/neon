#ifndef DYNARRAYS_H
#define DYNARRAYS_H

#include "constants.h"

typedef struct ChainonPtr
{
    void* tete;
    struct ChainonPtr* queue;
} ptrlist;




typedef struct
{
  int* tab;
  int len;
  int capacity;
    
} intlist;


typedef struct
{
    int**tab;
    int len;
    int capacity;
} intptrlist;

typedef struct
{
  char** tab;
  int len;
  int capacity;
  
} strlist;



typedef struct
{
  intlist* tab;
  int len;
  int capacity;
  
} listlist;




void strlist_copy(strlist* list, const char** tab, int len);
void listlist_copy(listlist* list, const intlist* tab, int len);
void intlist_copy(intlist* list, const int* tab, int len);



//prototypes des fonctions

/**************************listlist**********************/
listlist listlist_create(int len);
void listlist_append(listlist* list, intlist* ptr);
void listlist_remove(listlist* list, int index);
void listlist_aff(listlist* list);
void listlist_destroy(listlist* list);



/**************************ptrlist**********************/
ptrlist* ptrlist_create(void);
void ptrlist_append(ptrlist* list, void* ptr);
void ptrlist_remove(ptrlist* list, void* ptr, bool error);
int ptrlist_index(ptrlist* list, void* ptr);
void ptrlist_aff(ptrlist* l);
int ptrlist_len(ptrlist* l);
int ptrlist_destroy(ptrlist* l, bool freeElements, bool freeTab);
void ptrlist_replace(ptrlist* liste, void* aRemplacer, void* nvlleValeur);
bool ptrlist_isEmpty(ptrlist* l);
void* ptrlist_pop(ptrlist* list);
bool ptrlist_inList(ptrlist* l, void* el);
void ptrlist_direct_remove(ptrlist* list, ptrlist* ptr, ptrlist* prev);


/*********************intptrlist**********************/
intptrlist intptrlist_create(int len);
void intptrlist_append(intptrlist* list, int* ptr);
void intptrlist_destroy(intptrlist* list);

/**************************intlist**********************/
intlist intlist_create(int len);
void intlist_aff(intlist* list);
void intlist_append(intlist* list,int nombre);
void intlist_resize(intlist* list, int newLen);
void intlist_remove(intlist* list,int index);
int intlist_count(intlist* list, int nb);
bool intlist_inList(intlist* list, int nombre);
int intlist_index(intlist* list, int nombre);
void intlist_insert(intlist* list,int nombre, int index);
int intlist_max(intlist* list);



/**************************strlist**********************/
void strlist_destroy(strlist* list, bool bo);
strlist* strlist_create(int len);
void strlist_aff(strlist* list);
void strlist_append(strlist* list, char* chaine);
void strlist_resize(strlist* list, int newLen, bool freeElement);
void strlist_remove(strlist* list,int index, bool freeElement);
int strlist_count(strlist* list, char* chaine);
bool strlist_inList(strlist* list, char* chaine);
int strlist_index(strlist* list, char* chaine);
void strlist_insert(strlist* list,char* chaine, int index);




#endif