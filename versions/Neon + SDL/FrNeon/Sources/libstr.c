#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


#include "headers.c"







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

extern strlist boolean;

extern int CODE_ERROR;




void cleanStdin(void)// vide le buffer
{
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF);
}



void pause(void)
{
  printString("Pressez une touche pour quitter...");
  cleanStdin();
  //getchar();

}






int sindex (char* string, char car)
{
    for (unsigned i=0 ; i < strlen(string) ; i++)
    {
        if (string[i] == car)
            return (int)i;
    }
    
    return -1;
}   



_Bool strInf(char* str1, char* str2)
{
    if (str1[0] > str2[0])
        return false;
    else if (str1[0] < str2[0])
        return true;
    else
    {
        if (str1[0] == '\0' || str2[0] == '\0')
            return false;
        else
            return strInf(str1 + 1, str2 + 1);
    }
}




_Bool strInfEq(char* str1, char* str2)
{
    if (str1[0] > str2[0])
        return false;
    else if (str1[0] <= str2[0])
        return true;
    else
    {
        if (str1[0] == '\0' || str2[0] == '\0')
            return str1[0] <= str2[0];
        else
            return strInf(str1 + 1, str2 + 1);
    }
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
	char* string8=replace(string7,"\\\\", "\\");
	err_free(string1);err_free(string2);err_free(string3);err_free(string4);err_free(string5);err_free(string6);err_free(string7);
	return string8;
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
	char* string8=replace(string7,"\\", "\\\\");
	err_free(string1);err_free(string2);err_free(string3);err_free(string4);err_free(string5);err_free(string6);err_free(string7);
	return string8;
}





int compteAcc(char* str) // compte le nombre d'accolades ouvrantes non complétées par des fermantes afin d'afficher l'indentation
{
    char* string = addStr(str, " ");
    int len_string = strlen(string);
    int acc = 0;

    #define ouvrant "alors"
    #define longueur_ouvrant 5

    #define ouvrant2 "faire"
    #define longueur_ouvrant2 5

    #define fermant "fin"
    #define longueur_fermant 3

    _Bool str1 = false;
    _Bool str2 = false;
    _Bool isPotentiallyWord = false;

    // après, il y a les commentaires, mais on s'en fout
    for (int k = 0 ; k < len_string ; k++)
    {

        if (isalnum(string[k]) || string[k] == '_' || string[k] == '\'' && !str1 && !str2)
            isPotentiallyWord = true;
        
        if (isPotentiallyWord && string[k] != '_' && string[k] != '\'' && !isalnum(string[k]))
            isPotentiallyWord = false;


        if (string[k] == '\'' && !isPotentiallyWord) str1 = !str1;
        if (string[k] == '"') str2 = !str2;

        if (!str1 && !str2 && !isalnum(string[k])) // pas dans les chaines de caractères, ni dans les noms de variable
        {
            if (k < len_string - longueur_ouvrant - 1) // pour le 'do'
            {
                if (isString(string + k + 1, ouvrant, longueur_ouvrant)) // remplacement du 'do'
                    acc++;
            }

            if (k < len_string - longueur_ouvrant2 - 1) // pour le 'do'
            {
                if (isString(string + k + 1, ouvrant2, longueur_ouvrant2)) // remplacement du 'do'
                    acc++;
            }

            // --------fermants-----------
    
            if (k < len_string - longueur_fermant - 1) // pour le 'end'
            {
                if (isString(string + k + 1, fermant, longueur_fermant)) // remplacement du 'end'
                    acc--;
            }
            
            if (string[k] == '(' || string[k] == '[')
                acc++;
            else if (string[k] == ')' || string[k] == ']')
                acc--;
        }
    }

    return acc;

}







char* inputCode(char* text)
{
    setColor(BLUE);
    printString(text);
    setColor(WHITE);
    
    char* str = input("");

    if (CODE_ERROR != 0)
        return NULL;
    
    while (!isFull(str))
    {
        if (CODE_ERROR != 0)
        {
            err_free(str);
            return NULL;
        }

        int nbAcc = compteAcc(str);

        printString("  ");
        for (int i=0 ; i < nbAcc ; i++)
          printString("| ");
        
        char* newStr = input("");

        if (CODE_ERROR != 0)
            return NULL;
            
        char* temp = addStr("\n", newStr);
        char* temp2 = addStr(str, temp);
        err_free(str);
        err_free(newStr);
        str = temp2;
        err_free(temp);
    }
    return str;
}










int count(char* string, char* search)//compte le nombre d'occurrences d'une chaîne dans une autre
{

int count=0;

  if ( strlen(search) > strlen(string) )//si la chaîne à rechercher est plus grande que celle dans laquelle on effectue la recherche
  {
    CODE_ERROR = 67;
    return 0;
  }
  
  char* tempstr;
  for(unsigned i=0;i<=strlen(string)-strlen(search);i++)
  {
    tempstr=sub(string,(int)i,(int)(i+strlen(search)));//on va tester la sous chaine de position i
      
    if ( strcmp(search,tempstr)==0 )
    {
      count++;
    }
    err_free(tempstr);
  }
  
  
  return count;
}




char* sub(char* string,int debut,int fin)//permet d'extraire une sous-chaine
{
  int longueur = fin-debut;
  
  if ((unsigned) (debut+longueur) > strlen(string) || longueur < 0)
  {
    CODE_ERROR = 67;
    return NULL;
  }
  
  char* newStr = err_malloc(longueur * sizeof(char)+1);//allocation d'un pointeur

  if (newStr == NULL)
  {
      CODE_ERROR = 12;
      return NULL;
  }
  
  err_memset(newStr,0,longueur*sizeof(char)+1);//initialisation à 0
  
  
  for (unsigned i=0; i < (unsigned)longueur && i+(unsigned)debut<strlen(string); i++)//recopie les caractères
    newStr[i]=string[i+debut];
  
  
  newStr[longueur]='\0';// ajoute je caractère nul
  
  return newStr;
}



char* sandwich(char* string, char car)
{

    /* fonction qui ajoute le caractère car avant et après la chaine de caractères string*/
    int len = strlen(string);
    char* newStr = err_malloc(len + 3);
  
    newStr[0]=car;
    newStr[len+1]=car;
    newStr[len+2]='\0';
    for (int i = 0 ; i < len ; i++)
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



_Bool strToBool(char* string)
{
    return strcmp(string, boolean.tab[0])==0;
}








char* subReplace(char* string, int debut, int longueur, char* remplacement)//effectue un remplacement quand on lui indique l’endroit et la longueur
{
  if ((unsigned) (debut+longueur) > strlen(string))
  {
    CODE_ERROR = 67;
    return 0;
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
    tmp=subReplace(chnRemplacee,(int long)ptrSubStr-(int long)chnRemplacee,strlen(aRemplacer),remplacement);//enregistrement du résultat dans une chaine temporaire pour pouvoir libérer le contenu pointé par string avant de réaffecter string
    err_free(chnRemplacee);
    chnRemplacee=tmp;
  }
  
  return chnRemplacee;
}
