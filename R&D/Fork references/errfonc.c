#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


#include "headers.c"

extern void* PTRERROR;
extern char* EXCEPTION;

extern int CODE_ERROR;

//creation du tableau de pointeurs generiques a liberer en cas d’erreur
extern strlist ERRORS_MESSAGES;


extern strlist exceptions;
extern intlist exceptions_err;

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
extern strlist* NOMS;
extern NeList* ADRESSES;

extern strlist NOMSBUILTINSFONC;
extern strlist HELPBUILTINSFONC;
extern intlist TYPESBUILTINSFONC;

extern NeObject* (*OPFONC[NBOPERATEURS])(NeObject*,NeObject*);
extern NeObject* (*BUILTINSFONC[NBBUILTINFONC])(NeList*);




void printError(int code)
{

    if (CODE_ERROR != 74) // 74 si quitté avec une exception
    {
        int type = exceptions_err.tab[code];

        setColor(RED);

        printString(" ");
        printString(exceptions.tab[type]);
        printString(" : ");

        setColor(GREEN);
        
        printString(ERRORS_MESSAGES.tab[code]);
        printString(" (");setColor(WHITE);printInt(code);setColor(GREEN);printString(")");
        
        setColor(WHITE);

        if (type != 0 && PTRERROR != NULL) {
          printString(" : ");
          affProgram((Tree*)PTRERROR);
        }
        else
          newLine();
    }
    else
    {
        setColor(RED);
        printString(" The program exited with the exception : ");
        printString(EXCEPTION);
        newLine();
        setColor(WHITE);
    }

}





void* err_malloc(size_t taille)
{
  return malloc(taille);
}



void err_memset(void* ptr, int valeur, size_t taille)
{
  void* err=memset(ptr, valeur, taille);
  if (err==NULL)
  {
    CODE_ERROR = 12;
    return;
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

  
  ptr=tmp;
  
  return ptr;
}


void err_free(void* ptr)
{
  free(ptr);
}


void b(double balise)
{
    // printf("b%lf ", balise);
    return;
}


