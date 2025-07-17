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





char* _print_(strlist* args, intlist* types, int* typeRetour)
{
  for (unsigned i=0 ; i < args->len ; i++)
  {
    if (types->tab[i] == TYPE_NUMBER[0] || types->tab[i] == TYPE_BOOL[0])
    {
      printf("%s",args->tab[i]);
    }
    
    if (types->tab[i] == TYPE_STRING[0])
    {
      char* aff=sub(args->tab[i],1,strlen(args->tab[i])-1);
      printf("%s",aff);
      err_free(aff);
    }



    
    if (types->tab[i]==TYPE_TEMPLIST[0] || types->tab[i]==TYPE_LIST[0])
    {
      unsigned indexListe=strlist_index(&NOMS, args->tab[i]);
      strlist list = LISTES.tab[ADRESSES.tab[indexListe]];
      if (list.len == 0)
      {
        printf("[]\n");
      }
      else
      {
        printf("[");
        for ( unsigned i = 0 ; i < list.len -1 ; i++)
          printf("%s, ",list.tab[i]);//affiche l'élément
  
  
	    printf("%s]",list.tab[list.len-1]);//dernier élément
      }
    }




    
    printf(" ");
    
  }
  printf("\n");
  
  *typeRetour=TYPE_NONE[0];
  
  return copy("None");
}



char* _input_(strlist* args, intlist* types, int* typeRetour)
{

  if (types->tab[0]==TYPE_STRING[0])
  {
    char* nouvArg=sub(args->tab[0],1,strlen(args->tab[0])-1);
    err_free(args->tab[0]);
    args->tab[0]=nouvArg;
  }
  
  char *entree=input(args->tab[0]);
  *typeRetour=TYPE_STRING[0];
  
  char* entree2=addStr("\"",entree);
  char* retour=addStr(entree2,"\"");
  
  err_free(entree);
  err_free(entree2);
  
  
  return retour;
  
}




char* _nbr_(strlist* args, intlist* types, int* typeRetour)
{
  *typeRetour=TYPE_NUMBER[0];
  char* retour;
  
  if (args->tab[0][0]=='"' && args->tab[0][strlen(args->tab[0])-1]=='"')
  {
    retour=sub(args->tab[0],1,strlen(args->tab[0])-1);
  }
  else
  {
    retour=copy(args->tab[0]);
  }
  double nb=strToNb(retour);
  
  
  return retour;
}



char* _str_(strlist* args, intlist* types, int* typeRetour)
{
  /*
  printf("Cette fonction n'est pas prete pour le moment.\n");
  *typeRetour = TYPE_NONE[0];
  return copy("None");
  */
  
  *typeRetour=TYPE_STRING[0];
  
  if (types->tab[0] == TYPE_NUMBER[0])
  {
      char* tmp1=addStr("\"",args->tab[0]);
      char* tmp2=addStr(tmp1,"\"");
      err_free(tmp1);
      return tmp2;
  }
  else if (types->tab[0] == TYPE_LIST[0])
  {
      unsigned index = strlist_index(&NOMS, args->tab[0]);
      index = ADRESSES.tab[index];// index désigne l'index de la liste dans LISTES
      
      strlist* liste = &LISTES.tab[index];
      
      unsigned len = 0;
      for (unsigned i=0 ; i < liste->len ; i++)
      {
          len += strlen(liste->tab[i]);
      }
      char* retour = err_malloc((len + 2*liste->len + 3)*sizeof(char));
      
      retour[0] = '"';retour[1] = '[';
      index = 2;
      for (unsigned i=0 ; i < liste->len ; i++)
      {
          for (unsigned j=0 ; liste->tab[i][j] != '\0' ; j++)
          {
              retour[index] = liste->tab[i][j];
              index ++;
          }
          retour[index] = ',' ; retour[index+1] = ' ';
          index += 2;
      }
      retour[index-2] = ']' ; retour[index-1] = '"'; retour[index] = '\0';
      
      return retour;
     
      
  }
}


char* _len_(strlist* args, intlist* types, int* typeRetour)
{
  *typeRetour=TYPE_NUMBER[0];
  char* retour="";
  
  if (types->tab[0]==TYPE_LIST[0] || types->tab[0]==TYPE_TEMPLIST[0])
  {
    retour = nbToStr(LISTES.tab[ADRESSES.tab[strlist_index(&NOMS,args->tab[0])]].len);
  }
  else if (types->tab[0]==TYPE_STRING[0])
  {
    retour = nbToStr(strlen(args->tab[0])-2);
  }
  else
  {
    quitError("Cet objet n a pas de longueur.");
  }
  
  return retour;
  
}


char* _substring_(strlist* args, intlist* types, int* typeRetour)
{
  *typeRetour=TYPE_STRING[0];
  char* substring = sub(args->tab[0],strToNb(args->tab[1])+1,strToNb(args->tab[2])+1);
  char* str2=addStr("\"",substring);
  char* str3=addStr(str2,"\"");
  err_free(substring);err_free(str2);
  
  return str3;
}



char* _exit_(strlist* args, intlist* types, int* typeRetour)
{
  freeAllPtr();
  exit(0);
  return 0;
}



char* _append_(strlist* args, intlist* types, int* typeRetour)
{
  
  unsigned index=strlist_index(&NOMS, args->tab[0]);
  unsigned indexListe=ADRESSES.tab[index];

  
  for (unsigned i=1;i<args->len;i++)//boucle qui ajoute tous les éléments (sans oublier d'en faire une copie)
  {
    strlist_append(&LISTES.tab[indexListe],copy(args->tab[i]));
  }
  

  *typeRetour = TYPES.tab[index];
  
  return copy(args->tab[0]);
}







char* _remove_(strlist* args, intlist* types, int* typeRetour)
{
  
  unsigned index=strlist_index(&NOMS, args->tab[0]);
  unsigned indexListe=ADRESSES.tab[index];

  unsigned indexDel;
  
  for (unsigned i=1;i<args->len;i++)//boucle qui ajoute tous les éléments (sans oublier d'en faire une copie)
  {
    indexDel=strToNb(args->tab[i]);
    strlist_remove(&LISTES.tab[indexListe],indexDel,1);
  }
  

  *typeRetour = TYPES.tab[index];
  
  return copy(args->tab[0]);
}





char* _insert_(strlist* args, intlist* types, int* typeRetour)
{
  
  unsigned index=strlist_index(&NOMS, args->tab[0]);
  unsigned indexListe=ADRESSES.tab[index];

  unsigned indexIns;
  
  for (unsigned i=1;i<args->len;i+=2)//boucle qui ajoute tous les éléments (sans oublier d'en faire une copie)
  {
    indexIns=strToNb(args->tab[i+1]);
    strlist_insert(&LISTES.tab[indexListe],copy(args->tab[i]),indexIns);
  }
  

  *typeRetour = TYPES.tab[index];
  
  return copy(args->tab[0]);
}




char* _type_(strlist* args, intlist* types, int* typeRetour)
{
  *typeRetour=TYPE_STRING[0];
  unsigned type=0;
  
  char* retour="";
  
  if (types->tab[0]==TYPE_VARIABLE[0])
  {
    type=TYPES.tab[strlist_index(&NOMS,args->tab[0])];
  }
  else if (types->tab[0]==TYPE_LIST[0] || types->tab[0]==TYPE_TEMPLIST[0])
  {
    type=TYPE_LIST[0];
  }
  else if (types->tab[0]==TYPE_LISTINDEX[0])
  {
  
    char* nomListe=nameListIndex(args->tab[0]);
    unsigned index=indexListIndex(args->tab[0]);
    
    unsigned indexListe=strlist_index(&NOMS,nomListe);
    
    if (index >= LISTES.tab[ADRESSES.tab[indexListe]].len)
      quitError(ERROR_OUT_OF_RANGE);
    
    type=typeListItem(LISTES.tab[ADRESSES.tab[indexListe]].tab[index]);
    err_free(nomListe);
    
  }
  else
  {
    type=types->tab[0];
  }
  
  switch(type)
  {
    case 'n': //TYPE_NUMBER[0]
      retour="\"Nombre\"";
      break;
      
    case 's': //TYPE_STRING[0]
      retour="\"Texte\"";
      break;
      
    case 'l': //TYPE_LIST[0]
      retour="\"Liste\"";
      break;
    
    case 'b': //TYPE_BOOL[0]
      retour="\"Booleen\"";
      break;
    
    case 'N': //TYPE_NONE[0]
      retour="\"NoneType\"";
      break;
  }
  
  return copy(retour);
  
}


char* _reverse_(strlist* args, intlist* types, int* typeRetour)
{
    if (types->tab[0] == TYPE_STRING[0])
    {
        unsigned len = strlen(args->tab[0]);
        char* retour = err_malloc((len+1)*sizeof(char));
        for (unsigned i=0 ; i<len ; i++)
        {
            retour[i] = args->tab[0][len-i-1];
        }
        retour[len] = '\0';
        
        *typeRetour = TYPE_STRING[0];
        
        return retour;
    }
    else if (types->tab[0] == TYPE_LIST[0])
    {
        
        
        quitError("Type non supportes pour reverse\n");
        
    }
    else
        quitError("Type non supportes pour reverse\n");
}


char* _eval_(strlist* args, intlist* types, int* typeRetour)
{
    Tree* tree = tree_create(NULL, 0, 0);
    
    char* exp = sub(args->tab[0], 1, strlen(args->tab[0])-1);
    
    createExpressionTree(tree, exp);
    char* res = eval(tree, typeRetour);
    
    err_free(exp);
    
    tree_destroy(tree);
    
    return res;
}



