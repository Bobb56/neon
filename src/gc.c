#define NEON_SOURCE_ID 6

#include "headers/constants.h"
#include "headers/contexts.h"
#include "headers/neobj.h"
#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/gc.h"
#include "headers/neon.h"
#include "headers/errors.h"
#include "headers/processcycle.h"

// Cette fonction implémente une exception lors de la suppression d'objets dans une garbage collection
// Parfois, certains objets, bien qu'ils soient dans le garbage collector et éligibles à la suppression car rien ne pointe sur eux, le runtime a encore besoin qu'ils vivent pour fonctionner correctement
// Exemple : Cela pourrait arriver qu'une UserFunc en cours d'exécution soit supprimée car deviendrait inaccessible depuis le runtime, mais cette fonction a quand même besoin de vivre jusqu'à la fin de son exécution
bool gc_excluded(NeObj neo) {
    if (NEO_TYPE(neo) == TYPE_USERFUNC) {
        if (neo.userfunc->runningInstances > 0) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}



// récupère l'objet d'après dans la liste chaînée
NeObj get_next(const NeObj obj) {
    if (NEO_TYPE(obj) == TYPE_CONTAINER) {
        Container* c = neo_to_container(obj);
        return c->next;
    }
    else if (NEO_TYPE(obj) == TYPE_LIST) {
        NeList* l = neo_to_list(obj);
        return l->next;
    }
    else if (NEO_TYPE(obj) == TYPE_USERFUNC) {
        UserFunc* f = neo_to_userfunc(obj);
        return f->next;
    }
    else {
        return NEO_VOID;
    }
}


NeObj get_prev(const NeObj obj) {
    if (NEO_TYPE(obj) == TYPE_CONTAINER) {
        Container* c = neo_to_container(obj);
        return c->prev;
    }
    else if (NEO_TYPE(obj) == TYPE_LIST) {
        NeList* l = neo_to_list(obj);
        return l->prev;
    }
    else if (NEO_TYPE(obj) == TYPE_USERFUNC) {
        UserFunc* f = neo_to_userfunc(obj);
        return f->prev;
    }
    else {
        return NEO_VOID;
    }
}


// change l'objet d'après dans la liste chaînée
// Remarque: On ne modifie pas la structure, mais le Container ou la NeList en eux-mêmes
void set_next(NeObj obj, NeObj next) {
    if (NEO_TYPE(obj) == TYPE_CONTAINER) {
        Container* c = neo_to_container(obj);
        c->next = next;
    }
    else if (NEO_TYPE(obj) == TYPE_LIST) {
        NeList* l = neo_to_list(obj);
        l->next = next;
    }
    else if (NEO_TYPE(obj) == TYPE_USERFUNC) {
        UserFunc* f = neo_to_userfunc(obj);
        f->next = next;
    }
}

void set_prev(NeObj obj, NeObj prev) {
    if (NEO_TYPE(obj) == TYPE_CONTAINER) {
        Container* c = neo_to_container(obj);
        c->prev = prev;
    }
    else if (NEO_TYPE(obj) == TYPE_LIST) {
        NeList* l = neo_to_list(obj);
        l->prev = prev;
    }
    else if (NEO_TYPE(obj) == TYPE_USERFUNC) {
        UserFunc* f = neo_to_userfunc(obj);
        f->prev = prev;
    }
}


// ajoute un objet profond (liste ou container) dans la liste chaînée d'objets suivis
void gc_add_deep_object(const NeObj next) {
    set_next(next, global_env->OBJECTS_LIST);
    set_prev(global_env->OBJECTS_LIST, next);
    global_env->OBJECTS_LIST = next;
}

// on supprime l'objet de la liste chaînée
void gc_remove_container(Container* c) {
    if (neo_is_void(c->prev)) { // c'est le premier élément de la chaîne
        global_env->OBJECTS_LIST = c->next;
        if (!neo_is_void(global_env->OBJECTS_LIST))
            set_prev(global_env->OBJECTS_LIST, NEO_VOID);
    }
    else {
        set_next(c->prev, c->next);

        if (!neo_is_void(c->next))
            set_prev(c->next, c->prev);
    }
}

void gc_remove_nelist(NeList* l) {
    if (neo_is_void(l->prev)) { // c'est le premier élément de la chaîne
        global_env->OBJECTS_LIST = l->next;
        if (!neo_is_void(global_env->OBJECTS_LIST))
            set_prev(global_env->OBJECTS_LIST, NEO_VOID);
    }
    else {
        set_next(l->prev, l->next);
        
        if (!neo_is_void(l->next))
            set_prev(l->next, l->prev);
    }
}


void gc_remove_userfunc(UserFunc* f) {
    if (neo_is_void(f->prev)) { // c'est le premier élément de la chaîne
        global_env->OBJECTS_LIST = f->next;
        if (!neo_is_void(global_env->OBJECTS_LIST))
            set_prev(global_env->OBJECTS_LIST, NEO_VOID);
    }
    else {
        set_next(f->prev, f->next);

        if (!neo_is_void(f->next))
            set_prev(f->next, f->prev);
    }
}


// comme les objets qu'on a ajoutés ont subi un neo_copy, il faut les supprimer proprement
void gc_free_objects_list(void) {
    NeObj temp;
    for (NeObj ptr = global_env->OBJECTS_LIST ; !neo_is_void(ptr) ;) {
        temp = get_next(ptr);
        neobject_destroy(ptr);
        ptr = temp;
    }
}


/*
Détruit les objets simples et les listes/containers encore accessibles qui sont dans la liste à une profondeur 1
*/
void nelist_partial_destroy(NeList* list) {
    for (int i=0 ; i < list->len ; i++) {
        // si l'objet est soit une feuille, soit une liste ou un container encore accessible depuis adresses
        // les objets ne vérifiant pas cette condition sont les objets qui sont à supprimer dans global_env->OBJECTS_LIST
        if ((NEO_TYPE(list->tab[i]) != TYPE_CONTAINER && NEO_TYPE(list->tab[i]) != TYPE_LIST && NEO_TYPE(list->tab[i]) != TYPE_USERFUNC) || ismarked(list->tab[i])) {
            neobject_destroy(list->tab[i]);
        }
    }
}

/*
Cette fonction supprime les objets simples qui se situent dans neo si neo est un container ou une liste
avec un niveau de profondeur de 1.
*/
void neobject_partial_destroy(NeObj neo)
{
    if (NEO_TYPE(neo) == TYPE_LIST) {
        nelist_partial_destroy(neo_to_list(neo));
    }
    else if (NEO_TYPE(neo) == TYPE_CONTAINER) {
        Container* c = neo_to_container(neo);
        nelist_partial_destroy(c->data);
    }
    else if (NEO_TYPE(neo) == TYPE_USERFUNC) {
        UserFunc* f = neo_to_userfunc(neo);
        nelist_partial_destroy(f->opt_args);
    }
    // on ne peut pas finir la suppression totale de l'objet car on a besoin de démarquer à la fin tous les objets
}






// marque profondément toutes les listes et containers contenues dans la nelist
void gc_nelist_mark(NeList* list) {
    for (int i=0 ; i < list->len ; i++) {
        gc_mark(list->tab[i]);
    }
}




void gc_mark(NeObj obj) {
    if (ismarked(obj))
        return;

    if (NEO_TYPE(obj) == TYPE_CONTAINER) {
        mark(obj);
        Container* c = neo_to_container(obj);
        gc_nelist_mark(c->data);
    }
    else if (NEO_TYPE(obj) == TYPE_LIST) {
        mark(obj);
        gc_nelist_mark(neo_to_list(obj));
    }
    else if (NEO_TYPE(obj) == TYPE_USERFUNC) {
        mark(obj);
        UserFunc* f = neo_to_userfunc(obj);
        gc_nelist_mark(f->opt_args);
    }
}



void gc_var_loc_mark(ContextStack* var_loc) {
    for (int i=0 ; i < var_loc->len ; i++) {
        if (!isContextMark(var_loc->tab[i])) {
            gc_mark(var_loc->tab[i].object);
        }
    }
}

// on parcourt tous les contextes de tous les processus pour marquer les variables qu'ils ont sauvegardés
void gc_processes_var_loc_mark(ProcessCycle* pc) {
    ProcessCycle* ptr = pc;
    do {
        gc_var_loc_mark(&ptr->process->var_loc);
        ptr = ptr->next;
    } while (ptr != pc);
}





// affiche l'ensemble des objets traqués par le GC
void print_objects_list(void) {
    if (neo_is_void(global_env->OBJECTS_LIST)) {
        setColor(BLUE) ; printString("The Garbage Collector doesn't track any object."); newLine();
    }
    else {
        setColor(BLUE) ; printString("Objects currently tracked by the Garbage Collector :"); newLine();
        setColor(GREEN) ; printString("{ "); setColor(DEFAULT);
        NeObj next;
        for (NeObj ptr = global_env->OBJECTS_LIST ; !neo_is_void(ptr) ; ptr = next) {
            next = get_next(ptr);

            neobject_aff(ptr);

            if (!neo_is_void(next)) {
                setColor(GREEN) ; printString(" | "); setColor(DEFAULT) ;
            }
        }

        setColor(GREEN) ; printString(" }");
        newLine();
    }
    setColor(DEFAULT);
}


// implémentation de l'algorithme Mark & Sweep
void gc_mark_and_sweep(void) {
    if (neo_is_void(global_env->OBJECTS_LIST))
        return;

    // on marque tous les objets accessibles depuis les variables
    gc_nelist_mark(global_env->ADRESSES);
    gc_processes_var_loc_mark(global_env->process_cycle);

    // on parcourt maintenant tous les containers et listes
    // on fait une première passe dans laquelle on supprime de manière non récursive les objets présents
    for (NeObj ptr = global_env->OBJECTS_LIST ; !neo_is_void(ptr) ; ptr = get_next(ptr)) {

        if (!ismarked(ptr) && !gc_excluded(ptr)) {
            // l'objet n'est pas marqué, et n'est pas exclu de suppression, on va donc devoir nous en débarrasser
            // on commence par supprimer les objets qu'il contient avec une profondeur de 1.
            // en effet :
            // - soit c'est un objet accessible depuis ADRESSES, auquel cas il faut juste décrémenter son compteur de références
            // - sinon, pas besoin d'être récursif car il est à un autre endroit dans la global_env->OBJECTS_LIST et on va le supprimer aussi
            // a l'issue de cette suppression, les seuls objets restants à supprimer sont les NeObjects*, data et les refc des
            // containers et listes contenues dans global_env->OBJECTS_LISTS
            
            neobject_partial_destroy(ptr);
        }
    }

    // deuxième tour, où on termine de supprimer totalement les objets à supprimer

    NeObj prev = NEO_VOID;
    NeObj next;

    for (NeObj ptr = global_env->OBJECTS_LIST ; !neo_is_void(ptr) ; ptr = next) {

        if (ismarked(ptr)) { // l'objet est marqué, on doit donc le garder et on le démarque
            unmark(ptr);
            prev = ptr;
            next = get_next(prev);
        }
        else if (gc_excluded(ptr)) { // Si l'objet est exclu de suppression, on passe juste au suivant
            prev = ptr;
            next = get_next(prev);
        }
        else { // l'objet n'est pas marqué, on va donc devoir nous en débarrasser
            // on fait pointer prev sur next(ptr) au lieu de ptr et next sur prev au lieu de ptr
            next = get_next(ptr);

            if (neo_is_void(prev)) { // ca veut dire que ptr est global_env->OBJECTS_LIST
                global_env->OBJECTS_LIST = next;
                set_prev(global_env->OBJECTS_LIST, NEO_VOID);
            }
            else {
                set_next(prev, next); // la chaîne saute ptr

                if (!neo_is_void(next))
                    set_prev(next, prev);
            }

            if (NEO_TYPE(ptr) == TYPE_CONTAINER) {
                Container* c = neo_to_container(ptr);
                neon_free(c->data->tab);
                neon_free(c->data);
                neon_free(c);
            }
            else if (NEO_TYPE(ptr) == TYPE_USERFUNC) {
                UserFunc* f = neo_to_userfunc(ptr);
                neon_free(f->opt_args->tab);
                neon_free(f->opt_args);
                if (f->doc != NULL)
                    neon_free(f->doc);
                neon_free(f->args);
                neon_free(f);
            }
            else if (NEO_TYPE(ptr) == TYPE_LIST) {
                NeList* list = neo_to_list(ptr);
                neon_free(list->tab);
                neon_free(list);
            }
            // du coup le pointeur prev ne change pas
        }
    }
}




// variante de Mark & Sweep qui est faite pour fonctionner même si ADRESSES n'existe plus
// supprime tous les objets traqués, peu importe leur état, leur compteur de références
void gc_final_sweep(void) {
    if (neo_is_void(global_env->OBJECTS_LIST))
        return;

    // on parcourt maintenant tous les containers et listes
    // on fait une première passe dans laquelle on supprime de manière non récursive les objets présents
    for (NeObj ptr = global_env->OBJECTS_LIST ; !neo_is_void(ptr) ; ptr = get_next(ptr)) {
        neobject_partial_destroy(ptr);
    }

    // deuxième tour, où on termine de supprimer totalement les objets à supprimer

    NeObj next;
    for (NeObj ptr = global_env->OBJECTS_LIST ; !neo_is_void(ptr) ; ptr = next) {

        // l'objet n'est pas marqué, on va donc devoir nous en débarrasser
        // on fait pointer prev sur next(ptr) au lieu de ptr
        next = get_next(ptr);

        if (NEO_TYPE(ptr) == TYPE_CONTAINER) {
            Container* c = neo_to_container(ptr);
            neon_free(c->data->tab);
            neon_free(c->data);
            neon_free(c);
        }
        else if (NEO_TYPE(ptr) == TYPE_USERFUNC) {
            UserFunc* f = neo_to_userfunc(ptr);
            neon_free(f->opt_args->tab);
            neon_free(f->opt_args);
            if (f->doc != NULL)
                neon_free(f->doc);
            neon_free(f->args);
            neon_free(f);
        }
        else if (NEO_TYPE(ptr) == TYPE_LIST) {
            NeList* list = neo_to_list(ptr);
            neon_free(list->tab);
            neon_free(list);
        }
        // du coup le pointeur prev ne change pas
    }
}