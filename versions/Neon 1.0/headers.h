
#include <stdlib.h>



#define START_MESSAGE "Bienvenue dans Neon.\n"
#define SEQUENCE_ENTREE "\033[1;34mneon@ready> \033[0m"






//pour les booléens :
#define true 1
#define false 0


// pour builtinfonc et opfonc
#define NBOPERATEURS 28
#define NBBUILTINFONC 15


//definition des types d’arguments pris par les operateurs
#define VARLEFT 1 // une variable a gauche
#define RIGHT 2 // une operande a droite
#define RIGHT_LEFT 3 // une operande a droite et a gauche
#define VAR_RIGHT 4 // une variable a gauche et une operande a droite
#define VARRIGHT 5 // une variable a droite



#define TYPE_FONCTION "f"
#define TYPE_PARENTHESE1 "p"
#define TYPE_OPERATOR "o"
#define TYPE_ENDOFLINE "e"
#define TYPE_VIRGULE "v"
#define TYPE_LISTINDEX "i"
#define TYPE_NUMBER "n"
#define TYPE_STRING "s"
#define TYPE_LIST "l"
#define TYPE_VARIABLE "V"
#define TYPE_BOOL "b"
#define TYPE_PARENTHESE2 "P"
#define TYPE_BLOCK "B"
#define TYPE_BLOCKLINE "L"
#define TYPE_KEYWORD "k"
#define TYPE_CONST "c"
#define TYPE_WORD "w"
#define TYPE_COMMENT "C"
#define TYPE_ACCOLADE "a"
#define TYPE_CROCHET "d"
#define TYPE_BLOCKWORD "D"
#define TYPE_EXPRESSION "E"
#define TYPE_CONDITIONBLOCK "K"
#define TYPE_NONE "N"
#define TYPE_TEMPLIST "t"

#define TYPE_STATEMENTFOR "0"
#define TYPE_STATEMENTWHILE "1"
#define TYPE_STATEMENTIF "2"
#define TYPE_STATEMENTELIF "3"
#define TYPE_STATEMENTELSE "4"

#define TYPE_BLOCKFOR "5"
#define TYPE_BLOCKWHILE "6"
#define TYPE_BLOCKIF "7"
#define TYPE_BLOCKELIF "8"
#define TYPE_BLOCKELSE "9"

#define TYPE_SYNTAXTREE "T"











#define ERROR_MALLOC "Erreur lors de l allocation des ressources (malloc)."
#define ERROR_MEMSET "Erreur lors de l initialisation du pointeur (memset)."
#define ERROR_REALLOC "Erreur lors de la reallocation de la memoire (realloc)."
#define ERROR_PTR_NOT_IN_TYPE_LIST "Erreur : Le pointeur n est pas dans la liste."
#define ERROR_OUT_OF_RANGE "Erreur : Hors intervalle."
#define ERROR_INT_NOT_IN_TYPE_LIST "Erreur : Le nombre n est pas dans la liste."
#define ERROR_STR_NOT_IN_TYPE_LIST "Erreur : La chaine de caracteres n est pas dans la liste ou la variable n est pas encore definie."
#define ERROR_INPUT "Erreur lors de l entree utilisateur (scanf)."
#define ERROR_STRCPY "Erreur lors de la copie de la chaine (strcpy)."
#define ERROR_SSCANF "Erreur lors de la conversion en nombre (sscanf)."
#define ERROR_SNPRINTF "Erreur lors de la conversion en chaine de caractere (snprintf)."






typedef struct ChainonPtr
{
    void* tete;
    struct ChainonPtr* queue;
} ptrlist;




typedef struct
{
  int* tab;
  unsigned len;
  unsigned capacity;
    
} intlist;




typedef struct
{
  char** tab;
  unsigned len;
  unsigned capacity;
  
} strlist;



typedef struct
{
  strlist* tab;
  unsigned len;
  unsigned capacity;
  
} listlist;









typedef struct NeObject
{
    void* data;
    char type;
} NeObject;





typedef struct NeList
{
    NeObject** tab; // tableau de pointeurs de NeObjects
    unsigned len;
    unsigned capacity;
} NeList;










struct Tree
{
    int type;
    NeObject* label;
    char* label1;
    long double label2;
    struct Tree** sons;
    unsigned nbSons;
    unsigned capacity;
};

typedef struct Tree Tree;








/*MAIN.C*/
char* eval(Tree* tree, int* typeRetour);
char* evalAux(Tree* tree, int* typeRetour, _Bool calcListIndex);
char* nameListIndex(char* listindex);
unsigned typeListItem(char* item);
unsigned indexListIndex(char* listindex);
void printRes(char* res, int type);
void exec(Tree* tree);
_Bool isTrue(Tree* tree);
void execConditionBlock(Tree* tree);
void execFile(char* filename);
void startMessage(void);
void terminal (void);




/*VARLIB*/
NeObject* neobject_create(char type);
void neobject_destroy(NeObject* neo);
void neobject_aff(NeObject* neo);
NeList* nelist_create(unsigned len);
void nelist_append(NeList* list, NeObject* ptr);//ajoute un élément à la fin de la liste
void nelist_remove(NeList* list,unsigned index);
void nelist_aff(NeList* liste);
void nelist_destroy(NeList* list);
NeObject* neo_nb_create(double number);
NeObject* neo_bool_create(_Bool boolean);
NeObject* neo_str_create(char* string);
NeObject* neo_list_create(unsigned len);
void neo_list_append(NeObject* neo, NeObject* ptr);
NeObject* neo_list_nth(NeObject* neo, unsigned index);






/*LIBTREE.C*/
Tree* tree_create(char* label1, long double label2, long double type);
void tree_appendSon(Tree* tree, Tree* son);
void tree_destroy(Tree* tree);
void tree_removeSon(Tree* tree, unsigned index);
char* blockFromStatement(char* statement);
char* expressionFromStatement(char* statement);
char* fonctionName(char* fonction, char* set);
char* fonctionArgs(char* fonction, char* set);
void createExpressionTree(Tree* tree, char* string);
void createConditionBlockTree(Tree* tree, char* conditionblock);
void createStatementElseTree(Tree* tree, char* statement);
void createStatementIEWFTree(Tree* tree, char* statement, int type);
void createBlockTree(Tree* tree, char* block);
void cutConditionBlock(strlist* tokens, intlist* types, char* conditionblock);
void createSyntaxTree(Tree* tree, char* program);




/*LIBTOK.C*/

// ancien libtok :
_Bool isMatrixIndex(char* str);
_Bool isBool(char* str);
_Bool isVariable(char* str);
_Bool isNumber(char* str);
_Bool isOperator(char* str);
_Bool isBlank(char* str);
_Bool isKeyword(char* str);
_Bool isFonction(char* str);
_Bool isListIndex(char* str);
_Bool isStr(char* str);
_Bool isList(char* str);




char* nomBlockLine(char* blockline);//a liberer apres
void finTokensSimples(char* string, _Bool* isPotentiallyNumber, _Bool* isPotentiallyString, _Bool* isPotentiallyWord, _Bool* isPotentiallyOp, _Bool* isPotentiallyLongComm, _Bool* isPotentiallyString2, _Bool* isPotentiallyComm, char * char2, char* char1, _Bool* nouvTok, strlist* tokenAdd, intlist* typeTok, unsigned* debTok, unsigned i, unsigned* stepNumber);
void debutTokensSimples(unsigned i, unsigned* debTok, char* char1, _Bool* isPotentiallyString, _Bool* isPotentiallyNumber, _Bool* isPotentiallyWord, _Bool* isPotentiallyOp, _Bool* isPotentiallyString2, _Bool* isPotentiallyComm, _Bool* isPotentiallyLongComm);
void annulerOuPoursuivre(_Bool* isPotentiallyFonc, unsigned* foncStep, _Bool* isPotentiallyInst, unsigned* instStep, _Bool* isPotentiallyListIndex, unsigned* listIndexStep, strlist* tokenAdd, unsigned k, unsigned *foncParDeb, unsigned *listeParDeb, unsigned *indexParDeb);
void debutTokensComposes(unsigned k, intlist* typeTok, strlist* tokenAdd, unsigned* debTok2, unsigned* debTok3, strlist* tokens, intlist *types, _Bool* nouvTokComp, _Bool* isPotentiallyList, _Bool* isPotentiallyFonc, _Bool* isPotentiallyListIndex, _Bool* isPotentiallyComm, _Bool* isPotentiallyBlock, _Bool* isPotentiallyInst, _Bool* isPotentiallyLongComm, unsigned* listIndexStep, unsigned *foncStep, unsigned *instStep, unsigned *foncParDeb, unsigned *listeParDeb, unsigned *indexParDeb, int* nbPar, int* nbCro);
void finsTypesComposes(unsigned *debTok2, unsigned *debTok3, unsigned k, _Bool *nouvTokComp, _Bool *isPotentiallyFonc, unsigned *foncStep, strlist* tokenAdd, unsigned *nbPar, strlist* tokens, intlist* types, _Bool *isPotentiallyInst, unsigned *instStep, unsigned *listIndexStep, _Bool *isPotentiallyList, _Bool *isPotentiallyListIndex, unsigned *nbCro, _Bool *isPotentiallyBlock, unsigned *nbAcc, unsigned *foncParDeb, unsigned *listeParDeb, unsigned *indexParDeb);
void verificationGrammaire(strlist* tokens, intlist* types, _Bool* isPotentiallyExpr, unsigned* debExpr);
void compConditionBlock(unsigned cas, _Bool* isPotentiallyConBlock, unsigned *conBlockStep, strlist* tokens, intlist* types, unsigned *i, char* typeact, unsigned* debConBlock);
void statements(intlist* types, strlist* tokens, _Bool *isPotentiallyStat, unsigned *debStat);
void cut(strlist* tokens, intlist* types, char* str, _Bool traiterStatements);



/*ERRFONC.C*/
void quitError(char* message);
void* err_malloc(size_t taille);
void err_memset(void* ptr, int valeur, size_t taille);
void* err_realloc(void* ptr, size_t taille);
void err_free(void* ptr);
void freeAllPtr(void);
void b(double);






/* LIBLIST.C*/



//prototypes des fonctions

/**************************listlist**********************/
listlist listlist_create(unsigned len);
void listlist_append(listlist* list, strlist* ptr);
void listlist_remove(listlist* list, unsigned index);
void listlist_aff(listlist* list);
void listlist_destroy(listlist* list);



/**************************ptrlist**********************/
ptrlist ptrlist_create(void);
void ptrlist_append(ptrlist* list, void* ptr);
void ptrlist_remove(ptrlist* list, void* ptr);
unsigned ptrlist_index(ptrlist* list, void* ptr);
void ptrlist_aff(ptrlist* l);
unsigned ptrlist_len(ptrlist* l);
unsigned ptrlist_destroy(ptrlist* l, _Bool freeElements, _Bool freeTab);
void ptrlist_replace(ptrlist* liste, void* aRemplacer, void* nvlleValeur);



/**************************intlist**********************/
intlist intlist_create(unsigned len);
void intlist_aff(intlist* list);
void intlist_append(intlist* list,int nombre);
void intlist_resize(intlist* list, unsigned newLen);
void intlist_remove(intlist* list,unsigned index);
unsigned intlist_count(intlist* list, int nb);
_Bool intlist_inList(intlist* list, int nombre);
int intlist_index(intlist* list, int nombre);
void intlist_insert(intlist* list,int nombre, unsigned index);
int intlist_max(intlist* list);



/**************************strlist**********************/
void strlist_destroy(strlist* list);
strlist strlist_create(unsigned len);
void strlist_aff(strlist* list);
void strlist_append(strlist* list, char *chaine);
void strlist_resize(strlist* list, unsigned newLen, _Bool freeElement);
void strlist_remove(strlist* list,unsigned index, _Bool freeElement);
unsigned strlist_count(strlist* list, char* chaine);
_Bool strlist_inList(strlist* list, char* chaine);
int strlist_index(strlist* list, char* chaine);
void strlist_insert(strlist* list,char* chaine, unsigned index);



/*LIBSTR.C*/
char *copy(char * string);
char* sub(char* string,unsigned debut,unsigned fin);//a liberer après
char* replace(char*, char*, char*);//a liberer après
void cleanStdin(void);
char* addStr(char* str1, char* str2);//a liberer après
double strToNb(char *string);
char* nbToStr(double number);//a liberer après
unsigned count(char* string, char* search);
void pause(void);
char* input(char *text);//a libérer après
char* subReplace(char* string, unsigned debut, unsigned longueur, char* remplacement);//à liberer après
int sindex (char* string, char car);
char* traitementString(char* string);//a liberer apres
char* traitementStringInverse(char* string);//a liberer apres
char* addStr2(char* str1, char* str2);
char* charToString(char car);
char* sandwich(char* string, char car);
_Bool strToBool(char* string);






/*OPFONC.C*/
char* _and(char*,unsigned,char*,unsigned,int*);
char* _or(char*,unsigned,char*,unsigned,int*);
char* _xor(char*,unsigned,char*,unsigned,int*);
char* _add(char*,unsigned,char*,unsigned,int*);
char* _mul(char*,unsigned,char*,unsigned,int*);
char* _sub(char*,unsigned,char*,unsigned,int*);
char* _div(char*,unsigned,char*,unsigned,int*);
char* _pow(char*,unsigned,char*,unsigned,int*);
char* _equal(char*,unsigned,char*,unsigned,int*);
char* _notEq(char*,unsigned,char*,unsigned,int*);
char* _infEqual(char*,unsigned,char*,unsigned,int*);
char* _supEqual(char*,unsigned,char*,unsigned,int*);
char* _inf(char*,unsigned,char*,unsigned,int*);
char* _sup(char*,unsigned,char*,unsigned,int*);
char* _affect(char*,unsigned,char*,unsigned,int*);
char* _addEqual(char*,unsigned,char*,unsigned,int*);
char* _subEqual(char*,unsigned,char*,unsigned,int*);
char* _mulEqual(char*,unsigned,char*,unsigned,int*);
char* _divEqual(char*,unsigned,char*,unsigned,int*);
char* _incr(char*,unsigned,char*,unsigned,int*);
char* _decr(char*,unsigned,char*,unsigned,int*);
char* _not(char*,unsigned,char*,unsigned,int*);
char* _mod(char*,unsigned,char*,unsigned,int*);
char* _eucl(char*,unsigned,char*,unsigned,int*);
char* _ref(char*,unsigned,char*,unsigned,int*);
char* _goIn(char*,unsigned,char*,unsigned,int*);
char* _deref(char*,unsigned,char*,unsigned,int*);
char* _minus(char* op1,unsigned nat1,char* op2,unsigned nat2,int* typeRetour);

/*BUILTINSFONC.C*/
char* _print_(strlist* args, intlist* types, int* typeRetour);
char* _input_(strlist* args, intlist* types, int* typeRetour);
char* _nbr_(strlist* args, intlist* types, int* typeRetour);
char* _str_(strlist* args, intlist* types, int* typeRetour);
char* _len_(strlist* args, intlist* types, int* typeRetour);
char* _substring_(strlist* args, intlist* types, int* typeRetour);
char* _exit_(strlist* args, intlist* types, int* typeRetour);
char* _append_(strlist* args, intlist* types, int* typeRetour);
char* _remove_(strlist* args, intlist* types, int* typeRetour);
char* _insert_(strlist* args, intlist* types, int* typeRetour);
char* _type_(strlist* args, intlist* types, int* typeRetour);
char* _reverse_(strlist* args, intlist* types, int* typeRetour);
char* _eval_(strlist* args, intlist* types, int* typeRetour);
char* _help_(strlist* args, intlist* types, int* typeRetour);
char* _clear_(strlist* args, intlist* types, int* typeRetour);




