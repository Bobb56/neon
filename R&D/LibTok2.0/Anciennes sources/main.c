#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
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

#if defined(LINUX) || defined(WINDOWS11) || defined(WINDOWS10)
    #include <signal.h>

    void handle_sigint(int sig) {
        CODE_ERROR = 104;
    }
#endif


/*
Liste des choses qui marchent pas :
le pb que je viens de régler permet d'appeler des méthodes sur des @. C'est cool prcq sinon ça craint, mais il faut faire
gaffe qu'on puisse pas appeler des méthodes sur des trucs invalides

Lignes 1398 et 1434 commentées : double deleteContext()

_____________________




Avancement et choses à faire :
> vérifier les priorités de tous les opérateurs
> Fonction show_code pour voir le code d'une fonction non built-ins ?

Procédure pour ajouter une fonction built-in :
1) Programmer cette fonction selon le prototype NeObject* _maFonction_ (NeList* args)
4) Ajouter 1 à la constante pré-processeur NBBUILTINFONC définie au début de neon.h
3) Ajouter le prototype de cette fonction tout à la fin de neon.h
4) Ajouter la fonction dans le tableau de fonctions au début de ce fichier
5) Compléter les informations sur la fonction dans neonInit

NOUVEAUTéS depuis la dernière publication :
- Ajout d'un opérateur de swap <->
- Ajout de la syntaxe := pour les fonctions, et tout ce qui en découle
Tous les arguments sont remplis dans l'ordre, à exception des arguments dont la variable est explicitement spécifiée
- Ajout de la syntaxe ... et de __local_args__ pour les fonctions à nombre illimité de paramètres
On peut définir des arguments après ..., mais ces arguments ne pourront être atteints qu'avec la syntaxe x := y. D'ailleurs il ne fait
pas de sens de définir des arguments obligatoires après ...
- Ajout de la fonction setFunctionDoc, et extension de la fonction help pour les fonctions définies par l'utilisateur
- Les boucles for rendent automatiquement leur indice local. Ca permet de faire des boucles for imbriquées avec la même variable,
ou de na pas avoir de soucis sur des appels récursifs qui font des boucles for, et où le programmeur aurait oublié de mettre
l'indice en local. On ne peut également pas modifier l'indice d'une boucle for
- On peut, par la même occasion nous-même rendre des variables locales dans les if
- Possibilité de lancer des fonctions en parallèle avec le mot-clé parallel
- Attente passive dans un processus avec await(condition)
- Execution de blocs d'instructions atomiques
*/

// promises associe aux identifiants des processus leur valeur tandis que promises_cnt compte le nombre de promises d'un même processus non transformées en valeur
int ATOMIC_TIME = 1; // le nombre d'instructions exécuté sur chaque processus avant de passer au suivant

NeList* PROMISES = NULL;
intlist PROMISES_CNT;
intlist PROCESS_FINISH; // une liste de booléens qui indique si un processus a fini pour savoir si on peut lui piquer sa place

void* PTRERROR = NULL; // pointeur qui contient soit un pointeur d'arbre soit une chaine de caractères à propos de l'endroit où est apparue une erreur

char* EXCEPTION = NULL; // exception levée par l'utilisateur, à afficher par printError

int NAME = 0; // adresse du NeObject représentant le nom actuel de fichier. A ne pas utiliser, il y a à la place la fonction updateFileName


strlist ERRORS_MESSAGES;

ptrlist* VAR_LOC; // c'est une pile de pile qui contiendra dans chaque contexte les variables à supprimer


NeObject* RETURN_VALUE = NULL; // contient la valeur de retour de la fonction si le programme a été interrompu par un return ()

strlist* CONTAINERS; // à un indice du tableau, associe le nom du type de container
NeList* ATTRIBUTES; // contient pour chaque type de container, une liste des noms de champs

Tree* FONCTIONS; // cet arbre va contenir en fils tous les arbres de fonction des fonctions et des méthodes définies par l'utilisateur

ptrlist* allocptr; // stocke tous les pointeurs alloués depuis le début
ptrlist* freeptr; // stocke tous les pointeurs libérés depuis le début

strlist* NOMS; // contient les noms des variables associés à leurs adresses mémoire dans ADRESSSES
NeList* ADRESSES;


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
strlist boolean;
strlist exceptions;
strlist constant;
strlist OPERATEURS;
intlist PRIORITE;
intlist OPERANDES;
intlist exceptions_err;

strlist NOMSBUILTINSFONC;





//créer le tableau de fonctions
NeObject* (*OPFONC[NBOPERATEURS])(NeObject*,NeObject*)={_and,_or,_xor,_add,_mul,_sub,_div,_pow,_equal,_notEq,_infEqual,_supEqual,_inf,_sup,_affectNone,_addEqual,_subEqual,_mulEqual,_divEqual,_incr,_decr,_not,_mod,_eucl,_ref,_goIn,NULL, _minus, _del, _affect, NULL, _exponent, _implique, _in, NULL, NULL, _swap, NULL, NULL};


NeObject* (*BUILTINSFONC[NBBUILTINFONC])(NeList*)={_print_,_input_,_nbr_,_str_,_len_,_substring_,_exit_,_append_,_remove_,_insert_,_type_, _reverse_, _eval_,_clear_,_help_, _randint_,_quitError_, _time_, _assert_, _output_, _chr_, _ord_, _list_comp_, _create_exception_, _exception_, _int_, _index_, _replace_, _count_, _list_, _sort_asc_, _sort_desc_, _sin_, _cos_, _tan_, _deg_, _rad_, _sqrt_, _ln_, _exp_, _log_, _log2_, _round_, _abs_, _ceil_, _floor_, _readFile_, _writeFile_, _setFunctionDoc_, _setAtomicTime};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////




void updateFileName(char* name)
{
    neobject_destroy(ADRESSES->tab[NAME], true);
    ADRESSES->tab[NAME] = neo_str_create(name);

    return ;
}










int indexListIndex(char* listindex)
{
  int i = 0;
  while (listindex[i]!='[')
  {
    i++;
  }
  char* indexstr=sub(listindex,i+1,strlen(listindex)-1);
  
  int index=atoi(indexstr);
  err_free(indexstr);
  return index;
}



NeObject* evalAux(Tree* tree, _Bool calcListIndex) {
    return NULL;
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
    var->data = NULL;
    var->type = TYPE_EMPTY;

    //ajout de la sauvegarde au contexte actuel
    
    ptrlist_append((ptrlist*)(VAR_LOC->tete), (void*)ns);
}






_Bool isExpression(Tree* tree) {
    return (tree->type == TYPE_STRING || tree->type == TYPE_OPERATOR || tree->type == TYPE_NUMBER || tree->type == TYPE_VARIABLE || tree->type == TYPE_LIST || tree->type == TYPE_FONCTION || tree->type == TYPE_LISTINDEX || tree->type == TYPE_CONTAINER || tree->type == TYPE_ATTRIBUTE || tree->type == TYPE_BOOL || tree->type == TYPE_EXCEPTION || tree->type == TYPE_NONE);
}



void return_int_execval(ptrlist* valstack, int value) {
    intptr_t val2 = (intptr_t)value;
    ptrlist_append(valstack, (void*)val2);
}

void return_neo_execval(ptrlist* valstack, NeObject* value) {
    ptrlist_append(valstack, (void*)value);
}

void get_arguments_expr(Tree** tree, intptr_t* calcListIndex, void** cadre) {
    *tree = cadre[0];
    *calcListIndex = (intptr_t)cadre[1];
}

void get_tree(Tree** tree, void** cadre) {
    *tree = cadre[0];
    return ;
}

void stack_args_expr(Tree* tree, intptr_t calcListIndex, ptrlist* varstack) {
    void** cadre = err_malloc(sizeof(void*) * NBLOCALVARS);
    cadre[0] = (void*)tree;
    cadre[1] = (void*)calcListIndex;
    ptrlist_append(varstack, (void*)cadre);
    return ;
}



void stack_args(Tree* tree , ptrlist* varstack) {
    void** cadre = err_malloc(sizeof(void*) * NBLOCALVARS);
    cadre[0] = (void*)tree;
    ptrlist_append(varstack, (void*)cadre);
    return ;
}

void stack_ret_zone(intptr_t retzone, ptrlist* retstack) {
    ptrlist_append(retstack, (void*)retzone);
}



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
        void* data_temp = ns->data; // on garde une copie de la sauvegarde de notre variable
        char type_temp = ns->type;

        ns->data = ns->object->data; // on sauvegarde la valeur actuelle
        ns->type = ns->object->type;

        ns->object->data = data_temp; // on restaure la copie de notre variable
        ns->object->type = type_temp;
    }
}




ProcessCycle* nextProcess(ProcessCycle* pc, ptrlist** varstack, ptrlist** valstack, ptrlist** retstack, ptrlist** variables_a_sauvegarder) {
    
    if (pc->next != NULL) {
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






Process* processCycle_add(ProcessCycle* pc, Tree* tree, int id) { // renvoie un pointeur vers le processus créé

    // on crée le nouveau processus
    Process* p = err_malloc(sizeof(Process));
    p->varstack = ptrlist_create();
    p->valstack = ptrlist_create();
    p->retstack = ptrlist_create();
    p->var_loc = ptrlist_create();
    p->id = id;
    p->wait_expr = NULL;
    p->varsToSave = ptrlist_create();

    // on envoie évidemment les arguments
    stack_args_expr(tree, 1, p->varstack);

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


ProcessCycle* processCycle_remove(ProcessCycle* pc, ptrlist** varstack, ptrlist** valstack, ptrlist** retstack, ptrlist** variables_a_sauvegarder) {
    Process* p = pc->process;

    
    if (p->id != -1) { // c'est pas thread principal
        err_free(p->var_loc); // on suppose que tous les contextes créés dans le cadre de ce processus ont bien été supprimés
    }


    ptrlist_destroy(p->valstack, false, true);
    ptrlist_destroy(p->varstack, false, true);
    ptrlist_destroy(p->retstack, false, true);

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
    Cette fonction combine les deux fonction exec et eval. Elle détecte le type de l'arbre et évalue l'expression si c'est une expression
    ou exécute le code si ce sont des instructions
    La valeur de retour de la fonction est un NeObject* dans le cas d'une expression et un intptr_t casté en NeObject* dans le cas de exec
    Cette fonction est entièrement écrite en impératif, sans aucun appel récursif afin de pouvoir implémenter un entrelacement et
    un changement de processus
    Ainsi, voici la structure de la fonction :

    while (fini d'évaluer l'abre) {
        label0:
        zone_retour = 0
        label1:
        switch (zone_retour) {
            // ici, fait des trucs
        }
        label2:
        zone_retour <- dernier endroit où il faut aller
        Renvoyer le résultat si il n'y a aucun endroit où retourner
        Sinon, dépiler le dernier cadre de pile
        goto label1
    }

    Pour effectuer un appel récursif, il faut empiler l'arbre à évaluer dans la pile d'arguments,et empiler le case du switch
    auquel il faut retourner, puis sauter au début de la boucle
    Celle-ci va trouver où il faut aller.
    Pour renvoyer une valeur, il faut l'empiler sur la pile de retour, puis sauter à label2. Le label2 va lui-même affecter
    la zone de retour à la dernière zone où il faut retourner, puis sauter en label1 pour effectuer le switch sur la zone de retour

    Pour l'entrelacement, il suffit de stocker les trois piles associées à chaque processus dans une liste ou un tableau,
    puis d'affecter les trois piles aux piles qu'il faut aux variables de piles. En effet, toute l'information (variables
    locales, arguments, retour...) est stockée dans ces piles. La boucle ne fait qu'interpréter la valeur des piles
    Chaque processus possède une NeList des NeObject à affecter une fois le résultat calculé
    
    
    Les case vont jusqu'à 35
    */

    // les variables utilisées avant et après des appels récursifs sont déclarées ici pour exister même après les goto
    NeObject * op1, * op2, *neo_liste, *expr, *var, *valeur, *nom, *args, *mainThreadReturnValue = NULL;
    NeObject* un = neo_nb_create(number_fromDouble(1));
    NeList* l, *liste, *val;
    Function* fun;
    intptr_t ext_index = 0, int_ret = 0, bloc = 0, inst = 0; // pour les boucles for qui contiennent des goto
    Tree* maintree = NULL, *arguments;
    _Bool cond = false, elif = false;
    intptr_t max;
    void** cadre;

    intptr_t calcListIndex = 1;
    intptr_t zone_retour;

    ptrlist* var_loc_sov = VAR_LOC;


    // création de la chaîne de processus
    ProcessCycle* process_cycle = processCycle_create();

    // ajout du processus principal, reconnaissable car il n'a pas de promesses
    processCycle_add(process_cycle, tree, -1); // le processus principal a pour id : -1

    ptrlist* variables_a_sauvegarder = process_cycle->process->varsToSave;

    ptrlist* varstack = process_cycle->process->varstack;
    ptrlist* valstack = process_cycle->process->valstack;
    ptrlist* retstack = process_cycle->process->retstack;
    err_free(process_cycle->process->var_loc); // on utilise directelent VAR_LOC de base pour le thread principal
    process_cycle->process->var_loc = VAR_LOC;
    
    intptr_t atomic_counter = ATOMIC_TIME;

    tree = NULL;

    while (!ptrlist_isEmpty(varstack)) { // boucle principale. On reste dedans tant que l'arbre entier n'a pas été exécuté (ou évalué)
        debut_boucle:

        
        // on est sûr que l'endroit où l'on reprend le processus est l'endroit où il avait été mis en pause
        if (atomic_counter == 0) { // c'est le moment où l'on change de processus, juste après un appel récursif
            process_cycle = nextProcess(process_cycle, &varstack, &valstack, &retstack, &variables_a_sauvegarder);

            atomic_counter = ATOMIC_TIME;

            if (process_cycle->process->wait_expr != NULL) // le processus est actuellement en attente passive
            {
                goto check_condition;
            }
        }
        
        atomic_counter--;


        if (CODE_ERROR != 0) // il y a déjà une erreur, donc on retourne en arrière
        {
            return_int_execval(valstack, 0);
            goto fin_boucle;
        }

        zone_retour = 0;

        // on dépile l'arbre à traiter
        cadre = varstack->tete; // cadre existe, on récupère la valeur de tree et éventuellement calcListIndex
        // mais après un appel récursif il faut les re-récupérer

        get_tree(&tree, cadre);


        label_return: // pour revenir après un appel


        switch (zone_retour) {

            case 0: // aucun endroit de retour spécifié

            // disjonction de cas suivant si c'est une expression ou des instructions

            if (isExpression(tree)) { // on arrive dans le eval

                get_arguments_expr(&tree, &calcListIndex, cadre);

                if (tree->type == TYPE_OPERATOR) // traité
                {

                    if (!calcListIndex && (long int)number_toDouble(tree->label2) != 26)
                    {
                        CODE_ERROR = 103;
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }

                    // il faut envoyer les opérandes calculées à la fonction opérateur
                    if (tree->nbSons == 1) // operateur unaire
                    {

                        if ((long int)number_toDouble(tree->label2) == 39) { // opérateur parallel
                            // C'est un appel de fonction déguisé. On va appeler la fonction mais dans un nouveau thread
                            
                            maintree = tree->sons[0];

                            if (maintree->data->type != TYPE_USERFUNC) {
                                CODE_ERROR = 100;
                                return_neo_execval(valstack, NULL);
                                goto fin_boucle;
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

                            Process* p = processCycle_add(process_cycle, maintree, id);
                            stack_ret_zone(6, p->retstack);
                            //stack_args(maintree, p->varstack);

                            // on a ajouté le processus à la liste de processus, donc maintenant y a plus qu'à attendre qu'un jour on revienne en case 6

                            return_neo_execval(valstack, neo_promise_create((intptr_t)id)); // plus tard, ce sera une promesse renvoyée par le parallel
                            
                            goto fin_boucle;
                            

                            case 6: // quand le processus sera fini on sera ici

                            // peut etre faire quelque chose si CODE_ERROR != 0

                            // ici, on recommence complètement le processus suivant, ce qui n'est pas du tout normal

                            id = process_cycle->process->id; // on a fait plein de trucs donc il faut reprendre le bon id

                            //err_free(ptrlist_pop(varstack)); // on supprime le NULL avec lequel on est arrivé

                            // si on se retrouve ici c'est que le processus actuellement exécuté est notre processus, donc
                            // process_cycle->process est notre processus
                            // maintenant on supprime le processus, et on affecte les variables à affecter
                            NeObject* result = (NeObject*)ptrlist_pop(valstack);

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
                                // cette fonction a également chargé le nouveau processus dans les variables et a renvoyé le nouveau processus
                                goto debut_boucle; // on continue l'exécution sur le processus d'après
                            }


                        }


                        // récupération de la grammaire de l'opérateur
                        if (OPERANDES.tab[(int)number_toDouble(tree->label2)] == VARRIGHT || OPERANDES.tab[(int)number_toDouble(tree->label2)] == VARLEFT)
                        {
                            PTRERROR = tree;

                            stack_args_expr(tree->sons[0], 0, varstack); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
                            stack_ret_zone(1, retstack);
                            // on peut lancer l'appel récursif
                            goto debut_boucle;
                            case 1: // on revient ici après le calcul

                            cadre = varstack->tete;
                            get_tree(&tree, cadre);

                            op1 = (NeObject*)ptrlist_pop(valstack);

                            cadre = varstack->tete;
                            get_arguments_expr(&tree, &calcListIndex, cadre); // on récupère les arguments car on ne peut se fier qu'à ce qui est dans cadre[2]
                            
                            if (CODE_ERROR != 0) {
                                return_neo_execval(valstack, NULL);
                                goto fin_boucle;
                            }
                            
                        }
                        else
                        {
                            stack_args_expr(tree->sons[0], 1, varstack);
                            stack_ret_zone(2, retstack);
                            goto debut_boucle;
                            case 2:

                            cadre = varstack->tete;
                            get_tree(&tree, cadre);

                            op1 = (NeObject*)ptrlist_pop(valstack);

                            cadre = varstack->tete;
                            get_arguments_expr(&tree, &calcListIndex, cadre);

                            if (CODE_ERROR != 0) {
                                return_neo_execval(valstack, NULL);
                                goto fin_boucle;
                            }
                        }

                        NeObject* retour;


                        // l'opérateur @ a le droit à un traitement spécial car suivant calcListIndex, il renvoie l'objet ou une copie de l'objet
                        if ((long int)number_toDouble(tree->label2) == 26) { // opérateur @

                            if (op1->type != TYPE_STRING) {
                                CODE_ERROR = 60; // ceci n'est pas une chaine de caractères
                                neobject_destroy(op1, true);
                                return_neo_execval(valstack, NULL);
                                goto fin_boucle;
                            }
                            char* nom=neo_to_string(op1);

                            if(!strlist_inList(NOMS, nom))
                            {
                                CODE_ERROR = 5;
                                neobject_destroy(op1, true);
                                return_neo_execval(valstack, NULL);
                                goto fin_boucle;
                            }
                                
                            int index = strlist_index(NOMS,nom);
                            
                            if (calcListIndex)
                                return_neo_execval(valstack, neo_copy(ADRESSES->tab[index]));
                            else
                                return_neo_execval(valstack, ADRESSES->tab[index]);
                            
                            neobject_destroy(op1, true);
                            goto fin_boucle;
                        }
                        else {
                            retour = OPFONC[(int)number_toDouble(tree->label2)](op1,un);
                        }


                        if (CODE_ERROR != 0)
                        {
                            PTRERROR = tree;

                            if (OPERANDES.tab[(int)number_toDouble(tree->label2)] != VARRIGHT && OPERANDES.tab[(int)number_toDouble(tree->label2)] != VARLEFT)
                                neobject_destroy(op1, true);
                            
                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        }
                        
                        
                        if (OPERANDES.tab[(int)number_toDouble(tree->label2)] != VARRIGHT && OPERANDES.tab[(int)number_toDouble(tree->label2)] != VARLEFT)  // on ne libère op1 que si op1 n'est pas directement d'adresse d'une variable
                            neobject_destroy(op1,true);

                                                
                        
                        return_neo_execval(valstack, retour);
                        goto fin_boucle;
                    }
                    else if (tree->nbSons == 2) // operateur binaire
                    {

                        if ((int)number_toDouble(tree->label2) == 37) { // opérateur :=
                            CODE_ERROR = 95;
                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        } else if ((int)number_toDouble(tree->label2) == 35) { // opérateur :
                            CODE_ERROR = 92;
                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        }
                        
                        
                        if (OPERANDES.tab[(int)number_toDouble(tree->label2)] == VAR_RIGHT)
                        {

                            PTRERROR = tree;

                            stack_args_expr(tree->sons[0], 0, varstack);
                            stack_ret_zone(3, retstack);
                            goto debut_boucle;
                            case 3:

                            cadre = varstack->tete;
                            get_tree(&tree, cadre);

                            if (CODE_ERROR != 0) {
                                // comme on ne fait pas de pop (on ne récupère pas la valeur, c'est comme si on récupérait la valeur et qu'on la renvoyait direct)
                                goto fin_boucle;
                            }

                            cadre = varstack->tete;
                            get_arguments_expr(&tree, &calcListIndex, cadre);
                            
                            stack_args_expr(tree->sons[1], 1, varstack);
                            stack_ret_zone(4, retstack);
                            goto debut_boucle;
                        }
                        else if (OPERANDES.tab[(int)number_toDouble(tree->label2)] == LEFT_VAR)
                        {

                            PTRERROR = tree;

                            
                            stack_args_expr(tree->sons[0], 1, varstack);
                            stack_ret_zone(5, retstack);
                            goto debut_boucle;
                            case 5:

                            cadre = varstack->tete;
                            get_tree(&tree, cadre);

                            if (CODE_ERROR != 0) {
                                goto fin_boucle;
                            }
                            
                            cadre = varstack->tete;
                            get_arguments_expr(&tree, &calcListIndex, cadre);
                            
                            stack_args_expr(tree->sons[1], 0, varstack);
                            stack_ret_zone(4, retstack);
                            goto debut_boucle;
                        }
                        else if (OPERANDES.tab[(int)number_toDouble(tree->label2)] == VAR_VAR)
                        {

                            PTRERROR = tree;

                            stack_args_expr(tree->sons[0], 0, varstack);
                            stack_ret_zone(7, retstack);
                            goto debut_boucle;
                            case 7:

                            cadre = varstack->tete;
                            get_tree(&tree, cadre);

                            if (CODE_ERROR != 0) {
                                return_neo_execval(valstack, NULL);
                                goto fin_boucle;
                            }

                            cadre = varstack->tete;
                            get_arguments_expr(&tree, &calcListIndex, cadre);
                            
                            stack_args_expr(tree->sons[1], 0, varstack);
                            stack_ret_zone(4, retstack);
                            goto debut_boucle;
                        }
                        else
                        {
                            stack_args_expr(tree->sons[0], 1, varstack);
                            stack_ret_zone(9, retstack);

                            goto debut_boucle;
                            case 9:

                            cadre = varstack->tete;
                            get_tree(&tree, cadre);

                            if (CODE_ERROR != 0)
                            {
                                op1 = (NeObject*)ptrlist_pop(valstack);
                                neobject_destroy(op1, true);
                                return_neo_execval(valstack, NULL);
                                goto fin_boucle;
                            }

                            cadre = varstack->tete;
                            get_arguments_expr(&tree, &calcListIndex, cadre);

                            stack_args_expr(tree->sons[1], 1, varstack);
                            stack_ret_zone(4, retstack);
                            goto debut_boucle;
                        }
                        
                        case 4:

                        cadre = varstack->tete;
                        get_tree(&tree, cadre);

                        op2 = (NeObject*)ptrlist_pop(valstack);
                        op1 = (NeObject*)ptrlist_pop(valstack);


                        if (CODE_ERROR != 0)
                        {
                            if (OPERANDES.tab[(int)number_toDouble(tree->label2)] != VAR_RIGHT && OPERANDES.tab[(int)number_toDouble(tree->label2)] != VAR_VAR)
                                neobject_destroy(op1, true);
                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        }

                        
                        NeObject* retour = OPFONC[(long int)number_toDouble(tree->label2)](op1, op2);
                        
                        if (CODE_ERROR != 0)
                        {
                            PTRERROR = tree;

                            if (OPERANDES.tab[(int)number_toDouble(tree->label2)] != VAR_RIGHT && OPERANDES.tab[(int)number_toDouble(tree->label2)] != VAR_VAR)
                                neobject_destroy(op1, true);
                            neobject_destroy(op2, true);
                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        }


                        if (OPERANDES.tab[(int)number_toDouble(tree->label2)] != VAR_RIGHT && OPERANDES.tab[(int)number_toDouble(tree->label2)] != VAR_VAR) // on ne libère op1 que si op1 n'est pas directement d'adresse d'une variable
                            neobject_destroy(op1,true);
                        
                        if (OPERANDES.tab[(int)number_toDouble(tree->label2)] != LEFT_VAR && OPERANDES.tab[(int)number_toDouble(tree->label2)] != VAR_VAR)
                            neobject_destroy(op2,true);

                        
                        return_neo_execval(valstack, retour);
                        goto fin_boucle;
                    }
                    
                }
                
                // s'il n'y a qu'un seul élément final, on le calcule (traiterOperande)
                
                if (tree->type == TYPE_NUMBER) // traité
                {
                    if (!calcListIndex)
                    {
                        CODE_ERROR = 89;
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }
                    else {
                        return_neo_execval(valstack, neo_copy(tree->data));
                        goto fin_boucle;
                    }
                }
                
                
                if (tree->type == TYPE_VARIABLE) // traité
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
                        goto fin_boucle;
                    }
                    else
                    {
                        
                        NeObject* neo = tree->data;

                        if (neo->type == TYPE_PROMISE) {
                            
                            intptr_t id = (intptr_t)neo->data;
                            if (PROMISES->tab[(int)id] != NULL) { // la promesse a été résolue
                                
                                if (PROMISES_CNT.tab[(int)id] == 1) // on s'apprête à renvoyer la dernière valeur associée à cette promesse
                                {
                                    // quand on supprime neo, on passe le compteur à zéro, donc neobject_destroy va libérer PROMISES->tab[id]
                                    // pour cette raison on ajoute un au compteur
                                    PROMISES_CNT.tab[(int)id]++;

                                    neobject_destroy(neo, false); // du coup on est sûr que le PROMISES n'est pas supprimé

                                    // la libération de neon en false supprime PROMISES[id]
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

                            }
                        }

                        return_neo_execval(valstack, tree->data);
                        goto fin_boucle;
                    }
                    

                }
                
                if (tree->type == TYPE_LIST) // traité
                {
                    if (!calcListIndex) // erreur car sinon ça fait une fuite mémoire
                    {
                        CODE_ERROR = 89;
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }

                    // donc les enfants de tree sont les éléments de la liste
                    l = nelist_create(tree->nbSons);
                    cadre[2] = (void*)l;
                    ext_index = 0;
                    cadre[3] = (void*)ext_index;
                    

                    for (; ext_index < l->len ; ext_index++) {
                        cadre[3] = (void*)ext_index;

                        stack_args_expr(tree->sons[ext_index], 1, varstack);
                        stack_ret_zone(11, retstack);
                        goto debut_boucle;
                        case 11:

                        cadre = varstack->tete; // on récupère nos variables locales
                        get_tree(&tree, cadre);
                        l = ((NeList*)cadre[2]);
                        ext_index = (intptr_t)cadre[3];

                        l->tab[ext_index] = (NeObject*)ptrlist_pop(valstack);
                    }


                    if (CODE_ERROR != 0)
                    {
                        err_free(l->tab);
                        err_free(l);                        
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }

                    return_neo_execval(valstack, neo_list_convert(l));
                    goto fin_boucle;
                }
                
                if (tree->type == TYPE_FONCTION)
                {

                    if (!calcListIndex)
                    {
                        CODE_ERROR = 103;
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }

                    PTRERROR = tree;
                    
                    if (tree->data->type == TYPE_FONCTION)
                    {
                        
                        l = nelist_create(tree->nbSons);
                        cadre[2] = (void*)l; // on a accroché la liste au cadre de pile

                        ext_index = 0;
                        cadre[3] = (void*)ext_index;
                        
                        fun = tree->data->data;

    
                        for (; (ext_index < fun->nbArgsMeth && ext_index < tree->nbSons) || (fun->nbArgsMeth == -1 && ext_index < tree->nbSons) ; ext_index++)
                        {
                            cadre[3] = (void*)ext_index; // si on modifie la variable en tant que variable locale, il faut la modifier au bon endroit


                            stack_args_expr(tree->sons[ext_index], 0, varstack);
                            stack_ret_zone(12, retstack);
                            goto debut_boucle;
                            case 12:

                            cadre = varstack->tete;
                            l = ((NeList*)cadre[2]);
                            tree = (Tree*)cadre[0];
                            fun = tree->data->data;
                            ext_index = (intptr_t)cadre[3];

                            l->tab[ext_index] = (NeObject*)ptrlist_pop(valstack);


                            if (CODE_ERROR != 0) {
                                err_free(l->tab);
                                err_free(l);
                                return_neo_execval(valstack, NULL);
                                goto fin_boucle;
                            }
                            
                            if (l->tab[ext_index]->type == TYPE_EMPTY)
                            {
                                CODE_ERROR = 5;
                                return_neo_execval(valstack, NULL);
                                goto fin_boucle;
                            }
                        }

                        for (;ext_index < tree->nbSons ; ext_index++)
                        {
                            cadre[3] = (void*)ext_index;

                            stack_args_expr(tree->sons[ext_index], 1, varstack);
                            stack_ret_zone(13, retstack);
                            goto debut_boucle;
                            case 13:

                            cadre = varstack->tete;
                            l = ((NeList*)cadre[2]);
                            tree = (Tree*)cadre[0];
                            fun = tree->data->data;
                            ext_index = (intptr_t)cadre[3];

                            l->tab[ext_index] = (NeObject*)ptrlist_pop(valstack);
                            
                            if (CODE_ERROR != 0)
                            {
                                // si y a eu un problème dans l'évaluation d'un argument, on doit libérer toute la liste créée jusqu'alors
                                for (int j = fun->nbArgsMeth ; j <= ext_index ; j++) // on ne détruit que les éléments que l'on a copiés
                                    neobject_destroy(l->tab[j],true);
                                
                                err_free(l->tab);
                                err_free(l);
                                
                                return_neo_execval(valstack, NULL);
                                goto fin_boucle;
                            }
                            
                        }

                        args = neo_list_convert(l);
                        

                        if (CODE_ERROR != 0)
                        {
                            err_free(((NeList*)(args->data))->tab);
                            err_free(args->data);
                            err_free(args);
                            
                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        }
                
                        if (!funcArgsCheck(fun, args->data))
                        {
                            if (fun->nbArgsMeth != -1)
                            {
                                for (int i=fun->nbArgsMeth ; i < neo_list_len(args) ; i++) // on ne détruit que les éléments que l'on a copiés
                                {
                                    neobject_destroy(neo_list_nth(args,i),true);
                                }
                            }
                
                            err_free(((NeList*)(args->data))->tab);
                            err_free(args->data);
                            err_free(args);
                            
                            CODE_ERROR = 14;
                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        }
                
                        
                        NeObject* retour = functionCall(tree->data,(NeList*)args->data);

                        
                        if (CODE_ERROR != 0)
                        {
                            if (fun->nbArgsMeth != -1)
                            {
                                for (int i=fun->nbArgsMeth ; i < neo_list_len(args) ; i++) // on ne détruit que les éléments que l'on a copiés
                                {
                                    neobject_destroy(neo_list_nth(args,i),true);
                                }
                            }
                
                            err_free(((NeList*)(args->data))->tab);
                            err_free(args->data);
                            err_free(args);

                            neobject_destroy(retour, true);
                            
                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        }
                        
                        if (fun->nbArgsMeth != -1)
                        {
                            for (int i=fun->nbArgsMeth ; i < neo_list_len(args) ; i++) // on ne détruit que les éléments que l'on a copiés
                            {
                                neobject_destroy(neo_list_nth(args,i),true);
                            }
                        }
                
                
                        err_free(((NeList*)(args->data))->tab);
                        err_free(args->data);
                        err_free(args);


                        return_neo_execval(valstack, retour);
                        goto fin_boucle;
                    }

                    else if (tree->data->type == TYPE_USERFUNC || tree->data->type == TYPE_USERMETHOD)
                    {
                        UserFunc* fun = tree->data->data;

                        if (tree->nbSons > fun->nbArgs && ! fun->unlimited_arguments)
                        {
                            CODE_ERROR = 6;
                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        }


                        // on compte le nombre d'arguments très facultatifs
                        int given_nbOptArgs = 0;

                        for (int i = 0 ; i < tree->nbSons ; i++) {
                            if (tree->sons[i]->type == TYPE_OPERATOR && (long int)number_toDouble(tree->sons[i]->label2) == 37) { // on est sur du :=
                                int index = 0;
                                while (index < fun->nbArgs && fun->args[index] != tree->sons[i]->sons[0]->data) index++;

                                if (index >= fun->nbArgs - fun->nbOptArgs && index < fun->nbArgs)
                                    given_nbOptArgs ++;
                            }
                        }

                        
                        // on sait maintenant que le nombre d'arguments réel final est :
                        // -> Si tree->nbSons > fun->nbArgs : tree->nbSons - given_nbOptArgs
                        // -> Sinon, fun->nbArgs



                        // on va commencer par construire un tableau d'expressions à évaluer (un tableau d'arbres) à partir des arguments qu'on nous donne
                        arguments = err_malloc(sizeof(Tree));
                        arguments->nbSons = (fun->unlimited_arguments && tree->nbSons > fun->nbArgs - fun->nbOptArgs) ? tree->nbSons + fun->nbOptArgs - given_nbOptArgs : fun->nbArgs;
                        arguments->sons = malloc(sizeof(Tree*) * arguments->nbSons);

                        // on va faire un tableau de booléens qui compte si on donne la valeur des arguments très optionnels (après ...)

                        
                        for (int i = 0 ; i < arguments->nbSons ; i++) arguments->sons[i] = NULL; // pour savoir s'il y a des problèmes



                        // on peut commencer par mettre à leur valeur par défaut les variables définies après ...
                        // Ainsi elles ne seront pas remplacés sauf si on l'indique explicitement
                        for (int i = fun->nbArgs - fun->nbOptArgs ; i < fun -> nbArgs ; i++) {
                            arguments->sons[i] = fun->opt_args->sons[i];
                        }


                        // 1ère boucle pour placer les arguments à des endroits où ils ne bougeront pas
                        for (int i = 0 ; i < tree->nbSons ; i++) {
                            // on commence par remplir les éléments donnés dans le mauvais ordre (avec :=)
                            if (tree->sons[i]->type == TYPE_OPERATOR && (long int)number_toDouble(tree->sons[i]->label2) == 37) { // on est sur du :=
                                // on regarde la variable de gauche, et on met le truc au bon endroit
                                // on récupère l'index du vrai argument dans fun->args, et on le met au bon endroit dans arguments
                                int index = 0;

                                if (tree->sons[i]->sons[0]->type != TYPE_VARIABLE) {
                                    err_free(arguments->sons);
                                    err_free(arguments);
                                    CODE_ERROR = 93;
                                    return_neo_execval(valstack, NULL);
                                    goto fin_boucle;
                                }

                                while (index < fun->nbArgs && fun->args[index] != tree->sons[i]->sons[0]->data) index++;

                                if (index == fun->nbArgs) {
                                    err_free(arguments->sons);
                                    err_free(arguments);
                                    CODE_ERROR = 93;
                                    return_neo_execval(valstack, NULL);
                                    goto fin_boucle;
                                }
                                
                                if (index < fun->nbArgs - fun->nbOptArgs && arguments->sons[index] != NULL) {
                                    err_free(arguments->sons);
                                    err_free(arguments);
                                    CODE_ERROR = 94;
                                    return_neo_execval(valstack, NULL);
                                    goto fin_boucle;
                                }
                                arguments->sons[index] = tree->sons[i]->sons[1];
                            }
                        }

                        // deuxième boucle pour insérer dans l'ordre et en évitant les := les arguments restants
                        int index = 0; // on remplit petit à petit arguments avec les arguments restants
                        while (index < arguments->nbSons && arguments->sons[index] != NULL) index++;

                        for (int i = 0 ; index < arguments->nbSons && i < tree->nbSons ; i++) {
                            
                            if (tree->sons[i]->type != TYPE_OPERATOR || (long int)number_toDouble(tree->sons[i]->label2) != 37) {
                                arguments->sons[index] = tree->sons[i];
                                while (index < arguments->nbSons && arguments->sons[index] != NULL) index++;
                            }
                        }
                        // c'est un choix philosophique : si f attend x, y facultatif et z, alors f(1,2) ne fonctionnera pas car il manque z
                        // pour mettre uniquement le x et le z, il y a une syntaxe pour le faire

                        if (tree->nbSons < fun->nbArgs) {
                            // à ce stade-là, certains champs de arguments sont encore vides, on va donc parcourir une dernière fois afin de lui associer les expressions définies lors de la définition de la fonction
                            for (int i = 0 ; i < fun->nbArgs ; i++) {
                                if (arguments->sons[i] == NULL) { // c'est précisément le cas où l'utilisateur n'a rien mis
                                    arguments->sons[i] = fun->opt_args->sons[i];
                                    if (arguments->sons[i]->nbSons == 0 && arguments->sons[i]->type == 0) {
                                        CODE_ERROR = 7;
                                        err_free(arguments->sons);
                                        err_free(arguments);
                                        return_neo_execval(valstack, NULL);
                                        goto fin_boucle;
                                    }
                                }
                            }
                        }

                        // maintenant, normalement arguments est un tableau plein d'arbres, on peut donc l'évaluer


                        // calcul des arguments et affectation des arguments
                        liste = nelist_create(arguments->nbSons);

                        // variables à sauvegarder : fun, liste, ext_index et arguments


                        for (ext_index = 0 ; ext_index < liste->len ; ext_index++) {
                            
                            cadre[2] = (void*)fun;
                            cadre[3] = (void*)liste;
                            cadre[4] = (void*)ext_index;
                            cadre[5] = (void*)arguments;

                            if (ext_index == 0 && tree->data->type == TYPE_USERMETHOD)
                                stack_args_expr(arguments->sons[ext_index], 0, varstack);
                            else
                                stack_args_expr(arguments->sons[ext_index], 1, varstack);

                            stack_ret_zone(33, retstack);
                            goto debut_boucle;
                            case 33:


                            cadre = varstack->tete;
                            get_tree(&tree, cadre);
                            fun = (UserFunc*)cadre[2];
                            liste = (NeList*)cadre[3];
                            ext_index = (intptr_t)cadre[4];
                            arguments = (Tree*)cadre[5];

                            liste->tab[ext_index] = (NeObject*)ptrlist_pop(valstack);

                            if (CODE_ERROR != 0) {
                                if (tree->data->type != TYPE_USERMETHOD)
                                    neobject_destroy(liste->tab[0], true);

                                for (int k = 1 ; k <= ext_index ; k++)
                                    neobject_destroy(liste->tab[k], true);

                                err_free(liste->tab);
                                err_free(liste);
                                err_free(arguments->sons);
                                err_free(arguments);
                                return_neo_execval(valstack, NULL);
                                goto fin_boucle;
                            }

                        }
                        
                        err_free(arguments->sons);
                        err_free(arguments);

                        args = neo_list_convert(liste);


                        if (CODE_ERROR != 0)
                        {
                            err_free(((NeList*)(args->data))->tab);
                            err_free(args->data);
                            err_free(args);
                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        }


                        // ouvre un nouveau contexte pour sauvegarder les variables locales de cet appel
                        newContext();

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
                                ((NeList*)args->data)->len = fun->nbArgs;
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
                                    
                                    ((NeList*)args->data)->tab[0] = neo_none_create();
                                    neobject_destroy(args, true);
                                    // libération des arguments
                                    for (int j = 1 ; j < i ; j++)
                                        neobject_destroy(fun->args[j], false);

                                    deleteContext();

                                    return_neo_execval(valstack, NULL);
                                    goto fin_boucle;
                                }
                            }
                        }
                        else
                        {
                            for (int i = 0 ; i < fun->nbArgs ; i++) // affectation des arguments
                            {
                                NeObject* temp = neo_copy(neo_list_nth(args, i));
                                
                                save_later(variables_a_sauvegarder, fun->args[i]);
                                local(fun->args[i]);
                                
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
                                    
                                    return_neo_execval(valstack, NULL);
                                    goto fin_boucle;
                                }
                                
                            }
                        }

                        // on stocke fun et args
                        cadre[2] = (void*)fun;
                        cadre[3] = (void*)args;
                        stack_args(fun->code, varstack);
                        stack_ret_zone(34, retstack);
                        goto debut_boucle; // exécute le corps de la fonction
                        case 34:


                        cadre = varstack->tete;
                        get_tree(&tree, cadre);
                        fun = (UserFunc*)cadre[2];
                        args = (NeObject*)cadre[3];

                        int_ret = (intptr_t)ptrlist_pop(valstack);


                        if (CODE_ERROR != 0) // erreur
                        {
                            for (int i = 0 ; i < fun->nbArgs ; i++)
                                neobject_destroy(fun->args[i], false);

                            if (tree->data->type == TYPE_USERMETHOD)
                                ((NeList*)args->data)->tab[0] = neo_none_create();
                            neobject_destroy(args, true);

                            deleteContext();

                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        }

                        // sauvegarde des valeurs obtenues à la fin de la méthode pour que le paramètre puisse éventuellement récupérer son ancienne valeur
                        if (tree->data->type == TYPE_USERMETHOD && fun->nbArgs > 0)
                        {

                            // sauvegarde uniquement le data, et on l'enlève de fun->args[0]
                            void* s = fun->args[0]->data;
                            int t = fun->args[0]->type;
                            
                            fun->args[0]->data = NULL;

                            deleteContext(); // supprime le contexte : restaure les variables d'avant


                            neobject_destroy(((NeList*)args->data)->tab[0], false);
                            
                            // on modifie list[0]
                            ((NeList*)args->data)->tab[0]->data = s;
                            ((NeList*)args->data)->tab[0]->type = t;

                            ((NeList*)args->data)->tab[0] = NULL; // neobject au pif

                        }
                        else
                            deleteContext(); // réaffecte les anciennes valeurs des variables qui ont été mises en local

                        
                        neobject_destroy(args, true);
                        
                        if (int_ret == EXIT_RETURN) {
                            return_neo_execval(valstack, RETURN_VALUE);
                            RETURN_VALUE = NULL; // pour dire que l'on l'a utilisé
                        }
                        else
                            return_neo_execval(valstack, neo_none_create());

                        // ainsi si une return_value n'est pas à NULL c'est que forcément la valeur n'a pas été récupérée

                        goto fin_boucle;
                        
                    }

                    else if (tree->data->type == TYPE_EMPTY)
                    {
                        CODE_ERROR = 8;
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }
                    else
                    {
                        CODE_ERROR = 9;
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }
                    
                }
                
                if (tree->type == TYPE_LISTINDEX) // traité
                {
                    // il faut au préalable calculer l'index
                    // tree->data contient un pointeur vers la liste concernée
                    

                    stack_args_expr(tree->sons[0], 0, varstack);
                    stack_ret_zone(14, retstack);
                    goto debut_boucle;
                    case 14:

                    cadre = varstack->tete;
                    get_arguments_expr(&tree, &calcListIndex, cadre);

                    // la valeur de la liste est stockée dans la pile de retour

                    if (CODE_ERROR != 0) {
                        ptrlist_pop(valstack);
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }


                    stack_args_expr(tree->sons[1], 1, varstack);
                    stack_ret_zone(15, retstack);
                    goto debut_boucle;
                    case 15:

                    cadre = varstack->tete;
                    get_arguments_expr(&tree, &calcListIndex, cadre);

                    NeObject* index = (NeObject*)ptrlist_pop(valstack); // on les dépile dans tous les cas
                    NeObject* obj = (NeObject*)ptrlist_pop(valstack);


                    if (CODE_ERROR != 0) {
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }


                    if (obj->type != TYPE_LIST)
                    {
                        CODE_ERROR = 15;
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }

                    
                    if (index->type != TYPE_NUMBER)
                    {
                        neobject_destroy(index, true);
                        CODE_ERROR = 16;
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }
                    
                    long int index2 = (long int)number_toDouble(neo_to_nb(index));
                    neobject_destroy(index,true);
                    

                    if (obj->type == TYPE_EMPTY)
                    {
                        CODE_ERROR = 17;
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }
                    else if (index2 >= neo_list_len(obj) || index2 < 0)
                    {
                        CODE_ERROR = 18;
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }
                    
                    if (calcListIndex)
                    {
                        NeObject* retour = neo_copy(neo_list_nth(obj,index2));
                        return_neo_execval(valstack, retour);
                        goto fin_boucle;
                    }
                    else
                    {
                        NeObject* neo = neo_list_nth(obj,index2);

                        if (neo->type == TYPE_PROMISE) {
                            intptr_t id = (intptr_t)neo->data;
                            if (PROMISES->tab[(int)id] != NULL) { // la promesse a été résolue
                                if (PROMISES_CNT.tab[(int)id] == 1) // on s'apprête à renvoyer la dernière valeur associée à cette promesse
                                {
                                    PROMISES_CNT.tab[(int)id]++;
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

                            }
                        }
                        // dans ce cas-là, on va devoir retourner directement le pointeur de l'élément
                        return_neo_execval(valstack, neo); // au lieu de faire une copie, on renvoie directement le bon pointeur
                        goto fin_boucle;
                    }

                }

                if (tree->type == TYPE_CONTAINER)
                {

                    if (!calcListIndex)
                    {
                        CODE_ERROR = 81; // affectation d'un champ de container littéral : illégal car -> fuite mémoire
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }

                    val = nelist_create(tree->nbSons);
                    ext_index = 0;

                    cadre[2] = (void*)val; // sauvegarde de la liste en variable locale
                    cadre[3] = (void*)ext_index;

                    for ( ; ext_index < tree->nbSons ; ext_index++)
                    {
                        cadre[2] = (void*)val;
                        cadre[3] = (void*)ext_index;

                        stack_args_expr(tree->sons[ext_index]->sons[0], 1, varstack);
                        stack_ret_zone(16, retstack);
                        goto debut_boucle;
                        case 16:

                        // on est revenus, il faut donc récupérer les variables
                        cadre = varstack->tete;
                        get_arguments_expr(&tree, &calcListIndex, cadre);
                        val = (NeList*)cadre[2];
                        ext_index = (intptr_t)cadre[3];

                        val->tab[ext_index] = (NeObject*)ptrlist_pop(valstack);

                        if (CODE_ERROR != 0)
                        {
                            for (int k=0 ; k < ext_index ; k++) // libération de tous les éléments alloués jusqu'a i exclus
                            {
                                neobject_destroy(val->tab[k], true);
                            }
                            err_free(val->tab);
                            err_free(val);
                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        }
                    }
                    NeObject* ret = neo_container_create((int)number_toDouble(tree->label2), val);
                    
                    return_neo_execval(valstack, ret);
                    goto fin_boucle;
                }
                
                if (tree->type == TYPE_ATTRIBUTE)
                {
                    stack_args_expr(tree->sons[0], calcListIndex, varstack);
                    stack_ret_zone(17, retstack);
                    goto debut_boucle;
                    case 17:

                    cadre = varstack->tete;
                    get_arguments_expr(&tree, &calcListIndex, cadre);

                    NeObject* neo = (NeObject*)ptrlist_pop(valstack); // vérifier que le fils existe bien

                    if (CODE_ERROR != 0) {
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }

                    if (neo->type != TYPE_CONTAINER)
                    {
                        // Erreur : essaie d'accéder à un champ d'une variable qui n'est pas un container
                        neobject_destroy(neo, calcListIndex);
                        CODE_ERROR = 80;
                        return_neo_execval(valstack, NULL);
                        goto fin_boucle;
                    }


                    // va chercher la valeur
                    NeObject* ret;

                    Container* c = (Container*)neo->data;

                    int index = 0;

                    if (!number_isDefault(tree->label2)) // ça veut dire qu'on sait exactement où chercher la valeur
                    {
                        // passage optimisé
                        index = (long int)number_toDouble(tree->label2);
                    }
                    else
                    {
                        // passage complexe
                        // il faut au préalable calculer l'index de où est la valeur dans le tableau
                        NeList* list = (NeList*)(ATTRIBUTES->tab[c->type]->data);
                        
                        for (; index < list->len && strcmp(neo_to_string(list->tab[index]), tree->label1) != 0 ; index++)

                        if (index == list->len - 1 && strcmp(neo_to_string(list->tab[index]), tree->label1) != 0)
                        {
                            CODE_ERROR = 82;
                            neobject_destroy(neo, calcListIndex);
                            return_neo_execval(valstack, NULL);
                            goto fin_boucle;
                        }

                        tree->label2 = number_fromDouble((double)index);

                        // pour l'affichage de l'erreur on garde ça
                        //err_free(tree->label1) ; tree->label1 = NULL;
                    }


                    if (calcListIndex) {
                        ret = neo_copy(c->data->tab[index]);
                        neobject_destroy(neo, true);
                    }
                    else {
                        NeObject* neo2 = c->data->tab[index];

                        if (neo2->type == TYPE_PROMISE) {
                            
                            intptr_t id = (intptr_t)neo2->data;
                            if (PROMISES->tab[(int)id] != NULL) { // la promesse a été résolue
                                
                                if (PROMISES_CNT.tab[(int)id] == 1) // on s'apprête à renvoyer la dernière valeur associée à cette promesse
                                {
                                    // quand on supprime neo2, on passe le compteur à zéro, donc neobject_destroy va libérer PROMISES->tab[id]
                                    // pour cette raison on ajoute un au compteur
                                    PROMISES_CNT.tab[(int)id]++;

                                    neobject_destroy(neo2, false); // du coup on est sûr que le PROMISES n'est pas supprimé

                                    // la libération de neon en false supprime PROMISES[id]
                                    neo2->type = PROMISES->tab[(int)id]->type;
                                    neo2->data = PROMISES->tab[(int)id]->data;
                                    err_free(PROMISES->tab[(int)id]);
                                    PROMISES->tab[(int)id] = NULL;
                                    PROMISES_CNT.tab[(int)id] = 0;
                                }
                                else
                                {
                                    _affect2(neo2, PROMISES->tab[(int)id]);
                                }

                            }
                        }

                        ret = neo2;
                    }

                    
                    
                    return_neo_execval(valstack, ret);
                    goto fin_boucle;
                }
                
                if (tree->type == TYPE_STRING) // traité
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
                    
                    goto fin_boucle;
                }
                
                if (tree->type == TYPE_BOOL) // traité
                {
                    if (calcListIndex)
                        return_neo_execval(valstack, neo_copy(tree->data));
                    else
                    {
                        CODE_ERROR = 89;
                        return_neo_execval(valstack, NULL);
                    }
                    goto fin_boucle;
                }

                if (tree->type == TYPE_EXCEPTION) // traité
                {
                    if (calcListIndex)
                        return_neo_execval(valstack, neo_copy(tree->data));
                    else
                    {
                        CODE_ERROR = 89;
                        return_neo_execval(valstack, NULL);
                    }
                    goto fin_boucle;
                }
                
                if (tree->type == TYPE_NONE) // traité
                {
                    if (calcListIndex)
                        return_neo_execval(valstack, neo_copy(tree->data));
                    else
                    {
                        CODE_ERROR = 89;
                        return_neo_execval(valstack, NULL);
                    }
                    goto fin_boucle;
                }

                if (tree->type == 0) {
                    return_neo_execval(valstack, neo_none_create());
                    goto fin_boucle;
                }
                
                CODE_ERROR = 19;
                return_neo_execval(valstack, NULL);
                goto fin_boucle;

            }
            else { // on arrive dans le exec

                for (inst=0 ; inst < tree->nbSons ; inst++)
                {
                    PTRERROR = tree->sons[inst];


                    if (tree->sons[inst]->type == TYPE_TRYEXCEPT)
                    {

                        // trucs qu'il y a à sauvegarder ici : inst

                        cadre[2] = (void*)inst;
                        stack_args(tree->sons[inst]->sons[0], varstack);
                        stack_ret_zone(31, retstack);
                        goto debut_boucle;
                        case 31:

                        cadre = varstack->tete;
                        get_tree(&tree, cadre);
                        inst = (intptr_t)cadre[2];

                        int_ret = (intptr_t)ptrlist_pop(valstack);
                        maintree = tree->sons[inst];

                        // exécution du try

                        if (CODE_ERROR > 1) // exit() n'est pas considéré comme une erreur
                        {
                            // on va parcourir la liste des exceptions jusqu'à en trouver une qui corresponde
                            _Bool bo = true;
                            for (int i = 0 ; i < maintree->sons[1]->sons[0]->nbSons ; i++)
                            {
                                PTRERROR = tree->sons[inst];

                                /*
                                PREVIOUS :
                                Tree* obj = maintree->sons[1]->sons[0]->sons[i];

                                if (!(obj->type == TYPE_EXCEPTION || (obj->type == TYPE_VARIABLE && ((NeObject*)obj->data)->type == TYPE_EXCEPTION)))
                                */


                                NeObject* obj = maintree->sons[1]->sons[0]->sons[i]->data;

                                if (obj->type != TYPE_EXCEPTION)
                                {
                                    CODE_ERROR = 78;
                                    return_int_execval(valstack, int_ret);
                                    goto fin_boucle;
                                }
                                else
                                {
                                    int code = *((int*)maintree->sons[1]->sons[0]->sons[i]->data->data);
                                    if (exceptions_err.tab[CODE_ERROR] == code || (CODE_ERROR == 74 && EXCEPTION != NULL && strcmp(EXCEPTION, exceptions.tab[code])==0)) // l'erreur correspond
                                    {
                                        if (CODE_ERROR == 74)
                                            EXCEPTION = NULL;
                                        
                                        bo = true;
                                        break;
                                    }
                                    else
                                        bo = false;
                                }
                            }

                            if (bo) // exécution du except
                            {
                                CODE_ERROR = 0; // du coup c'est bon on repart (la police a arrêté le programme, a vérifié ses papiers, et le programme est reparti)
                                
                                cadre[2] = (void*)inst;
                                stack_args(maintree->sons[1]->sons[1], varstack);
                                stack_ret_zone(32, retstack);
                                goto debut_boucle;
                                case 32:

                                cadre = varstack->tete;
                                get_tree(&tree, cadre);
                                inst = (intptr_t)cadre[2];

                                int_ret = (intptr_t)ptrlist_pop(valstack);

                                if (CODE_ERROR != 0) {
                                    return_int_execval(valstack, int_ret);
                                    goto fin_boucle;
                                }
                            }
                        }

                    }

                    else if (tree->sons[inst]->type == TYPE_ATOMICBLOCK)
                    {
                        // on va exécuter le bloc de code d'une traite, sans changer de processus
                        cadre[2] = (void*)inst;
                        cadre[3] = (void*)atomic_counter;
                        atomic_counter = -1; // tant qu'on n'a pas remis une valeur positive, on ne passera pas à un autre processus
                        
                        stack_args(tree->sons[inst], varstack);
                        stack_ret_zone(10, retstack);
                        goto debut_boucle;
                        case 10:

                        cadre = varstack->tete;
                        get_tree(&tree, cadre);
                        inst = (intptr_t)cadre[2];

                        // s'il reste des crédits à ce processus, on les lui rend, sinon on passe au suivant
                        atomic_counter = (atomic_counter + (intptr_t)cadre[3] > 0) ? atomic_counter + (intptr_t)cadre[3] : 0;


                        int_ret = (intptr_t)ptrlist_pop(valstack);


                        if (CODE_ERROR != 0 || int_ret != 0) {
                            return_int_execval(valstack, int_ret);
                            goto fin_boucle;
                        }

                    }
                    
                    else if (tree->sons[inst]->type == TYPE_BLOCKWORD1LINE)
                    {
                        if ((int)number_toDouble(tree->sons[inst]->label2) == RETURN) // action de return
                        {
                            if (tree->sons[inst]->nbSons > 1)
                            {
                                CODE_ERROR = 21;
                                return_int_execval(valstack, 0);
                                goto fin_boucle;
                            }
                            
                            if (RETURN_VALUE != NULL) // c'est pas correct, car on ne peut pas renvoyer une valeur alors que la précédente n'a pas été récupérée
                            {
                                CODE_ERROR = 99;
                                return_int_execval(valstack, 0);
                                goto fin_boucle;
                            }

                            if (tree->sons[inst]->nbSons == 0) {
                                RETURN_VALUE = neo_none_create();
                            }
                            else {
                                cadre[2] = (void*)inst;
                                stack_args_expr(tree->sons[inst]->sons[0], 1, varstack);
                                stack_ret_zone(35, retstack);
                                goto debut_boucle;
                                case 35:

                                cadre = varstack->tete;
                                get_tree(&tree, cadre);
                                inst = (intptr_t)cadre[2];
                                RETURN_VALUE = (NeObject*)ptrlist_pop(valstack);
                            }

                            

                            if (CODE_ERROR != 0) {
                                return_int_execval(valstack, 0);
                                goto fin_boucle;
                            }
                            
                            return_int_execval(valstack, EXIT_RETURN);
                            goto fin_boucle;
                            
                        }

                        
                        else if ((int)number_toDouble(tree->sons[inst]->label2) == IMPORT) // action de import
                        {
                            for (ext_index = 0 ; ext_index < tree->sons[inst]->nbSons ; ext_index++)
                            {
                                // on empile inst et ext_index
                                cadre[2] = (void*)inst;
                                cadre[3] = (void*)ext_index;
                                stack_args_expr(tree->sons[inst]->sons[ext_index], 1, varstack);
                                stack_ret_zone(30, retstack);
                                goto debut_boucle;
                                case 30:
                                
                                nom = (NeObject*)ptrlist_pop(valstack);

                                cadre=varstack->tete;
                                get_tree(&tree, cadre);
                                ext_index = (intptr_t)cadre[3];
                                inst = (intptr_t)cadre[2];

                                if (CODE_ERROR != 0) {
                                    return_int_execval(valstack, 0);
                                    goto fin_boucle;
                                }

                                char* nomAct = strdup(neo_to_string(ADRESSES->tab[NAME])); // pour restaurer le nom de fichier actuel

                                #ifndef TI83PCE
                                    char* nomFichier = addStr(neo_to_string(nom), ".ne");

                                    updateFileName(strdup(neo_to_string(nom)));

                                    importFile(nomFichier);
                                    err_free(nomFichier);
                                #else
                                    updateFileName(strdup(nom));
                                    importFile(neo_to_string(nom));
                                #endif

                                updateFileName(nomAct);

                                neobject_destroy(nom, true);

                                if (CODE_ERROR != 0) {
                                    return_int_execval(valstack, 0);
                                    goto fin_boucle;
                                }

                            }
                        }

                        else if ((int)number_toDouble(tree->sons[inst]->label2) == LOCAL)
                        {
                            if (tree->sons[inst]->nbSons == 0) // il faut au moins un argument
                            {
                                CODE_ERROR = 69;
                                return_int_execval(valstack, 0);
                                goto fin_boucle;
                            }
                            else if (VAR_LOC->tete == NULL)
                            {
                                CODE_ERROR = 70;
                                return_int_execval(valstack, 0);
                                goto fin_boucle;
                            }

                            for (int i = 0 ; i < tree->sons[inst]->nbSons ; i++) // pour toutes les variables
                            {
                                // va traiter la variable comme étant locale
                                save_later(variables_a_sauvegarder, tree->sons[inst]->sons[i]->data);
                                local(tree->sons[inst]->sons[i]->data);
                            }
                            
                        }
                        else if ((int)number_toDouble(tree->sons[inst]->label2) == AWAIT) { // attend tant qu'une condition soit vérifiée
                            if (tree->sons[inst]->nbSons > 1)
                            {
                                CODE_ERROR = 101;
                                return_int_execval(valstack, 0);
                                goto fin_boucle;
                            }

                            cadre[2] = (void*)inst; // on sauvegarde inst maintenant

                            process_cycle->process->wait_expr = tree->sons[inst]->sons[0];

                            PTRERROR = tree->sons[inst]; // s'il y a une erreur dans le await

                            check_condition: // on vient du haut de la boucle, donc on charge les paramètres actuels

                            cadre = varstack->tete; // on récupère un cadre qui n'est pas le bon. PK ?
                            get_tree(&tree, cadre);
                            cadre[3] = (void*)process_cycle->process->wait_expr;

                            // évaluation de la condition
                            stack_args_expr(process_cycle->process->wait_expr, 1, varstack);
                            stack_ret_zone(8, retstack);

                            process_cycle->process->wait_expr = NULL; // le temps de l'évaluation de l'expression, on lui fait croire qu'il n'est plus en pause
                            
                            goto debut_boucle;
                            // si l'évaluation consomme plus que ATOMIC_TIME instructions, on passe qd même au processus suivant
                            // en revanche si on revient avant et que la condition est fausse , on passe direct au suivant

                            case 8:

                            cadre = varstack->tete;


                            get_tree(&tree, cadre);
                            inst = (intptr_t)cadre[2];


                            if (CODE_ERROR != 0)
                            {
                                goto fin_boucle;
                            }


                            process_cycle->process->wait_expr = (Tree*)cadre[3];


                            NeObject* neo = (NeObject*)ptrlist_pop(valstack);

                            if (neoIsTrue(neo)) // fin de l'attente passive
                            {
                                neobject_destroy(neo, true);
                                process_cycle->process->wait_expr = NULL;
                                // on continue normalement, car on a bien conservé le inst du tout début
                            }
                            else
                            {
                                neobject_destroy(neo, true);
                                atomic_counter = 0;
                                goto debut_boucle;
                            }

                        }
                    }

                    
                    
                    else if (tree->sons[inst]->type == TYPE_KEYWORD)
                    {
                        if ((int)number_toDouble(tree->sons[inst]->label2) != PASS) {
                            return_int_execval(valstack, (int)number_toDouble(tree->sons[inst]->label2));
                            goto fin_boucle;
                        }
                    }

                    
                
                    else if (tree->sons[inst]->type == TYPE_CONDITIONBLOCK)
                    {

                        maintree = tree->sons[inst];
                        int_ret = 0;
                        bloc = 0;

                        cadre[2] = (void*)int_ret;
                        cadre[3] = (void*)bloc;
                        cadre[4] = (void*)inst;

                        // on a comme variables locales : int_ret, bloc, inst : d'ailleurs inst doit être empilé partout

                        while (bloc < maintree->nbSons)
                        {

                            PTRERROR = maintree->sons[bloc];

                            // évaluation de la condition
                            cadre[2] = (void*)int_ret;
                            cadre[3] = (void*)bloc;
                            cadre[4] = (void*)inst;

                            stack_args_expr(maintree->sons[bloc]->sons[0], 1, varstack);
                            stack_ret_zone(19, retstack);
                            goto debut_boucle;
                            case 19:

                            cadre = varstack->tete;
                            get_tree(&tree, cadre);
                            int_ret = (intptr_t)cadre[2];
                            bloc = (intptr_t)cadre[3];
                            inst = (intptr_t)cadre[4];
                            maintree = tree->sons[inst];

                            expr = (NeObject*)ptrlist_pop(valstack);
                            cond = neoIsTrue(expr);
                            neobject_destroy(expr, true);

                            if (CODE_ERROR != 0) {
                                return_int_execval(valstack, 0);
                                goto fin_boucle;
                            }


                            if (cond)
                            {
                                // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
                                newContext();

                                cadre[2] = (void*)int_ret;
                                cadre[3] = (void*)bloc;
                                cadre[4] = (void*)inst;

                                stack_args(maintree->sons[bloc]->sons[1], varstack);
                                stack_ret_zone(20, retstack);
                                goto debut_boucle;
                                case 20:

                                cadre = varstack->tete;
                                get_tree(&tree, cadre);
                                int_ret = (intptr_t)cadre[2];
                                bloc = (intptr_t)cadre[3];
                                inst = (intptr_t)cadre[4];
                                maintree = tree->sons[inst];

                                int_ret = (intptr_t)ptrlist_pop(valstack);

                                deleteContext(); // on vient d'exécuter le code, donc on a fini le if

                                
                                if (CODE_ERROR != 0) {
                                    return_int_execval(valstack, 0);
                                    goto fin_boucle;
                                }

                                if (int_ret != 0) {
                                    return_int_execval(valstack, int_ret);
                                    goto fin_boucle;
                                }
                                
                                
                                bloc++;
                                while (bloc < maintree->nbSons && maintree->sons[bloc]->type != TYPE_STATEMENTIF)
                                    bloc++;

                                // on vient d'exécuter le if, donc on a fini ce if, on peut supprimer le contexte

                                if (bloc == maintree->nbSons) {
                                    break;
                                }
                                else
                                    continue;
                            }
                            
                            bloc++;

                            PTRERROR = maintree->sons[bloc];


                            elif = false;
                            while (bloc < maintree->nbSons && maintree->sons[bloc]->type == TYPE_STATEMENTELIF)
                            {

                                // évaluation de la condition
                                cadre[2] = (void*)int_ret;
                                cadre[3] = (void*)bloc;
                                cadre[4] = (void*)inst;

                                stack_args_expr(maintree->sons[bloc]->sons[0], 1, varstack);
                                stack_ret_zone(21, retstack);
                                goto debut_boucle;
                                case 21:

                                cadre = varstack->tete;
                                get_tree(&tree, cadre);
                                int_ret = (intptr_t)cadre[2];
                                bloc = (intptr_t)cadre[3];
                                inst = (intptr_t)cadre[4];
                                maintree = tree->sons[inst];

                                expr = (NeObject*)ptrlist_pop(valstack);
                                cond = neoIsTrue(expr);
                                neobject_destroy(expr, true);

                                if (CODE_ERROR != 0) {
                                    return_int_execval(valstack, 0);
                                    goto fin_boucle;
                                }

                                if (!elif && cond)
                                {
                                    // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
                                    newContext();

                                    cadre[2] = (void*)int_ret;
                                    cadre[3] = (void*)bloc;
                                    cadre[4] = (void*)inst;

                                    stack_args(maintree->sons[bloc]->sons[1], varstack);
                                    stack_ret_zone(22, retstack);
                                    goto debut_boucle;
                                    case 22:

                                    cadre = varstack->tete;
                                    get_tree(&tree, cadre);
                                    int_ret = (intptr_t)cadre[2];
                                    bloc = (intptr_t)cadre[3];
                                    inst = (intptr_t)cadre[4];
                                    maintree = tree->sons[inst];

                                    int_ret = (intptr_t)ptrlist_pop(valstack);

                                    deleteContext(); // on vient d'exécuter le code, donc on a fini le if


                                    if (CODE_ERROR != 0) {
                                        return_int_execval(valstack, 0);
                                        goto fin_boucle;
                                    }

                                    if (int_ret != 0) {
                                        return_int_execval(valstack, int_ret);
                                        goto fin_boucle;
                                    }

                                    
                                    elif = true;
                                }
                                bloc++;
                            }

                            PTRERROR = maintree->sons[bloc];
                            
                            if (bloc < maintree->nbSons && maintree->sons[bloc]->type == TYPE_STATEMENTELSE) // s'il y a un bloc else
                            {
                                if (!elif) // si on a le droit d'y aller
                                {
                                    newContext();

                                    cadre[2] = (void*)int_ret;
                                    cadre[3] = (void*)bloc;
                                    cadre[4] = (void*)inst;

                                    stack_args(maintree->sons[maintree->nbSons - 1], varstack);
                                    stack_ret_zone(23, retstack);
                                    goto debut_boucle;
                                    case 23:

                                    cadre = varstack->tete;
                                    get_tree(&tree, cadre);
                                    int_ret = (intptr_t)cadre[2];
                                    bloc = (intptr_t)cadre[3];
                                    inst = (intptr_t)cadre[4];
                                    maintree = tree->sons[inst];

                                    int_ret = (intptr_t)ptrlist_pop(valstack);

                                    deleteContext(); // on vient d'exécuter le code, donc on a fini le if

                                    if (CODE_ERROR != 0) {
                                        return_int_execval(valstack, 0);
                                        goto fin_boucle;
                                    }

                                    if (int_ret != 0) {
                                        return_int_execval(valstack, int_ret);
                                        goto fin_boucle;
                                    }

                                }
                                bloc++;
                            }
                        }

                        cadre = varstack->tete;
                        get_tree(&tree, cadre);
                        int_ret = (intptr_t)cadre[2];
                        inst = (intptr_t)cadre[4];

                    }

                    

                    else if (tree->sons[inst]->type == TYPE_STATEMENTWHILE)
                    {
                        int_ret = 0;


                        // évaluation de la condition
                        cadre[2] = (void*)int_ret;
                        cadre[3] = (void*)inst;

                        stack_args_expr(tree->sons[inst]->sons[0], 1, varstack);
                        stack_ret_zone(24, retstack);
                        goto debut_boucle;
                        case 24:

                        cadre = varstack->tete;
                        get_tree(&tree, cadre);
                        int_ret = (intptr_t)cadre[2];
                        inst = (intptr_t)cadre[3];

                        expr = (NeObject*)ptrlist_pop(valstack);
                        cond = neoIsTrue(expr);
                        neobject_destroy(expr, true);

                        if (CODE_ERROR != 0) {
                            return_int_execval(valstack, 0);
                            goto fin_boucle;
                        }

                        while (cond)
                        {

                            cadre[2] = (void*)int_ret;
                            cadre[3] = (void*)inst;

                            stack_args(tree->sons[inst]->sons[1], varstack);
                            stack_ret_zone(25, retstack);
                            goto debut_boucle;
                            case 25:

                            cadre = varstack->tete;
                            get_tree(&tree, cadre);
                            int_ret = (intptr_t)cadre[2];
                            inst = (intptr_t)cadre[3];

                            int_ret = (intptr_t)ptrlist_pop(valstack);

                            
                            if (CODE_ERROR != 0) {
                                return_int_execval(valstack, 0);
                                goto fin_boucle;
                            }
                            
                            if (int_ret != 0)
                                break;

                            // réévaluation de la condition

                            stack_args_expr(tree->sons[inst]->sons[0], 1, varstack);
                            stack_ret_zone(26, retstack);
                            goto debut_boucle;
                            case 26:

                            cadre = varstack->tete;
                            get_tree(&tree, cadre);
                            int_ret = (intptr_t)cadre[2];
                            inst = (intptr_t)cadre[3];

                            expr = (NeObject*)ptrlist_pop(valstack);
                            cond = neoIsTrue(expr);
                            neobject_destroy(expr, true);

                            if (CODE_ERROR != 0) {
                                return_int_execval(valstack, 0);
                                goto fin_boucle;
                            }
                            
                        }

                        
                        if (int_ret == EXIT_RETURN) { // en sortant d'une boucle, on n'a pas de break ou continue
                            return_int_execval(valstack, int_ret);
                            goto fin_boucle;
                        }
                    }
                    
                    

                    else if (tree->sons[inst]->type == TYPE_STATEMENTFOR)
                    {
                        if (tree->sons[inst]->sons[0]->sons[0]->type != TYPE_VARIABLE)
                        {
                            CODE_ERROR = 22;
                            return_int_execval(valstack, 0);
                            goto fin_boucle;
                        }
                        

                        cadre[2] = (void*)inst;

                        stack_args_expr(tree->sons[inst]->sons[0]->sons[1], 1, varstack);
                        stack_ret_zone(27, retstack);
                        goto debut_boucle;
                        case 27:

                        cadre = varstack->tete;
                        get_tree(&tree, cadre);
                        inst = (intptr_t)cadre[2];

                        valeur = (NeObject*)ptrlist_pop(valstack);


                        var = tree->sons[inst]->sons[0]->sons[0]->data; // variable à incrémenter lors de la boucle

                        newContext(); // nouveau contexte pour rendre des variables locales à la boucle for

                        save_later(variables_a_sauvegarder, var);
                        local(var); // on localise l'indice de la boucle

                        neobject_destroy(var, false); // on va mettre des nouveaux trucs dedans

                        
                        if (CODE_ERROR != 0)
                        {
                            neobject_destroy(valeur, true);
                            deleteContext();
                            return_int_execval(valstack, 0);
                            goto fin_boucle;
                        }
                        
                        if (valeur -> type != TYPE_NUMBER)
                        {
                            neobject_destroy(valeur, true);
                            CODE_ERROR = 10;
                            deleteContext();
                            return_int_execval(valstack, 0);
                            goto fin_boucle;
                        }


                        // le vrai indice de la boucle sera un entier
                        ext_index = (intptr_t)number_toDouble(neo_to_nb(valeur));
                        neobject_destroy(valeur,true);


                        cadre[2] = (void*)inst;
                        cadre[3] = (void*)var;
                        cadre[4] = (void*)ext_index;
                        stack_args_expr(tree->sons[inst]->sons[0]->sons[2], 1, varstack);
                        stack_ret_zone(28, retstack);
                        goto debut_boucle;
                        case 28:

                        cadre = varstack->tete;
                        get_tree(&tree, cadre);
                        inst = (intptr_t)cadre[2];
                        var = (NeObject*)cadre[3];
                        ext_index = (intptr_t)cadre[4];

                        NeObject* tempMax = (NeObject*)ptrlist_pop(valstack);


                        if (CODE_ERROR != 0)
                        {
                            neobject_destroy(tempMax, true);
                            deleteContext();
                            return_int_execval(valstack, 0);
                            goto fin_boucle;
                        }

                        if (tempMax -> type != TYPE_NUMBER) {
                            CODE_ERROR = 10;
                            neobject_destroy(tempMax, true);
                            deleteContext();
                            return_int_execval(valstack, 0);
                            goto fin_boucle;
                        }
                        
                        max = (intptr_t)number_toDouble(neo_to_nb(tempMax)); // borne supérieure des valeurs atteintes par la variable

                        neobject_destroy(tempMax, true); // du coup le data, on le garde donc on libère que l'enveloppe

                        int_ret = 0;
                        
                        while (ext_index < max)
                        {
                            // on restaure la valeur de l'indice au début du corps de la boucle
                            NeObject* tempnb = neo_nb_create(number_fromDouble((double)ext_index));
                            var->data = tempnb->data;
                            var->type = tempnb->type;
                            err_free(tempnb);


                            cadre[2] = (void*)inst;
                            cadre[3] = (void*)var;
                            cadre[4] = (void*)ext_index;
                            cadre[5] = (void*)max;
                            cadre[6] = (void*)int_ret;
                            stack_args(tree->sons[inst]->sons[1], varstack);
                            stack_ret_zone(29, retstack);
                            goto debut_boucle;
                            case 29:

                            cadre = varstack->tete;
                            get_tree(&tree, cadre);
                            inst = (intptr_t)cadre[2];
                            var = (NeObject*)cadre[3];
                            ext_index = (intptr_t)cadre[4];
                            max = (intptr_t)cadre[5];
                            int_ret = (intptr_t)cadre[6];


                            int_ret = (intptr_t)ptrlist_pop(valstack);

                            neobject_destroy(var, false);


                            if (CODE_ERROR != 0)
                            {
                                deleteContext();
                                return_int_execval(valstack, 0);
                                goto fin_boucle;
                            }

                            if (int_ret != 0)
                                break;
                            
                            ext_index++;
                        }

                        deleteContext();
                        
                        if (int_ret == EXIT_RETURN) {
                            return_int_execval(valstack, int_ret);
                            goto fin_boucle;
                        }
                    }

                    

                    else if (tree->sons[inst]->type == TYPE_FUNCTIONDEF)
                    {
                        // définition de fonction en cours d'exécution
                        int index = strlist_index(NOMS, tree->sons[inst]->label1);
                        if (ADRESSES->tab[index] != tree->sons[inst]->data)
                            _affect2(ADRESSES->tab[index], tree->sons[inst]->data);

                    }
                    
                    else // expression
                    {
                        cadre[2] = (void*)inst;

                        stack_args_expr(tree->sons[inst], 1, varstack);
                        stack_ret_zone(18, retstack);
                        goto debut_boucle;
                        case 18:

                        // on récupère les valeurs
                        cadre = varstack->tete;
                        inst = (intptr_t)cadre[2];
                        
                        neobject_destroy((NeObject*)ptrlist_pop(valstack), true); // sinon, évaluation de l'expression, en la libérant juste après
                    }


                    if (CODE_ERROR != 0) { // ben oui sinon les erreurs ne seront pas captées au bon endroit
                        return_int_execval(valstack, 0);
                        goto fin_boucle;
                    }

                    cadre = varstack->tete;
                    get_tree(&tree, cadre);

                    
                    
                }
                return_int_execval(valstack, 0);
                goto fin_boucle;


            }


        }

        fin_boucle:

        // si on arrive ici c'est qu'il n'y avait pas d'appel récursif à lancer donc on peut supprimer le cadre de pile actuel
        err_free(ptrlist_pop(varstack)); // on n'aura plus besoin de ce cadre de pile

        // par construction, si retstack est vide c'est forcément le processus principal

        if (ptrlist_isEmpty(retstack) && process_cycle->next == NULL && process_cycle->prev == NULL && process_cycle->process->wait_expr == NULL) { // on vient de finir le processus principal et il ne reste plus aucun processus
            NeObject* ret = ptrlist_pop(valstack);

            // supprime le dernier processus
            ptrlist_destroy(valstack, false, true);
            ptrlist_destroy(varstack, false, true);
            ptrlist_destroy(retstack, false, true);

            neobject_destroy(un,true);

            err_free(process_cycle->process);
            err_free(process_cycle);

            ptrlist_destroy(variables_a_sauvegarder, true, true);

            VAR_LOC = var_loc_sov;

            return ret;
        }
        else if (ptrlist_isEmpty(retstack) && process_cycle->process->wait_expr == NULL) // il reste des processus annexes à exécuter
        {
            /////////// on vient de finir un thread, donc on catch l'erreurprocessCycle_remove maintenant

            // on est donc forcément sur le processus principal
            mainThreadReturnValue = ptrlist_pop(valstack);

            // on supprime le processus, on passe au suivant
            process_cycle = processCycle_remove(process_cycle, &varstack, &valstack, &retstack, &variables_a_sauvegarder);

            goto debut_boucle;

        }


        // on dépile la valeur qui nous dit où revenir
        zone_retour = (intptr_t)ptrlist_pop(retstack);
        goto label_return; // et on retourne au bon endroit
        // s'il n'y avait pas d'endroit particulier à revenir, on revient en zéro


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
    setColor(GREEN);
    printString("Welcome to Neon");
    setColor(WHITE);
    newLine();
    
    printString("Neon v");
    printString(VERSION);
    printString(" compiled on ");
    printString(__TIMESTAMP__);
    printString(".");
    newLine();
    
    printString("Discuss on the official Discord server : ");
    printString(INVITATION);
    printString(" or contact me at raphael@langage-neon.org");
    newLine();
    
    printString("Visit ");
    setColor(GREEN); printString("https://langage-neon.org"); setColor(WHITE);
    printString(" for more information");
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
    
    	Tree* tree = tree_create(NULL, 0, 0);
    	createSyntaxTree(tree, code);
        if (CODE_ERROR != 0)
        {
            printError(CODE_ERROR);
            CODE_ERROR = 0;
            tree_destroy(tree);
            return;
        }
    	// s'il n'y a qu'une expression, alors, on affiche le résultat de l'expression
    	if (tree->nbSons == 1 && tree->sons[0]->type != TYPE_TRYEXCEPT && tree->sons[0]->type != TYPE_CONDITIONBLOCK && tree->sons[0]->type != TYPE_STATEMENTFOR && tree->sons[0]->type != TYPE_STATEMENTWHILE && tree->sons[0]->type != TYPE_KEYWORD && tree->sons[0]->type != TYPE_FUNCTIONDEF && tree->sons[0]->type != TYPE_BLOCKWORD1LINE)
        {
            
            res = execval(tree->sons[0]);


            if (CODE_ERROR > 1)
            {
                if (PTRERROR == NULL)
                    PTRERROR = tree;

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







void terminal (void)
{
    
    
    char* exp;
    NeObject* res ; Tree* tree;
        
        while (true)
        {

            CODE_ERROR = 0; // réinitialise les erreurs
            PTRERROR = NULL; // réinitialise le pointeur d'erreur

            exp = inputCode(SEQUENCE_ENTREE);

            if (CODE_ERROR > 1)
            {
                printError(CODE_ERROR);
                CODE_ERROR = 0;
                continue;
            }
            else if (CODE_ERROR == 1)
                return;

            if (exp == NULL) // pour afficher le keyboardInterrupt
                //CODE_ERROR = 104;
                return;



            if (strcmp(exp,"")==0) // si l'utilisateur n'a rien ecrit
            {
                err_free(exp);
                continue;
            }

            /*
            
            tree = tree_create(NULL, number_default(), 0);

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
            if (tree->nbSons == 1 && tree->sons[0]->type != TYPE_TRYEXCEPT && tree->sons[0]->type != TYPE_CONDITIONBLOCK && tree->sons[0]->type != TYPE_STATEMENTFOR && tree->sons[0]->type != TYPE_STATEMENTWHILE && tree->sons[0]->type != TYPE_KEYWORD && tree->sons[0]->type != TYPE_FUNCTIONDEF && tree->sons[0]->type != TYPE_BLOCKWORD1LINE)
            {
                
                res = execval(tree->sons[0]);


                if (CODE_ERROR > 1)
                {
                    if (PTRERROR == NULL)
                        PTRERROR = tree;

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
            tree_destroy(tree);
            
            */
            strlist* tokens = strlist_create(0);
            intlist types = intlist_create(0);
            Ast** ast;

            cut(tokens, &types, exp, true);

            if (CODE_ERROR == 0) {
                ast = ast_create(&types);
                
                parse(tokens, types, ast, 0);

                if (CODE_ERROR == 0)
                    statements(&types, tokens, ast, 0);
                
                strlist_aff(tokens);
                ast_aff(ast, tokens->len);

                ast_destroy(ast, tokens->len);
            }

            strlist_destroy(tokens, true);
            err_free(types.tab);

            if (CODE_ERROR != 0)
                printError(CODE_ERROR);

            err_free(exp);
            

            
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
    

    /*
    // exécution du fichier
    Tree* tree = tree_create(NULL, 0, 0);
    createSyntaxTree(tree, program);
    err_free(program);

    if (CODE_ERROR != 0)
    {
        printError(CODE_ERROR);
        CODE_ERROR=0;
        tree_destroy(tree);
        return;
    }

    
    execval(tree);
    
    if (CODE_ERROR > 1)
    {
        printError(CODE_ERROR);
    }

    tree_destroy(tree);*/

    strlist* tokens = strlist_create(0);
    intlist types = intlist_create(0);
    cut(tokens, &types, program, false);

    Ast** ast = ast_create(&types);
    parse(tokens, types, ast, 0);

    statements(&types, tokens, ast, 0);

    printf("Parsing terminé\n");

    ast_destroy(ast, tokens->len);
    strlist_destroy(tokens, true);
    err_free(types.tab);
    err_free(program);
    
    CODE_ERROR = 0;
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
    
    // exécution du fichier
    Tree* tree = tree_create(NULL, 0, 0);
    createSyntaxTree(tree, program);
    err_free(program);

    if (CODE_ERROR != 0)
    {
        tree_destroy(tree);
        return;
    }

    
    execval(tree);

    tree_destroy(tree);
    return ;
}




void defineVariables(void)
{

    // valeurs spéciales mathématiques
    #ifndef TI83PCE
        strlist_append(NOMS, strdup("Infinity"));
        nelist_append(ADRESSES, neo_nb_create(number_fromDouble(INFINITY)));
    
        strlist_append(NOMS, strdup("NegInfinity"));
        nelist_append(ADRESSES, neo_nb_create(number_fromDouble(-INFINITY)));
    
        strlist_append(NOMS, strdup("NaN"));
        nelist_append(ADRESSES, neo_nb_create(number_fromDouble(NAN)));
    #endif


    // définition de la variable de version
    strlist_append(NOMS, strdup("__version__"));
    nelist_append(ADRESSES, neo_str_create(strdup(VERSION)));

    // définition de la variable de plateforme
    #ifdef LINUX
        char* p = "linux";
    #endif
    #ifdef WINDOWS11
        char* p = "windows11";
    #endif
    #ifdef WINDOWS10
        char* p = "windows10";
    #endif
    #ifdef TI83PCE
        char* p = "ti83pce";
    #endif
    #ifdef WASM
        char* p = "wasm";
    #endif

    strlist_append(NOMS, strdup("__platform__"));
    nelist_append(ADRESSES, neo_str_create(strdup(p)));

    strlist_append(NOMS, strdup("__name__"));
    nelist_append(ADRESSES, NULL);

    NAME = ADRESSES->len - 1; // l'adresse de __name__ à modifier


    // pi
    nelist_append(ADRESSES, neo_nb_create(number_fromDouble(PI)));
    strlist_append(NOMS, strdup("pi"));


}












void neonInit(void)
{

    #if defined(LINUX) || defined(WINDOWS11) || defined(WINDOWS10)
        signal(SIGINT, handle_sigint);
    #endif


    //définition de acceptedChars
    const char* acceptedChars_tab[31] = {"\"", "'", "+","*","-","/","<",">","=","%","&","@","!", ",", ";", "\n", "#", "$", "[", "]", "(", ")", "{", "}", "\\", ".", "_", " ", "\t", ".", ":"};
    strlist_copy(&acceptedChars, acceptedChars_tab, 31);





    // definit les types de tokens composes et indique par quel type de token compose ils peuvent etre precedes

    // les tableaux à l'intérieur ne sont pas des const, car sinon ça pose un problème avec le type strlist. Ce n'est pas grave, j'enlèverais ce tableau bientôt pour mettre quelque chose de moins "lourd"
    const intlist syntax_tab[20] = {
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_KEYWORDFUNCTION} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1, TYPE_NUMBER,TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_FONCTION, TYPE_BOOL, TYPE_LISTINDEX, TYPE_ENDOFLINE,  TYPE_OPERATOR, TYPE_PARENTHESE2, TYPE_CONST, TYPE_VIRGULE, TYPE_NONE, TYPE_EXCEPTION} , .len = 15},//suivant la grammaire de chaque operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_CROCHET} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_BLOCKLINE, TYPE_KEYWORD, TYPE_FONCTION} , .len = 4},
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE,TYPE_PARENTHESE2} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_BLOCK} , .len = 2},
            (intlist) {.tab = (int[]) {TYPE_VIRGULE, TYPE_NUMBER, TYPE_PARENTHESE1, TYPE_VARIABLE, TYPE_LISTINDEX, TYPE_BOOL, TYPE_OPERATOR, TYPE_LIST, TYPE_STRING, TYPE_BLOCKLINE, TYPE_BLOCK, TYPE_FONCTION, TYPE_PARENTHESE2, TYPE_KEYWORD, TYPE_CONST, TYPE_NONE, TYPE_BLOCKWORD1LINE, TYPE_EXCEPTION} , .len = 18}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_VIRGULE, TYPE_PARENTHESE1} , .len = 4},
            (intlist) {.tab = (int[]) {TYPE_OPERATOR, TYPE_NUMBER, TYPE_STRING, TYPE_LIST, TYPE_FONCTION, TYPE_LISTINDEX, TYPE_VARIABLE, TYPE_PARENTHESE2,TYPE_BOOL} , .len = 9},
            (intlist) {.tab = (int[]) {TYPE_NUMBER,TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_FONCTION, TYPE_BOOL, TYPE_LISTINDEX, TYPE_ENDOFLINE, TYPE_OPERATOR, TYPE_CONST, TYPE_NONE, TYPE_EXCEPTION} , .len = 12},
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_BLOCK} , .len = 2},
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE}, .len = 1},
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE}, .len = 1},
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE}, .len = 4}
        };


    listlist_copy(&syntax, syntax_tab, 20);

    // types acceptés en début d'entrée

    
    int types_debut_temp[21] = {TYPE_FONCTION, TYPE_PARENTHESE1, TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_LISTINDEX, TYPE_NUMBER, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_BOOL, TYPE_BLOCKLINE, TYPE_KEYWORD, TYPE_CONST, TYPE_WORD, TYPE_COMMENT, TYPE_CROCHET, TYPE_BLOCKWORD, TYPE_BLOCKWORD1LINE, TYPE_EXPRESSION, TYPE_NONE, TYPE_EXCEPTION};
    intlist_copy(&types_debut, types_debut_temp, 21);

    int types_fin_temp[20] = {TYPE_FONCTION, TYPE_PARENTHESE2, TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_LISTINDEX, TYPE_NUMBER, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_BOOL, TYPE_KEYWORD, TYPE_CONST, TYPE_WORD, TYPE_COMMENT, TYPE_CROCHET, TYPE_EXPRESSION, TYPE_NONE, TYPE_EXCEPTION, TYPE_BLOCK, TYPE_BLOCKWORD1LINE};
    intlist_copy(&types_fin, types_fin_temp, 20);


    //définition de sousop
    const char* sousop_temp[] = {"+","*","-","/","<",">","=","%","&","@","!", ".",":"};
    strlist_copy(&sousop, sousop_temp, 13);


    // liste qui contient les opérandes prises par les opérateurs, correspond a operateurs3
    int gramm1_temp[NBOPERATEURS] = {RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,RIGHT, RIGHT, VARRIGHT, LEFT_VAR, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, VAR_VAR, VAR_RIGHT, 0, RIGHT};
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


    const int PRIORITE_temp[NBOPERATEURS] = {8, 8, 8, 5, 4, 5, 4, 3, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 9, 3, 3, 7, 4, 4, 2, 9, 2, 4, 9, 9, 2, 4, 8, 6, 1, 9, 9, 9, 9, 8};
    intlist_copy(&PRIORITE, PRIORITE_temp, NBOPERATEURS);

    const int OPERANDES_temp[NBOPERATEURS] = {RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,RIGHT, RIGHT, VARRIGHT, LEFT_VAR, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, VAR_VAR, VAR_RIGHT, 0, RIGHT};
    intlist_copy(&OPERANDES, OPERANDES_temp, NBOPERATEURS);


    // ------------------

    // défnit les tokens de délimitation de blocks

    //mots de blocs d'instructions
    const char* blockwords_temp[] = {"if","while","for", "ei", "return", "import", "local", "tr", "expt", "await", "atomic"};
    strlist_copy(&blockwords, blockwords_temp, 11);

    const char* blockwords1Line_temp[] = {"return", "import", "local", "await"};
    strlist_copy(&blockwords1Line, blockwords1Line_temp, 4);

    const char* keywordFunction_temp[] = {"function", "method"};
    strlist_copy(&keywordFunction, keywordFunction_temp, 2);

    const char* keywords_temp[] = {"es", "continue", "break", "pass", "tr", "atomic"};
    strlist_copy(&keywords, keywords_temp, 6);

    const char* lkeywords_temp[] = {"continue", "break", "pass"};
    strlist_copy(&lkeywords, lkeywords_temp, 3);


    const char* boolean_temp[] = {"True","False"};
    strlist_copy(&boolean, boolean_temp, 2);

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
    

    const char* constant_temp[] = {"None"};
    strlist_copy(&constant, constant_temp, 1);




    /*----- Préparation des fonctions ------*/
    const char* NOMSBUILTINSFONC_temp[NBBUILTINFONC] = {"print","input","nbr","str","len","sub","exit","append","remove","insert","type", "reverse", "eval","clear","help", "randint", "failwith", "time", "assert", "output", "chr", "ord", "listComp", "createException", "exception", "int", "index", "replace", "count", "list", "sortAsc", "sortDesc", "sin", "cos", "tan", "deg", "rad", "sqrt", "ln", "exp", "log", "log2", "round", "abs", "ceil", "floor", "readFile", "writeFile", "setFunctionDoc", "setAtomicTime"};
    strlist_copy(&NOMSBUILTINSFONC, NOMSBUILTINSFONC_temp, NBBUILTINFONC);

    const int typesRetour[NBBUILTINFONC] = {TYPE_NONE, TYPE_STRING, TYPE_NUMBER, TYPE_STRING, TYPE_NUMBER, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_STRING, TYPE_STRING, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NUMBER, TYPE_NONE, TYPE_NUMBER, TYPE_NONE, TYPE_NONE, TYPE_STRING, TYPE_NUMBER, TYPE_LIST, TYPE_EXCEPTION, TYPE_NONE, TYPE_NUMBER, TYPE_NUMBER, TYPE_STRING, TYPE_NUMBER, TYPE_LIST, TYPE_NONE, TYPE_NONE, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NONE};

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
        "Time to allow for each process before switching"
    };


    //indique le nombre de premiers arguments à ne pas copier
    const int typesbuiltinsfonc[NBBUILTINFONC] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}; // le nombre d'arguments "méthode"


    const int nbargs[NBBUILTINFONC] = {-1,-1,1,1,1,3,0,2,2,3,1,1,1,0,1,2,1,0,-1,-1,1,1,6,1,1,1,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,2,2,1}; // nombre d'arguments en tout

    // les fonctions ayant un nombre illimité d'arguments ne doivent avoir qu'un seul élément dans typeArgs
    const int* typesArgs[NBBUILTINFONC] = {
        (int[]){-1},
        (int[]){-1},
        (int[]){TYPE_STRING},
        (int[]){-1},
        (int[]){-1},
        (int[]){TYPE_STRING, TYPE_NUMBER, TYPE_NUMBER},
        NULL,
        (int[]){TYPE_LIST, -1},
        (int[]){TYPE_LIST, TYPE_NUMBER},
        (int[]){TYPE_LIST, -1, TYPE_NUMBER},
        (int[]){-1},
        (int[]){TYPE_STRING},
        (int[]){TYPE_STRING},
        NULL,
        (int[]){-1},
        (int[]){TYPE_NUMBER, TYPE_NUMBER},
        (int[]){TYPE_STRING},
        NULL,
        (int[]){TYPE_BOOL},
        (int[]){-1},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_STRING},
        (int[]){TYPE_STRING, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_STRING, TYPE_STRING},
        (int[]){TYPE_STRING},
        (int[]){TYPE_EXCEPTION},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_LIST, -1},
        (int[]){TYPE_STRING, TYPE_STRING, TYPE_STRING},
        (int[]){-1, -1},
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
        (int[]){-1, TYPE_STRING},
        (int[]){TYPE_NUMBER}
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
        "Program interrupted" // code 104 spécial pour intercepter un CTRL-C
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
        14
    };

    intlist_copy(&exceptions_err, exceptions_err_temp, NB_ERRORS);



    // ----- fin messages d'erreur

    CONTAINERS = strlist_create(0);
    ATTRIBUTES = nelist_create(0);


    PROMISES = nelist_create(0);
    PROMISES_CNT = intlist_create(0);
    PROCESS_FINISH = intlist_create(0);


    VAR_LOC = ptrlist_create();

    FONCTIONS = tree_create(NULL, number_default(), 0);
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
    strlist_destroy(&boolean, false);
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

            // ajout des argument dans le tableau contenant les arguments du programme
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
