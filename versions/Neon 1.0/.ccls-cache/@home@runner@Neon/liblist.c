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





//listlist

listlist listlist_create(unsigned len)// crée une liste de pointeurs
{
  listlist list;//crée la structure
  
  list.capacity = 0;
  while (pow(2, list.capacity) < len)
    list.capacity++;
  
  list.tab=err_malloc(pow(2, list.capacity)*sizeof(strlist));//initialise le tableau de longueur len avec de zéros
  
  err_memset(list.tab,0,len);
  list.len=len;//initialise la bonne longueur
  return list;//retourne la structure
}



void listlist_append(listlist* list, strlist* ptr)//ajoute un élément à la fin de la liste
{
  strlist* tmp;

  if (pow(2, list->capacity)==list->len)
  {
    list->capacity++;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(strlist));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }

  
  
  list->tab[list->len]=*ptr;//affecte nombre au dernier élément
  list->len++;//incrémente la longueur
}




void listlist_remove(listlist* list,unsigned index)//indiquer si il faut libérer l'élément avant de le supprimer
{


  if (index >= list->len)
  {
    quitError(ERROR_OUT_OF_RANGE);
  }
  
  
  
  strlist_destroy(&list->tab[index]);
  
  for (unsigned i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i+1];
    
  strlist* tmp;
  
  if (pow(2, list->capacity-1)==list->len-1)
  {
    list->capacity--;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(strlist));//réalloue un nouveau pointeur de la bonne taille
    list->tab = tmp;
  }
  
  
  list->len--;
  
}



void listlist_aff(listlist* liste)
{
  for (unsigned i=0;i<liste->len;i++)
  {
    strlist_aff(&liste->tab[i]);
  }
  
}



void listlist_destroy(listlist* list)
{
  for (unsigned i=0;i<list->len;i++)
  {
    strlist_destroy(&list->tab[i]);
  }
  err_free(list->tab);
}







ptrlist ptrlist_create(void)
{
    ptrlist* l = malloc(sizeof(ptrlist));
    l->tete = NULL;
    l->queue = NULL;
    return *l;
}





void ptrlist_append(ptrlist* q, void* t)
{   
    if (q->tete == NULL)
    {
        q->tete = t;
    }
    else
    {
        ptrlist* chainon = malloc(sizeof(ptrlist));//on crée une copie du premier chaînon
        chainon->tete = q->tete;
        chainon->queue = q->queue;
        
        q->queue = chainon;//puis on fait pointer le premier élément de la liste sur le nouveau chaînon
        q->tete = t;
    }
    return ;
}




void ptrlist_aff(ptrlist* l)
{
    if (l->queue == NULL && l->tete == NULL)
        printf("[]\n");
    else if (l->queue == NULL)
        printf("[%p]\n", l->tete);
    
    else
    {
        printf("[");
        ptrlist* ptr = l;
        for (unsigned i = 1; ptr->queue != NULL; i++)
        {
            printf("%p, ", ptr->tete);
            ptr = ptr->queue;
        }
        printf("%p]\n", ptr->tete);
    }
    
    return;
}






unsigned ptrlist_destroy(ptrlist* l, _Bool freeElement)
{
    unsigned i=0;
    if (l->queue == NULL && (l->tete == NULL || !freeElement))
    {
        free(l);
    }
    else if (l->queue == NULL && freeElement)
    {
        free(l->tete);
        free(l);
        // printf("Liberation du pointeur %p  (1)\n", l);
        i=1;
    }
    
    else
    {
        ptrlist* ptr = l;
        ptrlist* tmp;
        for (i = 0; ptr != NULL; i++)
        {
            if (freeElement)
            {
                free(ptr->tete);
                // printf("Liberation du pointeur %p  (2)\n", ptr->tete);
            }
            tmp = ptr->queue;
            free(ptr);
            ptr=tmp;
        }
    }
    
    return i;
}



void ptrlist_replace(ptrlist* liste, void* aRemplacer, void* nvlleValeur)
{
    ptrlist* ptr = liste;
    for (unsigned i = 1; ptr->queue != NULL && ptr->tete != aRemplacer; i++)
    {
        ptr = ptr->queue;
    }
    
    if (ptr->queue == NULL && ptr->tete != aRemplacer)
        quitError(ERROR_PTR_NOT_IN_TYPE_LIST);
    
    ptr->tete = nvlleValeur;
    
    return ;
}



unsigned ptrlist_index(ptrlist* liste, void* element)
{
    ptrlist* ptr = liste;
    unsigned i=0;
    for (i = 0; ptr->queue != NULL && ptr->tete != element; i++)
    {
        ptr = ptr->queue;
    }
    
    if (ptr->queue == NULL && ptr->tete != element)
        quitError(ERROR_PTR_NOT_IN_TYPE_LIST);
    
    
    return i;
}





void ptrlist_remove(ptrlist* list, void* l)
{
    ptrlist* ptr = list;
    ptrlist* ptr2 = NULL;
    for (unsigned i = 1; ptr->queue != NULL && ptr->tete != l; i++)
    {
        ptr2 = ptr;
        ptr = ptr->queue;
    }
    
    
    if (ptr->queue == NULL && ptr->tete != l)
        quitError(ERROR_PTR_NOT_IN_TYPE_LIST);
    
    
    else if (ptr->queue == NULL && ptr2 == NULL)// on est le dernier élément de la liste
        list->tete = NULL;
    
    else if (ptr2 == NULL)// si on est au debut de la liste, donc ptr2 == NULL
    {
        ptr->tete = (ptr->queue)->tete;
        ptr->queue = (ptr->queue)->queue;
        free(ptr);
    }
    
    else //on est au milieu
    {
        ptr2->queue = ptr->queue;
        free(ptr);
    }
    
}





unsigned ptrlist_len(ptrlist* l)
{
    if (l->tete == NULL)
        return 0;
    
    ptrlist* ptr = l;
    unsigned i;
    for (i = 0; ptr != NULL; i++)
        ptr = ptr->queue;
    return i;
}




 
 
//intlist

intlist intlist_create(unsigned len)// crée une liste d'entiers
{
  intlist list;//crée la structure
  
  list.capacity = 0;
  
  while (pow(2, list.capacity) < len)
    list.capacity++;
  
  list.tab=err_malloc(pow(2, list.capacity)*sizeof(int));//initialise le tableau de longueur len avec de zéros
  
  err_memset(list.tab,0,len);
  
  list.len=len;//initialise la bonne longueur
  return list;//retourne la structure
}




void intlist_aff(intlist* list)//affiche une liste d'entiers
{
  if (list->len == 0)//si la liste a une longueur de zéro
  {
    printf("[]\n");
  }
  else
  {
    printf("[");
    for ( unsigned i = 0 ; i < list->len -1 ; i++)//affiche les éléments du premier à l'avant-dernier
      printf("%i, ",list->tab[i]);
  
    printf("%i]\n",list->tab[list->len-1]);//affiche le dernier élément
  }
}




void intlist_append(intlist* list,int nombre)//ajoute un élément à la fin de la liste
{
    
  int *tmp;
  if (pow(2, list->capacity)==list->len)
  {
    list->capacity++;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(int));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }

  list->tab[list->len]=nombre;//affecte nombre au dernier élément
  list->len++;//incrémente la longueur
}



void intlist_resize(intlist* list, unsigned newLen)//redimensionne la liste avec la nouvelle longueur
{
  int* tmp;
  if (newLen > pow(2, list->capacity) || newLen <= pow(2, list->capacity-1)) // on détermine la nouvelle capacité
  {
    list->capacity = 0;
    while (pow(2, list->capacity) < newLen)
      list->capacity++;
      
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(int));//réalloue un pointeur de la nouvelle taille
    
    if (tmp == NULL)
      quitError(ERROR_REALLOC);
    
    list->tab = tmp;
    
  }
  
  if (newLen > list->len)//initialisation des nouveaux éléments à 0 si nouveaux éléments il y a
  {
    for (unsigned i = list->len ; i < newLen ; i++)
	  list->tab[i]=0;
  }
  
  list->len=newLen;//modification de la longueur
  
}





void intlist_remove(intlist* list,unsigned index)//supprime un élément de la liste
{
  if (index >= list->len)
  {
    quitError(ERROR_OUT_OF_RANGE);
  }
  
  for (unsigned i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i+1];
    
  int *tmp;
  
  if (pow(2, list->capacity-1)==list->len-1)
  {
    list->capacity--;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(int));//réalloue un nouveau pointeur de la bonne taille
    list->tab = tmp;
  }
  
  list->len--;//décrémentation de la longueur
  
}



unsigned intlist_count(intlist* list, int nb)
{
  unsigned count=0;
  
  for (unsigned i = 0 ; i < list->len ; i++)
  {
    if (list->tab[i] == nb)
    {
      count++;
    }
  }
  
  return count;
}


_Bool intlist_inList(intlist* list, int nombre)
{
  for (unsigned i=0; i<list->len; i++)
  {
    if (nombre==list->tab[i])
    {
      return 1;
    }
    
  }
  return 0;
}




int intlist_index(intlist* list, int nombre)
{
  for (unsigned i=0; i<list->len; i++)
  {
    if (nombre==list->tab[i])
    {
      return i;
    }
    
  }
  quitError(ERROR_INT_NOT_IN_TYPE_LIST);
  return 1;
}



void intlist_insert(intlist* list,int nombre, unsigned index)//ajoute un élément à la place indiquée
{
  if (index > list->len)
  {
    quitError(ERROR_OUT_OF_RANGE);
  }
  
  int *tmp;
  if (pow(2, list->capacity)==list->len)
  {
    list->capacity++;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(int));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }
  
  for (unsigned i = list->len ; i > index; i--)//décale tous les éléments d’une case en + jusqu’a la place a inserer
    list->tab[i]=list->tab[i-1];
  
  list->tab[index]=nombre;

  list->len++;//incrémente la longueur
}


int intlist_max(intlist* list)
{
  int max;
  
  if (list->len>0)
  {
    max = list->tab[0];
    for (unsigned i=0 ; i<list->len;i++)
    {
      if (list->tab[i] > max)
        max=list->tab[i];
    }
    
  }
  else
  {
    quitError(ERROR_OUT_OF_RANGE);
  }
  return max;
}









//strlist


strlist strlist_create(unsigned len)
{
  strlist list;
  
  list.capacity = 0;
  
  while (pow(2, list.capacity) < len)
    list.capacity++;
  
  list.tab=err_malloc(pow(2, list.capacity)*sizeof(int));
  
  err_memset(list.tab,0,len);
  list.len=len;
  return list;
}





void strlist_aff(strlist* list)
{
  if (list->len == 0)
  {
    printf("[]\n");
  }
  else
  {
    printf("[");
    char* tmp;
    for ( unsigned i = 0 ; i < list->len -1 ; i++)
    {
        tmp = traitementStringInverse(list->tab[i]);
        (tmp==NULL) ? printf("\"\", ") : printf("\"%s\", ",tmp);//affiche "" si le pointeur est nul
        err_free(tmp);
    }
  
  
	(list->tab[list->len-1]==NULL) ? printf("\"\"]\n") : printf("\"%s\"]\n",list->tab[list->len-1]);//dernier élément
  }
}




void strlist_append(strlist* list, char *chaine)
{
  char** tmp;
  if (pow(2, list->capacity)==list->len)
  {
    list->capacity++;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(char*));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }

  list->tab[list->len]=chaine;
  list->len++;
}




void strlist_destroy(strlist* list)
{
  for (unsigned i=0 ; i < list->len;i++)
  {
    err_free(list->tab[i]);
  }
  err_free(list->tab);
  
}




void strlist_resize(strlist* list, unsigned newLen, _Bool freeElement)
{
  
  //il faut au préalable libérer les éléments qui vont être tronqués si la nouvelle longueur est plus petite
  if (newLen < list->len && freeElement)
  {
      for (unsigned i = newLen ; i < list->len ; i++)
	    err_free(list->tab[i]);
  }
  
  
  char** tmp;
  
  if (newLen > pow(2, list->capacity) || newLen <= pow(2, list->capacity-1)) // on détermine la nouvelle capacité
  {
    list->capacity = 0;
    while (pow(2, list->capacity) < newLen)
      list->capacity++;
      
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(char*));//réalloue un pointeur de la nouvelle taille
    
    if (tmp == NULL)
      quitError(ERROR_REALLOC);
    
    list->tab = tmp;
    
  }
  
  
  if (newLen > list->len)//on initialise les nouveaux elements si nouveaux elements il y a
  {
    for (unsigned i = list->len ; i < newLen ; i++)
	  list->tab[i]=(char*)0;
  }
  
  list->len=newLen;
  
}



void strlist_remove(strlist* list,unsigned index, _Bool freeElement)//indiquer si il faut libérer l'élément avant de le supprimer
{
  
  if (index >= list->len)
  {
    quitError(ERROR_OUT_OF_RANGE);
  }
  
  if (freeElement)
      err_free(list->tab[index]);
  
  for (unsigned i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i+1];
    
  char **tmp;
  
  if (pow(2, list->capacity-1)==list->len-1)
  {
    list->capacity--;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(char*));//réalloue un nouveau pointeur de la bonne taille
    list->tab = tmp;
  }
  
  list->len--;//décrémentation de la longueur
  
}






unsigned strlist_count(strlist* list, char* chaine)
{
  unsigned count=0;
  
  for (unsigned i = 0 ; i < list->len ; i++)
  {
    if (strcmp(list->tab[i],chaine)==0)
    {
      count++;
    }
  }
  
  return count;
}


_Bool strlist_inList(strlist* list, char* chaine)
{
  for (unsigned i=0; i<list->len; i++)
  {
    if (strcmp(list->tab[i],chaine)==0)
    {
      return 1;
    }
    
  }
  return 0;
}




int strlist_index(strlist* list, char* chaine)
{
  for (unsigned i=0; i<list->len; i++)
  {
    if (strcmp(chaine,list->tab[i])==0)
    {
      return i;
    }
    
  }
  quitError(ERROR_STR_NOT_IN_TYPE_LIST);
  return 1;
}





void strlist_insert(strlist* list,char* chaine, unsigned index)//ajoute un élément à la place indiquée
{
  if (index > list->len)
  {
    quitError(ERROR_OUT_OF_RANGE);
  }
  
  char** tmp;
  if (pow(2, list->capacity)==list->len)
  {
    list->capacity++;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(char*));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }
  
  for (unsigned i = list->len ; i > index; i--)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i-1];
  
  list->tab[index]=chaine;

  list->len++;//incrémente la longueur
}


