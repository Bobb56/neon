#include <stdlib.h>

#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/dynarrays.h"
#include "headers/gc.h"

extern NeObj OBJECTS_LIST; // liste chaînée de containers et de listes
extern NeList* ADRESSES;
extern strlist* NOMS;

/*
Ajoute un objet profond (liste/container) à la banque d'objets du garbage collector
*/

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
}


void gc_add_deep_object(const NeObj next) {
    set_next(next, OBJECTS_LIST);
    set_prev(OBJECTS_LIST, next);
    OBJECTS_LIST = next;
}

// on supprime l'objet de la liste chaînée
void gc_remove_container(Container* c) {
    if (neo_is_void(c->prev)) { // c'est le premier élément de la chaîne
        OBJECTS_LIST = c->next;
        if (!neo_is_void(OBJECTS_LIST))
            set_prev(OBJECTS_LIST, NEO_VOID);
    }
    else {
        set_next(c->prev, c->next);

        if (!neo_is_void(c->next))
            set_prev(c->next, c->prev);
    }
}

void gc_remove_nelist(NeList* l) {
    if (neo_is_void(l->prev)) { // c'est le premier élément de la chaîne
        OBJECTS_LIST = l->next;
        if (!neo_is_void(OBJECTS_LIST))
            set_prev(OBJECTS_LIST, NEO_VOID);
    }
    else {
        set_next(l->prev, l->next);
        
        if (!neo_is_void(l->next))
            set_prev(l->next, l->prev);
    }
}



// comme les objets qu'on a ajoutés ont subi un neo_copy, il faut les supprimer proprement
void gc_free_objects_list(void) {
    NeObj temp;
    for (NeObj ptr = OBJECTS_LIST ; !neo_is_void(ptr) ;) {
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
        // les objets ne vérifiant pas cette condition sont les objets qui sont à supprimer dans OBJECTS_LIST
        if ((NEO_TYPE(list->tab[i]) != TYPE_CONTAINER && NEO_TYPE(list->tab[i]) != TYPE_LIST) || ismarked(list->tab[i])) {
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
}


void print_objects_list(void) {
    if (neo_is_void(OBJECTS_LIST)) {
        setColor(RED) ; printString("The Garbage Collector is empty.");
    }
    else {
        setColor(RED) ; printString("Objects currently tracked by the Garbage Collector :"); newLine();
        setColor(BLUE) ; printString("{ "); setColor(GREEN);
        NeObj next;
        for (NeObj ptr = OBJECTS_LIST ; !neo_is_void(ptr) ; ptr = next) {
            next = get_next(ptr);

            neobject_aff(ptr);

            if (!neo_is_void(next)) {
                setColor(BLUE) ; printString(" | "); setColor(GREEN) ;
            }
        }

        setColor(BLUE) ; printString(" }"); setColor(WHITE);
        newLine();
    }
}



void gc_mark_and_sweep(void) {

    if (neo_is_void(OBJECTS_LIST))
        return;

    // on marque tous les objets accessibles depuis les variables
    gc_nelist_mark(ADRESSES);

    // on parcourt maintenant tous les containers et listes
    // on fait une première passe dans laquelle on supprime de manière non récursive les objets présents
    for (NeObj ptr = OBJECTS_LIST ; !neo_is_void(ptr) ; ptr = get_next(ptr)) {

        if (!ismarked(ptr)) {
            // l'objet n'est pas marqué, on va donc devoir nous en débarrasser
            // on commence par supprimer les objets qu'il contient avec une profondeur de 1.
            // en effet :
            // - soit c'est un objet accessible depuis ADRESSES, auquel cas il faut juste décrémenter son compteur de références
            // - sinon, pas besoin d'être récursif car il est à un autre endroit dans la OBJECTS_LIST et on va le supprimer aussi
            // a l'issue de cette suppression, les seuls objets restants à supprimer sont les NeObjects*, data et les refc des
            // containers et listes contenues dans OBJECTS_LISTS
            neobject_partial_destroy(ptr);
        }
    }

    // deuxième tour, où on termine de supprimer totalement les objets à supprimer

    NeObj prev = NEO_VOID;
    NeObj next;

    for (NeObj ptr = OBJECTS_LIST ; !neo_is_void(ptr) ; ptr = next) {

        if (ismarked(ptr)) { // l'objet est marqué, on doit donc le garder et on le démarque
            unmark(ptr);
            prev = ptr;
            next = get_next(prev);
        }
        else { // l'objet n'est pas marqué, on va donc devoir nous en débarrasser
            // on fait pointer prev sur next(ptr) au lieu de ptr et next sur prev au lieu de ptr
            next = get_next(ptr);

            if (neo_is_void(prev)) { // ca veut dire que ptr est OBJECTS_LIST
                OBJECTS_LIST = next;
                set_prev(OBJECTS_LIST, NEO_VOID);
            }
            else {
                set_next(prev, next); // la chaîne saute ptr

                if (!neo_is_void(next))
                    set_prev(next, prev);
            }

            if (NEO_TYPE(ptr) == TYPE_CONTAINER) {
                Container* c = neo_to_container(ptr);
                free(c->data->tab);
                free(c->data);
                free(c);
            }
            else if (NEO_TYPE(ptr) == TYPE_LIST) {
                NeList* list = neo_to_list(ptr);
                free(list->tab);
                free(list);
            }
            // du coup le pointeur prev ne change pas
        }
    }
}





void gc_final_sweep(void) {
    if (neo_is_void(OBJECTS_LIST))
        return;


    // on parcourt maintenant tous les containers et listes
    // on fait une première passe dans laquelle on supprime de manière non récursive les objets présents
    for (NeObj ptr = OBJECTS_LIST ; !neo_is_void(ptr) ; ptr = get_next(ptr)) {

        // l'objet n'est pas marqué, on va donc devoir nous en débarrasser
        // on commence par supprimer les objets qu'il contient avec une profondeur de 1.
        // en effet :
        // - soit c'est un objet accessible depuis ADRESSES, auquel cas il faut juste décrémenter son compteur de références
        // - sinon, pas besoin d'être récursif car il est à un autre endroit dans la OBJECTS_LIST et on va le supprimer aussi
        // a l'issue de cette suppression, les seuls objets restants à supprimer sont les NeObjects*, data et les refc des
        // containers et listes contenues dans OBJECTS_LISTS
        neobject_partial_destroy(ptr);
    }

    // deuxième tour, où on termine de supprimer totalement les objets à supprimer

    NeObj next;
    for (NeObj ptr = OBJECTS_LIST ; !neo_is_void(ptr) ; ptr = next) {

        // l'objet n'est pas marqué, on va donc devoir nous en débarrasser
        // on fait pointer prev sur next(ptr) au lieu de ptr
        next = get_next(ptr);

        if (NEO_TYPE(ptr) == TYPE_CONTAINER) {
            Container* c = neo_to_container(ptr);
            free(c->data->tab);
            free(c->data);
            free(c);
        }
        else if (NEO_TYPE(ptr) == TYPE_LIST) {
            NeList* list = neo_to_list(ptr);
            free(list->tab);
            free(list);
        }
        // du coup le pointeur prev ne change pas
    }
}