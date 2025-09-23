// cette bibliothèque est spécifique à ez80

#include <string.h>
#include "headers/graphics.h"
#include "headers/constants.h"
#include "headers/neon.h"
#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/dynarrays.h"
#include "headers/runtime.h"


#include <graphx.h>
#include <ti/getcsc.h>


/*
TODO :
- setPixel(x, y, color)
- getPixel(x, y)
- really implement graphic functions
- Objet FloodFill, Triangle, FillTriangle, ellipse, polygon
- get_string_width
- la fonction pour afficher du texte fait grossir à tour de rôle le scaling factor pour width et height

Une couleur à None correspond à du transparent


*/


static struct ContainersAssoc graphic_containers;

void initGraphics(void) {
    int nb_types = 6;

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
        },
        (struct ContainerType) {
            .name = "Text",
            .fields = (char*[]) {"text", "x", "y", "fgcolor", "bgcolor", "size"},
            .nb_fields = 6,
            .container_assoc_index = &graphic_containers.Text
        }
    };

    for (int i = 0 ; i < nb_types ; i++) {
        int index = 0; // l'indice du container dans CONTAINERS

        // on regarde si ce type de container existe déjà
        if (strlist_inList(global_env->CONTAINERS, container_types[i].name)) {
            index = strlist_index(global_env->CONTAINERS, container_types[i].name);
        }
        else {
            index = global_env->CONTAINERS->len;
            strlist_append(global_env->CONTAINERS, strdup(container_types[i].name));
            nelist_append(global_env->ATTRIBUTES, NEO_VOID);
        }

        *container_types[i].container_assoc_index = index; // on associe le type de container à son indice dans CONTAINERS

        // crée la NeList des attributs
        NeList* fields = nelist_create(container_types[i].nb_fields);
        for (int j = 0 ; j < container_types[i].nb_fields ; j++) {
            fields->tab[j] = neo_str_create(strdup(container_types[i].fields[j]));
        }

        // et on met la liste d'attributs dans le tableau d'attributs global
        if (!neo_is_void(global_env->ATTRIBUTES->tab[index]) && !nelist_equal(neo_to_list(global_env->ATTRIBUTES->tab[index]), fields)) {
            // si on met pas ce garde fou, on peut overrider le type de container en silence
            nelist_destroy(fields);
            global_env->CODE_ERROR = 116;
            return;
        }

        neobject_destroy(global_env->ATTRIBUTES->tab[index]);
        global_env->ATTRIBUTES->tab[index] = gc_extern_neo_list_convert(fields);
    }


    // functions
    int nb_functions = 3;

    char* names[] = {
        "getKey",
        "draw",
        "setPixel"
    };

    Function functions[] = {
        (Function) {
            .ptr = getKey,
            .help = "Returns the code of the last key pressed or 0 if no key was pressed",
            .nbArgs = 0,
            .typeArgs = NULL,
            .typeRetour = TYPE_INTEGER
        },
        (Function) {
            .ptr = draw,
            .help = "Draws anything drawable",
            .nbArgs = -1,
            .typeArgs = (int[]) {TYPE_UNSPECIFIED},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = setPixel,
            .help = "Sets the indicated pixel to the specified color",
            .nbArgs = 3,
            .typeArgs = (int[]) {TYPE_INTEGER, TYPE_INTEGER, TYPE_INTEGER},
            .typeRetour = TYPE_NONE
        }
    };

    for (int i = 0 ; i < nb_functions ; i++)
    {
        Function f = functions[i];
        NeObj func = neo_fun_create(f.ptr, f.help, f.nbArgs, f.typeArgs, f.typeRetour);

        Var var = get_var(names[i]);
        replace_var(var, func);
    }
    return;

}


NeObj getKey(NeList* args) {
    uint8_t key = os_GetCSC();
    return neo_integer_create(key);
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
        else if (c->type == graphic_containers.Text) { // draws text
            printString("text\n");
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


NeObj setPixel(NeList* args) {
    gfx_SetColor(neo_to_integer(ARG(2)));
    gfx_SetPixel(neo_to_integer(ARG(0)), neo_to_integer(ARG(1)));
    return neo_none_create();
}