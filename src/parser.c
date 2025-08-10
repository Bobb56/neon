#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "headers/constants.h"
#include "headers/neon.h"
#include "headers/neonio.h"
#include "headers/dynarrays.h"
#include "headers/operators.h"
#include "headers/strings.h"
#include "headers/parser.h"



// définition des élément modifiables de la syntaxe

static strlist acceptedChars = (strlist) {
    .tab = (char*[]){"\"", "'", "+","*","-","/","<",">","=","%","&","@","!", ",", ";", "\n", "#", "$", "[", "]", "(", ")", "{", "}", "\\", ".", "_", " ", "\t", ".", ":", "~"},
    .len = 32,
    .capacity = 5
};


// definit les types de tokens composes et indique par quel type de token compose ils peuvent etre precedes
static listlist syntax = (listlist) {
    .tab = (intlist[]) {
        {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_KEYWORDFUNCTION} , .len = 5}, // suivant la grammaire de l’operateur
        {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
        {.tab = (int[]) {TYPE_PARENTHESE1, TYPE_DOUBLE, TYPE_INTEGER, TYPE_EXPRESSION, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_FONCTION, TYPE_BOOL, TYPE_LISTINDEX, TYPE_ENDOFLINE,  TYPE_OPERATOR, TYPE_PARENTHESE2, TYPE_CONST, TYPE_VIRGULE, TYPE_EXCEPTION} , .len = 16},//suivant la grammaire de chaque operateur
        {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
        {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_CROCHET} , .len = 5}, // suivant la grammaire de l’operateur
        {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_CROCHET} , .len = 5}, // suivant la grammaire de l’operateur
        {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
        {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_BLOCKLINE, TYPE_KEYWORD, TYPE_FONCTION} , .len = 4},
        {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE,TYPE_PARENTHESE2} , .len = 5}, // suivant la grammaire de l’operateur
        {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
        {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
        {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_BLOCK} , .len = 2},
        {.tab = (int[]) {TYPE_VIRGULE, TYPE_DOUBLE, TYPE_INTEGER, TYPE_EXPRESSION, TYPE_PARENTHESE1, TYPE_VARIABLE, TYPE_LISTINDEX, TYPE_BOOL, TYPE_OPERATOR, TYPE_LIST, TYPE_STRING, TYPE_BLOCKLINE, TYPE_BLOCK, TYPE_FONCTION, TYPE_PARENTHESE2, TYPE_KEYWORD, TYPE_CONST, TYPE_BLOCKWORD1LINE, TYPE_EXCEPTION} , .len = 19}, // suivant la grammaire de l’operateur
        {.tab = (int[]) {TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_VIRGULE, TYPE_PARENTHESE1} , .len = 4},
        {.tab = (int[]) {TYPE_OPERATOR, TYPE_DOUBLE, TYPE_INTEGER, TYPE_EXPRESSION, TYPE_STRING, TYPE_LIST, TYPE_FONCTION, TYPE_LISTINDEX, TYPE_VARIABLE, TYPE_PARENTHESE2,TYPE_BOOL} , .len = 11},
        {.tab = (int[]) {TYPE_DOUBLE, TYPE_INTEGER, TYPE_EXPRESSION, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_FONCTION, TYPE_BOOL, TYPE_LISTINDEX, TYPE_ENDOFLINE, TYPE_OPERATOR, TYPE_CONST, TYPE_EXCEPTION} , .len = 13},
        {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_BLOCK} , .len = 2},
        {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
        {.tab = (int[]) {TYPE_ENDOFLINE}, .len = 1},
        {.tab = (int[]) {TYPE_ENDOFLINE}, .len = 1},
        {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE}, .len = 4},
        {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_CROCHET} , .len = 5}
    },
    .len = 22,
    .capacity = 5
};


// types acceptés en début d'entrée
static intlist types_debut = (intlist) {
    .tab = (int[]) {TYPE_FONCTION, TYPE_PARENTHESE1, TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_LISTINDEX, TYPE_DOUBLE, TYPE_INTEGER, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_BOOL, TYPE_BLOCKLINE, TYPE_KEYWORD, TYPE_CONST, TYPE_WORD, TYPE_COMMENT, TYPE_CROCHET, TYPE_BLOCKWORD, TYPE_BLOCKWORD1LINE, TYPE_EXPRESSION, TYPE_EXCEPTION},
    .len = 21,
    .capacity = 5
};

static intlist types_fin = (intlist) {
    .tab = (int[]) {TYPE_FONCTION, TYPE_PARENTHESE2, TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_LISTINDEX, TYPE_DOUBLE, TYPE_INTEGER, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_BOOL, TYPE_KEYWORD, TYPE_CONST, TYPE_WORD, TYPE_COMMENT, TYPE_CROCHET, TYPE_EXPRESSION, TYPE_EXCEPTION, TYPE_BLOCK, TYPE_BLOCKWORD1LINE},
    .len = 20,
    .capacity = 5
};



static strlist sousop = (strlist) {
    .tab = (char*[]) {"+","*","-","/","<",">","=","%","&","@","!", ".",":"},
    .len = 13,
    .capacity = 4
};





//définition de operateurs1
// opérateurs composés de caractères de sousop
static strlist operateurs1 = (strlist) {
    .tab = (char*[]) {"+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","%","//","&","<-","@", "->", ".", "=>", ">>", ":", "<->", ":=", "..."},
    .len = 31,
    .capacity = 5
};


// opérateurs mots
static strlist operateurs2 = (strlist) {
    .tab = (char*[]) {"and","or","xor","not", "del", "EE", "in", "parallel"},
    .len = 8,
    .capacity = 3
};


static strlist OPERATEURS = (strlist) {
    .tab = (char*[]) {"and","or","xor","+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","not","%","//","&","<-","@", "_", "del", "->", ".", "EE", "=>", "in", ">>", ":", "<->", ":=", "...", "parallel"},
    .len = NBOPERATEURS,
    .capacity = NBOPERATEURS
};


static intlist PRIORITE = (intlist) {
    .tab = (int[]) {7, 7, 7, 5, 4, 5, 4, 3, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, 3, 3, 7, 4, 4, 2, 8, 2, 1, 8, 8, 2, 3, 7, 6, 0, 8, 8, 8, 8, 7},
    .len = NBOPERATEURS,
    .capacity = NBOPERATEURS // les tableaux ne sont jamais modifiés de toute façon, donc la capacité est anecdotique
};


static intlist OPERANDES = (intlist) {
    .tab = (int[]) {RIGHT_LEFT | LAZY,RIGHT_LEFT | LAZY,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,RIGHT, RIGHT, VARRIGHT, LEFT_VAR, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT | LAZY, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, VAR_VAR, VAR_RIGHT, SPECIAL, RIGHT},
    .len = NBOPERATEURS,
    .capacity = NBOPERATEURS // les tableaux ne sont jamais modifiés de toute façon, donc la capacité est anecdotique
};


// ------------------

// définit les tokens de délimitation de blocks

//mots de blocs d'instructions
static strlist blockwords = (strlist) {
    .tab = (char*[]) {"if","while","for", "foreach", "ei", "return", "import", "local", "tr", "expt", "await", "atmc"},
    .len = 12,
    .capacity = 4
};

static strlist blockwords1Line = (strlist) {
    .tab = (char*[]) {"return", "import", "local", "await"},
    .len = 4,
    .capacity = 2
};

static strlist keywordFunction = (strlist) {
    .tab = (char*[]) {"function", "method"},
    .len = 2,
    .capacity = 1
};

static strlist keywords = (strlist) {
    .tab = (char*[]) {"es", "continue", "break", "pass", "tr", "atmc"},
    .len = 6,
    .capacity = 3
};

static strlist lkeywords = (strlist) {
    .tab = (char*[]) {"continue", "break", "pass"},
    .len = 3,
    .capacity = 2
};

static strlist neon_boolean = (strlist) {
    .tab = (char*[]) {"True","False"},
    .len = 2,
    .capacity = 1
};

static strlist constant = (strlist) {
    .tab = (char*[]) {"None", "Infinity", "NaN", "Pi"},
    .len = 4,
    .capacity = 2
};

char* get_infinity(void) {
    return constant.tab[1];
}

char* get_nan(void) {
    return constant.tab[2];
}

char* get_pi(void) {
    return constant.tab[3];
}

char* get_none(void) {
    return constant.tab[0];
}

int get_operator_index(char* operator) {
    return strlist_index(&OPERATEURS, operator);
}


int get_type_operande_index(int index) {
    return OPERANDES.tab[index];
}

int get_type_operande(char* operator_string) {
    return get_type_operande_index(get_operator_index(operator_string));
}

char* get_True(void) {
    return neon_boolean.tab[0];
}

bool operatorIs(int index, int mask) {
    return get_type_operande_index(index) & mask;
}

int get_blockword1Line_index(char* word) {
    return strlist_index(&blockwords1Line, word);
}

int get_lkeywords_index(char* word) {
    return strlist_index(&lkeywords, word);
}



///////////// DÉFINITION DE LA STRUCTURE AST //////////////


Ast** ast_create(intlist* typeTok) {
    Ast** ast = malloc(sizeof(Ast*) * typeTok->len);
    for (int i = 0 ; i < typeTok->len ; i++) {
        ast[i] = malloc(sizeof(Ast));
        ast[i]->type = typeTok->tab[i];
        ast[i]->fin = i;
        ast[i]->suiv = NULL;
    }
    return ast;
}


void ast_push(Ast* ast) {
    Ast* ast2 = malloc(sizeof(Ast));
    ast2->fin = ast->fin;
    ast2->type = ast->type;
    ast2->suiv = ast->suiv;
    // on a fait une copie du chaînon

    ast->suiv = ast2;
}


void ast_push_check(Ast** ast, int fin, int type) {
    //if (ast[0]->fin != fin || ast[0]->type != type)
    //{
        ast_push(ast[0]);
        ast[0]->type = type;
        ast[0]->fin = fin;
    //}
}



int ast_pop(Ast* ast) {
    if (ast -> suiv == NULL) {
        return -1;
    }

    Ast* sov = ast->suiv;
    ast->fin = sov->fin;
    ast->type = sov->type;
    ast->suiv = sov->suiv;
    free(sov);
    return 0;
}


void ast_destroy(Ast ** ast, int length) {
    for (int i = 0 ; i < length ; i++) {
        while (ast[i]->suiv != NULL)
            ast_pop(ast[i]);
        free(ast[i]);
    }
    free(ast);
}





bool strlist_ast_inList(strlist* list, Ast** ast, char* elt, int type, int offset) {
    int i = 0;
    while (i < list->len) {
        if (ast[i]->type == type && strcmp(list->tab[i], elt) == 0)
            return true;

        i = ast[i]->fin + 1 - offset;
    }
    return false;
}


bool ast_typeInAst(Ast** ast, int len, int type, int offset) {
    int i = 0;
    while (i < len) {
        if (ast[i]->type == type)
            return true;

        i = ast[i]->fin + 1 - offset;
    }
    return false;
}



int ast_typeCountAst(Ast** ast, int len, int type, int offset) {
    int i = 0;
    int c = 0;
    while (i < len) {
        if (ast[i]->type == type)
            c++;

        i = ast[i]->fin + 1 - offset;
    }
    return c;
}



int ast_minOp(Ast** ast, strlist* tokens, int offset) {
    int max = -1;
    int priomax = -1;
    int i = 0;
    while (i < tokens->len) {

        if (ast[i]->type == TYPE_OPERATOR) {
            int nprio = PRIORITE.tab[strlist_index(&OPERATEURS, tokens->tab[i])];

            if (nprio >= priomax) {
                priomax = nprio;
                max = i;
            }
        }

        i = ast[i]->fin + 1 - offset;
    }
    return max;
}


int ast_length(Ast** ast, int token_length, int offset) {
    int l = 0;
    int i = 0;
    while (i < token_length) {
        l++;
        i = ast[i]->fin + 1 - offset;
    }
    return l;
}

int ast_index(Ast** ast, int index, int offset) {
    return ast_length(ast, index, offset);
}


int ast_prec(Ast** ast, int index, int offset) {
    int i = 0;
    int i2 = 0;
    while (i < index) {
        i2 = i;
        i = ast[i]->fin - offset + 1;
    }
    return i2;
}





void ast_aff(Ast** ast, int len) // affiche une liste d'entiers
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
        "expr"
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
        printInt(ast[i]->type);
      printString(", ");
      printInt(ast[i]->fin);
      printString("), ");
    }

    printString("(");
    if (ast[len-1]->type < 21)
        printString(tab[ast[len-1]->type]);
    else
        printInt(ast[len-1]->type);
    printString(", ");
    printInt(ast[len-1]->fin);
    printString(")]");
    newLine();
  }
}




bool isFull(char* string)
{

    if (strlen(string) == 0)
        return true;

    strlist* tokens = strlist_create(0);
    intlist types = intlist_create(0);
    intlist lines = intlist_create(0);
    Ast** ast;

    cut(tokens, &types, string, true, &lines);

    if (global_env->CODE_ERROR == 0) {
        ast = ast_create(&types);
        
        parse(tokens, types, ast, &lines, 0);

        if (global_env->CODE_ERROR == 0)
            statements(&types, tokens, ast, &lines, 0);

        ast_destroy(ast, tokens->len);
    }

    strlist_destroy(tokens, true);
    free(types.tab);
    free(lines.tab);

    
    if (global_env->CODE_ERROR == 29 || global_env->CODE_ERROR == 26 || global_env->CODE_ERROR == 11 || global_env->CODE_ERROR == 77)
    {
        global_env->CODE_ERROR = 0;
        return false;
    }
    else if (global_env->CODE_ERROR != 0)
        return false;
    else
        return true;
}




void write_libtok(char* buf, char* string, int size_string, int size)
{
    for (int i=0 ; i < size_string ; i++)
        buf[i] = string[i];
    
    for (int i = size_string ; i < size ; i++)
        buf[i] = ' ';
}




void cut(strlist* tokens, intlist* types, char* str, bool traiterStatements, intlist* lines)
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

    #define sep5 "atomic"
    #define longueur_sep5 6


    bool str1 = false;
    bool str2 = false;
    bool isPotentiallyWord = false;

    // après, il y a les commentaires, mais on s'en fout
    for (int k = 0 ; k < len_string ; k++)
    {

        if ((isidentifier(string[k]) || string[k] == '_') && !str1 && !str2)
            isPotentiallyWord = true;
        
        if (isPotentiallyWord && string[k] != '_' && string[k] != '\'' && !isidentifier(string[k]) && string[k] != '~')
            isPotentiallyWord = false;


        if (string[k] == '\'' && !isPotentiallyWord && !str2) str1 = !str1;
        if (string[k] == '"' && !str1) str2 = !str2;




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

            if (k < len_string - longueur_sep5 - 1) // pour le 'atomic'
            {
                if (isString(string + k + 1, sep5, longueur_sep5)) // remplacement du 'atomic'
                    write_libtok(string + k + 1, "atmc {", 6, longueur_sep5);
            }
        }
    }


    //--------------------------- FIN DE L'ALGORITHME DE REMPLACEMENT ------------------------------------

    bool nouvTok=false; //on modifie la valeur si on a un nouveau token à ajouter
    
    // pour les sous-tokens
    bool isPotentiallyNumber=false;
    int stepNumber = 0; // définit à quelle étape de définition de nombre on est lors de la détection
    bool isPotentiallyHexBin=false;
    int stepHexBin = 0;
    bool isPotentiallyString=false;
    bool isPotentiallyString2=false; // chaine avec un guillemet court
          isPotentiallyWord=false;
    bool isPotentiallyOp=false;
    bool isPotentiallyComm=false;
    bool isPotentiallyLongComm=false;
    
    
    
    
    
    
    
    
    int debTok=0; // contient la place de debut de detection du token etudie
    int debTok2=0; // contient la place de debut de detection du token compose etudie
    int debTok3=0;
    
    int debExpr=0; // va contenir la place de debut de l’expression dans le dernier algorithme
    bool isPotentiallyExpr=false; // pour la derniere boucle dans l’empaquetage d’expressions
    
    
    
    
    // pour la dernière boucle aussi, empaquetage des blocs conditionnels if/elif/else et for, while (statement)
    int debStat=0;
    bool isPotentiallyStat=false;
    
    
    
    
    char char2=0; // va contenir le caractere precedent
    char char1=0; // va contenir le caractère actuel
    char* char1_2; // peut servir d'intermédiaire quand on veut transformer char1 en chaine de caractères
    
    int nombrePoints=0; // nombre de points comptes apres le debut d’un nombre

    global_env->LINENUMBER = 0;

    
    for (int i=0 ; i<len_string ; i++)
    {
        char1 = string[i];
        
        // compte le numéro de ligne du programme
        if (char2 == '\n') // si le caractère précédent était un retour à la ligne, le caractère actuel est le premier caractère de la nouvelle ligne
            global_env->LINENUMBER++;
        
        // comptage des variables relatives à la détection de nombres
        if (!isPotentiallyString && !isPotentiallyString2 && char1 == '.')
            nombrePoints+=1;
        if (isPotentiallyNumber && !isPotentiallyString && !isPotentiallyString2 && char1=='.' && nombrePoints==1 && i+1<len_string && isdigit(string [i+1]))
            stepNumber=1;
        else if (isPotentiallyNumber && char1=='.' && (i+1>=len_string || isdigit(string[i+1])))
        {
            global_env->CODE_ERROR = 2;// plusieurs virgules décimales
            free(string);
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
            if (isdigit(char1) || (char1 >= 'a' && char1 <= 'f') || (char1 >= 'A' && char1 <= 'F'))
                stepHexBin = 2;
            else
            {
                global_env->CODE_ERROR = 73;
                free(string);
                return ;
            }
        }

        
        //fins tokens simples
        finTokensSimples(string, &isPotentiallyNumber, &isPotentiallyString, &isPotentiallyWord, &isPotentiallyOp, &isPotentiallyLongComm, &isPotentiallyString2, &isPotentiallyComm, &isPotentiallyHexBin, &char2, &char1, &nouvTok, tokens, types, lines, &debTok, i, &stepNumber, &stepHexBin, len_string, &nombrePoints);

        if (global_env->CODE_ERROR != 0)
        {
            free(string);
            return;
        }


        
        //debut tokens simples
        debutTokensSimples(i, &debTok, &char1, &isPotentiallyString, &isPotentiallyNumber,&isPotentiallyWord, &isPotentiallyOp, &isPotentiallyString2, &isPotentiallyComm, &isPotentiallyLongComm, &isPotentiallyHexBin);
        
        
        // vérifie si le caractère est reconnu
        if (!isPotentiallyHexBin && !isPotentiallyWord && !isPotentiallyString && !isPotentiallyNumber && !isPotentiallyOp && !isPotentiallyString2 && !isPotentiallyComm && !isPotentiallyLongComm && !nouvTok && char1 != ' ' && char1 != '\t')
        {
            global_env->CODE_ERROR = 25; // caractère inconnu
            free(string);
            return;
        }
        
        char1_2 = charToString(char1);
        if (nouvTok) //si on a un nouveau token, on vérifie que le caractère actuel est accepté par la grammaire globale
        {
            if (!isalnum(char1) && !(strlist_inList(&acceptedChars, char1_2)))
            {
                global_env->CODE_ERROR = 25;
                free(string);
                free(char1_2);
                return;
            }
        }
        free(char1_2);
        
        nouvTok=false;
        
        
        char2=char1;
    }
    
    
    // si une variable de condition de détection de token est encore à True, faute de syntaxe
    if (isPotentiallyHexBin || isPotentiallyWord || isPotentiallyString || isPotentiallyNumber || isPotentiallyOp  || isPotentiallyString2 || isPotentiallyComm || isPotentiallyLongComm)
    {
        free(string);

        global_env->CODE_ERROR = 26; // chaine de caractère, liste ou autre non terminée
        
        return;
    }
    
    free(string);

    return ;
}






void finTokensSimples(char* string, bool* isPotentiallyNumber, bool* isPotentiallyString, bool* isPotentiallyWord, bool* isPotentiallyOp, bool* isPotentiallyLongComm, bool* isPotentiallyString2, bool* isPotentiallyComm, bool* isPotentiallyHexBin, char * char2, char* char1, bool* nouvTok, strlist* tokenAdd, intlist* typeTok, intlist* lines, int* debTok, int i, int* stepNumber, int* stepHexBin, int len_string, int* nombrePoints)
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
        intlist_append(lines, global_env->LINENUMBER);
        (*isPotentiallyString)=false;
    }
    
    if ((*char2)=='\'' && (*isPotentiallyString2) && i-1!=(*debTok)) // fin de chaîne si le guillemet etudie n’est pas le meme que le premier de la chaine
    {
        char* sousch = sub(string, (*debTok), i);
        strlist_append(tokenAdd,sousch);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_STRING);
        intlist_append(lines, global_env->LINENUMBER);
        (*isPotentiallyString2)=false;
    }
    
    if ((((*char1)=='.' && (*stepNumber)==2) || (!isdigit(*char1) && (*char1) != '.')) && (*isPotentiallyNumber) && !(*stepNumber==1) || (*isPotentiallyNumber && *char1=='.' && !isdigit(string[i+1]))) // fin de nombre
    {
        char* nb = sub(string, (*debTok), i);
        strlist_append(tokenAdd,nb);
        (*nouvTok) = true;

        if (*stepNumber == 2)
            intlist_append(typeTok, TYPE_DOUBLE);
        else
            intlist_append(typeTok, TYPE_INTEGER);

        intlist_append(lines, global_env->LINENUMBER);
        (*isPotentiallyNumber)=false;
        *stepNumber = 0;
        *nombrePoints = 0;
    }

    if (*isPotentiallyHexBin && *stepHexBin == 2 && !isdigit(*char1) && !(*char1 >= 'a' && *char1 <= 'f') && !(*char1 >= 'A' && *char1 <= 'F')) // fin hex-bin
    {
        // conversion en nombre décimal

        intptr_t trad = (string[*debTok+1] == 'x') ? hexToDec(string, *debTok + 2, i - *debTok - 2) : binToDec(string, 2 + *debTok, i - *debTok - 2);

        if (global_env->CODE_ERROR != 0)
            return ;
        
        char* nb = int_to_str(trad);

        // ajout du nombre converti
        strlist_append(tokenAdd,nb);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_INTEGER);
        intlist_append(lines, global_env->LINENUMBER);
        (*isPotentiallyHexBin)=false;
        *stepHexBin = 0;
        *nombrePoints = 0;
    }
    
    
    
    if ((*isPotentiallyWord) && !(isalnum(*char1) || *char1 == '_' || *char1 == '\'' || *char1 == '~')) // fin word
    {
        strlist_append(tokenAdd,sub(string, (*debTok), i));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_WORD);
        intlist_append(lines, global_env->LINENUMBER);
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
        intlist_append(lines, global_env->LINENUMBER);
        (*isPotentiallyOp)=false;
    }
    
    free(sousch1);free(sousch2);
    
    // elements d’un caractere
    
    if (!(*isPotentiallyLongComm) && !(*isPotentiallyComm) && !(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && ((*char1)=='(' || (*char1)==')'))
    {
        (*nouvTok)=true;
        strlist_append(tokenAdd,charToString(*char1));
        if ((*char1)=='(')
            intlist_append(typeTok,TYPE_PARENTHESE1);
        if ((*char1)==')')
            intlist_append(typeTok,TYPE_PARENTHESE2);

        intlist_append(lines, global_env->LINENUMBER);
        
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    
    if (!(*isPotentiallyLongComm) && !(*isPotentiallyComm) && !(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && ((*char1)=='{' || (*char1)=='}'))
    {
        strlist_append(tokenAdd,charToString(*char1));
        (*nouvTok)=true;
        intlist_append(typeTok, TYPE_ACCOLADE);
        intlist_append(lines, global_env->LINENUMBER);
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    
    if (!(*isPotentiallyLongComm) && !(*isPotentiallyComm) && !(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && ((*char1)=='[' || (*char1)==']'))
    {
        strlist_append(tokenAdd,charToString(*char1));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_CROCHET);
        intlist_append(lines, global_env->LINENUMBER);
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    if (!(*isPotentiallyLongComm) && !(*isPotentiallyComm) && !(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && (*char1)==',')
    {
        strlist_append(tokenAdd,charToString(*char1));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_VIRGULE);
        intlist_append(lines, global_env->LINENUMBER);
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
            intlist_append(lines, global_env->LINENUMBER);
            (*isPotentiallyNumber)=false;
        }
        
        if ((*isPotentiallyString) || (*isPotentiallyString2))
        {
    		global_env->CODE_ERROR = 33; // chaine de carcatère non fermée en fin de ligne
            return;
        }
        
        (*nouvTok)=true;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
        (*isPotentiallyComm)=false;
    }
    return ;
}





void debutTokensSimples(int i, int* debTok, char* char1, bool* isPotentiallyString, bool* isPotentiallyNumber, bool* isPotentiallyWord, bool* isPotentiallyOp, bool* isPotentiallyString2, bool* isPotentiallyComm, bool* isPotentiallyLongComm, bool* isPotentiallyHexBin)
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
    
    free(char1_2);
    
    return ;
}




bool isAcceptedWO(int type, int typeanc) {
    // cette fonction dit si oui ou non il peut y avoir type derrière typeanc, sans prendre en compte les opérateurs (Without Operators)
    for (int i = 0 ; i < syntax.tab[FLATTEN(type)].len ; i++) { // on remet le type dans l'intervalle [0, 127] en enlevant le IMMEDIATE
        if (syntax.tab[FLATTEN(type)].tab[i] == typeanc)
            return true;
    }
    return false;
}



// code d'erreur 30
bool isAccepted(char* token, uint8_t type, char* tokenAnc, int typeanc) {

    int gramm = 0, grammact = 0, grammanc = 0;

    if (FLATTEN(type) > 21 || FLATTEN(typeanc) > 21)
        return false;
    
    if (!isAcceptedWO(type, typeanc)) {
        return false;
    }

    if (type == TYPE_OPERATOR) {
        // on vérifie que typeanc peut être avant cet opérateur
        gramm = get_type_operande(token);


        if (typeanc==TYPE_OPERATOR)
            grammanc = get_type_operande(tokenAnc);


        if (((gramm & VARLEFT || gramm & VAR_RIGHT || gramm & VAR_VAR) && typeanc != TYPE_VARIABLE && typeanc != TYPE_LISTINDEX) || ((gramm & RIGHT || gramm & VARRIGHT)  && (typeanc==TYPE_INTEGER || typeanc==TYPE_DOUBLE || typeanc==TYPE_EXPRESSION || typeanc==TYPE_STRING || typeanc==TYPE_LIST ||  typeanc==TYPE_VARIABLE ||  typeanc==TYPE_FONCTION ||  typeanc==TYPE_BOOL || typeanc==TYPE_CONST || typeanc==TYPE_NONE || typeanc==TYPE_EXCEPTION || typeanc==TYPE_LISTINDEX)) || ((gramm & LEFT_VAR || gramm & RIGHT_LEFT || gramm & VAR_VAR) && !(typeanc==TYPE_INTEGER || typeanc==TYPE_DOUBLE || typeanc==TYPE_EXPRESSION || typeanc==TYPE_STRING || typeanc==TYPE_LIST ||  typeanc==TYPE_VARIABLE ||  typeanc==TYPE_FONCTION ||  typeanc==TYPE_BOOL ||  typeanc==TYPE_LISTINDEX || typeanc==TYPE_CONST || typeanc==TYPE_NONE || typeanc==TYPE_EXCEPTION || typeanc==TYPE_PARENTHESE2 || (typeanc==TYPE_OPERATOR && grammanc & VARLEFT))))
            return (gramm & SPECIAL || grammanc & SPECIAL); // si l'un des deux est spécial, alors on dit rien
        
    }

    if (typeanc == TYPE_OPERATOR) {
        // dans ce cas on vérifie que type peut suivre typeanc

        //recupere l’operateur pour verifier que ca correspond
        gramm = get_type_operande(tokenAnc);
        
        if (type==TYPE_OPERATOR)
            grammact = get_type_operande(token);

        
        // on verifie que le token peut effectivement etre apres l’operateur
        if ((gramm & VARLEFT && type != TYPE_ENDOFLINE && type != TYPE_VIRGULE && type != TYPE_PARENTHESE2 && !(type==TYPE_OPERATOR && grammact & RIGHT_LEFT)) || ((gramm & VARRIGHT || gramm & LEFT_VAR || gramm & VAR_VAR) && !(type==TYPE_LISTINDEX || type==TYPE_VARIABLE)) || ((gramm & RIGHT_LEFT || gramm & VAR_RIGHT || gramm & RIGHT || gramm & VAR_VAR) && (type==TYPE_ENDOFLINE || type==TYPE_VIRGULE || type==TYPE_PARENTHESE2)) || (gramm & VARRIGHT && type != TYPE_VARIABLE))
            return (gramm & SPECIAL || grammact & SPECIAL); // si l'un des deux est spécial, alors on dit rien
    }

    return true;
}









void parse(strlist* tokenAdd, intlist typeTok, Ast** ast, intlist* lines, int offset) {
    //printf("Appel à parse %d %d\n", offset, tokenAdd->len);

    // cette fonction a pour rôle de créer les éléments de syntaxe tels que des blocs, des blockline, des fonctions, des listes, etc

    int debTok = 0, debTok2 = 0, debTok3 = 0, lastDebTok = 0, gramm = 0;

    // le rôle de lastDebTok est de retenir la dernière fois qu'on a fini une fonction ou un listindex afin de ne parser récursivement que le nouveau contenu

    bool nouvTokComp = false;

    //pour les tokens composés
    bool isPotentiallyListIndex = false;
    bool isPotentiallyList = false;
    bool isPotentiallyFonc = false;
    bool isPotentiallyBlock = false;
    bool isPotentiallyInst = false;
    bool isPotentiallyPar = false;
    
    //nombre d’accollades ouvrantes detectees et d’accollades fermantes
    //on ajoute 1 en rencontrant une ouverte et on enleve un en rencontrant une fermee
    int nbAcc = 0;
    int nbPar = 0; // pareil pour les parentheses
    int nbCro = 0; // pour les crochets
    
    int foncParDeb = 0; // va contenir le nombre de parenthèses déjà traversées au moment où on arrive à la fonction
    int listeParDeb = 0; // va contenir le nombre de parenthèses déjà traversées au moment où on arrive à la liste
    int indexParDeb = 0; // va contenir le nombre de parenthèses déjà traversées au moment où on arrive à l'index de liste
    
    
    
    
    //etapes de detection par exemple variable, puis parenthese "(" puis parenthese ")"
    int listIndexStep = 0;
    int foncStep = 0;
    int instStep = 0;

    // ce tableau contient à chaque élément l'élément se syntaxe le plus extérieur qui commence ici, et son type


    for (int k=0 ; k < tokenAdd->len ; k++) // boucle pour chaque nouveau token ajoute. chaque tour de cette boucle est comme un tour de boucle normal sur tokens. cela permet d’eviter une boucle supplementaire                
    {
        // savoir si on a un operateur ou un mot cle ou un booleen
        if (typeTok.tab[k] == TYPE_OPERATOR) // si il a déjà été détecté comme opérateur, on vérifie si c'est bien un opérateur valide
        {
            if (!(strlist_inList(&OPERATEURS, tokenAdd->tab[k])))
            {
                global_env->CODE_ERROR = 24; // opérateur inconnu
                global_env->LINENUMBER = lines->tab[offset + k]; // pour indiquer la ligne du programme de l'erreur
                return;
            }
        }
        
        
        
        else if (typeTok.tab[k] == TYPE_WORD)
        {
            if (strlist_inList(&operateurs2, tokenAdd->tab[k]))
            {
                typeTok.tab[k] = TYPE_OPERATOR;
            }
            else if (strlist_inList(&neon_boolean, tokenAdd->tab[k]))
            {
                typeTok.tab[k] = TYPE_BOOL;
            }

            else if (strlist_inList(global_env->EXCEPTIONS, tokenAdd->tab[k]))
            {
                typeTok.tab[k] = TYPE_EXCEPTION;
            }
            
            else if (strlist_inList(&constant, tokenAdd->tab[k]))
            {
                /*if (strcmp(tokenAdd->tab[k], "None")==0)
                    typeTok.tab[k] = TYPE_NONE;
                else
                    */typeTok.tab[k] = TYPE_CONST;
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
            global_env->LINENUMBER = lines->tab[offset + k];
            global_env->CODE_ERROR = 68;
            return;
        }


        
        // annuler ou poursuivre les fonctions, les instructions, les index de listes....
        
        annulerOuPoursuivre(&isPotentiallyPar, &isPotentiallyFonc, &foncStep, &isPotentiallyInst, &instStep, &isPotentiallyListIndex, &listIndexStep, tokenAdd, &typeTok, k, &foncParDeb, &indexParDeb, &listeParDeb, &nbCro);


        
        // fins de fonctions, d'instructions, d'index de listes...
        
        finsTypesComposes(&debTok2, &debTok3, &lastDebTok, k, &nouvTokComp, &isPotentiallyPar, &isPotentiallyFonc, &foncStep, tokenAdd, typeTok, &nbPar, &isPotentiallyInst, &instStep, &listIndexStep, &isPotentiallyList, &isPotentiallyListIndex, &nbCro, &isPotentiallyBlock, &nbAcc, &foncParDeb, &indexParDeb, &listeParDeb, ast, offset, lines);

        // fin des fins et debut des debuts (hi hi hi)

        if (global_env->CODE_ERROR != 0)
        {
            return;
        }

        
        debutTokensComposes(k, &typeTok, tokenAdd, &debTok2, &debTok3, &lastDebTok, &nouvTokComp, &isPotentiallyList, &isPotentiallyPar, &isPotentiallyFonc, &isPotentiallyListIndex, &isPotentiallyBlock, &isPotentiallyInst, &listIndexStep, &foncStep, &instStep, &foncParDeb, &indexParDeb, &listeParDeb, &nbPar, &nbCro);
        
    }

    if (nbAcc != 0 || nbCro != 0 || nbPar != 0) //isPotentiallyBlock || isPotentiallyFonc || isPotentiallyList || isPotentiallyListIndex || isPotentiallyInst)
    {
        //strlist_aff(tokenAdd);
        //ast_aff(ast, tokenAdd->len);
        //printf("%d %d %d\n", nbAcc, nbCro, nbPar);
        global_env->CODE_ERROR = 26;
        global_env->LINENUMBER = lines->tab[offset + tokenAdd->len - 1];
        return;
    }
    

    // a la fin du parsing, on vérifie la syntaxe

    if (tokenAdd->len > 0) {

        bool plusieursTokens = ast[0]->fin - offset < tokenAdd->len-1; // l'indice de fin du 1er token est < au dernier element

        // la boucle d'après ne traite pas le premier opérateur, donc on s'occupe de lui ici
        if (ast[0]->type == TYPE_OPERATOR && strcmp(tokenAdd->tab[0], "-") == 0) {
            free(tokenAdd->tab[0]);
            tokenAdd->tab[0] = strdup("_");
        }
        

        if (ast[0]->type == TYPE_OPERATOR)
            gramm = get_type_operande(tokenAdd->tab[0]);

        if (!intlist_inList(&types_debut, ast[0]->type) || (ast[0]->type == TYPE_OPERATOR && !(gramm & VARRIGHT) && !(gramm & RIGHT) && !(gramm & SPECIAL)))
        {
            global_env->CODE_ERROR = 32;
            global_env->LINENUMBER = lines->tab[offset];
            return ;
        }


        // ces deux variables sont des indices indiquant quel est le dernier token fabriqué et l'avant-dernier token fabriqué
        int i_act = ast[0]->fin + 1 - offset;
        int i_anc = 0;


        while (plusieursTokens) { // si on a plus d'un token composé ou simple

            //printf("types : %d   %d\n", ast[i_anc]->type, ast[i_act]->type);


            // on va traiter l'opérateur moins (-) selon si c'est un unaire ou pas

            // ici on s'occuppe de modifier les signes '-'
            if (ast[i_act]->type==TYPE_OPERATOR && strcmp(tokenAdd->tab[i_act], "-") == 0) // si on a un opérateur et que cet opérateur est un moins
            {
                if (i_act == 0)
                {
                    free(tokenAdd->tab[i_act]);
                    tokenAdd->tab[i_act] = strdup("_"); // si j=0 (c'est-à-dire que c'est le premier token), on sait que c'est forcément un moins unaire
                }
                
                else if (!(ast[i_anc]->type == TYPE_DOUBLE || ast[i_anc]->type == TYPE_INTEGER || ast[i_anc]->type == TYPE_EXPRESSION || ast[i_anc]->type == TYPE_STRING || ast[i_anc]->type == TYPE_LISTINDEX || ast[i_anc]->type == TYPE_FONCTION || ast[i_anc]->type == TYPE_LIST || ast[i_anc]->type == TYPE_BOOL || ast[i_anc]->type == TYPE_CONST || ast[i_anc]->type == TYPE_NONE || ast[i_anc]->type == TYPE_EXCEPTION || ast[i_anc]->type == TYPE_PARENTHESE2 || ast[i_anc]->type == TYPE_VARIABLE || (ast[i_anc]->type==TYPE_OPERATOR && get_type_operande(tokenAdd->tab[i_anc]) & VARLEFT)) )
                {
                    // si on a un moins qui est directement précédé d'une autre unité lexicale de type nombre etc, ça veut dire que c'est l'opérateur moins donc
                    // là on teste la condition inverse c'est à dire que l'on se trouve en présence d'un moins unaire
                    // on va intégrer le moins à l'unité lexicale à qui il est adressé
                    if (ast[i_act]->fin - offset < tokenAdd->len - 1) // si le moins étudié n'est pas le dernier caractère des tokens
                    {
                        free(tokenAdd->tab[i_act]);
                        tokenAdd->tab[i_act] = strdup("_");
                    }
                }
            }



            if (!isAccepted(tokenAdd->tab[i_act], ast[i_act]->type, tokenAdd->tab[i_anc], ast[i_anc]->type))
            {
                global_env->CODE_ERROR = 30;
                global_env->LINENUMBER = lines->tab[offset + i_act];
                return;
            }

            if (ast[i_act]->fin + 1 - offset > tokenAdd->len - 1)
                break;

            i_anc = i_act;
            i_act = ast[i_act]->fin + 1 - offset;
        }
        
        if (i_act < tokenAdd->len) { // si ça n'a pas dépassé pendant la boucle
            if (ast[i_act]->type == TYPE_OPERATOR)
                gramm = get_type_operande(tokenAdd->tab[i_act]);

            if ((plusieursTokens && (!intlist_inList(&types_fin, ast[i_act]->type) || (ast[i_act]->type == TYPE_OPERATOR && !(gramm & VARLEFT) && !(gramm & SPECIAL)))) || (!plusieursTokens && !intlist_inList(&types_fin, ast[0]->type)))
            {
                global_env->CODE_ERROR = 32;
                global_env->LINENUMBER = lines->tab[offset + i_act];
                return ;
            }
        }
    }
}



void annulerOuPoursuivre(bool* isPotentiallyPar, bool* isPotentiallyFonc, int* foncStep, bool* isPotentiallyInst, int* instStep, bool* isPotentiallyListIndex, int* listIndexStep, strlist* tokenAdd, intlist* typeTok, int k, int *foncParDeb, int *listeParDeb, int *indexParDeb, int *nbCro)
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






void debutTokensComposes(int k, intlist* typeTok, strlist* tokenAdd, int* debTok2, int* debTok3, int* lastDebTok, bool* nouvTokComp, bool* isPotentiallyList, bool* isPotentiallyPar, bool* isPotentiallyFonc, bool* isPotentiallyListIndex, bool* isPotentiallyBlock, bool* isPotentiallyInst, int* listIndexStep, int *foncStep, int *instStep, int *foncParDeb, int *listeParDeb, int *indexParDeb, int* nbPar, int* nbCro)
{
    
    if (tokenAdd->tab[k][0] =='[' && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyBlock) && !(*isPotentiallyInst) && !(*isPotentiallyPar)) // debut de liste
    {
        (*debTok2) = k;
        (*isPotentiallyList)=true;
    }

    if (strcmp(tokenAdd->tab[k], "(") == 0 && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyBlock) && !(*isPotentiallyInst) && !(*isPotentiallyPar)) { // début de parenthèse
        *debTok2 = k;
        *isPotentiallyPar = true;
        *foncParDeb = *nbPar;
    }



    if (tokenAdd->tab[k][0] =='{' && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyBlock) && !(*isPotentiallyInst) && !(*isPotentiallyPar)) // debut de bloc d’instructions
    {
        (*debTok2)=k;
        (*isPotentiallyBlock)=true;
    }



    

    if (typeTok->tab[k] == TYPE_VARIABLE && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyBlock) && !(*isPotentiallyInst) && !(*isPotentiallyPar)) // debut de fonction et d’index de liste
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
    


    if (typeTok->tab[k] == TYPE_BLOCKWORD && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyInst) && !(*isPotentiallyPar) && !(*isPotentiallyBlock)) // debut d’instruction de bloc
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











void finsTypesComposes(int *debTok2, int *debTok3, int* lastDebTok, int k, bool *nouvTokComp, bool* isPotentiallyPar, bool *isPotentiallyFonc, int *foncStep, strlist* tokenAdd, intlist typeTok, int *nbPar, bool *isPotentiallyInst, int *instStep, int *listIndexStep, bool *isPotentiallyList, bool *isPotentiallyListIndex, int *nbCro, bool *isPotentiallyBlock, int *nbAcc, int *foncParDeb, int *listeParDeb, int *indexParDeb, Ast** ast, int offset, intlist* lines)
{

    //printf("%d %d %d %d %d %d %c\n", *isPotentiallyListIndex, *listIndexStep, k, *nbCro, *indexParDeb, tokenAdd->len, tokenAdd->tab[k][0]);

    if (*isPotentiallyFonc && *foncStep==2 && tokenAdd->tab[k][0] == ')' && (*nbPar - *foncParDeb)==0) // fin fonction
    {
        // on ajoute une autre unité syntaxique plus grande

        ast_push_check(&ast[*debTok2], k + offset, TYPE_FONCTION);


        // il ne faut parser récursivement que dans la partie que l'on a ajoutée
        
        // appel récursif sur parse
        strlist tokens2 = (strlist) {.tab = tokenAdd->tab + *lastDebTok + 2, .len = k - *lastDebTok - 2};
        intlist types2 = (intlist) {.tab = typeTok.tab + *lastDebTok + 2, .len = k - *lastDebTok - 2};
        parse(&tokens2, types2, ast + *lastDebTok + 2, lines, *lastDebTok + 2 + offset);

        if (global_env->CODE_ERROR != 0)
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
        global_env->CODE_ERROR = 28; // mot clé incompatible utilisé dans une expression
        global_env->LINENUMBER = lines->tab[offset + k];
        return;
    }
    
    else if (*isPotentiallyPar && strcmp(tokenAdd->tab[k], ")") == 0 && (*nbPar - *foncParDeb) == -1) { // fin de parenthèse
        // appel récursif à l'intérieur de la parenthèse
        ast[*debTok2]->type = TYPE_ENDOFLINE;
        ast[k]->type = TYPE_ENDOFLINE;

        ast_push_check(&ast[*debTok2], k + offset, TYPE_EXPRESSION);

        strlist tokens2 = {.tab = tokenAdd->tab + *debTok2 + 1, .len = k - *debTok2 - 1};
        intlist types2 = {.tab = typeTok.tab + *debTok2 + 1, .len = k - *debTok2 - 1};
        parse(&tokens2, types2, ast + *debTok2 + 1, lines, *debTok2 + 1 + offset);

        if (global_env->CODE_ERROR != 0)
            return;

        *isPotentiallyPar = false;
        *isPotentiallyListIndex = true;
        *isPotentiallyFonc = true;
        *foncStep = 1;
        *listIndexStep = 1;
        *lastDebTok = k;

        *nouvTokComp=true;
        *foncParDeb = 0;
    }
    
    
    else if ((*isPotentiallyInst) && (*instStep)==2 && tokenAdd->tab[k][0] == ')' && (*nbPar)==0) // fin instruction de bloc
    {
        (*isPotentiallyInst)=false;
        (*instStep)=0;
        
        if (strlist_inList(&blockwords1Line, tokenAdd->tab[*debTok2]))
            ast_push_check(&ast[*debTok2], k + offset, TYPE_BLOCKWORD1LINE);
        else
            ast_push_check(&ast[*debTok2], k + offset, TYPE_BLOCKLINE);

        // appel récursif
        strlist tokens2 = (strlist) {.tab = tokenAdd->tab + *debTok2 + 2, .len = k - *debTok2 - 2};
        intlist types2 = (intlist) {.tab = typeTok.tab + *debTok2 + 2, .len = k - *debTok2 - 2};
        parse(&tokens2, types2, ast + *debTok2 + 2, lines, offset + *debTok2 + 2);
            
        (*nouvTokComp)=true;
    }
    
    else if ((*isPotentiallyInst) && (*instStep)==2 && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
    {
        global_env->CODE_ERROR = 28; // mot clé incompatible utilisé dans une expression
        global_env->LINENUMBER = lines->tab[offset + k];
        return;
    }


    else if (*isPotentiallyListIndex && *listIndexStep==2 && tokenAdd->tab[k][0] == ']' && (*nbCro - *indexParDeb)==0) // fin index de liste
    {
        ast_push_check(&ast[*debTok2], k + offset, TYPE_LISTINDEX);

        // il ne faut parser récursivement que dans la partie que l'on a ajoutée
        
        // appel récursif sur parse

        if (k - *lastDebTok - 2 == 0) {
            global_env->CODE_ERROR = 30;
            global_env->LINENUMBER = lines->tab[offset + k];
            return;
        }


        strlist tokens2 = (strlist) {.tab = tokenAdd->tab + *lastDebTok + 2, .len = k - *lastDebTok - 2};
        intlist types2 = (intlist) {.tab = typeTok.tab + *lastDebTok + 2, .len = k - *lastDebTok - 2};
        parse(&tokens2, types2, ast + *lastDebTok + 2, lines, *lastDebTok + 2 + offset);

        if (global_env->CODE_ERROR != 0)
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
        global_env->CODE_ERROR = 28; // mot clé incompatible utilisé dans une expression
        global_env->LINENUMBER = lines->tab[offset + k];
        return;
    }
      
    
    
    
    
    else if (tokenAdd->tab[k][0] == ']' && (*isPotentiallyList) && *nbCro == 0)// fin de liste
    {
        (*isPotentiallyList)=false;

        ast_push_check(&ast[*debTok2], k + offset, TYPE_LIST);

        // appel récursif sur parse
        strlist tokens2 = (strlist) {.tab = tokenAdd->tab + *debTok2 + 1, .len = k - *debTok2 - 1};
        intlist types2 = (intlist) {.tab = typeTok.tab + *debTok2 + 1, .len = k - *debTok2 - 1};
        parse(&tokens2, types2, ast + *debTok2 + 1, lines, *debTok2 + 1 + offset);

        if (global_env->CODE_ERROR != 0)
            return;

        
        (*nouvTokComp)=true;
    }

    else if ((*isPotentiallyList) && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
    {
        global_env->CODE_ERROR = 28; // mot clé incompatible utilisé dans une expression
        global_env->LINENUMBER = lines->tab[offset + k];
        return;
    }

    
    
    else if (tokenAdd->tab[k][0] =='}' && (*isPotentiallyBlock) && (*nbAcc)==0)// fin de bloc d’instructions
    {
        (*isPotentiallyBlock)=false;

        ast_push_check(&ast[*debTok2], k + offset, TYPE_BLOCK);

        // appel récursif sur parse à l'intérieur du bloc
        strlist tokens2 = (strlist) {.tab = tokenAdd->tab + *debTok2 + 1, .len = k - *debTok2 - 1};
        intlist types2 = (intlist) {.tab = typeTok.tab + *debTok2 + 1, .len = k - *debTok2 - 1};
        parse(&tokens2, types2, ast + *debTok2 + 1, lines, *debTok2 + 1 + offset);


        (*nouvTokComp)=true;
    }
    
    
    return;
}



int get_type(char* token, int type) {

    if (strcmp(token, "if")==0)
        return TYPE_BLOCKIF;
    
    if (strcmp(token, "ei")==0)
        return TYPE_BLOCKELIF;
    
    if (strcmp(token, "es")==0)
        return TYPE_BLOCKELSE;
    
    if (strcmp(token, "while")==0)
        return TYPE_BLOCKWHILE;
    
    if (strcmp(token, "for")==0)
        return TYPE_BLOCKFOR;

    if (strcmp(token, "foreach")==0)
        return TYPE_BLOCKFOREACH;

    if (strcmp(token, "tr")==0)
        return TYPE_BLOCKTRY;
    
    if (strcmp(token, "expt")==0)
        return TYPE_BLOCKEXCEPT;

    if (strcmp(token, "atmc") == 0)
        return TYPE_ATOMICLINE;

    return type;
}







void statements(intlist* types, strlist* tokens, Ast** ast, intlist* lines, int offset)
{
    //printf("statements :\n"); //strlist_aff(tokens);ast_aff(ast, tokens->len);
    // statements est censé être récursif

    bool isPotentiallyConBlock = false, isPotentiallyStat = false, isPotentiallyTE = false;
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
            global_env->CODE_ERROR = 34;
            global_env->LINENUMBER = lines->tab[offset + i];
            return;
        }

        
        if (funcMethStep == 2)
        {
            if (ast[i]->type == TYPE_BLOCK)
            {
                funcMethStep = 0;
                
                bool isMethod = strcmp("method", tokens->tab[debFunc]) == 0;

                // avant, c'était push à l'indice i-2
                ast_push_check(&ast[debFunc], ast[i]->fin, (isMethod) ? TYPE_METHODDEF : TYPE_FUNCTIONDEF);

                // appel récursif
                intlist types2 = {.tab = types->tab + i + 1, .len = ast[i]->fin - 1 - offset - i};
                strlist tokens2 = {.tab = tokens->tab + i + 1, .len = ast[i]->fin - 1 - offset - i};

                statements(&types2, &tokens2, ast + i + 1, lines, offset + i + 1);

            }
            else if (typeact != TYPE_ENDOFLINE)
            {
                global_env->CODE_ERROR = 11;
                global_env->LINENUMBER = lines->tab[offset + i];
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
                global_env->CODE_ERROR = 11;
                global_env->LINENUMBER = lines->tab[offset + i];
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
                global_env->CODE_ERROR = 11;
                global_env->LINENUMBER = lines->tab[offset + i];
                return;
            }
        }


        
        int nextindex = ast[i]->fin - offset + 1;
        
        // modifie le blockline pour savoir quel bloc c'est particulièrement
        if (ast[i]->type==TYPE_BLOCKLINE || (ast[i]->type == TYPE_KEYWORD && !strlist_inList(&lkeywords, tokens->tab[i])))
            ast[i]->type = get_type(tokens->tab[i], ast[i]->type);
        
        // pour faire des tests sur nextindex
        if (nextindex < tokens->len && (ast[nextindex]->type==TYPE_BLOCKLINE || (ast[nextindex]->type == TYPE_KEYWORD && !strlist_inList(&lkeywords, tokens->tab[nextindex]))))
            ast[nextindex]->type = get_type(tokens->tab[nextindex], ast[nextindex]->type);

        
        typeact=ast[i]->type;        


        // on vérifie entre les moments où on finit des statements que les trucs sont bien ce qu'il faut


        
        /*else if (TEStep == 1 && typeact != TYPE_ENDOFLINE) {
            global_env->CODE_ERROR = 33;
            return;
        }*/
        
        // conditions de fin de conblock

        // ast[ast[i]->fin - offset + 1]->type c'est pour vérifier le type du token composé d'après

        //printf("i : %d, typeact : %d, conBlockStep : %d\n", i + offset, typeact, conBlockStep);
        //strlist_aff(tokens);
        //ast_aff(ast, tokens->len);
        
        if (isPotentiallyConBlock &&
            ((nextindex >= tokens->len && (conBlockStep == 1 || (conBlockStep == 2 && typeact == TYPE_BLOCK))) // ou alors on est sur un juste à la fin sur un endofline, ou alors on est sur un block tout à la fin
            ||
            (nextindex < tokens->len && ((conBlockStep == 1 && typeact != TYPE_BLOCKIF && typeact != TYPE_BLOCKELSE && typeact != TYPE_BLOCKELIF)|| (conBlockStep == 2 && typeact == TYPE_BLOCK)) && ast[nextindex]->type != TYPE_ENDOFLINE && ast[nextindex]->type != TYPE_BLOCKIF && ast[nextindex]->type != TYPE_BLOCKELSE && ast[nextindex]->type != TYPE_BLOCKELIF && ast[nextindex]->type != TYPE_BLOCK))
            ) {
            //printf("CONDITIONBLOCK\n");
            // fin conblock si le type d'après ne correspond pas ou qu'on est à la fin
            int fin;
            if (typeact == TYPE_BLOCK)
                fin = ast[i]->fin;
            else
                fin = ast[debStat]->fin;

            ast_push_check(&ast[debConBlock], fin, TYPE_CONDITIONBLOCK);
            isPotentiallyConBlock = false;
        }

        else if (isPotentiallyConBlock && conBlockStep == 1) {
            if (typeact == TYPE_BLOCKIF || typeact == TYPE_BLOCKELSE || typeact == TYPE_BLOCKELIF)
                conBlockStep = 2;
            else if (typeact != TYPE_ENDOFLINE) {
                //printf("Z0\n");
                global_env->CODE_ERROR = 33;
                global_env->LINENUMBER = lines->tab[offset + i];
                return;
            }
        }

        else if (isPotentiallyConBlock && conBlockStep == 2) {
            if (typeact == TYPE_BLOCK)
                conBlockStep = 1;
            else if (typeact != TYPE_ENDOFLINE) {
                //printf("Z1\n");
                global_env->CODE_ERROR = 33;
                global_env->LINENUMBER = lines->tab[offset + i];
                return;
            }
        }



        // --------- TRY/EXCEPT ------------
        // condition d'arrêt : le dernier bloc en date est un statementexcept et soit c'est la fin des tokens, soit le suivant est ni un endofline, ni un blockexcept
        //printf("%d %d %d %d %d\n", isPotentiallyTE, TEStep, nextindex, i, tokens->len);
        //strlist_aff(tokens);
        //ast_aff(ast, tokens->len);

        if (isPotentiallyTE &&
            ((nextindex >= tokens->len && (TEStep == 1 || (TEStep == 2 && typeact == TYPE_BLOCK)))
            ||
            (nextindex < tokens->len && ((TEStep == 1 && typeact != TYPE_BLOCKEXCEPT)|| (TEStep == 2 && typeact == TYPE_BLOCK)) && ast[nextindex]->type != TYPE_BLOCKEXCEPT && ast[nextindex]->type != TYPE_ENDOFLINE))
            ) {
            // on réunit les deux
            int fin;
            if (typeact == TYPE_BLOCK)
                fin = ast[i]->fin;
            else
                fin = ast[debStat]->fin;
            ast_push_check(&ast[debTE], fin, TYPE_TRYEXCEPT);
            isPotentiallyTE = false;
        }
        /*else if (ast[debStat]->type == TYPE_STATEMENTEXCEPT) {
            //printf("Z3\n");
            global_env->CODE_ERROR = 33;
            return;
        }*/

        if (isPotentiallyTE && typeact == TYPE_BLOCK && TEStep == 2)
        {
            TEStep = 1;
        }
        else if (TEStep == 2 && typeact != TYPE_ENDOFLINE) {
            //printf("Z4\n");
            global_env->CODE_ERROR = 33;
            global_env->LINENUMBER = lines->tab[offset + i];
            return;
        }

        if (isPotentiallyTE && typeact == TYPE_BLOCKEXCEPT && TEStep == 1)
        {
            TEStep = 2;
        }
        
        
        ////////////// FIN DE STATEMENT /////////////////
        
        
        if (isPotentiallyStat && typeact==TYPE_BLOCK) // fin de statement, on l'empaquette
        {
            
            // ici, il faut créer le STATEMENTTYPE

            int typeChoisi;
            
            switch (ast[debStat]->type)
            {
                case TYPE_BLOCKFOR :
                    typeChoisi = TYPE_STATEMENTFOR;
                    break;
                
                case TYPE_BLOCKFOREACH :
                    typeChoisi = TYPE_STATEMENTFOREACH;
                    break;
                
                case TYPE_BLOCKWHILE :
                    typeChoisi = TYPE_STATEMENTWHILE;
                    break;
                
                case TYPE_BLOCKIF :
                    typeChoisi = TYPE_STATEMENTIF;
                    break;
                
                case TYPE_BLOCKELIF :
                    typeChoisi = TYPE_STATEMENTELIF;
                    break;
                
                case TYPE_BLOCKELSE :
                    typeChoisi = TYPE_STATEMENTELSE;
                    break;

                case TYPE_BLOCKTRY :
                    typeChoisi = TYPE_STATEMENTTRY;
                    break;

                case TYPE_BLOCKEXCEPT :
                    typeChoisi = TYPE_STATEMENTEXCEPT;
                    break;
                
                case TYPE_ATOMICLINE:
                    typeChoisi = TYPE_ATOMICBLOCK;
                    break;
            }

            ast_push_check(&ast[debStat], ast[i]->fin, typeChoisi);

            // appel récursif de statements

            int longueur = ast[i]->fin - offset - i - 1;


            strlist tokens2 = (strlist) {.tab = tokens->tab + i + 1, .len = longueur};
            intlist types2 = (intlist) {.tab = types->tab + i + 1, .len = longueur};
            Ast** ast2 = ast + i + 1;
            
            statements(&types2, &tokens2, ast2, lines, offset + i + 1);

            ////// ICI LE TRAITEMENT POST-STATEMENT

            // --------- CONBLOCK ------------

            if (ast[debStat]->type == TYPE_STATEMENTIF && !isPotentiallyConBlock) // debut conblock
            {
                conBlockStep = 1;
                isPotentiallyConBlock = true;
                debConBlock = debStat;
            }





            if (ast[debStat]->type == TYPE_STATEMENTTRY) { // debut try except
                debTE = debStat;
                isPotentiallyTE = true;
                TEStep = 1;
            }
            
            
            isPotentiallyStat=false;
        }
        else if (isPotentiallyStat && typeact != TYPE_ENDOFLINE && typeact != TYPE_ACCOLADE) {
            global_env->CODE_ERROR = 27;
            global_env->LINENUMBER = lines->tab[offset + i];
            return;
        }


        

        

        /////////////// DEBUT DES STATEMENTS ///////////////
        
        if (typeact==TYPE_ATOMICLINE || typeact==TYPE_BLOCKTRY || typeact == TYPE_BLOCKEXCEPT || typeact==TYPE_BLOCKFOR || typeact==TYPE_BLOCKFOREACH || typeact==TYPE_BLOCKWHILE || typeact==TYPE_BLOCKIF || typeact==TYPE_BLOCKELSE || typeact==TYPE_BLOCKELIF) // debut statement
        {
            if (ast[i]->fin - offset == tokens->len - 1)
            {
                global_env->CODE_ERROR = 29; // Ligne de bloc d'instructions non suivi par un bloc d'instructions (ex: un if() tout seul)
                global_env->LINENUMBER = lines->tab[offset + i];
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
        global_env->CODE_ERROR = 29;
        global_env->LINENUMBER = lines->tab[offset + i];
    }
    

    // les deux blocs qui suivent sont utiles au cas où lors du dernier tour de boucle, le TYPE_STATEMENT* vient d'être fabriqué
    // du coup le isPotentially* a eu le temps de s'activer, mais il n'y a pas eu de tour supplémentaire pour valider

    if (isPotentiallyConBlock && conBlockStep == 1) {
        // peut etre que le dernier token etait un block et qu'on est sortis de la boucle direct avant de finir
        ast_push_check(&ast[debConBlock], tokens->len + offset - 1, TYPE_CONDITIONBLOCK);
    }
    else if (isPotentiallyTE && TEStep == 1) {
        ast_push_check(&ast[debTE], tokens->len + offset - 1, TYPE_TRYEXCEPT);
    }


    return ;
}