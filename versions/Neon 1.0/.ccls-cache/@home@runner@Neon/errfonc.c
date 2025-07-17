#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


#include "headers.h"



//creation du tableau de pointeurs generiques a liberer en cas d’erreur


extern ptrlist allocptr;



//déclaration des variables globales à cut
extern strlist acceptedChars;
extern listlist syntax;
extern strlist sousop;
extern intlist gramm1;
extern strlist operateurs3;
extern strlist operateurs1;
extern strlist operateurs2;
extern strlist blockwords;
extern strlist boolean;
extern strlist keywords;
extern strlist lkeywords;
extern strlist constant;


extern strlist OPERATEURS;
extern intlist PRIORITE;
extern intlist OPERANDES;


//stockage des variables
extern strlist NOMS;
extern intlist TYPES;
extern intlist ADRESSES;
extern strlist VARIABLES;
extern listlist LISTES;

extern strlist NOMSBUILTINSFONC;



extern char* (*OPFONC[NBOPERATEURS])(char*,unsigned,char*,unsigned,int*);
extern char* (*BUILTINSFONC[NBBUILTINFONC])(strlist*, intlist*, int*);



extern unsigned nbTempList;



void freeAllPtr(void)
{
    unsigned i=0;
    i=ptrlist_destroy(&allocptr, true);
    // printf("%u fuite(s) memoire.\n", i);
    return;
}









void quitError(char* message)
{
  printf("%s\n",message);
  freeAllPtr();
  pause();
  exit(EXIT_FAILURE);
}


/*liste de fonctions a transformer en err_fonc() :*/



void* err_malloc(size_t taille)
{
  void* ptr=malloc(taille);
  
  
  if (ptr==NULL)
  {
    quitError(ERROR_MALLOC);
  }
  
  ptrlist_append(&allocptr, ptr);
  // printf("m%p ",(unsigned long)ptr);
  return ptr;
}



void err_memset(void* ptr, int valeur, size_t taille)
{
  void* err=memset(ptr, valeur, taille);
  if (err==NULL)
  {
    quitError(ERROR_MEMSET);
  }
}



void* err_realloc(void* ptr, size_t taille)
{
  void* tmp;
  
  if (taille==0)
  {
    tmp = malloc(0);
    free(ptr);
  }
  else
  {
    tmp=realloc(ptr, taille);
  }
  
  if (tmp==NULL)
  {
    quitError(ERROR_REALLOC);
  }
  
  ptrlist_replace(&allocptr, ptr, tmp);
    
  ptr=tmp;
  // printf("r%p ",(unsigned long)ptr);
  return ptr;
}


void err_free(void* ptr)
{
  // printf("f%p ",ptr);
  free(ptr);
  ptrlist_remove(&allocptr,ptr);
}


void b(double balise)
{
    printf("b%lf ", balise);
    return;
}


