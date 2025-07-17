#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "headers.c"



extern int CODE_ERROR;



//déclaration des variables globales à cut
extern strlist acceptedChars;
extern listlist syntax;
extern intlist types_debut;
extern intlist types_fin;
extern strlist sousop;
extern intlist gramm1;
extern strlist operateurs3;
extern strlist operateurs1;
extern strlist operateurs2;
extern strlist blockwords;
extern strlist blockwords1Line;
extern strlist keywordFunction;
extern strlist boolean;
extern strlist exceptions;
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



Ast** ast_create(intlist* typeTok) {
    Ast** ast = err_malloc(sizeof(Ast*) * typeTok->len);
    for (int i = 0 ; i < typeTok->len ; i++) {
        ast[i] = err_malloc(sizeof(Ast));
        ast[i]->type = typeTok->tab[i];
        ast[i]->fin = i;
        ast[i]->suiv = NULL;
    }
    return ast;
}


void ast_push(Ast* ast) {
    Ast* ast2 = err_malloc(sizeof(Ast));
    ast2->fin = ast->fin;
    ast2->type = ast->type;
    ast2->suiv = ast->suiv;
    // on a fait une copie du chaînon

    ast->suiv = ast2;
}



void ast_pop(Ast* ast) {
    if (ast -> suiv == NULL) {
        printf("Vous ne pouvez pas poper un ast qui n'a plus qu'un élément\n");
        return;
    }

    Ast* sov = ast->suiv;
    ast->fin = sov->fin;
    ast->type = sov->fin;
    ast->suiv = sov->suiv;
    err_free(sov);
}


void ast_destroy(Ast ** ast, int length) {
    for (int i = 0 ; i < length ; i++) {
        while (ast[i]->suiv != NULL)
            ast_pop(ast[i]);
        err_free(ast[i]);
    }
    err_free(ast);
}





void ast_aff(Ast** ast, int len)//affiche une liste d'entiers
{

    char* tab[] = {
        "func",
        "index",
        "op",
        "str",
        "nbr",
        "list",
        "block",
        "bool",
        "const",
        "var",
        "line",
        "eol",
        "lpar",
        "rpar",
        "virg",
        "key",
        "none",
        "bwl",
        "kwfunc",
        "expt",
        "acc"
    };





  if (len == 0)//si la liste a une longueur de zéro
  {
    printString("[]");
    newLine();
  }
  else
  {
    printString("[");
    for ( int i = 0 ; i < len -1 ; i++)//affiche les éléments du premier à l'avant-dernier
    {
      printString("(");
      if (ast[i]->type < 21)
        printString(tab[ast[i]->type]);
      else
        printf("%d", ast[i]->type);
      printString(", ");
      printInt(ast[i]->fin);
      printString("), ");
    }

    printString("(");
    if (ast[len-1]->type < 21)
        printString(tab[ast[len-1]->type]);
    else
        printString("unknwn");
    printString(", ");
    printInt(ast[len-1]->fin);
    printString(")]");
    newLine();
  }
}




_Bool isFull(char* string)
{
    strlist* tokens = strlist_create(0);
    intlist types = intlist_create(0);
    Ast** ast;

    cut(tokens, &types, string, true);

    if (CODE_ERROR == 0) {
        ast = ast_create(&types);
        
        parse(tokens, types, ast, 0);

        if (CODE_ERROR == 0)
            statements(&types, tokens, ast, 0);

        ast_destroy(ast, tokens->len);
    }

    strlist_destroy(tokens, true);
    err_free(types.tab);

    
    if (CODE_ERROR == 29 || CODE_ERROR == 26 || CODE_ERROR == 11 || CODE_ERROR == 77)
    {
        CODE_ERROR = 0;
        return false;
    }
    else if (CODE_ERROR != 0)
        return false;
    else
        return true;
}


int unitCharToInt(char car, const char base)
{
    if (car != '0' && car != '1' && base == 'b') // le binaire n'autorise que 0 ou 1
        CODE_ERROR = 73;
    
    if (isdigit(car))
        return car - 48;
    else if (car >= 'a' && car <= 'f')
        return car - 87;

    CODE_ERROR = 73;
    return 0;
}


long int binToDec(char* chaine, int debut, int longueur)
{
    int res = 0;
    for (int i = debut ; i < debut + longueur ; i++)
    {
        res *= 2;
        res += unitCharToInt(chaine[i], 'b');

        if (CODE_ERROR != 0)
            return 0;
        
    }
    return res;
}




long int hexToDec(char* chaine, int debut, int longueur)
{
    int res = 0;
    for (int i = debut ; i < debut + longueur ; i++)
    {
        res *= 16;
        res += unitCharToInt(chaine[i], 'h');
        
        if (CODE_ERROR != 0)
            return 0;
    }
    return res;
}


_Bool isidentifier(char c) {
    return isalnum(c) || c == '_';
}



_Bool isString(char* string, char* test, int size)
{
    _Bool bo = true;
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


void write_libtok(char* buf, char* string, int size_string, int size)
{
    for (int i=0 ; i < size_string ; i++)
        buf[i] = string[i];
    
    for (int i = size_string ; i < size ; i++)
        buf[i] = ' ';
}




void cut(strlist* tokens, intlist* types, char* str, _Bool traiterStatements)
{
    /*fonction qui coupe la chaine de caractères en tokens
    traiterStatements est un booléen qui indique s'il faut regrouper les statements*/

    char* string;

    
    
    if (traiterStatements)
        string = sandwich(str, '\n');
    else
        string = addStr(str, "\n");

    int len_string = strlen(string);//pour utiliser la longueur de string sans appeler a chaque fois strlen

    //-------------------- ALGORITHME QUI REMPLACE LES DO ET END PAR DES ACCOLADES ------------------------
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


    _Bool str1 = false;
    _Bool str2 = false;
    _Bool isPotentiallyWord = false;

    // après, il y a les commentaires, mais on s'en fout
    for (int k = 0 ; k < len_string ; k++)
    {

        if (isidentifier(string[k]) || string[k] == '_' || string[k] == '\'' && !str1 && !str2)
            isPotentiallyWord = true;
        
        if (isPotentiallyWord && string[k] != '_' && string[k] != '\'' && !isidentifier(string[k]))
            isPotentiallyWord = false;


        if (string[k] == '\'' && !isPotentiallyWord) str1 = !str1;
        if (string[k] == '"') str2 = !str2;

        if (!str1 && !str2 && !isidentifier(string[k])) // pas dans les chaines de caractères, ni dans les noms de variable
        {
            if (k < len_string - longueur_ouvrant - 1) // pour le 'do'
            {
                if (isString(string + k + 1, ouvrant, longueur_ouvrant)) // remplacement du 'do'
                    write_libtok(string + k + 1, "{", 1, longueur_ouvrant);
            }
            
            if (k < len_string - longueur_ouvrant - 1) // pour le 'then'
            {
                if (isString(string + k + 1, ouvrant2, longueur_ouvrant2)) // remplacement du 'then'
                    write_libtok(string + k + 1, "{", 1, longueur_ouvrant2);
            }
    
            if (k < len_string - longueur_fermant - 1) // pour le 'end'
            {
                if (isString(string + k + 1, fermant, longueur_fermant)) // remplacement du 'end'
                    write_libtok(string + k + 1, "}", 1, longueur_fermant);
            }

            if (k < len_string - longueur_sep1 - 1) // pour le 'elif'
            {
                if (isString(string + k + 1, sep1, longueur_sep1)) // remplacement du 'elif'
                    write_libtok(string + k + 1, "} ei", 4, longueur_sep1);
            }

            if (k < len_string - longueur_sep2 - 1) // pour le 'else'
            {
                if (isString(string + k + 1, sep2, longueur_sep2)) // remplacement du 'else'
                    write_libtok(string + k + 1, "}es{", 4, longueur_sep2);
            }

            if (k < len_string - longueur_sep3 - 1) // pour le 'try'
            {
                if (isString(string + k + 1, sep3, longueur_sep3)) // remplacement du 'try'
                    write_libtok(string + k + 1, "tr{", 3, longueur_sep3);
            }

            if (k < len_string - longueur_sep4 - 1) // pour le 'except'
            {
                if (isString(string + k + 1, sep4, longueur_sep4)) // remplacement du 'except'
                    write_libtok(string + k + 1, "} expt", 6, longueur_sep4);
            }
        }
    }

    //--------------------------- FIN DE L'ALGORITHME DE REMPLACEMENT ------------------------------------

    _Bool nouvTok=false; //on modifie la valeur si on a un nouveau token à ajouter
    
    // pour les sous-tokens
    _Bool isPotentiallyNumber=false;
    int stepNumber = 0; // définit à quelle étape de définition de nombre on est lors de la détection
    _Bool isPotentiallyHexBin=false;
    int stepHexBin = 0;
    _Bool isPotentiallyString=false;
    _Bool isPotentiallyString2=false; // chaine avec un guillemet court
          isPotentiallyWord=false;
    _Bool isPotentiallyOp=false;
    _Bool isPotentiallyComm=false;
    _Bool isPotentiallyLongComm=false;
    
    
    
    
    
    
    
    
    int debTok=0; // contient la place de debut de detection du token etudie
    int debTok2=0; // contient la place de debut de detection du token compose etudie
    int debTok3=0;
    
    int debExpr=0; // va contenir la place de debut de l’expression dans le dernier algorithme
    _Bool isPotentiallyExpr=false; // pour la derniere boucle dans l’empaquetage d’expressions
    
    
    
    
    // pour la dernière boucle aussi, empaquetage des blocs conditionnels if/elif/else et for, while (statement)
    int debStat=0;
    _Bool isPotentiallyStat=false;
    
    
    
    
    char char2=0; // va contenir le caractere precedent
    char char1=0; // va contenir le caractère actuel
    char* char1_2; // peut servir d'intermédiaire quand on veut transformer char1 en chaine de caractères
    
    int nombrePoints=0; // nombre de points comptes apres le debut d’un nombre

    
    for (int i=0 ; i<len_string ; i++)
    {
        char1 = string[i];
        
        // comptage des variables relatives à la détection de nombres
        if (char1 == '.')
            nombrePoints+=1;
        if (isPotentiallyNumber && char1=='.' && nombrePoints==1 && i+1<len_string && isdigit(string [i+1]))
            stepNumber=1;
        else if (isPotentiallyNumber && char1=='.' && (i+1>=len_string || isdigit(string[i+1])))
        {
            CODE_ERROR = 2;// plusieurs virgules décimales
            err_free(string);
            return;
        }
        
        if (isPotentiallyNumber && isdigit(char1) && stepNumber == 1)
          stepNumber = 2;

        //hex-bin
        if (stepHexBin == 0 && isPotentiallyHexBin)
        {
            if ((char1 == 'x' || char1 == 'b') && char2 == '0') // bonne syntaxe
            {
                stepHexBin  = 1;
                isPotentiallyNumber = false;
            }
            else
                isPotentiallyHexBin = false;
        }
        else if (stepHexBin == 1)
        {
            if (isdigit(char1) || (char1 >= 97 && char1 <= 102))
                stepHexBin = 2;
            else
            {
                CODE_ERROR = 73;
                err_free(string);
                return ;
            }
        }

        
        //fins tokens simples
        finTokensSimples(string, &isPotentiallyNumber, &isPotentiallyString, &isPotentiallyWord, &isPotentiallyOp, &isPotentiallyLongComm, &isPotentiallyString2, &isPotentiallyComm, &isPotentiallyHexBin, &char2, &char1, &nouvTok, tokens, types, &debTok, i, &stepNumber, &stepHexBin, len_string, &nombrePoints);

        if (CODE_ERROR != 0)
        {
            err_free(string);
            return;
        }


        
        //debut tokens simples
        debutTokensSimples(i, &debTok, &char1, &isPotentiallyString, &isPotentiallyNumber,&isPotentiallyWord, &isPotentiallyOp, &isPotentiallyString2, &isPotentiallyComm, &isPotentiallyLongComm, &isPotentiallyHexBin);
        
        
        
        // vérifie si le caractère est reconnu
        if (!isPotentiallyHexBin && !isPotentiallyWord && !isPotentiallyString && !isPotentiallyNumber && !isPotentiallyOp && !isPotentiallyString2 && !isPotentiallyComm && !isPotentiallyLongComm && !nouvTok && char1 != ' ' && char1 != '\t')
        {
            CODE_ERROR = 25; // caractère inconnu
            err_free(string);
            return;
        }
        
        char1_2 = charToString(char1);
        if (nouvTok) //si on a un nouveau token, on vérifie que le caractère actuel est accepté par la grammaire globale
        {
            if (!isalnum(char1) && !(strlist_inList(&acceptedChars, char1_2)))
            {
                CODE_ERROR = 25;
                err_free(string);
                err_free(char1_2);
                return;
            }
        }
        err_free(char1_2);
        
        nouvTok=false;
        
        
        char2=char1;
    }
    
    
    // si une variable de condition de détection de token est encore à True, faute de syntaxe
    if (isPotentiallyHexBin || isPotentiallyWord || isPotentiallyString || isPotentiallyNumber || isPotentiallyOp  || isPotentiallyString2 || isPotentiallyComm || isPotentiallyLongComm)
    {
        err_free(string);

        CODE_ERROR = 26; // chaine de caractère, liste ou autre non terminée
        
        return;
    }
    
    err_free(string);

    return ;
}






void finTokensSimples(char* string, _Bool* isPotentiallyNumber, _Bool* isPotentiallyString, _Bool* isPotentiallyWord, _Bool* isPotentiallyOp, _Bool* isPotentiallyLongComm, _Bool* isPotentiallyString2, _Bool* isPotentiallyComm, _Bool* isPotentiallyHexBin, char * char2, char* char1, _Bool* nouvTok, strlist* tokenAdd, intlist* typeTok, int* debTok, int i, int* stepNumber, int* stepHexBin, int len_string, int* nombrePoints)
{
    /*
    fonction interne à cut permettant de mettre fin à la détection des tokens simples
    */
    // chaque condition permet de mettre un terme à la détection d'un token en particulier
        
    // les listes typeTok et tokenAdd permettent de mettre fin à la détection de plusieurs tokens dans la même itération
    
    if ((*char2)=='"' && (*isPotentiallyString) && i-1!=(*debTok)) // fin de chaîne si le guillemet etudie n’est pas le meme que le premier de la chaine
    {
        char* sousch = sub(string, (*debTok), i);
        strlist_append(tokenAdd,sousch);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_STRING);
        (*isPotentiallyString)=false;
    }
    
    if ((*char2)=='\'' && (*isPotentiallyString2) && i-1!=(*debTok)) // fin de chaîne si le guillemet etudie n’est pas le meme que le premier de la chaine
    {
        char* sousch = sub(string, (*debTok), i);
        strlist_append(tokenAdd,sousch);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_STRING);
        (*isPotentiallyString2)=false;
    }
    
    if ((((*char1)=='.' && (*stepNumber)==2) || (!isdigit(*char1) && (*char1) != '.')) && (*isPotentiallyNumber) && !(*stepNumber==1) || (*isPotentiallyNumber && *char1=='.' && !isdigit(string[i+1]))) // fin de nombre
    {
        char* nb = sub(string, (*debTok), i);
        strlist_append(tokenAdd,nb);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_NUMBER);
        (*isPotentiallyNumber)=false;
        *stepNumber = 0;
        *nombrePoints = 0;
    }

    if (*isPotentiallyHexBin && *stepHexBin == 2 && !isdigit(*char1) && !(*char1 >= 97 && *char1 <= 102)) // fin hex-bin
    {
        // conversion en nombre décimal

        long int trad = (string[*debTok+1] == 'x') ? hexToDec(string, *debTok + 2, i - *debTok - 2) : binToDec(string, 2 + *debTok, i - *debTok - 2);

        if (CODE_ERROR != 0)
            return ;
        
        Number n = number_fromDouble((double)trad);
        char* nb = number_toStr(n);
        number_destroy(n);

        // ajout du nombre converti
        strlist_append(tokenAdd,nb);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_NUMBER);
        (*isPotentiallyHexBin)=false;
        *stepHexBin = 0;
        *nombrePoints = 0;
    }
    
    
    
    if ((*isPotentiallyWord) && !(isalnum(*char1) || *char1 == '_' || *char1 == '\'')) // fin word
    {
        strlist_append(tokenAdd,sub(string, (*debTok), i));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_WORD);
        (*isPotentiallyWord)=false;
    }
    
    
    if ((*isPotentiallyComm) && (*char1)=='\n') // fin commentaire
    {
        // quand il y a un commentaire, on ne fait rien
        /*strlist_append(tokenAdd,sub(string, (*debTok), i));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_COMMENT);*/

        (*nouvTok)=true;
        (*isPotentiallyComm)=false;
    }
        
    
    
    
        
    if ((*char2)=='$' && (*isPotentiallyLongComm) && (i-1)!=(*debTok))
    {
        // long commentaire : on ne fait rien
        /*strlist_append(tokenAdd,sub(string, (*debTok), i));
        
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_COMMENT);*/
        
        (*nouvTok)=true;
        (*isPotentiallyLongComm)=false;
    }
    
    
    char* sousch1 = sub(string, (*debTok), i+1);
    char* sousch2 = (i < len_string - 1) ? sub(string, (*debTok), i+2) : strdup(sousch1);
    if ((*isPotentiallyOp) && !strlist_inList(&operateurs1, sousch1) && !strlist_inList(&operateurs1, sousch2)) // fin operateur
    {
        strlist_append(tokenAdd,sub(string, (*debTok), i));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_OPERATOR);
        (*isPotentiallyOp)=false;
    }
    
    err_free(sousch1);err_free(sousch2);
    
    // elements d’un caractere
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && ((*char1)=='(' || (*char1)==')'))
    {
        (*nouvTok)=true;
        strlist_append(tokenAdd,charToString(*char1));
        if ((*char1)=='(')
            intlist_append(typeTok,TYPE_PARENTHESE1);
        if ((*char1)==')')
            intlist_append(typeTok,TYPE_PARENTHESE2);
        
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && ((*char1)=='{' || (*char1)=='}'))
    {
        strlist_append(tokenAdd,charToString(*char1));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_ACCOLADE);
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && ((*char1)=='[' || (*char1)==']'))
    {
        strlist_append(tokenAdd,charToString(*char1));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_CROCHET);
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && (*char1)==',')
    {
        strlist_append(tokenAdd,charToString(*char1));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_VIRGULE);
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    
    if (!(*isPotentiallyNumber) && ((*char1)==';' || (*char1)=='\n'))
    {

        // si le token précédent était aussi un retour à la ligne, alors on ne compte pas celui-là

        if (tokenAdd->len == 0 || typeTok->tab[typeTok->len - 1] != TYPE_ENDOFLINE)
        {
            char* str = charToString(*char1);
            strlist_append(tokenAdd,str);
            intlist_append(typeTok,TYPE_ENDOFLINE);
            (*isPotentiallyNumber)=false;
        }
        
        if ((*isPotentiallyString) || (*isPotentiallyString2))
        {
    		CODE_ERROR = 33; // chaine de carcatère non fermée en fin de ligne
            return;
        }
        
        (*nouvTok)=true;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
        (*isPotentiallyComm)=false;
    }
    return ;
}





void debutTokensSimples(int i, int* debTok, char* char1, _Bool* isPotentiallyString, _Bool* isPotentiallyNumber, _Bool* isPotentiallyWord, _Bool* isPotentiallyOp, _Bool* isPotentiallyString2, _Bool* isPotentiallyComm, _Bool* isPotentiallyLongComm, _Bool* isPotentiallyHexBin)
{

    if ((isalpha(*char1) || *char1 == '_') && !(*isPotentiallyHexBin) && !(*isPotentiallyWord) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyOp)  && !(*isPotentiallyString2) && !*isPotentiallyComm && !*isPotentiallyLongComm) // debut word
    {
        (*debTok)=i;
        (*isPotentiallyWord)=true;
    }

    if ((*char1)=='"' && !(*isPotentiallyHexBin) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !*isPotentiallyComm && !*isPotentiallyLongComm) // début de chaîne
    {
        (*debTok)=i;
        (*isPotentiallyString)=true;
    }
    
    
    if ((*char1)=='#' && !(*isPotentiallyHexBin) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !*isPotentiallyComm && !*isPotentiallyLongComm) // début de commentaire
    {
        (*debTok)=i;
        (*isPotentiallyComm)=true;
    }
        
    if ((*char1)=='$' && !(*isPotentiallyHexBin) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !*isPotentiallyComm && !*isPotentiallyLongComm) // début de commentaire long
    {
        (*debTok)=i;
        (*isPotentiallyLongComm)=true;
    }
    
    
    
    if ((*char1)=='\'' && !*isPotentiallyWord && !(*isPotentiallyHexBin) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !*isPotentiallyComm && !*isPotentiallyLongComm) // début de chaîne
    {
        (*debTok)=i;
        (*isPotentiallyString2)=true;
    }
    
    
    
    
    if (isdigit((*char1)) && !(*isPotentiallyHexBin) && !(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !*isPotentiallyComm && !*isPotentiallyLongComm) // début de nombre
    {
        (*debTok)=i;
        (*isPotentiallyNumber)=true;
        *isPotentiallyHexBin = true;
    }
    
    
    
    char *char1_2 = charToString(*char1);
    if (strlist_inList(&sousop, char1_2) && !(*isPotentiallyHexBin) && !(*isPotentiallyWord) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyOp)  && !(*isPotentiallyString2) && !*isPotentiallyComm && !*isPotentiallyLongComm) // debut operateur
    {
        (*debTok)=i;
        (*isPotentiallyOp)=true;
    }
    
    err_free(char1_2);
    
    return ;
}




_Bool isAcceptedWO(int type, int typeanc) {
    // cette fonction dit si oui ou non il peut y avoir type derrière typeanc, sans prendre en compte les opérateurs (Without Operators)
    for (int i = 0 ; i < syntax.tab[type].len ; i++) {
        if (syntax.tab[type].tab[i] == typeanc)
            return true;
    }
    
    return false;
}



// code d'erreur 30
_Bool isAccepted(char* token, int type, char* tokenAnc, int typeanc) {

    int gramm, grammact, grammanc;

    if (type > 19 || typeanc > 19)
        return false;
    
    if (!isAcceptedWO(type, typeanc)) {
        return false;
    }

    if (type == TYPE_OPERATOR) {
        // on vérifie que typeanc peut être avant cet opérateur
        gramm=gramm1.tab[strlist_index(&operateurs3, token)];
                        
        if (typeanc==TYPE_OPERATOR)
            grammanc=gramm1.tab[strlist_index(&operateurs3, tokenAnc)];


        if (((gramm==VARLEFT || gramm==VAR_RIGHT || gramm==VAR_VAR) && typeanc != TYPE_VARIABLE && typeanc != TYPE_LISTINDEX) || ((gramm==RIGHT || gramm==VARRIGHT)  && (typeanc==TYPE_NUMBER || typeanc==TYPE_STRING || typeanc==TYPE_LIST ||  typeanc==TYPE_VARIABLE ||  typeanc==TYPE_FONCTION ||  typeanc==TYPE_BOOL || typeanc==TYPE_CONST || typeanc==TYPE_NONE || typeanc==TYPE_EXCEPTION || typeanc==TYPE_LISTINDEX)) || ((gramm==LEFT_VAR || gramm==RIGHT_LEFT || gramm==VAR_VAR) && !(typeanc==TYPE_NUMBER || typeanc==TYPE_STRING || typeanc==TYPE_LIST ||  typeanc==TYPE_VARIABLE ||  typeanc==TYPE_FONCTION ||  typeanc==TYPE_BOOL ||  typeanc==TYPE_LISTINDEX || typeanc==TYPE_CONST || typeanc==TYPE_NONE || typeanc==TYPE_EXCEPTION || typeanc==TYPE_PARENTHESE2 || (typeanc==TYPE_OPERATOR && grammanc==VARLEFT))))
            return false;
        
    }

    if (typeanc == TYPE_OPERATOR) {
        // dans ce cas on vérifie que type peut suivre typeanc

        //recupere l’operateur pour verifier que ca correspond
        gramm=gramm1.tab[strlist_index(&operateurs3, tokenAnc)];
        
        
        if (type==TYPE_OPERATOR)
            grammact=gramm1.tab[strlist_index(&operateurs3, token)];
        
        
        // on verifie que le token peut effectivement etre apres l’operateur
        if ((gramm==VARLEFT && type != TYPE_ENDOFLINE && type != TYPE_VIRGULE && type != TYPE_PARENTHESE2 && !(type==TYPE_OPERATOR && grammact==RIGHT_LEFT)) || ((gramm==VARRIGHT || gramm == LEFT_VAR || gramm==VAR_VAR) && !(type==TYPE_LISTINDEX || type==TYPE_VARIABLE)) || ((gramm==RIGHT_LEFT || gramm==VAR_RIGHT || gramm==RIGHT || gramm==VAR_VAR) && (type==TYPE_ENDOFLINE || type==TYPE_VIRGULE || type==TYPE_PARENTHESE2)) || (gramm==VARRIGHT && type != TYPE_VARIABLE))
            return false;

    }

    return true;
}









void parse(strlist* tokenAdd, intlist typeTok, Ast** ast, int offset) {
    
    // cette fonction a pour rôle de créer les éléments de syntaxe tels que des blocs, des blockline, des fonctions, des listes, etc

    int debTok = 0, debTok2 = 0, debTok3 = 0, lastDebTok = 0, gramm = 0;

    // le rôle de lastDebTok est de retenir la dernière fois qu'on a fini une fonction ou un listindex afin de ne parser récursivement que le nouveau contenu

    _Bool nouvTokComp = false;

    //pour les tokens composés
    _Bool isPotentiallyListIndex=false;
    _Bool isPotentiallyList=false;
    _Bool isPotentiallyFonc=false;
    _Bool isPotentiallyBlock=false;
    _Bool isPotentiallyInst=false;
    
    //nombre d’accollades ouvrantes detectees et d’accollades fermantes
    //on ajoute 1 en rencontrant une ouverte et on enleve un en rencontrant une fermee
    int nbAcc=0;
    int nbPar=0; // pareil pour les parentheses
    int nbCro=0; // pour les crochets
    
    int foncParDeb=0; // va contenir le nombre de parenthèses déjà traversées au moment où on arrive à la fonction
    int listeParDeb=0; // va contenir le nombre de parenthèses déjà traversées au moment où on arrive à la liste
    int indexParDeb=0; // va contenir le nombre de parenthèses déjà traversées au moment où on arrive à l'index de liste
    
    
    
    
    //etapes de detection par exemple variable, puis parenthese "(" puis parenthese ")"
    int listIndexStep=0;
    int foncStep=0;
    int instStep=0;

    // ce tableau contient à chaque élément l'élément se syntaxe le plus extérieur qui commence ici, et son type


    for (int k=0 ; k < tokenAdd->len ; k++) // boucle pour chaque nouveau token ajoute. chaque tour de cette boucle est comme un tour de boucle normal sur tokens. cela permet d’eviter une boucle supplementaire                
    {
        // savoir si on a un operateur ou un mot cle ou un booleen
        if (typeTok.tab[k] == TYPE_OPERATOR) // si il a déjà été détecté comme opérateur, on vérifie si c'est bien un opérateur valide
        {
            if (!(strlist_inList(&operateurs1, tokenAdd->tab[k])))
            {
                CODE_ERROR = 24; // opérateur inconnu
                return;
            }
        }
        
        
        
        else if (typeTok.tab[k] == TYPE_WORD)
        {
            if (strlist_inList(&operateurs2, tokenAdd->tab[k]))
            {
                typeTok.tab[k] = TYPE_OPERATOR;
            }
            else if (strlist_inList(&boolean, tokenAdd->tab[k]))
            {
                typeTok.tab[k] = TYPE_BOOL;
            }

            else if (strlist_inList(&exceptions, tokenAdd->tab[k]))
            {
                typeTok.tab[k] = TYPE_EXCEPTION;
            }
            
            else if (strlist_inList(&constant, tokenAdd->tab[k]))
            {
                if (strcmp(tokenAdd->tab[k], "None")==0)
                    typeTok.tab[k] = TYPE_NONE;
                else
                    typeTok.tab[k] = TYPE_CONST;
            }

            else if (strlist_inList(&keywordFunction, tokenAdd->tab[k]))
            {
                typeTok.tab[k] = TYPE_KEYWORDFUNCTION;
            }

                
            else if (strlist_inList(&keywords, tokenAdd->tab[k]))
            {
                typeTok.tab[k] = TYPE_KEYWORD;
            }
            
            else if (strlist_inList(&blockwords, tokenAdd->tab[k]))
            {
                
                typeTok.tab[k] = TYPE_BLOCKWORD;
            }
            
            else
            {
                typeTok.tab[k] = TYPE_VARIABLE;
            }

            ast[k]->type = typeTok.tab[k];
        
        }
        
        
        //etapes de fonction et d’index de liste
        
        // a chaque nouveau token que l’on reçoit, on effectue les memes verifications que pour les sous tokens a savoir les verifications de grammaire pour les tokens composes
        
        //sorte de pile qui compte les accolades et les parentheses
        if (tokenAdd->tab[k][0]=='{')
            nbAcc++;
        else if (tokenAdd->tab[k][0]=='}')
            nbAcc--;
        
        
        if (tokenAdd->tab[k][0]=='(')
            nbPar++;
        else if (tokenAdd->tab[k][0]==')')
            nbPar--;
        
        
        if (tokenAdd->tab[k][0]=='[')
            nbCro++;
        else if (tokenAdd->tab[k][0]==']')
            nbCro--;
        

        // vérification du nombre de parentheses
        if (nbAcc < 0 || nbCro < 0 || nbPar < 0) {
            CODE_ERROR = 68;
            return;
        }


        
        // annuler ou poursuivre les fonctions, les instructions, les index de listes....
        
        annulerOuPoursuivre(&isPotentiallyFonc, &foncStep, &isPotentiallyInst, &instStep, &isPotentiallyListIndex, &listIndexStep, tokenAdd, &typeTok, k, &foncParDeb, &indexParDeb, &listeParDeb, &nbCro);


        
        // fins de fonctions, d'instructions, d'index de listes...
        
        finsTypesComposes(&debTok2, &debTok3, &lastDebTok, k, &nouvTokComp, &isPotentiallyFonc, &foncStep, tokenAdd, typeTok, &nbPar, &isPotentiallyInst, &instStep, &listIndexStep, &isPotentiallyList, &isPotentiallyListIndex, &nbCro, &isPotentiallyBlock, &nbAcc, &foncParDeb, &indexParDeb, &listeParDeb, ast, offset);

        // fin des fins et debut des debuts (hi hi hi)

        if (CODE_ERROR != 0)
        {
            return;
        }

        
        debutTokensComposes(k, &typeTok, tokenAdd, &debTok2, &debTok3, &lastDebTok, &nouvTokComp, &isPotentiallyList, &isPotentiallyFonc, &isPotentiallyListIndex, &isPotentiallyBlock, &isPotentiallyInst, &listIndexStep, &foncStep, &instStep, &foncParDeb, &indexParDeb, &listeParDeb, &nbPar, &nbCro);
        
    }

    if (nbAcc != 0 || nbCro != 0 || nbPar != 0) //isPotentiallyBlock || isPotentiallyFonc || isPotentiallyList || isPotentiallyListIndex || isPotentiallyInst)
    {
        CODE_ERROR = 26;
        return;
    }
    

    // a la fin du parsing, on vérifie la syntaxe

    if (tokenAdd->len > 0) {

        _Bool plusieursTokens = ast[0]->fin - offset < tokenAdd->len-1; // l'indice de fin du 1er token est < au dernier element

        if (ast[0]->type == TYPE_OPERATOR)
            gramm = gramm1.tab[strlist_index(&operateurs3, tokenAdd->tab[0])];

        if (!intlist_inList(&types_debut, ast[0]->type) || (ast[0]->type == TYPE_OPERATOR && gramm != VARRIGHT && gramm != RIGHT))
        {
            CODE_ERROR = 32;
            return ;
        }


        // ces deux variables sont des indices indiquant quel est le dernier token fabriqué et l'avant-dernier token fabriqué
        int i_act = ast[0]->fin + 1 - offset;
        int i_anc = 0;


        while (plusieursTokens) { // si on a plus d'un token composé ou simple

            //printf("types : %d   %d\n", ast[i_anc]->type, ast[i_act]->type);


            // on va traiter l'opérateur moins (-) selon si c'est un unaire ou pas

            // ici on s'occuppe de modifier les signes '-'
            if (ast[i_act]->type==TYPE_OPERATOR && tokenAdd->tab[i_act][0] == '-' && tokenAdd->tab[i_act][1] == '\0') // si on a un opérateur et que cet opérateur est un moins
            {
                if (i_act == 0)
                {
                    err_free(tokenAdd->tab[i_act]);
                    tokenAdd->tab[i_act] = strdup("_"); // si j=0 (c'est-à-dire que c'est le premier token), on sait que c'est forcément un moins unaire
                }
                
                else if (!(ast[i_anc]->type == TYPE_NUMBER || ast[i_anc]->type == TYPE_STRING || ast[i_anc]->type == TYPE_LISTINDEX || ast[i_anc]->type == TYPE_FONCTION || ast[i_anc]->type == TYPE_LIST || ast[i_anc]->type == TYPE_BOOL || ast[i_anc]->type == TYPE_CONST || ast[i_anc]->type == TYPE_NONE || ast[i_anc]->type == TYPE_EXCEPTION || ast[i_anc]->type == TYPE_PARENTHESE2 || ast[i_anc]->type == TYPE_VARIABLE || (ast[i_anc]->type==TYPE_OPERATOR && gramm1.tab[strlist_index(&operateurs3, tokenAdd->tab[i_anc])] == VARLEFT)) )
                {
                    // si on a un moins qui est directement précédé d'une autre unité lexicale de type nombre etc, ça veut dire que c'est l'opérateur moins donc
                    // là on teste la condition inverse c'est à dire que l'on se trouve en présence d'un moins unaire
                    // on va intégrer le moins à l'unité lexicale à qui il est adressé
                    if (ast[i_act]->fin - offset < tokenAdd->len - 1) // si le moins étudié n'est pas le dernier caractère des tokens
                    {
                        err_free(tokenAdd->tab[i_act]);
                        tokenAdd->tab[i_act] = strdup("_");
                    }
                }
            }



            if (!isAccepted(tokenAdd->tab[i_act], ast[i_act]->type, tokenAdd->tab[i_anc], ast[i_anc]->type))
            {
                CODE_ERROR = 30;
                return;
            }

            if (ast[i_act]->fin + 1 - offset > tokenAdd->len - 1)
                break;

            i_anc = i_act;
            i_act = ast[i_act]->fin + 1 - offset;
        }
        
        if (ast[i_act]->type == TYPE_OPERATOR)
            gramm = gramm1.tab[strlist_index(&operateurs3, tokenAdd->tab[i_act])];

        if ((plusieursTokens && (!intlist_inList(&types_fin, ast[i_act]->type) || (ast[i_act]->type == TYPE_OPERATOR && gramm != VARLEFT))) || (!plusieursTokens && !intlist_inList(&types_fin, ast[0]->type)))
        {
            CODE_ERROR = 32;
            return ;
        }
    }
}



void annulerOuPoursuivre(_Bool* isPotentiallyFonc, int* foncStep, _Bool* isPotentiallyInst, int* instStep, _Bool* isPotentiallyListIndex, int* listIndexStep, strlist* tokenAdd, intlist* typeTok, int k, int *foncParDeb, int *listeParDeb, int *indexParDeb, int *nbCro)
{

    // quand on arrête un isPotentiallyTruc, c'est qu'on accepte un nouveau token

    if ((*isPotentiallyFonc) && (*foncStep)==1) // annuler ou poursuivre la fonction
    {
        
        if (tokenAdd->tab[k][0]=='(' && strlen(tokenAdd->tab[k])==1)
            (*foncStep)=2;
        else if (typeTok->tab[k] != TYPE_VARIABLE)
        {
            *foncStep = 0;
            *isPotentiallyFonc = 0;
        }
    }
    
    
    
    if ((*isPotentiallyInst) && (*instStep)==1) // annuler ou poursuivre l’instruction de bloc
    {
        if (tokenAdd->tab[k][0]=='(' && strlen(tokenAdd->tab[k])==1)
            (*instStep)=2;
        else
        {
            (*instStep)=0;
            (*isPotentiallyInst)=false;
        }
    }

    if ((*isPotentiallyListIndex) && (*listIndexStep)==1) // annuler ou poursuivre la fonction
    {
        if (tokenAdd->tab[k][0]=='[' && tokenAdd->tab[k][1] == '\0')
            (*listIndexStep)=2;
        else if (typeTok->tab[k] != TYPE_VARIABLE)
        {
            *listIndexStep = 0;
            *isPotentiallyListIndex = 0;
        }
    }
    
    return;
}






void debutTokensComposes(int k, intlist* typeTok, strlist* tokenAdd, int* debTok2, int* debTok3, int* lastDebTok, _Bool* nouvTokComp, _Bool* isPotentiallyList, _Bool* isPotentiallyFonc, _Bool* isPotentiallyListIndex, _Bool* isPotentiallyBlock, _Bool* isPotentiallyInst, int* listIndexStep, int *foncStep, int *instStep, int *foncParDeb, int *listeParDeb, int *indexParDeb, int* nbPar, int* nbCro)
{
    
    if (tokenAdd->tab[k][0] =='[' && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyBlock) && !(*isPotentiallyInst)) // debut de liste
    {
        (*debTok2) = k;
        (*isPotentiallyList)=true;
    }



    if (tokenAdd->tab[k][0] =='{' && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyBlock) && !(*isPotentiallyInst)) // debut de bloc d’instructions
    {
        (*debTok3)=k;
        (*isPotentiallyBlock)=true;
    }



    

    if (typeTok->tab[k] == TYPE_VARIABLE && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyBlock) && !(*isPotentiallyInst)) // debut de fonction et d’index de liste
    {
        *debTok2 = k;
       *lastDebTok = k;
       (*isPotentiallyFonc)=true;
       (*isPotentiallyListIndex)=true;
       (*listIndexStep)=1;
       (*foncStep)=1;
       
       *foncParDeb = *nbPar;
       *indexParDeb = *nbCro;
    }
    


    if (typeTok->tab[k] == TYPE_BLOCKWORD && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyInst) ) // debut d’instruction de bloc
    {
       (*debTok2)=k;
       (*isPotentiallyInst)=true;
       (*instStep)=1;
    }


    if ((*nouvTokComp)) // reinitialise les variables de detection de tokens car on repart a zero avec un nouveau token
    {
        // on ne réinitialise pas tout car un listindex peut en cacher un autre plus gros
        (*isPotentiallyList)=false;
        (*isPotentiallyInst)=false;
        
        (*nouvTokComp)=false;
    }
    return;
}











void finsTypesComposes(int *debTok2, int *debTok3, int* lastDebTok, int k, _Bool *nouvTokComp, _Bool *isPotentiallyFonc, int *foncStep, strlist* tokenAdd, intlist typeTok, int *nbPar, _Bool *isPotentiallyInst, int *instStep, int *listIndexStep, _Bool *isPotentiallyList, _Bool *isPotentiallyListIndex, int *nbCro, _Bool *isPotentiallyBlock, int *nbAcc, int *foncParDeb, int *listeParDeb, int *indexParDeb, Ast** ast, int offset)
{

    //printf("%d %d %d %d %d %d %c\n", *isPotentiallyListIndex, *listIndexStep, k, *nbCro, *indexParDeb, tokenAdd->len, tokenAdd->tab[k][0]);

    if (*isPotentiallyFonc && *foncStep==2 && tokenAdd->tab[k][0] == ')' && (*nbPar - *foncParDeb)==0) // fin fonction
    {
        // on ajoute une autre unité syntaxique plus grande

        ast_push(ast[*debTok2]);
        ast[*debTok2]->type = TYPE_FONCTION;
        ast[*debTok2]->fin = k + offset;

        // il ne faut parser récursivement que dans la partie que l'on a ajoutée
        
        // appel récursif sur parse
        strlist tokens2 = (strlist) {.tab = tokenAdd->tab + *lastDebTok + 2, .len = k - *lastDebTok - 2};
        intlist types2 = (intlist) {.tab = typeTok.tab + *lastDebTok + 2, .len = k - *lastDebTok - 2};
        parse(&tokens2, types2, ast + *lastDebTok + 2, *lastDebTok + 2 + offset);

        if (CODE_ERROR != 0)
            return ;


        // une exécution de fonction peut en cacher une autre
        *isPotentiallyFonc = true;
        *isPotentiallyListIndex = true;
        *foncStep = 1;
        *listIndexStep = 1;
        *lastDebTok = k;

        (*nouvTokComp)=true;
        *foncParDeb = 0;
    }

    else if ((*isPotentiallyFonc) && (*foncStep)==2 && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
    {
        CODE_ERROR = 28; // mot clé incompatible utilisé dans une expression
        return;
    }
    
    
    
    else if ((*isPotentiallyInst) && (*instStep)==2 && tokenAdd->tab[k][0] == ')' && (*nbPar)==0) // fin instruction de bloc
    {
        (*isPotentiallyInst)=false;
        (*instStep)=0;

        ast_push(ast[*debTok2]);

        ast[*debTok2]->fin = k + offset;
        
        if (strlist_inList(&blockwords1Line, tokenAdd->tab[*debTok2]))
            ast[*debTok2]->type = TYPE_BLOCKWORD1LINE;
        else
            ast[*debTok2]->type = TYPE_BLOCKLINE;

            
        (*nouvTokComp)=true;
    }
    
    else if ((*isPotentiallyInst) && (*instStep)==2 && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
    {
        CODE_ERROR = 28; // mot clé incompatible utilisé dans une expression
        return;
    }


    else if (*isPotentiallyListIndex && *listIndexStep==2 && tokenAdd->tab[k][0] == ']' && (*nbCro - *indexParDeb)==0) // fin index de liste
    {

        ast_push(ast[*debTok2]);

        ast[*debTok2]->type = TYPE_LISTINDEX;
        ast[*debTok2]->fin = k + offset;

        // il ne faut parser récursivement que dans la partie que l'on a ajoutée
        
        // appel récursif sur parse

        if (k - *lastDebTok - 2 == 0) {
            CODE_ERROR = 30;
            return;
        }



        strlist tokens2 = (strlist) {.tab = tokenAdd->tab + *lastDebTok + 2, .len = k - *lastDebTok - 2};
        intlist types2 = (intlist) {.tab = typeTok.tab + *lastDebTok + 2, .len = k - *lastDebTok - 2};
        parse(&tokens2, types2, ast + *lastDebTok + 2, *lastDebTok + 2 + offset);

        if (CODE_ERROR != 0)
            return ;

        *nouvTokComp=true;
        *indexParDeb = 0;

        // on vient de compute un list index. Mais potentiellement ce listindex est une liste pour un autre list index
        *isPotentiallyFonc = true;
        *isPotentiallyListIndex = true;
        *foncStep = 1;
        *listIndexStep = 1;
        // ce nouveau potentiel listindex commence toujours à debTok2
        // en revanche il faut se souvenir d'ou il commence vraiment pour le parsing
        *lastDebTok = k;

    }
      
    else if ((*isPotentiallyListIndex) && ((*listIndexStep)==2 || (*listIndexStep)==3) && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
    {
        CODE_ERROR = 28; // mot clé incompatible utilisé dans une expression
        return;
    }
      
    
    
    
    
    else if (tokenAdd->tab[k][0] == ']' && (*isPotentiallyList) && *nbCro == 0)// fin de liste
    {
        (*isPotentiallyList)=false;

        ast_push(ast[*debTok2]);

        ast[*debTok2]->type = TYPE_LIST;
        ast[*debTok2]->fin = k + offset;

        // appel récursif sur parse
        strlist tokens2 = (strlist) {.tab = tokenAdd->tab + *debTok2 + 1, .len = k - *debTok2 - 1};
        intlist types2 = (intlist) {.tab = typeTok.tab + *debTok2 + 1, .len = k - *debTok2 - 1};
        parse(&tokens2, types2, ast + *debTok2 + 1, *debTok2 + 1 + offset);

        if (CODE_ERROR != 0)
            return;

        
        (*nouvTokComp)=true;
    }

    else if ((*isPotentiallyList) && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
    {
        CODE_ERROR = 28; // mot clé incompatible utilisé dans une expression
        return;
    }

    
    
    else if (tokenAdd->tab[k][0] =='}' && (*isPotentiallyBlock) && (*nbAcc)==0)// fin de bloc d’instructions
    {
        (*isPotentiallyBlock)=false;

        ast_push(ast[*debTok3]);
        ast[*debTok3]->type = TYPE_BLOCK;
        ast[*debTok3]->fin = k + offset;

        // appel récursif sur parse à l'intérieur du bloc
        strlist tokens2 = (strlist) {.tab = tokenAdd->tab + *debTok3 + 1, .len = k - *debTok3 - 1};
        intlist types2 = (intlist) {.tab = typeTok.tab + *debTok3 + 1, .len = k - *debTok3 - 1};
        parse(&tokens2, types2, ast + *debTok3 + 1, *debTok3 + 1 + offset);


        (*nouvTokComp)=true;
    }
    
    
    return;
}





void statements(intlist* types, strlist* tokens, Ast** ast, int offset)
{

    // statements est censé être récursif

    _Bool isPotentiallyConBlock = false, isPotentiallyStat = false, isPotentiallyTE = false;
    int funcMethStep = 0;
    int conBlockStep = 0;
    int debConBlock = 0;
    int debStat = 0;
    int debFunc = 0;
    int debTE = 0;
    int TEStep = 0;
    int i=0;
    char typeact=ast[0]->type;
    
    
    while (true)
    {

        typeact = ast[i]->type;
        
        // empaquetage bloc de fonction

        if (ast[i]->type == TYPE_BLOCK && ! isPotentiallyStat && funcMethStep != 2)//un block non précédé d'un blockline
        {
            // Erreur de syntaxe : Bloc d'instructions tout seul
            CODE_ERROR = 34;
            return;
        }

        
        if (funcMethStep == 2)
        {
            if (ast[i]->type == TYPE_BLOCK)
            {
                funcMethStep = 0;
                
                _Bool isMethod = strcmp("method", tokens->tab[debFunc]) == 0;

                ast_push(ast[i-2]);
                ast[debFunc]->type = (isMethod) ? TYPE_METHODDEF : TYPE_FUNCTIONDEF;
                ast[debFunc]->fin = ast[i]->fin;

                // appel récursif à faire
            }
            else if (typeact != TYPE_ENDOFLINE)
            {
                CODE_ERROR = 11;
                return;
            }
        }


        
        if (funcMethStep == 1)
        {
            if (ast[i]->type == TYPE_FONCTION && ast[i]->fin - offset < tokens->len - 1)
            {
                funcMethStep = 2;
            }
            else if (ast[i]->type != TYPE_ENDOFLINE)
            {
                CODE_ERROR = 11;
                return;
            }
        }


        
        if (ast[i]->type == TYPE_KEYWORDFUNCTION)
        {
            if (ast[ast[i+1]->fin - offset + 1]->fin - offset < tokens->len) {
                funcMethStep = 1;
                debFunc = i;
            }
            else
            {
                CODE_ERROR = 11;
                return;
            }
        }


        
        
        // modifie le blockline pour savoir quel bloc c'est particulièrement
        if (ast[i]->type==TYPE_BLOCKLINE || (ast[i]->type == TYPE_KEYWORD && !strlist_inList(&lkeywords, tokens->tab[i])))
        {
            // ici il faut créer le TYPE_BLOCKTYPE

            if (strcmp(tokens->tab[i], "if")==0)
                ast[i]->type = TYPE_BLOCKIF;
            
            if (strcmp(tokens->tab[i], "ei")==0)
                ast[i]->type = TYPE_BLOCKELIF;
            
            if (strcmp(tokens->tab[i], "es")==0)
                ast[i]->type = TYPE_BLOCKELSE;
            
            if (strcmp(tokens->tab[i], "while")==0)
                ast[i]->type = TYPE_BLOCKWHILE;
            
            if (strcmp(tokens->tab[i], "for")==0)
                ast[i]->type = TYPE_BLOCKFOR;

            if (strcmp(tokens->tab[i], "tr")==0)
                ast[i]->type = TYPE_BLOCKTRY;
            
            if (strcmp(tokens->tab[i], "expt")==0)
                ast[i]->type = TYPE_BLOCKEXCEPT;

            if (strcmp(tokens->tab[i], "atomic") == 0)
                ast[i]->type = TYPE_ATOMICLINE;
            
        }

        typeact=ast[i]->type;


        // on vérifie entre les moments où on finit des statements que les trucs sont bien ce qu'il faut


        if (isPotentiallyTE && typeact == TYPE_BLOCK && TEStep == 2)
        {
            TEStep = 0;
        }
        else if (TEStep == 2 && typeact != TYPE_ENDOFLINE) {
            CODE_ERROR = 33;
            return;
        }



        if (isPotentiallyTE && typeact == TYPE_BLOCKEXCEPT && TEStep == 1)
        {
            TEStep = 2;
        }
        else if (TEStep == 1 && typeact != TYPE_ENDOFLINE) {
            CODE_ERROR = 33;
            return;
        }
        
        // conditions de fin de conblock

        // ast[ast[i]->fin - offset + 1]->type c'est pour vérifier le type du token composé d'après
        
        if (isPotentiallyConBlock && conBlockStep == 1 && (ast[i]->fin - offset == tokens->len - 1 || (ast[ast[i]->fin - offset + 1]->type != TYPE_ENDOFLINE && ast[ast[i]->fin - offset + 1]->type != TYPE_BLOCKIF && ast[ast[i]->fin - offset + 1]->type != TYPE_BLOCKELSE && ast[ast[i]->fin - offset + 1]->type != TYPE_BLOCKELIF && ast[ast[i]->fin - offset + 1]->type != TYPE_BLOCK))) {
            // fin conblock si le type d'après ne correspond pas ou qu'on est à la fin
            ast_push(ast[debConBlock]);
            ast[debConBlock]->fin = ast[i]->fin;
            ast[debConBlock]->type = TYPE_CONDITIONBLOCK;
            isPotentiallyConBlock = false;
        }

        else if (isPotentiallyConBlock && conBlockStep == 1) {
            if (typeact == TYPE_BLOCKIF || typeact == TYPE_BLOCKELSE || typeact == TYPE_BLOCKELIF)
                conBlockStep = 2;
            else if (typeact != TYPE_ENDOFLINE) {
                CODE_ERROR = 33;
                return;
            }
        }

        else if (isPotentiallyConBlock && conBlockStep == 2) {
            if (typeact == TYPE_BLOCK)
                conBlockStep = 1;
            else if (typeact != TYPE_ENDOFLINE) {
                CODE_ERROR = 33;
                return;
            }
        }
        
        
        ////////////// FIN DE STATEMENT /////////////////
        
        
        if (isPotentiallyStat && typeact==TYPE_BLOCK) // fin de statement, on l'empaquette
        {

            ast_push(ast[debStat]);
            
            // ici, il faut créer le STATEMENTTYPE
            
            switch (ast[debStat]->suiv->type)
            {
                case TYPE_BLOCKFOR :
                    ast[debStat]->type = TYPE_STATEMENTFOR;
                    break;
                
                case TYPE_BLOCKWHILE :
                    ast[debStat]->type = TYPE_STATEMENTWHILE;
                    break;
                
                case TYPE_BLOCKIF :
                    ast[debStat]->type = TYPE_STATEMENTIF;
                    break;
                
                case TYPE_BLOCKELIF :
                    ast[debStat]->type = TYPE_STATEMENTELIF;
                    break;
                
                case TYPE_BLOCKELSE :
                    ast[debStat]->type = TYPE_STATEMENTELSE;
                    break;

                case TYPE_BLOCKTRY :
                    ast[debStat]->type = TYPE_STATEMENTTRY;
                    break;

                case TYPE_BLOCKEXCEPT :
                    ast[debStat]->type = TYPE_STATEMENTEXCEPT;
                    break;
                case TYPE_ATOMICLINE:
                    ast[debStat]->type = TYPE_ATOMICBLOCK;
                    break;
            }

            ast[debStat]->fin = ast[i]->fin;

            // appel récursif de statements

            int longueur = ast[i]->fin - offset - i - 1;


            strlist tokens2 = (strlist) {.tab = tokens->tab + i + 1, .len = longueur};
            intlist types2 = (intlist) {.tab = types->tab + i + 1, .len = longueur};
            Ast** ast2 = ast + i + 1;

            statements(&types2, &tokens2, ast2, offset + i + 1);




            ////// ICI LE TRAITEMENT POST-STATEMENT

            // --------- CONBLOCK ------------

            if (ast[debStat]->type == TYPE_STATEMENTIF) // debut conblock
            {
                conBlockStep = 1;
                isPotentiallyConBlock = true;
                debConBlock = debStat;
            }





            // --------- TRY/EXCEPT ------------


            if (isPotentiallyTE && ast[debStat]->type == TYPE_STATEMENTEXCEPT) {
                // on réunit les deux
                ast_push(ast[debTE]);
                ast[debTE]->fin = ast[i]->fin;
                ast[debTE]->type = TYPE_TRYEXCEPT;
                isPotentiallyTE = false;
            }
            else if (ast[debStat]->type == TYPE_STATEMENTEXCEPT) {
                CODE_ERROR = 33;
                return;
            }



            if (ast[debStat]->type == TYPE_STATEMENTTRY) { // debut try except
                if (ast[debStat]->fin - offset == tokens->len-1)
                {
                    CODE_ERROR = 77;
                    return;
                }
                else {
                    debTE = debStat;
                    isPotentiallyTE = true;
                    TEStep = 1;
                }
            }
            
            
            isPotentiallyStat=false;
            debStat=0;
        }
        else if (isPotentiallyStat && typeact != TYPE_ENDOFLINE && typeact != TYPE_ACCOLADE) {
            CODE_ERROR = 27;
            return;
        }


        

        

        /////////////// DEBUT DES STATEMENTS ///////////////
        
        if (typeact==TYPE_ATOMICLINE || typeact==TYPE_BLOCKTRY || typeact == TYPE_BLOCKEXCEPT || typeact==TYPE_BLOCKFOR || typeact==TYPE_BLOCKWHILE || typeact==TYPE_BLOCKIF || typeact==TYPE_BLOCKELSE || typeact==TYPE_BLOCKELIF) // debut statement
        {
            if (ast[i]->fin - offset == tokens->len - 1)
            {
                CODE_ERROR = 29; // Ligne de bloc d'instructions non suivi par un bloc d'instructions (ex: un if() tout seul)
                return;
            }
            isPotentiallyStat=true;
            debStat=i;
        }
        
        if (ast[i]->fin + 1 - offset > tokens->len - 1)
            break;
        
        i = ast[i]->fin + 1 - offset;
        
    }

    if (isPotentiallyStat || funcMethStep != 0) {
        CODE_ERROR = 29;
    }
    else if (isPotentiallyTE)
        CODE_ERROR = 33;


    return ;
}