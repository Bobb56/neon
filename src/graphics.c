// cette bibliothèque est spécifique à ez80

#include <string.h>
#include "headers/graphics.h"
#include "headers/constants.h"
#include "headers/neon.h"
#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/dynarrays.h"
#include "headers/runtime.h"
#include "headers/errors.h"
#include "headers/lowlevel.h"

#include <graphx.h>
#include <keypadc.h>
#include <time.h>

#define FLOODFILL_STACK_SIZE        3328


// cette variable globale statique permet d'associer à chaque type de figure son indice
// dans la liste CONTAINERS au moment où les types sont chargés en mémoire
static struct ContainersAssoc graphic_containers;

void initGraphics(void) {
    int nb_types = 9;

    struct ContainerType container_types[] = {
        (struct ContainerType) {
            .name = "Point",
            .fields = (char*[]) {"x", "y"},
            .nb_fields = 2,
            .container_assoc_index = &graphic_containers.Point
        },
        (struct ContainerType) {
            .name = "Circle",
            .fields = (char*[]) {"x", "y", "radius", "color", "filled"},
            .nb_fields = 5,
            .container_assoc_index = &graphic_containers.Circle
        },
        (struct ContainerType) {
            .name = "Rect",
            .fields = (char*[]) {"x", "y", "width", "height", "color", "filled"},
            .nb_fields = 6,
            .container_assoc_index = &graphic_containers.Rect
        },
        (struct ContainerType) {
            .name = "Line",
            .fields = (char*[]) {"x0", "y0", "x1", "y1", "color"},
            .nb_fields = 5,
            .container_assoc_index = &graphic_containers.Line
        },
        (struct ContainerType) {
            .name = "Text",
            .fields = (char*[]) {"text", "x", "y", "fgcolor", "bgcolor", "size"},
            .nb_fields = 6,
            .container_assoc_index = &graphic_containers.Text
        },
        (struct ContainerType) {
            .name = "Triangle",
            .fields = (char*[]) {"x0", "y0", "x1", "y1", "x2", "y2", "color"},
            .nb_fields = 7,
            .container_assoc_index = &graphic_containers.Triangle
        },
        (struct ContainerType) {
            .name = "Polygon",
            .fields = (char*[]) {"points", "color"},
            .nb_fields = 2,
            .container_assoc_index = &graphic_containers.Polygon
        },
        (struct ContainerType) {
            .name = "Ellipse",
            .fields = (char*[]) {"x", "y", "a", "b", "color", "filled"},
            .nb_fields = 6,
            .container_assoc_index = &graphic_containers.Ellipse
        },
        (struct ContainerType) {
            .name = "FloodFill",
            .fields = (char*[]) {"x", "y", "color"},
            .nb_fields = 3,
            .container_assoc_index = &graphic_containers.FloodFill
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
    int nb_functions = 7;

    char* names[] = {
        "getKey",
        "draw",
        "setPixel",
        "getPixel",
        "setTextTransparentColor",
        "getTextWidth",
        "rgb"
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
            .typeArgs = (int[]) {TYPE_UNSPECIFIED, TYPE_UNSPECIFIED, TYPE_INTEGER},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = getPixel,
            .help = "Returns the color of the specified pixel, None if the pixel is not within the screen boundaries",
            .nbArgs = 2,
            .typeArgs = (int[]) {TYPE_UNSPECIFIED, TYPE_UNSPECIFIED},
            .typeRetour = TYPE_INTEGER
        },
        (Function) {
            .ptr = setTextTransparentColor,
            .help = "Sets the color that will be used for transparent while drawing text",
            .nbArgs = 1,
            .typeArgs = (int[]) {TYPE_INTEGER},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = getTextWidth,
            .help = "Returns the pixel width of a given Text object",
            .nbArgs = 1,
            .typeArgs = (int[]) {TYPE_CONTAINER},
            .typeRetour = TYPE_INTEGER
        },
        (Function) {
            .ptr = rgb,
            .help = "Returns a number in range 0-255 corresponding approximatively to the rgb color",
            .nbArgs = 3,
            .typeArgs = (int[]) {TYPE_UNSPECIFIED, TYPE_UNSPECIFIED, TYPE_UNSPECIFIED},
            .typeRetour = TYPE_INTEGER
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

bool in_screen(intptr_t x, intptr_t y) {
    return x >= 0 && x < 320 && y >= 0 && y < 240;
}


NeObj rgb(NeList* args) {
    uint8_t r = neo_to_integer(ARG(0));
    uint8_t g = neo_to_integer(ARG(1));
    uint8_t b = neo_to_integer(ARG(2));
    uint8_t palette_index = (r/36) << 5 | (g/36) << 2 | (b/85);
    return neo_integer_create(palette_index);
}




int neon_getKey(void) {
    static uint8_t last_key;
    static clock_t local_clock;

    uint8_t only_key = 0;

    kb_Scan();
    for (uint8_t key = 1, group = 7; group; --group) {
        for (uint8_t mask = 1; mask; mask <<= 1, ++key) {
            if (kb_Data[group] & mask) {
                if (only_key) {
                    last_key = 0;
                    return 0;
                }
                else {
                    only_key = key;
                }
            }
        }
    }

    // processing of the key code

    if (only_key == last_key) {
        if (clock() - local_clock > 10000) {
            return only_key;
        }
        else {
            return 0;
        }
    }
    else {

        // resets the clock since it's a new key
        local_clock = clock();

        last_key = only_key;
        return only_key;
    }
}

NeObj getKey(NeList* args) {
    return neo_integer_create(neon_getKey());
}


void custom_floodfill(intptr_t tempx, intptr_t tempy, intptr_t tempcolor) {
    // on stocke les arguments en global pour les retrouver après changement de pile
    static intptr_t x, y, color;
    static void *buffer, *old_stack;
    x = tempx;
    y = tempy;
    color = tempcolor;

    // on alloue une pile toute neuve et suffisamment grande
    buffer = malloc(FLOODFILL_STACK_SIZE);
    old_stack = floodfill_switch_stack(buffer + FLOODFILL_STACK_SIZE - 1);
    gfx_FloodFill(x, y, color);
    floodfill_switch_stack(old_stack);
    free(buffer);
    return;
}





NeObj setPixel(NeList* args) {
    intptr_t x = neo_to_integer(ARG(0));
    intptr_t y = neo_to_integer(ARG(1));
    
    // vérification des coordonnées
    if (!in_screen(x, y))
        return neo_none_create();
    
    gfx_SetColor(neo_to_integer(ARG(2)));
    gfx_SetPixel(neo_to_integer(ARG(0)), neo_to_integer(ARG(1)));
    return neo_none_create();
}

NeObj getPixel(NeList* args) {
    intptr_t x = neo_to_integer(ARG(0));
    intptr_t y = neo_to_integer(ARG(1));

    // vérification des coordonnées
    if (!in_screen(x, y))
        return neo_none_create();
    else
        return neo_integer_create(gfx_GetPixel(x, y));
}

NeObj setTextTransparentColor(NeList* args) {
    intptr_t color = neo_to_integer(ARG(0));
    gfx_SetTextTransparentColor(color);
    return neo_none_create();
}


NeObj getTextWidth(NeList* args) {
    Container* c = neo_to_container(ARG(0));

    if (c->type != graphic_containers.Text) {
        global_env->CODE_ERROR = 14;
        return neo_none_create();
    }

    if (NEO_TYPE(c->data->tab[0]) != TYPE_STRING ||
        NEO_TYPE(c->data->tab[1]) != TYPE_INTEGER ||
        NEO_TYPE(c->data->tab[2]) != TYPE_INTEGER ||
        NEO_TYPE(c->data->tab[3]) != TYPE_INTEGER ||
        NEO_TYPE(c->data->tab[4]) != TYPE_INTEGER ||
        NEO_TYPE(c->data->tab[5]) != TYPE_INTEGER) {
            global_env->CODE_ERROR = 117;
            return neo_none_create();
    }
    char* text = neo_to_string(c->data->tab[0]);
    intptr_t size = neo_to_integer(c->data->tab[5]) - 1;

    intptr_t x_width = size/2;
    intptr_t y_width = size - x_width;

    gfx_SetTextScale(x_width+1, y_width+1);

    return neo_integer_create(gfx_GetStringWidth(text));
}


// this function draws an object
NeObj draw(NeList* args) {
    gfx_BlitScreen(); // copy the screen to the buffer
    gfx_SetDrawBuffer();

    draw_nelist(args); // draws to the buffer

    gfx_BlitBuffer();
    gfx_SetDrawScreen();

    return neo_none_create();
}

void draw_obj(NeObj obj) {
    // on dessine directement l'objet
    if (NEO_TYPE(obj) == TYPE_CONTAINER) {
        Container* c = neo_to_container(obj);
        NeList* args = c->data;

        if (c->type == graphic_containers.Circle) { // draws a circle
            if (!is_number(ARG(0)) ||
                !is_number(ARG(1)) ||
                !is_number(ARG(2)) ||
                NEO_TYPE(ARG(3)) != TYPE_INTEGER ||
                NEO_TYPE(ARG(4)) != TYPE_BOOL) {
                    global_env->CODE_ERROR = 117;
                    return;
            }
            intptr_t x = neo_to_integer(ARG(0));
            intptr_t y = neo_to_integer(ARG(1));
            intptr_t radius = neo_to_integer(ARG(2));
            intptr_t color = neo_to_integer(ARG(3));
            bool filled = neo_to_bool(ARG(4));

            gfx_SetColor(color);
            if (filled)
                gfx_FillCircle(x, y, radius);
            else
                gfx_Circle(x, y, radius);
        }
        else if (c->type == graphic_containers.Rect) { // draws a rectangle
            if (!is_number(ARG(0)) ||
                !is_number(ARG(1)) ||
                !is_number(ARG(2)) ||
                !is_number(ARG(3)) ||
                NEO_TYPE(ARG(4)) != TYPE_INTEGER ||
                NEO_TYPE(ARG(5)) != TYPE_BOOL) {
                    global_env->CODE_ERROR = 117;
                    return;
            }
            intptr_t x = neo_to_integer(ARG(0));
            intptr_t y = neo_to_integer(ARG(1));
            intptr_t width = neo_to_integer(ARG(2));
            intptr_t height = neo_to_integer(ARG(3));
            intptr_t color = neo_to_integer(ARG(4));
            bool filled = neo_to_bool(ARG(5));
            
            gfx_SetColor(color);
            if (filled)
                gfx_FillRectangle(x, y, width, height);
            else
                gfx_Rectangle(x, y, width, height);
        }
        else if (c->type == graphic_containers.Line) { // draws a line
            if (!is_number(ARG(0)) ||
                !is_number(ARG(1)) ||
                !is_number(ARG(2)) ||
                !is_number(ARG(3)) ||
                NEO_TYPE(ARG(4)) != TYPE_INTEGER) {
                    global_env->CODE_ERROR = 117;
                    return;
            }
            intptr_t x0 = neo_to_integer(ARG(0));
            intptr_t y0 = neo_to_integer(ARG(1));
            intptr_t x1 = neo_to_integer(ARG(2));
            intptr_t y1 = neo_to_integer(ARG(3));
            intptr_t color = neo_to_integer(ARG(4));
            
            gfx_SetColor(color);
            gfx_Line(x0, y0, x1, y1);
        }
        else if (c->type == graphic_containers.Text) { // draws text
            if (NEO_TYPE(ARG(0)) != TYPE_STRING ||
                !is_number(ARG(1)) ||
                !is_number(ARG(2)) ||
                NEO_TYPE(ARG(3)) != TYPE_INTEGER ||
                NEO_TYPE(ARG(4)) != TYPE_INTEGER ||
                NEO_TYPE(ARG(5)) != TYPE_INTEGER) {
                    global_env->CODE_ERROR = 117;
                    return;
            }
            char* text = neo_to_string(ARG(0));
            intptr_t x = neo_to_integer(ARG(1));
            intptr_t y = neo_to_integer(ARG(2));
            uint8_t fgcolor = (uint8_t)neo_to_integer(ARG(3));
            uint8_t bgcolor = (uint8_t)neo_to_integer(ARG(4));
            uint8_t size = (uint8_t)neo_to_integer(ARG(5)) - 1;

            uint8_t x_width = size/2;
            uint8_t y_width = size - x_width;
            
            gfx_SetTextBGColor(bgcolor);
            gfx_SetTextFGColor(fgcolor);
            gfx_SetTextScale(x_width+1, y_width+1);
            gfx_PrintStringXY((const char*)text, x, y);
        }
        else if (c->type == graphic_containers.Triangle) { // draws a triangle
            if (!is_number(ARG(0)) ||
                !is_number(ARG(1)) ||
                !is_number(ARG(2)) ||
                !is_number(ARG(3)) ||
                !is_number(ARG(4)) ||
                !is_number(ARG(5)) ||
                NEO_TYPE(ARG(6)) != TYPE_INTEGER) {
                    global_env->CODE_ERROR = 117;
                    return;
            }
            intptr_t x0 = neo_to_integer(ARG(0));
            intptr_t y0 = neo_to_integer(ARG(1));
            intptr_t x1 = neo_to_integer(ARG(2));
            intptr_t y1 = neo_to_integer(ARG(3));
            intptr_t x2 = neo_to_integer(ARG(4));
            intptr_t y2 = neo_to_integer(ARG(5));
            intptr_t color = neo_to_integer(ARG(6));
            
            gfx_SetColor(color);
            gfx_FillTriangle(x0, y0, x1, y1, x2, y2);
        }
        else if (c->type == graphic_containers.Polygon) { // draws a polygon
            if (NEO_TYPE(ARG(0)) != TYPE_LIST ||
                NEO_TYPE(ARG(1)) != TYPE_INTEGER) {
                    global_env->CODE_ERROR = 117;
                    return;
            }
            NeList* points = neo_to_list(ARG(0));
            intptr_t color = neo_to_integer(ARG(1));
            
            int* points_arr = neon_malloc(sizeof(int) * points->len * 2);

            for (int i = 0 ; i < points->len ; i++) {
                if (NEO_TYPE(points->tab[i]) != TYPE_CONTAINER) {
                    global_env->CODE_ERROR = 117;
                    return;
                }
                Container* point = neo_to_container(points->tab[i]);

                if (point->type != graphic_containers.Point ||
                    !is_number(point->data->tab[0]) ||
                    !is_number(point->data->tab[1])) {
                    global_env->CODE_ERROR = 117;
                    return;
                }

                points_arr[2*i] = neo_to_integer(point->data->tab[0]);
                points_arr[2*i+1] = neo_to_integer(point->data->tab[1]);
            }

            gfx_SetColor(color);
            gfx_Polygon(points_arr, points->len);
            neon_free(points_arr);
        }
        else if (c->type == graphic_containers.Ellipse) { // draws an ellipse
            if (!is_number(ARG(0)) ||
                !is_number(ARG(1)) ||
                !is_number(ARG(2)) ||
                !is_number(ARG(3)) ||
                NEO_TYPE(ARG(4)) != TYPE_INTEGER ||
                NEO_TYPE(ARG(5)) != TYPE_BOOL) {
                    global_env->CODE_ERROR = 117;
                    return;
            }
            intptr_t x = neo_to_integer(ARG(0));
            intptr_t y = neo_to_integer(ARG(1));
            intptr_t a = neo_to_integer(ARG(2));
            intptr_t b = neo_to_integer(ARG(3));
            intptr_t color = neo_to_integer(ARG(4));
            bool filled = neo_to_bool(ARG(5));
            
            gfx_SetColor(color);
            if (filled)
                gfx_FillEllipse(x, y, a, b);
            else
                gfx_Ellipse(x, y, a, b);
        }
        if (c->type == graphic_containers.FloodFill) { // fills an area
            if (!is_number(ARG(0)) ||
                !is_number(ARG(1)) ||
                NEO_TYPE(ARG(2)) != TYPE_INTEGER) {
                    global_env->CODE_ERROR = 117;
                    return;
            }
            intptr_t x = neo_to_integer(ARG(0));
            intptr_t y = neo_to_integer(ARG(1));
            intptr_t color = neo_to_integer(ARG(2));

            if (in_screen(x, y))
                custom_floodfill(x, y, color);
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


uint16_t rgb8_to_1555(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t new_r = 4.4 * (double)r;  // valeur sur 5 bits
    uint8_t new_g = 4.4 * (double)g;  // valeur sur 5 bits
    uint8_t new_b = 10.3 * (double)b; // valeur sur 5 bits
    uint16_t color = new_r << 10 | new_g << 5 | new_b;
    return color;
}

void set_neon_palette(void) {
    for (uint8_t r = 0 ; r < 8 ; r++) {
        for (uint8_t g = 0 ; g < 8 ; g++) {
            for (uint8_t b = 0 ; b < 4 ; b++) {
                gfx_palette[r << 5 | g << 2 | b] = rgb8_to_1555(r, g, b);
            }
        }
    }
}