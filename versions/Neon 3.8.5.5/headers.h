#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define VERSION "3.8.5.5-beta"
/*
Numérotation de versions :
Ajout d'opérateur ou de fonction ou modification d'un élément de syntaxe : +0.0.1
Ajout d'une nouvelle syntaxe ou modification de la structure d'exécution : +0.1
Modification mineure : +0.0.0.1
Modification du look de l'interpréteur en mode console : +0.1
Ajout d'un type de données NeObject : +0.0.1
*/

#define INVITATION "https://discord.gg/wkBdK35w2a"

#include "platform.h"

#ifdef LINUX
    #define PLATFORM "linux"
#endif
#ifdef WINDOWS11
    #define PLATFORM "windows11"
#endif
#ifdef WINDOWS10
    #define PLATFORM "windows10"
#endif
#ifdef TI83PCE
    #define PLATFORM "ti83pce"
#endif
#ifdef WASM
    #define PLATFORM "wasm"
#endif


#ifdef TI83PCE
    #include <ti/real.h>
    #include <sys/rtc.h>
#endif


#if defined(LINUX) || defined(WINDOWS11) || defined(TI83PCE)
    // a définir uniquement si la console standard du système d'exploitation visé supporte les couleurs
    #define COLOR
#endif



// si la version actuelle n'est pas stable mais est en distribution
//#define EXPERIMENTAL

#define SEQUENCE_ENTREE    ">> "
#define SEQUENCE_SUITE     ".. "

#define NB_ERRORS 113

#define STACK_SIZE (8*1024*1024) // taille de la pile par défaut à 8 Mio, ça peut changer en fonction de la plateforme

/*constantes de couleur*/
#define BLUE 0
#define RED 1
#define WHITE 2
#define GREEN 3

#define PI 3.141592653589793

#define UNIT ((void*)-1) // valeur de pointeur spéciale comme NULL

// pour builtinfonc et opfonc
#define NBOPERATEURS 40
#define NBBUILTINFONC 54


// definition des types d’arguments pris par les operateurs
// pour construire un type d'opérateur, on fait GRAMMAR ou bien GRAMMAR | LAZY s'il est paresseux

#define VARLEFT     1   // une variable a gauche
#define RIGHT       2   // une operande a droite
#define RIGHT_LEFT  4   // une operande a droite et a gauche 
#define VAR_RIGHT   8   // une variable a gauche et une operande a droite
#define LEFT_VAR    16  // une variable à doite et n'importe quoi à gauche
#define VARRIGHT    32  // une variable a droite
#define VAR_VAR     64  // une variable à droite et une variable à gauche
#define SPECIAL     128 // les opérateurs que l'on peut retrouver partout
#define LAZY        256 // pour les opérateurs attendant un Tree* au lieu d'un NeObj

// valeurs de retour
#define CONTINUE 1
#define BREAK 2
#define PASS 3
#define EXIT_RETURN 4

// valeurs des blockword1line
#define RETURN 1
#define IMPORT 2
#define LOCAL 3
#define AWAIT 4

// ces types servent à la fois de type d'objet dans l'arbre de syntaxe,
// de type d'élément de syntaxe utilisé par cut et de type de NeObject
#define FLATTEN(type)           (type & 127) // on enlève le HEAP_ALLOCATED pour remettre le type sur l'intervalle [0, 127]
#define HEAP_ALLOCATED          128 // pour les objets n'ayant pas de dépendance sur le tas


#define TYPE_ANYTYPE            127
#define TYPE_FONCTION           (0 | HEAP_ALLOCATED)
#define TYPE_LISTINDEX          1
#define TYPE_OPERATOR           2
#define TYPE_STRING             (3 | HEAP_ALLOCATED)
#define TYPE_INTEGER            4
#define TYPE_DOUBLE             5
#define TYPE_LIST               (6 | HEAP_ALLOCATED)
#define TYPE_BLOCK              7
#define TYPE_BOOL               8
#define TYPE_CONST              (9 | HEAP_ALLOCATED)
#define TYPE_VARIABLE           10
#define TYPE_BLOCKLINE          11
#define TYPE_ENDOFLINE          12
#define TYPE_PARENTHESE1        13
#define TYPE_PARENTHESE2        14
#define TYPE_VIRGULE            15
#define TYPE_KEYWORD            16
#define TYPE_NONE               17
#define TYPE_BLOCKWORD1LINE     18
#define TYPE_KEYWORDFUNCTION    19
#define TYPE_EXCEPTION          20
#define TYPE_EXPRESSION         21

#define TYPE_COMMENT            22
#define TYPE_WORD               23
#define TYPE_CROCHET            24
#define TYPE_BLOCKWORD          25
#define TYPE_ACCOLADE           26
#define TYPE_CONDITIONBLOCK     27

#define TYPE_STATEMENTFOR       28
#define TYPE_STATEMENTWHILE     29
#define TYPE_STATEMENTIF        30
#define TYPE_STATEMENTELIF      31
#define TYPE_STATEMENTELSE      32
#define TYPE_STATEMENTTRY       33
#define TYPE_STATEMENTEXCEPT    34

#define TYPE_FUNCTIONDEF        35
#define TYPE_METHODDEF          36

#define TYPE_BLOCKFOR           37
#define TYPE_BLOCKWHILE         38
#define TYPE_BLOCKIF            39
#define TYPE_BLOCKELIF          40
#define TYPE_BLOCKELSE          41
#define TYPE_BLOCKTRY           42
#define TYPE_BLOCKEXCEPT        43
#define TYPE_TRYEXCEPT          44
#define TYPE_ATOMICLINE         45
#define TYPE_ATOMICBLOCK        46

#define TYPE_SYNTAXTREE         47

#define TYPE_BUILTINFUNC        48
#define TYPE_USERMETHOD         (49 | HEAP_ALLOCATED)
#define TYPE_USERFUNC           (50 | HEAP_ALLOCATED)
#define TYPE_CONTAINER          (51 | HEAP_ALLOCATED)
#define TYPE_ATTRIBUTE          52
#define TYPE_PROMISE            (53 | HEAP_ALLOCATED)
#define TYPE_AWAIT              54

#define TYPE_EMPTY              55

#define TYPE_BLOCKFOREACH       56
#define TYPE_STATEMENTFOREACH   57





typedef struct ChainonPtr
{
    void* tete;
    struct ChainonPtr* queue;
} ptrlist;




typedef struct
{
  int* tab;
  int len;
  int capacity;
    
} intlist;


typedef struct
{
    int**tab;
    int len;
    int capacity;
} intptrlist;

typedef struct
{
  char** tab;
  int len;
  int capacity;
  
} strlist;



typedef struct
{
  intlist* tab;
  int len;
  int capacity;
  
} listlist;




typedef struct Ast
{
    uint8_t type;
    int fin;
    struct Ast* suiv;
} Ast;



// définition de la structure de NeObj
typedef struct Function Function;
typedef struct UserFunc UserFunc;
typedef struct Container Container;
typedef struct NeList NeList;
typedef struct String String;


typedef struct NeObj
{
    union {
        int* refc_ptr; // ce champ permet d'accéder de manière directe au premier champ de chacune des structures

        Container* container;
        NeList* nelist;
        Function* function;
        UserFunc* userfunc;
        String* string;
        double floating;
        intptr_t integer;
    };
    uint8_t type;
} NeObj;



typedef int Var;



struct String
{
    int refc;
    char* string;
};



struct NeList
{
    int refc;
    NeObj* tab; // tableau de pointeurs de NeObjects
    int len;
    int capacity;
    struct NeList* myCopy;
    NeObj next;
    NeObj prev;
};

struct Container
{
    int refc;
    int type;
    NeList* data;
    struct Container* myCopy;
    NeObj next;
    NeObj prev;
};



struct Function
{
    int refc;
    NeObj (*ptr) (NeList*);
    const char* help;
    int nbArgs;
    int* typeArgs;
    int typeRetour;
};

typedef struct Tree Tree;

struct UserFunc
{
    int refc;
    Var* args;
    int nbArgs;
    bool unlimited_arguments;
    int nbOptArgs; // nombre d'arguments vraiment, vraiment optionnels (par définition, ceux qui sont après ...)
    Tree * code;
    Tree* opt_args;// expressions pour calculer les arguments optionnels
    char* doc;
};



struct Tree
{
    uint8_t type;
    int line;
    char* label1;
    int label2;
    struct Tree** sons;
    int nbSons;
    int capacity;
    NeObj data; // permet de stocker les données contenues dans les feuilles
    Var variable; // adresse de la variable relative au tableau ADRESSES : indice de la variable dans ADRESSES
};







typedef struct NeSave
{
    NeObj object;
    Var var;
} NeSave;





typedef struct Process
{
    uint8_t registers[64]; // sert à faire les échanges de registres
    Tree* arg_tree;
    Tree* original_call; // pointeur vers l'arbre original (le premier argument) de ce processus. Sert à libérer les arbres temporaires que l'on crée pour lancer des promesses
    void* stack; // ceci est un pointeur sur l'adresse de début de la pile (le haut de la pile), qui servira à la libérer
    ptrlist* var_loc; // les variables locales créés depuis le lancement du processus
    int id;
    ptrlist* varsToSave;
} Process;

typedef struct ProcessCycle
{
    Process* process;
    struct ProcessCycle* next;
    struct ProcessCycle* prev;
} ProcessCycle;




/*MAIN.C*/
void printRes(NeObj res);
void execFile(char* filename);
void importFile(char* filename);
void startMessage(void);
void terminal (void);
void neonInit(void);
void neonExit(void);




/*NEONIO.C*/
bool is_integer(char* string);
intptr_t str_to_int(char* string);
char* input(char*);
void printInt(int n);
void printDouble(double n);
void flush(void);
void printString(char* s);
void newLine(void);
void setColor(uint8_t color);
void printBack(void);
void clearConsole(void);
char* openFile(char* filename);
void writeFile(char* filename, char* content);
char* argsAns(void);



/*NUMBERS.C*/
double str_to_double(char* string);
char* double_to_str(double number);
char* int_to_str(intptr_t number);



/*VARLIB*/
#define NEO_TYPE(neo)                       neo.type
#define NEO_VOID                            ((NeObj) {.type = TYPE_EMPTY})
#define NEO_SPECIAL(code)                   ((NeObj) {.type = TYPE_EMPTY, .integer = code})
#define IS_NEO_SPECIAL_CODE(neo, code)      (neo.type == TYPE_EMPTY && neo.integer == code)

bool neo_is_void(NeObj neo);
bool neo_exact_equal(NeObj a, NeObj b);

void free_var(Var var);
void replace_var(Var var, NeObj object);
void set_var(Var var, NeObj object);
NeObj* get_absolute_address(Var var);
NeObj get_var_value(Var var);
Var get_var(char* name);
char* get_name(Var var);
UserFunc* neo_to_userfunc(NeObj neo);
Function* neo_to_function(NeObj neo);
NeObj neo_empty_create(void);
void var_reset(NeObj* neo);
Container* container_create(int type, NeList* data);
NeObj neo_container_create(int type, NeList* data);
Container* neo_to_container(NeObj);
NeObj gc_extern_neo_container_convert(Container* c);
NeObj neo_container_convert(Container* c);
int get_field_index(Container* c, char* name);
NeObj* get_container_field_addr(Container* c, int index);
NeObj get_container_field(Container* c, int index);
void container_destroy(Container* c);
NeObj neo_promise_create(int id);
int get_promise_id(NeObj promise);
void neobject_destroy(NeObj neo);
void neobject_aff(NeObj neo);
NeList* nelist_create(int len);
void nelist_append(NeList* list, NeObj ptr);//ajoute un élément à la fin de la liste
NeObj* nelist_nth_addr(NeList* list, int index);
NeObj nelist_nth(NeList* list, int index);
void nelist_remove(NeList* list,int index);
void nelist_aff(NeList* liste);
void nelist_destroy(NeList* list);
void nelist_destroy_until(NeList *list, int index_max);
NeObj neo_integer_create(intptr_t number);
NeObj neo_double_create(double number);
intptr_t randint(intptr_t min, intptr_t max);
intptr_t neo_to_integer(NeObj neo);
double neo_to_double(NeObj neo);
NeObj neo_bool_create(bool neon_boolean);
bool neo_is_true(NeObj neo);
NeObj neo_str_create(char* string);
NeObj neo_list_create(int len);
void neo_list_append(NeObj neo, NeObj ptr);
NeObj neo_list_nth(NeObj neo, int index);
NeObj neo_const_create(char* string);
bool neo_to_bool(NeObj neo);
char* neo_to_string(NeObj neo);
char* neo_to_const(NeObj neo);
NeObj neo_none_create(void);
NeObj neo_copy(NeObj neo);
NeObj neo_dup(NeObj neo);
NeObj neo_deepcopy(NeObj neo);
NeList* nelist_dup(NeList*);
int neo_list_len(NeObj neo);
int nelist_index(NeList* liste, NeObj neo);
int nelist_index2(NeList* l, NeObj neo);
void nelist_insert(NeList* list,NeObj neo, int index);
void neo_list_insert(NeObj neo, NeObj ptr, int index);
void neo_list_remove(NeObj neo, int index);
NeList* neo_to_list(NeObj neo);
char* neobject_str(NeObj);
char* nelist_str(NeList*);
NeObj neo_list_convert(NeList* list);
NeObj gc_extern_neo_list_convert(NeList* list);
Function* function_create(NeObj (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour);
NeObj neo_fun_create(NeObj (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour);
void compFunc(const char** helpbuiltinsfonc, const int* nbargs, const int** typesArgs, const int* typesRetour);
bool funcArgsCheck(Function* fun, NeList* args);
NeObj functionCall(NeObj fun, NeList* args);
char* type(NeObj neo);
NeObj userFuncCreate(Var* args, Tree* code, int nbArgs, bool unlimited_arguments, int nbOptArgs, Tree* opt_args);
NeObj neo_exception_create(int index);
int get_exception_code(NeObj exception);
bool neo_equal(NeObj _op1, NeObj _op2);
bool nelist_inList(NeList* list, NeObj neo);
bool nelist_inList2(NeList* list, NeObj neo);
bool nelist_equal(NeList* l1, NeList* l2);
int neo_compare(NeObj a, NeObj b); // ordre croissant
int neo_compare2(NeObj a, NeObj b); // ordre décroissant
void neo_list_free(NeObj list);
void mark(NeObj neo);
void unmark(NeObj neo);
void recursive_unmark(NeObj neo);
bool ismarked(NeObj neo);
void update_if_promise(NeObj* promise);
char* var_name(NeObj obj);
NeObj callOverloadedBinaryOperator(NeObj op1, NeObj op2, char* opname);
NeObj callOverloadedUnaryOperator(NeObj op1, char* opname);


/*LIBTREE.C*/
Tree* tree_create(char* label1, int label2, uint8_t type);
void tree_appendSon(Tree* tree, Tree* son);
bool tree_isEmpty(Tree* tree);
void tree_destroy(Tree* tree);
void tree_removeSon(Tree* tree, int index);
Tree* tree_copy(Tree* tree);
char* blockFromStatement(char* statement);
char* expressionFromStatement(char* statement);
char* fonctionName(char* fonction, char* set);
char* fonctionArgs(char* fonction, char* set);
void createExpressionTree(Tree* tree, char* string);
void createConditionBlockTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset);
void createStatementElseTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset);
void createStatementIEWFTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset, int type);
void createSyntaxTree(Tree* tree, char* program);
void createFunctionTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset, bool isMethod);
void createSyntaxTreeAux(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset);




/*LIBTOK.C*/



bool isFull(char* code);
char* nomBlockLine(char* blockline);//a liberer apres
void finTokensSimples(char* string, bool* isPotentiallyNumber, bool* isPotentiallyString, bool* isPotentiallyWord, bool* isPotentiallyOp, bool* isPotentiallyLongComm, bool* isPotentiallyString2, bool* isPotentiallyComm, bool* isPotentiallyHexBin, char* char2, char* char1, bool* nouvTok, strlist* tokenAdd, intlist* typeTok, intlist* lines, int* debTok, int i, int* stepNumber, int* stepHexBin, int len_string, int* nombrePoints);
void debutTokensSimples(int i, int* debTok, char* char1, bool* isPotentiallyString, bool* isPotentiallyNumber, bool* isPotentiallyWord, bool* isPotentiallyOp, bool* isPotentiallyString2, bool* isPotentiallyComm, bool* isPotentiallyLongComm, bool* isPotentiallyHexBin);
void annulerOuPoursuivre(bool* isPotentiallyPar, bool* isPotentiallyFonc, int* foncStep, bool* isPotentiallyInst, int* instStep, bool* isPotentiallyListIndex, int* listIndexStep, strlist* tokenAdd, intlist* typeTok, int k, int *foncParDeb, int *listeParDeb, int *indexParDeb, int* nbCro);
void debutTokensComposes(int k, intlist* typeTok, strlist* tokenAdd, int* debTok2, int* debTok3, int* lastDebTok, bool* nouvTokComp, bool* isPotentiallyList, bool* isPotentiallyPar, bool* isPotentiallyFonc, bool* isPotentiallyListIndex, bool* isPotentiallyBlock, bool* isPotentiallyInst, int* listIndexStep, int *foncStep, int *instStep, int *foncParDeb, int *listeParDeb, int *indexParDeb, int* nbPar, int* nbCro);
void finsTypesComposes(int *debTok2, int *debTok3, int* lastDebTok, int k, bool *nouvTokComp, bool* isPotentiallyPar, bool *isPotentiallyFonc, int *foncStep, strlist* tokenAdd, intlist typeTok, int *nbPar, bool *isPotentiallyInst, int *instStep, int *listIndexStep, bool *isPotentiallyList, bool *isPotentiallyListIndex, int *nbCro, bool *isPotentiallyBlock, int *nbAcc, int *foncParDeb, int *listeParDeb, int *indexParDeb, Ast** ast, int offset, intlist* lines);
void verificationGrammaire(strlist* tokens, Ast** ast, bool* isPotentiallyExpr, int* debExpr);
void compConditionBlock(int cas, bool* isPotentiallyConBlock, int *conBlockStep, strlist* tokens, intlist* types, int *i, char* typeact, int* debConBlock);
void statements(intlist* types, strlist* tokens, Ast** ast, intlist* lines, int offset);
void cut(strlist* tokens, intlist* types, char* str, bool traiterStatements, intlist* lines);
bool isString(char* string, char* test, int size);
void parse(strlist* tokenAdd, intlist typeTok, Ast** ast, intlist* lines, int offset);
Ast** ast_create(intlist* typeTok);
void ast_push(Ast* ast);
int ast_pop(Ast* ast);
void ast_destroy(Ast ** ast, int length);
void ast_aff(Ast** ast, int len);
bool strlist_ast_inList(strlist* list, Ast** ast, char* elt, int type, int offset);
bool ast_typeInAst(Ast** ast, int len, int type, int offset);
int ast_typeCountAst(Ast** ast, int len, int type, int offset);
int ast_minOp(Ast** ast, strlist* tokens, int offset);
int ast_length(Ast** ast, int token_length, int offset);
int ast_minOp(Ast** ast, strlist* tokens, int offset);
int ast_length(Ast** ast, int token_length, int offset);
int ast_index(Ast** ast, int index, int offset);
int ast_prec(Ast** ast, int index, int offset);




/*ERRFONC.C*/
#define err_malloc(taille)             malloc(taille)
#define err_realloc(ptr, taille)       realloc(ptr, taille)
#define err_free(ptr)                  free(ptr)
void err_memset(void* ptr, int valeur, size_t taille);
void b(double);
void printError(int code);
void segfault(void);





/* LIBLIST.C*/
void strlist_copy(strlist* list, const char** tab, int len);
void listlist_copy(listlist* list, const intlist* tab, int len);
void intlist_copy(intlist* list, const int* tab, int len);



//prototypes des fonctions

/**************************listlist**********************/
listlist listlist_create(int len);
void listlist_append(listlist* list, intlist* ptr);
void listlist_remove(listlist* list, int index);
void listlist_aff(listlist* list);
void listlist_destroy(listlist* list);



/**************************ptrlist**********************/
ptrlist* ptrlist_create(void);
void ptrlist_append(ptrlist* list, void* ptr);
void ptrlist_remove(ptrlist* list, void* ptr, bool error);
int ptrlist_index(ptrlist* list, void* ptr);
void ptrlist_aff(ptrlist* l);
int ptrlist_len(ptrlist* l);
int ptrlist_destroy(ptrlist* l, bool freeElements, bool freeTab);
void ptrlist_replace(ptrlist* liste, void* aRemplacer, void* nvlleValeur);
bool ptrlist_isEmpty(ptrlist* l);
void* ptrlist_pop(ptrlist* list);
bool ptrlist_inList(ptrlist* l, void* el);
void ptrlist_direct_remove(ptrlist* list, ptrlist* ptr, ptrlist* prev);


/*********************intptrlist**********************/
intptrlist intptrlist_create(int len);
void intptrlist_append(intptrlist* list, int* ptr);
void intptrlist_destroy(intptrlist* list);

/**************************intlist**********************/
intlist intlist_create(int len);
void intlist_aff(intlist* list);
void intlist_append(intlist* list,int nombre);
void intlist_resize(intlist* list, int newLen);
void intlist_remove(intlist* list,int index);
int intlist_count(intlist* list, int nb);
bool intlist_inList(intlist* list, int nombre);
int intlist_index(intlist* list, int nombre);
void intlist_insert(intlist* list,int nombre, int index);
int intlist_max(intlist* list);



/**************************strlist**********************/
void strlist_destroy(strlist* list, bool bo);
strlist* strlist_create(int len);
void strlist_aff(strlist* list);
void strlist_append(strlist* list, char* chaine);
void strlist_resize(strlist* list, int newLen, bool freeElement);
void strlist_remove(strlist* list,int index, bool freeElement);
int strlist_count(strlist* list, char* chaine);
bool strlist_inList(strlist* list, char* chaine);
int strlist_index(strlist* list, char* chaine);
void strlist_insert(strlist* list,char* chaine, int index);



/*LIBSTR.C*/
char* inputCode(char*);
char* sub(char* string,int debut,int fin);//a liberer après
char* replace(char*, char*, char*);//a liberer après
void cleanStdin(void);
char* addStr(char* str1, char* str2);//a liberer après
int count(char* string, char* search);
char* subReplace(char* string, int len, int debut, int longueur, char* remplacement, int len_remplacement);
int sindex (char* string, char car);
char* traitementString(char* string);//a liberer apres
char* traitementStringInverse(char* string);//a liberer apres
char* addStr2(char* str1, char* str2);
char* charToString(char car);
char* sandwich(char* string, char car);
bool strToBool(char* string);
bool isidentifier(char c);
bool has_strict_prefix(char* string, char* prefix);
char* remove_prefix(char* string, char* prefix);
char* get_prefix(char* string);
strlist* get_all_modules(void);
bool is_module(char* module);
int function_module(char* module, char* function);



/*OPFONC.C*/
NeObj _and(Tree* tree1, Tree* tree2);
NeObj _or(Tree* tree1, Tree* tree2);
NeObj _xor(NeObj op1, NeObj op2);
NeObj _add(NeObj op1, NeObj op2);
NeObj _mul(NeObj op1, NeObj op2);
NeObj _sub(NeObj op1, NeObj op2);
NeObj _div(NeObj op1, NeObj op2);
NeObj _pow(NeObj op1, NeObj op2);
NeObj _equal(NeObj op1, NeObj op2);
NeObj _notEq(NeObj op1, NeObj op2);
NeObj _infEqual(NeObj op1, NeObj op2);
NeObj _supEqual(NeObj op1, NeObj op2);
NeObj _inf(NeObj op1, NeObj op2);
NeObj _sup(NeObj op1, NeObj op2);
NeObj _affect(NeObj op1, NeObj* op2);
NeObj _affectNone(NeObj* op1, NeObj op2);
void _affect2(NeObj* op1, NeObj op2);
NeObj _addEqual(NeObj* op1, NeObj op2);
NeObj _subEqual(NeObj* op1, NeObj op2);
NeObj _mulEqual(NeObj* op1, NeObj op2);
NeObj _divEqual(NeObj* op1, NeObj op2);
NeObj _incr(NeObj* op1);
void _incr2(NeObj* op1, int incr);
NeObj _decr(NeObj* op1);
NeObj _not(NeObj op1);
NeObj _mod(NeObj op1, NeObj op2);
NeObj _eucl(NeObj op1, NeObj op2);
NeObj _ref(NeObj op1);
NeObj _deref(NeObj op1);
NeObj _goIn(NeObj op1, NeObj op2);
NeObj _minus(NeObj op1);
NeObj _del(NeObj* op1);
NeObj _exponent(NeObj op1, NeObj op2);
NeObj _implique(NeObj op1, NeObj op2);
NeObj _in(NeObj op1, NeObj op2);
NeObj _swap(NeObj* op1, NeObj* op2);

/*BUILTINSFONC.C*/
#define ARG(n)      args->tab[n]
NeObj _print_(NeList* args);
NeObj _input_(NeList* args);
NeObj _nbr_(NeList* args);
NeObj _str_(NeList* args);
NeObj _len_(NeList* args);
NeObj _substring_(NeList* args);
NeObj _exit_(NeList* args);
NeObj _append_(NeList* args);
NeObj _remove_(NeList* args);
NeObj _insert_(NeList* args);
NeObj _type_(NeList* args);
NeObj _reverse_(NeList* args);
NeObj _eval_(NeList* args);
NeObj _clear_(NeList* args);
NeObj _help_(NeList* args);
NeObj _randint_(NeList* args);
NeObj _failwith_(NeList* args);
NeObj _time_(NeList* args);
NeObj _assert_(NeList* args);
NeObj _output_(NeList* args);
NeObj _chr_(NeList* args);
NeObj _ord_(NeList* args);
NeObj _list_comp_(NeList* args);
NeObj _create_exception_(NeList* args);
NeObj _raise_(NeList* args);
NeObj _int_(NeList* args);
NeObj _index_(NeList* args);
NeObj _replace_(NeList* args);
NeObj _count_(NeList* args);
NeObj _list_(NeList* args);
NeObj _sort_asc_(NeList* args);
NeObj _sort_desc_(NeList* args);
NeObj _sin_(NeList* args);
NeObj _cos_(NeList* args);
NeObj _tan_(NeList* args);
NeObj _deg_(NeList* args);
NeObj _rad_(NeList* args);
NeObj _sqrt_(NeList* args);
NeObj _ln_(NeList* args);
NeObj _exp_(NeList* args);
NeObj _log_(NeList* args);
NeObj _log2_(NeList* args);
NeObj _round_(NeList* args);
NeObj _abs_(NeList* args);
NeObj _ceil_(NeList* args);
NeObj _floor_(NeList* args);
NeObj _writeFile_(NeList* args);
NeObj _readFile_(NeList* args);
NeObj _setFunctionDoc_(NeList* args);
NeObj _setAtomicTime_(NeList* args);
NeObj _copy_(NeList* args);
NeObj _load_namespace_(NeList* args);
NeObj _gc_(NeList* args);
NeObj _setColor_(NeList* args);


/*AFFPROG.C*/
void affLine(char* file, int line);
void affExpr(Tree* tree);


/*LIBMEM.C*/
void print_objects_list(void);
void gc_free_objects_list(void);
void gc_add_deep_object(const NeObj neo);
void gc_remove_container(Container* c);
void gc_remove_nelist(NeList* l);
void neobject_partial_destroy(NeObj neo);
void nelist_partial_destroy(NeList* neo);
void gc_nelist_mark(NeList* list);
void gc_mark(NeObj obj);
void gc_mark_and_sweep(void);
void gc_final_sweep(void);
void print_objects_list(void);


/* RUNTIME.C*/
bool neoIsTrue(NeObj neo);
bool isTrue(Tree* tree);
NeObj eval(Tree* tree);
NeObj* get_address(Tree* tree);
void local(Var var, ptrlist* var_loc);
void newContext(ptrlist* var_loc);
void deleteContext(ptrlist* var_loc);
int exec_aux(Tree* tree);
void exec(Tree* tree);
bool isTrue(Tree* tree);
void updateFileName(char* name);
ProcessCycle* processCycle_create(void);
NeObj eval_prolog(Tree* tree);
NeObj eval_aux(Tree* tree);
NeObj eval_epilog(void);
NeObj callUserFunc(UserFunc* fun, NeList* args, NeObj neo_local_args);
void initRuntime(void);
void exitRuntime(void);

/*------------------------------------LINENOISE.H-------------------------------------*/
#ifdef LINUX

#ifndef __LINENOISE_H
#define __LINENOISE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> /* For size_t. */

extern char* linenoiseEditMore;

/* The linenoiseState structure represents the state during line editing.
 * We pass this state to functions implementing specific editing
 * functionalities. */
struct linenoiseState {
    int in_completion;  /* The user pressed TAB and we are now in completion
                         * mode, so input is handled by completeLine(). */
    size_t completion_idx; /* Index of next completion to propose. */
    int ifd;            /* Terminal stdin file descriptor. */
    int ofd;            /* Terminal stdout file descriptor. */
    char* buf;          /* Edited line buffer. */
    size_t buflen;      /* Edited line buffer size. */
    const char* prompt; /* Prompt to display. */
    size_t plen;        /* Prompt length. */
    size_t pos;         /* Current cursor position. */
    size_t oldpos;      /* Previous refresh cursor position. */
    size_t len;         /* Current edited line length. */
    size_t cols;        /* Number of columns in terminal. */
    size_t oldrows;     /* Rows used by last refrehsed line (multiline mode) */
    int history_index;  /* The history index we are currently editing. */
};

typedef struct linenoiseCompletions {
  size_t len;
  char** cvec;
} linenoiseCompletions;

/* Non blocking API. */
int linenoiseEditStart(struct linenoiseState *l, int stdin_fd, int stdout_fd, char* buf, size_t buflen, const char* prompt);
char* linenoiseEditFeed(struct linenoiseState *l);
void linenoiseEditStop(struct linenoiseState *l);
void linenoiseHide(struct linenoiseState *l);
void linenoiseShow(struct linenoiseState *l);

/* Blocking API. */
char* linenoise(const char* prompt);
void linenoiseFree(void *ptr);

/* Completion API. */
typedef void(linenoiseCompletionCallback)(const char* , linenoiseCompletions *);
typedef char*(linenoiseHintsCallback)(const char* , int *color, int *bold);
typedef void(linenoiseFreeHintsCallback)(void *);
void linenoiseSetCompletionCallback(linenoiseCompletionCallback *);
void linenoiseSetHintsCallback(linenoiseHintsCallback *);
void linenoiseSetFreeHintsCallback(linenoiseFreeHintsCallback *);
void linenoiseAddCompletion(linenoiseCompletions *, const char* );

/* History API. */
int linenoiseHistoryAdd(const char* line);
int linenoiseHistorySetMaxLen(int len);
int linenoiseHistorySave(const char* filename);
int linenoiseHistoryLoad(const char* filename);

/* Other utilities. */
void linenoiseClearScreen(void);
void linenoiseSetMultiLine(int ml);
void linenoisePrintKeyCodes(void);
void linenoiseMaskModeEnable(void);
void linenoiseMaskModeDisable(void);

#ifdef __cplusplus
}
#endif

#endif /* __LINENOISE_H */

#endif
