// bibliothèque générale de structures des données Neon


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "headers.h"



/*
Correspondances NeObject data void* avec le type du NeObject

number -> double*
bool -> _Bool*
string -> char*
list / templist -> NeList*




TESTS :
    
    // création de 3 types de base
    NeObject* neo1 = neo_str_create(copy("coucou"));
    NeObject* neo2 = neo_nb_create(12.7);
    NeObject* neo3 = neo_bool_create(strToBool("True"));
    
    //creation de 3 autres types de base
    NeObject* neo4 = neo_str_create(copy("chat"));
    NeObject* neo5 = neo_nb_create(-1.234);
    NeObject* neo6 = neo_bool_create(strToBool("False"));
    
    //ajout des 3 premiers types de base dans une liste
    NeObject* neo7 = neo_list_create(0);
    neo_list_append(neo7, neo1);
    neo_list_append(neo7, neo2);
    neo_list_append(neo7, neo3);
    
    //ajout des 3 autres types de base dans une deuxième liste
    NeObject* neo8 = neo_list_create(0);
    neo_list_append(neo8, neo4);
    neo_list_append(neo8, neo5);
    neo_list_append(neo8, neo6);
    
    // création d'une troisième liste dans laquelle on met les deux précédentes
    NeObject* neo9 = neo_list_create(0);
    neo_list_append(neo9, neo7);
    neo_list_append(neo9, neo8);
    
    
    
    
    neobject_aff(neo9);
    printf("\n");
    neobject_aff(neo_list_nth(neo9,0));
    printf("\n");
    neobject_aff(neo_list_nth(neo9,1));
    
    printf("\n");
    
    neobject_destroy(neo9);


*/




NeObject* neobject_create(char type)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = NULL;
    neo->type = type;
    
    return neo;
}





NeObject* neo_nb_create(double number)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = err_malloc(sizeof(double*));
    neo->type = TYPE_NUMBER[0];
    *((double*)neo->data) = number;
    return neo;
}



NeObject* neo_bool_create(_Bool boolean)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = err_malloc(sizeof(_Bool*));
    neo->type = TYPE_BOOL[0];
    *((_Bool*)neo->data) = boolean;
    return neo;
}





NeObject* neo_str_create(char* string) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = string;
    neo->type = TYPE_STRING[0];
    return neo;
}



NeObject* neo_list_create(unsigned len)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = nelist_create(len);
    neo->type = TYPE_LIST[0];
    return neo;
}










void neobject_destroy(NeObject* neo)
{
    if (neo->type == 'l' || neo->type == 't')
    {
        nelist_destroy((NeList*)neo->data);
    }
    else
        err_free(neo->data);
    
    err_free(neo);
    
}



void neobject_aff(NeObject* neo)
{
    
    if (neo->type == TYPE_NUMBER[0])
    {
        printf("%lf", *((double*)neo->data));
    }
    else if (neo->type == TYPE_BOOL[0])
    {
        if (*((_Bool*)neo->data))
            printf("True");
        else
            printf("False");
    }
    else if (neo->type == TYPE_STRING[0])
        printf("\"%s\"", (char*)neo->data);
    else if (neo->type == TYPE_LIST[0] || neo->type == TYPE_TEMPLIST[0])
    {
        nelist_aff((NeList*)neo->data);
    }

}



void neo_list_append(NeObject* neo, NeObject* ptr)
{
    nelist_append((NeList*)neo->data, ptr);
    return;
}


NeObject* neo_list_nth(NeObject* neo, unsigned index)
{
  return ((NeList*)neo->data)->tab[index];
}



//nelist

NeList* nelist_create(unsigned len)
{
  NeList* list = err_malloc(sizeof(NeList));//crée la structure
  
  list->capacity = 0;
  while (pow(2, list->capacity) < len)
    list->capacity++;
  
  list->tab=err_malloc(pow(2, list->capacity)*sizeof(NeObject*));//initialise le tableau de longueur len avec de zéros
  
  err_memset(list->tab,0,len);
  list->len=len;//initialise la bonne longueur
  return list;//retourne la structure
}



void nelist_append(NeList* list, NeObject* ptr)//ajoute un élément à la fin de la liste
{
  NeObject** tmp;

  if (pow(2, list->capacity)==list->len)
  {
    list->capacity++;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(NeObject*));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }

  
  
  list->tab[list->len]=ptr;//affecte ptr au dernier élément
  list->len++;//incrémente la longueur
  
  
}




void nelist_remove(NeList* list,unsigned index)
{


  if (index >= list->len)
  {
    quitError(ERROR_OUT_OF_RANGE);
  }
  
  
  
  neobject_destroy(list->tab[index]);
  
  for (unsigned i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i+1];
    
  NeObject** tmp;
  
  if (pow(2, list->capacity-1)==list->len-1)
  {
    list->capacity--;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(NeObject*));//réalloue un nouveau pointeur de la bonne taille
    list->tab = tmp;
  }
  
  
  list->len--;
  
}



void nelist_aff(NeList* liste)
{
  printf("[");
  for (unsigned i=0;i<liste->len-1;i++)
  {
    neobject_aff(liste->tab[i]);
    printf(", ");
  }
  neobject_aff(liste->tab[liste->len-1]);
  printf("]");
  
}



void nelist_destroy(NeList* list)
{
  for (unsigned i=0;i<list->len;i++)
  {
    neobject_destroy(list->tab[i]);
  }
  err_free(list->tab);
  err_free(list);
}

