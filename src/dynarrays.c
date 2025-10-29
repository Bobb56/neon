#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "headers/neonio.h"
#include "headers/dynarrays.h"
#include "headers/neon.h"
#include "headers/errors.h"

// fonctions de copies. Servent à "déplacer" un tableau de la pile vers le tas


void printToken(Token tok) {
  char sov = stringize(tok);
  printString(tok.debut);
  unstringize(tok, sov);
}

char stringize(Token tok) {
  char sov = tok.debut[tok.len];
  tok.debut[tok.len] = '\0';
  return sov;
}

void unstringize(Token tok, char sov) {
  tok.debut[tok.len] = sov;
}


char* tokdup(Token tok) {
  return strndup(tok.debut, tok.len);
}

bool tokeq(Token token, char* string) {
  return strncmp(token.debut, string, token.len) == 0 && string[token.len] == '\0';
}


toklist* toklist_create(int len)
{
  toklist* list = neon_malloc(sizeof(toklist));
  
  list->capacity = 0;
  list->source_string = NULL;
  
  while ((1<<list->capacity) < len)
    list->capacity++;
  
  list->tab=neon_malloc((1<<list->capacity)*sizeof(Token));
  
  memset(list->tab,0,len);
  list->len=len;
  return list;
}





void toklist_aff(toklist* list)
{
  if (list->len == 0)
  {
    printString("[]\n");
  }
  else
  {
    printString("[");
    Token tmp;
    for (int i = 0 ; i < list->len -1 ; i++)
    {
        tmp = list->tab[i];
        //tmp = traitementStringInverse(list->tab[i]);
        
        printString("\"");
        if (tmp.debut != NULL)
            printToken(tmp);
        printString("\", ");
    }

    printString("\"");
    if (list->tab[list->len-1].debut != NULL)
        printToken(list->tab[list->len-1]);
    printString("\"]");
    newLine();
    
  }
}




void toklist_append(toklist* list, Token chaine)
{
  Token* tmp;
  if (1<<list->capacity == list->len)
  {
    list->capacity++;
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(Token));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }
  
  list->tab[list->len]=chaine;
  list->len++;
}




void toklist_destroy(toklist* list)
{
  if (list->source_string != NULL)
    neon_free(list->source_string);
  
  neon_free(list->tab);
  neon_free(list);
}




void toklist_resize(toklist* list, int newLen)
{
  Token* tmp;
  
  if (newLen > (1<<list->capacity) || newLen <= 1 << (list->capacity - 1)) // on détermine la nouvelle capacité
  {
    list->capacity = 0;
    while ((1<<list->capacity) < newLen)
      list->capacity++;
      
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(Token));//réalloue un pointeur de la nouvelle taille
    
    if (tmp == NULL)
    {
        global_env->CODE_ERROR = 12;
        return ;
    }
    
    list->tab = tmp;
    
  }
  
  list->len=newLen;
  
}



void toklist_remove(toklist* list, int index)
{
  
  if (index >= list->len)
  {
    global_env->CODE_ERROR = 38;
    return ;
  }
  
  
  for (int i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i+1];
    
  Token* tmp;
  
  if (1 << (list->capacity - 1) == list->len-1)
  {
    list->capacity--;
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(Token));//réalloue un nouveau pointeur de la bonne taille
    list->tab = tmp;
  }
  
  list->len--;//décrémentation de la longueur
  
}






int toklist_count(toklist* list, char* chaine)
{
  int count=0;
  
  for (int i = 0 ; i < list->len ; i++)
  {
    if (tokeq(list->tab[i], chaine))
    {
      count++;
    }
  }
  
  return count;
}


bool strlist_token_inList(strlist* list, Token chaine)
{
  for (int i=0; i<list->len; i++)
  {
    if (tokeq(chaine, list->tab[i]))
    {
      return true;
    }
    
  }
  return false;
}



int toklist_index(toklist* list, char* chaine)
{

  for (int i=0; i<list->len; i++)
  {
    if (tokeq(list->tab[i], chaine))
    {
      return i;
    }
    
  }
  global_env->CODE_ERROR = 18;
  return -1;
}


int strlist_token_index(strlist* list, Token chaine)
{
  for (int i=0; i<list->len; i++)
  {
    if (tokeq(chaine, list->tab[i]))
    {
      return i;
    }
  }
  global_env->CODE_ERROR = 18;
  return -1;
}



void toklist_insert(toklist* list, Token chaine, int index)//ajoute un élément à la place indiquée
{
  if (index > list->len)
  {
    global_env->CODE_ERROR = 38;
    return ;
  }
  
  Token* tmp;
  if ((1<<list->capacity)==list->len)
  {
    list->capacity++;
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(Token));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }
  
  for (int i = list->len ; i > index; i--)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i-1];
  
  list->tab[index]=chaine;

  list->len++;//incrémente la longueur
}




// ================== PTRLIST ==================



ptrlist* ptrlist_create(void)
{
    ptrlist* l = neon_malloc(sizeof(ptrlist));

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
        ptrlist* chainon = neon_malloc(sizeof(ptrlist));//on crée une copie du premier chaînon
        chainon->tete = q->tete;
        chainon->queue = q->queue;
        
        q->queue = chainon;//puis on fait pointer le premier élément de la liste sur le nouveau chaînon

        q->tete = t;
    }
    return ;
}



/*
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
        printInt((intptr_t)l->tete);
        printString("]");
        newLine();
    }
    
    else
    {
        printString("[");
        ptrlist* ptr = l;
        for (int i = 1; ptr->queue != NULL; i++)
        {
            printInt((intptr_t)ptr->tete);
            printString(", ");
            ptr = ptr->queue;
        }
        printInt((intptr_t)ptr->tete);
        printString("]");
        newLine();
    }
    
    return;
}

*/



int ptrlist_destroy(ptrlist* l, bool freeElements, bool freeTab)
{
    int i=0;
    if (l->queue == NULL && (l->tete == NULL || !freeElements))  // on ne libère pas l s'il ne faut pas
    {
        if (freeTab)
            neon_free(l);
    }
    else if (l->queue == NULL && freeElements)
    {
        neon_free(l->tete);
        
        if (freeTab) // on ne libère pas l s'il ne faut pas
            neon_free(l);
        
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
                neon_free(ptr->tete);
                // printString("Liberation du pointeur %p  (2)\n", ptr->tete);
            }
            tmp = ptr->queue;
            
            if (freeTab) // on ne libère pas l s'il ne faut pas
                neon_free(ptr);
            
            ptr=tmp;
        }
    }
    
    return i;
}


/*
void ptrlist_replace(ptrlist* liste, void* aRemplacer, void* nvlleValeur)
{
    ptrlist* ptr = liste;
    for (int i = 1; ptr->queue != NULL && ptr->tete != aRemplacer; i++)
    {
        ptr = ptr->queue;
    }
    
    if (ptr->queue == NULL && ptr->tete != aRemplacer)
    {
        global_env->CODE_ERROR = 65;
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


void ptrlist_direct_remove(ptrlist* list, ptrlist* ptr, ptrlist* prev) {
    if (prev == NULL && list->queue == NULL) { // ptr est en tête de liste et la liste contient un seul élément
        list->tete = NULL;
    }
    else if (prev == NULL && list->queue != NULL) {
        list->tete = list->queue->tete; // on met le deuxième élément dans le premier
        ptrlist* sov = list->queue;
        list->queue = sov->queue; // on raccorde le premier chainon au troisième chainon
        neon_free(sov);
    }
    else if (ptr->queue == NULL) { // ptr est à la fin de la liste
        prev->queue = NULL;
        neon_free(ptr);
    }
    else {
        prev->queue = ptr->queue;
        neon_free(ptr);
    }
}

*/


void ptrlist_remove(ptrlist* list, void* l, bool error)
{
    ptrlist* ptr = list;
    ptrlist* ptr2 = NULL;
    

    // cherche l'élément l dans la liste
    for (int i = 1; ptr->queue != NULL && ptr->tete != l; i++)
    {
        ptr2 = ptr;
        ptr = ptr->queue;
    }
    
    // l'élément n'a pas été trouvé
    if (ptr->queue == NULL && ptr->tete != l && error)
    {
        global_env->CODE_ERROR = 65;
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
        
        neon_free(ptrALiberer);
    }
    
    else //on est au milieu
    {
        ptr2->queue = ptr->queue;
        neon_free(ptr);
    }
    
}

/*
bool ptrlist_inList(ptrlist* l, void* el) {
  return ptrlist_index(l, el) != -1;
}
*/



void* ptrlist_pop(ptrlist* list)
{
    void* val = list->tete;
    void* sov_chainon = list->queue;

    if (sov_chainon == NULL) {
      list->tete = NULL;
    }
    else {
      list->tete = list->queue->tete;
      list->queue = list->queue->queue;
      neon_free(sov_chainon);
    }
    return val;
}




/*
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
*/

bool ptrlist_isEmpty(ptrlist* l) {
  return l->queue == NULL && l->tete == NULL;
}


//////////////////// INTLIST ////////////////////



intlist intlist_create(int len)// crée une liste d'entiers
{
  intlist list;//crée la structure
  
  list.capacity = 0;
  
  while ((1<<list.capacity) < len)
    list.capacity++;
  
  list.tab=neon_malloc((1<<list.capacity)*sizeof(int));//initialise le tableau de longueur len avec de zéros
  
  memset(list.tab,0,len);
  
  list.len=len;//initialise la bonne longueur
  return list;//retourne la structure
}



/*
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
*/



void intlist_append(intlist* list,int nombre)//ajoute un élément à la fin de la liste
{
    
  int *tmp;
  if ((1<<list->capacity)==list->len)
  {
    list->capacity++;
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(int));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }

  list->tab[list->len]=nombre;//affecte nombre au dernier élément
  list->len++;//incrémente la longueur
}


/*
void intlist_resize(intlist* list, int newLen)//redimensionne la liste avec la nouvelle longueur
{
  int* tmp;
  if (newLen > (1<<list->capacity) || newLen <= 1 << (list->capacity - 1)) // on détermine la nouvelle capacité
  {
    list->capacity = 0;
    while ((1<<list->capacity) < newLen)
      list->capacity++;
      
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(int));//réalloue un pointeur de la nouvelle taille
    
    if (tmp == NULL)
    {
        global_env->CODE_ERROR = 12;
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
*/




void intlist_remove(intlist* list,int index)//supprime un élément de la liste
{
  if (index >= list->len)
  {
    global_env->CODE_ERROR = 38;
    return ;
  }
  
  for (int i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i+1];
    
  int *tmp;
  
  if (1 << (list->capacity - 1) == list->len-1)
  {
    list->capacity--;
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(int));//réalloue un nouveau pointeur de la bonne taille
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


bool intlist_inList(intlist* list, int nombre)
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
  global_env->CODE_ERROR = 18;
  return 1;
}


/*
void intlist_insert(intlist* list,int nombre, int index)//ajoute un élément à la place indiquée
{
  if (index > list->len)
  {
    global_env->CODE_ERROR = 38;
    return ;
  }
  
  int *tmp;
  if ((1<<list->capacity)==list->len)
  {
    list->capacity++;
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(int));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }
  
  for (int i = list->len ; i > index; i--)//décale tous les éléments d’une case en + jusqu’a la place a inserer
    list->tab[i]=list->tab[i-1];
  
  list->tab[index]=nombre;

  list->len++;//incrémente la longueur
}
*/


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
      global_env->CODE_ERROR = 38;
      return EXIT_FAILURE;
  }
  return EXIT_FAILURE;
}



////////////////// INTPTRLIST /////////////////////:
intptrlist intptrlist_create(int len)// crée une liste d'entiers
{
  intptrlist list;//crée la structure
  
  list.capacity = 0;
  
  while ((1<<list.capacity) < len)
    list.capacity++;
  
  list.tab=neon_malloc((1<<list.capacity)*sizeof(int*));//initialise le tableau de longueur len avec de zéros
  
  memset(list.tab,0,len);
  
  list.len=len;//initialise la bonne longueur
  return list;//retourne la structure
}


void intptrlist_append(intptrlist* list, int* ptr)//ajoute un élément à la fin de la liste
{
    
  int **tmp;
  if ((1<<list->capacity)==list->len)
  {
    list->capacity++;
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(int*));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }

  list->tab[list->len] = ptr;//affecte nombre au dernier élément
  list->len++;//incrémente la longueur
}


void intptrlist_destroy(intptrlist* list) {
  for (int i = 0 ; i < list->len ; i++) {
    if (list->tab[i] != NULL)
      neon_free(list->tab[i]);
  }
  neon_free(list->tab);
}


//strlist


strlist* strlist_create(int len)
{
  strlist* list = neon_malloc(sizeof(strlist));
  
  list->capacity = 0;
  
  while ((1<<list->capacity) < len)
    list->capacity++;
  
  list->tab=neon_malloc((1<<list->capacity)*sizeof(char*));
  
  memset(list->tab,0,len);
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
        //neon_free(tmp);
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
  if (1<<list->capacity == list->len)
  {
    list->capacity++;
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(char*));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }
    
  list->tab[list->len]=chaine;
  list->len++;
}




void strlist_destroy(strlist* list, bool bo)
{
  for (int i=0 ; i < list->len;i++)
  {
    neon_free(list->tab[i]);
  }
  neon_free(list->tab);
  if (bo)
    neon_free(list);
}



/*
void strlist_resize(strlist* list, int newLen, bool freeElement)
{
  
  //il faut au préalable libérer les éléments qui vont être tronqués si la nouvelle longueur est plus petite
  if (newLen < list->len && freeElement)
  {
      for (int i = newLen ; i < list->len ; i++)
	    neon_free(list->tab[i]);
  }
  
  
  char** tmp;
  
  if (newLen > (1<<list->capacity) || newLen <= 1 << (list->capacity - 1)) // on détermine la nouvelle capacité
  {
    list->capacity = 0;
    while ((1<<list->capacity) < newLen)
      list->capacity++;
      
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(char*));//réalloue un pointeur de la nouvelle taille
    
    if (tmp == NULL)
    {
        global_env->CODE_ERROR = 12;
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



void strlist_remove(strlist* list,int index, bool freeElement)//indiquer si il faut libérer l'élément avant de le supprimer
{
  
  if (index >= list->len)
  {
    global_env->CODE_ERROR = 38;
    return ;
  }
  
  if (freeElement)
      neon_free(list->tab[index]);
  
  for (int i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i+1];
    
  char **tmp;
  
  if (1 << (list->capacity - 1) == list->len-1)
  {
    list->capacity--;
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(char*));//réalloue un nouveau pointeur de la bonne taille
    list->tab = tmp;
  }
  
  list->len--;//décrémentation de la longueur
  
}
*/





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


bool strlist_inList(strlist* list, char* chaine)
{
  for (int i=0; i<list->len; i++)
  {
    if (strcmp(list->tab[i],chaine)==0)
    {
      return true;
    }
    
  }
  return false;
}

bool strlist_inList_sub(strlist* list, char* chaine, int debut, int fin)
{
  char value = chaine[fin];
  chaine[fin] = '\0';

  for (int i=0; i<list->len; i++)
  {
    if (strcmp(list->tab[i],chaine + debut)==0)
    {
      chaine[fin] = value;
      return true;
    }
  }
  chaine[fin] = value;
  return false;
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
  global_env->CODE_ERROR = 18;
  return -1;
}




/*
void strlist_insert(strlist* list,char* chaine, int index)//ajoute un élément à la place indiquée
{
  if (index > list->len)
  {
    global_env->CODE_ERROR = 38;
    return ;
  }
  
  char** tmp;
  if ((1<<list->capacity)==list->len)
  {
    list->capacity++;
    tmp = neon_realloc(list->tab, (1<<list->capacity)*sizeof(char*));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }
  
  for (int i = list->len ; i > index; i--)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i-1];
  
  list->tab[index]=chaine;

  list->len++;//incrémente la longueur
}


*/