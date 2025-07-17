#include "headers.c"

extern ptrlist* OBJECTS_LIST;
extern NeList* ADRESSES;
extern strlist* NOMS;

/*
Ajoute un objet profond (liste/container) à la banque d'objets du garbage collector
*/
void gc_add_deep_object(NeObject* neo) {
    ptrlist_append(OBJECTS_LIST, neo_copy(neo));
}

void gc_init_objects_list(void) {
    OBJECTS_LIST = ptrlist_create();
}


// comme les objets qu'on a ajoutés ont subi un neo_copy, il faut les supprimer proprement
void gc_free_objects_list(void) {
    if (!ptrlist_isEmpty(OBJECTS_LIST)) {
        ptrlist* temp;
        for (ptrlist* ptr = OBJECTS_LIST ; ptr != NULL ;) {
            neobject_destroy(ptr->tete, true);
            
            // suppression du maillon
            temp = ptr;
            ptr = ptr->queue;
            err_free(temp);
        }
    }
    else
        err_free(OBJECTS_LIST);
}


/*
Détruit les objets simples et les listes/containers encore accessibles qui sont dans la liste à une profondeur 1
*/
void nelist_partial_destroy(NeList* list) {
    for (int i=0 ; i < list->len ; i++) {
        if (list->tab[i] != NULL) {
            // si l'objet est soit une feuille, soit une liste ou un container encore accessible depuis adresses
            // les objets ne vérifiant pas cette condition sont les objets qui sont à supprimer dans OBJECTS_LIST
            if (list->tab[i]->type != TYPE_CONTAINER && list->tab[i]->type != TYPE_LIST || ismarked(list->tab[i])) {
                neobject_destroy(list->tab[i], true);
            }
            else {
                err_free(list->tab[i]);
            }
        }
    }
}

/*
Cette fonction supprime les objets simples qui se situent dans neo si neo est un container ou une liste
avec un niveau de profondeur de 1.
*/
void neobject_partial_destroy(NeObject* neo)
{
    if (neo->type == TYPE_LIST) {
        nelist_partial_destroy(neo->data.ptr);
    }
    else if (neo->type == TYPE_CONTAINER) {
        Container* c = neo->data.ptr;
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




void gc_mark(NeObject* obj) {
    if (ismarked(obj))
        return;

    if (obj->type == TYPE_CONTAINER) {
        mark(obj);
        Container* c = obj->data.ptr;
        gc_nelist_mark(c->data);
    }
    else if (obj->type == TYPE_LIST) {
        mark(obj);
        gc_nelist_mark(obj->data.ptr);
    }
}



void gc_mark_and_sweep(void) {

    if (ptrlist_isEmpty(OBJECTS_LIST))
        return;

    // on marque tous les objets accessibles depuis les variables
    gc_nelist_mark(ADRESSES);

    // on parcourt maintenant tous les containers et listes
    // on fait une première passe dans laquelle on supprime de manière non récursive les objets présents
    for (ptrlist* ptr = OBJECTS_LIST ; ptr != NULL ; ptr = ptr->queue) {
        NeObject* obj = ptr->tete;

        if (!ismarked(obj)) { // l'objet est marqué, on doit donc le garder et on le démarque
            // l'objet n'est pas marqué, on va donc devoir nous en débarrasser
            // on commence par supprimer les objets qu'il contient avec une profondeur de 1.
            // en effet :
            // - soit c'est un objet accessible depuis ADRESSES, auquel cas il faut juste décrémenter son compteur de références
            // - sinon, pas besoin d'être récursif car il est à un autre endroit dans la OBJECTS_LIST et on va le supprimer aussi
            // a l'issue de cette suppression, les seuls objets restants à supprimer sont les NeObjects*, data et les refc des
            // containers et listes contenues dans OBJECTS_LISTS
            neobject_partial_destroy(obj);
        }
    }

    // deuxième tour, où on termine de supprimer totalement les objets à supprimer

    ptrlist* prev = NULL;
    for (ptrlist* ptr = OBJECTS_LIST ; ptr != NULL && !ptrlist_isEmpty(OBJECTS_LIST) ;) {
        NeObject* obj = ptr->tete;

        if (ismarked(obj)) { // l'objet est marqué, on doit donc le garder et on le démarque
            unmark(obj);
            prev = ptr;
            ptr = ptr->queue; // on passe au suivant
        }
        else { // l'objet n'est pas marqué, on va donc devoir nous en débarrasser
            if (obj->type == TYPE_CONTAINER) {
                Container* c = obj->data.ptr;
                err_free(c->data->tab);
                err_free(c->data);
                err_free(c);
                err_free(obj->data.refc);
            }
            else if (obj->type == TYPE_LIST) {
                NeList* list = obj->data.ptr;
                err_free(list->tab);
                err_free(list);
                err_free(obj->data.refc);
            }
            err_free(obj);
            // et on supprime l'objet de la liste d'objets puisqu'il n'existe plus
            ptrlist_direct_remove(OBJECTS_LIST, ptr, prev);
            // du coup le pointeur prev ne change pas
            ptr = (prev == NULL) ? OBJECTS_LIST : prev->queue; // et le suivant est prev->queue
        }
    }
}





void gc_final_sweep(void) {
    // il s'agit d'un mark and sweep où l'on ne fait pas de phase de marquage et on suppose qu'aucun objet n'est marqué

    if (ptrlist_isEmpty(OBJECTS_LIST))
        return;

    // on parcourt maintenant tous les containers et listes
    // on fait une première passe dans laquelle on supprime de manière non récursive les objets présents
    for (ptrlist* ptr = OBJECTS_LIST ; ptr != NULL ; ptr = ptr->queue) {
        NeObject* obj = ptr->tete;

        // l'objet n'est pas marqué, on va donc devoir nous en débarrasser
        // on commence par supprimer les objets qu'il contient avec une profondeur de 1.
        // en effet :
        // - soit c'est un objet accessible depuis ADRESSES, auquel cas il faut juste décrémenter son compteur de références
        // - sinon, pas besoin d'être récursif car il est à un autre endroit dans la OBJECTS_LIST et on va le supprimer aussi
        // a l'issue de cette suppression, les seuls objets restants à supprimer sont les NeObjects*, data et les refc des
        // containers et listes contenues dans OBJECTS_LISTS
        neobject_partial_destroy(obj);
    }

    // deuxième tour, où on termine de supprimer totalement les objets à supprimer
    ptrlist* prev = NULL;
    for (ptrlist* ptr = OBJECTS_LIST ; ptr != NULL ; ptr = ptr->queue) {
        NeObject* obj = ptr->tete;
        // l'objet n'est pas marqué, on va donc devoir nous en débarrasser
        if (obj->type == TYPE_CONTAINER) {
            Container* c = obj->data.ptr;
            err_free(c->data->tab);
            err_free(c->data);
            err_free(c);
            err_free(obj->data.refc);
        }
        else if (obj->type == TYPE_LIST) {
            NeList* list = obj->data.ptr;
            err_free(list->tab);
            err_free(list);
            err_free(obj->data.refc);
        }
        err_free(obj);
        // et on supprime l'objet de la liste d'objets puisqu'il n'existe plus
    }
    // on a tout supprimé sans exception, donc on peut libérer tous les chainons de la liste d'un coup
    ptrlist_destroy(OBJECTS_LIST, false, true);
}