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



#ifndef LINUX
    void cleanStdin(void)// vide le buffer
    {
        int c = 0;
        while ((c = getchar()) != '\n' && c != EOF);
    }
#endif








int sindex (char* string, char car)
{
    for (unsigned i=0 ; i < strlen(string) ; i++)
    {
        if (string[i] == car)
            return (int)i;
    }
    
    return -1;
}


/*
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
*/






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
    char* string = sandwich(str, ' ');
    int len_string = strlen(string);
    int acc = 0;

    #define ouvrant "faire"
    #define longueur_ouvrant 5
    
    #define ouvrant2 "alors"
    #define longueur_ouvrant2 5

    #define fermant "Fin"
    #define longueur_fermant 3

    #define sep1 "SinonSi"
    #define longueur_sep1 7

    #define sep2 "Sinon"
    #define longueur_sep2 5

    #define sep3 "Essayer"
    #define longueur_sep3 7

    #define sep4 "SiErreur"
    #define longueur_sep4 8

    _Bool str1 = false;
    _Bool str2 = false;
    _Bool isPotentiallyWord = false;

    // après, il y a les commentaires, mais on s'en fout de remplacer dedans
    
    
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
            if (string[k] == '|') // spécifique à Freon
                string[k] = ' ';
            
            if (k < len_string - longueur_ouvrant - 1) // pour le 'do'
            {
                if (isString(string + k + 1, ouvrant, longueur_ouvrant)) // remplacement du 'do'
                    acc++;
            }
            
            if (k < len_string - longueur_ouvrant2 - 1) // pour le 'then'
            {
                if (isString(string + k + 1, ouvrant2, longueur_ouvrant2)) // remplacement du 'do'
                    acc++;
            }
    
            if (k < len_string - longueur_fermant - 1) // pour le 'end'
            {
                if (isString(string + k + 1, fermant, longueur_fermant)) // remplacement du 'end'
                    acc--;
            }

            if (k < len_string - longueur_sep1 - 1) // pour le 'elif'
            {
                if (isString(string + k + 1, sep1, longueur_sep1)) // remplacement du 'elif'
                    acc--;
            }

            if (k < len_string - longueur_sep2 - 1) // pour le 'else'
            {
                if (isString(string + k + 1, sep2, longueur_sep2)) // remplacement du 'else'
                    acc--;
            }

            if (k < len_string - longueur_sep3 - 1) // pour le 'try'
            {
                if (isString(string + k + 1, sep3, longueur_sep3)) // remplacement du 'try'
                    acc++;
            }

            if (k < len_string - longueur_sep4 - 1) // pour le 'except'
            {
                if (isString(string + k + 1, sep4, longueur_sep4)) // remplacement du 'except'
                    acc--;
            }
            
            
            if (string[k] == '(' || string[k] == '[')
                acc++;
                
            else if (string[k] == ')' || string[k] == ']')
                acc--;
        }
    }

    err_free(string);
    return acc;

}







char* inputCode(char* text)
{
    #ifndef LINUX
        setColor(BLUE);
        printString(text);
        setColor(WHITE);
        char* str = input("");
    #else
        char* str = input(SEQUENCE_ENTREE); // dans le cas Linux, input se charge lui-même de mettre le bleu
    #endif
    
    

    if (CODE_ERROR != 0)
        return NULL;
    
    while (!isFull(str))
    {
        if (CODE_ERROR != 0)
        {
            err_free(str);
            return NULL;
        }

        char* text = strdup("~ ");
        
        int nbAcc = compteAcc(str);
        
        for (int i=0 ; i < nbAcc - 1 ; i++) // affiche un niveau d'indentation en moins pour laisser la liberté au programmeur de revenir un niveau en dessous
            text = addStr2(text, "| ");
        
        char* newStr = input(text);

        err_free(text);

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
    int len_search = strlen(search), len = strlen(string);
    int count=0;

    for (int i = 0 ; i < len-len_search ; i++)
    {
        char temp = string[i+len_search];
        string[i+len_search] = '\0';
        if (strcmp(search, string + i) == 0)
            count++;

        string[i+len_search] = temp;
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






char* subReplace(char* string, int len, int debut, int longueur, char* remplacement, int len_remplacement)//effectue un remplacement quand on lui indique l’endroit et la longueur
{
    if ((unsigned) (debut+longueur) > strlen(string))
    {
        CODE_ERROR = 67;
        return 0;
    }
    
    char* resultat = err_malloc(sizeof(char) * (len - longueur + len_remplacement + 1));

    for (int i = 0 ; i < debut ; i++) // partie inchangée
        resultat[i] = string[i];

    for (int i = 0 ; i < len_remplacement ; i++)
        resultat[i + debut] = remplacement[i];

    for (int i = debut + longueur ; i < len ; i++) // on indexe sur string, il faut donc transformer pour resultat
        resultat[i - longueur + len_remplacement] = string[i];
    
    resultat[len - longueur + len_remplacement] = '\0';
    
    return resultat;
}



char* replace(char* string, char* aRemplacer, char* remplacement) //remplace toutes les occurrences d’une chaine par une autre
{
    int len_remplacement = strlen(remplacement);
    int len = strlen(string);
    int len_aremplacer = strlen(aRemplacer);

    if (len_aremplacer == 0 || len == 0)
    {
        return strdup("");
    }

    intlist l = intlist_create(0);
    for (int i = 0 ; i < len - len_aremplacer + 1; i++)
    {
        char temp = string[i+len_aremplacer];
        string[i+len_aremplacer] = '\0';
        
        if (strcmp(aRemplacer, string + i) == 0)
            intlist_append(&l, i);

        string[i+len_aremplacer] = temp;
    }

    // création de la nouvelle chaine de caractères

    int new_len = len + l.len * (len_remplacement - len_aremplacer);
    char* res = err_malloc(sizeof(char)*(new_len+1));
    res[new_len] = '\0';

    int i_string = 0, i_res = 0;
    while (i_string < len && i_res < new_len)
    {
        if (l.len != 0 && l.tab[0] == i_string) // ici, il faut écrire remplacement
        {
            for (int i = 0 ; i < len_remplacement ; i++)
                res[i+i_res] = remplacement[i];
            
            i_string += len_aremplacer;
            i_res += len_remplacement;

            while (l.len != 0 && l.tab[0] <= i_string) // au cas où les occurrences sont entremêlées
                intlist_remove(&l, 0);
        }
        
        res[i_res] = string[i_string];
        

        i_string ++;
        i_res ++;
    }

    err_free(l.tab);

    return res;
}
