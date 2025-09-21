#include <string.h>
#include "headers/graphics.h"
#include "headers/constants.h"
#include "headers/neon.h"
#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/dynarrays.h"
#include "headers/runtime.h"

static struct ContainersAssoc graphic_containers;

void initGraphics(void) {
    int nb_types = 5;

    struct ContainerType container_types[] = {
        (struct ContainerType) {
            .name = "Circle",
            .fields = (char*[]) {"x", "y", "radius", "color"},
            .nb_fields = 4,
            .container_assoc_index = &graphic_containers.Circle
        },
        (struct ContainerType) {
            .name = "FilledCircle",
            .fields = (char*[]) {"x", "y", "radius", "color"},
            .nb_fields = 4,
            .container_assoc_index = &graphic_containers.FilledCircle
        },
        (struct ContainerType) {
            .name = "Rect",
            .fields = (char*[]) {"x", "y", "width", "height", "color"},
            .nb_fields = 5,
            .container_assoc_index = &graphic_containers.Rect
        },
        (struct ContainerType) {
            .name = "FilledRect",
            .fields = (char*[]) {"x", "y", "width", "height", "color"},
            .nb_fields = 5,
            .container_assoc_index = &graphic_containers.FilledRect
        },
        (struct ContainerType) {
            .name = "Line",
            .fields = (char*[]) {"x1", "y1", "x2", "y2", "color"},
            .nb_fields = 5,
            .container_assoc_index = &graphic_containers.Line
        }
    };

    for (int i = 0 ; i < nb_types ; i++) {
        *container_types[i].container_assoc_index = global_env->CONTAINERS->len; // on set le type de container
        strlist_append(global_env->CONTAINERS, strdup(container_types[i].name));
        NeList* fields = nelist_create(container_types[i].nb_fields);

        // crée la NeList des attributs
        for (int j = 0 ; j < container_types[i].nb_fields ; j++) {
            fields->tab[j] = neo_str_create(strdup(container_types[i].fields[j]));
        }

        // ajout de la liste d'attributs au tableau d'attributs global
        nelist_append(global_env->ATTRIBUTES, gc_extern_neo_list_convert(fields));
    }


    // functions
    int nb_functions = 1;

    char* names[] = {
        "draw"
    };

    Function functions[] = {
        (Function) {
            .ptr = draw,
            .help = "Draws anything drawable",
            .nbArgs = -1,
            .typeArgs = (int[]) {TYPE_UNSPECIFIED},
            .typeRetour = TYPE_NONE
        }
    };

    for (int i = 0 ; i < nb_functions ; i++)
    {
        Function f = functions[i];
        NeObj func = neo_fun_create(f.ptr, f.help, f.nbArgs, f.typeArgs, f.typeRetour);
        strlist_append(global_env->NOMS, strdup(names[i]));
        nelist_append(global_env->ADRESSES, func);
    }
    return;

}



// this function draws an object
NeObj draw(NeList* args) {
    draw_nelist(args);
    return neo_none_create();
}

void draw_obj(NeObj obj) {
    // on dessine directement l'objet
    if (NEO_TYPE(obj) == TYPE_CONTAINER) {
        Container* c = neo_to_container(obj);

        if (c->type == graphic_containers.Circle) { // draws a circle
            printString("circle\n");
        }
        else if (c->type == graphic_containers.FilledCircle) { // draws a filled circle
            printString("filled circle\n");
        }
        else if (c->type == graphic_containers.Rect) { // draws a rectangle
            printString("rectangle\n");
        }
        else if (c->type == graphic_containers.FilledRect) { // draws a filled rectangle
            printString("filled rectangle\n");
        }
        else if (c->type == graphic_containers.Line) { // draws a line
            printString("line\n");
        }
        else {
            draw_nelist(c->data);
        }
    }
    else if (NEO_TYPE(obj) == TYPE_USERFUNC) {
        UserFunc* fun = neo_to_userfunc(obj);

        if (fun->nbArgs != 0) {
            global_env->CODE_ERROR = 14;
            return;
        }

        neobject_destroy(callUserFunc(fun, NULL, NEO_VOID));
    }
    else if (NEO_TYPE(obj) == TYPE_BUILTINFUNC) {
        Function* fun = neo_to_function(obj);

        if (fun->nbArgs != 0) {
            global_env->CODE_ERROR = 14;
            return;
        }

        NeObj (*ptr) (NeList*) = fun->ptr;

        neobject_destroy(ptr(NULL));
    }
    else if (NEO_TYPE(obj) == TYPE_LIST) {
        draw_nelist(neo_to_list(obj));
    }
}



void draw_nelist(NeList* list) {
    for (int i = 0 ; i < list->len ; i++) {
        draw_obj(list->tab[i]);
    }
}