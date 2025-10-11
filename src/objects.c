// bibliothèque générale de structures des données Neon

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/dynarrays.h"
#include "headers/gc.h"
#include "headers/operators.h"
#include "headers/runtime.h"
#include "headers/strings.h"
#include "headers/neon.h"
#include "headers/errors.h"



/////////////// MANIPULATION DES VARIABLES ///////////////////

void variable_append(NeonEnv* env, char* name, NeObj value) {
    nelist_append(env->ADRESSES, value);
    strlist_append(env->NOMS, strdup(name));
}

NeObj* get_absolute_address(Var rel_var_addr) {
    return nelist_nth_addr(global_env->ADRESSES, rel_var_addr);
}

NeObj get_var_value(Var rel_var_addr) {
    return nelist_nth(global_env->ADRESSES, rel_var_addr);
}


char* var_name(NeObj obj) {
    if (nelist_inList(global_env->ADRESSES, obj)) {
        return global_env->NOMS->tab[nelist_index(global_env->ADRESSES, obj)];
    }
    else {
        return "<anonymous object>";
    }
}

Var get_var_from_addr(NeObj* obj) {
    return ((intptr_t)obj - (intptr_t)global_env->ADRESSES->tab) / sizeof(NeObj);
}


// cette fonction récupère la variable nommée name, et la crée si elle n'existe pas
Var get_var(char* name) {
    int index = strlist_index(global_env->NOMS,name);
    if (index < 0) { // on crée la variable
        global_env->CODE_ERROR = 0;
        strlist_append(global_env->NOMS, strdup(name));
        nelist_append(global_env->ADRESSES, neo_empty_create());
        return global_env->ADRESSES->len - 1;
    }
    return index;
}

char* get_name(Var var) {
    return global_env->NOMS->tab[var];
}

// change le contenu d'une variable sans se soucier de ce qui était dedans avant
void set_var(Var var, NeObj object) {
    global_env->ADRESSES->tab[var] = object;
}

// remplace le contenu d'une variable
void replace_var(Var var, NeObj object) {
    neobject_destroy(global_env->ADRESSES->tab[var]);
    global_env->ADRESSES->tab[var] = object;
}

void free_var(Var var) {
    neobject_destroy(global_env->ADRESSES->tab[var]);
}


//////////////////// MANIPULATION D'OBJETS SECONDAIRES ////////////////////


NeObj neobject_create(uint8_t type)
{
    NeObj neo = (NeObj) {.type = type};
    return neo;
}



void var_reset(NeObj* var) {
    *var = neo_empty_create();
    return;
}



////////////////////////// TYPE EMPTY /////////////////////////

NeObj neo_empty_create(void) {
    return neobject_create(TYPE_EMPTY);
}

bool neo_is_void(NeObj neo) {
    return NEO_TYPE(neo) == TYPE_EMPTY;
}

bool neo_exact_equal(NeObj a, NeObj b) {
    return memcmp(&a, &b, sizeof(NeObj)) == 0;
}

//////////////////////// TYPE_CONTAINER ///////////////////



Container* container_create(int type, NeList* data) {
    Container* c = neon_malloc(sizeof(Container));
    c->type = type;
    c->data = data;
    c->myCopy = NULL;
    c->refc = 1;
    c->next = NEO_VOID;
    c->prev = NEO_VOID;
    return c;
}


NeObj gc_extern_neo_container_create(int type, NeList* data)
{
    NeObj neo = neobject_create(TYPE_CONTAINER);
    neo.container = container_create(type, data);
    return neo;
}



NeObj neo_container_create(int type, NeList* data)
{
    return neo_container_convert(container_create(type, data));
}


NeObj gc_extern_neo_container_convert(Container* c) {
    NeObj neo = neobject_create(TYPE_CONTAINER);
    neo.container = c;
    return neo;
}

NeObj neo_container_convert(Container* c) {
    NeObj neo = neobject_create(TYPE_CONTAINER);
    neo.container = c;

    gc_add_deep_object(neo);
    return neo;
}

Container* neo_to_container(NeObj neo) {
    return neo.container;
}


// renvoie l'indice dans le container du champ du nom name
int get_field_index(Container* c, char* name) {
    NeList* list = neo_to_list(global_env->ATTRIBUTES->tab[c->type]);
            
    int index = 0;
    for (; index < list->len && strcmp(neo_to_string(list->tab[index]), name) != 0 ; index++);

    if (index == list->len - 1 && strcmp(neo_to_string(list->tab[index]), name) != 0)
    {
        global_env->CODE_ERROR = 82;
        return -1;
    }
    return index;
}


NeObj* get_container_field_addr(Container* c, int index) {
    return nelist_nth_addr(c->data, index);
}

NeObj get_container_field(Container* c, int index) {
    return nelist_nth(c->data, index);
}

void container_destroy(Container* c) {
    nelist_destroy(c->data);
    neon_free(c);
}

void neo_container_aff(NeObj neo) {
    Container* c = neo_to_container(neo);
    // on regarde si l'affichage est surchargé
    char* container_name = global_env->CONTAINERS->tab[c->type];
    int index = function_module(container_name, "repr");
    
    if (index >= 0) { // on exécute la fonction surchargée
        unmark(neo); // on va passer l'objet en argument à une fonction utilisateur, donc :
        // 1- C'est l'utilisateur qui gère les boucles infinies dans son objet
        // 2- Sinon ça va faire n'importe quoi dans les copies. Les marquages doivent rester invisibles à l'exécution
        
        NeObj neo_fun = global_env->ADRESSES->tab[index];
        UserFunc* fun = neo_fun.userfunc;
        bool isMethod = NEO_TYPE(neo_fun) == TYPE_USERMETHOD;
        
        NeList args = (NeList) {
            .tab = (NeObj[]) {neo},
            .len = 1,
            .capacity = 0
        };
        NeObj ret = callUserFunc(fun, &args, NEO_VOID);
        
        neobject_destroy(ret);
        if (global_env->CODE_ERROR != 0) {
            return ;
        }
    }
    else {
        // on n'a pas trouvé de surcharge sur l'affichage

        NeList* list = global_env->ATTRIBUTES->tab[c->type].nelist;

        printString(global_env->CONTAINERS->tab[c->type]);
        printString("(");
        for (int i=0 ; i < c->data->len ; i++)
        {
            printString(neo_to_string(list->tab[i]));
            printString(": ");
            neobject_aff(get_container_field(c, i));
            if (i < c->data->len - 1)
                printString(", ");
        }
        printString(")");
    }
}


char* neo_container_str(NeObj neo) {
    Container* c = neo_to_container(neo);

    // on regarde si str est surchargé
    char* container_name = global_env->CONTAINERS->tab[c->type];
    int index = function_module(container_name, "str");
    
    if (index >= 0) { // on exécute la fonction surchargée
        unmark(neo); // sinon ça va faire caca, et de toute façon c'est l'utilisateur qui gère les boucles

        NeObj neo_fun = global_env->ADRESSES->tab[index];
        UserFunc* fun = neo_fun.userfunc;
        bool isMethod = NEO_TYPE(neo_fun) == TYPE_USERMETHOD;

        NeList args = (NeList) {
            .tab = (NeObj[]) {neo},
            .len = 1,
            .capacity = 0
        };
        NeObj obj = callUserFunc(fun, &args, NEO_VOID);
        
        if (global_env->CODE_ERROR != 0) {
            neobject_destroy(obj);
            return NULL;
        }
        char* ret = strdup(neo_to_string(obj));
        neobject_destroy(obj);
        return ret;
    }
    else {
        // on n'a pas trouvé de surcharge sur str

        NeList* list = global_env->ATTRIBUTES->tab[c->type].nelist;

        char* str1 = strdup(global_env->CONTAINERS->tab[c->type]);
        str1 = addStr2(str1, "(");
        for (int i=0 ; i < c->data->len ; i++)
        {
            str1 = addStr2(str1, neo_to_string(list->tab[i]));
            str1 = addStr2(str1, ": ");

            char* s = neobject_str(get_container_field(c, i));
            str1 = addStr2(str1, s);
            neon_free(s);

            if (i < c->data->len - 1)
                str1 = addStr2(str1, ", ");
        }

        return addStr2(str1, ")");
    }
}


/////////////////// TYPE_BOOL /////////////////////

NeObj neo_bool_create(bool neon_boolean)
{
    NeObj neo = neobject_create(TYPE_BOOL);
    neo.integer = neon_boolean;

    return neo;
}


bool neo_is_true(NeObj neo)
{
    return neo.integer;
}


bool neo_to_bool(NeObj neo)
{
  return neo.integer;
}

void neo_bool_aff(NeObj neo) {
    if (neo_is_true(neo))
        printString("True");
    else
        printString("False");
}

char* neo_bool_str(NeObj neo) {
    if (neo_is_true(neo))
        return strdup("True");
    else
        return strdup("False");
}


///////////////// TYPE_NUMBER ////////////////////

NeObj neo_double_create(double number) {
    NeObj neo = neobject_create(TYPE_DOUBLE);
    neo.floating = number;

    return neo;
}


NeObj neo_integer_create(intptr_t number) {
    NeObj neo = neobject_create(TYPE_INTEGER);
    neo.integer = number;

    return neo;
}

intptr_t randint(intptr_t min, intptr_t max) {

    if (min >= max)
    {
        global_env->CODE_ERROR = 64;// la fonction randint prend en argument un entier positif
        return 0;
    }

    intptr_t range = max - min;

    int result = rand()%range + min;

    return result;
}


intptr_t neo_to_integer(NeObj neo)
{
    if (NEO_TYPE(neo) == TYPE_DOUBLE)
        return (int)neo.floating;

    return neo.integer;
}

double neo_to_double(NeObj neo)
{
    if (NEO_TYPE(neo) == TYPE_INTEGER)
        return (double)neo.integer;
    
    return neo.floating;
}




///////////////// TYPE_FUNCTION //////////////////

NeObj neo_fun_create(NeObj (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour)
{
    NeObj neo = neobject_create(TYPE_FONCTION);
    neo.function = function_create(ptr, help, nbArgs, typeArgs, typeRetour);

    return neo;
}

void function_destroy(Function* fun) {
    neon_free(fun->typeArgs);
    neon_free(fun);
}


Function* function_create(NeObj (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour)
{
    Function* fun = neon_malloc(sizeof(Function));
    fun->ptr = ptr;
    fun->nbArgs = nbArgs;
    fun->help = help;
    fun->typeRetour = typeRetour;
    fun->refc = 1;

    // copie de typeArgs dans le tas
    if (nbArgs == -1)
    {
        fun->typeArgs = neon_malloc(sizeof(int));
        fun->typeArgs[0] = typeArgs[0];
    }
    else
    {
        fun->typeArgs = neon_malloc(sizeof(int)*nbArgs);
        for (int i = 0 ; i < nbArgs ; i++)
            fun->typeArgs[i] = typeArgs[i];
    }
    
    return fun;
}


NeObj functionCall(NeObj fun, NeList* args)
{
    Function* f = fun.function;
    if (f->nbArgs != -1 && f->nbArgs != args->len)
    {
        global_env->CODE_ERROR = 36;// nombre d'arguments invalide pour appeler cette fonction
        return NEO_VOID;
    }
    NeObj (*ptr) (NeList*) = f->ptr;
    NeObj ret = ptr(args);
    return ret;
}







bool funcArgsCheck(Function* fun, NeList* args)
{

    if (fun->nbArgs == -1)
    {
        for (int i=0 ; i < args->len ; i++)
        {
            if (fun->typeArgs[0] != TYPE_UNSPECIFIED && NEO_TYPE(args->tab[i]) != fun->typeArgs[i])
            {
                return false;
            }
        }
    }
    else
    {
        if (fun->nbArgs != args->len)
            return false;
        
        for (int i=0 ; i < args->len ; i++)
        {
            if (fun->typeArgs[i] != TYPE_UNSPECIFIED && NEO_TYPE(args->tab[i]) != fun->typeArgs[i])
            {
                return false;
            }
        }
    }

    return true;
}



Function* neo_to_function(NeObj neo) {
    return neo.function;
}


////////////////////// TYPE_USERFUNC ///////////////////////

UserFunc* neo_to_userfunc(NeObj neo) {
    return neo.userfunc;
}


NeObj userFuncCreate(Var* args, NeTree code, int nbArgs, bool unlimited_arguments, int nbOptArgs, NeList* opt_args, uint8_t type)
{
    UserFunc* fun = neon_malloc(sizeof(UserFunc));
    fun->args = args;
    fun->code = code;
    fun->nbArgs = nbArgs;
    fun->opt_args = opt_args;
    fun->nbOptArgs = nbOptArgs;
    fun -> unlimited_arguments = unlimited_arguments;
    fun->doc = NULL;
    fun->refc = 1;

    NeObj neo = neobject_create(type);
    neo.userfunc = fun;    
    return neo;
}



// cette fonction prend en entrée une userFunc pas encore définie (donc opt_args = NULL), et renvoie une nouvelle fonction
// qui correspond à la userfunc définie
NeObj userFuncDefine(NeObj obj, NeList* opt_args) {
    UserFunc* fun = neo_to_userfunc(obj);
    Var* args = neon_malloc(sizeof(Var) * fun->nbArgs);

    // copie des global_env->NOMS de variable des arguments
    for (int i = 0 ; i < fun->nbArgs ; i++)
        args[i] = fun->args[i];

    return userFuncCreate(args, fun->code, fun->nbArgs, fun->unlimited_arguments, fun->nbOptArgs, opt_args, NEO_TYPE(obj));
}


void userfunc_destroy(UserFunc* fun) {
    if (fun->doc != NULL)
        neon_free(fun->doc);
    neon_free(fun->args);
    if (fun->opt_args != NULL)
        nelist_destroy(fun->opt_args);
    neon_free(fun);
}


//////////////////// TYPE_PROMISE /////////////////




NeObj neo_promise_create(int id) {
    // une promesse contient un identifiant correspondant au processus dont elle dépend
    NeObj neo = neobject_create(TYPE_PROMISE);
    neo.refc_ptr = global_env->PROMISES_CNT.tab[id];

    return neo;
}

int get_promise_id(NeObj promise) {
    int id = 0;
    while (global_env->PROMISES_CNT.tab[id] != promise.refc_ptr) id++;
    return id;
}


void neo_promise_aff(NeObj promise) {
    printString("<promise from process ");
    printInt(get_promise_id(promise));
    printString(">");
}

char* neo_promise_str(NeObj neo) {
    char* s1 = strdup("<promise from process ");
    char* s2 = int_to_str(neo_to_integer(neo));
    s1 = addStr2(s1, s2);
    neon_free(s2);
    s2 = strdup(">");
    char* ret = addStr2(s1, s2);
    neon_free(s2);
    return ret;
}


///////////////////// TYPE_STRING ////////////////////




NeObj neo_str_create(char* string) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObj neo = neobject_create(TYPE_STRING);
    neo.string = neon_malloc(sizeof(String));
    neo.string->refc = 1;
    neo.string->string = string;
    return neo;
}

char* neo_to_string(NeObj neo)
{
    return neo.string->string;
}


void neo_string_aff(NeObj neo) {
    printString("\"");
    char* traite = traitementStringInverse(neo.string->string);
    printString(traite);
    printString("\"");
    neon_free(traite);
}

char* neo_string_str(NeObj neo) {
    char* traite = traitementStringInverse(neo.string->string);
    char* str1 = addStr("\"",traite);
    char* ret = addStr(str1,"\"");
    neon_free(str1);neon_free(traite);
    return ret;
}

void string_destroy(String* string) {
    neon_free(string->string);
    neon_free(string);
}

///////////////////// TYPE_EXCEPTION ////////////////


NeObj neo_exception_create(int index)
{
    NeObj neo = neobject_create(TYPE_EXCEPTION);
    // détermine la place de la chaine dans le tableau exception
    neo.integer = index;

    return neo;
}

int get_exception_code(NeObj exception) {
    return exception.integer;
}

void neo_exception_aff(NeObj neo) {
    printString(global_env->EXCEPTIONS->tab[get_exception_code(neo)]);
}

///////////////////// TYPE_CONST //////////////////
// je crois que ce type sert à rien, mais il est là quand même au cas où


NeObj neo_const_create(char* string) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObj neo = neobject_create(TYPE_CONST);
    neo.string = neon_malloc(sizeof(String));
    neo.string->refc = 1;
    neo.string->string = string;
    return neo;
}





char* neo_to_const(NeObj neo)
{
    return neo.string->string;
}



//////////////////// TYPE_NONE /////////////////


NeObj neo_none_create(void) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObj neo = neobject_create(TYPE_NONE);
    return neo;
}



///////////////////// TYPE_LIST /////////////////


bool nelist_equal(NeList* l1, NeList* l2)
{
    bool bo = l1->len==l2->len;
    for (int i=0 ; bo && i < l1->len ; i++)
        bo = bo && neo_equal(nelist_nth(l1, i), nelist_nth(l2, i));
    
    return bo;
}


void nelist_aff(NeList* liste)
{
    if (liste->len == 0)
    {
        printString("[]");
    }
    else
    {
        printString("[");
        for (int i=0 ; i < liste->len-1 ; i++)
        {
            neobject_aff(nelist_nth(liste, i));
            printString(", ");
        }
        neobject_aff(nelist_nth(liste, liste->len-1));
        printString("]");
    }
}



char* nelist_str(NeList* list)
{
    if (list->len == 0)
    {
        return strdup("[]");
    }
    else
    {
        char* str1 = strdup("["), * str2, *temp;
        for (int i=0 ; i < list->len - 1 ; i++)
        {
            temp = neobject_str(nelist_nth(list, i));
            str2 = addStr(str1,temp);
            neon_free(temp);neon_free(str1);
            str1 = addStr(str2,", ");
            neon_free(str2);
        }
        temp = neobject_str(nelist_nth(list, list->len-1));
        str2 = addStr(str1,temp);
        neon_free(str1);neon_free(temp);
        str1 = addStr(str2, "]");
        neon_free(str2);
        return str1;
    }
    return strdup("Erreur17");
}






void nelist_destroy(NeList* list)
{
    for (int i=0;i<list->len;i++)
    {
        //if (list == global_env->ADRESSES)
        //    printf("Suppression de %s\n", global_env->NOMS->tab[i]);
        neobject_destroy(list->tab[i]);
    }
    neon_free(list->tab);
    neon_free(list);
}



void nelist_destroy_until(NeList *list, int index_max) {
    for (int i=0 ; i <= index_max ; i++)
    {
        neobject_destroy(list->tab[i]);
    }
    neon_free(list->tab);
    neon_free(list);
}


NeList* nelist_reverse(NeList* list) {
    NeList* reversed = nelist_create(list->len);
    for (int i = 0 ; i < list->len ; i++) {
        reversed->tab[i] = neo_copy(list->tab[list->len - i - 1]);
    }
    return reversed;
}





bool nelist_inList2(NeList* list, NeObj neo)
{
    bool bo = false;
    
    for (int i = 0 ; i < list->len ; i++)
    {
        if (neo_equal(list->tab[i], neo))
        {
            bo = true;
            break;
        }
    }

    return bo;
}



bool nelist_inList(NeList* list, NeObj neo)
{
    bool bo = false;
    
    for (int i = 0 ; i < list->len ; i++)
    {
        if (neo_exact_equal(list->tab[i], neo))
        {
            bo = true;
            break;
        }
    }

    return bo;
}


NeList* neo_to_list(NeObj neo) {
    return neo.nelist;
}


NeList* nelist_dup(NeList* l)
{
    //copie de deux listes, partie récursive de la fonction
    
    NeList* liste = nelist_create(l->len); //nouvelle liste
    for (int i=0 ; i < l->len ; i++)
    {
        liste->tab[i] = neo_dup(l->tab[i]); // on ajoute à la liste liste une copie de l'élément i de la liste neo
    }
    return liste;
}


NeList* nelist_create(int len)
{
    NeList* list = neon_malloc(sizeof(NeList));//crée la structure
    list->myCopy = NULL;
    list->refc = 1;
    list->len = len; // initialise la bonne longueur
    list->capacity = 0;
    list->next = NEO_VOID;
    list->prev = NEO_VOID;
    
    while (pow(2, list->capacity) < list->len)
        list->capacity++;
  
    list->tab = neon_malloc(pow(2, list->capacity)*sizeof(NeObj));//initialise le tableau de longueur len avec de zéros
  
    return list;//retourne la structure
}



void nelist_append(NeList* list, NeObj ptr)//ajoute un élément à la fin de la liste
{
    NeObj* tmp;

    if (pow(2, list->capacity)==list->len)
    {
        list->capacity++;
        tmp = neon_realloc(list->tab, pow(2, list->capacity)*sizeof(NeObj));//réallocation de list.tab
        list->tab = tmp;//affectation du pointeur de tmp vers list.tab
    }

    list->tab[list->len]=ptr;//affecte ptr au dernier élément
    list->len++;//incrémente la longueur
}


NeObj* nelist_nth_addr(NeList* list, int index) {
    update_if_promise(&list->tab[index]);
    return &list->tab[index];
}

NeObj nelist_nth(NeList* list, int index) {
    update_if_promise(&list->tab[index]);
    return list->tab[index];
}


void nelist_insert(NeList* list,NeObj neo, int index)//ajoute un élément à la place indiquée
{
    if (index > list->len) {
        global_env->CODE_ERROR = 37; // out of range
        return ;
    }
  
    NeObj* tmp;
  
    if (pow(2, list->capacity)==list->len) {
        list->capacity++;
        tmp = neon_realloc(list->tab, pow(2, list->capacity)*sizeof(NeObj)); // réallocation de list.tab
        list->tab = tmp; // affectation du pointeur de tmp vers list.tab
    }
  
    for (int i = list->len ; i > index; i--)//décale tous les éléments à partir de celui à supprimer
        list->tab[i]=list->tab[i-1];
    
    
    list->tab[index]=neo;
    list->len++;//incrémente la longueur
}






void nelist_remove(NeList* list,int index)
{
    if (index >= list->len)
    {
        global_env->CODE_ERROR = 38;//out of range
        return ;
    }
  
    neobject_destroy(list->tab[index]);
  
    for (int i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
        list->tab[i]=list->tab[i+1];
    
    NeObj* tmp;
  
    if (pow(2, list->capacity-1)==list->len-1)
    {
        list->capacity--;
        tmp = neon_realloc(list->tab, pow(2, list->capacity)*sizeof(NeObj));//réalloue un nouveau pointeur de la bonne taille
        list->tab = tmp;
    }
  
  
    list->len--;
}




int nelist_index(NeList* liste, NeObj neo)
{
    
    for (int i=0 ; i < liste->len ; i++)
    {
        if (neo_exact_equal(liste->tab[i], neo))
        {
            return i;
        }
    }
    global_env->CODE_ERROR = 39; // cet objet n'existe pas
    
    return -1;
}




int nelist_index2(NeList* l, NeObj neo)
{
    for (int i = 0; i < l->len ; i++)
    {
        if (neo_equal(neo, l->tab[i]))
            return i;
    }
    global_env->CODE_ERROR = 88;
    return -1;
}



// listes dans les neobjects

/*
Cette fonction doit absolument recevoir une liste valide, car on va directement copier l'objet pour l'ajouter
au garbage collector
*/
NeObj neo_list_convert(NeList* list)
{
    NeObj neo = neobject_create(TYPE_LIST);
    neo.nelist = list;

    // pour le garbage collector
    gc_add_deep_object(neo);

    return neo;
}

// cette fonction fabrique un NeObj à partir d'une NeList*, mais ne l'inclus pas dans la système de Garbage Collection
// ca permet de fabriquer des objets qui ne sont pas accessibles depuis global_env->ADRESSES et qui ne disparaissent pas lorsque l'on
// appelle le garbage collector
NeObj gc_extern_neo_list_convert(NeList* list) {
    NeObj neo = neobject_create(TYPE_LIST);
    neo.nelist = list;

    return neo;
}



NeObj neo_list_create(int len)
{
    return neo_list_convert(nelist_create(len));
}



// supprime totalement une liste sans libérer ses éléments, de manière respectueuse du compteur de références
void neo_list_neon_free(NeObj list) {
    if (*list.refc_ptr == 1) { // il ne restait plus qu'un seul pointeur sur cette NeList, donc on peut bel et bien la supprimer
        NeList* nelist = neo_to_list(list);
        neon_free(nelist->tab);
        neon_free(nelist);
    }
    // si des gens pointent encore vers la liste, il restera encore le refc et la NeList, donc c'est ok
}



void neo_list_append(NeObj neo, NeObj ptr)
{
    nelist_append(neo.nelist, ptr);
    return;
}




void neo_list_remove(NeObj neo, int index)
{
    nelist_remove(neo.nelist, index);
    return;
}



void neo_list_insert(NeObj neo, NeObj ptr,int index)
{
    nelist_insert(neo.nelist, ptr, index);
    return;
}



NeObj neo_list_nth(NeObj neo, int index)
{
    return nelist_nth(neo.nelist, index);
}


int neo_list_len(NeObj neo)
{
    return neo.nelist->len;
}







NeObj neo_copy(NeObj neo) {
    // il y a le cas des TYPE_EMPTY et des TYPE_NONE que l'on peut copier mais on a aucun refc à incrémenter
    // en fin de compte, un refc ça sert à compter les références du ptr au dessus donc s'il n'y en a pas il sert à rien
    
    if (neo.type & HEAP_ALLOCATED)
        *neo.refc_ptr += 1; // pour les promesses, cette opération va incrémenter le bon indice de global_env->PROMISES_CNT

    return neo;
}


/*
Toutes ces fonctions de manipulation de NeObjects entiers doivent faire attention à ne pas boucler infiniment
et de manière générale à gérer correctement les objets qui pointent sur eux-même
Pour un titre purement informatif, on a mis en place un système de marquage consistant à changer la valeur de refc en la
mettant en négatif. Ca permet de la conserver tout en ajoutant une information binaire.
Pour neo_dup, il faut ajouter plus qu'une information binaire puisqu'au cas où l'objet a déjà été
copié il faut être capable de retrouver le pointeur de la copie de l'objet pour le renvoyer.


*/

// ces deux fonctions sont utiles pour l'algorithme de deepcopy. Quand il commence la copie d'un NeObject qui est une liste
// ou un container, il stocke dans l'objet deux informations : le fait qu'il a déjà été copié, et le pointeur vers sa copie.
// comme ça dans les appels récursifs de la deepcopy, quand on rencontre un objet qui a déjà été copié, on peut directement
// mettre le pointeur de sa version copiée

void mark_as_already_copied(NeObj neo, void* copy) {
    if (NEO_TYPE(neo) == TYPE_LIST) {
        NeList* original = neo_to_list(neo);
        original->myCopy = copy;
    }
    else if (NEO_TYPE(neo) == TYPE_CONTAINER) {
        Container* original = neo_to_container(neo);
        original->myCopy = copy;
    }
    // les autres types d'objets ne sont pas sensibles aux dépendances de pointeurs
}

NeObj get_previous_copy(NeObj neo) {
    if (NEO_TYPE(neo) == TYPE_LIST) {
        NeList* list = neo_to_list(neo);
        if (list->myCopy != UNIT && list->myCopy != NULL)
            return gc_extern_neo_list_convert(list->myCopy); // la liste list->myCopy est déjà dans le GC depuis la première fois qu'elle a été copiée, la seule chose qu'on a à faire c'est l'envelopper dans un NeObject
        else
            return NEO_VOID;
    }
    else if (NEO_TYPE(neo) == TYPE_CONTAINER) {
        Container* container = neo_to_container(neo);
        if (container->myCopy != UNIT && container->myCopy != NULL)
            return gc_extern_neo_container_convert(container->myCopy);
        else
            return NEO_VOID;
    }
    return NEO_VOID;
    // les autres types d'objets ne sont pas sensibles aux dépendances de pointeurs
}


NeObj neo_deepcopy(NeObj neo) {
    NeObj copy = neo_dup(neo);
    // tous les global_env->CONTAINERS et toutes les listes ont été marqué.es, il faut donc les démarquer
    recursive_unmark(neo);
    return copy;
}




/*
Cette fonction prend en argument un pointeur sur une promesse, et la remplace par la valeur de renvoi du processus correspondant
si celui-ci a terminé
Cette fonction peut être appelées sur n'importe quel objet, mais n'aura d'effet que sur une promesse
Cette fonction ne doit pas être utilisée directement en dehors de varlib. Elle est interne à varlib
*/
void update_if_promise(NeObj* promise) {
    if (NEO_TYPE((*promise)) == TYPE_PROMISE)
    {
        int id = get_promise_id(*promise);

        if (!neo_is_void(global_env->PROMISES->tab[id])) // la promesse a été résolue
        {
            // il n'y a pas besoin de spécialement soustraire un au compteur car comme on détruit neo, neobject_destroy le fait tout seul

            if (*global_env->PROMISES_CNT.tab[id] == 1) // cette promesse était la dernière promesse non résolue
            {
                *global_env->PROMISES_CNT.tab[id] += 1; // si on laisse le compteur à 1, global_env->PROMISES.tab[id] va être supprimé par le neobject_destroy

                neobject_destroy(*promise); // on supprime la promesse en elle même
                // on la remplace par la valeur de retour (sans faire de copie puisque c'est la dernière)
                *promise = global_env->PROMISES->tab[id];

                global_env->PROMISES->tab[id] = NEO_VOID;
                *global_env->PROMISES_CNT.tab[id] = 0;
            }
            else
            {
                _affect2(promise, global_env->PROMISES->tab[id]);
                *global_env->PROMISES_CNT.tab[id] -= 1;
            }

            update_if_promise(promise);
        }
        return;
    }
    return;
}






NeObj neo_dup(NeObj neo) {

    // si l'objet que l'on veut copier a déjà été copié, on renvoie directement sa copie
    // quand l'objet a été créé pour la première fois, son refc a été initialisé à 0
    // puis à chaque fois que l'on récupère sa copie ici, il faut incrémenter son refc puisqu'un nouvel objet pointe désormais sur lui
    NeObj copy = get_previous_copy(neo);

    if (!neo_is_void(copy)) {
        return neo_copy(copy); // il faut quand même incrémenter le compteur de références
    }

    // sinon, on le copie en bonne et due forme

    // pour les listes et les global_env->CONTAINERS, une copie profonde a un réel impact puisque ces objets contiennent d'autres objets
    // en revanche pour tous les autres objets ça ne change strictement rien pour l'utilisateur d'avoir une réelle copie ou non
    
    if (NEO_TYPE(neo) == TYPE_LIST) {
        // on ne peut pas ajouter la liste maintenant au garbage collector puisque son pointeur est pour le moment invalide
        NeList* original_list = neo_to_list(neo);
        NeList* copied_list = nelist_create(original_list->len);
        
        mark_as_already_copied(neo, copied_list); // inscrit le nouveau pointeur de la copie directement dans l'objet

        for (int i = 0 ; i < original_list->len ; i++)
            copied_list->tab[i] = neo_dup(nelist_nth(original_list, i));

        return neo_list_convert(copied_list); // et c'est le moment où on l'ajoute au GC
    }

    else if (NEO_TYPE(neo) == TYPE_CONTAINER) {
        Container* original_cntr = neo_to_container(neo);
        Container* copied_cntr = container_create(original_cntr->type, nelist_create(original_cntr->data->len));
        
        mark_as_already_copied(neo, copied_cntr); // inscrit le nouveau pointeur de la copie directement dans l'objet

        for (int i = 0 ; i < original_cntr->data->len ; i++)
            copied_cntr->data->tab[i] = neo_dup(nelist_nth(original_cntr->data, i));

        return neo_container_convert(copied_cntr); // et c'est le moment où on l'ajoute au GC
    }

    else
        return neo_copy(neo);
    
    return NEO_VOID;
}





void neobject_destroy(NeObj neo)
{
    if (neo.type & HEAP_ALLOCATED)
    {
        *neo.refc_ptr -= 1;

        if (*neo.refc_ptr == 0) // on supprime neo->data
        {
            if (ismarked(neo)) // si l'objet est marqué, c'est qu'on est un des appels récursifs de sa suppression, donc il sera supprimé
                return;
            
            mark(neo);

            if (NEO_TYPE(neo) == TYPE_STRING) {
                string_destroy(neo.string);
            }

            else if (NEO_TYPE(neo) == TYPE_LIST) {
                gc_remove_nelist(neo.nelist);
                nelist_destroy(neo.nelist);
            }
            else if (NEO_TYPE(neo) == TYPE_FONCTION) {
                function_destroy(neo.function);
            }

            else if (NEO_TYPE(neo) == TYPE_USERFUNC || NEO_TYPE(neo) == TYPE_USERMETHOD) {
                userfunc_destroy(neo.userfunc);
            }

            else if (NEO_TYPE(neo) == TYPE_CONTAINER) {
                gc_remove_container(neo.container);
                container_destroy(neo.container);
            }
            else if (NEO_TYPE(neo) == TYPE_PROMISE)
            {
                int id = get_promise_id(neo);
                neobject_destroy(global_env->PROMISES->tab[(int)id]);
                global_env->PROMISES->tab[(int)id] = NEO_VOID;
            }

        }
    }
    
}





void neobject_aff(NeObj neo)
{
    // on doit à tout prix éviter une boucle infinie due aux objets auto-référençant
    // pour ça avant d'afficher un objet on le marque, on le démarque après
    // si au moment d'afficher un objet il est marqué, alors on ne l'affiche pas

    if (ismarked(neo)) { // c'est le deuxième fois qu'on nous demande d'afficher cet objet
        setColor(RED);
        printString("*");
        setColor(WHITE);
    }
    else {
        mark(neo);
        
        if (NEO_TYPE(neo) == TYPE_DOUBLE) {
            printDouble(neo_to_double(neo));
        }
        else if (NEO_TYPE(neo) == TYPE_INTEGER) {
            printInt(neo_to_integer(neo));
        }
        else if (NEO_TYPE(neo) == TYPE_BOOL) {
            neo_bool_aff(neo);
        }
        else if (NEO_TYPE(neo) == TYPE_STRING) {
            neo_string_aff(neo);
        }
        else if (NEO_TYPE(neo) == TYPE_LIST) {
            nelist_aff(neo_to_list(neo));
        }
        else if (NEO_TYPE(neo) == TYPE_CONST) {
            printString(neo_to_string(neo));
        }
        else if (NEO_TYPE(neo) == TYPE_EXCEPTION) {
            neo_exception_aff(neo);
        }
        else if (NEO_TYPE(neo) == TYPE_NONE) {
            printString("None");
        }
        else if (NEO_TYPE(neo) == TYPE_FONCTION) {
            printString("<built-in function>");
        }

        else if (NEO_TYPE(neo) == TYPE_USERFUNC) {
            printString("<function>");
        }

        else if (NEO_TYPE(neo) == TYPE_USERMETHOD) {
            printString("<method>");
        }
        else if (NEO_TYPE(neo) == TYPE_CONTAINER) {
            neo_container_aff(neo);
        }
        else if (NEO_TYPE(neo) == TYPE_PROMISE) {
            neo_promise_aff(neo);
        }

        unmark(neo);
    }

}



char* neobject_str(NeObj neo)
{
    // on doit à tout prix éviter une boucle infinie due aux objets auto-référençant
    // pour ça avant d'afficher un objet on le marque, on le démarque après
    // si au moment d'afficher un objet il est marqué, alors on ne l'affiche pas

    if (ismarked(neo)) { // c'est le deuxième fois qu'on nous demande d'afficher cet objet
        return strdup("*");
    }
    else {
        mark(neo);

        char* ret = NULL; // la chaîne finale que l'on va renvoyer

        if (NEO_TYPE(neo) == TYPE_INTEGER)
        {
            ret = int_to_str(neo_to_integer(neo));
        }
        else if (NEO_TYPE(neo) == TYPE_DOUBLE)
        {
            ret = double_to_str(neo_to_double(neo));
        }
        else if (NEO_TYPE(neo) == TYPE_BOOL)
        {
            ret = neo_bool_str(neo);
        }
        else if (NEO_TYPE(neo) == TYPE_STRING)
        {
            ret = neo_string_str(neo);
        }
        else if (NEO_TYPE(neo) == TYPE_LIST)
        {
            ret = nelist_str(neo.nelist);
        }
        else if (NEO_TYPE(neo) == TYPE_CONST)
        {
            ret = strdup(neo_to_string(neo));
        }
        else if (NEO_TYPE(neo) == TYPE_EXCEPTION)
        {
            ret = strdup(global_env->EXCEPTIONS->tab[get_exception_code(neo)]);
        }
        else if (NEO_TYPE(neo) == TYPE_NONE)
        {
            ret = strdup("None");
        }
        else if (NEO_TYPE(neo) == TYPE_FONCTION)
        {        
            ret = strdup("<built-in function>");
        }

        else if (NEO_TYPE(neo) == TYPE_USERFUNC)
        {
            ret = strdup("<function>");
        }

        else if (NEO_TYPE(neo) == TYPE_USERMETHOD)
        {
            ret = strdup("<method>");
        }

        else if (NEO_TYPE(neo) == TYPE_CONTAINER)
        {
            ret = neo_container_str(neo);
        }
        else if (NEO_TYPE(neo) == TYPE_PROMISE) {
            ret = neo_promise_str(neo);
        }
        unmark(neo);

        return ret;
    }

    return NULL;
}




char* type(NeObj neo)
{
    if (NEO_TYPE(neo) == TYPE_BOOL)
        return "Bool";

    if (NEO_TYPE(neo) == TYPE_STRING)
        return "String";

    if (NEO_TYPE(neo) == TYPE_INTEGER)
        return "Integer";

    if (NEO_TYPE(neo) == TYPE_DOUBLE)
        return "Real";

    if (NEO_TYPE(neo) == TYPE_FONCTION)
        return "Built-in function";

    if (NEO_TYPE(neo) == TYPE_LIST)
        return "List";

    if (NEO_TYPE(neo) == TYPE_USERFUNC)
        return "Function";

    if (NEO_TYPE(neo) == TYPE_USERMETHOD)
        return "Method";

    if (NEO_TYPE(neo) == TYPE_EXCEPTION)
        return "Exception";
    
    if (NEO_TYPE(neo) == TYPE_CONTAINER)
    {
        Container* c = neo_to_container(neo);
        return global_env->CONTAINERS->tab[c->type];
    }
    if (NEO_TYPE(neo) == TYPE_PROMISE)
    {
        return "Promise";
    }
    if (NEO_TYPE(neo) == TYPE_EMPTY)
    {
        return "Undefined";
    }

    if (NEO_TYPE(neo) == TYPE_UNSPECIFIED)
        return "unspecified type";

    return "NoneType";

}



bool neo_equal(NeObj _op1, NeObj _op2)
{

    if (ismarked(_op1) && ismarked(_op2))
        return true;

    // si l'un des objets est un NeObject, on regarde si == est surchargé
    if (NEO_TYPE(_op1) == TYPE_CONTAINER || NEO_TYPE(_op2) == TYPE_CONTAINER) {
        Container* c = (NEO_TYPE(_op1) == TYPE_CONTAINER) ? neo_to_container(_op1) : neo_to_container(_op2);
        char* container_name = global_env->CONTAINERS->tab[c->type];
        int index = function_module(container_name, "equal");
        if (index >= 0) {
            NeObj neo_fun = global_env->ADRESSES->tab[index];
            UserFunc* fun = neo_fun.userfunc;
            bool isMethod = NEO_TYPE(neo_fun) == TYPE_USERMETHOD;

            NeList args = (NeList) {
                .tab = (NeObj[]) {_op1, _op2},
                .len = 2,
                .capacity = 1
            };
            NeObj ret = callUserFunc(fun, &args, NEO_VOID);
            if (global_env->CODE_ERROR != 0) {
                return false;
            }

            bool bo = neoIsTrue(ret);
            neobject_destroy(ret);
            return bo;
        }
    }


    if ((NEO_TYPE(_op1)==TYPE_INTEGER || NEO_TYPE(_op1)==TYPE_DOUBLE) && (NEO_TYPE(_op2)==TYPE_INTEGER || NEO_TYPE(_op2)==TYPE_DOUBLE))
    {
        double op1 = (NEO_TYPE(_op1) == TYPE_INTEGER) ? (double)neo_to_integer(_op1) : neo_to_double(_op1);
        double op2 = (NEO_TYPE(_op2) == TYPE_INTEGER) ? (double)neo_to_integer(_op2) : neo_to_double(_op2);
        return op1 == op2;
    }

    else if (NEO_TYPE(_op1)==TYPE_STRING)
    {
        if (NEO_TYPE(_op2)==TYPE_STRING)
        {
            return strcmp(neo_to_string(_op1),neo_to_string(_op2)) == 0;
        }
        else
        {
            return false;
        }
    }

    else if (NEO_TYPE(_op1)==TYPE_BOOL)
    {
        if (NEO_TYPE(_op2)==TYPE_BOOL)
        {
            return neo_to_bool(_op1)==neo_to_bool(_op2);
        }
        else
        {
            return false;
        }
    }
  
    else if (NEO_TYPE(_op1)==TYPE_NONE && NEO_TYPE(_op2)==TYPE_NONE)
        return true;
  
    else if (NEO_TYPE(_op1)==TYPE_EXCEPTION)
    {
        if (NEO_TYPE(_op2)==TYPE_EXCEPTION)
        {
            return get_exception_code(_op1) == get_exception_code(_op2);
        }
        else
        {
            return false;
        }
    }

    else if (NEO_TYPE(_op1) == TYPE_FONCTION)
    {
        if (NEO_TYPE(_op2) == TYPE_FONCTION)
        {
            return _op1.function->ptr == _op2.function->ptr;
        }
        else
        {
            return false;
        }
    }

    else if (NEO_TYPE(_op1) == TYPE_USERFUNC || NEO_TYPE(_op1) == TYPE_USERMETHOD)
    {
        if (NEO_TYPE(_op2) == TYPE_USERFUNC || NEO_TYPE(_op2) == TYPE_USERMETHOD)
        {
            return _op1.userfunc->code.pointer == _op2.userfunc->code.pointer && nelist_equal(_op1.userfunc->opt_args, _op2.userfunc->opt_args);
        }
        else
        {
            return false;
        }
    }

    else if (NEO_TYPE(_op1) == TYPE_LIST)
    {
        if (NEO_TYPE(_op2) == TYPE_LIST)
        {
            mark(_op1); mark(_op2); // marque les deux objets comme déjà comparés, on lance l'appel récursif
            bool bo = nelist_equal(neo_to_list(_op1), neo_to_list(_op2));
            unmark(_op1); unmark(_op2);
            return bo;
        }
        else
            return false;
    }

    else if (NEO_TYPE(_op1) == TYPE_CONTAINER)
    {
        if (NEO_TYPE(_op2) == TYPE_CONTAINER)
        {
            mark(_op1); mark(_op2); // marque les deux objets comme déjà comparés, on lance l'appel récursif
            Container* c1 = neo_to_container(_op1);
            Container* c2 = neo_to_container(_op2);
            bool bo = c1->type == c2->type && nelist_equal(c1->data, c2->data);
            unmark(_op1); unmark(_op2);
            return bo;
        }
        else
            return false;
    }
    else if (NEO_TYPE(_op1) == TYPE_PROMISE)
        return NEO_TYPE(_op2) == TYPE_PROMISE && get_promise_id(_op1) == get_promise_id(_op2);

    return false; // dans tous les autres cas
  
}






int neo_compare(NeObj a, NeObj b)
{
    // si l'un des objets est un NeObject, on regarde si == est surchargé
    if (NEO_TYPE(a) == TYPE_CONTAINER || NEO_TYPE(b) == TYPE_CONTAINER) {
        Container* c = (NEO_TYPE(a) == TYPE_CONTAINER) ? neo_to_container(a) : neo_to_container(b);
        char* container_name = global_env->CONTAINERS->tab[c->type];
        int index = function_module(container_name, "comp");
        if (index >= 0) {
            NeObj neo_fun = global_env->ADRESSES->tab[index];
            UserFunc* fun = neo_fun.userfunc;
            bool isMethod = NEO_TYPE(neo_fun) == TYPE_USERMETHOD;

            NeList args = (NeList) {
                .tab = (NeObj[]) {a, b},
                .len = 2,
                .capacity = 1
            };
            NeObj ret = callUserFunc(fun, &args, NEO_VOID);
            if (global_env->CODE_ERROR != 0) {
                neobject_destroy(ret);
                return 0;
            }
            
            int res = neo_to_integer(ret);
            neobject_destroy(ret);
            return res;
        }
    }

    if ((NEO_TYPE(a) == TYPE_INTEGER || NEO_TYPE(a) == TYPE_DOUBLE) && (NEO_TYPE(b) == TYPE_INTEGER || NEO_TYPE(b) == TYPE_DOUBLE))
    {
        double op1 = (NEO_TYPE(a) == TYPE_INTEGER) ? (double)neo_to_integer(a) : neo_to_double(a);
        double op2 = (NEO_TYPE(b) == TYPE_INTEGER) ? (double)neo_to_integer(b) : neo_to_double(b);
        if (op1 < op2)
            return -1;
        else if (op1 > op2)
            return 1;
        else
            return 0;
    }
    
    else if (NEO_TYPE(a) == TYPE_STRING && NEO_TYPE(b) == TYPE_STRING)
    {
        return strcmp(neo_to_string(a), neo_to_string(b));
    }
    else
    {
        //erreur
        global_env->CODE_ERROR = 90;
        return 0;
    }
}


int neo_compare2(NeObj a, NeObj b)
{
    int c = neo_compare(b,a);
    if (global_env->CODE_ERROR != 0)
        return 0;
    else
        return c;
}







// Des fonctions pour marquer temporairement des NeObjects pour savoir si on est déjà passé par eux.


void mark(NeObj neo) {
    if (NEO_TYPE(neo) == TYPE_CONTAINER) {
        Container* c = neo_to_container(neo);
        c->myCopy = UNIT;
    }
    else if (NEO_TYPE(neo) == TYPE_LIST) {
        NeList* l = neo_to_list(neo);
        l->myCopy = UNIT;
    }
}

void unmark(NeObj neo) {
    if (NEO_TYPE(neo) == TYPE_CONTAINER) {
        Container* c = neo_to_container(neo);
        c->myCopy = NULL;
    }
    else if (NEO_TYPE(neo) == TYPE_LIST) {
        NeList* l = neo_to_list(neo);
        l->myCopy = NULL;
    }
}

bool ismarked(NeObj neo) {
    if (NEO_TYPE(neo) == TYPE_CONTAINER) {
        Container* c = neo_to_container(neo);
        return c->myCopy;
    }
    else if (NEO_TYPE(neo) == TYPE_LIST) {
        NeList* l = neo_to_list(neo);
        return l->myCopy;
    }
    return false;
}

void recursive_unmark(NeObj neo) {
    // si l'objet n'est pas marqué, il n'y a forcément rien à démarquer dedans, soit parce que l'on ne l'a pas copié, soit
    // parce que cet algorithme l'a déjà démarqué
    if (ismarked(neo)) {
        unmark(neo);

        NeList* list;
        if (NEO_TYPE(neo) == TYPE_LIST)
            list = neo_to_list(neo);
        else if (NEO_TYPE(neo) == TYPE_CONTAINER)
            list = neo_to_container(neo)->data;
        else
            return;

        // démarque tous les fils
        for (int i=0 ; i < list->len ; i++) {
            recursive_unmark(list->tab[i]);
        }
    }
}






NeObj callOverloadedBinaryOperator(NeObj op1, NeObj op2, char* opname) {
    Container* c = (NEO_TYPE(op1) == TYPE_CONTAINER) ? neo_to_container(op1) : neo_to_container(op2);
    char* container_name = global_env->CONTAINERS->tab[c->type];
    int index = function_module(container_name, opname);
    if (index == -1) {
        global_env->CODE_ERROR = 107;
        return NEO_VOID;
    }
    else {
        NeObj neo_fun = global_env->ADRESSES->tab[index];
        UserFunc* fun = neo_fun.userfunc;
        bool isMethod = NEO_TYPE(neo_fun) == TYPE_USERMETHOD;

        NeList args = (NeList) {
            .tab = (NeObj[]) {op1, op2},
            .len = 2,
            .capacity = 1
        };
        NeObj ret = callUserFunc(fun, &args, NEO_VOID);
        if (global_env->CODE_ERROR != 0) {
            neobject_destroy(ret);
            return NEO_VOID;
        }
        return ret;
    }
}


NeObj callOverloadedUnaryOperator(NeObj op1, char* opname) {
    Container* c = neo_to_container(op1);
    char* container_name = global_env->CONTAINERS->tab[c->type];
    int index = function_module(container_name, opname);
    if (index == -1) {
        global_env->CODE_ERROR = 107;
        return NEO_VOID;
    }
    else {
        NeObj neo_fun = global_env->ADRESSES->tab[index];
        UserFunc* fun = neo_fun.userfunc;
        bool isMethod = NEO_TYPE(neo_fun) == TYPE_USERMETHOD;

        NeList args = (NeList) {
            .tab = (NeObj[]) {op1},
            .len = 1,
            .capacity = 0
        };
        NeObj ret = callUserFunc(fun, &args, NEO_VOID);
        if (global_env->CODE_ERROR != 0) {
            neobject_destroy(ret);
            return NEO_VOID;
        }
        return ret;
    }
}