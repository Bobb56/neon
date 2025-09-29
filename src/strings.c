#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "headers/constants.h"
#include "headers/neonio.h"
#include "headers/dynarrays.h"
#include "headers/strings.h"
#include "extern/linenoise.h"
#include "headers/objects.h"
#include "headers/neon.h"
#include "headers/parser.h"
#include "headers/errors.h"

#ifdef WINDOWS
#include <stdio.h>
#endif

void cleanStdin(void)// vide le buffer
{
#ifdef WINDOWS
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF);
#endif
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


/*
bool strInf(char* str1, char* str2)
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




bool strInfEq(char* str1, char* str2)
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
	neon_free(string1);neon_free(string2);neon_free(string3);neon_free(string4);neon_free(string5);neon_free(string6);neon_free(string7);
	return string8;
}


char* traitementStringInverse(char* string)
{
    char* string8=replace(string,"\\", "\\\\");
	char* string1=replace(string8, "\a", "\\a");
	char* string2=replace(string1,"\b", "\\b");
	char* string3=replace(string2,"\f", "\\f");
	char* string4=replace(string3,"\r", "\\r");
	char* string5=replace(string4,"\v", "\\v");
	char* string6=replace(string5,"\t", "\\t");
	char* string7=replace(string6,"\n", "\\n");
	neon_free(string1);neon_free(string2);neon_free(string3);neon_free(string4);neon_free(string5);neon_free(string6);neon_free(string8);
	return string7;
}



bool isidentifier(char c) {
    return isalnum(c) || c == '_';
}



int unitCharToInt(char car, const char base)
{
    if (car != '0' && car != '1' && base == 'b') // le binaire n'autorise que 0 ou 1
        global_env->CODE_ERROR = 73;
    
    if (isdigit(car))
        return car - '0';
    else if (car >= 'a' && car <= 'f')
        return car - 'a' + 10;
    else if (car >= 'A' && car <= 'F')
        return car - 'A' + 10;

    global_env->CODE_ERROR = 73;
    return 0;
}


intptr_t binToDec(char* chaine, int debut, int longueur)
{
    int res = 0;
    for (int i = debut ; i < debut + longueur ; i++)
    {
        res *= 2;
        res += unitCharToInt(chaine[i], 'b');

        if (global_env->CODE_ERROR != 0)
            return 0;
        
    }
    return res;
}




intptr_t hexToDec(char* chaine, int debut, int longueur)
{
    int res = 0;
    for (int i = debut ; i < debut + longueur ; i++)
    {
        res *= 16;
        res += unitCharToInt(chaine[i], 'h');
        
        if (global_env->CODE_ERROR != 0)
            return 0;
    }
    return res;
}



bool isString(char* string, char* test, int size)
{
    bool bo = true;
    for (int i = 0 ; i < size ; i++)
    {
        if (string[i] != test[i])
        {
            bo = false;
            break;
        }
    }
    return bo && !isidentifier(string[size]);
}



int compteAcc(char* str) // compte le nombre d'accolades ouvrantes non complétées par des fermantes afin d'afficher l'indentation
{
    char* string = sandwich(str, ' ');
    int len_string = strlen(string);
    int acc = 0;

    #define ouvrant "do"
    #define longueur_ouvrant 2
    
    #define ouvrant2 "then"
    #define longueur_ouvrant2 4

    #define fermant "end"
    #define longueur_fermant 3

    #define sep1 "elif"
    #define longueur_sep1 4

    #define sep2 "else"
    #define longueur_sep2 4

    #define sep3 "try"
    #define longueur_sep3 3

    #define sep4 "except"
    #define longueur_sep4 6

    #define sep5 "atomic"
    #define longueur_sep5 6

    bool str1 = false;
    bool str2 = false;
    bool isPotentiallyWord = false;

    // après, il y a les commentaires, mais on s'en fout de remplacer dedans
    
    
    for (int k = 0 ; k < len_string ; k++)
    {

        if ((isidentifier(string[k]) || string[k] == '_') && !str1 && !str2)
            isPotentiallyWord = true;
        
        if (isPotentiallyWord && string[k] != '\'' && !isidentifier(string[k]))
            isPotentiallyWord = false;


        if (string[k] == '\'' && !isPotentiallyWord && !str2) str1 = !str1;
        if (string[k] == '"' && !str1) str2 = !str2;


        if (!str1 && !str2 && !isalnum(string[k])) // pas dans les chaines de caractères, ni dans les noms de variable
        {
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

            if (k < len_string - longueur_sep5 - 1) // pour le 'atomic'
            {
                if (isString(string + k + 1, sep5, longueur_sep5)) // remplacement du 'atomic'
                    acc++;
            }
            
            
            if (string[k] == '(' || string[k] == '[')
                acc++;
                
            else if (string[k] == ')' || string[k] == ']')
                acc--;
        }
    }

    neon_free(string);
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
    
    
    if (global_env->CODE_ERROR != 0 || str == NULL)
        return NULL;
    
    while (!isFull(str))
    {
        if (global_env->CODE_ERROR != 0)
        {
            neon_free(str);
            return NULL;
        }

        char* text = strdup(SEQUENCE_SUITE);
        
        int nbAcc = compteAcc(str);
        
        for (int i=0 ; i < nbAcc - 1 ; i++) // affiche un niveau d'indentation en moins pour laisser la liberté au programmeur de revenir un niveau en dessous
            text = addStr2(text, "  ");
        
        char* newStr = input(text);

        neon_free(text);

        if (global_env->CODE_ERROR != 0 || newStr == NULL) {
            neon_free(str);
            return NULL;
        }
            
        char* temp = addStr("\n", newStr);
        char* temp2 = addStr(str, temp);
        neon_free(str);
        neon_free(newStr);
        str = temp2;
        neon_free(temp);
    }
    return str;
}










int count(char* string, char* search)//compte le nombre d'occurrences d'une chaîne dans une autre
{
    int len_search = strlen(search), len = strlen(string);
    int count=0;

    for (int i = 0 ; i <= len-len_search ; i++)
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
    global_env->CODE_ERROR = 67;
    return NULL;
  }
  
  char* newStr = malloc(longueur * sizeof(char)+1);//allocation d'un pointeur

  if (newStr == NULL)
  {
      global_env->CODE_ERROR = 12;
      return NULL;
  }
  
  memset(newStr,0,longueur*sizeof(char)+1);//initialisation à 0
  
  
  for (unsigned i=0; i < (unsigned)longueur && i+(unsigned)debut<strlen(string); i++)//recopie les caractères
    newStr[i]=string[i+debut];
  
  
  newStr[longueur]='\0';// ajoute je caractère nul
  
  return newStr;
}



char* sandwich(char* string, char car)
{

    /* fonction qui ajoute le caractère car avant et après la chaine de caractères string*/
    int len = strlen(string);
    char* newStr = malloc(len + 3);
  
    newStr[0]=car;
    newStr[len+1]=car;
    newStr[len+2]='\0';
    for (int i = 0 ; i < len ; i++)
        newStr[i+1] = string[i];
    

    return newStr;
}




char* addStr(char* str1, char* str2)// concatène deux chaines de caractères
{
  char* newStr = malloc(strlen(str1) + strlen(str2) + 1);
  
  strcpy(newStr,str1);
  strcat(newStr,str2);

  return newStr;
}


char* charToString(char car)
{
    char* retour = malloc(2*sizeof(char));
    retour[0] = car ; retour[1] = '\0';
    return retour;
}

char* addStr2(char* str1, char* str2)
{
    /*concatène deux chaines de caractères, doit etre utilisées comme suit : str1 = addStr2(str1, " coucou");
    la fonction libère la première str1
    */
    char* str = addStr(str1, str2);
    neon_free(str1);
    return str;
}



bool strToBool(char* string)
{
    return strcmp(string, get_True()) == 0;
}






char* subReplace(char* string, int len, int debut, int longueur, char* remplacement, int len_remplacement)//effectue un remplacement quand on lui indique l’endroit et la longueur
{
    if ((unsigned) (debut+longueur) > strlen(string))
    {
        global_env->CODE_ERROR = 67;
        return 0;
    }
    
    char* resultat = malloc(sizeof(char) * (len - longueur + len_remplacement + 1));

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
        
        if (strcmp(aRemplacer, string + i) == 0 && (l.len == 0 || l.tab[l.len-1] + len_aremplacer <= i)) {
            intlist_append(&l, i);
        }

        string[i+len_aremplacer] = temp;
    }


    // création de la nouvelle chaine de caractères

    int new_len = len + l.len * (len_remplacement - len_aremplacer);
    char* res = malloc(sizeof(char)*(new_len+1));
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


            while (l.len != 0 && l.tab[0] < i_string) // au cas où les occurrences sont entremêlées
                intlist_remove(&l, 0);
            
        }
        else
        { // si on n'a pas fait de remplacement on incrémente quand même les indices et on copie
            res[i_res] = string[i_string];
            i_string ++;
            i_res ++;
        }
    }

    neon_free(l.tab);

    return res;
}




// Les fonctions suivantes servent à gérer les noms de variables du type Module~nom

// sert à savoir si une variable commence par prefix~
bool has_strict_prefix(char* string, char* prefix) {
    int len = strlen(prefix);
    if (len >= strlen(string))
        return false;

    char temp = string[len];
    string[len] = '\0';

    bool bo = (strcmp(string, prefix) == 0);

    string[len] = temp;

    return bo;
}

/*
Renvoie un nom de variable nettoyé du préfixe
On suppose que string commence bel et bien par prefix
*/
char* remove_prefix(char* string, char* prefix) {
    return strdup(string + strlen(prefix));
}

// renvoie le préfixe d'un nom de variable s'il existe, NULL sinon
char* get_prefix(char* string) {
    int i=0;
    while (string[i] != '\0' && string[i] != '~')
        i++;

    if (string[i] == '~' && string[i+1] != '\0') { // la chaine de caractères correspond à une chaine de module
        string[i] = '\0';
        char* prefix = strdup(string);
        string[i] = '~';
        return prefix;
    }
    return NULL;
}

/*
Renvoie tous les noms de modules existants
*/
strlist* get_all_modules(void) {
    strlist* modules = strlist_create(0);
    // on récupère tous les noms de modules
    for (int i=0 ; i < global_env->NOMS->len ; i++) {
        char* prefix = get_prefix(global_env->NOMS->tab[i]);
        if (prefix != NULL && NEO_TYPE(global_env->ADRESSES->tab[i]) != TYPE_EMPTY && !strlist_inList(modules, prefix)) {
            strlist_append(modules, prefix);
        }
        else if (prefix != NULL)
            neon_free(prefix);
    }
    return modules;
}

/*
Cette fonction vérifie si un module existe
*/
bool is_module(char* module) {
    for (int i=0 ; i < global_env->NOMS->len ; i++) {
        if (has_strict_prefix(global_env->NOMS->tab[i], module)) {
            return true;
        }
    }
    return false;
}


int function_module(char* module, char* function) {
    char* functionName = addStr2(addStr(module, "~"), function);
    int n = strlist_index(global_env->NOMS, functionName);
    global_env->CODE_ERROR = 0;
    neon_free(functionName);

    if (n < 0 || (NEO_TYPE(global_env->ADRESSES->tab[n]) != TYPE_USERFUNC && NEO_TYPE(global_env->ADRESSES->tab[n]) != TYPE_USERMETHOD))
        return -1;

    return n;
}