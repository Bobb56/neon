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





void cleanStdin(void)// vide le buffer
{
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF);
}



void pause(void)
{
  printf("\nAppuyez sur une touche pour continuer...");
  cleanStdin();
  //getchar();

}



char *copy(char * string) //permet de copier une chaine de caractere vers un autre pointeur sans affectation pour pouvoir la liberer sans liberer l’originale
{
  char* retour = err_malloc ((strlen(string)+1)*sizeof(char));
  strcpy(retour, string);
  return retour;
}


int sindex (char* string, char car)
{
    for (unsigned i=0 ; i<strlen(string) ; i++)
    {
        if (string[i] == car)
            return i;
        
    }
    
    return -1;
}   





char* traitementString(char* string)
{
	char* string1=replace(string, "\\a", "\a");
	char* string2=replace(string1,"\\b", "\b");
	char* string3=replace(string2,"\\f", "\f");
	char* string4=replace(string3,"\\r", "\r");
	char* string5=replace(string4,"\\v", "\v");
	char* string6=replace(string5,"\\t", "\t");
	char* string7=replace(string6,"\\n", "\n");
	err_free(string1);err_free(string2);err_free(string3);err_free(string4);err_free(string5);err_free(string6);
	return string7;
}


char* traitementStringInverse(char* string)
{
	char* string1=replace(string, "\a", "\\a");
	char* string2=replace(string1,"\b", "\\b");
	char* string3=replace(string2,"\f", "\\f");
	char* string4=replace(string3,"\r", "\\r");
	char* string5=replace(string4,"\v", "\\v");
	char* string6=replace(string5,"\t", "\\t");
	char* string7=replace(string6,"\n", "\\n");
	err_free(string1);err_free(string2);err_free(string3);err_free(string4);err_free(string5);err_free(string6);
	return string7;
}



char* input(char *text)
{
  char* var=err_malloc(1001*sizeof(char)); // allocation d'un pointeur pour l'entrée de l'utilisateur (+1 char pour le caractère nul)
  err_memset(var,(char)0,1001*sizeof(char));//initialise le pointeur à '\0' partout
  //on effectue l'entrée
  printf("%s",text);

  int err = scanf("%4000[^\n]",var);

  cleanStdin();

  if (err!=1)
  {
    return NULL;
  }

  //crée un deuxième pointeur pour y copier le contenu de l'entrée de la vraie longueur
  char* newVar = err_malloc(sizeof(char)*(strlen(var)+1));//réserve une place de la longueur de l'entrée + 1 pour le caractère nul

  void * ptrtest=strcpy(newVar,var);//copie de var dans newVar
  if (ptrtest==NULL)
  {
    quitError(ERROR_STRCPY);
  }
  err_free(var);var=NULL;//libération de var
  return newVar;
}




unsigned count(char* string, char* search)//compte le nombre d'occurrences d'une chaîne dans une autre
{

unsigned count=0;

  if ( strlen(search) > strlen(string) )//si la chaîne à rechercher est plus grande que celle dans laquelle on effectue la recherche
  {
    quitError(ERROR_OUT_OF_RANGE);
  }
  
  char* tempstr;
  for(unsigned i=0;i<=strlen(string)-strlen(search);i++)
  {
    tempstr=sub(string,i,i+strlen(search));//on va tester la sous chaine de position i
      
    if ( strcmp(search,tempstr)==0 )
    {
      count++;
    }
    err_free(tempstr);
  }
  
  
  return count;
}




char* sub(char* string,unsigned debut,unsigned fin)//permet d'extraire une sous-chaine
{
  int longueur = fin-debut;
  
  if (debut+longueur > strlen(string) || longueur < 0)
  {
    quitError(ERROR_OUT_OF_RANGE);
  }
  
  char* newStr = err_malloc(longueur * sizeof(char)+1);//allocation d'un pointeur
  
  err_memset(newStr,0,longueur*sizeof(char)+1);//initialisation à 0
  
  
  for (int i=0; i<longueur && i+debut<strlen(string); i++)//recopie les caractères
    newStr[i]=string[i+debut];
  
  
  newStr[longueur]='\0';// ajoute je caractère nul
  
  return newStr;
}



char* sandwich(char* string, char car)
{

    /* fonction qui ajoute le caractère car avant et après la chaine de caractères string*/
    unsigned len = strlen(string);
    char* newStr = err_malloc(len + 3);
  
    newStr[0]=car;
    newStr[len+1]=car;
    newStr[len+2]='\0';
    for (unsigned i = 0 ; i < len ; i++)
        newStr[i+1] = string[i];
    

    return newStr;
}




char* addStr(char* str1, char* str2)// concatène deux chaines de caractères
{

  char* newStr = err_malloc(strlen( str1 ) + strlen( str2 ) + 1);
  
  strcpy(newStr,str1 );
  strcat(newStr,str2 );

  return newStr;
}


char* charToString(char car)
{
    char* retour = err_malloc(2*sizeof(char));
    retour[0] = car ; retour[1] = '\0';
    return retour;
}

char* addStr2(char* str1, char* str2)
{
    /*concatène deux chaines de caractères, doit etre utilisées comme suit : str1 = addStr2(str1, " coucou");
    la fonction libère la première str1
    */
    char* str = addStr(str1, str2);
    err_free(str1);
    return str;
}


double strToNb(char *string)//convertit une chaîne de caractère en nombre
{
  double nombre;
  int err=sscanf(string,"%lf",&nombre);
  if (err!=1)
  {
    quitError(ERROR_SSCANF);
  }
  
  return nombre;
}



char* nbToStr(double number)//nombre en chaine de caractère
{
  //une fois qu'on a copié le nombre dans la chaine de caracteres, il faut le réallouer de la bonne taille
  char* strNombre=err_malloc(310*sizeof(char));//on estime qu'un double ne fait pas plus de 50 caractères de longueur
  int err=snprintf(strNombre, 310, "%lf", number);//converison du nombre
  
  if (err<0 || err!=(int)strlen(strNombre))
  {
    quitError(ERROR_SNPRINTF);
  }
  
  while (strNombre[strlen(strNombre)-1]=='0')//on enlève les zéros inutiles à la fin
  {
    strNombre[strlen(strNombre)-1]='\0';
  }
  
  
  if (strNombre[strlen(strNombre)-1]=='.')//si on a enlevés tous les zéros, on enlève aussi la virgule
  {
    strNombre[strlen(strNombre)-1]='\0';
  }
  
  //ici, faire un realloc avec strlen
  
  return strNombre;
  
}



char* subReplace(char* string, unsigned debut, unsigned longueur, char* remplacement)//effectue un remplacement quand on lui indique l’endroit et la longueur
{
  if (debut+longueur > strlen(string))
  {
    quitError(ERROR_OUT_OF_RANGE);
  }
  
  
  char* debutChaine=sub(string,0,debut);//prend le début

  char* suiteChaine=addStr(debutChaine,remplacement);//ajoute la chaîne de remplacement
  
  char* fin = sub(string,debut+longueur,strlen(string));
  
  char* resultat=addStr(suiteChaine,fin);//ajoute la fin
  
  err_free(debutChaine);err_free(suiteChaine);err_free(fin);
    
  return resultat;
}



char* replace(char* string, char* aRemplacer, char* remplacement)//remplace toutes les occurrences d’une chaine par une autre
{
  char* ptrSubStr=0;
  char* tmp;
  
  /*création d'une nouvelle chaine pour effectuer le remplacement*/
  char* chnRemplacee=err_malloc(sizeof(char)*strlen(string)+1);
  
  
  strcpy(chnRemplacee,string);
  
  while ((ptrSubStr=strstr(chnRemplacee,aRemplacer)) != NULL)//appels successifs a subReplace tant que l’on detecte la chaine a remplacer dans string
  {
    tmp=subReplace(chnRemplacee,(unsigned long)ptrSubStr-(unsigned long)chnRemplacee,strlen(aRemplacer),remplacement);//enregistrement du résultat dans une chaine temporaire pour pouvoir libérer le contenu pointé par string avant de réaffecter string
    err_free(chnRemplacee);
    chnRemplacee=tmp;
  }
  
  return chnRemplacee;
}
