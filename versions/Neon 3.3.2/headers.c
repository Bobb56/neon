#include <stdlib.h>
#include <stdint.h>

#define VERSION "3.3.2.1-beta"
/*
Numérotation de versions :
Ajout d'opérateur ou de fonction ou modification d'un élément de syntaxe : +0.0.1
Ajout d'une nouvelle syntaxe ou modification de la structure d'exécution : +0.1
Modification mineure : +0.0.0.1
Modification du look de l'interpréteur en mode console : +0.1
Ajout d'un type de données NeObject : +0.0.1
*/

#define INVITATION "https://discord.gg/wkBdK35w2a"

// à définir en fonction des destinations de compilation
#define LINUX
//#define WINDOWS10
//#define WINDOWS11
//#define WASM
//#define TI83PCE


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

#define SEQUENCE_ENTREE ">> "

#define NB_ERRORS 106

#define VALSTACK_INITIAL_SIZE 1000
#define VARSTACK_INITIAL_SIZE 5000
#define RETSTACK_INITIAL_SIZE 1000

/*constantes de couleur*/
#define BLUE 0
#define RED 1
#define WHITE 2
#define GREEN 3

#define PI 3.141592653589793

#define NBLOCALVARS 7 // nombre maximal de variables locales à empiler pour la fonction execval


//fin de fichier
#define EOF (-1)

//pour les booléens :
#define true 1
#define false 0


// pour builtinfonc et opfonc
#define NBOPERATEURS 40
#define NBBUILTINFONC 50


//definition des types d’arguments pris par les operateurs
#define VARLEFT 1 // une variable a gauche
#define RIGHT 2 // une operande a droite
#define RIGHT_LEFT 3 // une operande a droite et a gauche
#define VAR_RIGHT 4 // une variable a gauche et une operande a droite
#define LEFT_VAR 6 // une variable à doite et n'importe quoi à gauche
#define VARRIGHT 5 // une variable a droite
#define VAR_VAR 7 // une variable à droite et une variable à gauche
#define SPECIAL 8 // les opérateurs que l'on peut retrouver partout

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

#define TYPE_FONCTION           0
#define TYPE_LISTINDEX          1
#define TYPE_OPERATOR           2
#define TYPE_STRING             3
#define TYPE_NUMBER             4
#define TYPE_LIST               5
#define TYPE_BLOCK              6
#define TYPE_BOOL               7
#define TYPE_CONST              8
#define TYPE_VARIABLE           9
#define TYPE_BLOCKLINE          10
#define TYPE_ENDOFLINE          11
#define TYPE_PARENTHESE1        12
#define TYPE_PARENTHESE2        13
#define TYPE_VIRGULE            14
#define TYPE_KEYWORD            15
#define TYPE_NONE               16
#define TYPE_BLOCKWORD1LINE     17
#define TYPE_KEYWORDFUNCTION    18
#define TYPE_EXCEPTION          19
#define TYPE_EXPRESSION         20

#define TYPE_COMMENT            21
#define TYPE_WORD               22
#define TYPE_CROCHET            23
#define TYPE_BLOCKWORD          24
#define TYPE_ACCOLADE           25
#define TYPE_CONDITIONBLOCK     32

#define TYPE_STATEMENTFOR       33
#define TYPE_STATEMENTWHILE     34
#define TYPE_STATEMENTIF        35
#define TYPE_STATEMENTELIF      36
#define TYPE_STATEMENTELSE      37
#define TYPE_STATEMENTTRY       38
#define TYPE_STATEMENTEXCEPT    39

#define TYPE_FUNCTIONDEF        40
#define TYPE_METHODDEF          41

#define TYPE_BLOCKFOR           42
#define TYPE_BLOCKWHILE         43
#define TYPE_BLOCKIF            44
#define TYPE_BLOCKELIF          45
#define TYPE_BLOCKELSE          46
#define TYPE_BLOCKTRY           47
#define TYPE_BLOCKEXCEPT        48
#define TYPE_TRYEXCEPT          49
#define TYPE_ATOMICLINE         50
#define TYPE_ATOMICBLOCK        51

#define TYPE_SYNTAXTREE         52

#define TYPE_BUILTINFUNC        53
#define TYPE_USERMETHOD         54
#define TYPE_USERFUNC           55
#define TYPE_CONTAINER          56
#define TYPE_ATTRIBUTE          57
#define TYPE_PROMISE            58

#define TYPE_EMPTY              59


// defines pour les zones de retour de execval
#define CASE_DEFAUT             0
#define CASE_OP1                1
#define CASE_OP2                2
#define CASE_OP3                3
#define CASE_OP4                4
#define CASE_OP5                5
#define CASE_PARALLEL           6
#define CASE_OP6 7
#define CASE_AWAIT              8
#define CASE_OP7                9
#define CASE_ATOMIC             10
#define CASE_LIST               11
#define CASE_FUNCTION1          12
#define CASE_FUNCTION2          13
#define CASE_LISTINDEX1         14
#define CASE_LISTINDEX2         15
#define CASE_CONTAINER          16
#define CASE_ATTRIBUTE          17
#define CASE_EXPRESSION         18
#define CASE_CONDITIONBLOCK1    19
#define CASE_CONDITIONBLOCK2    20
#define CASE_CONDITIONBLOCK3    21
#define CASE_CONDITIONBLOCK4    22
#define CASE_CONDITIONBLOCK5    23
#define CASE_WHILE1             24
#define CASE_WHILE2             25
#define CASE_WHILE3             26
#define CASE_FOR1               27
#define CASE_FOR2               28
#define CASE_FOR3               29
#define CASE_IMPORT             30
#define CASE_TRYEXCEPT1         31
#define CASE_TRYEXCEPT2         32
#define CASE_FUNCTION3          33
#define CASE_FUNCTION4          34
#define CASE_BLOCKWORD1LINE     35
#define CASE_FUNCTION0          36
#define CASE_PARALLEL0          37
#define CASE_ENDOFSTACK         38



// define des noms de variables locales pour execval

// POUR FOR
//    NORMAL
#define LOCAL_FOR_inst             varstack->tab[varstack->len - 1]
#define LOCAL_FOR_sov_vars_to_save varstack->tab[varstack->len - 2]
#define LOCAL_FOR_ext_index        varstack->tab[varstack->len - 3]
#define LOCAL_FOR_var              varstack->tab[varstack->len - 4]
#define LOCAL_FOR_max              varstack->tab[varstack->len - 5]
#define LOCAL_FOR_tree             varstack->tab[varstack->len - 6]
#define LOCAL_FOR_calcListIndex    varstack->tab[varstack->len - 7]
//    CASTED
#define LOCAL_FOR_inst_c             ((intptr_t)LOCAL_FOR_inst)
#define LOCAL_FOR_ext_index_c        ((intptr_t)LOCAL_FOR_ext_index)
#define LOCAL_FOR_var_c              ((NeObject*)LOCAL_FOR_var)
#define LOCAL_FOR_max_c              ((intptr_t)LOCAL_FOR_max)
#define LOCAL_FOR_tree_c             ((Tree*)LOCAL_FOR_tree)
#define LOCAL_FOR_calcListIndex_c    ((intptr_t)LOCAL_FOR_calcListIndex)


// POUR CONDITIONBLOCK
//    NORMAL
#define LOCAL_CB_inst             varstack->tab[varstack->len - 1]
#define LOCAL_CB_sov_vars_to_save varstack->tab[varstack->len - 2]
#define LOCAL_CB_bloc             varstack->tab[varstack->len - 3]
#define LOCAL_CB_int_ret          varstack->tab[varstack->len - 4]
#define LOCAL_CB_tree             varstack->tab[varstack->len - 5]
#define LOCAL_CB_calcListIndex    varstack->tab[varstack->len - 6]
//    CASTED
#define LOCAL_CB_inst_c             ((intptr_t)LOCAL_CB_inst)
#define LOCAL_CB_bloc_c             ((intptr_t)LOCAL_CB_bloc)
#define LOCAL_CB_int_ret_c          ((intptr_t)LOCAL_CB_int_ret)
#define LOCAL_CB_tree_c             ((Tree*)LOCAL_CB_tree)
#define LOCAL_CB_calcListIndex_c    ((intptr_t)LOCAL_CB_calcListIndex)


// POUR OPERATOR
//    NORMAL
#define LOCAL_OP_tree             varstack->tab[varstack->len - 1]
#define LOCAL_OP_calcListIndex    varstack->tab[varstack->len - 2]
//    CASTED
#define LOCAL_OP_tree_c             ((Tree*)LOCAL_OP_tree)
#define LOCAL_OP_calcListIndex_c    ((intptr_t)LOCAL_OP_calcListIndex)



#ifndef TI83PCE
    typedef double* Number;
#else
    typedef real_t* Number;
#endif




typedef struct ChainonPtr
{
    void* tete;
    struct ChainonPtr* queue;
} ptrlist;



typedef struct
{
  void** tab;
  int len;
  int capacity;
    
} Stack;


typedef struct
{
  int* tab;
  int len;
  int capacity;
    
} intlist;




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
    char type;
    int fin;
    struct Ast* suiv;
} Ast;




typedef struct NeObject
{
    void* data;
    char type;
} NeObject;





typedef struct NeList
{
    NeObject** tab; // tableau de pointeurs de NeObjects
    int len;
    int capacity;
} NeList;




typedef struct Function
{
    char type;
    NeObject* (*ptr) (NeList*);
    const char* help;
    int nbArgs;
    int* typeArgs;
    int typeRetour;
    int nbArgsMeth;
} Function;




struct Tree
{
    int type;
    int line;
    char* label1;
    int label2;
    struct Tree** sons;
    int nbSons;
    int capacity;
    NeObject* data; // permet de stocker les données contenues dans les feuilles
};

typedef struct Tree Tree;





typedef struct UserFunc
{
    NeObject** args;
    int nbArgs;
    _Bool unlimited_arguments;
    int nbOptArgs; // nombre d'arguments vraiment, vraiment optionnels (par définition, ceux qui sont après ...)
    Tree * code;
    Tree* opt_args;// expressions pour calculer les arguments optionnels
    char* doc;
} UserFunc;


typedef struct NeSave
{
    NeObject* object;
    void* data;
    int type;
} NeSave;



typedef struct Container
{
    int type;
    NeList* data;
} Container;


typedef struct Process
{
    Stack* valstack;
    Stack* varstack;
    Stack* retstack;
    ptrlist* var_loc; // les variables locales créés depuis le lancement du processus
    int id;
    Tree* wait_expr;
    ptrlist* varsToSave;
} Process;

typedef struct ProcessCycle
{
    Process* process;
    struct ProcessCycle* next;
    struct ProcessCycle* prev;
} ProcessCycle;




/*MAIN.C*/
_Bool neoIsTrue(NeObject* neo);
NeObject* execval(Tree* tree);
int indexListIndex(char* listindex);
void printRes(NeObject* res);
void local(NeObject* var);
void newContext(void);
void deleteContext(void);
int exec(Tree* tree);
_Bool isTrue(Tree* tree);
int execConditionBlock(Tree* tree);
void execFile(char* filename);
void importFile(char* filename);
void treeToList(Tree* tree, NeObject* list, int nbArgsMeth);
void startMessage(void);
void terminal (void);
void neonInit(void);
void neonExit(void);




/*NEONIO.C*/
char* input(char*);
void printInt(int n);
void printNumber(Number n);
void flush(void);
void printString(char* s);
void newLine(void);
void setColor(unsigned char color);
void printBack(void);
void clearConsole(void);
char* openFile(char* filename);
void writeFile(char* filename, char* content);
char* argsAns(void);



/*NUMBERS.C*/
Number number_default(void);
Number number_fromStr(char *string);
char* number_toStr(Number number);
_Bool number_isInf(Number n);
void number_destroy(Number number);
Number number_copy(Number number);
Number number_add(Number n1, Number n2);
Number number_sub(Number n1, Number n2);
Number number_mul(Number n1, Number n2);
Number number_div(Number n1, Number n2);
Number number_pow(Number n1, Number n2);
_Bool number_equal(Number n1, Number n2);
_Bool number_inf(Number n1, Number n2);
_Bool number_infEq(Number n1, Number n2);
_Bool number_sup(Number n1, Number n2);
_Bool number_supEq(Number n1, Number n2);
Number number_minus(Number n1);
Number number_mod(Number n1, Number n2);
Number number_eucl(Number nb1, Number nb2);
void number_aff(Number number);
Number number_fromDouble(double num);
double number_toDouble(Number num);
int number_compare(Number a, Number b);
Number number_sin(Number a);
Number number_cos(Number a);
Number number_tan(Number a);
Number number_deg(Number a);
Number number_rad(Number a);
Number number_sqrt(Number a);
Number number_ln(Number a);
Number number_exp(Number a);
Number number_log(Number a);
Number number_log2(Number a);
Number number_round(Number a,Number b);
Number number_abs(Number a);
Number number_ceil(Number a);
Number number_floor(Number a);
Number number_randint(Number a, Number b);
_Bool number_isDefault(Number n);

/*VARLIB*/
NeObject* neobject_create(char type);
NeObject* neo_container_create(int type, NeList* data);
NeObject* neo_promise_create(intptr_t);
void neobject_destroy(NeObject* neo,_Bool bo);
void neobject_aff(NeObject* neo);
NeList* nelist_create(int len);
void nelist_append(NeList* list, NeObject* ptr);//ajoute un élément à la fin de la liste
void nelist_remove(NeList* list,int index);
void nelist_aff(NeList* liste);
void nelist_destroy(NeList* list,_Bool bo);
NeObject* neo_nb_create(Number number);
NeObject* neo_bool_create(_Bool neon_boolean);
_Bool neo_is_true(NeObject* neo);
NeObject* neo_str_create(char* string);
NeObject* neo_list_create(int len);
void neo_list_append(NeObject* neo, NeObject* ptr);
NeObject* neo_list_nth(NeObject* neo, int index);
NeObject* neo_const_create(char* string);
Number neo_to_nb(NeObject* neo);
_Bool neo_to_bool(NeObject* neo);
char* neo_to_string(NeObject* neo);
char* neo_to_const(NeObject* neo);
NeObject* neo_none_create(void);
char* neo_to_none(NeObject* neo);
NeObject* neo_copy(NeObject* neo);
int neo_list_len(NeObject* neo);
int nelist_index(NeList* liste, NeObject* neo);
int nelist_index2(NeList* l, NeObject* neo);
void nelist_insert(NeList* list,NeObject* neo, int index);
void neo_list_insert(NeObject* neo, NeObject* ptr, int index);
void neo_list_remove(NeObject* neo, int index);
char* neobject_str(NeObject*);
char* nelist_str(NeList*);
NeObject* neo_list_convert(NeList* list);
Function* function_create(int nbArgsMeth, NeObject * (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour);
NeObject* neo_fun_create(int nbArgsMeth,NeObject * (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour);
void compFunc(const int* typesbuiltinsfonc, const char** helpbuiltinsfonc, const int* nbargs, const int** typesArgs, const int* typesRetour);
_Bool funcArgsCheck(Function* fun, NeList* args);
NeObject* functionCall(NeObject* fun, NeList* args);
char* type(NeObject* neo);
NeObject* userFuncCreate(NeObject** args, Tree* code, int nbArgs, _Bool unlimited_arguments, int nbOptArgs, Tree* opt_args);
NeObject* neo_exception_create(int index);
_Bool neo_equal(NeObject* _op1, NeObject* _op2);
_Bool nelist_inList(NeList* list, NeObject* neo);
NeList* nelist_copy(NeList* l);
_Bool nelist_equal(NeList* l1, NeList* l2);
int neo_compare(NeObject* a, NeObject* b); // ordre croissant
int neo_compare2(NeObject* a, NeObject* b); // ordre décroissant



/*LIBTREE.C*/
Tree* tree_create(char* label1, int label2, char type);
void tree_appendSon(Tree* tree, Tree* son);
_Bool tree_isEmpty(Tree* tree);
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
void createFunctionTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset, _Bool isMethod);
void createSyntaxTreeAux(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset);




/*LIBTOK.C*/



_Bool isFull(char* code);
char* nomBlockLine(char* blockline);//a liberer apres
void finTokensSimples(char* string, _Bool* isPotentiallyNumber, _Bool* isPotentiallyString, _Bool* isPotentiallyWord, _Bool* isPotentiallyOp, _Bool* isPotentiallyLongComm, _Bool* isPotentiallyString2, _Bool* isPotentiallyComm, _Bool* isPotentiallyHexBin, char * char2, char* char1, _Bool* nouvTok, strlist* tokenAdd, intlist* typeTok, intlist* lines, int* debTok, int i, int* stepNumber, int* stepHexBin, int len_string, int* nombrePoints);
void debutTokensSimples(int i, int* debTok, char* char1, _Bool* isPotentiallyString, _Bool* isPotentiallyNumber, _Bool* isPotentiallyWord, _Bool* isPotentiallyOp, _Bool* isPotentiallyString2, _Bool* isPotentiallyComm, _Bool* isPotentiallyLongComm, _Bool* isPotentiallyHexBin);
void annulerOuPoursuivre(_Bool* isPotentiallyPar, _Bool* isPotentiallyFonc, int* foncStep, _Bool* isPotentiallyInst, int* instStep, _Bool* isPotentiallyListIndex, int* listIndexStep, strlist* tokenAdd, intlist* typeTok, int k, int *foncParDeb, int *listeParDeb, int *indexParDeb, int* nbCro);
void debutTokensComposes(int k, intlist* typeTok, strlist* tokenAdd, int* debTok2, int* debTok3, int* lastDebTok, _Bool* nouvTokComp, _Bool* isPotentiallyList, _Bool* isPotentiallyPar, _Bool* isPotentiallyFonc, _Bool* isPotentiallyListIndex, _Bool* isPotentiallyBlock, _Bool* isPotentiallyInst, int* listIndexStep, int *foncStep, int *instStep, int *foncParDeb, int *listeParDeb, int *indexParDeb, int* nbPar, int* nbCro);
void finsTypesComposes(int *debTok2, int *debTok3, int* lastDebTok, int k, _Bool *nouvTokComp, _Bool* isPotentiallyPar, _Bool *isPotentiallyFonc, int *foncStep, strlist* tokenAdd, intlist typeTok, int *nbPar, _Bool *isPotentiallyInst, int *instStep, int *listIndexStep, _Bool *isPotentiallyList, _Bool *isPotentiallyListIndex, int *nbCro, _Bool *isPotentiallyBlock, int *nbAcc, int *foncParDeb, int *listeParDeb, int *indexParDeb, Ast** ast, int offset, intlist* lines);
void verificationGrammaire(strlist* tokens, Ast** ast, _Bool* isPotentiallyExpr, int* debExpr);
void compConditionBlock(int cas, _Bool* isPotentiallyConBlock, int *conBlockStep, strlist* tokens, intlist* types, int *i, char* typeact, int* debConBlock);
void statements(intlist* types, strlist* tokens, Ast** ast, intlist* lines, int offset);
void cut(strlist* tokens, intlist* types, char* str, _Bool traiterStatements, intlist* lines);
_Bool isString(char* string, char* test, int size);
void parse(strlist* tokenAdd, intlist typeTok, Ast** ast, intlist* lines, int offset);
Ast** ast_create(intlist* typeTok);
void ast_push(Ast* ast);
int ast_pop(Ast* ast);
void ast_destroy(Ast ** ast, int length);
void ast_aff(Ast** ast, int len);
_Bool strlist_ast_inList(strlist* list, Ast** ast, char* elt, int type, int offset);
_Bool ast_typeInAst(Ast** ast, int len, int type, int offset);
int ast_typeCountAst(Ast** ast, int len, int type, int offset);
int ast_minOp(Ast** ast, strlist* tokens, int offset);
int ast_length(Ast** ast, int token_length, int offset);
int ast_minOp(Ast** ast, strlist* tokens, int offset);
int ast_length(Ast** ast, int token_length, int offset);
int ast_index(Ast** ast, int index, int offset);
int ast_prec(Ast** ast, int index, int offset);




/*ERRFONC.C*/
void* err_malloc(size_t taille);
void err_memset(void* ptr, int valeur, size_t taille);
void* err_realloc(void* ptr, size_t taille);
void err_free(void* ptr);
void b(double);
void printError(int code);
void segfault(void);





/* LIBLIST.C*/

Stack* stack_create(int len);
void stack_check_for(Stack* stack, int n);
void stack_push(Stack* list, void* ptr);
void* stack_pop(Stack* stack);
void stack_destroy(Stack* stack);
void stack_extend(Stack* stack, int size);
void stack_reduce(Stack* stack, int size);


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
void ptrlist_remove(ptrlist* list, void* ptr, _Bool error);
int ptrlist_index(ptrlist* list, void* ptr);
void ptrlist_aff(ptrlist* l);
int ptrlist_len(ptrlist* l);
int ptrlist_destroy(ptrlist* l, _Bool freeElements, _Bool freeTab);
void ptrlist_replace(ptrlist* liste, void* aRemplacer, void* nvlleValeur);
_Bool ptrlist_isEmpty(ptrlist* l);
void* ptrlist_pop(ptrlist* list);
_Bool ptrlist_inList(ptrlist* l, void* el);


/**************************intlist**********************/
intlist intlist_create(int len);
void intlist_aff(intlist* list);
void intlist_append(intlist* list,int nombre);
void intlist_resize(intlist* list, int newLen);
void intlist_remove(intlist* list,int index);
int intlist_count(intlist* list, int nb);
_Bool intlist_inList(intlist* list, int nombre);
int intlist_index(intlist* list, int nombre);
void intlist_insert(intlist* list,int nombre, int index);
int intlist_max(intlist* list);



/**************************strlist**********************/
void strlist_destroy(strlist* list, _Bool bo);
strlist* strlist_create(int len);
void strlist_aff(strlist* list);
void strlist_append(strlist* list, char *chaine);
void strlist_resize(strlist* list, int newLen, _Bool freeElement);
void strlist_remove(strlist* list,int index, _Bool freeElement);
int strlist_count(strlist* list, char* chaine);
_Bool strlist_inList(strlist* list, char* chaine);
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
_Bool strToBool(char* string);






/*OPFONC.C*/
NeObject* _and(NeObject* op1, NeObject* op2);
NeObject* _or(NeObject* op1, NeObject* op2);
NeObject* _xor(NeObject* op1, NeObject* op2);
NeObject* _add(NeObject* op1, NeObject* op2);
NeObject* _mul(NeObject* op1, NeObject* op2);
NeObject* _sub(NeObject* op1, NeObject* op2);
NeObject* _div(NeObject* op1, NeObject* op2);
NeObject* _pow(NeObject* op1, NeObject* op2);
NeObject* _equal(NeObject* op1, NeObject* op2);
NeObject* _notEq(NeObject* op1, NeObject* op2);
NeObject* _infEqual(NeObject* op1, NeObject* op2);
NeObject* _supEqual(NeObject* op1, NeObject* op2);
NeObject* _inf(NeObject* op1, NeObject* op2);
NeObject* _sup(NeObject* op1, NeObject* op2);
NeObject* _affect(NeObject* op1, NeObject* op2);
NeObject* _affectNone(NeObject* op1, NeObject* op2);
void _affect2(NeObject* op1, NeObject* op2);
NeObject* _addEqual(NeObject* op1, NeObject* op2);
NeObject* _subEqual(NeObject* op1, NeObject* op2);
NeObject* _mulEqual(NeObject* op1, NeObject* op2);
NeObject* _divEqual(NeObject* op1, NeObject* op2);
NeObject* _incr(NeObject* op1, NeObject* op2);
NeObject* _decr(NeObject* op1, NeObject* op2);
NeObject* _not(NeObject* op1, NeObject* op2);
NeObject* _mod(NeObject* op1, NeObject* op2);
NeObject* _eucl(NeObject* op1, NeObject* op2);
NeObject* _ref(NeObject* op1, NeObject* op2);
NeObject* _goIn(NeObject* op1, NeObject* op2);
NeObject* _minus(NeObject* op1, NeObject* op2);
NeObject* _del(NeObject* op1, NeObject* op2);
NeObject* _exponent(NeObject* op1, NeObject* op2);
NeObject* _implique(NeObject* op1, NeObject* op2);
NeObject* _in(NeObject* op1, NeObject* op2);
NeObject* _swap(NeObject* op1, NeObject* op2);

/*BUILTINSFONC.C*/
NeObject* _print_(NeList* args);
NeObject* _input_(NeList* args);
NeObject* _nbr_(NeList* args);
NeObject* _str_(NeList* args);
NeObject* _len_(NeList* args);
NeObject* _substring_(NeList* args);
NeObject* _exit_(NeList* args);
NeObject* _append_(NeList* args);
NeObject* _remove_(NeList* args);
NeObject* _insert_(NeList* args);
NeObject* _type_(NeList* args);
NeObject* _reverse_(NeList* args);
NeObject* _eval_(NeList* args);
NeObject* _clear_(NeList* args);
NeObject* _help_(NeList* args);
NeObject* _randint_(NeList* args);
NeObject* _quitError_(NeList* args);
NeObject* _time_(NeList* args);
NeObject* _assert_(NeList* args);
NeObject* _output_(NeList* args);
NeObject* _chr_(NeList* args);
NeObject* _ord_(NeList* args);
NeObject* _list_comp_(NeList* args);
NeObject *_create_exception_(NeList* args);
NeObject* _exception_(NeList* args);
NeObject * _int_(NeList* args);
NeObject* _index_(NeList* args);
NeObject* _replace_(NeList* args);
NeObject* _count_(NeList* args);
NeObject* _list_(NeList* args);
NeObject* _sort_asc_(NeList* args);
NeObject* _sort_desc_(NeList* args);
NeObject* _sin_(NeList* args);
NeObject* _cos_(NeList* args);
NeObject* _tan_(NeList* args);
NeObject* _deg_(NeList* args);
NeObject* _rad_(NeList* args);
NeObject* _sqrt_(NeList* args);
NeObject* _ln_(NeList* args);
NeObject* _exp_(NeList* args);
NeObject* _log_(NeList* args);
NeObject* _log2_(NeList* args);
NeObject* _round_(NeList* args);
NeObject* _abs_(NeList* args);
NeObject* _ceil_(NeList* args);
NeObject* _floor_(NeList* args);
NeObject* _writeFile_(NeList* args);
NeObject* _readFile_(NeList* args);
NeObject* _setFunctionDoc_(NeList* args);
NeObject* _setAtomicTime(NeList* args);




/*AFFPROG*/
void affLine(char* file, int line);
void affExpr(Tree* tree);




/*------------------------------------LINENOISE.H-------------------------------------*/
#ifdef LINUX

#ifndef __LINENOISE_H
#define __LINENOISE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> /* For size_t. */

extern char *linenoiseEditMore;

/* The linenoiseState structure represents the state during line editing.
 * We pass this state to functions implementing specific editing
 * functionalities. */
struct linenoiseState {
    int in_completion;  /* The user pressed TAB and we are now in completion
                         * mode, so input is handled by completeLine(). */
    size_t completion_idx; /* Index of next completion to propose. */
    int ifd;            /* Terminal stdin file descriptor. */
    int ofd;            /* Terminal stdout file descriptor. */
    char *buf;          /* Edited line buffer. */
    size_t buflen;      /* Edited line buffer size. */
    const char *prompt; /* Prompt to display. */
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
  char **cvec;
} linenoiseCompletions;

/* Non blocking API. */
int linenoiseEditStart(struct linenoiseState *l, int stdin_fd, int stdout_fd, char *buf, size_t buflen, const char *prompt);
char *linenoiseEditFeed(struct linenoiseState *l);
void linenoiseEditStop(struct linenoiseState *l);
void linenoiseHide(struct linenoiseState *l);
void linenoiseShow(struct linenoiseState *l);

/* Blocking API. */
char *linenoise(const char *prompt);
void linenoiseFree(void *ptr);

/* Completion API. */
typedef void(linenoiseCompletionCallback)(const char *, linenoiseCompletions *);
typedef char*(linenoiseHintsCallback)(const char *, int *color, int *bold);
typedef void(linenoiseFreeHintsCallback)(void *);
void linenoiseSetCompletionCallback(linenoiseCompletionCallback *);
void linenoiseSetHintsCallback(linenoiseHintsCallback *);
void linenoiseSetFreeHintsCallback(linenoiseFreeHintsCallback *);
void linenoiseAddCompletion(linenoiseCompletions *, const char *);

/* History API. */
int linenoiseHistoryAdd(const char *line);
int linenoiseHistorySetMaxLen(int len);
int linenoiseHistorySave(const char *filename);
int linenoiseHistoryLoad(const char *filename);

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
