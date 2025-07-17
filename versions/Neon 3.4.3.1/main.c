#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "headers.c"

#ifdef WASM
    #include <emscripten/emscripten.h>
    char* INPUTMESSAGE = NULL;
#endif



#ifdef TI83PCE
    #include "nio_ce.h"
    nio_console console;
#endif



int CODE_ERROR = 0; // contient le code de l'erreur survenue lors d'un appel de fonction


#if defined(LINUX)
    #include <signal.h>

    void handle_sigint(int sig) {
        CODE_ERROR = 104;
    }
#endif


#if defined(WINDOWS11) || defined(WINDOWS10)
    #include <windows.h>

    // Fonction de gestion du signal
    BOOL WINAPI ctrlHandler(DWORD signal) {
        if (signal == CTRL_C_EVENT) {
            CODE_ERROR = 104;
            return TRUE;  // Renvoyer TRUE pour indiquer que l'événement a été traité
        }
        return FALSE;
    }
#endif


/*
Liste des choses qui marchent pas :
Les objets qui pointent sur eux-même ne seront pas libérés
Doubles retours à la ligne
la multiplication de liste cause des soucis de fuite mémoire
ligne 1713 c'est débile ce qu'on fait
_____________________
Pour éviter d'empiler plusieurs fois la même chose dans les ast (du fait que la construction est basée sur typeTok),
j'ai créé une fonction qui vérifie si on essaie d'empiler deux fois la même chose à la suite.
Cette fonction est pour le moment désactivée car je pense avoir trouvé la solution en mettant block sur debTok2 au lieu de debTok3
On pourra la réactiver au besoin

J'ai inséré la vérification (et agrandissement si nécessaire) de la taille des piles au moment de switcher entre processus,
ça a drastiquement amélioré les performances de l'interpréteur.

Optimisations à venir :
-------------------------
> Pré-allocation de NeObject* grâce au malloc en O(1) développé en python
> Restriction des 3 piles à seulement une pile
    > Ordre précis dans l'empilement des arguments et de la zone de retour : empilement de la zone de retour, puis des arguments
    > Quant à la valeur de retour on n'est même pas obligé de l'empile, on peut la stocker dans un "registre" global
> Tentative de retour à une utilisation standard de la pile, avec des petites routines en assembleur pour faire les échanges
de pointeur de pile
    > Une fonction execval principale qui appelle une petite routine en assembleur qui se charge de faire des échanges de pointeurs de pile
    > Une fonction execval_aux qui est codée entièrement en C, qui appelle récursivement execval et qui repose entièrement sur
    la pile système.


Ceci a le potentiel de nous faire devenir encore plus rapide qu'en 2.x



Avancement et choses à faire :
-------------------------------
> WARNING : Il peut y avoir des problèmes de deleteContext qui supprime des choses en trop
> Tester de fond en comble ce compteur de références, fichier de test listes doublement chaînées
> Petit Mark & Sweep à venir
> NeObject* préalloués, malloc en O(1)
> Restriction de l'utilisation des piles à une seule pile
> Retour à une gestion automatique des variables locales d'execval

> Mettre des caractères spéciaux dans les mots clés modifiés (expt, ei, es, tr, atmc, ...)
> Faire des vrais arguments (genre -i, etc)
> Permettre à un processus d'arrêter ou de pauser un processus via une promesse
> vérifier les priorités de tous les opérateurs

Procédure pour ajouter une fonction built-in :
------------------------------------------------
1) Programmer cette fonction selon le prototype NeObject* _maFonction_ (NeList* args)
4) Ajouter 1 à la constante pré-processeur NBBUILTINFONC définie au début de neon.h
3) Ajouter le prototype de cette fonction tout à la fin de neon.h
4) Ajouter la fonction dans le tableau de fonctions au début de ce fichier
5) Compléter les informations sur la fonction dans neonInit

NOUVEAUTéS depuis la dernière publication :
-------------------------------------------
- Nouveaux algorithmes de parsing : +0.1
    - Possibilité d'appeler des fonctions juste après l'indexation de liste, et de combiner ces schémas comme bon nous semble
    - Meilleure gestion des parenthèses dans les expressions
- Multiples blocs except +0.1
- Indication de la ligne du programme où l'erreur est survenue, et affichage de la ligne, peu importe l'erreur +0.1
- Variable Ans qui stocke le résultat des expressions dans le terminal +0.0.1
- Multiplications entre booléens et listes/strings/nombres +0.0.1
- Fonctionnalités étendues de la fonction help +0.0.0.1
- Compteur de références, nouveaux algorithmes de comparaison, de copie et fonction built-in de copie
*/

// définitions de macro relatives aux piles, permet d'accélérer un peu l'exécution
#define RETURN_BACK()                                       stack_reduce(varstack, 2); goto *stack_pop(retstack)
#define stack_push(list, ptr)                               *(++list->stack_ptr) = ptr
#define stack_pop(stack)                                    *(stack->stack_ptr--)
#define stack_reduce(stack, size)                           stack->stack_ptr -= size
#define stack_extend(stack, size)                           stack->stack_ptr += size
#define return_int_execval(stack, value)                    *(++stack->stack_ptr) = (void*)(intptr_t)value
#define return_neo_execval(stack, value)                    *(++stack->stack_ptr) = (void*)value
#define stack_ret_zone(retzone, stack)                      *(++stack->stack_ptr) = (void*)retzone





/************** DÉFINITION DES VARIABLES GLOBALES ESSENTIELLES AU FONCTIONNEMENT DE NEON ****************/

// Neon utilise les variables globales à foison car cela permet de ne pas trimbaler de structure d'état dans toutes les
// fonctions. Cela a pour avantage d'être (un tout petit peu) plus rapide
// en revanche l'inconvénient est qu'il est difficile d'utiliser en même temps plusieurs instances différentes de
// l'interpréteur Neon (il faut échanger la valeur de toutes ces variables globales)


// LES VARIABLES RELATIVES AU MULTITÂCHE

// cette variable indique le nombre d'instructions exécuté sur chaque processus avant de passer au suivant
// il s'agit d'une variable et non d'une constante car la fonction setAtomicTime doit pouvoir le modifier
int ATOMIC_TIME = 1500;


// LES VARIABLES PERMETTANT DE GERER LES PROMESSES

// PROMISES associe aux identifiants des processus (l'indice dans le tableau) la valeur renvoyée par le processus
// Cette valeur est NULL quand le processus n'a pas terminé, et devient NeObject* avec la valeur renvoyée une fois le processus terminé
NeList* PROMISES = NULL;

// PROMISES_CNT compte le nombre de promises d'un même processus créées (en faisant des copies de la promesse originale)
// qui n'ont pas encore été transformées en la valeur renvoyée par le processus
// Ces compteurs servent à savoir jusqu'à quand garder la valeur renvoyée par le processus dans PROMISES
// Quand un compteur atteint zéro, on n'a plus besoin de la valeur, donc on peut la supprimer
intlist PROMISES_CNT;

// une liste de booléens qui indique si un processus a fini pour savoir si on peut lui piquer sa place
// Quand un processus n'a pas fini, on ne peut pas lui prendre sa place, en revanche ce n'est pas parce qu'il a fini qu'on peut lui prendre sa place
// En effet, même si un processus a fini, parfois toutes les promesses n'ont pas encore récupéré leur valeur
intlist PROCESS_FINISH;

// cette variable globale sert à stocker le texte des exceptions lancées par l'utilisateur (user-defined exceptions)
// printError affiche cette chaine de caractères lorsqu'il détecte une exception lancée par l'utilisateur
char* EXCEPTION = NULL; // exception levée par l'utilisateur, à afficher par printError


// VARIABLES RELATIVES A LA ZONE DU PROGRAMME ACTUELLEMENT INTERPRÉTÉE
// NAME représente l'indice dans ADRESSES du NeObject représentant le nom actuel de fichier interprété
// Cette valeur est utilisée par la variable __name__
int NAME = 0; // adresse du NeObject représentant le nom actuel de fichier. A ne pas utiliser, il y a à la place la fonction updateFileName

// cette chaine de caractères représente le nom du fichier actuellement interprété
// Ce n'est pas la même chose que NAME car ceci n'est utilisé que pour se souvenir du nom du fichier dans lequel aller lire
// la ligne à laquelle s'est produite une erreur. Par conséquent l'utilisateur ne doit pas pouvoir la modifier, elle ne peut
// pas valoir "__main__" comme c'est le cas pour NAME
char* FILENAME = NULL;

// cette variable est mise à jour à chaque nouveau sous-arbre interprété, et contient le numéro de la ligne actuellement interprétée
// cette variable couplée à FILENAME permet d'afficher la ligne du programme actuellement interprétée lorsqu'une erreur est levée
int LINENUMBER = 0; // contient le numéro de la ligne actuellement interprétée

// ce tableau de chaines de caractères contient les messages d'erreurs associés à chaque code d'erreur Neon.
// Il est de taille NB_ERRORS, et peut être indexé directement par le code d'erreur
strlist ERRORS_MESSAGES;

// cette variable est une liste chainée de listes chainées de NeSave.
// chaque liste chainée de NeSave correspond à un étage de localité.
// Quand on crée un contexte, on ajoute une nouvelle liste chainée dans VAR_LOC, et quand on veut rendre une variable locale,
// on ajoute une NeSave correspondante en tête de la liste chainée en tête de VAR_LOC
ptrlist* VAR_LOC; // c'est une pile de pile qui contiendra dans chaque contexte les variables à supprimer

// Cette variable est affectée lorsqu'une fonction utilisateur utilise return (). La valeur affectée est récupérée juste après
// par le bout de code qui avait lancé l'exécution de la fonction
NeObject* RETURN_VALUE = NULL; // contient la valeur de retour de la fonction si le programme a été interrompu par un return ()

// VARIABLES RELATIVES AU CONTAINERS
// les types containers sont chacun associées à un indice unique, le même dans ces deux tableaux
// pour un type de container, son indice dans CONTAINERS donne son nom (exemple : Cmplx, Point, ...)
strlist* CONTAINERS; // à un indice du tableau, associe le nom du type de container
// dans les containers, en interne, les champs sont stockés dans une NeList, toujours dans le même ordre
// Pour chaque type de container, ATTRIBUTES contient une liste de chaines de caractères correspondant aux noms des champs dans l'ordre
NeList* ATTRIBUTES; // contient pour chaque type de container, une liste des noms de champs

// Ceci n'est pas un arbre, mais plutot une forêt. Tous les fils de cet arbre sont les arbres syntaxiques des fonctions
// définies par l'utilisateur. Les stocker permet de garder des versions uniques de ces arbres, même lors de la copie
// de fonctions utilisateurs, et de tout libérer à la fin
Tree* FONCTIONS; // cet arbre va contenir en fils tous les arbres de fonction des fonctions et des méthodes définies par l'utilisateur

// Ce couple de variables permet de gérer les variables de Neon, c'est-à-dire les objets auxquels on peut directement accéder
// par un nom depuis l'interpréteur.
// au cours de toute la vie d'une variable, son NeObject* restera toujours le même, même quand on la détruit avec del
// Les noeuds des arbres faisant références à des variables stockent également le même NeObject* que dans ADRESSES
// Ainsi, pour modifier une variable, on ne peut pas changer son NeObject* (qui est fixe), on change donc directement son
// type, et son data. Cela engendre les magouilles qu'on fait pour appeler des fonctions et affecter ses arguments, et pour
// récupérer les valeurs de la variable objet d'une méthode
strlist* NOMS; // contient les noms des variables associés à leurs adresses mémoire dans ADRESSSES
NeList* ADRESSES; // contient les objets associés aux noms dans NOMS

// VARIABLES GLOBALES SERVANT A DEFINIR LES ELEMENTS MODIFIABLES DE LA SYNTAXE
// UNIQUEMENT UTILISES PAR LIBTOK (et un tout petit peu LIBSTR)

//définition de acceptedChars
strlist acceptedChars ;

// definit les types de tokens composes et indique par quel type de token compose ils peuvent etre precedes
listlist syntax;
intlist types_debut;
intlist types_fin;

//définition de sousop
strlist sousop ;

// liste qui contient les opérandes prises par les opérateurs, correspond a operateurs3[]
intlist gramm1 ;

//définition de operateurs3
strlist operateurs3;

//définition de operateurs1
// opérateurs composés de caractères de sousop
strlist operateurs1;

// opérateurs mots
strlist operateurs2;

//mots de blocs d'instructions
strlist blockwords;
strlist blockwords1Line;
strlist keywordFunction;
strlist keywords;
strlist lkeywords;
strlist neon_boolean;
strlist exceptions;
strlist constant;
strlist OPERATEURS;
intlist PRIORITE;
intlist OPERANDES;
intlist exceptions_err;

strlist NOMSBUILTINSFONC;




// fonctions des opérateurs
NeObject* (*OPFONC[NBOPERATEURS])(NeObject*,NeObject*)={_and,_or,_xor,_add,_mul,_sub,_div,_pow,_equal,_notEq,_infEqual,_supEqual,_inf,_sup,_affectNone,_addEqual,_subEqual,_mulEqual,_divEqual,_incr,_decr,_not,_mod,_eucl,_ref,_goIn,NULL, _minus, _del, _affect, NULL, _exponent, _implique, _in, NULL, NULL, _swap, NULL, NULL};

// fonctions des fonctions built-in
NeObject* (*BUILTINSFONC[NBBUILTINFONC])(NeList*)={_print_,_input_,_nbr_,_str_,_len_,_substring_,_exit_,_append_,_remove_,_insert_,_type_, _reverse_, _eval_,_clear_,_help_, _randint_,_quitError_, _time_, _assert_, _output_, _chr_, _ord_, _list_comp_, _create_exception_, _exception_, _int_, _index_, _replace_, _count_, _list_, _sort_asc_, _sort_desc_, _sin_, _cos_, _tan_, _deg_, _rad_, _sqrt_, _ln_, _exp_, _log_, _log2_, _round_, _abs_, _ceil_, _floor_, _readFile_, _writeFile_, _setFunctionDoc_, _setAtomicTime_, _copy_};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// ceci est un tableau qui à un type d'arbre permet de savoir instantanément si c'est une expresssion (opérateur, liste, ...)
// ou un bloc de code à exécuter (conditionblock, for, return, ...)
static _Bool isExpression[] = {
    true,
    true,
    true,
    true,
    true,
    true,
    false,
    true,
    false,
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    true,
    false,
    false,
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    true,
    true,
    false,
    false
};




void updateFileName(char* name)
{
    neobject_destroy(ADRESSES->tab[NAME], true);
    ADRESSES->tab[NAME] = neo_str_create(name);

    return ;
}





void newContext(void)
{
    // création d'un nouveau contexte
    ptrlist* ctxt = ptrlist_create();
    ptrlist_append(VAR_LOC, ctxt);
}


void deleteContext(void)
{
    //suppression du contexte
    ptrlist* ctxt2 = (ptrlist*)VAR_LOC->tete;

    
    if (ctxt2->tete == NULL) // aucun élément
    {
        err_free(ctxt2);
    }

    else if (ctxt2->queue == NULL) // un seul élément
    {
        //-------------
        NeSave* ns = ctxt2->tete;
        neobject_destroy(ns->object, false);
        ns->object->data = ns->data;
        ns->object->type = ns->type;
        err_free(ns);
        //-------------

        err_free(ctxt2);
    }
    else // plusieurs éléments
    {
        ptrlist* temp = NULL;
        ptrlist* ptr = ctxt2;
        while (ptr->queue != NULL)
        {
            //-------------
            NeSave* ns = ptr->tete;
            neobject_destroy(ns->object, false);
            ns->object->data = ns->data;
            ns->object->type = ns->type;
            err_free(ns);
            //-------------
            
            temp = ptr->queue;
            err_free(ptr);
            ptr = temp;
        }
        //-------------
        NeSave* ns = ptr->tete;
        neobject_destroy(ns->object, false);
        ns->object->data = ns->data;
        ns->object->type = ns->type;
        err_free(ns);
        //-------------
        
        err_free(ptr);
    }

    ptrlist_remove(VAR_LOC, ctxt2, true); // enlève le pointeur

    return ;
}






_Bool neoIsTrue(NeObject* expr)
{
    if (CODE_ERROR != 0)
    {
        return NULL;
    }
    
    if (expr->type == TYPE_BOOL)
    {
        _Bool res = neo_to_bool(expr);
        return res;
    }
    else if (expr->type == TYPE_NUMBER)
    {
        _Bool res = (int)number_toDouble(neo_to_nb(expr)) != 0;
        return res;
    }
    else
    {
        CODE_ERROR = 20;
    }
    return false;
}






void local(NeObject* var)
{
    // sauvegarde de l'object actuel
    NeSave* ns = err_malloc(sizeof(NeSave));
    ns->object = var;
    ns->data = var->data;
    ns->type = var->type;

    // réinitialisation de la variable
    neobject_reset(var);

    //ajout de la sauvegarde au contexte actuel
    
    ptrlist_append((ptrlist*)(VAR_LOC->tete), (void*)ns);
}




void get_arguments_expr(Tree** tree, intptr_t* calcListIndex, Stack* varstack) {
    *tree = (Tree*)stack_pop(varstack);
    *calcListIndex = (intptr_t)stack_pop(varstack);
}


void stack_args_expr(Tree* tree, intptr_t calcListIndex, Stack* varstack) {
    {*(++varstack->stack_ptr) = (void*)calcListIndex ; *(++varstack->stack_ptr) = (void*)tree;}
}




/*
void stack_ret_zone(void* retzone, Stack* retstack) {
    //stack_check_for(retstack, 1);
    stack_push(retstack, retzone);
}
*/




ProcessCycle* processCycle_create(void) {
    ProcessCycle* pc = err_malloc(sizeof(ProcessCycle));
    pc->process = NULL;
    pc->next = NULL;
    pc->prev = NULL;
    return pc;
}








void save_later(ptrlist* variables_a_sauvegarder, NeObject* neo) {
    // on regarde si cette variable faisait déjà partie de nos variables privatisées ou pas
    _Bool bo = false;
    for (ptrlist* ptr = variables_a_sauvegarder ; ptr != NULL && ptr->tete != NULL && !bo ; ptr = ptr->queue) {
        NeSave* ns = (NeSave*)ptr->tete;
        bo = bo || ns->object == neo;
    }

    if (!bo) { // alors il faut ajouter cette variable à nos variables privatisées
        NeSave* ns = err_malloc(sizeof(NeSave));
        ns->object = neo;
        ns->data = neo->data;
        ns->type = neo->type;
        ptrlist_append(variables_a_sauvegarder, (void*)ns);
    }
}




void switchGlobalLocalVariables(ptrlist* varsToSave) {
    /*
    Cette fonction sauvegarde la valeur des variables locales au moment où on arrive et restaure la valeur locale des variables locales
    */
    for (ptrlist* ptr = varsToSave ; ptr != NULL && ptr->tete != NULL ; ptr = ptr->queue) {
        NeSave* ns = (NeSave*)ptr->tete;

        // on va switcher entre la valeur stockée dans le nesave et la valeur actuelle
        NeObjDat data_temp = ns->data; // on garde une copie de la sauvegarde de notre variable
        char type_temp = ns->type;

        ns->data = ns->object->data; // on sauvegarde la valeur actuelle
        ns->type = ns->object->type;

        ns->object->data = data_temp; // on restaure la copie de notre variable
        ns->object->type = type_temp;
    }
}




ProcessCycle* nextProcess(ProcessCycle* pc, Stack** varstack, Stack** valstack, Stack** retstack, ptrlist** variables_a_sauvegarder) {
    
    if (pc->next != NULL) { // && pc->next != pc
        // on ne sauvegarde que si on change de processus
        // on garde en mémoire la valeur des ces objets

        pc->process->varsToSave = *variables_a_sauvegarder;
        switchGlobalLocalVariables(pc->process->varsToSave);

        Process* p = pc->next->process; // le nouveau processus que l'on va exécuter
        VAR_LOC = p->var_loc;
        *varstack = p->varstack;
        *valstack = p->valstack;
        *retstack = p->retstack;

        // on va maintenant restaurer les anciennes variables
        switchGlobalLocalVariables(pc->next->process->varsToSave);
        *variables_a_sauvegarder = pc->next->process->varsToSave;

        return pc->next;
    }

    return pc;
}






Process* processCycle_add(ProcessCycle* pc, Tree* tree, int id, void* label_endofstack) { // renvoie un pointeur vers le processus créé

    // on crée le nouveau processus
    Process* p = err_malloc(sizeof(Process));
    p->varstack = stack_create(VARSTACK_INITIAL_SIZE);
    p->valstack = stack_create(VALSTACK_INITIAL_SIZE);
    p->retstack = stack_create(RETSTACK_INITIAL_SIZE);
    p->var_loc = ptrlist_create();
    p->id = id;
    p->wait_expr = NULL;
    p->varsToSave = ptrlist_create();


    // on envoie évidemment les arguments
    stack_args_expr(tree, 1, p->varstack);

    // et on tapisse le fond de pile pour le retour
    stack_ret_zone(label_endofstack, p->retstack);

    // et maintenant on chaîne le processus au cycle des processus
    // trois cas de figure possibles
    if (pc->process == NULL && pc->next == NULL && pc->prev == NULL) {
        pc->process = p;
    }
    else if (pc->next == NULL && pc->prev == NULL) { // vu que c'est un cycle, la seule possibilité est qu'il n'y ait qu'un seul élément
        ProcessCycle* npc = processCycle_create();
        npc->process = p;

        pc->next = npc; npc->prev = pc;
        npc->next = pc; ; pc->prev = npc;
        // ça y est on a un cycle
    }
    else { // aucun des deux n'est nul, donc on s'insère entre les deux
        ProcessCycle* npc = processCycle_create();
        npc->process = p;

        npc->next = pc->next;
        pc->next->prev = npc;
        pc->next = npc;
        npc->prev = pc;
    }

    // du coup le prochain processus à exécuter dans la chaîne est le nouveau processus

    return p;

}


ProcessCycle* processCycle_remove(ProcessCycle* pc, Stack** varstack, Stack** valstack, Stack** retstack, ptrlist** variables_a_sauvegarder) {
    Process* p = pc->process;

    // on remet les variables d'avant ce processus comme il faut
    switchGlobalLocalVariables(p->varsToSave);

    
    if (p->id != -1) { // c'est pas thread principal
        err_free(p->var_loc); // on suppose que tous les contextes créés dans le cadre de ce processus ont bien été supprimés
    }


    stack_destroy(p->valstack);
    stack_destroy(p->varstack);
    stack_destroy(p->retstack);

    ptrlist_destroy(*variables_a_sauvegarder, true, true);

    err_free(p);

    ProcessCycle* next = pc->next;

    if (pc->next == NULL && pc->prev == NULL) {
        err_free(pc);
    }
    else if (pc->next == pc->prev) {// il y a uniquement deux processus
        err_free(pc);
        next->next = NULL;
        next->prev = NULL;
    }
    else {
        pc->prev->next = pc->next;
        pc->next->prev = pc->prev;
        err_free(pc);
    }

    if (next == NULL)
        return NULL;


    p = next->process;

    VAR_LOC = p->var_loc;
    *varstack = p->varstack;
    *valstack = p->valstack;
    *retstack = p->retstack;

    // on restaure les variables
    switchGlobalLocalVariables(p->varsToSave);
    *variables_a_sauvegarder = p->varsToSave;

    return next;

}




NeObject* execval(Tree* tree) {
    /*
    Cette fonction combine les deux fonctions exec et eval. Elle détecte le type de l'arbre et évalue l'expression si c'est une expression
    ou exécute le code si ce sont des instructions
    La valeur de retour de la fonction est un NeObject* dans le cas d'une expression et un intptr_t casté en NeObject* dans le cas de exec
    Cette fonction est entièrement écrite en impératif, sans aucun appel récursif afin de pouvoir implémenter un entrelacement et
    un changement de processus

    Pour effectuer un appel récursif, il faut empiler l'arbre à évaluer dans la pile d'arguments, et empiler l'adresse du label
    de retour, puis sauter au début de la boucle
    Celle-ci va trouver où il faut aller.
    Pour renvoyer une valeur, il faut l'empiler sur la pile de retour, puis appeler RETURN_BACK(). Cette macro va se charger de réduire
    la pile, d'enlever les arguments et de revenir à l'endroit précédent

    Pour l'entrelacement, il suffit de stocker les trois piles associées à chaque processus dans une liste ou un tableau,
    puis d'affecter les trois piles aux piles qu'il faut aux variables de piles. En effet, toute l'information (variables
    locales, arguments, retour...) est stockée dans ces piles. La boucle ne fait qu'interpréter la valeur des piles
    
    */

    // les variables utilisées avant et après des appels récursifs sont déclarées ici pour exister même après les goto
    NeObject * op1, * op2, *neo_liste, *expr, *var, *valeur, *nom, *args, *tempMax, *mainThreadReturnValue = NULL;
    NeObject* un = neo_nb_create(number_fromDouble(1));
    NeList* l, *liste, *val;
    Function* fun;
    intptr_t ext_index = 0, int_ret = 0, bloc = 0, inst = 0; // pour les boucles for qui contiennent des goto
    Tree* maintree = NULL, *arguments, *temptree;
    _Bool cond = false, elif = false;
    intptr_t max;
    void* sov_vars_to_save = NULL;

    intptr_t calcListIndex = 1;

    ptrlist* var_loc_sov = VAR_LOC;


    // création de la chaîne de processus
    ProcessCycle* process_cycle = processCycle_create();

    // ajout du processus principal, reconnaissable car il n'a pas de promesses
    processCycle_add(process_cycle, tree, -1, &&LABEL_ENDOFSTACK); // le processus principal a pour id : -1

    ptrlist* variables_a_sauvegarder = process_cycle->process->varsToSave;

    Stack* varstack = process_cycle->process->varstack;
    Stack* valstack = process_cycle->process->valstack;
    Stack* retstack = process_cycle->process->retstack;
    err_free(process_cycle->process->var_loc); // on utilise directement VAR_LOC de base pour le thread principal
    process_cycle->process->var_loc = VAR_LOC;
    
    intptr_t atomic_counter = 0;


    // boucle principale. On reste dedans tant que l'arbre entier n'a pas été exécuté (ou évalué)
    debut_boucle:

    
    // on est sûr que l'endroit où l'on reprend le processus est l'endroit où il avait été mis en pause
    if (atomic_counter == 0) { // c'est le moment où l'on change de processus, juste après un appel récursif
        // on profite d'abord de cette interruption pour vérifier la pile pour les ATOMIC_TIME prochains appels récursifs
        stack_check_for(retstack, ATOMIC_TIME);
        stack_check_for(valstack, ATOMIC_TIME);
        stack_check_for(varstack, 6 * ATOMIC_TIME); // le 6 correspond à l'incrément max de la pile en 1 tour de boucle principale

        // et on fait le travail de changement de processus
        process_cycle = nextProcess(process_cycle, &varstack, &valstack, &retstack, &variables_a_sauvegarder);

        atomic_counter = ATOMIC_TIME;

        if (process_cycle->process->wait_expr != NULL) { // le processus est actuellement en attente passive
            goto check_condition;
        }
    }
    else if (atomic_counter < 0) { // on est dans un atomic block, donc comme on ne va pas chanher de processus on vérifie à chaque tour
        stack_check_for(retstack, 1);
        stack_check_for(valstack, 1);
        stack_check_for(varstack, 6);
    }

    atomic_counter--;

    // récupération des arguments. On ne les pop pas afin de pouvoir les réutiliser
    tree = (Tree*)varstack->stack_ptr[0];
    calcListIndex = (intptr_t)varstack->stack_ptr[-1];

    LINENUMBER = tree->line;


    // disjonction de cas suivant si c'est une expression ou des instructions
    // on calcule en O(1) si notre type est une expression ou pas

    if (isExpression[tree->type]) {

        switch (tree->type) {

            case TYPE_OPERATOR:
            {
                // a partir d'ici, on n'utilise plus que les appellations de variables locales

                if (!LOCAL_OP_calcListIndex_c && LOCAL_OP_tree_c->label2 != 26) {
                    CODE_ERROR = 103;
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }

                // il faut envoyer les opérandes calculées à la fonction opérateur
                if (LOCAL_OP_tree_c->nbSons == 1) // operateur unaire
                {

                    if (LOCAL_OP_tree_c->label2 == 39) { // opérateur parallel
                        // C'est un appel de fonction déguisé. On va appeler la fonction mais dans un nouveau thread
                        
                        maintree = LOCAL_OP_tree_c->sons[0]; // cet arbre est de type TYPE_FONCTION

                        // il faut évaluer la fonction donnée à parallel, donc le premier fils de maintree afin de vérifier si la fonction dedans est une userfunc
                        
                        stack_args_expr(maintree->sons[0], 1, varstack);
                        stack_ret_zone(&&LABEL_PARALLEL0, retstack);
                        goto debut_boucle;
                        LABEL_PARALLEL0:

                        maintree = LOCAL_OP_tree_c->sons[0];

                        if (CODE_ERROR != 0) { // erreur lors de l'évaluation de la fonction
                            RETURN_BACK();
                        }


                        // on crée un clone de maintree, mais dans lequel la fonction à exécuter est fixée comme étant celle que l'on vient d'évaluer
                        // permet de résoudre le problème de nw.ne
                        Tree* maintree_copy = (Tree*) err_malloc(sizeof(Tree));
                        maintree_copy->type = maintree->type;
                        maintree_copy->label1 = maintree->label1;
                        maintree_copy->label2 = maintree->label2;
                        maintree_copy->sons = maintree->sons;
                        maintree_copy->nbSons = maintree->nbSons;
                        maintree_copy->capacity = maintree->capacity;
                        maintree_copy->data = (NeObject*)stack_pop(valstack);


                        if (maintree_copy->data->type != TYPE_USERFUNC) {
                            CODE_ERROR = 100;
                            neobject_destroy(maintree_copy->data, true);
                            return_neo_execval(valstack, NULL);
                            RETURN_BACK();
                        }


                        // calcul de l'identifiant du processus
                        int id = 0;

                        for (; id < PROMISES->len && (PROCESS_FINISH.tab[id] != 1 || PROMISES_CNT.tab[id] > 0) ; id++);
                        
                        if (id >= PROMISES->len) { // l'identifiant est un nouvel identifiant
                            nelist_append(PROMISES, NULL);
                            intlist_append(&PROMISES_CNT, 0);
                            intlist_append(&PROCESS_FINISH, 0);
                        }

                        
                        PROMISES->tab[id] = NULL;
                        PROMISES_CNT.tab[id] = 1;
                        PROCESS_FINISH.tab[id] = 0;
                        

                        // création de trois toutes nouvelles varstack, valstack et retstack

                        Process* p = processCycle_add(process_cycle, maintree_copy, id, &&LABEL_ENDOFSTACK);
                        // ce nouveau processus est lancé à partir d'un arbre qui n'est pas l'arbre original, car la fonction a exécuter est déjà spécifiée.
                        // ce nouvel arbre aura besoin d'être libéré


                        // on ajoute un élément sur la pile après l'arbre a exécuter et le calclistindex, pour sauvegarder maintree_copy afin de le libérer après
                        stack_check_for(p->varstack, 1);
                        stack_push(p->varstack, p->varstack->stack_ptr[0]); // le dernier élément a été décalé
                        p->varstack->stack_ptr[-1] = p->varstack->stack_ptr[-2]; // le calcListIndex a lui aussi été décalé

                        // l'avant avant dernier élément (après tree et calcListIndex) est notre copie
                        p->varstack->stack_ptr[-2] = (void*)maintree_copy; // on garde cette fonction dans la pile du nouveau processus créé

                        stack_ret_zone(&&LABEL_PARALLEL, p->retstack);

                        // on a ajouté le processus à la liste de processus, donc maintenant y a plus qu'à attendre qu'un jour on revienne en LABEL_PARALLEL

                        return_neo_execval(valstack, neo_promise_create((intptr_t)id)); // plus tard, ce sera une promesse renvoyée par le parallel
                        
                        RETURN_BACK(); // on a terminé de préparer le lancement du processus, on continue donc l'exécution
                        LABEL_PARALLEL: // quand le processus sera fini on sera ici

                        err_free(stack_pop(varstack)); // on libère maintree_copy

                        
                        id = process_cycle->process->id; // on a fait plein de trucs donc il faut reprendre le bon id


                        // si on se retrouve ici c'est que le processus actuellement exécuté est notre processus, donc
                        // process_cycle->process est notre processus
                        // maintenant on supprime le processus, et on affecte les variables à affecter
                        NeObject* result = (NeObject*)stack_pop(valstack);

                        // le résultat est mis ici en attente, et neo_copy se chargera de vérifier si la valeur a changé ou pas
                        if (PROMISES_CNT.tab[id] != 0) { // si c'est zéro, je sais que toutes les promesses ont été détruites avant même de les utiliser
                            PROMISES->tab[id] = result;
                        }
                        else {
                            neobject_destroy(result, true);
                        }
                        PROCESS_FINISH.tab[id] = 1; // on a fini le processus, on libère la place



                        // maintenant, suppression du processus
                        process_cycle = processCycle_remove(process_cycle, &varstack, &valstack, &retstack, &variables_a_sauvegarder);
                        
                        // ici, on se rend compte que le data de la fonction est toujours celui de la copie du processus
                        // alors même que la suppression du processus devait restaurer les variables utilisées

                        atomic_counter = ATOMIC_TIME; // on donne du temps au nouveau processus, évidemment


                        if (process_cycle == NULL) // on vient de finir le dernier processus
                        {
                            neobject_destroy(un, true);
                            VAR_LOC = var_loc_sov;

                            return mainThreadReturnValue;
                        }


                        if (process_cycle->process->wait_expr != NULL) // le processus est actuellement en attente passive
                        {
                            goto check_condition;
                        }
                        else
                        {
                            if (CODE_ERROR != 0) {
                                // on était arrivé dans un nouveau processus qui venait tout juste de lancer un appel récursif
                                // pas d'arguments à enlever, puisque maintenant on conserve tree et calcListIndex et on les enlève tout à la fin
                                return_neo_execval(valstack, NULL);
                                RETURN_BACK();
                            }
                            // cette fonction a également chargé le nouveau processus dans les variables et a renvoyé le nouveau processus
                            goto debut_boucle; // on continue l'exécution sur le processus d'après
                        }


                    }




                    // récupération de la grammaire de l'opérateur
                    if (OPERANDES.tab[LOCAL_OP_tree_c->label2] == VARRIGHT || OPERANDES.tab[LOCAL_OP_tree_c->label2] == VARLEFT)
                    {
                        stack_args_expr(LOCAL_OP_tree_c->sons[0], 0, varstack); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
                        stack_ret_zone(&&LABEL_OP1, retstack);
                        // on peut lancer l'appel récursif
                        goto debut_boucle;
                        LABEL_OP1: // on revient ici après le calcul

                        op1 = (NeObject*)stack_pop(valstack);
                        
                        if (CODE_ERROR != 0) {
                            return_neo_execval(valstack, NULL);
                            RETURN_BACK();
                        }
                        
                    }
                    else
                    {
                        stack_args_expr(LOCAL_OP_tree_c->sons[0], 1, varstack);
                        stack_ret_zone(&&LABEL_OP2, retstack);
                        goto debut_boucle;
                        LABEL_OP2:

                        op1 = (NeObject*)stack_pop(valstack);

                        if (CODE_ERROR != 0) {
                            return_neo_execval(valstack, NULL);
                            RETURN_BACK();
                        }
                    }

                    NeObject* retour;


                    // l'opérateur @ a le droit à un traitement spécial car suivant calcListIndex, il renvoie l'objet ou une copie de l'objet
                    if (LOCAL_OP_tree_c->label2 == 26) { // opérateur @

                        if (op1->type != TYPE_STRING) {
                            CODE_ERROR = 60; // ceci n'est pas une chaine de caractères
                            neobject_destroy(op1, true);
                            return_neo_execval(valstack, NULL);
                            RETURN_BACK();
                        }
                        char* nom=neo_to_string(op1);

                        if(!strlist_inList(NOMS, nom))
                        {
                            CODE_ERROR = 5;
                            neobject_destroy(op1, true);
                            return_neo_execval(valstack, NULL);
                            RETURN_BACK();
                        }
                            
                        int index = strlist_index(NOMS,nom);
                        
                        if (LOCAL_OP_calcListIndex_c)
                            return_neo_execval(valstack, neo_copy(ADRESSES->tab[index]));
                        else
                            return_neo_execval(valstack, ADRESSES->tab[index]);
                        
                        neobject_destroy(op1, true);
                        RETURN_BACK();
                    }
                    else {
                        retour = OPFONC[LOCAL_OP_tree_c->label2](op1,un);
                    }


                    if (CODE_ERROR != 0)
                    {
                        if (OPERANDES.tab[LOCAL_OP_tree_c->label2] != VARRIGHT && OPERANDES.tab[LOCAL_OP_tree_c->label2] != VARLEFT)
                            neobject_destroy(op1, true);
                        
                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }
                    
                    
                    if (OPERANDES.tab[LOCAL_OP_tree_c->label2] != VARRIGHT && OPERANDES.tab[LOCAL_OP_tree_c->label2] != VARLEFT)  // on ne libère op1 que si op1 n'est pas directement d'adresse d'une variable
                        neobject_destroy(op1,true);

                                            
                    
                    return_neo_execval(valstack, retour);
                    RETURN_BACK();
                }
                else if (LOCAL_OP_tree_c->nbSons == 2) // operateur binaire
                {

                    if (LOCAL_OP_tree_c->label2 == 37) { // opérateur :=
                        CODE_ERROR = 95;
                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    } else if (LOCAL_OP_tree_c->label2 == 35) { // opérateur :
                        CODE_ERROR = 92;
                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }
                    
                    
                    if (OPERANDES.tab[LOCAL_OP_tree_c->label2] == VAR_RIGHT)
                    {
                        stack_args_expr(LOCAL_OP_tree_c->sons[0], 0, varstack);
                        stack_ret_zone(&&LABEL_OP3, retstack);
                        goto debut_boucle;
                        LABEL_OP3:

                        if (CODE_ERROR != 0) {

                            // comme on ne fait pas de pop (on ne récupère pas la valeur, c'est comme si on récupérait la valeur et qu'on la renvoyait direct)
                            RETURN_BACK();
                        }
                        
                        stack_args_expr(LOCAL_OP_tree_c->sons[1], 1, varstack);
                        stack_ret_zone(&&LABEL_OP4, retstack);
                        goto debut_boucle;
                    }
                    else if (OPERANDES.tab[LOCAL_OP_tree_c->label2] == LEFT_VAR)
                    {                    
                        stack_args_expr(LOCAL_OP_tree_c->sons[0], 1, varstack);
                        stack_ret_zone(&&LABEL_OP5, retstack);
                        goto debut_boucle;
                        LABEL_OP5:

                        if (CODE_ERROR != 0) {

                            RETURN_BACK();
                        }
                        
                        stack_args_expr(LOCAL_OP_tree_c->sons[1], 0, varstack);
                        stack_ret_zone(&&LABEL_OP4, retstack);
                        goto debut_boucle;
                    }
                    else if (OPERANDES.tab[LOCAL_OP_tree_c->label2] == VAR_VAR)
                    {
                        stack_args_expr(LOCAL_OP_tree_c->sons[0], 0, varstack);
                        stack_ret_zone(&&LABEL_OP6, retstack);
                        goto debut_boucle;
                        LABEL_OP6:

                        if (CODE_ERROR != 0) {
                            return_neo_execval(valstack, NULL);
                            RETURN_BACK();
                        }
                        
                        stack_args_expr(LOCAL_OP_tree_c->sons[1], 0, varstack);
                        stack_ret_zone(&&LABEL_OP4, retstack);
                        goto debut_boucle;
                    }
                    else
                    {
                        stack_args_expr(LOCAL_OP_tree_c->sons[0], 1, varstack);
                        stack_ret_zone(&&LABEL_OP7, retstack);

                        goto debut_boucle;
                        LABEL_OP7:

                        if (CODE_ERROR != 0)
                        {
                            // on dépile tree et calcListIndex

                            op1 = (NeObject*)stack_pop(valstack);
                            neobject_destroy(op1, true);
                            return_neo_execval(valstack, NULL);
                            RETURN_BACK();
                        }

                        stack_args_expr(LOCAL_OP_tree_c->sons[1], 1, varstack);
                        stack_ret_zone(&&LABEL_OP4, retstack);
                        goto debut_boucle;
                    }
                    
                    LABEL_OP4:

                    op2 = (NeObject*)stack_pop(valstack);
                    op1 = (NeObject*)stack_pop(valstack);


                    if (CODE_ERROR != 0)
                    {
                        if (OPERANDES.tab[LOCAL_OP_tree_c->label2] != VAR_RIGHT && OPERANDES.tab[LOCAL_OP_tree_c->label2] != VAR_VAR)
                            neobject_destroy(op1, true);
                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }

                    
                    NeObject* retour = OPFONC[LOCAL_OP_tree_c->label2](op1, op2);

                    
                    if (CODE_ERROR != 0)
                    {
                        if (OPERANDES.tab[LOCAL_OP_tree_c->label2] != VAR_RIGHT && OPERANDES.tab[LOCAL_OP_tree_c->label2] != VAR_VAR)
                            neobject_destroy(op1, true);
                        neobject_destroy(op2, true);
                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }


                    if (OPERANDES.tab[LOCAL_OP_tree_c->label2] != VAR_RIGHT && OPERANDES.tab[LOCAL_OP_tree_c->label2] != VAR_VAR) // on ne libère op1 que si op1 n'est pas directement d'adresse d'une variable
                        neobject_destroy(op1,true);
                    
                    if (OPERANDES.tab[LOCAL_OP_tree_c->label2] != LEFT_VAR && OPERANDES.tab[LOCAL_OP_tree_c->label2] != VAR_VAR)
                    {
                        neobject_destroy(op2,true);
                    }

                    
                    return_neo_execval(valstack, retour);
                    RETURN_BACK();
                }
                
            }
            
            // s'il n'y a qu'un seul élément final, on le calcule (traiterOperande)
            
            case TYPE_NUMBER:
            {
                if (!calcListIndex)
                {
                    CODE_ERROR = 89;
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }
                else {
                    return_neo_execval(valstack, neo_copy(tree->data));
                    RETURN_BACK();
                }
            }
            
            
            case TYPE_VARIABLE:
            {
                if (calcListIndex)
                {
                    if (tree->data->type == TYPE_EMPTY)
                    {
                        CODE_ERROR = 5;
                        return_neo_execval(valstack, NULL);
                    }
                    else {
                        return_neo_execval(valstack, neo_copy(tree->data));
                    }
                    RETURN_BACK();
                }
                else
                {
                    update_if_promise(tree->data);

                    return_neo_execval(valstack, tree->data);
                    RETURN_BACK();
                }
                

            }
            
            case TYPE_LIST:
            {
                if (!calcListIndex) // erreur car sinon ça fait une fuite mémoire
                {
                    CODE_ERROR = 89;
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }

                // donc les enfants de tree sont les éléments de la liste
                l = nelist_create(tree->nbSons);
                ext_index = 0;

                //stack_check_for(varstack, 4);

                for (; ext_index < l->len ; ext_index++) {
                    stack_push(varstack, (void*)l);
                    stack_push(varstack, (void*)ext_index);
                    stack_push(varstack, (void*)calcListIndex);
                    stack_push(varstack, (void*)tree);

                    stack_args_expr(tree->sons[ext_index], 1, varstack);
                    stack_ret_zone(&&LABEL_LIST, retstack);
                    goto debut_boucle;
                    LABEL_LIST:

                    tree = (Tree*)stack_pop(varstack);
                    calcListIndex = (intptr_t)stack_pop(varstack);
                    ext_index = (intptr_t)stack_pop(varstack);
                    l = (NeList*)stack_pop(varstack);

                    l->tab[ext_index] = (NeObject*)stack_pop(valstack);
                }


                if (CODE_ERROR != 0)
                {
                    err_free(l->tab);
                    err_free(l);                        
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }

                return_neo_execval(valstack, neo_list_convert(l));
                RETURN_BACK();
            }
            
            case TYPE_FONCTION:
            {
                if (!calcListIndex)
                {
                    CODE_ERROR = 103;
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }

                // ici, appel récursif pour évaluer la fonction (1er fils), et on la met dans ->data de l'arbre


                // si le champ data de la fonction est déjà occuppé, on n'a pas besoin d'évaluer la fonction
                if (tree->data == NULL) {
                    stack_push(varstack, (void*)calcListIndex);
                    stack_push(varstack, (void*)tree);

                    stack_args_expr(tree->sons[0], 1, varstack);
                    stack_ret_zone(&&LABEL_FUNCTION0, retstack);
                    goto debut_boucle;
                    LABEL_FUNCTION0:

                    tree = (Tree*)stack_pop(varstack);
                    calcListIndex = (intptr_t)stack_pop(varstack);

                    tree->data = (NeObject*)stack_pop(valstack); // on le stocke temporairement dans tree->data, mais ça n'est pas un stockage pérenne, car sur des appels récursifs, ca va écraser la valeur
                    
                    if (CODE_ERROR != 0) { // erreur lors de l'évaluation de la fonction
                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }
                
                }


                // ensuite, pour évaluer les arguments, on fait bien la boucle sur tree->sons[1]
                
                if (tree->data->type == TYPE_FONCTION)
                {
                    
                    l = nelist_create(tree->sons[1]->nbSons);

                    //stack_check_for(varstack, 5);

                    ext_index = 0;
                    
                    fun = tree->data->data.ptr;

                    for (; (ext_index < fun->nbArgsMeth && ext_index < tree->sons[1]->nbSons) || (fun->nbArgsMeth == -1 && ext_index < tree->sons[1]->nbSons) ; ext_index++)
                    {

                        stack_push(varstack, (void*)l);
                        stack_push(varstack, (void*)ext_index); // si on modifie la variable en tant que variable locale, il faut la modifier au bon endroit
                        stack_push(varstack, (void*)tree->data); // on l'empile, car sinon dans tree->data ça écrase
                        tree->data = NULL; // pour que les appels futurs n'oublient pas d'évaluer la fonction. La valeur sera restaurée juste après au retour
                        
                        stack_push(varstack, (void*)calcListIndex);
                        stack_push(varstack, (void*)tree);

                        stack_args_expr(tree->sons[1]->sons[ext_index], 0, varstack);
                        stack_ret_zone(&&LABEL_FUNCTION1, retstack);
                        goto debut_boucle;
                        LABEL_FUNCTION1:

                        tree = (Tree*)stack_pop(varstack);
                        calcListIndex = (intptr_t)stack_pop(varstack);                    
                        tree->data = (NeObject*)stack_pop(varstack);
                        ext_index = (intptr_t)stack_pop(varstack);
                        l = (NeList*)stack_pop(varstack);

                        fun = tree->data->data.ptr;

                        l->tab[ext_index] = (NeObject*)stack_pop(valstack);


                        if (CODE_ERROR != 0) {
                            err_free(l->tab);
                            err_free(l);
                            neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                            tree->data = NULL;
                            return_neo_execval(valstack, NULL);
                            RETURN_BACK();
                        }
                        
                        if (l->tab[ext_index]->type == TYPE_EMPTY)
                        {
                            CODE_ERROR = 5;
                            err_free(l->tab);
                            err_free(l);
                            neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                            tree->data = NULL;
                            return_neo_execval(valstack, NULL);
                            RETURN_BACK();
                        }
                    }


                    for (;ext_index < tree->sons[1]->nbSons ; ext_index++)
                    {
                        //stack_check_for(varstack, 5);
                        stack_push(varstack, (void*)ext_index);
                        stack_push(varstack, (void*)tree->data);
                        stack_push(varstack, (void*)l);
                        tree->data = NULL; // pour que les appels futurs n'oublient pas d'évaluer la fonction

                        stack_push(varstack, (void*)calcListIndex);
                        stack_push(varstack, (void*)tree);

                        stack_args_expr(tree->sons[1]->sons[ext_index], 1, varstack);
                        stack_ret_zone(&&LABEL_FUNCTION2, retstack);
                        goto debut_boucle;
                        LABEL_FUNCTION2:

                        tree = (Tree*)stack_pop(varstack);
                        calcListIndex = (intptr_t)stack_pop(varstack);
                        l = (NeList*)stack_pop(varstack);
                        tree->data = (NeObject*)stack_pop(varstack);
                        ext_index = (intptr_t)stack_pop(varstack);


                        fun = tree->data->data.ptr;

                        l->tab[ext_index] = (NeObject*)stack_pop(valstack);
                        
                        if (CODE_ERROR != 0)
                        {
                            // si y a eu un problème dans l'évaluation d'un argument, on doit libérer toute la liste créée jusqu'alors
                            for (int j = fun->nbArgsMeth ; j <= ext_index ; j++) // on ne détruit que les éléments que l'on a copiés
                                neobject_destroy(l->tab[j],true);
                            
                            err_free(l->tab);
                            err_free(l);

                            neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                            tree->data = NULL;
                            
                            return_neo_execval(valstack, NULL);
                            RETURN_BACK();
                        }
                        
                    }

                    args = neo_list_convert(l);
                    

                    if (CODE_ERROR != 0)
                    {
                        neo_list_free(args); // supprime args, mais pas ses éléments

                        neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                        tree->data = NULL;
                        
                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }
            
                    if (!funcArgsCheck(fun, args->data.ptr))
                    {
                        if (fun->nbArgsMeth != -1)
                        {
                            for (int i=fun->nbArgsMeth ; i < neo_list_len(args) ; i++) // on ne détruit que les éléments que l'on a copiés
                            {
                                neobject_destroy(neo_list_nth(args,i),true);
                            }
                        }
            
                        neo_list_free(args); // supprime args, mais pas ses éléments

                        neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                        tree->data = NULL;
                        
                        CODE_ERROR = 14;
                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }
            
                    
                    NeObject* retour = functionCall(tree->data,(NeList*)args->data.ptr);

                    
                    if (CODE_ERROR != 0)
                    {
                        if (fun->nbArgsMeth != -1)
                        {
                            for (int i=fun->nbArgsMeth ; i < neo_list_len(args) ; i++) // on ne détruit que les éléments que l'on a copiés
                            {
                                neobject_destroy(neo_list_nth(args,i),true);
                            }
                        }
            
                        neo_list_free(args); // supprime args, mais pas ses éléments

                        neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                        tree->data = NULL;

                        neobject_destroy(retour, true);
                        
                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }
                    
                    if (fun->nbArgsMeth != -1)
                    {
                        for (int i=fun->nbArgsMeth ; i < neo_list_len(args) ; i++) // on ne détruit que les éléments que l'on a copiés
                        {
                            neobject_destroy(neo_list_nth(args,i),true);
                        }
                    }
            
            
                    neo_list_free(args); // supprime args, mais pas ses éléments

                    neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient d'exécuter
                    tree->data = NULL; // pour bien indiquer qu'il faut réévaluer la fonction la prochaine fois qu'on passe par là


                    return_neo_execval(valstack, retour);
                    RETURN_BACK();
                }

                else if (tree->data->type == TYPE_USERFUNC || tree->data->type == TYPE_USERMETHOD)
                {

                    UserFunc* fun = tree->data->data.ptr;

                    if (tree->sons[1]->nbSons > fun->nbArgs && ! fun->unlimited_arguments)
                    {
                        CODE_ERROR = 6;
                        neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                        tree->data = NULL;

                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }


                    // on compte le nombre d'arguments très facultatifs
                    int given_nbOptArgs = 0;

                    for (int i = 0 ; i < tree->sons[1]->nbSons ; i++) {
                        if (tree->sons[1]->sons[i]->type == TYPE_OPERATOR && tree->sons[1]->sons[i]->label2 == 37) { // on est sur du :=
                            int index = 0;
                            while (index < fun->nbArgs && fun->args[index] != tree->sons[1]->sons[i]->sons[0]->data) index++;

                            if (index >= fun->nbArgs - fun->nbOptArgs && index < fun->nbArgs)
                                given_nbOptArgs ++;
                        }
                    }

                    
                    // on sait maintenant que le nombre d'arguments réel final est :
                    // -> Si tree->nbSons > fun->nbArgs : tree->nbSons - given_nbOptArgs
                    // -> Sinon, fun->nbArgs



                    // on va commencer par construire un tableau d'expressions à évaluer (un tableau d'arbres) à partir des arguments qu'on nous donne
                    arguments = err_malloc(sizeof(Tree));
                    arguments->nbSons = (fun->unlimited_arguments && tree->sons[1]->nbSons > fun->nbArgs - fun->nbOptArgs) ? tree->sons[1]->nbSons + fun->nbOptArgs - given_nbOptArgs : fun->nbArgs;
                    arguments->sons = err_malloc(sizeof(Tree*) * arguments->nbSons);
                    

                    // on va faire un tableau de booléens qui compte si on donne la valeur des arguments très optionnels (après ...)

                    
                    for (int i = 0 ; i < arguments->nbSons ; i++) arguments->sons[i] = NULL; // pour savoir s'il y a des problèmes



                    // on peut commencer par mettre à leur valeur par défaut les variables définies après ...
                    // Ainsi elles ne seront pas remplacés sauf si on l'indique explicitement
                    for (int i = fun->nbArgs - fun->nbOptArgs ; i < fun -> nbArgs ; i++) {
                        arguments->sons[i] = fun->opt_args->sons[i];
                    }


                    // 1ère boucle pour placer les arguments à des endroits où ils ne bougeront pas
                    for (int i = 0 ; i < tree->sons[1]->nbSons ; i++) {
                        // on commence par remplir les éléments donnés dans le mauvais ordre (avec :=)
                        if (tree->sons[1]->sons[i]->type == TYPE_OPERATOR && tree->sons[1]->sons[i]->label2 == 37) { // on est sur du :=
                            // on regarde la variable de gauche, et on met le truc au bon endroit
                            // on récupère l'index du vrai argument dans fun->args, et on le met au bon endroit dans arguments
                            int index = 0;

                            if (tree->sons[1]->sons[i]->sons[0]->type != TYPE_VARIABLE) {
                                err_free(arguments->sons);
                                err_free(arguments);
                                neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                                tree->data = NULL;
                                CODE_ERROR = 93;
                                return_neo_execval(valstack, NULL);
                                RETURN_BACK();
                            }

                            while (index < fun->nbArgs && fun->args[index] != tree->sons[1]->sons[i]->sons[0]->data) index++;

                            if (index == fun->nbArgs) {
                                err_free(arguments->sons);
                                err_free(arguments);
                                neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                                tree->data = NULL;
                                CODE_ERROR = 93;
                                return_neo_execval(valstack, NULL);
                                RETURN_BACK();
                            }
                            
                            if (index < fun->nbArgs - fun->nbOptArgs && arguments->sons[index] != NULL) {
                                err_free(arguments->sons);
                                err_free(arguments);
                                neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                                tree->data = NULL;
                                CODE_ERROR = 94;
                                return_neo_execval(valstack, NULL);
                                RETURN_BACK();
                            }
                            arguments->sons[index] = tree->sons[1]->sons[i]->sons[1];
                        }
                    }

                    // deuxième boucle pour insérer dans l'ordre et en évitant les := les arguments restants
                    int index = 0; // on remplit petit à petit arguments avec les arguments restants
                    while (index < arguments->nbSons && arguments->sons[index] != NULL) index++;

                    for (int i = 0 ; index < arguments->nbSons && i < tree->sons[1]->nbSons ; i++) {
                        
                        if (tree->sons[1]->sons[i]->type != TYPE_OPERATOR || tree->sons[1]->sons[i]->label2 != 37) {
                            arguments->sons[index] = tree->sons[1]->sons[i];
                            while (index < arguments->nbSons && arguments->sons[index] != NULL) index++;
                        }
                    }
                    // c'est un choix philosophique : si f attend x, y facultatif et z, alors f(1,2) ne fonctionnera pas car il manque z
                    // pour mettre uniquement le x et le z, il y a une syntaxe pour le faire

                    if (tree->sons[1]->nbSons < fun->nbArgs) {
                        // à ce stade-là, certains champs de arguments sont encore vides, on va donc parcourir une dernière fois afin de lui associer les expressions définies lors de la définition de la fonction
                        for (int i = 0 ; i < fun->nbArgs ; i++) {
                            if (arguments->sons[i] == NULL) { // c'est précisément le cas où l'utilisateur n'a rien mis
                                arguments->sons[i] = fun->opt_args->sons[i];
                                if (tree_isEmpty(arguments->sons[i])) {
                                    CODE_ERROR = 7;
                                    neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                                    tree->data = NULL;
                                    err_free(arguments->sons);
                                    err_free(arguments);
                                    return_neo_execval(valstack, NULL);
                                    RETURN_BACK();
                                }
                            }
                        }
                    }

                    // maintenant, normalement arguments est un tableau plein d'arbres, on peut donc l'évaluer

                    // calcul des arguments et affectation des arguments
                    liste = nelist_create(arguments->nbSons);

                    // variables à sauvegarder : fun, liste, ext_index et arguments


                    for (ext_index = 0 ; ext_index < liste->len ; ext_index++) {
                        
                        //stack_check_for(varstack, 6);
                        
                        stack_push(varstack, (void*)tree->data);
                        stack_push(varstack, (void*)liste);
                        stack_push(varstack, (void*)ext_index);
                        stack_push(varstack, (void*)arguments);

                        stack_push(varstack, (void*)calcListIndex);
                        stack_push(varstack, (void*)tree);

                        if (ext_index == 0 && tree->data->type == TYPE_USERMETHOD) {
                            stack_args_expr(arguments->sons[ext_index], 0, varstack);
                        }
                        else {
                            stack_args_expr(arguments->sons[ext_index], 1, varstack);
                        }

                        tree->data = NULL; // pour que les appels futurs n'oublient pas d'évaluer la fonction

                        stack_ret_zone(&&LABEL_FUNCTION3, retstack);
                        goto debut_boucle;
                        LABEL_FUNCTION3:

                        tree = (Tree*)stack_pop(varstack);
                        calcListIndex = (intptr_t)stack_pop(varstack);
                        arguments = (Tree*)stack_pop(varstack);
                        ext_index = (intptr_t)stack_pop(varstack);
                        liste = (NeList*)stack_pop(varstack);
                        tree->data = (NeObject*)stack_pop(varstack);


                        fun = (UserFunc*)(tree->data)->data.ptr;

                        liste->tab[ext_index] = (NeObject*)stack_pop(valstack);

                        if (CODE_ERROR != 0) {
                            if (tree->data->type != TYPE_USERMETHOD)
                                neobject_destroy(liste->tab[0], true);

                            for (int k = 1 ; k <= ext_index ; k++)
                                neobject_destroy(liste->tab[k], true);

                            err_free(liste->tab);
                            err_free(liste);
                            err_free(arguments->sons);
                            neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                            tree->data = NULL;
                            err_free(arguments);
                            return_neo_execval(valstack, NULL);
                            RETURN_BACK();
                        }

                    }
                    
                    err_free(arguments->sons);
                    err_free(arguments);

                    args = neo_list_convert(liste);


                    if (CODE_ERROR != 0)
                    {
                        neo_list_free(args); // supprime args, mais pas ses éléments
                        neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                        tree->data = NULL;
                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }


                    // ouvre un nouveau contexte pour sauvegarder les variables locales de cet appel
                    newContext();


                    // on sauvegarde les "variables à sauvegarder" de ce processus avant d'en ajouter d'autres
                    sov_vars_to_save = variables_a_sauvegarder->tete;

                    if (fun->unlimited_arguments) {
                        // on raccourcit args, et on crée la liste locale
                        int len = (neo_list_len(args) - given_nbOptArgs > fun->nbArgs - fun->nbOptArgs) ? neo_list_len(args) - fun->nbArgs : 0;

                        NeList* local_args = nelist_create(len);
                        for (int i = 0 ; i < len ; i++) {
                            local_args->tab[i] = neo_list_nth(args, fun->nbArgs + i);
                        }
                        NeObject* neo_local_args = neo_list_convert(local_args);


                        int index = strlist_index(NOMS, "__local_args__");
                        if (index < 0) {
                            CODE_ERROR = 0; // strlist_index a modifié le code d'erreur en renvoyant -1
                            strlist_append(NOMS, strdup("__local_args__"));
                            nelist_append(ADRESSES, neo_local_args);
                        }
                        else {

                            save_later(variables_a_sauvegarder, ADRESSES->tab[index]);
                            local(ADRESSES->tab[index]);
                            // on lui dit de  sauvegarder cette variable avant de switcher de processus

                            ADRESSES->tab[index]->data = neo_local_args->data;
                            ADRESSES->tab[index]->type = neo_local_args->type;
                            err_free(neo_local_args);
                        }

                        if (len > 0) // du coup on enlève les éléments en trop de args
                            ((NeList*)args->data.ptr)->len = fun->nbArgs;
                    }
                    
                    

                    if (tree->data->type == TYPE_USERMETHOD)
                    {
                        // affectation de la variable de méthode
                        if (fun->nbArgs > 0)
                        {
                            NeObject* c = neo_copy(neo_list_nth(args, 0)); // on crée une copie de l'argument
                            // on empile la valeur actuelle de l'argument pour avoir champ libre dans cette fonction
                            
                            save_later(variables_a_sauvegarder, fun->args[0]);
                            local(fun->args[0]);

                            fun->args[0]->data = c->data;
                            fun->args[0]->type = c->type;
                            err_free(c);
                        }
                        
                        for (int i = 1 ; i < fun->nbArgs ; i++) // affectation des arguments
                        {
                            NeObject* temp = neo_copy(neo_list_nth(args, i));
                            
                            save_later(variables_a_sauvegarder, fun->args[i]);
                            local(fun->args[i]);
                            
                            fun->args[i]->data = temp->data;
                            fun->args[i]->type = temp->type;


                            err_free(temp);
                            
                            if (CODE_ERROR != 0) // erreur : libération des arguments affectés
                            {
                                
                                // libérer la sauvegarde des variables
                                //deleteContext();
                                
                                ((NeList*)args->data.ptr)->tab[0] = neo_none_create();
                                neobject_destroy(args, true);
                                // libération des arguments
                                for (int j = 1 ; j < i ; j++)
                                    neobject_destroy(fun->args[j], false);

                                deleteContext();

                                neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                                tree->data = NULL;

                                return_neo_execval(valstack, NULL);
                                RETURN_BACK();
                            }
                        }
                    }
                    else
                    {
                        for (int i = 0 ; i < fun->nbArgs ; i++) // affectation des arguments
                        {
                            NeObject* temp = neo_copy(neo_list_nth(args, i));
                            
                            save_later(variables_a_sauvegarder, fun->args[i]);
                            local(fun->args[i]); // on sauvegarde l'ancienne valeur de la variable avant de l'affecter
                            
                            fun->args[i]->data = temp->data;
                            fun->args[i]->type = temp->type;

                            err_free(temp);
                            
                            if (CODE_ERROR != 0) // erreur : libération des arguments affectés
                            {
                                // libération des sauvegardes de variables
                                //deleteContext();
                                
                                neobject_destroy(args, true);
                                // libération des arguments
                                for (int j = 0 ; j < i ; j++)
                                    neobject_destroy(fun->args[j], false);

                                deleteContext();

                                neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                                tree->data = NULL;
                                
                                return_neo_execval(valstack, NULL);
                                RETURN_BACK();
                            }
                            
                        }
                    }

                    // on stocke fun et args

                    //stack_check_for(varstack, 5);

                    stack_push(varstack, (void*)tree->data);
                    stack_push(varstack, (void*)args);
                    stack_push(varstack, (void*)sov_vars_to_save);
                    tree->data = NULL; // pour que les appels futurs n'oublient pas d'évaluer la fonction

                    stack_push(varstack, (void*)calcListIndex);
                    stack_push(varstack, (void*)tree);

                    stack_args_expr(fun->code, 0, varstack);
                    stack_ret_zone(&&LABEL_FUNCTION4, retstack);


                    goto debut_boucle; // exécute le corps de la fonction
                    LABEL_FUNCTION4:


                    tree = (Tree*)stack_pop(varstack);
                    calcListIndex = (intptr_t)stack_pop(varstack);
                    sov_vars_to_save = (void*)stack_pop(varstack);
                    args = (NeObject*)stack_pop(varstack);
                    tree->data = (NeObject*)stack_pop(varstack);

                    
                    fun = (UserFunc*)(tree->data)->data.ptr;

                    int_ret = (intptr_t)stack_pop(valstack);

                    if (CODE_ERROR != 0) // erreur
                    {
                        // supprime les valeurs des variables locales de la fonction
                        // il n'y a pas besoin de le faire car on va déjà supprimer les objets en restaurant les variables sauvegardées avec local
                        // peut etre que dans le cas d'une méthode il faut quand même supprimer le premier élément
                        /*for (int i = 0 ; i < fun->nbArgs ; i++) {
                            neobject_destroy(fun->args[i], false);
                        }*/
                        
                        // on supprime les arguments, sauf le premier parce qu'on le réutilise
                        if (tree->data->type == TYPE_USERMETHOD)
                            ((NeList*)args->data.ptr)->tab[0] = neo_none_create();

                        neobject_destroy(args, true);

                        neobject_destroy(tree->data, true);
                        tree->data = NULL;

                        deleteContext();

                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }

                    // sauvegarde des valeurs obtenues à la fin de la méthode pour que le paramètre puisse éventuellement récupérer son ancienne valeur
                    if (tree->data->type == TYPE_USERMETHOD && fun->nbArgs > 0)
                    {

                        // sauvegarde uniquement le data, et on l'enlève de fun->args[0]
                        NeObjDat s = fun->args[0]->data;
                        int t = fun->args[0]->type;
                        
                        neobject_reset(fun->args[0]);

                        deleteContext(); // supprime le contexte : restaure les variables d'avant

                        neobject_destroy(((NeList*)args->data.ptr)->tab[0], false);
                        
                        // on modifie list[0]
                        ((NeList*)args->data.ptr)->tab[0]->data = s;
                        ((NeList*)args->data.ptr)->tab[0]->type = t;

                        ((NeList*)args->data.ptr)->tab[0] = NULL; // neobject au pif

                    }
                    else {
                        deleteContext(); // réaffecte les anciennes valeurs des variables qui ont été mises en local
                    }


                    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
                    // on enlève une à une toutes les variables qu'on avait rajoutées
                    while (variables_a_sauvegarder->tete != sov_vars_to_save) {
                        err_free(ptrlist_pop(variables_a_sauvegarder));
                    }

                    neobject_destroy(args, true);
                    neobject_destroy(tree->data, true);
                    tree->data = NULL; // la prochaine fois qu'on appellera cette fonction, on n'oubliera pas de l'évaluer
                    
                    if (int_ret == EXIT_RETURN) {
                        return_neo_execval(valstack, RETURN_VALUE);
                        RETURN_VALUE = NULL; // pour dire que l'on l'a utilisé
                    }
                    else
                        return_neo_execval(valstack, neo_none_create());

                    // ainsi si une return_value n'est pas à NULL c'est que forcément la valeur n'a pas été récupérée

                    RETURN_BACK();
                    
                }

                else if (tree->data->type == TYPE_EMPTY)
                {
                    CODE_ERROR = 8;
                    neobject_destroy(tree->data, true);
                    tree->data = NULL;
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }
                else
                {
                    CODE_ERROR = 9;
                    neobject_destroy(tree->data, true);
                    tree->data = NULL;
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }
                
            }
            
            case TYPE_LISTINDEX:
            {
                // il faut au préalable calculer l'index
                // tree->data contient un pointeur vers la liste concernée
                //stack_check_for(varstack, 2);
                stack_push(varstack, (void*)calcListIndex);
                stack_push(varstack, (void*)tree);

                stack_args_expr(tree->sons[0], 0, varstack);
                stack_ret_zone(&&LABEL_LISTINDEX1, retstack);
                goto debut_boucle;
                LABEL_LISTINDEX1:

                if (CODE_ERROR != 0)
                {
                    RETURN_BACK();
                }

                // la valeur de la liste est stockée dans la pile de retour

                if (CODE_ERROR != 0) {
                    RETURN_BACK();
                }

                // on ne dépile pas, mais on récupère la valeur de tree dans la pile
                tree = (Tree*)varstack->stack_ptr[0];

                stack_args_expr(tree->sons[1], 1, varstack);
                stack_ret_zone(&&LABEL_LISTINDEX2, retstack);
                goto debut_boucle;
                LABEL_LISTINDEX2:

                tree = (Tree*)stack_pop(varstack);
                calcListIndex = (intptr_t)stack_pop(varstack);

                NeObject* index = (NeObject*)stack_pop(valstack); // on les dépile dans tous les cas
                NeObject* obj = (NeObject*)stack_pop(valstack);


                if (CODE_ERROR != 0) {
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }


                if (obj->type != TYPE_LIST && obj->type != TYPE_STRING)
                {
                    CODE_ERROR = 15;
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }

                
                if (index->type != TYPE_NUMBER)
                {
                    neobject_destroy(index, true);
                    CODE_ERROR = 16;
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }
                
                long int index2 = (long int)number_toDouble(neo_to_nb(index));
                neobject_destroy(index,true);
                

                if (obj->type == TYPE_EMPTY)
                {
                    CODE_ERROR = 17;
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }
                else if ((obj->type == TYPE_LIST && index2 >= neo_list_len(obj)) || index2 < 0 || (obj->type == TYPE_STRING && index2 >= strlen(neo_to_string(obj))))
                {
                    CODE_ERROR = 18;
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }
                
                
                if (calcListIndex)
                {
                    NeObject* retour = NULL;

                    if (obj->type == TYPE_LIST) {
                        retour = neo_copy(neo_list_nth(obj,index2));
                    }
                    else {
                        retour = neo_str_create(charToString(neo_to_string(obj)[index2]));
                    }

                    return_neo_execval(valstack, retour);
                    RETURN_BACK();
                }
                else if (obj->type == TYPE_LIST)
                {
                    NeObject* neo = neo_list_nth(obj,index2);
                    update_if_promise(neo);
                    // dans ce cas-là, on va devoir retourner directement le pointeur de l'élément
                    return_neo_execval(valstack, neo); // au lieu de faire une copie, on renvoie directement le bon pointeur
                    RETURN_BACK();
                }
                else {
                    CODE_ERROR = 105;
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }

            }

            case TYPE_CONTAINER:
            {
                if (!calcListIndex)
                {
                    CODE_ERROR = 81; // affectation d'un champ de container littéral : illégal car -> fuite mémoire
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }

                val = nelist_create(tree->nbSons);
                ext_index = 0;

                for ( ; ext_index < tree->nbSons ; ext_index++)
                {
                    //stack_check_for(varstack, 4);
                    stack_push(varstack, (void*)val);
                    stack_push(varstack, (void*)ext_index);

                    stack_push(varstack, (void*)calcListIndex);
                    stack_push(varstack, (void*)tree);

                    stack_args_expr(tree->sons[ext_index]->sons[0], 1, varstack);
                    stack_ret_zone(&&LABEL_CONTAINER, retstack);
                    goto debut_boucle;
                    LABEL_CONTAINER:

                    tree = (Tree*)stack_pop(varstack);
                    calcListIndex = (intptr_t)stack_pop(varstack);
                    ext_index = (intptr_t)stack_pop(varstack);
                    val = (NeList*)stack_pop(varstack);

                    val->tab[ext_index] = (NeObject*)stack_pop(valstack);

                    if (CODE_ERROR != 0)
                    {
                        for (int k=0 ; k < ext_index ; k++) // libération de tous les éléments alloués jusqu'a i exclus
                        {
                            neobject_destroy(val->tab[k], true);
                        }
                        err_free(val->tab);
                        err_free(val);
                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }
                }
                NeObject* ret = neo_container_create(tree->label2, val);
                
                return_neo_execval(valstack, ret);
                RETURN_BACK();
            }
            
            case TYPE_ATTRIBUTE:
            {
                stack_push(varstack, (void*)calcListIndex);
                stack_push(varstack, (void*)tree);

                stack_args_expr(tree->sons[0], calcListIndex, varstack);
                stack_ret_zone(&&LABEL_ATTRIBUTE, retstack);
                goto debut_boucle;
                LABEL_ATTRIBUTE:

                tree = (Tree*)stack_pop(varstack);
                calcListIndex = (intptr_t)stack_pop(varstack);

                NeObject* neo = (NeObject*)stack_pop(valstack); // vérifier que le fils existe bien

                if (CODE_ERROR != 0) {
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }

                if (neo->type != TYPE_CONTAINER)
                {
                    // Erreur : essaie d'accéder à un champ d'une variable qui n'est pas un container
                    neobject_destroy(neo, calcListIndex);
                    CODE_ERROR = 80;
                    return_neo_execval(valstack, NULL);
                    RETURN_BACK();
                }


                // va chercher la valeur
                NeObject* ret;

                Container* c = (Container*)neo->data.ptr;

                int index = 0;

                if (tree->label2 != -1) // ça veut dire qu'on sait exactement où chercher la valeur
                {
                    // passage optimisé
                    index = tree->label2;
                }
                else
                {
                    // passage complexe
                    // il faut au préalable calculer l'index de où est la valeur dans le tableau
                    NeList* list = (NeList*)(ATTRIBUTES->tab[c->type]->data.ptr);
                    
                    for (; index < list->len && strcmp(neo_to_string(list->tab[index]), tree->label1) != 0 ; index++);

                    if (index == list->len - 1 && strcmp(neo_to_string(list->tab[index]), tree->label1) != 0)
                    {
                        CODE_ERROR = 82;
                        neobject_destroy(neo, calcListIndex);
                        return_neo_execval(valstack, NULL);
                        RETURN_BACK();
                    }

                    tree->label2 = index;

                    // pour l'affichage de l'erreur on garde ça
                    //err_free(tree->label1) ; tree->label1 = NULL;
                }


                if (calcListIndex) {
                    ret = neo_copy(c->data->tab[index]);
                    neobject_destroy(neo, true);
                }
                else {
                    NeObject* neo2 = c->data->tab[index];
                    update_if_promise(neo2);
                    ret = neo2;
                }
                
                return_neo_execval(valstack, ret);
                RETURN_BACK();
            }
            
            case TYPE_STRING:
            {
                if (calcListIndex)
                {
                    return_neo_execval(valstack, neo_copy(tree->data));
                }
                else
                {
                    CODE_ERROR = 89;
                    return_neo_execval(valstack, NULL);
                }
                
                RETURN_BACK();
            }
            
            case TYPE_BOOL:
            {
                if (calcListIndex)
                    return_neo_execval(valstack, neo_copy(tree->data));
                else
                {
                    CODE_ERROR = 89;
                    return_neo_execval(valstack, NULL);
                }
                RETURN_BACK();
            }

            case TYPE_EXCEPTION:
            {
                if (calcListIndex)
                    return_neo_execval(valstack, neo_copy(tree->data));
                else
                {
                    CODE_ERROR = 89;
                    return_neo_execval(valstack, NULL);
                }
                RETURN_BACK();
            }
            
            case TYPE_NONE:
            {
                if (calcListIndex)
                    return_neo_execval(valstack, neo_copy(tree->data));
                else
                {
                    CODE_ERROR = 89;
                    return_neo_execval(valstack, NULL);
                }
                RETURN_BACK();
            }
        }
    }
    // sinon, on est pas dans une expression, et on saute directement ici
    else { // on arrive dans le exec

        for (inst=0 ; inst < tree->nbSons ; inst++)
        {

            // si c'est une expression, on l'évalue direct
            if (isExpression[tree->sons[inst]->type]) {
                //stack_check_for(varstack, 1);
                stack_push(varstack, (void*)inst);

                stack_args_expr(tree->sons[inst], 1, varstack);
                stack_ret_zone(&&LABEL_EXPRESSION, retstack);
                goto debut_boucle;
                LABEL_EXPRESSION:

                inst = (intptr_t)stack_pop(varstack);
                tree = (Tree*)varstack->stack_ptr[0];
                calcListIndex = (intptr_t)varstack->stack_ptr[-1];
                
                neobject_destroy((NeObject*)stack_pop(valstack), true); // sinon, évaluation de l'expression, en la libérant juste après
            }

            else {

                switch (tree->sons[inst]->type) {

                    case TYPE_TRYEXCEPT:
                    {

                        // trucs qu'il y a à sauvegarder ici : inst
                        //stack_check_for(varstack, 3);
                        stack_push(varstack, (void*)inst);
                        stack_push(varstack, (void*)calcListIndex);
                        stack_push(varstack, (void*)tree);
                        
                        stack_args_expr(tree->sons[inst]->sons[0], 0, varstack);
                        stack_ret_zone(&&LABEL_TRYEXCEPT1, retstack);
                        goto debut_boucle;
                        LABEL_TRYEXCEPT1:

                        tree = (Tree*)stack_pop(varstack);
                        calcListIndex = (intptr_t)stack_pop(varstack);
                        inst = (intptr_t)stack_pop(varstack);

                        int_ret = (intptr_t)stack_pop(valstack);
                        maintree = tree->sons[inst];

                        // exécution du try

                        //printf("ERRREUR : %d\n", CODE_ERROR);

                        if (CODE_ERROR > 1) // exit() n'est pas considéré comme une erreur
                        {
                            // on va parcourir la liste des exceptions jusqu'à en trouver une qui corresponde
                            int bo = 0; // va correspondre à l'indice du sous arbre que l'on va exécuter

                            // parcours des différents blocs except pour trouver le bon
                            for (int fils = 1 ; fils < maintree->nbSons && bo == 0 ; fils++) { // on arrête dès que bo correspond à un vrai sous arbre except

                                if (maintree->sons[fils]->sons[0]->nbSons == 0) {
                                    bo = fils; // si le bloc except ne spécifie aucune exception, on peut directement l'exécuter
                                }

                                for (int i = 0 ; i < maintree->sons[fils]->sons[0]->nbSons ; i++)
                                {
                                    NeObject* obj = maintree->sons[fils]->sons[0]->sons[i]->data;

                                    if (obj->type != TYPE_EXCEPTION)
                                    {
                                        CODE_ERROR = 78;
                                        return_int_execval(valstack, int_ret);
                                        RETURN_BACK();
                                    }
                                    else
                                    {
                                        int code = *((int*)maintree->sons[fils]->sons[0]->sons[i]->data->data.ptr);
                                        if (exceptions_err.tab[CODE_ERROR] == code || (CODE_ERROR == 74 && EXCEPTION != NULL && strcmp(EXCEPTION, exceptions.tab[code])==0)) // l'erreur correspond
                                        {
                                            if (CODE_ERROR == 74)
                                                EXCEPTION = NULL;
                                            
                                            bo = fils;
                                            break;
                                        }
                                    }
                                }

                            }

                            if (bo > 0) // exécution du except
                            {
                                CODE_ERROR = 0; // du coup c'est bon on repart (la police a arrêté le programme, a vérifié ses papiers, et le programme est reparti)
                                
                                //stack_check_for(varstack, 3);
                                stack_push(varstack, (void*)inst);
                                stack_push(varstack, (void*)calcListIndex);
                                stack_push(varstack, (void*)tree);

                                stack_args_expr(maintree->sons[bo]->sons[1], 0, varstack);
                                stack_ret_zone(&&LABEL_TRYEXCEPT2, retstack);
                                goto debut_boucle;
                                LABEL_TRYEXCEPT2:

                                tree = (Tree*)stack_pop(varstack);
                                calcListIndex = (intptr_t)stack_pop(varstack);
                                inst = (intptr_t)stack_pop(varstack);

                                int_ret = (intptr_t)stack_pop(valstack);

                                if (CODE_ERROR != 0) {
                                    return_int_execval(valstack, int_ret);
                                    RETURN_BACK();
                                }
                            }
                        }
                        break;
                    }

                    case TYPE_ATOMICBLOCK:
                    {
                        // on va exécuter le bloc de code d'une traite, sans changer de processus
                        //stack_check_for(varstack, 4);
                        stack_push(varstack, (void*)inst);
                        stack_push(varstack, (void*)atomic_counter);
                        stack_push(varstack, (void*)calcListIndex);
                        stack_push(varstack, (void*)tree);

                        atomic_counter = -1; // tant qu'on n'a pas remis une valeur positive, on ne passera pas à un autre processus
                        
                        stack_args_expr(tree->sons[inst], 0, varstack);
                        stack_ret_zone(&&LABEL_ATOMIC, retstack);
                        goto debut_boucle;
                        LABEL_ATOMIC:

                        tree = (Tree*)stack_pop(varstack);
                        calcListIndex = (intptr_t)stack_pop(varstack);
                        intptr_t temp = (intptr_t)stack_pop(varstack);
                        inst = (intptr_t)stack_pop(varstack);

                        
                        // s'il reste des crédits à ce processus, on les lui rend, sinon on passe au suivant
                        atomic_counter = (atomic_counter + temp > 0) ? atomic_counter + temp : 0;


                        int_ret = (intptr_t)stack_pop(valstack);


                        if (CODE_ERROR != 0 || int_ret != 0) {
                            return_int_execval(valstack, int_ret);
                            RETURN_BACK();
                        }
                        break;
                    }
                    
                    case TYPE_BLOCKWORD1LINE:
                    {
                        if (tree->sons[inst]->label2 == RETURN) // action de return
                        {
                            if (tree->sons[inst]->nbSons > 1)
                            {
                                CODE_ERROR = 21;
                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }
                            
                            if (RETURN_VALUE != NULL) // c'est pas correct, car on ne peut pas renvoyer une valeur alors que la précédente n'a pas été récupérée
                            {
                                CODE_ERROR = 99;
                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }

                            if (tree->sons[inst]->nbSons == 0) {
                                RETURN_VALUE = neo_none_create();
                            }
                            else {
                                //stack_check_for(varstack, 3);
                                stack_push(varstack, (void*)inst);
                                stack_push(varstack, (void*)calcListIndex);
                                stack_push(varstack, (void*)tree);

                                stack_args_expr(tree->sons[inst]->sons[0], 1, varstack);
                                stack_ret_zone(&&LABEL_BLOCKWORD1LINE, retstack);
                                goto debut_boucle;
                                LABEL_BLOCKWORD1LINE:

                                tree = (Tree*)stack_pop(varstack);
                                calcListIndex = (intptr_t)stack_pop(varstack);
                                inst = (intptr_t)stack_pop(varstack);

                                RETURN_VALUE = (NeObject*)stack_pop(valstack);
                            }

                            

                            if (CODE_ERROR != 0) {
                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }
                            
                            return_int_execval(valstack, EXIT_RETURN);
                            RETURN_BACK();
                            
                        }

                        
                        else if (tree->sons[inst]->label2 == IMPORT) // action de import
                        {
                            for (ext_index = 0 ; ext_index < tree->sons[inst]->nbSons ; ext_index++)
                            {
                                // on empile inst et ext_index
                                //stack_check_for(varstack, 4);
                                stack_push(varstack, (void*)inst);
                                stack_push(varstack, (void*)ext_index);
                                stack_push(varstack, (void*)calcListIndex);
                                stack_push(varstack, (void*)tree);

                                stack_args_expr(tree->sons[inst]->sons[ext_index], 1, varstack);
                                stack_ret_zone(&&LABEL_IMPORT, retstack);
                                goto debut_boucle;
                                LABEL_IMPORT:
                                
                                nom = (NeObject*)stack_pop(valstack);

                                tree = (Tree*)stack_pop(varstack);
                                calcListIndex = (intptr_t)stack_pop(varstack);
                                ext_index = (intptr_t)stack_pop(varstack);
                                inst = (intptr_t)stack_pop(varstack);

                                if (CODE_ERROR != 0) {
                                    return_int_execval(valstack, 0);
                                    RETURN_BACK();
                                }

                                char* nomAct = strdup(neo_to_string(ADRESSES->tab[NAME])); // pour restaurer le nom de fichier actuel

                                #ifndef TI83PCE
                                    char* nomFichier = addStr(neo_to_string(nom), ".ne");

                                    updateFileName(strdup(neo_to_string(nom)));

                                    importFile(nomFichier);
                                    err_free(nomFichier);
                                #else
                                    updateFileName(strdup(neo_to_string(nom)));
                                    importFile(neo_to_string(nom));
                                #endif

                                updateFileName(nomAct);

                                neobject_destroy(nom, true);

                                if (CODE_ERROR != 0) {
                                    return_int_execval(valstack, 0);
                                    RETURN_BACK();
                                }

                            }
                        }

                        else if (tree->sons[inst]->label2 == LOCAL)
                        {
                            if (tree->sons[inst]->nbSons == 0) // il faut au moins un argument
                            {
                                CODE_ERROR = 69;
                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }
                            else if (VAR_LOC->tete == NULL)
                            {
                                CODE_ERROR = 70;
                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }

                            for (int i = 0 ; i < tree->sons[inst]->nbSons ; i++) // pour toutes les variables
                            {
                                // va traiter la variable comme étant locale
                                save_later(variables_a_sauvegarder, tree->sons[inst]->sons[i]->data);
                                local(tree->sons[inst]->sons[i]->data);
                            }
                            
                        }
                        else if (tree->sons[inst]->label2 == AWAIT) { // attend tant qu'une condition soit vérifiée

                            if (tree->sons[inst]->nbSons > 1)
                            {
                                CODE_ERROR = 101;
                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }

                            //stack_check_for(varstack, 2);
                            stack_push(varstack, (void*)inst);
                            stack_push(varstack, (void*)tree);

                            process_cycle->process->wait_expr = tree->sons[inst]->sons[0];


                            check_condition: // on vient du haut de la boucle, donc on charge les paramètres actuels

                            if (CODE_ERROR != 0) {
                                // erreur survenue ailleurs dans le programme
                                // il faut dépiler inst et tree
                                process_cycle->process->wait_expr = NULL;
                                tree = (Tree*)stack_pop(varstack);
                                inst = (intptr_t)stack_pop(varstack);

                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }

                            //stack_check_for(varstack, 1);
                            stack_push(varstack, (void*)process_cycle->process->wait_expr);


                            // évaluation de la condition
                            stack_args_expr(process_cycle->process->wait_expr, 1, varstack);
                            stack_ret_zone(&&LABEL_AWAIT, retstack);

                            process_cycle->process->wait_expr = NULL; // le temps de l'évaluation de l'expression, on lui fait croire qu'il n'est plus en pause
                            
                            goto debut_boucle;
                            // si l'évaluation consomme plus que ATOMIC_TIME instructions, on passe qd même au processus suivant
                            // en revanche si on revient avant et que la condition est fausse , on passe direct au suivant

                            LABEL_AWAIT:

                            process_cycle->process->wait_expr = (Tree*)stack_pop(varstack);


                            if (CODE_ERROR != 0)
                            {
                                process_cycle->process->wait_expr = NULL;
                                tree = (Tree*)stack_pop(varstack);
                                inst = (intptr_t)stack_pop(varstack);

                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }


                            NeObject* neo = (NeObject*)stack_pop(valstack);

                            if (neoIsTrue(neo)) // fin de l'attente passive
                            {
                                neobject_destroy(neo, true);
                                process_cycle->process->wait_expr = NULL;
                                
                                // on restaure les variables du début
                                tree = (Tree*)stack_pop(varstack);
                                inst = (intptr_t)stack_pop(varstack);
                            }
                            else
                            {
                                neobject_destroy(neo, true);
                                
                                if (atomic_counter < 0) {
                                    // on est dans un bloc atomique donc on peut pas passer au processus suivant
                                    goto check_condition;
                                }
                                else {
                                    atomic_counter = 0;
                                    goto debut_boucle;
                                }
                            }

                        }
                        break;
                    }

                    
                    
                    case TYPE_KEYWORD:
                    {
                        if (tree->sons[inst]->label2 != PASS) {
                            return_int_execval(valstack, tree->sons[inst]->label2);
                            RETURN_BACK();
                        }
                        break;
                    }

                    
                
                    case TYPE_CONDITIONBLOCK:
                    {
                        // setup des variables locales
                        stack_extend(varstack, 5);
                        LOCAL_CB_inst = (void*)inst;
                        
                        // a partir de maintenant, on n'utilise plus que les variables locales
                        maintree = LOCAL_CB_tree_c->sons[LOCAL_CB_inst_c];
                        LOCAL_CB_int_ret = (void*)0;
                        LOCAL_CB_bloc = 0;

                        LOCAL_CB_sov_vars_to_save = variables_a_sauvegarder->tete;

                        // on a comme variables locales : int_ret, bloc, inst : d'ailleurs inst doit être empilé partout

                        while (LOCAL_CB_bloc_c < maintree->nbSons)
                        {
                            stack_args_expr(maintree->sons[LOCAL_CB_bloc_c]->sons[0], 1, varstack);
                            stack_ret_zone(&&LABEL_CONDITIONBLOCK1, retstack);
                            goto debut_boucle;
                            LABEL_CONDITIONBLOCK1:


                            if (CODE_ERROR != 0) {
                                tree = LOCAL_CB_tree_c;
                                inst = LOCAL_CB_inst_c;
                                stack_reduce(varstack, 5);

                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }

                            maintree = LOCAL_CB_tree_c->sons[LOCAL_CB_inst_c];

                            expr = (NeObject*)stack_pop(valstack);

                            cond = neoIsTrue(expr);
                            neobject_destroy(expr, true);

                            if (CODE_ERROR != 0) {
                                tree = LOCAL_CB_tree_c;
                                inst = LOCAL_CB_inst_c;
                                stack_reduce(varstack, 5);

                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }

                            if (cond)
                            {
                                // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
                                newContext();

                                stack_args_expr(maintree->sons[LOCAL_CB_bloc_c]->sons[1], 0, varstack);
                                stack_ret_zone(&&LABEL_CONDITIONBLOCK2, retstack);
                                goto debut_boucle;
                                LABEL_CONDITIONBLOCK2:

                                maintree = LOCAL_CB_tree_c->sons[LOCAL_CB_inst_c];

                                LOCAL_CB_int_ret = (void*)((intptr_t)stack_pop(valstack));

                                deleteContext(); // on vient d'exécuter le code, donc on a fini le if

                                
                                if (CODE_ERROR != 0) {
                                    tree = LOCAL_CB_tree_c;
                                    inst = LOCAL_CB_inst_c;
                                    stack_reduce(varstack, 5);

                                    return_int_execval(valstack, 0);
                                    RETURN_BACK();
                                }

                                if (LOCAL_CB_int_ret_c != 0) {
                                    tree = LOCAL_CB_tree_c;
                                    inst = LOCAL_CB_inst_c;
                                    int_ret = LOCAL_CB_int_ret_c;
                                    stack_reduce(varstack, 5);

                                    return_int_execval(valstack, int_ret);
                                    RETURN_BACK();
                                }
                                
                                
                                LOCAL_CB_bloc++;
                                while (LOCAL_CB_bloc_c < maintree->nbSons && maintree->sons[LOCAL_CB_bloc_c]->type != TYPE_STATEMENTIF)
                                    LOCAL_CB_bloc++;

                                // on vient d'exécuter le if, donc on a fini ce if, on peut supprimer le contexte

                                if (LOCAL_CB_bloc_c == maintree->nbSons) {
                                    break;
                                }
                                else
                                    continue;
                            }
                            
                            LOCAL_CB_bloc++;


                            LOCAL_CB_elif = (void*)false;
                            while (LOCAL_CB_bloc_c < maintree->nbSons && maintree->sons[LOCAL_CB_bloc_c]->type == TYPE_STATEMENTELIF)
                            {

                                stack_args_expr(maintree->sons[LOCAL_CB_bloc_c]->sons[0], 1, varstack);
                                stack_ret_zone(&&LABEL_CONDITIONBLOCK3, retstack);
                                goto debut_boucle;
                                LABEL_CONDITIONBLOCK3:

                                maintree = LOCAL_CB_tree_c->sons[LOCAL_CB_inst_c];

                                expr = (NeObject*)stack_pop(valstack);
                                cond = neoIsTrue(expr);
                                neobject_destroy(expr, true);

                                if (CODE_ERROR != 0) {
                                    tree = LOCAL_CB_tree_c;
                                    inst = LOCAL_CB_inst_c;
                                    stack_reduce(varstack, 5);

                                    return_int_execval(valstack, 0);
                                    RETURN_BACK();
                                }


                                if (!LOCAL_CB_elif_c && cond)
                                {
                                    // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
                                    newContext();

                                    stack_args_expr(maintree->sons[LOCAL_CB_bloc_c]->sons[1], 0, varstack);
                                    stack_ret_zone(&&LABEL_CONDITIONBLOCK4, retstack);
                                    goto debut_boucle;
                                    LABEL_CONDITIONBLOCK4:

                                    maintree = LOCAL_CB_tree_c->sons[LOCAL_CB_inst_c];

                                    LOCAL_CB_int_ret = (void*)((intptr_t)stack_pop(valstack));

                                    deleteContext(); // on vient d'exécuter le code, donc on a fini le if


                                    if (CODE_ERROR != 0) {
                                        tree = LOCAL_CB_tree_c;
                                        inst = LOCAL_CB_inst_c;
                                        stack_reduce(varstack, 5);

                                        return_int_execval(valstack, 0);
                                        RETURN_BACK();
                                    }

                                    if (LOCAL_CB_int_ret_c != 0) {
                                        tree = LOCAL_CB_tree_c;
                                        inst = LOCAL_CB_inst_c;
                                        int_ret = LOCAL_CB_int_ret_c;
                                        stack_reduce(varstack, 5);

                                        return_int_execval(valstack, int_ret);
                                        RETURN_BACK();
                                    }

                                    
                                    LOCAL_CB_elif = (void*)true;
                                }
                                LOCAL_CB_bloc++;
                            }


                            if (LOCAL_CB_bloc_c < maintree->nbSons && maintree->sons[LOCAL_CB_bloc_c]->type == TYPE_STATEMENTELSE) // s'il y a un bloc else
                            {
                                if (!LOCAL_CB_elif_c) // si on a le droit d'y aller
                                {
                                    newContext();

                                    stack_args_expr(maintree->sons[LOCAL_CB_bloc_c], 0, varstack);
                                    stack_ret_zone(&&LABEL_CONDITIONBLOCK5, retstack);
                                    goto debut_boucle;
                                    LABEL_CONDITIONBLOCK5:

                                    maintree = LOCAL_CB_tree_c->sons[LOCAL_CB_inst_c];

                                    LOCAL_CB_int_ret = (void*)((intptr_t)stack_pop(valstack));

                                    deleteContext(); // on vient d'exécuter le code, donc on a fini le if

                                    if (CODE_ERROR != 0) {
                                        tree = LOCAL_CB_tree_c;
                                        inst = LOCAL_CB_inst_c;
                                        stack_reduce(varstack, 5);

                                        return_int_execval(valstack, 0);
                                        RETURN_BACK();
                                    }

                                    if (LOCAL_CB_int_ret_c != 0) {
                                        tree = LOCAL_CB_tree_c;
                                        inst = LOCAL_CB_inst_c;
                                        int_ret = LOCAL_CB_int_ret_c;
                                        stack_reduce(varstack, 5);

                                        return_int_execval(valstack, int_ret);
                                        RETURN_BACK();
                                    }

                                }
                                LOCAL_CB_bloc++;
                            }
                        }

                        // on enlève les variables qu'on avait marquées comme "à sauvegarder"
                        // on enlève une à une toutes les variables qu'on avait rajoutées

                        while (variables_a_sauvegarder->tete != LOCAL_CB_sov_vars_to_save) {
                            err_free(ptrlist_pop(variables_a_sauvegarder));
                        }

                        // restauration des variables
                        tree = LOCAL_CB_tree_c;
                        inst = LOCAL_CB_inst_c;
                        stack_reduce(varstack, 5);
                        break;
                    }

                    

                    case TYPE_STATEMENTWHILE:
                    {
                        int_ret = 0;

                        //stack_check_for(varstack, 4);
                        // évaluation de la condition
                        stack_push(varstack, (void*)int_ret);
                        stack_push(varstack, (void*)inst);
                        stack_push(varstack, (void*)calcListIndex);
                        stack_push(varstack, (void*)tree);

                        stack_args_expr(tree->sons[inst]->sons[0], 1, varstack);
                        stack_ret_zone(&&LABEL_WHILE1, retstack);
                        goto debut_boucle;
                        LABEL_WHILE1:

                        tree = (Tree*)stack_pop(varstack);
                        calcListIndex = (intptr_t)stack_pop(varstack);
                        inst = (intptr_t)stack_pop(varstack);
                        int_ret = (intptr_t)stack_pop(varstack);

                        expr = (NeObject*)stack_pop(valstack);
                        cond = neoIsTrue(expr);
                        neobject_destroy(expr, true);

                        if (CODE_ERROR != 0) {
                            return_int_execval(valstack, 0);
                            RETURN_BACK();
                        }

                        while (cond)
                        {

                            stack_push(varstack, (void*)inst);
                            stack_push(varstack, (void*)calcListIndex);
                            stack_push(varstack, (void*)tree);

                            stack_args_expr(tree->sons[inst]->sons[1], 0, varstack);
                            stack_ret_zone(&&LABEL_WHILE2, retstack);
                            goto debut_boucle;
                            LABEL_WHILE2:

                            tree = (Tree*)stack_pop(varstack);
                            calcListIndex = (intptr_t)stack_pop(varstack);
                            inst = (intptr_t)stack_pop(varstack);

                            int_ret = (intptr_t)stack_pop(valstack);

                            
                            if (CODE_ERROR != 0) {
                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }
                            
                            if (int_ret != 0 && int_ret != CONTINUE)
                                break;

                            // réévaluation de la condition

                            stack_push(varstack, (void*)int_ret);
                            stack_push(varstack, (void*)inst);
                            stack_push(varstack, (void*)calcListIndex);
                            stack_push(varstack, (void*)tree);

                            stack_args_expr(tree->sons[inst]->sons[0], 1, varstack);
                            stack_ret_zone(&&LABEL_WHILE3, retstack);
                            goto debut_boucle;
                            LABEL_WHILE3:

                            tree = (Tree*)stack_pop(varstack);
                            calcListIndex = (intptr_t)stack_pop(varstack);
                            inst = (intptr_t)stack_pop(varstack);
                            int_ret = (intptr_t)stack_pop(varstack);

                            expr = (NeObject*)stack_pop(valstack);
                            cond = neoIsTrue(expr);
                            neobject_destroy(expr, true);

                            if (CODE_ERROR != 0) {
                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }
                            
                        }

                        
                        if (int_ret == EXIT_RETURN) { // en sortant d'une boucle, on n'a pas de break ou continue
                            return_int_execval(valstack, int_ret);
                            RETURN_BACK();
                        }
                        break;
                    }
                    
                    

                    case TYPE_STATEMENTFOR:
                    {
                        stack_extend(varstack, 5); // on agrandit la pile pour nos cinq variables locales

                        // on charge la valeur des variables qui existaient déjà
                        LOCAL_FOR_inst = (void*)inst;
                        LOCAL_FOR_sov_vars_to_save = sov_vars_to_save;
                        
                        if (LOCAL_FOR_tree_c->sons[LOCAL_FOR_inst_c]->sons[0]->sons[0]->type != TYPE_VARIABLE)
                        {
                            CODE_ERROR = 22;

                            // restauration des variables
                            tree = LOCAL_FOR_tree_c;
                            calcListIndex = LOCAL_FOR_calcListIndex_c;
                            inst = LOCAL_FOR_inst_c;
                            sov_vars_to_save = LOCAL_FOR_sov_vars_to_save;
                            stack_reduce(varstack, 5);

                            return_int_execval(valstack, 0);
                            RETURN_BACK();
                        }
                        

                        stack_args_expr(LOCAL_FOR_tree_c->sons[LOCAL_FOR_inst_c]->sons[0]->sons[1], 1, varstack);
                        stack_ret_zone(&&LABEL_FOR1, retstack);
                        goto debut_boucle;
                        LABEL_FOR1:

                        valeur = (NeObject*)stack_pop(valstack);


                        LOCAL_FOR_var = LOCAL_FOR_tree_c->sons[LOCAL_FOR_inst_c]->sons[0]->sons[0]->data; // variable à incrémenter lors de la boucle

                        newContext(); // nouveau contexte pour rendre des variables locales à la boucle for
                        LOCAL_FOR_sov_vars_to_save = variables_a_sauvegarder->tete; // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

                        save_later(variables_a_sauvegarder, LOCAL_FOR_var);
                        local(LOCAL_FOR_var); // on localise l'indice de la boucle

                        neobject_destroy(LOCAL_FOR_var, false); // on va mettre des nouveaux trucs dedans

                        
                        if (CODE_ERROR != 0)
                        {
                            neobject_destroy(valeur, true);

                            // restauration des variables
                            tree = LOCAL_FOR_tree_c;
                            calcListIndex = LOCAL_FOR_calcListIndex_c;
                            inst = LOCAL_FOR_inst_c;
                            sov_vars_to_save = LOCAL_FOR_sov_vars_to_save;
                            stack_reduce(varstack, 5);

                            deleteContext();
                            return_int_execval(valstack, 0);
                            RETURN_BACK();
                        }
                        
                        if (valeur -> type != TYPE_NUMBER)
                        {
                            neobject_destroy(valeur, true);
                            CODE_ERROR = 10;

                            // restauration des variables
                            tree = LOCAL_FOR_tree_c;
                            calcListIndex = LOCAL_FOR_calcListIndex_c;
                            inst = LOCAL_FOR_inst_c;
                            sov_vars_to_save = LOCAL_FOR_sov_vars_to_save;
                            stack_reduce(varstack, 5);

                            deleteContext();
                            return_int_execval(valstack, 0);
                            RETURN_BACK();
                        }


                        // le vrai indice de la boucle sera un entier
                        LOCAL_FOR_ext_index = (void*)((intptr_t)number_toDouble(neo_to_nb(valeur)));
                        neobject_destroy(valeur,true);


                        stack_args_expr(LOCAL_FOR_tree_c->sons[LOCAL_FOR_inst_c]->sons[0]->sons[2], 1, varstack);
                        stack_ret_zone(&&LABEL_FOR2, retstack);
                        goto debut_boucle;
                        LABEL_FOR2:

                        tempMax = (NeObject*)stack_pop(valstack);


                        if (CODE_ERROR != 0)
                        {
                            neobject_destroy(tempMax, true);

                            // restauration des variables
                            tree = LOCAL_FOR_tree_c;
                            calcListIndex = LOCAL_FOR_calcListIndex_c;
                            inst = LOCAL_FOR_inst_c;
                            sov_vars_to_save = LOCAL_FOR_sov_vars_to_save;
                            stack_reduce(varstack, 5);

                            deleteContext();
                            return_int_execval(valstack, 0);
                            RETURN_BACK();
                        }

                        if (tempMax -> type != TYPE_NUMBER) {
                            CODE_ERROR = 10;
                            neobject_destroy(tempMax, true);

                            // restauration des variables
                            tree = LOCAL_FOR_tree_c;
                            calcListIndex = LOCAL_FOR_calcListIndex_c;
                            inst = LOCAL_FOR_inst_c;
                            sov_vars_to_save = LOCAL_FOR_sov_vars_to_save;
                            stack_reduce(varstack, 5);

                            deleteContext();
                            return_int_execval(valstack, 0);
                            RETURN_BACK();
                        }

                        Number zero = number_fromDouble((double)0);
                        Number tempMaxNb = neo_to_nb(tempMax);

                        _Bool isinfinity = number_isInf(tempMaxNb) && number_inf(zero, tempMaxNb);
                        _Bool isneginfinity = number_isInf(tempMaxNb) && number_inf(tempMaxNb, zero);

                        number_destroy(zero);
                        
                        LOCAL_FOR_max = (void*)((intptr_t)number_toDouble(neo_to_nb(tempMax))); // borne supérieure des valeurs atteintes par la variable

                        neobject_destroy(tempMax, true); // du coup le data, on le garde donc on libère que l'enveloppe

                        int_ret = 0;

                        // on empile une seule fois le code de la boucle for
                        // les infos sont là dans la pile, y a qu'à incrémenter le pointeur de pile
                        stack_args_expr(LOCAL_FOR_tree_c->sons[LOCAL_FOR_inst_c]->sons[1], 0, varstack);
                        stack_ret_zone(&&LABEL_FOR3, retstack);
                        varstack->stack_ptr -= 2;
                        retstack->stack_ptr --;
                        
                        while ((LOCAL_FOR_ext_index < LOCAL_FOR_max || isinfinity) && !isneginfinity)
                        {
                            // on restaure la valeur de l'indice au début du corps de la boucle
                            NeObject* tempnb = neo_nb_create(number_fromDouble((double)LOCAL_FOR_ext_index_c));
                            LOCAL_FOR_var_c->data = tempnb->data;
                            LOCAL_FOR_var_c->type = tempnb->type;
                            err_free(tempnb);

                            varstack->stack_ptr += 2;
                            retstack->stack_ptr ++;
                            goto debut_boucle;
                            LABEL_FOR3:


                            int_ret = (intptr_t)stack_pop(valstack);
                            neobject_destroy(LOCAL_FOR_var_c, false);

                            if (CODE_ERROR != 0)
                            {
                                // restauration des variables
                                tree = LOCAL_FOR_tree_c;
                                calcListIndex = LOCAL_FOR_calcListIndex_c;
                                inst = LOCAL_FOR_inst_c;
                                sov_vars_to_save = LOCAL_FOR_sov_vars_to_save;
                                stack_reduce(varstack, 5);

                                deleteContext();
                                return_int_execval(valstack, 0);
                                RETURN_BACK();
                            }

                            if (int_ret != 0 && int_ret != CONTINUE)
                                break;
                            
                            LOCAL_FOR_ext_index++;
                        }

                        // la variable représentant le variant de boucle a été supprimée à la fin de la dernière
                        // itération, et si on la laisse telle quelle, elle va se refaire supprimer lors du delete context
                        // ce qui se passe : le newContext nous offre une nouvelle variable libre pour faire nos trucs
                        // a chaque itération, on crée l'objet et on le supprime
                        // à la fin de la boucle, on a donc un bilan neutre, on peut donc reset la variable pour qu'elle ne se
                        // refasse pas supprimer
                        // conclusion : quand on delete un context, il faut faire attention :
                        // -> soit si on a un bilan neutre sur la variable, de la vider de ses pointeurs
                        // -> sinon, si il faut la supprimer, on la laiss telle quelle et le deleteContext va la supprimer
                        neobject_reset(LOCAL_FOR_var_c);

                        deleteContext();

                        // on enlève les variables qu'on avait marquées comme "à sauvegarder"
                        // on enlève une à une toutes les variables qu'on avait rajoutées
                        while (variables_a_sauvegarder->tete != LOCAL_FOR_sov_vars_to_save) {
                            err_free(ptrlist_pop(variables_a_sauvegarder));
                        }

                        // restauration des variables locales
                        tree = LOCAL_FOR_tree_c;
                        inst = LOCAL_FOR_inst_c;
                        //sov_vars_to_save = LOCAL_FOR_sov_vars_to_save;

                        stack_reduce(varstack, 5);
                        
                        if (int_ret == EXIT_RETURN) {
                            return_int_execval(valstack, int_ret);
                            RETURN_BACK();
                        }
                        break;
                    }

                    

                    case TYPE_FUNCTIONDEF:
                    {
                        // définition de fonction en cours d'exécution
                        int index = strlist_index(NOMS, tree->sons[inst]->label1);
                        if (ADRESSES->tab[index] != tree->sons[inst]->data)
                            _affect2(ADRESSES->tab[index], tree->sons[inst]->data);

                        break;
                    }
                
                }
            
            }


            if (CODE_ERROR != 0) { // ben oui sinon les erreurs ne seront pas captées au bon endroit
                return_int_execval(valstack, 0);
                RETURN_BACK();
            }
            
        }
        return_int_execval(valstack, 0);
        RETURN_BACK();

    }

    LABEL_ENDOFSTACK: // ceci est la valeur de fond de pile de retstack. On arrive ici quand la pile est vide

    if (process_cycle->next == NULL && process_cycle->prev == NULL && process_cycle->process->wait_expr == NULL) { // on vient de finir le processus principal et il ne reste plus aucun processus
        NeObject* ret = stack_pop(valstack);

        // supprime le dernier processus
        stack_destroy(valstack);
        stack_destroy(varstack);
        stack_destroy(retstack);

        neobject_destroy(un,true);

        err_free(process_cycle->process);
        err_free(process_cycle);

        ptrlist_destroy(variables_a_sauvegarder, true, true);

        VAR_LOC = var_loc_sov;

        return ret;
    }
    else if (process_cycle->process->wait_expr == NULL) {  // il reste des processus annexes à exécuter
        /////////// on vient de finir un thread, donc on catch l'erreurprocessCycle_remove maintenant

        // on est donc forcément sur le processus principal
        mainThreadReturnValue = stack_pop(valstack);

        // on supprime le processus, on passe au suivant
        process_cycle = processCycle_remove(process_cycle, &varstack, &valstack, &retstack, &variables_a_sauvegarder);

        goto debut_boucle;
    }

    return mainThreadReturnValue;

}














void printRes(NeObject* res)
{
    if (res->type != TYPE_NONE)
    {
        printString(" (");
        setColor(GREEN);
        printString(type(res));
        setColor(WHITE);
        printString(") = ");
        neobject_aff(res);
        newLine();
    }
    return;
}





void startMessage(void)
{
    setColor(BLUE);
    printString("                  Welcome to Neon");

    newLine();

    setColor(GREEN);    
    printString("Neon v");
    printString(VERSION);
    setColor(WHITE);

    printString(" for ");
    printString(PLATFORM);

    printString(", compiled on ");
    printString(__TIMESTAMP__);
    /*printString(" by GCC ");
    printString(__VERSION__);*/
    newLine();
    
    /*printString("Discuss on the official Discord server : ");
    printString(INVITATION);
    printString(" or contact me at raphael@langage-neon.org");
    newLine();*/
    
    printString("Visit ");
    setColor(GREEN); printString("https://langage-neon.raphaael.fr"); setColor(WHITE);
    printString(" for more information.");
    newLine();

    #ifdef EXPERIMENTAL
        newLine();
        setColor(RED);
        printString("/!\\ THIS IS AN EXPERIMENTAL VERSION. IT MAY NOT WORK. /!\\");
        newLine();newLine();
        setColor(WHITE);
    #endif

    return;
}






#ifdef WASM
    void EMSCRIPTEN_KEEPALIVE execCode(char* code)
    {
        
    	if (strcmp(code,"")==0) // si l'utilisateur n'a rien ecrit
    	    return;
        
        FILENAME = NULL;
    
    	Tree* tree = tree_create(NULL, 0, 0);
    	createSyntaxTree(tree, code);

        if (CODE_ERROR != 0)
        {
            printError(CODE_ERROR);
            CODE_ERROR = 0;
            tree_destroy(tree);
            return;
        }

        FILENAME = strdup(tree->label1);

    	// s'il n'y a qu'une expression, alors, on affiche le résultat de l'expression
    	if (tree->nbSons == 1 && tree->sons[0]->type != TYPE_TRYEXCEPT && tree->sons[0]->type != TYPE_CONDITIONBLOCK && tree->sons[0]->type != TYPE_STATEMENTFOR && tree->sons[0]->type != TYPE_STATEMENTWHILE && tree->sons[0]->type != TYPE_KEYWORD && tree->sons[0]->type != TYPE_FUNCTIONDEF && tree->sons[0]->type != TYPE_BLOCKWORD1LINE && tree->sons[0]->type != TYPE_ATOMICBLOCK)
        {
            
            res = execval(tree->sons[0]);


            if (CODE_ERROR > 1)
            {
                printError(CODE_ERROR);
                tree_destroy(tree);
                continue;
            }
            else if (CODE_ERROR == 1) // quitte le terminal
            {
                tree_destroy(tree);
                return ;
            }
            
            printRes(res);
            neobject_destroy(res,true);
        }
        else if (tree->nbSons > 0)
        {
            execval(tree);
            if (CODE_ERROR > 1)
            {
                printError(CODE_ERROR);
                tree_destroy(tree);
                continue;
            }
            else if (CODE_ERROR == 1) // quitte le terminal
            {
                tree_destroy(tree);
                return ;
            }
        }
        CODE_ERROR = 0;
    	tree_destroy(tree);
        return ;
    }




    char* EMSCRIPTEN_KEEPALIVE inputMessage(void)
    {
        return INPUTMESSAGE;
    }

#endif







// cette fonction permet de réutiliser le résultat d'une expression grâce à la variable Ans dans le terminal
void storeAns(NeObject* res) {
    if (strlist_inList(NOMS, "Ans")) {
        int index = strlist_index(NOMS, "Ans");
        neobject_destroy(ADRESSES->tab[index], true); // supprime totalement le précédent
        ADRESSES->tab[index] = res; // met le nouveau dedans
    }
    else {
        strlist_append(NOMS, strdup("Ans"));
        nelist_append(ADRESSES, res);
    }
}






void terminal(void)
{
    char* exp;
    NeObject* res ; Tree* tree;
        
        while (true)
        {
            CODE_ERROR = 0; // réinitialise les erreurs

            FILENAME = NULL;

            exp = inputCode(SEQUENCE_ENTREE);

            if (exp == NULL && CODE_ERROR == 0) // pour afficher le keyboardInterrupt
                CODE_ERROR = 104;
            
            // quand l'utilisateur appuie sur CTRL-D, ça met CODE_ERROR à 1
            if (CODE_ERROR > 1)
            {
                printError(CODE_ERROR);
                CODE_ERROR = 0;
                continue;
            }
            else if (CODE_ERROR == 1)
                return;



            if (strcmp(exp,"")==0) // si l'utilisateur n'a rien ecrit
            {
                err_free(exp);
                continue;
            }

            
            
            tree = tree_create(NULL, 0, 0);

            if (CODE_ERROR != 0)
            {
                printError(CODE_ERROR);
                err_free(exp);
                continue;
            }


            createSyntaxTree(tree, exp);

            err_free(exp);

            if (CODE_ERROR > 1)
            {
                printError(CODE_ERROR);
                tree_destroy(tree);
                continue;
            }

            
            // s'il n'y a qu'une expression, alors, on affiche le résultat de l'expression
            if (tree->nbSons == 1 && tree->sons[0]->type != TYPE_TRYEXCEPT && tree->sons[0]->type != TYPE_CONDITIONBLOCK && tree->sons[0]->type != TYPE_STATEMENTFOR && tree->sons[0]->type != TYPE_STATEMENTWHILE && tree->sons[0]->type != TYPE_KEYWORD && tree->sons[0]->type != TYPE_FUNCTIONDEF && tree->sons[0]->type != TYPE_BLOCKWORD1LINE && tree->sons[0]->type != TYPE_ATOMICBLOCK)
            {
                
                res = execval(tree->sons[0]);

                if (CODE_ERROR > 1)
                {
                    printError(CODE_ERROR);
                    tree_destroy(tree);
                    continue;
                }
                else if (CODE_ERROR == 1) // quitte le terminal
                {
                    tree_destroy(tree);
                    return ;
                }
                
                printRes(res);

                storeAns(res); // stocke le résultat dans une variable au lieu de le supprimer
            }
            else if (tree->nbSons > 0)
            {
                execval(tree);
                if (CODE_ERROR > 1)
                {
                    printError(CODE_ERROR);
                    tree_destroy(tree);
                    continue;
                }
                else if (CODE_ERROR == 1) // quitte le terminal
                {
                    tree_destroy(tree);
                    return ;
                }
            }
            tree_destroy(tree);

        }

    return ;
}





void execFile(char* filename)
{
    
    char* program = openFile(filename); // fonction dépendant du système cible
    
    if (CODE_ERROR != 0)
        printError(CODE_ERROR);
    CODE_ERROR = 0;

    if (program == NULL)
        return ;
    
    //printf("Création de l'arbre syntaxique\n");

    FILENAME = strdup(filename);
    
    // exécution du fichier
    Tree* tree = tree_create(NULL, 0, 0);
    createSyntaxTree(tree, program);
    
    err_free(program);

    //printf("Arbre syntaxique terminé\n");

    if (CODE_ERROR != 0)
    {
        printError(CODE_ERROR);
        CODE_ERROR = 0;
        tree_destroy(tree);
        return;
    }


    execval(tree);
    
    if (CODE_ERROR > 1)
    {
        printError(CODE_ERROR);
    }

    tree_destroy(tree);

    return ;
}








void importFile(char* filename)
{
    char* program = openFile(filename); // fonction dépendant du système cible
    
    if (CODE_ERROR != 0)
        return;

    if (program == NULL) {
        CODE_ERROR = 67;
        return ;
    }

    char* sov = FILENAME;
    FILENAME = strdup(filename);
    
    // exécution du fichier
    Tree* tree = tree_create(NULL, 0, 0);
    createSyntaxTree(tree, program);
    err_free(program);

    if (CODE_ERROR != 0)
    {
        err_free(sov);
        tree_destroy(tree);
        return;
    }

    execval(tree);

    if (CODE_ERROR != 0) {
        err_free(sov);
        tree_destroy(tree);
        return;
    }

    err_free(FILENAME);
    FILENAME = sov;

    tree_destroy(tree);
    return ;
}




void defineVariables(void)
{
    // définition de la variable de version
    strlist_append(NOMS, strdup("__version__"));
    nelist_append(ADRESSES, neo_str_create(strdup(VERSION)));

    // définition de la variable de plateforme

    strlist_append(NOMS, strdup("__platform__"));
    nelist_append(ADRESSES, neo_str_create(strdup(PLATFORM)));

    strlist_append(NOMS, strdup("__name__"));
    nelist_append(ADRESSES, NULL);

    NAME = ADRESSES->len - 1; // l'adresse de __name__ à modifier


    // pi
    nelist_append(ADRESSES, neo_nb_create(number_fromDouble(PI)));
    strlist_append(NOMS, strdup("pi"));


}












void neonInit(void)
{
    #if defined(LINUX)
        linenoiseSetMultiLine(1); // spécial pour linenoise
        signal(SIGINT, handle_sigint);
    #endif
    
    #if defined(WINDOWS10) || defined(WINDOWS11)
        SetConsoleCtrlHandler(ctrlHandler, TRUE);
    #endif


    //définition de acceptedChars
    const char* acceptedChars_tab[31] = {"\"", "'", "+","*","-","/","<",">","=","%","&","@","!", ",", ";", "\n", "#", "$", "[", "]", "(", ")", "{", "}", "\\", ".", "_", " ", "\t", ".", ":"};
    strlist_copy(&acceptedChars, acceptedChars_tab, 31);



    // definit les types de tokens composes et indique par quel type de token compose ils peuvent etre precedes

    // les tableaux à l'intérieur ne sont pas des const, car sinon ça pose un problème avec le type strlist. Ce n'est pas grave, j'enlèverais ce tableau bientôt pour mettre quelque chose de moins "lourd"
    const intlist syntax_tab[21] = {
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_KEYWORDFUNCTION} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1, TYPE_NUMBER, TYPE_EXPRESSION, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_FONCTION, TYPE_BOOL, TYPE_LISTINDEX, TYPE_ENDOFLINE,  TYPE_OPERATOR, TYPE_PARENTHESE2, TYPE_CONST, TYPE_VIRGULE, TYPE_EXCEPTION} , .len = 15},//suivant la grammaire de chaque operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_CROCHET} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_BLOCKLINE, TYPE_KEYWORD, TYPE_FONCTION} , .len = 4},
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE,TYPE_PARENTHESE2} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_BLOCK} , .len = 2},
            (intlist) {.tab = (int[]) {TYPE_VIRGULE, TYPE_NUMBER, TYPE_EXPRESSION, TYPE_PARENTHESE1, TYPE_VARIABLE, TYPE_LISTINDEX, TYPE_BOOL, TYPE_OPERATOR, TYPE_LIST, TYPE_STRING, TYPE_BLOCKLINE, TYPE_BLOCK, TYPE_FONCTION, TYPE_PARENTHESE2, TYPE_KEYWORD, TYPE_CONST, TYPE_BLOCKWORD1LINE, TYPE_EXCEPTION} , .len = 18}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_VIRGULE, TYPE_PARENTHESE1} , .len = 4},
            (intlist) {.tab = (int[]) {TYPE_OPERATOR, TYPE_NUMBER, TYPE_EXPRESSION, TYPE_STRING, TYPE_LIST, TYPE_FONCTION, TYPE_LISTINDEX, TYPE_VARIABLE, TYPE_PARENTHESE2,TYPE_BOOL} , .len = 10},
            (intlist) {.tab = (int[]) {TYPE_NUMBER, TYPE_EXPRESSION, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_FONCTION, TYPE_BOOL, TYPE_LISTINDEX, TYPE_ENDOFLINE, TYPE_OPERATOR, TYPE_CONST, TYPE_EXCEPTION} , .len = 12},
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_BLOCK} , .len = 2},
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE}, .len = 1},
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE}, .len = 1},
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE}, .len = 4},
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_CROCHET} , .len = 5}
    };


    listlist_copy(&syntax, syntax_tab, 21);

    // types acceptés en début d'entrée

    
    int types_debut_temp[21] = {TYPE_FONCTION, TYPE_PARENTHESE1, TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_LISTINDEX, TYPE_NUMBER, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_BOOL, TYPE_BLOCKLINE, TYPE_KEYWORD, TYPE_CONST, TYPE_WORD, TYPE_COMMENT, TYPE_CROCHET, TYPE_BLOCKWORD, TYPE_BLOCKWORD1LINE, TYPE_EXPRESSION, TYPE_EXCEPTION};
    intlist_copy(&types_debut, types_debut_temp, 21);

    int types_fin_temp[20] = {TYPE_FONCTION, TYPE_PARENTHESE2, TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_LISTINDEX, TYPE_NUMBER, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_BOOL, TYPE_KEYWORD, TYPE_CONST, TYPE_WORD, TYPE_COMMENT, TYPE_CROCHET, TYPE_EXPRESSION, TYPE_EXCEPTION, TYPE_BLOCK, TYPE_BLOCKWORD1LINE};
    intlist_copy(&types_fin, types_fin_temp, 20);


    //définition de sousop
    const char* sousop_temp[] = {"+","*","-","/","<",">","=","%","&","@","!", ".",":"};
    strlist_copy(&sousop, sousop_temp, 13);


    // liste qui contient les opérandes prises par les opérateurs, correspond a operateurs3
    int gramm1_temp[NBOPERATEURS] = {RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,RIGHT, RIGHT, VARRIGHT, LEFT_VAR, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, VAR_VAR, VAR_RIGHT, SPECIAL, RIGHT};
    intlist_copy(&gramm1, gramm1_temp, NBOPERATEURS);



    //définition de operateurs3
    const char* operateurs3_temp[NBOPERATEURS] = {"and","or","xor","+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","not","%","//","&","<-","@", "_", "del", "->", ".", "EE", "=>", "in", ">>", ":", "<->", ":=", "...", "parallel"};
    strlist_copy(&operateurs3, operateurs3_temp, NBOPERATEURS);


    //définition de operateurs1
    // opérateurs composés de caractères de sousop
    const char* operateurs1_temp[] = {"+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","%","//","&","<-","@", "->", ".", "=>", ">>", ":", "<->", ":=", "..."};
    strlist_copy(&operateurs1, operateurs1_temp, 31);


    // opérateurs mots
    const char* operateurs2_temp[] = {"and","or","xor","not", "del", "EE", "in", "parallel"};
    strlist_copy(&operateurs2, operateurs2_temp, 8);



    const char* OPERATEURS_temp[NBOPERATEURS] = {"and","or","xor","+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","not","%","//","&","<-","@", "_", "del", "->", ".", "EE", "=>", "in", ">>", ":", "<->", ":=", "...", "parallel"};
    strlist_copy(&OPERATEURS, OPERATEURS_temp, NBOPERATEURS);


    const int PRIORITE_temp[NBOPERATEURS] = {8, 8, 8, 5, 4, 5, 4, 3, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 9, 3, 3, 3, 4, 4, 2, 9, 2, 1, 9, 9, 2, 4, 8, 6, 0, 9, 9, 9, 9, 8};
    intlist_copy(&PRIORITE, PRIORITE_temp, NBOPERATEURS);

    const int OPERANDES_temp[NBOPERATEURS] = {RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,RIGHT, RIGHT, VARRIGHT, LEFT_VAR, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, VAR_VAR, VAR_RIGHT, 0, RIGHT};
    intlist_copy(&OPERANDES, OPERANDES_temp, NBOPERATEURS);


    // ------------------

    // défnit les tokens de délimitation de blocks

    //mots de blocs d'instructions
    const char* blockwords_temp[] = {"if","while","for", "ei", "return", "import", "local", "tr", "expt", "await", "atmc"};
    strlist_copy(&blockwords, blockwords_temp, 11);

    const char* blockwords1Line_temp[] = {"return", "import", "local", "await"};
    strlist_copy(&blockwords1Line, blockwords1Line_temp, 4);

    const char* keywordFunction_temp[] = {"function", "method"};
    strlist_copy(&keywordFunction, keywordFunction_temp, 2);

    const char* keywords_temp[] = {"es", "continue", "break", "pass", "tr", "atmc"};
    strlist_copy(&keywords, keywords_temp, 6);

    const char* lkeywords_temp[] = {"continue", "break", "pass"};
    strlist_copy(&lkeywords, lkeywords_temp, 3);


    const char* neon_boolean_temp[] = {"True","False"};
    strlist_copy(&neon_boolean, neon_boolean_temp, 2);

    const char* exceptions_temp[] = {
        "SyntaxError",
        "FileNotFound",
        "UnmeasurableObject",
        "UndefinedVariable",
        "IncorrectFunctionCall",
        "MemoryError",
        "NonIndexableObject",
        "IncorrectListIndex",
        "OutOfRange",
        "IncorrectType",
        "DivisionByZero",
        "UnknownError",
        "AssertionFailed",
        "DefinitionError",
        "KeyboardInterrupt"
    };
    // ici il faut s'assurer que capacity a la bonne valeur
    exceptions.tab = err_malloc(sizeof(char*)*16);
    exceptions.len = 15;
    exceptions.capacity = 4;
    for (int i = 0 ; i < 15 ; i++)
        exceptions.tab[i] = strdup(exceptions_temp[i]);
    

    const char* constant_temp[] = {"None", "Infinity", "NaN"};
    strlist_copy(&constant, constant_temp, 3);




    /*----- Préparation des fonctions ------*/
    const char* NOMSBUILTINSFONC_temp[NBBUILTINFONC] = {"print","input","nbr","str","len","sub","exit","append","remove","insert","type", "reverse", "eval","clear","help", "randint", "failwith", "time", "assert", "output", "chr", "ord", "listComp", "createException", "exception", "int", "index", "replace", "count", "list", "sortAsc", "sortDesc", "sin", "cos", "tan", "deg", "rad", "sqrt", "ln", "exp", "log", "log2", "round", "abs", "ceil", "floor", "readFile", "writeFile", "setFunctionDoc", "setAtomicTime", "copy"};
    strlist_copy(&NOMSBUILTINSFONC, NOMSBUILTINSFONC_temp, NBBUILTINFONC);

    const int typesRetour[NBBUILTINFONC] = {TYPE_NONE, TYPE_STRING, TYPE_NUMBER, TYPE_STRING, TYPE_NUMBER, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_STRING, TYPE_STRING, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NUMBER, TYPE_NONE, TYPE_NUMBER, TYPE_NONE, TYPE_NONE, TYPE_STRING, TYPE_NUMBER, TYPE_LIST, TYPE_EXCEPTION, TYPE_NONE, TYPE_NUMBER, TYPE_NUMBER, TYPE_STRING, TYPE_NUMBER, TYPE_LIST, TYPE_NONE, TYPE_NONE, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_ANYTYPE};

    const char* helpbuiltinsfonc[NBBUILTINFONC] = {
        "Displays arguments in the terminal",
        "Displays the given argument and prompts the user to input in the terminal. Returns the entered value.",
        "Converts a string of characters into a number.",
        "Converts a number into a string of characters.",
        "Returns the length of an object.",
        "Performs substring extraction. sub(\"hello\", 1, 4) returns \"onj\".",
        "Exits the Neon interpreter.",
        "Adds an element to a list.",
        "Removes an element from a list.",
        "Inserts an element into a list.",
        "Returns the type of an object.",
        "Reverses the characters in a string of characters.",
        "Evaluates the given string of characters and returns the result.",
        "Clears the terminal.",
        "Displays help information related to a function.",
        "Returns a random integer between two integers.",
        "Displays the provided text in the terminal and exits the program.",
        "Returns the time elapsed since 1970 in seconds.",
        "Stops the program if at least 1 assertion has failed.",
        "Puts the given arguments on the terminal without spaces or \\n.",
        "Returns the ASCII character associated to the number.",
        "Returns the ASCII value of the given character.",
        "Arguments : index, start, end, step, condition, value\nEvaluates value for all indices between start and end according to step, and adds it to the list if condition is true.\nindex must be the name of the variable used, and condition and value must be string expressions.",
        "Creates an Exception object with the given name.",
        "Exits the program with the given exception.",
        "Takes the integer part of a number.",
        "Returns the index of an element in a list.",
        "replace(str, str1, str2) returns a string in which all occurrences of str1 have been replaced by str2 in str.",
        "Counts the number of occurrences in a list or a string.",
        "Separates every character of a string in a list.",
        "Sorts a list in ascending order.",
        "Sorts a list in descending order.",
        "sin: Sine function",
        "cos: Cosine function",
        "tan: Tangent function",
        "deg: Convert angle from radians to degrees",
        "rad: Convert angle from degrees to radians",
        "sqrt: Square root function",
        "ln: Natural logarithm (base e)",
        "exp: Exponential function (e^x)",
        "log: Logarithm (base 10)",
        "log2: Logarithm (base 2)",
        "round: Round to the nearest decimal number with the given precision",
        "abs: Absolute value",
        "ceil: Ceiling function (smallest integer greater than or equal to)",
        "floor: Floor function (largest integer less than or equal to)",
        "Returns the content of the file whose name was given",
        "Writes the given string in the file whose name was given. The syntax is writeFile(name, content)",
        "Sets a string documentation for a user-defined function or method",
        "Time to allow for each process before switching",
        "Performs a deep copy of an object, preserving the pointer dependencies"
    };


    //indique le nombre de premiers arguments à ne pas copier
    const int typesbuiltinsfonc[NBBUILTINFONC] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}; // le nombre d'arguments "méthode"


    const int nbargs[NBBUILTINFONC] = {-1,-1,1,1,1,3,0,2,2,3,1,1,1,0,-1,2,1,0,-1,-1,1,1,6,1,1,1,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,2,2,1,1}; // nombre d'arguments en tout

    // les fonctions ayant un nombre illimité d'arguments ne doivent avoir qu'un seul élément dans typeArgs
    const int* typesArgs[NBBUILTINFONC] = {
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_STRING},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_STRING, TYPE_NUMBER, TYPE_NUMBER},
        NULL,
        (int[]){TYPE_LIST, TYPE_ANYTYPE},
        (int[]){TYPE_LIST, TYPE_NUMBER},
        (int[]){TYPE_LIST, TYPE_ANYTYPE, TYPE_NUMBER},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_STRING},
        (int[]){TYPE_STRING},
        NULL,
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_NUMBER, TYPE_NUMBER},
        (int[]){TYPE_STRING},
        NULL,
        (int[]){TYPE_BOOL},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_STRING},
        (int[]){TYPE_STRING, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_STRING, TYPE_STRING},
        (int[]){TYPE_STRING},
        (int[]){TYPE_EXCEPTION},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_LIST, TYPE_ANYTYPE},
        (int[]){TYPE_STRING, TYPE_STRING, TYPE_STRING},
        (int[]){TYPE_ANYTYPE, TYPE_ANYTYPE},
        (int[]){TYPE_STRING},
        (int[]){TYPE_LIST},
        (int[]){TYPE_LIST},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER, TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_STRING},
        (int[]){TYPE_STRING, TYPE_STRING},
        (int[]){TYPE_ANYTYPE, TYPE_STRING},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_ANYTYPE}
    };

    /* ----- fin préparation des fonctions ------*/


    // tableau des messages d'erreur :

    const char* ERRORS_MESSAGES_temp[NB_ERRORS] = {
        "",// ne doit pas être utilisé
        "",// ne doit pas être utilisé
        "Multiple decimal points in the same number",
        "File doesn't exist",
        "This object has no length",
        "Use of an undefined variable",
        "Too many arguments in user-defined function call",
        "Insufficient arguments in user-defined function call",
        "Call to an undefined function",
        "Call to an invalid function",
        "Starting value of the for loop is not a number",
        "Incorrect function definition",
        "Memory error",
        "Incorrect index for subtree deletion",
        "Invalid arguments provided in function call",
        "This object is not indexable",
        "List index is not a number",
        "Attempt to index an undefined list",
        "List index out of range",
        "Undetermined error, likely a syntax issue",
        "Unevaluable condition due to incorrect type",
        "Multiple arguments for return statement",
        "'For' loop variant is not a variable",
        "Incorrect built-in function name in function call",
        "Unknown operator",
        "Unknown character",
        "String, list, or other structure not terminated",
        "Unclosed string at the end of line",
        "Incompatible keyword used in an expression",
        "Block instruction line not followed by an instruction block (e.g., a lone if() statement)",
        "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
        "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
        "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
        "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
        "Lone block of instructions",
        "Incorrect statement definition",
        "Incorrect number of arguments when calling a built-in function",
        "Index out of range for inserting into a NeList",
        "Index out of range for deleting from a NeList",
        "Index out of range to access in a NeList",
        "Unsupported types for addition",
        "Unsupported types for subtraction",
        "Unsupported types for division",
        "Unsupported types for multiplication",
        "Unsupported types for modulo",
        "Euclidean division by zero",
        "Unsupported types for Euclidean division",
        "Unsupported types for 'and' operator",
        "Unsupported types for 'or' operator",
        "Unsupported types for 'xor' operator",
        "Unsupported types for exponentiation",
        "Unsupported types for equality tests",
        "Unsupported types for <=",
        "Unsupported types for >=",
        "Unsupported types for <",
        "Unsupported types for >",
        "Attempt to assign the value of an undefined variable to another variable",
        "Improper use of the '->' operator",
        "Unsupported types for 'not' operator",
        "Referencing an undefined variable",
        "Dereferencing something other than a string",
        "Unsupported types for unary minus operator",
        "Unsupported types for reverse function",
        "Attempt to use help on a non-function variable",
        "The randint function must have two positive integers a and b as arguments, with a < b",
        "Pointer is not allocated",
        "Undefined error",
        "Can not open file",
        "More closing parentheses, curved brackets or square brackets than opening ones",
        "local must have at least one argument",
        "local can only be used in a function or in a method",
        "Assertion failed",
        "The '.' operator is only for functions or methods and can only be applied on a variable",
        "Incorrect hexadecimal or binary number",
        "", // code 74 spécial pour les appels d'exceptions
        "Unsupported types for implication.",
        "except block not preceded by a try block.",
        "try block not followed by an except block",
        "except expects exceptions",
        "in can only be calculated with a list",
        "'>>' is only for containers",
        "A container field is set but the container is not used",
        "Unknown container field",
        "Same container name but not same fields",
        "Containers must contain at least one field",
        "Unknown container type",
        "Two different fields cannot have the same name in a container",
        "Incorrect container definition",
        "Object not in list",
        "Cannot execute a method on a litteral object",
        "That list cannot be sorted",
        "Cannot call a function while dereferencing a container field",
        "':' outside container definition",
        "Unknown argument name in user-defined function call",
        "You have already given a value to this argument in user-defined function call",
        "':=' outside function call or function definition",
        "Bad arguments definition in function definition",
        "Use of '...' outside function definition",
        "Can only set a documentation for a user-defined function or method",
        "Use of return outside function",
        "Can only call a user-defined function in parallel",
        "await takes exactly one argument",
        "Atomic time must be a positive integer, greater or equal to 1",
        "Methods cannot be applied to volatile objects such as function or operators",
        "Program interrupted", // code 104 spécial pour intercepter un CTRL-C
        "Cannot assign a value to a string index"
    };

    strlist_copy(&ERRORS_MESSAGES, ERRORS_MESSAGES_temp, NB_ERRORS);

    const int exceptions_err_temp[NB_ERRORS] = {
        0,// ne doit pas être utilisé
        0,// ne doit pas être utilisé
        0,
        1,
        2,
        3,
        4,
        4,
        3,
        4,
        0,
        0,
        5,
        11,
        4,
        6,
        7,
        3,
        8,
        0,
        9,
        0,
        9,
        4,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        4,
        8,
        8,
        8,
        9,
        9,
        9,
        9,
        9,
        10,
        9,
        9,
        9,
        9,
        9,
        9,
        9,
        9,
        9,
        9,
        3,
        9,
        9,
        3,
        9,
        9,
        9,
        9,
        9,
        5,
        11,
        1,
        0,
        0,
        9,
        12,
        9,
        0,
        0,
        9,
        0,
        0,
        9,
        9,
        9,
        5,
        3,
        13,
        13,
        13,
        13,
        0,
        8,
        4,
        9,
        0,
        0,
        4,
        4,
        0,
        0,
        0,
        9,
        0,
        0,
        0,
        8,
        4,
        14,
        9
    };

    intlist_copy(&exceptions_err, exceptions_err_temp, NB_ERRORS);



    // ----- fin messages d'erreur

    CONTAINERS = strlist_create(0);
    ATTRIBUTES = nelist_create(0);


    PROMISES = nelist_create(0);
    PROMISES_CNT = intlist_create(0);
    PROCESS_FINISH = intlist_create(0);


    VAR_LOC = ptrlist_create();

    FONCTIONS = tree_create(NULL, 0, 0);
    ADRESSES = nelist_create(0);
    NOMS = strlist_create(0);
    compFunc(typesbuiltinsfonc, helpbuiltinsfonc, nbargs, typesArgs, typesRetour);

    srand(time(NULL));

    // définition des variables de base

    defineVariables();

    return;
}


void neonExit(void)
{
    //destruction de tout ce qui a été alloué.

    err_free(VAR_LOC);

    neobject_destroy(RETURN_VALUE, true);

    // il faut destroy tout
    strlist_destroy(&acceptedChars, false);
    listlist_destroy(&syntax);
    strlist_destroy(&sousop, false) ;
    err_free(gramm1.tab) ;
    err_free(types_debut.tab);
    err_free(types_fin.tab);
    strlist_destroy(&operateurs3, false);
    strlist_destroy(&operateurs1, false);
    strlist_destroy(&operateurs2, false);
    strlist_destroy(&blockwords, false);
    strlist_destroy(&blockwords1Line, false);
    strlist_destroy(&keywords, false);
    strlist_destroy(&lkeywords, false);
    strlist_destroy(&keywordFunction, false);
    strlist_destroy(&neon_boolean, false);
    strlist_destroy(&exceptions, false);
    strlist_destroy(&constant, false);
    strlist_destroy(&OPERATEURS, false);
    err_free(PRIORITE.tab);
    err_free(OPERANDES.tab);
    err_free(exceptions_err.tab);
    strlist_destroy(&NOMSBUILTINSFONC, false);
    strlist_destroy(&ERRORS_MESSAGES, false);

    tree_destroy(FONCTIONS);

    strlist_destroy(CONTAINERS, true);
    nelist_destroy(ATTRIBUTES, true);

    
    strlist_destroy(NOMS, true);
    nelist_destroy(ADRESSES,true);

    nelist_destroy(PROMISES, true);
    err_free(PROMISES_CNT.tab);
    err_free(PROCESS_FINISH.tab);

    if (FILENAME != NULL)
        err_free(FILENAME);

    /*printString("Fuites mémoire détectées : ");
    newLine();
    ptrlist_aff(ALLOCPTR);
    ptrlist_destroy(ALLOCPTR, false, true);*/

}




#ifdef TI83PCE
    int main(void)
#else
    int main (int argc, char* argv[])
#endif
{

    //stockage des variables
    neonInit();


    // définition de la liste des arguments
    NeObject* l = neo_list_create(0);
    updateFileName(strdup("__main__")); // nom du fichier actuel

    #ifdef TI83PCE
        // récupération du nom de fichier si existant
        nio_init(&console, NIO_MAX_COLS, NIO_MAX_ROWS, 0, 0, NIO_COLOR_BLACK, NIO_COLOR_WHITE, true);
        nio_set_default(&console);

        char* filename = argsAns();

        nelist_append(ADRESSES,l);
        strlist_append(NOMS,strdup("__args__"));

        if (filename == NULL) // lance le terminal
        {
            startMessage();
            terminal();
        }
        else
        {
            execFile(filename);
            err_free(filename);
        }
        neonExit();
    #else
        #ifndef WASM

            // ajout des arguments dans le tableau contenant les arguments du programme
            for (int i = 2 ; i < argc ; i++)
                neo_list_append(l,neo_str_create(strdup(argv[i])));

            nelist_append(ADRESSES,l);
            strlist_append(NOMS,strdup("__args__"));

            updateFileName(strdup("__main__"));

            if (argc >= 2)
            {
                execFile(argv[1]);
            }
            else
            {
                startMessage();
                terminal();
            }

            neonExit();
        #endif
    #endif


    #ifdef TI83PCE
        nio_free(&console);
    #endif

    return 0;
}
