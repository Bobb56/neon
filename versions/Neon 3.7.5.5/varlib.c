// bibliothèque générale de structures des données Neon

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "headers.c"


extern NeList* PROMISES;
extern intlist PROMISES_CNT;

extern int CODE_ERROR;

extern strlist* CONTAINERS;
extern NeList* ATTRIBUTES;

extern intlist TYPESBUILTINSFONC;
extern strlist NOMSBUILTINSFONC;
extern NeObject* (*BUILTINSFONC[NBBUILTINFONC])(NeList*);
extern NeList* ADRESSES;
extern strlist HELPBUILTINSFONC;
extern strlist* NOMS;
extern intlist NBARGS;

extern strlist exceptions;
extern int LINENUMBER;



/*
Fonctionnement du compteur de références :
Les NeObject sont des structures contenant :
-> Un type
-> Un pointeur sur un NeObjDat (data), qui est une structure qui contient :
--> Un void* (ptr)
--> Un int* (refc)

Un objet dont le data.ptr est NULL doit aussi avoir le data.refc de NULL. C'est pour ça que l'on n'alloue pas refc
dans neobject_create directement
*/



NeObject* neobject_create(char type)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data.ptr = NULL;
    neo->data.refc = NULL;
    neo->type = type;
    
    return neo;
}

NeObject* neo_empty_create(void) {
    return neobject_create(TYPE_EMPTY);
}


void neobject_reset(NeObject* neo) {
    neo->type = TYPE_EMPTY;
    neo->data.ptr = NULL;
    neo->data.refc = NULL;
    return;
}


void neo_alloc_refc(NeObject* neo) {
    neo->data.refc = err_malloc(sizeof(int));
    *neo->data.refc = 0;
}


NeObject* gc_extern_neo_container_create(int type, NeList* data)
{
    Container* c = err_malloc(sizeof(Container));
    c->type = type;
    c->data = data;

    NeObject* neo = neobject_create(TYPE_CONTAINER);
    neo->data.ptr = (void*)c;

    neo_alloc_refc(neo);

    return neo;
}



NeObject* neo_container_create(int type, NeList* data)
{
    Container* c = err_malloc(sizeof(Container));
    c->type = type;
    c->data = data;

    NeObject* neo = neobject_create(TYPE_CONTAINER);
    neo->data.ptr = (void*)c;

    neo_alloc_refc(neo);

    // pour le garbage collector
    gc_add_deep_object(neo);

    return neo;
}



NeObject* neo_nb_create(Number number)
{
    NeObject* neo = neobject_create(TYPE_NUMBER);
    neo->data.ptr = err_malloc(sizeof(Number));
    *((Number*)neo->data.ptr) = number;

    neo_alloc_refc(neo);
    return neo;
}



NeObject* neo_promise_create(intptr_t id) {
    // une promesse contient un identifiant correspondant au processus dont elle dépend
    NeObject* neo = neobject_create(TYPE_PROMISE);
    neo->data.ptr = (void*)id;

    neo_alloc_refc(neo);
    return neo;
}



NeObject* neo_bool_create(_Bool neon_boolean)
{
    NeObject* neo = neobject_create(TYPE_BOOL);
    neo->data.ptr = err_malloc(sizeof(_Bool*));
    *((_Bool*)neo->data.ptr) = neon_boolean;

    neo_alloc_refc(neo);
    return neo;
}


_Bool neo_is_true(NeObject* neo)
{
    return *(_Bool*)neo->data.ptr;
}


NeObject* neo_fun_create(int nbArgsMeth, NeObject * (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour)
{
    NeObject* neo = neobject_create(TYPE_FONCTION);
    neo->data.ptr = function_create(nbArgsMeth,ptr, help, nbArgs, typeArgs, typeRetour);

    neo_alloc_refc(neo);
    return neo;
}



Function* function_create(int nbArgsMeth, NeObject * (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour)
{
    Function* fun = err_malloc(sizeof(Function));
    fun->type = TYPE_BUILTINFUNC;
    fun->ptr = ptr;
    fun->nbArgs = nbArgs;
    fun->help = help;
    fun->nbArgsMeth = nbArgsMeth;
    fun->typeRetour = typeRetour;

    // copie de typeArgs dans le tas
    if (nbArgs == -1)
    {
        fun->typeArgs = err_malloc(sizeof(int));
        fun->typeArgs[0] = typeArgs[0];
    }
    else
    {
        fun->typeArgs = err_malloc(sizeof(int)*nbArgs);
        for (int i = 0 ; i < nbArgs ; i++)
            fun->typeArgs[i] = typeArgs[i];
    }
    
    return fun;
}


NeObject* functionCall(NeObject* fun, NeList* args)
{
    Function* f = fun->data.ptr;
    if (f->nbArgs != -1 && f->nbArgs != args->len)
    {
        CODE_ERROR = 36;// nombre d'arguments invalide pour appeler cette fonction
        return 0;
    }
    NeObject* (*ptr) (NeList*) = f->ptr;
    NeObject* ret = ptr(args);
    return ret;
}




void compFunc(const int* typesbuiltinsfonc, const char** helpbuiltinsfonc, const int* nbargs, const int** typesArgs, const int* typesRetour)
{
    NeObject* func;
    for (int i = 0 ; i < NBBUILTINFONC ; i++)
    {
        func = neo_fun_create(typesbuiltinsfonc[i], BUILTINSFONC[i], helpbuiltinsfonc[i], nbargs[i], typesArgs[i], typesRetour[i]);
        strlist_append(NOMS, strdup(NOMSBUILTINSFONC.tab[i]));
        nelist_append(ADRESSES, func);
    }
    return;
}




_Bool funcArgsCheck(Function* fun, NeList* args)
{

    if (fun->nbArgs == -1)
    {
        for (int i=0 ; i < args->len ; i++)
        {
            if (fun->typeArgs[0] != -1 && (int)args->tab[i]->type != fun->typeArgs[0])
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
            if (fun->typeArgs[i] != -1 && args->tab[i]->type != fun->typeArgs[i])
                return false;
        }
    }

    return true;
}



NeObject* userFuncCreate(NeObject** args, Tree* code, int nbArgs, _Bool unlimited_arguments, int nbOptArgs, Tree* opt_args)
{
    UserFunc* fun = err_malloc(sizeof(UserFunc));
    fun->args = args;
    fun->code = code;
    fun->nbArgs = nbArgs;
    fun->opt_args = opt_args;
    fun->nbOptArgs = nbOptArgs;
    fun -> unlimited_arguments = unlimited_arguments;
    fun->doc = NULL;

    NeObject* neo = neobject_create(TYPE_USERFUNC);
    neo->data.ptr = (void*)fun;
    
    neo_alloc_refc(neo);
    return neo;
}








NeObject* neo_str_create(char* string) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObject* neo = neobject_create(TYPE_STRING);
    neo->data.ptr = string;

    neo_alloc_refc(neo);
    return neo;
}



NeObject* neo_exception_create(int index)
{
    NeObject* neo = neobject_create(TYPE_EXCEPTION);
    // détermine la place de la chaine dans le tableau exception
    neo->data.ptr = (void*)(intptr_t)index;

    neo_alloc_refc(neo);
    return neo;
}

int get_exception_code(NeObject* exception) {
    return (int)(intptr_t)exception->data.ptr;
}



NeObject* neo_const_create(char* string) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObject* neo = neobject_create(TYPE_CONST);
    neo->data.ptr = string;

    neo_alloc_refc(neo);
    return neo;
}



NeObject* neo_none_create(void) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObject* neo = neobject_create(TYPE_NONE);
    return neo;
}


/*
NeObject* neo_copy(NeObject* neo) {

    // les promesses sont les seuls objets copiés en dur à chaque fois
    if (neo->type == TYPE_PROMISE) {
        intptr_t id = (intptr_t)neo->data.ptr;

        if (PROMISES->tab[(int)id] != NULL) // la promesse a été résolue
        {
            // il n'y a pas besoin de spécialement soustraire un au compteur car comme on détruit neo, neobject_destroy le fait tout seul

            if (PROMISES_CNT.tab[(int)id] == 1) // on s'apprête à renvoyer la dernière valeur associée à cette promesse
            {
                PROMISES_CNT.tab[(int)id]++; // pour être sur que l'objet PROMISES[id] soit pas supprimé
                neobject_destroy(neo, false);
                neo->type = PROMISES->tab[(int)id]->type;
                neo->data = PROMISES->tab[(int)id]->data;
                err_free(PROMISES->tab[(int)id]);
                PROMISES->tab[(int)id] = NULL;
                PROMISES_CNT.tab[(int)id] = 0;
            }
            else
            {
                _affect2(neo, PROMISES->tab[(int)id]);
            }

            return neo_copy(neo);
        }
        else {
            PROMISES_CNT.tab[(int)id]++; // une nouvelle promesse de ce type
            return neo_promise_create((intptr_t)neo->data.ptr);
        }
    }
    else {
        NeObject* neo_cop = neobject_create(neo->type);
        neo_cop->data = neo->data;
        // il y a le cas des TYPE_EMPTY et des TYPE_NONE que l'on peut copier mais on a aucun refc à incrémenter
        // en fin de compte, un refc ça sert à compter les références du ptr au dessus donc s'il n'y en a pas il sert à rien
        if (neo->data.ptr != NULL) {
            *neo_cop->data.refc += 1;
        }
        return neo_cop;
    }
}
*/

NeObject* neo_copy(NeObject* neo) {
    int ret = update_if_promise(neo);
    
    if (ret != -1) { // l'objet était une promesse non résolue
        // donc on copie la promesse
        PROMISES_CNT.tab[(int)ret]++; // une nouvelle promesse de ce type
        return neo_promise_create(ret); // une promesse doit toujours garder un refc de zéro car on a une action spéciale à effectuer à chaque suppression de promesse
    }

    NeObject* neo_cop = neobject_create(neo->type);
    neo_cop->data = neo->data;
    // il y a le cas des TYPE_EMPTY et des TYPE_NONE que l'on peut copier mais on a aucun refc à incrémenter
    // en fin de compte, un refc ça sert à compter les références du ptr au dessus donc s'il n'y en a pas il sert à rien
    if (neo->data.ptr != NULL) {
        *neo_cop->data.refc += 1;
    }
    return neo_cop;
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

void mark_as_already_copied(NeObject* neo, NeObject* copy) {
    if (neo->type == TYPE_LIST) {
        mark(neo);
        NeList* list = neo->data.ptr;
        nelist_append(list, copy); // on stocke la copie au bout de la liste
        list->len--; // on remet la taille d'origine pour pas que ça se voie
    }
    else if (neo->type == TYPE_CONTAINER) {
        mark(neo);
        Container* c = neo->data.ptr;
        NeList* list = c->data;
        nelist_append(list, copy); // on stocke la copie au bout de la liste
        list->len--; // on remet la taille d'origine pour pas que ça se voie
    }
    // les autres types d'objets ne sont pas sensibles aux dépendances de pointeurs
}

NeObject* get_previous_copy(NeObject* neo) {
    if (neo->type == TYPE_LIST && ismarked(neo)) {
        NeList* list = neo->data.ptr;
        return list->tab[list->len]; // comme l'objet est marqué on sait qu'on a le droit d'aller chercher au delà des limites légales
    }
    else if (neo->type == TYPE_CONTAINER && ismarked(neo)) {
        Container* c = neo->data.ptr;
        NeList* list = c->data;
        return list->tab[list->len];
    }
    return NULL;
    // les autres types d'objets ne sont pas sensibles aux dépendances de pointeurs
}


NeObject* neo_deepcopy(NeObject* neo) {
    NeObject* copy = neo_dup(neo);
    // tous les containers et toutes les listes ont été marqué.es, il faut donc les démarquer
    recursive_unmark(neo);
    return copy;
}




/*
Cette fonction prend en argument une promesse, et la remplace par la valeur de renvoi du processus correspondant
si celui-ci a terminé
Cette fonction peut être appelées sur n'importe quel objet, mais n'aura d'effet que sur une promesse
*/
int update_if_promise(NeObject* promise) {
    if (promise->type == TYPE_PROMISE)
    {
        intptr_t id = (intptr_t)promise->data.ptr;

        if (PROMISES->tab[(int)id] != NULL) // la promesse a été résolue
        {
            // il n'y a pas besoin de spécialement soustraire un au compteur car comme on détruit neo, neobject_destroy le fait tout seul

            if (PROMISES_CNT.tab[(int)id] == 1) // cette promesse était la dernière promesse non résolue
            {
                PROMISES_CNT.tab[(int)id]++; // pour être sur que l'objet PROMISES[id] soit pas supprimé
                neobject_destroy(promise, false); // on supprime la promesse en elle même
                // on la remplace par la valeur de retour (sans faire de copie puisque c'est la dernière)
                promise->type = PROMISES->tab[(int)id]->type;
                promise->data = PROMISES->tab[(int)id]->data;
                err_free(PROMISES->tab[(int)id]);
                PROMISES->tab[(int)id] = NULL;
                PROMISES_CNT.tab[(int)id] = 0;
            }
            else
            {
                _affect2(promise, PROMISES->tab[(int)id]);
            }

            return update_if_promise(promise);
        }
        return id; // on renvoie un entier différent de -1 si et seulement s'il s'agit d'une promesse non résolue
    }
    return -1;
}






NeObject* neo_dup(NeObject* neo) {

    // si l'objet que l'on veut copier a déjà été copié, on renvoie directement sa copie
    NeObject* copy = get_previous_copy(neo);

    if (copy) {
        // on ne peut pas faire pointer deux endroits différents directement sur le même NeObject*, il faut quand même copier l'enveloppe
        return neo_copy(copy);
    }

    // sinon, on le copie en bonne et due forme

    // on commence par résoudre la promesse si c'en est une
    update_if_promise(neo);
    
    if (neo->type == TYPE_NUMBER)
    {
        return neo_nb_create(number_copy(neo_to_nb(neo)));
    }

    else if (neo->type == TYPE_CONST)
    {
        return neo_const_create(strdup(neo_to_const(neo)));
    }

    else if (neo->type == TYPE_STRING)
    {
        return neo_str_create(strdup(neo_to_string(neo)));
    }

    else if (neo->type == TYPE_NONE)
    {
        return neo_none_create();
    }

    else if (neo->type == TYPE_BOOL)
    {
        return neo_bool_create(neo_to_bool(neo));
    }

    else if (neo->type == TYPE_EXCEPTION)
    {
        return neo_exception_create(get_exception_code(neo));
    }
    
    else if (neo->type == TYPE_LIST)
    {
        // on ne peut pas ajouter la liste maintenant au garbage collector puisque son pointeur est pour le moment invalide
        NeObject* copy = gc_extern_neo_list_convert(NULL);
        mark_as_already_copied(neo, copy); // inscrit le nouveau pointeur de la copie directement dans l'objet
        NeList* data = nelist_dup((NeList*)neo->data.ptr);
        copy->data.ptr = data;
        // maintenant que l'objet est complet on peut l'ajouter au garbage collector
        gc_add_deep_object(copy);
        return copy;
    }
    else if (neo->type == TYPE_FONCTION)
    {
        Function* fun = neo->data.ptr;
        NeObject* ret = neo_fun_create(fun->nbArgsMeth, fun->ptr, fun->help, fun->nbArgs, fun->typeArgs, fun->typeRetour);
        return ret;
        
    }
    else if (neo->type == TYPE_USERFUNC || neo->type == TYPE_USERMETHOD)
    {
        UserFunc* fun = neo->data.ptr;
        NeObject** args = err_malloc(sizeof(NeObject) * fun->nbArgs);
        
        for (int i = 0 ; i < fun->nbArgs ; i++)
            args[i] = fun->args[i];

        Tree* opt_args2 = tree_copy(fun->opt_args);

        NeObject* ret = userFuncCreate(args, fun->code, fun->nbArgs, fun->unlimited_arguments, fun->nbOptArgs, opt_args2);

        ((UserFunc*)ret->data.ptr)->doc = (fun->doc == NULL) ? NULL : strdup(fun->doc);

        ret->type = neo->type;
        
        return ret;
    }
    else if (neo->type == TYPE_CONTAINER)
    {
        Container* c = (Container*)neo->data.ptr;
        // comme pour les listes, on ne peut pas directement dire au GC de copier notre objet dans sa banque puisqu'il n'est pas encore valide
        NeObject* copy = gc_extern_neo_container_create(c->type, NULL);
        mark_as_already_copied(neo, copy); // on inscrit le nouveau pointeur de la copie directement dans l'objet
        NeList* data = nelist_dup(c->data);
        ((Container*)copy->data.ptr)->data = data; // remplit avec les éléments copiés
        // l'objet est complet, on peut l'ajouter au garbage collector
        gc_add_deep_object(copy);
        return copy;
    }
    else if (neo->type == TYPE_PROMISE)
    {
        // si on est là, c'est que notre objet est une promesse non résolue
        intptr_t id = (intptr_t)neo->data.ptr;
        PROMISES_CNT.tab[(int)id]++;
        return neo_promise_create(id);
    }
    else if (neo->type == TYPE_EMPTY)
    {
        return neobject_create(TYPE_EMPTY);
    }
    
    return NULL;
}







Number neo_to_nb(NeObject* neo)
{
  return *((Number*)neo->data.ptr);
}


_Bool neo_to_bool(NeObject* neo)
{
  return *((_Bool*)neo->data.ptr);
}



char* neo_to_string(NeObject* neo)
{
  return (char*)neo->data.ptr;
}


char* neo_to_const(NeObject* neo)
{
  return (char*)neo->data.ptr;
}



/*
Cette fonction doit absolument recevoir une liste valide, car on va directement copier l'objet pour l'ajouter
au garbage collector
*/
NeObject* neo_list_convert(NeList* list)
{
    NeObject* neo = neobject_create(TYPE_LIST);
    neo->data.ptr = list;

    neo_alloc_refc(neo);

    // pour le garbage collector
    gc_add_deep_object(neo);

    return neo;
}

// cette fonction fabrique un NeObject* à partir d'une NeList*, mais ne l'inclus pas dans la système de Garbage Collection
// ca permet de fabriquer des objets qui ne sont pas accessibles depuis ADRESSES et qui ne disparaissent pas lorsque l'on
// appelle le garbage collector
NeObject* gc_extern_neo_list_convert(NeList* list) {
    NeObject* neo = neobject_create(TYPE_LIST);
    neo->data.ptr = list;

    neo_alloc_refc(neo);
    return neo;
}



NeObject* neo_list_create(int len)
{
    return neo_list_convert(nelist_create(len));
}




void neobject_destroy(NeObject* neo, _Bool bo)
{

    if (neo != NULL)
    {
        if ((neo->data.refc != NULL && *neo->data.refc == 0 && neo->data.ptr != NULL) || neo->type == TYPE_PROMISE) // on supprime neo->data
        {
            if (ismarked(neo)) // si l'objet est marqué, c'est qu'on est un des appels récursifs de sa suppression, donc il sera supprimé
                return;
            
            mark(neo);

            if (neo->type == TYPE_NUMBER)
            {
                number_destroy(neo_to_nb(neo));
                err_free(neo->data.ptr);
            }
            else if (neo->type == TYPE_LIST)
            {
                nelist_destroy((NeList*)neo->data.ptr,true);
            }
            else if (neo->type == TYPE_FONCTION)
            {
                Function* fun = neo->data.ptr;
                if (fun->type == TYPE_BUILTINFUNC)
                    err_free(fun->typeArgs);
                
                err_free(fun);
            }
    
            else if (neo->type == TYPE_USERFUNC || neo->type == TYPE_USERMETHOD)
            {
                UserFunc* fun = neo->data.ptr;
                if (fun->doc != NULL)
                    err_free(fun->doc);
                err_free(fun->args);
                tree_destroy(fun->opt_args);
                err_free(fun);
            }

            else if (neo->type == TYPE_CONTAINER)
            {
                Container* c = (Container*)neo->data.ptr;
                nelist_destroy(c->data, true);
                err_free(c);
            }
            else if (neo->type == TYPE_PROMISE)
            {
                intptr_t id = (intptr_t)neo->data.ptr;
                PROMISES_CNT.tab[(int)id]--; // une promesse de moins qui pointe dessus

                if (PROMISES_CNT.tab[(int)id] == 0) { // il n'y a plus d'objets de cette 'famille' de promise, donc on peut autoriser la libération de la case associée
                    neobject_destroy(PROMISES->tab[(int)id], true);
                    PROMISES->tab[(int)id] = NULL;
                }
            }
            else if (neo->type == TYPE_EXCEPTION) {
                neo->data.ptr = NULL;
            }
            else if (neo->data.ptr != NULL) {
                err_free(neo->data.ptr);
            }
            neo->data.ptr = NULL;
            
            err_free(neo->data.refc);
            neo->data.refc = NULL;
    
        }
        else if (neo->data.refc != NULL) {
            *neo->data.refc -= 1;
        }
        

        if (bo) // on supprime neo
        {
            err_free(neo);
        }
    }
    return;
}



void neobject_aff(NeObject* neo)
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
        update_if_promise(neo);
        
        if (neo->type == TYPE_NUMBER)
        {
            number_aff(neo_to_nb(neo));
        }
        else if (neo->type == TYPE_BOOL)
        {
            if (*((_Bool*)neo->data.ptr))
                printString("True");
            else
                printString("False");
        }
        else if (neo->type == TYPE_STRING)
        {
            printString("\"");
            char* traite = traitementStringInverse((char*)neo->data.ptr);
            printString(traite);
            printString("\"");
            err_free(traite);
        }
        else if (neo->type == TYPE_LIST)
        {
            nelist_aff((NeList*)neo->data.ptr);
        }
        else if (neo->type == TYPE_CONST)
        {
            printString(neo_to_string(neo));
        }
        else if (neo->type == TYPE_EXCEPTION)
        {
            printString(exceptions.tab[get_exception_code(neo)]);
        }
        else if (neo->type == TYPE_NONE)
        {
            printString("None");
        }
        else if (neo->type == TYPE_FONCTION)
        {
            Function* fun = neo->data.ptr;
            
            if (fun->nbArgsMeth == 0 && fun->type == TYPE_BUILTINFUNC)
                printString("<built-in function>");
            else if (fun->nbArgsMeth != 0 && fun->type == TYPE_BUILTINFUNC)
                printString("<built-in method>");
        }

        else if (neo->type == TYPE_USERFUNC)
        {
            printString("<function>");
        }

        else if (neo->type == TYPE_USERMETHOD)
        {
            printString("<method>");
        }
        else if (neo->type == TYPE_CONTAINER)
        {
            Container* c = neo->data.ptr;

            // on regarde si l'affichage est surchargé
            char* container_name = CONTAINERS->tab[c->type];
            int index = function_module(container_name, "repr");
            
            if (index >= 0) { // on exécute la fonction surchargée
                unmark(neo); // on va passer l'objet en argument à une fonction utilisateur, donc :
                // 1- C'est l'utilisateur qui gère les boucles infinies dans son objet
                // 2- Sinon ça va faire n'importe quoi dans les copies. Les marquages doivent rester invisibles à l'exécution
                
                NeObject* neo_fun = ADRESSES->tab[index];
                UserFunc* fun = neo_fun->data.ptr;
                _Bool isMethod = neo_fun->type == TYPE_USERMETHOD;
                
                NeList args = (NeList) {
                    .tab = (NeObject*[]) {neo},
                    .len = 1,
                    .capacity = 0
                };
                NeObject* ret = callUserFunc(fun, isMethod, &args, NULL);
                
                neobject_destroy(ret, true);
                if (CODE_ERROR != 0) {
                    return ;
                }
            }
            else {
                // on n'a pas trouvé de surcharge sur l'affichage

                NeList* list = (NeList*)(ATTRIBUTES->tab[c->type]->data.ptr);

                printString(CONTAINERS->tab[c->type]);
                printString("(");
                for (int i=0 ; i < c->data->len ; i++)
                {
                    printString(neo_to_string(list->tab[i]));
                    printString(": ");
                    neobject_aff(c->data->tab[i]);
                    if (i < c->data->len - 1)
                        printString(", ");
                }
                printString(")");
            }
        }
        else if (neo->type == TYPE_PROMISE) {
            printString("<promise from process ");
            Number nb = number_fromDouble((double)(intptr_t)neo->data.ptr);
            number_aff(nb);
            number_destroy(nb);
            printString(">");
        }

        unmark(neo);
    }

}



char* neobject_str(NeObject* neo)
{
    // on doit à tout prix éviter une boucle infinie due aux objets auto-référençant
    // pour ça avant d'afficher un objet on le marque, on le démarque après
    // si au moment d'afficher un objet il est marqué, alors on ne l'affiche pas

    if (ismarked(neo)) { // c'est le deuxième fois qu'on nous demande d'afficher cet objet
        return strdup("*");
    }
    else {
        mark(neo);
        update_if_promise(neo);

        char* ret = NULL; // la chaîne finale que l'on va renvoyer

        if (neo->type == TYPE_NUMBER)
        {
            ret = number_toStr(neo_to_nb(neo));
        }
        else if (neo->type == TYPE_BOOL)
        {
            if (*((_Bool*)neo->data.ptr))
                ret = strdup("True");
            else
                ret = strdup("False");
        }
        else if (neo->type == TYPE_STRING)
        {
            char* traite = traitementStringInverse((char*)neo->data.ptr);
            char* str1 = addStr("\"",traite);
            ret = addStr(str1,"\"");
            err_free(str1);err_free(traite);
        }
        else if (neo->type == TYPE_LIST)
        {
            ret = nelist_str((NeList*)neo->data.ptr);
        }
        else if (neo->type == TYPE_CONST)
        {
            ret = strdup(neo_to_string(neo));
        }
        else if (neo->type == TYPE_EXCEPTION)
        {
            ret = strdup(exceptions.tab[get_exception_code(neo)]);
        }
        else if (neo->type == TYPE_NONE)
        {
            ret = strdup("None");
        }
        else if (neo->type == TYPE_FONCTION)
        {        
            Function* fun = neo->data.ptr;

            if (fun->nbArgsMeth == 0 && fun->type == TYPE_BUILTINFUNC)
                ret = strdup("<built-in function>");
            else if (fun->nbArgsMeth != 0 && fun->type == TYPE_BUILTINFUNC)
                ret = strdup("<built-in method>");
        }

        else if (neo->type == TYPE_USERFUNC)
        {
            ret = strdup("<function>");
        }

        else if (neo->type == TYPE_USERMETHOD)
        {
            ret = strdup("<method>");
        }

        else if (neo->type == TYPE_CONTAINER)
        {
            Container* c = (Container*)neo->data.ptr;

            // on regarde si str est surchargé
            char* container_name = CONTAINERS->tab[c->type];
            int index = function_module(container_name, "str");
            
            if (index >= 0) { // on exécute la fonction surchargée
                unmark(neo); // sinon ça va faire caca, et de toute façon c'est l'utilisateur qui gère les boucles

                NeObject* neo_fun = ADRESSES->tab[index];
                UserFunc* fun = neo_fun->data.ptr;
                _Bool isMethod = neo_fun->type == TYPE_USERMETHOD;

                NeList args = (NeList) {
                    .tab = (NeObject*[]) {neo},
                    .len = 1,
                    .capacity = 0
                };
                NeObject* obj = callUserFunc(fun, isMethod, &args, NULL);
                
                if (CODE_ERROR != 0) {
                    neobject_destroy(obj, true);
                    return NULL;
                }
                ret = strdup(neo_to_string(obj));
                neobject_destroy(obj, true);
            }
            else {
                // on n'a pas trouvé de surcharge sur str

                NeList* list = (NeList*)(ATTRIBUTES->tab[c->type]->data.ptr);

                char* str1 = strdup(CONTAINERS->tab[c->type]);
                str1 = addStr2(str1, "(");
                for (int i=0 ; i < c->data->len ; i++)
                {
                    str1 = addStr2(str1, neo_to_string(list->tab[i]));
                    str1 = addStr2(str1, ": ");

                    char* s = neobject_str(c->data->tab[i]);
                    str1 = addStr2(str1, s);
                    err_free(s);

                    if (i < c->data->len - 1)
                        str1 = addStr2(str1, ", ");
                }

                ret = addStr2(str1, ")");
            }
        }
        else if (neo->type == TYPE_PROMISE) {
            char* s1 = strdup("<promise from process ");
            Number nb = number_fromDouble((double)(intptr_t)neo->data.ptr);
            char* s2 = number_toStr(nb);
            number_destroy(nb);
            s1 = addStr2(s1, s2);
            err_free(s2);
            s2 = strdup(">");
            ret = addStr2(s1, s2);
            err_free(s2);
        }
        unmark(neo);

        return ret;
    }

    return NULL;
}


_Bool nelist_equal(NeList* l1, NeList* l2)
{
    _Bool bo = l1->len==l2->len;
    for (int i=0 ; bo && i < l1->len ; i++)
        bo = bo && neo_equal(l1->tab[i], l2->tab[i]);
    
    return bo;
}



_Bool neo_equal(NeObject* _op1, NeObject* _op2)
{

    if (ismarked(_op1) && ismarked(_op2))
        return true;

    update_if_promise(_op1);
    update_if_promise(_op2);

    // si l'un des objets est un NeObject, on regarde si == est surchargé
    if (_op1->type == TYPE_CONTAINER || _op2->type == TYPE_CONTAINER) {
        Container* c = (_op1->type == TYPE_CONTAINER) ? _op1->data.ptr : _op2->data.ptr;
        char* container_name = CONTAINERS->tab[c->type];
        int index = function_module(container_name, "equal");
        if (index >= 0) {
            NeObject* neo_fun = ADRESSES->tab[index];
            UserFunc* fun = neo_fun->data.ptr;
            _Bool isMethod = neo_fun->type == TYPE_USERMETHOD;

            NeList args = (NeList) {
                .tab = (NeObject*[]) {_op1, _op2},
                .len = 2,
                .capacity = 1
            };
            NeObject* ret = callUserFunc(fun, isMethod, &args, NULL);
            if (CODE_ERROR != 0) {
                return false;
            }

            _Bool bo = neoIsTrue(ret);
            neobject_destroy(ret, true);
            return bo;
        }
    }


    if (_op1->type==TYPE_NUMBER)
    {
        if (_op2->type==TYPE_NUMBER)
        {
            return number_equal(neo_to_nb(_op1), neo_to_nb(_op2));
        }
        else
        {
            return false;
        }
    }

    else if (_op1->type==TYPE_STRING)
    {
        if (_op2->type==TYPE_STRING)
        {
            return strcmp(neo_to_string(_op1),neo_to_string(_op2)) == 0;
        }
        else
        {
            return false;
        }
    }

    else if (_op1->type==TYPE_BOOL)
    {
        if (_op2->type==TYPE_BOOL)
        {
            return neo_to_bool(_op1)==neo_to_bool(_op2);
        }
        else
        {
            return false;
        }
    }
  
    else if (_op1->type==TYPE_NONE && _op2->type==TYPE_NONE)
        return true;
  
    else if (_op1->type==TYPE_EXCEPTION)
    {
        if (_op2->type==TYPE_EXCEPTION)
        {
            return get_exception_code(_op1) == get_exception_code(_op2);
        }
        else
        {
            return false;
        }
    }

    else if (_op1->type == TYPE_FONCTION)
    {
        if (_op2->type == TYPE_FONCTION)
        {
            Function* f1=_op1->data.ptr;
            Function* f2=_op2->data.ptr;
            return f1->ptr==f2->ptr;
        }
        else
        {
            return false;
        }
    }

    else if (_op1->type == TYPE_USERFUNC || _op1->type == TYPE_USERMETHOD)
    {
        if (_op2->type == TYPE_USERFUNC || _op2->type == TYPE_USERMETHOD)
        {
            UserFunc* f1=_op1->data.ptr;
            UserFunc* f2=_op2->data.ptr;
            return f1->code==f2->code;
        }
        else
        {
            return false;
        }
    }

    else if (_op1->type == TYPE_LIST)
    {
        if (_op2->type == TYPE_LIST)
        {
            mark(_op1); mark(_op2); // marque les deux objets comme déjà comparés, on lance l'appel récursif
            _Bool bo = nelist_equal(_op1->data.ptr, _op2->data.ptr);
            unmark(_op1); unmark(_op2);
            return bo;
        }
        else
            return false;
    }

    else if (_op1 -> type == TYPE_CONTAINER)
    {
        if (_op2 -> type == TYPE_CONTAINER)
        {
            mark(_op1); mark(_op2); // marque les deux objets comme déjà comparés, on lance l'appel récursif
            Container* c1 = _op1->data.ptr;
            Container* c2 = _op2->data.ptr;
            _Bool bo = c1->type == c2->type && nelist_equal(c1->data, c2->data);
            unmark(_op1); unmark(_op2);
            return bo;
        }
        else
            return false;
    }
    else if (_op1->type == TYPE_PROMISE)
        return _op2->type == TYPE_PROMISE && _op1->data.ptr == _op2->data.ptr;

    return false; // dans tous les autres cas
  
}






int neo_compare(NeObject* a, NeObject* b)
{
    // si l'un des objets est un NeObject, on regarde si == est surchargé
    if (a->type == TYPE_CONTAINER || b->type == TYPE_CONTAINER) {
        Container* c = (a->type == TYPE_CONTAINER) ? a->data.ptr : b->data.ptr;
        char* container_name = CONTAINERS->tab[c->type];
        int index = function_module(container_name, "comp");
        if (index >= 0) {
            NeObject* neo_fun = ADRESSES->tab[index];
            UserFunc* fun = neo_fun->data.ptr;
            _Bool isMethod = neo_fun->type == TYPE_USERMETHOD;

            NeList args = (NeList) {
                .tab = (NeObject*[]) {a, b},
                .len = 2,
                .capacity = 1
            };
            NeObject* ret = callUserFunc(fun, isMethod, &args, NULL);
            if (CODE_ERROR != 0) {
                neobject_destroy(ret, true);
                return 0;
            }
            
            int res = (int)number_toDouble(neo_to_nb(ret));
            neobject_destroy(ret, true);
            return res;
        }
    }

    if (a->type == TYPE_NUMBER && b->type == TYPE_NUMBER)
    {
        return number_compare(neo_to_nb(a), neo_to_nb(b));
    }
    else if (a->type == TYPE_STRING && b->type == TYPE_STRING)
    {
        return strcmp(neo_to_string(a), neo_to_string(b));
    }
    else
    {
        //erreur
        CODE_ERROR = 90;
        return 0;
    }
}


int neo_compare2(NeObject* a, NeObject* b)
{
    int c = neo_compare(b,a);
    if (CODE_ERROR != 0)
        return 0;
    else
        return c;
}




void neo_list_append(NeObject* neo, NeObject* ptr)
{
    nelist_append((NeList*)neo->data.ptr, ptr);
    return;
}




void neo_list_remove(NeObject* neo, int index)
{
    nelist_remove((NeList*)neo->data.ptr, index);
    return;
}



void neo_list_insert(NeObject* neo, NeObject* ptr,int index)
{
    nelist_insert((NeList*)neo->data.ptr, ptr, index);
    return;
}



NeObject* neo_list_nth(NeObject* neo, int index)
{
  return ((NeList*)neo->data.ptr)->tab[index];
}


int neo_list_len(NeObject* neo)
{
  return ((NeList*)neo->data.ptr)->len;
}


//nelist

NeList* nelist_create(int len)
{
  NeList* list = err_malloc(sizeof(NeList));//crée la structure
  
  list->capacity = 0;
  while (pow(2, list->capacity) < len)
    list->capacity++;
  
  list->tab=err_malloc(pow(2, list->capacity)*sizeof(NeObject*));//initialise le tableau de longueur len avec de zéros
  
  err_memset(list->tab,0,len);
  list->len=len;//initialise la bonne longueur
  return list;//retourne la structure
}



void nelist_append(NeList* list, NeObject* ptr)//ajoute un élément à la fin de la liste
{
    NeObject** tmp;

    if (pow(2, list->capacity)==list->len)
    {
        list->capacity++;
        tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(NeObject*));//réallocation de list.tab
        list->tab = tmp;//affectation du pointeur de tmp vers list.tab
    }

    list->tab[list->len]=ptr;//affecte ptr au dernier élément
    list->len++;//incrémente la longueur
}






void nelist_insert(NeList* list,NeObject* neo, int index)//ajoute un élément à la place indiquée
{
  if (index > list->len)
  {
        CODE_ERROR = 37;//out of range
        return ;
  }
  
  NeObject** tmp;
  
  if (pow(2, list->capacity)==list->len)
  {
    list->capacity++;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(NeObject*));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
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
        CODE_ERROR = 38;//out of range
        return ;
    }
  
    neobject_destroy(list->tab[index],true);
  
    for (int i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
        list->tab[i]=list->tab[i+1];
    
    NeObject** tmp;
  
    if (pow(2, list->capacity-1)==list->len-1)
    {
        list->capacity--;
        tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(NeObject*));//réalloue un nouveau pointeur de la bonne taille
        list->tab = tmp;
    }
  
  
    list->len--;
}




int nelist_index(NeList* liste, NeObject* neo)
{
    
    for (int i=0 ; i < liste->len ; i++)
    {
        if (liste->tab[i] == neo)
        {
            return i;
        }
    }
    CODE_ERROR = 39; // cet objet n'existe pas
    
    return -1;
}






int nelist_index2(NeList* l, NeObject* neo)
{
    for (int i = 0; i < l->len ; i++)
    {
        if (neo_equal(neo, l->tab[i]))
            return i;
    }
    CODE_ERROR = 88;
    return -1;
}


// Des fonctions pour marquer temporairement des NeObjects pour savoir si on est déjà passé par eux.


// mark et unmark sont des involutions
// il est donc essentiel de vérifier qu'un objet n'est pas déjà marqué avant de le marquer
// sinon dans neo_equal quand on veut comparer un objet avec lui-même, on le marque deux fois, et au final il n'est pas marqué
void mark(NeObject* neo) {
    if (neo->data.refc != NULL && *neo->data.refc >= 0)
        *neo->data.refc = -*neo->data.refc - 1;
}

void unmark(NeObject* neo) {
    if (neo->data.refc != NULL && *neo->data.refc < 0)
        *neo->data.refc = -*neo->data.refc - 1;
}

_Bool ismarked(NeObject* neo) {
    return neo->data.refc != NULL && *neo->data.refc < 0;
}

void recursive_unmark(NeObject* neo) {
    // si l'objet n'est pas marqué, il n'y a forcément rien à démarquer dedans, soit parce que l'on ne l'a pas copié, soit
    // parce que cet algorithme l'a déjà démarqué
    if (ismarked(neo)) {
        unmark(neo);

        NeList* list;
        if (neo->type == TYPE_LIST)
            list = neo->data.ptr;
        else if (neo->type == TYPE_CONTAINER)
            list = ((Container*)neo->data.ptr)->data;
        else
            return;

        // démarque tous les fils
        for (int i=0 ; i < list->len ; i++) {
            recursive_unmark(list->tab[i]);
        }
    }
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
        for (int i=0;i<liste->len-1;i++)
        {
            neobject_aff(liste->tab[i]);
            printString(", ");
        }
        neobject_aff(liste->tab[liste->len-1]);
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
            temp = neobject_str(list->tab[i]);
            str2 = addStr(str1,temp);
            err_free(temp);err_free(str1);
            str1 = addStr(str2,", ");
            err_free(str2);
        }
        temp = neobject_str(list->tab[list->len - 1]);
        str2 = addStr(str1,temp);
        err_free(str1);err_free(temp);
        str1 = addStr(str2, "]");
        err_free(str2);
        return str1;
    }
    return strdup("Erreur17");
}






void nelist_destroy(NeList* list,_Bool bo)
{
    for (int i=0;i<list->len;i++)
    {
        //if (list == ADRESSES)
        //    printf("Suppression de %s\n", NOMS->tab[i]);
        neobject_destroy(list->tab[i],true);
    }
    err_free(list->tab);

    if (bo)
        err_free(list);
}


char* type(NeObject* neo)
{
    if (neo->type == TYPE_BOOL)
        return "Bool";

    if (neo->type == TYPE_STRING)
        return "String";

    if (neo->type == TYPE_NUMBER)
        return "Number";

    if (neo->type == TYPE_FONCTION)
        return "Built-in function";

    if (neo->type == TYPE_LIST)
        return "List";

    if (neo->type == TYPE_USERFUNC)
        return "Function";

    if (neo->type == TYPE_USERMETHOD)
        return "Method";

    if (neo->type == TYPE_EXCEPTION)
        return "Exception";
    
    if (neo->type == TYPE_CONTAINER)
    {
        Container* c = (Container*)neo->data.ptr;
        return CONTAINERS->tab[c->type];
    }
    if (neo->type == TYPE_PROMISE)
    {
        return "Promise";
    }
    if (neo->type == TYPE_EMPTY)
    {
        return "Undefined";
    }

    if (neo->type == TYPE_ANYTYPE)
        return "any type";

    return "NoneType";

}


_Bool nelist_inList2(NeList* list, NeObject* neo)
{
    _Bool bo = false;
    
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



_Bool nelist_inList(NeList* list, NeObject* neo)
{
    _Bool bo = false;
    
    for (int i = 0 ; i < list->len ; i++)
    {
        if (list->tab[i] == neo)
        {
            bo = true;
            break;
        }
    }

    return bo;
}


NeList* neo_to_list(NeObject* neo) {
    return neo->data.ptr;
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

// supprime totalement une liste sans libérer ses éléments, de manière respectueuse du compteur de références
void neo_list_free(NeObject* list) {
    if (*list->data.refc == 0) { // plus personne ne pointe sur cette NeList, donc on peut bel et bien la supprimer
        err_free(((NeList*)(list->data.ptr))->tab);
        err_free(list->data.ptr);
        err_free(list->data.refc);
    }
    // si des gens pointent encore vers la liste, il restera encore le refc et la NeList, donc c'est ok
    err_free(list);
}


char* var_name(NeObject* obj) {
    if (nelist_inList(ADRESSES, obj)) {
        return NOMS->tab[nelist_index(ADRESSES, obj)];
    }
    else {
        return "<anonymous object>";
    }
}


NeObject* callOverloadedBinaryOperator(NeObject* op1, NeObject* op2, char* opname) {
    Container* c = (op1->type == TYPE_CONTAINER) ? op1->data.ptr : op2->data.ptr;
    char* container_name = CONTAINERS->tab[c->type];
    int index = function_module(container_name, opname);
    if (index == -1) {
        CODE_ERROR = 107;
        return NULL;
    }
    else {
        NeObject* neo_fun = ADRESSES->tab[index];
        UserFunc* fun = neo_fun->data.ptr;
        _Bool isMethod = neo_fun->type == TYPE_USERMETHOD;

        NeList args = (NeList) {
            .tab = (NeObject*[]) {op1, op2},
            .len = 2,
            .capacity = 1
        };
        NeObject* ret = callUserFunc(fun, isMethod, &args, NULL);
        if (CODE_ERROR != 0) {
            neobject_destroy(ret, true);
            return NULL;
        }
        return ret;
    }
}


NeObject* callOverloadedUnaryOperator(NeObject* op1, char* opname) {
    Container* c = op1->data.ptr;
    char* container_name = CONTAINERS->tab[c->type];
    int index = function_module(container_name, opname);
    if (index == -1) {
        CODE_ERROR = 107;
        return NULL;
    }
    else {
        NeObject* neo_fun = ADRESSES->tab[index];
        UserFunc* fun = neo_fun->data.ptr;
        _Bool isMethod = neo_fun->type == TYPE_USERMETHOD;

        NeList args = (NeList) {
            .tab = (NeObject*[]) {op1},
            .len = 1,
            .capacity = 0
        };
        NeObject* ret = callUserFunc(fun, isMethod, &args, NULL);
        if (CODE_ERROR != 0) {
            neobject_destroy(ret, true);
            return NULL;
        }
        return ret;
    }
}