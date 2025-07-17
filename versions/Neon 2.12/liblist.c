#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


#include "headers.c"


extern int CODE_ERROR;



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



extern int error;




// fonctions de copies. Servent à "déplacer" un tableau de la pile vers le tas





void strlist_copy(strlist* list, const char** tab, int len)
{
    // copie du tableau dans le tas
    char** res = err_malloc(sizeof(char*)*len);
    for (int i = 0 ; i < len ; i++)
    {
        res[i] = strdup(tab[i]);
    }

    list->tab = res;
    list->len = len;
    return;
}







void intlist_copy(intlist* list, const int* tab, int len)
{
    // copie du tableau dans le tas
    int* res = err_malloc(sizeof(int)*len);
    for (int i = 0 ; i < len ; i++)
    {
        res[i] = tab[i];
    }

    list->tab = res;
    list->len = len;
    return;
}








void listlist_copy(listlist* list, const intlist* tab, int len)
{
    // copie du tableau dans le tas
    intlist* res = err_malloc(sizeof(strlist)*len);
    intlist temp;
    for (int i = 0 ; i < len ; i++)
    {
        // ---- copie -----
        int* res2 = err_malloc(sizeof(char*)*tab[i].len);
        for (int j = 0 ; j < tab[i].len ; j++)
        {
            res2[j] = tab[i].tab[j];
        }
        
        temp.tab = res2;
        temp.len = tab[i].len;
        // ---- fin copie ----

        
        res[i].tab = temp.tab;
        res[i].len = temp.len;
    }

    list->tab = res;
    list->len = len;
    return;
}





//listlist

listlist listlist_create(int len)// crée une liste de pointeurs
{
  listlist list;//crée la structure
  
  list.capacity = 0;
  while (pow(2, list.capacity) < len)
    list.capacity++;
  
  list.tab=err_malloc(pow(2, list.capacity)*sizeof(intlist));//initialise le tableau de longueur len avec de zéros
  
  err_memset(list.tab,0,len);
  list.len=len;//initialise la bonne longueur
  return list;//retourne la structure
}



void listlist_append(listlist* list, intlist* ptr)//ajoute un élément à la fin de la liste
{
  intlist* tmp;

  if (pow(2, list->capacity)==list->len)
  {
    list->capacity++;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(intlist));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }

  
  
  list->tab[list->len]=*ptr;//affecte nombre au dernier élément
  list->len++;//incrémente la longueur
}




void listlist_remove(listlist* list,int index)//indiquer si il faut libérer l'élément avant de le supprimer
{


  if (index >= list->len)
  {
    CODE_ERROR = 38;
    return ;
  }
  
  err_free(list->tab[index].tab);
  
  for (int i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i+1];
    
  intlist* tmp;
  
  if (pow(2, list->capacity-1)==list->len-1)
  {
    list->capacity--;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(intlist));//réalloue un nouveau pointeur de la bonne taille
    list->tab = tmp;
  }
  
  
  list->len--;
  
}



void listlist_aff(listlist* liste)
{
  for (int i=0;i<liste->len;i++)
  {
    intlist_aff(&liste->tab[i]);
  }
  
}



void listlist_destroy(listlist* list)
{
  for (int i=0;i<list->len;i++)
  {
    err_free(list->tab[i].tab);
  }
  err_free(list->tab);
}







ptrlist* ptrlist_create(void)
{
    ptrlist* l = err_malloc(sizeof(ptrlist));

    l->tete = NULL;
    l->queue = NULL;
    return l;
}





void ptrlist_append(ptrlist* q, void* t)
{
    
    if (q->tete == NULL)
    {
        q->tete = t;
    }
    else
    {
        ptrlist* chainon = err_malloc(sizeof(ptrlist));//on crée une copie du premier chaînon
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
    {
        printString("[]");
        newLine();
    }
    else if (l->queue == NULL)
    {
        printString("[");
        printInt((long int)l->tete);
        printString("]");
        newLine();
    }
    
    else
    {
        printString("[");
        ptrlist* ptr = l;
        for (int i = 1; ptr->queue != NULL; i++)
        {
            printInt((long int)ptr->tete);
            printString(", ");
            ptr = ptr->queue;
        }
        printInt((long int)ptr->tete);
        printString("]");
        newLine();
    }
    
    return;
}






int ptrlist_destroy(ptrlist* l, _Bool freeElements, _Bool freeTab)
{
    int i=0;
    if (l->queue == NULL && (l->tete == NULL || !freeElements))  // on ne libère pas l s'il ne faut pas
    {
        if (freeTab)
            free(l);
    }
    else if (l->queue == NULL && freeElements)
    {
        free(l->tete);
        
        if (freeTab) // on ne libère pas l s'il ne faut pas
            free(l);
        
        // printString("Liberation du pointeur %p  (1)\n", l->tete);
        i=1;
    }
    
    else
    {
        ptrlist* ptr = l;
        ptrlist* tmp;
        for (i = 0; ptr != NULL; i++)
        {
            if (freeElements)
            {
                free(ptr->tete);
                // printString("Liberation du pointeur %p  (2)\n", ptr->tete);
            }
            tmp = ptr->queue;
            
            if (freeTab) // on ne libère pas l s'il ne faut pas
                free(ptr);
            
            ptr=tmp;
        }
    }
    
    return i;
}



void ptrlist_replace(ptrlist* liste, void* aRemplacer, void* nvlleValeur)
{
    ptrlist* ptr = liste;
    for (int i = 1; ptr->queue != NULL && ptr->tete != aRemplacer; i++)
    {
        ptr = ptr->queue;
    }
    
    if (ptr->queue == NULL && ptr->tete != aRemplacer)
    {
        CODE_ERROR = 65;
        return ;
    }
    
    ptr->tete = nvlleValeur;
    
    return ;
}



int ptrlist_index(ptrlist* liste, void* element)
{
    ptrlist* ptr = liste;
    int i=0;
    for (i = 0; ptr->queue != NULL && ptr->tete != element; i++)
    {
        ptr = ptr->queue;
    }
    
    if (ptr->queue == NULL && ptr->tete != element)
        return -1;
    
    
    return i;
}





void ptrlist_remove(ptrlist* list, void* l, _Bool error)
{
    ptrlist* ptr = list;
    ptrlist* ptr2 = NULL;
    

    
    for (int i = 1; ptr->queue != NULL && ptr->tete != l; i++)
    {
        ptr2 = ptr;
        ptr = ptr->queue;
    }
    
    
    if (ptr->queue == NULL && ptr->tete != l && error)
    {
        CODE_ERROR = 65;
        return ;
    }
    else if (ptr->queue == NULL && ptr->tete != l)
        return;
    
    
    else if (ptr->queue == NULL && ptr2 == NULL)// on est le dernier élément de la liste
        list->tete = NULL;
    
    else if (ptr2 == NULL)// si on est au debut de la liste, donc ptr2 == NULL
    {
        
        ptrlist* ptrALiberer = ptr->queue; // sauvegarder de ptr->queue pour pouvoir le libérer après
        
        ptr->tete = (ptr->queue)->tete;
        ptr->queue = (ptr->queue)->queue;
        
        free(ptrALiberer);
    }
    
    else //on est au milieu
    {
        ptr2->queue = ptr->queue;
        free(ptr);
    }
    
}





int ptrlist_len(ptrlist* l)
{
    if (l->tete == NULL)
        return 0;
    
    ptrlist* ptr = l;
    int i;
    for (i = 0; ptr != NULL; i++)
        ptr = ptr->queue;
    return i;
}




 
 
//intlist

intlist intlist_create(int len)// crée une liste d'entiers
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
    printString("[]");
    newLine();
  }
  else
  {
    printString("[");
    for ( int i = 0 ; i < list->len -1 ; i++)//affiche les éléments du premier à l'avant-dernier
    {
      printInt(list->tab[i]);
      printString(", ");
    }
    printInt(list->tab[list->len-1]);
    printString("]");//affiche le dernier élément
    newLine();
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



void intlist_resize(intlist* list, int newLen)//redimensionne la liste avec la nouvelle longueur
{
  int* tmp;
  if (newLen > pow(2, list->capacity) || newLen <= pow(2, list->capacity-1)) // on détermine la nouvelle capacité
  {
    list->capacity = 0;
    while (pow(2, list->capacity) < newLen)
      list->capacity++;
      
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(int));//réalloue un pointeur de la nouvelle taille
    
    if (tmp == NULL)
    {
        CODE_ERROR = 12;
        return;
    }
    
    list->tab = tmp;
    
  }
  
  if (newLen > list->len)//initialisation des nouveaux éléments à 0 si nouveaux éléments il y a
  {
    for (int i = list->len ; i < newLen ; i++)
	  list->tab[i]=0;
  }
  
  list->len=newLen;//modification de la longueur
  
}





void intlist_remove(intlist* list,int index)//supprime un élément de la liste
{
  if (index >= list->len)
  {
    CODE_ERROR = 38;
    return ;
  }
  
  for (int i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
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



int intlist_count(intlist* list, int nb)
{
  int count=0;
  
  for (int i = 0 ; i < list->len ; i++)
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
  for (int i=0; i<list->len; i++)
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
  for (int i=0; i<list->len; i++)
  {
    if (nombre==list->tab[i])
    {
      return i;
    }
    
  }
  CODE_ERROR = 18;
  return 1;
}



void intlist_insert(intlist* list,int nombre, int index)//ajoute un élément à la place indiquée
{
  if (index > list->len)
  {
    CODE_ERROR = 38;
    return ;
  }
  
  int *tmp;
  if (pow(2, list->capacity)==list->len)
  {
    list->capacity++;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(int));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }
  
  for (int i = list->len ; i > index; i--)//décale tous les éléments d’une case en + jusqu’a la place a inserer
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
    for (int i=0 ; i<list->len;i++)
    {
      if (list->tab[i] > max)
        max=list->tab[i];
    }
    return max;
  }
  else
  {
      CODE_ERROR = 38;
      return EXIT_FAILURE;
  }
  return EXIT_FAILURE;
}









//strlist


strlist* strlist_create(int len)
{
  strlist* list = err_malloc(sizeof(strlist));
  
  list->capacity = 0;
  
  while (pow(2, list->capacity) < len)
    list->capacity++;
  
  list->tab=err_malloc(pow(2, list->capacity)*sizeof(char*));
  
  err_memset(list->tab,0,len);
  list->len=len;
  return list;
}





void strlist_aff(strlist* list)
{
  if (list->len == 0)
  {
    printString("[]\n");
  }
  else
  {
    printString("[");
    char* tmp;
    for ( int i = 0 ; i < list->len -1 ; i++)
    {
        tmp = list->tab[i];
        //tmp = traitementStringInverse(list->tab[i]);
        
        printString("\"");
        if (tmp != NULL)
            printString(tmp);
        printString("\", ");
        //err_free(tmp);
    }

    printString("\"");
    if (list->tab[list->len-1] != NULL)
        printString(list->tab[list->len-1]);
    printString("\"]");
    newLine();
    
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




void strlist_destroy(strlist* list, _Bool bo)
{
  for (int i=0 ; i < list->len;i++)
  {
    err_free(list->tab[i]);
  }
  err_free(list->tab);
  if (bo)
    err_free(list);
}




void strlist_resize(strlist* list, int newLen, _Bool freeElement)
{
  
  //il faut au préalable libérer les éléments qui vont être tronqués si la nouvelle longueur est plus petite
  if (newLen < list->len && freeElement)
  {
      for (int i = newLen ; i < list->len ; i++)
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
    {
        CODE_ERROR = 12;
        return ;
    }
    
    list->tab = tmp;
    
  }
  
  
  if (newLen > list->len)//on initialise les nouveaux elements si nouveaux elements il y a
  {
    for (int i = list->len ; i < newLen ; i++)
	  list->tab[i]=(char*)0;
  }
  
  list->len=newLen;
  
}



void strlist_remove(strlist* list,int index, _Bool freeElement)//indiquer si il faut libérer l'élément avant de le supprimer
{
  
  if (index >= list->len)
  {
    CODE_ERROR = 38;
    return ;
  }
  
  if (freeElement)
      err_free(list->tab[index]);
  
  for (int i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
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






int strlist_count(strlist* list, char* chaine)
{
  int count=0;
  
  for (int i = 0 ; i < list->len ; i++)
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
  for (int i=0; i<list->len; i++)
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

  for (int i=0; i<list->len; i++)
  {
    if (strcmp(chaine,list->tab[i])==0)
    {
      return i;
    }
    
  }
  CODE_ERROR = 18;
  return -1;
}





void strlist_insert(strlist* list,char* chaine, int index)//ajoute un élément à la place indiquée
{
  if (index > list->len)
  {
    CODE_ERROR = 38;
    return ;
  }
  
  char** tmp;
  if (pow(2, list->capacity)==list->len)
  {
    list->capacity++;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(char*));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }
  
  for (int i = list->len ; i > index; i--)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i-1];
  
  list->tab[index]=chaine;

  list->len++;//incrémente la longueur
}


