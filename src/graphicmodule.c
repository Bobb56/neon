#define NEON_SOURCE_ID 7

// cette bibliothèque est spécifique à ez80

#include <string.h>
#include <time.h>
#include "headers/graphicmodule.h"
#include "headers/constants.h"
#include "headers/neon.h"
#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/dynarrays.h"
#include "headers/runtime.h"
#include "headers/errors.h"
#include "headers/lowlevel.h"
#include "extern/ide/headers/keys.h"

#ifdef TI_EZ80
#include <graphx.h>
#include <keypadc.h>
#endif

#define FLOODFILL_STACK_SIZE        3328
#define PALETTE_INDEX_RGB(r,g,b)    ((r/36) << 5 | (g/36) << 2 | (b/85))
#define TEXT_Y(i)                   (28 + 14*i)




bool in_screen(intptr_t x, intptr_t y) {
    return x >= 0 && x < 320 && y >= 0 && y < 240;
}


NeObj rgb(NeList* args) {
    uint8_t r = neo_to_integer(ARG(0));
    uint8_t g = neo_to_integer(ARG(1));
    uint8_t b = neo_to_integer(ARG(2));
    return neo_integer_create(PALETTE_INDEX_RGB(r, g, b));
}



NeObj getKey(NeList* args) {
    return neo_integer_create(ngetchx_backend());
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
    if (buffer == NULL) {
        neon_fail(12, NO_ARGS);
        return;
    }

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
    global_env->text_transparent_color = color;
    return neo_none_create();
}


NeObj getTextWidth(NeList* args) {
    Container* c = neo_to_container(ARG(0));

    if (c->type != global_env->graphic_containers.Text) {
        neon_fail(38, neo_new_const_create("getTextWidth"), neo_new_const_create("getTextWidth"));
        return neo_none_create();
    }

    if (NEO_TYPE(c->data.tab[0]) != TYPE_STRING ||
        NEO_TYPE(c->data.tab[1]) != TYPE_INTEGER ||
        NEO_TYPE(c->data.tab[2]) != TYPE_INTEGER ||
        NEO_TYPE(c->data.tab[3]) != TYPE_INTEGER ||
        NEO_TYPE(c->data.tab[4]) != TYPE_INTEGER ||
        NEO_TYPE(c->data.tab[5]) != TYPE_INTEGER) {
            neon_fail(117, neo_new_str_create("Text"), neo_new_str_create("Text"));
            return neo_none_create();
    }
    char* text = neo_to_string(c->data.tab[0]);
    intptr_t size = neo_to_integer(c->data.tab[5]) - 1;

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
        NeList* args = &c->data;

        if (c->type == global_env->graphic_containers.Circle) { // draws a circle
            if (!is_number(ARG(0)) ||
                !is_number(ARG(1)) ||
                !is_number(ARG(2)) ||
                NEO_TYPE(ARG(3)) != TYPE_INTEGER ||
                NEO_TYPE(ARG(4)) != TYPE_BOOL) {
                    neon_fail(117, neo_new_str_create("Circle"), neo_new_str_create("Circle"));
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
        else if (c->type == global_env->graphic_containers.Rect) { // draws a rectangle
            if (!is_number(ARG(0)) ||
                !is_number(ARG(1)) ||
                !is_number(ARG(2)) ||
                !is_number(ARG(3)) ||
                NEO_TYPE(ARG(4)) != TYPE_INTEGER ||
                NEO_TYPE(ARG(5)) != TYPE_BOOL) {
                    neon_fail(117, neo_new_str_create("Rect"), neo_new_str_create("Rect"));
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
        else if (c->type == global_env->graphic_containers.Line) { // draws a line
            if (!is_number(ARG(0)) ||
                !is_number(ARG(1)) ||
                !is_number(ARG(2)) ||
                !is_number(ARG(3)) ||
                NEO_TYPE(ARG(4)) != TYPE_INTEGER) {
                    neon_fail(117, neo_new_str_create("Line"), neo_new_str_create("Line"));
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
        else if (c->type == global_env->graphic_containers.Text) { // draws text
            if (NEO_TYPE(ARG(0)) != TYPE_STRING ||
                !is_number(ARG(1)) ||
                !is_number(ARG(2)) ||
                NEO_TYPE(ARG(3)) != TYPE_INTEGER ||
                NEO_TYPE(ARG(4)) != TYPE_INTEGER ||
                NEO_TYPE(ARG(5)) != TYPE_INTEGER) {
                    neon_fail(117, neo_new_str_create("Text"), neo_new_str_create("Text"));
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
        else if (c->type == global_env->graphic_containers.Triangle) { // draws a triangle
            if (!is_number(ARG(0)) ||
                !is_number(ARG(1)) ||
                !is_number(ARG(2)) ||
                !is_number(ARG(3)) ||
                !is_number(ARG(4)) ||
                !is_number(ARG(5)) ||
                NEO_TYPE(ARG(6)) != TYPE_INTEGER) {
                    neon_fail(117, neo_new_str_create("Triangle"), neo_new_str_create("Triangle"));
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
        else if (c->type == global_env->graphic_containers.Polygon) { // draws a polygon
            if (NEO_TYPE(ARG(0)) != TYPE_LIST ||
                NEO_TYPE(ARG(1)) != TYPE_INTEGER) {
                    neon_fail(117, neo_new_str_create("Polygon"), neo_new_str_create("Polygon"));
                    return;
            }
            NeList* points = neo_to_list(ARG(0));
            intptr_t color = neo_to_integer(ARG(1));
            
            int* points_arr = neon_malloc(sizeof(int) * points->len * 2);
            if (points_arr == NULL) {
                neon_fail(12, NO_ARGS);
                return;
            }

            for (int i = 0 ; i < points->len ; i++) {
                if (NEO_TYPE(points->tab[i]) != TYPE_CONTAINER) {
                    neon_fail(117, neo_new_str_create("Polygon"), neo_new_str_create("Polygon"));
                    return;
                }
                Container* point = neo_to_container(points->tab[i]);

                if (point->type != global_env->graphic_containers.Point ||
                    !is_number(point->data.tab[0]) ||
                    !is_number(point->data.tab[1])) {
                    neon_fail(117, neo_new_str_create("Polygon"), neo_new_str_create("Polygon"));
                    return;
                }

                points_arr[2*i] = neo_to_integer(point->data.tab[0]);
                points_arr[2*i+1] = neo_to_integer(point->data.tab[1]);
            }

            gfx_SetColor(color);
            gfx_Polygon(points_arr, points->len);
            neon_free(points_arr);
        }
        else if (c->type == global_env->graphic_containers.Ellipse) { // draws an ellipse
            if (!is_number(ARG(0)) ||
                !is_number(ARG(1)) ||
                !is_number(ARG(2)) ||
                !is_number(ARG(3)) ||
                NEO_TYPE(ARG(4)) != TYPE_INTEGER ||
                NEO_TYPE(ARG(5)) != TYPE_BOOL) {
                    neon_fail(117, neo_new_str_create("Ellipse"), neo_new_str_create("Ellipse"));
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
        if (c->type == global_env->graphic_containers.FloodFill) { // fills an area
            if (!is_number(ARG(0)) ||
                !is_number(ARG(1)) ||
                NEO_TYPE(ARG(2)) != TYPE_INTEGER) {
                    neon_fail(117, neo_new_str_create("FloodFill"), neo_new_str_create("FloodFill"));
                    return;
            }
            intptr_t x = neo_to_integer(ARG(0));
            intptr_t y = neo_to_integer(ARG(1));
            intptr_t color = neo_to_integer(ARG(2));

            if (in_screen(x, y))
                custom_floodfill(x, y, color);
        }
        else {
            draw_nelist(&c->data);
        }
    }
    else if (NEO_TYPE(obj) == TYPE_USERFUNC) {
        UserFunc* fun = neo_to_userfunc(obj);

        if (fun->nbArgs != 0) {
            neon_fail(14, NO_ARGS);
            return;
        }
        NeList* args = nelist_create(0);
        neobject_destroy(callNoArgsUserFunc(fun));
        nelist_destroy(args);
    }
    else if (NEO_TYPE(obj) == TYPE_BUILTINFUNC) {
        Function* fun = neo_to_function(obj);

        if (fun->nbArgs != 0) {
            const char* fname = get_function_name(fun->id, fun->module);
            neon_fail(38, neo_new_const_create(fname), neo_new_const_create(fname));
            return;
        }

        NeList* args = nelist_create(0);
        neobject_destroy(call_function(fun->module, fun->id, args));
        nelist_destroy(args);
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


/*
Affiche un menu pour laisser à l'utilisateur choisir un des éléments de la liste
Renvoie l'élément dans la liste ou None si l'utilisateur a quitté
*/
NeObj menu(NeList* args) {
    gfx_SetDrawBuffer();

    gfx_SetTextTransparentColor(1);
    gfx_SetTextScale(1, 1);

    char* title = neo_to_string(ARG(0));
    NeList* elements = neo_to_list(ARG(1));
    char* empty_text = neo_to_string(ARG(2));

    uint8_t highlight_color = PALETTE_INDEX_RGB(0, 100, 0);
    uint8_t anti_highlight_color = PALETTE_INDEX_RGB(255, 255, 255);
    uint8_t background_color = anti_highlight_color;
    uint8_t foreground_color = PALETTE_INDEX_RGB(0, 0, 0);

    // rectangle header
    gfx_SetColor(highlight_color);
    gfx_FillRectangle(0, 0, 320, 20);

    // text header
    gfx_SetTextBGColor(1);
    gfx_SetTextFGColor(anti_highlight_color);
    gfx_PrintStringXY(title, 10, 7);

    // backgroud
    gfx_SetColor(background_color);
    gfx_FillRectangle(0, 20, 320, 220);

    if (elements->len == 0) {
        gfx_SetTextBGColor(background_color);
        gfx_SetTextFGColor(foreground_color);
        gfx_PrintStringXY(empty_text, 90, 120);
        while (ngetchx_backend() != 15);
    }
    else {
        int start_disp_index = 0; // indice dans la liste d'éléments de l'élément affiché le plus haut
        int cursor_position = 0; // indice du curseur sur l'écran en teme de nombre d'items
        uint8_t vertical_limit = 15;

        bool refresh = true;
        uint8_t key = 0;
        while ((key = ngetchx_backend()) != 15) {
            if (key == 1 || key == 3) { // next item
                if (cursor_position + start_disp_index + 1 >= elements->len) {
                    cursor_position = 0;
                    start_disp_index = 0;
                }
                else if (cursor_position + 1 < vertical_limit) {
                    cursor_position++;
                }
                else {
                    start_disp_index++;
                }
                refresh = true;
            }
            else if (key == 2 || key == 4) { // previous item
                if (cursor_position > 0) {
                    cursor_position--;
                }
                else if (start_disp_index != 0) {
                    start_disp_index--;
                }
                else {
                    if (elements->len < vertical_limit) {
                        cursor_position = elements->len - 1;
                        start_disp_index = 0;
                    }
                    else {
                        cursor_position = vertical_limit - 1;
                        start_disp_index = elements->len - vertical_limit;
                    }
                }
                refresh = true;
            }
            else if (key == 9) {
                gfx_SetTextTransparentColor(global_env->text_transparent_color);
                return neo_copy(elements->tab[start_disp_index + cursor_position]);
            }

            // affichage du menu
            if (refresh) {
                gfx_SetDrawBuffer();
                refresh = false;
                int last_index;
                if (elements->len < vertical_limit)
                    last_index = elements->len;
                else {
                    last_index = vertical_limit;
                }

                // backgroud
                gfx_SetColor(background_color);
                gfx_FillRectangle(0, 20, 320, 220);

                for (int i=0 ; i < last_index ; i++) {
                    // display cursor
                    if (i == cursor_position) {
                        gfx_SetColor(highlight_color);
                        gfx_FillRectangle(7, TEXT_Y(i)-3, 306, 14);

                        gfx_SetTextBGColor(highlight_color);
                        gfx_SetTextFGColor(anti_highlight_color);
                        gfx_PrintStringXY(neo_to_string(elements->tab[start_disp_index + i]), 10, TEXT_Y(i));
                    }
                    // display normal item
                    else {
                        gfx_SetTextBGColor(background_color);
                        gfx_SetTextFGColor(foreground_color);
                        gfx_PrintStringXY(neo_to_string(elements->tab[start_disp_index + i]), 10, TEXT_Y(i));
                    }
                }

                // copy the buffer to the secreen
                gfx_BlitBuffer();
                gfx_SetDrawScreen();
            }
        }
    }
    gfx_SetTextTransparentColor(global_env->text_transparent_color);
    return neo_none_create();
}






NeObj (*graphicfunctions_pointers[NBGRAPHICFUNC])(NeList*) = {
    getKey,
    draw,
    setPixel,
    getPixel,
    setTextTransparentColor,
    getTextWidth,
    rgb,
    menu
};


char* graphicfunctions_names[NBGRAPHICFUNC] = {
    "getKey",
    "draw",
    "setPixel",
    "getPixel",
    "setTextTransparentColor",
    "getTextWidth",
    "rgb",
    "menu"
};


const Function graphicfunctions[NBGRAPHICFUNC] = {
    (Function) {
        .help = "Returns the code of the last key pressed or 0 if no key was pressed",
        .nbArgs = 0,
        .typeArgs = NULL,
        .typeRetour = TYPE_INTEGER
    },
    (Function) {
        .help = "Draws anything drawable",
        .nbArgs = -1,
        .typeArgs = (int[]) {TYPE_UNSPECIFIED},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Sets the indicated pixel to the specified color",
        .nbArgs = 3,
        .typeArgs = (int[]) {TYPE_UNSPECIFIED, TYPE_UNSPECIFIED, TYPE_INTEGER},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Returns the color of the specified pixel, None if the pixel is not within the screen boundaries",
        .nbArgs = 2,
        .typeArgs = (int[]) {TYPE_UNSPECIFIED, TYPE_UNSPECIFIED},
        .typeRetour = TYPE_INTEGER
    },
    (Function) {
        .help = "Sets the color that will be used for transparent while drawing text",
        .nbArgs = 1,
        .typeArgs = (int[]) {TYPE_INTEGER},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Returns the pixel width of a given Text object",
        .nbArgs = 1,
        .typeArgs = (int[]) {TYPE_CONTAINER},
        .typeRetour = TYPE_INTEGER
    },
    (Function) {
        .help = "This function takes 3 integers between 0 and 255 and returns a Neon color (number between 0 and 255) corresponding approximately to the RGB color",
        .nbArgs = 3,
        .typeArgs = (int[]) {TYPE_UNSPECIFIED, TYPE_UNSPECIFIED, TYPE_UNSPECIFIED},
        .typeRetour = TYPE_INTEGER
    },
    (Function) {
        .help = "Displays a menu for choosing between a list of items.\nThe first argument is the title of the menu.\nThe second argument is the list of items. It must me a list of strings.\nThe third argument is the text to be displayed if there are no elements in the list of items",
        .nbArgs = 3,
        .typeArgs = (int[]) {TYPE_STRING, TYPE_LIST, TYPE_STRING},
        .typeRetour = TYPE_UNSPECIFIED
    }
};



NeObj get_graphicfunction(int id) {
    neon_assert(id >= 0 && id < NBGRAPHICFUNC, NEO_VOID);
    
    Function f = graphicfunctions[id];
    return neo_fun_create(id, GraphicModule, f.help, f.nbArgs, f.typeArgs, f.typeRetour);
}

const char* get_graphicfunction_name(int id) {
    neon_assert(id >= 0 && id < NBGRAPHICFUNC, NULL);

    return graphicfunctions_names[id];
}

NeObj call_graphicfunction(int id, NeList* list) {
    neon_assert(id >= 0 && id < NBGRAPHICFUNC, NEO_VOID);

    return graphicfunctions_pointers[id](list);
}



void init_graphicmodule(NeonEnv* env) {
    int nb_types = 9;

    struct ContainerType container_types[] = {
        (struct ContainerType) {
            .name = "Point",
            .fields = (char*[]) {"x", "y"},
            .nb_fields = 2,
            .container_assoc_index = &env->graphic_containers.Point
        },
        (struct ContainerType) {
            .name = "Circle",
            .fields = (char*[]) {"x", "y", "r", "c", "f"},
            .nb_fields = 5,
            .container_assoc_index = &env->graphic_containers.Circle
        },
        (struct ContainerType) {
            .name = "Rect",
            .fields = (char*[]) {"x", "y", "w", "h", "c", "f"},
            .nb_fields = 6,
            .container_assoc_index = &env->graphic_containers.Rect
        },
        (struct ContainerType) {
            .name = "Line",
            .fields = (char*[]) {"x0", "y0", "x1", "y1", "c"},
            .nb_fields = 5,
            .container_assoc_index = &env->graphic_containers.Line
        },
        (struct ContainerType) {
            .name = "Text",
            .fields = (char*[]) {"t", "x", "y", "fg", "bg", "s"},
            .nb_fields = 6,
            .container_assoc_index = &env->graphic_containers.Text
        },
        (struct ContainerType) {
            .name = "Triangle",
            .fields = (char*[]) {"x0", "y0", "x1", "y1", "x2", "y2", "c"},
            .nb_fields = 7,
            .container_assoc_index = &env->graphic_containers.Triangle
        },
        (struct ContainerType) {
            .name = "Polygon",
            .fields = (char*[]) {"pts", "c"},
            .nb_fields = 2,
            .container_assoc_index = &env->graphic_containers.Polygon
        },
        (struct ContainerType) {
            .name = "Ellipse",
            .fields = (char*[]) {"x", "y", "a", "b", "c", "f"},
            .nb_fields = 6,
            .container_assoc_index = &env->graphic_containers.Ellipse
        },
        (struct ContainerType) {
            .name = "FloodFill",
            .fields = (char*[]) {"x", "y", "c"},
            .nb_fields = 3,
            .container_assoc_index = &env->graphic_containers.FloodFill
        }
    };

    for (int i = 0 ; i < nb_types ; i++) {
        int index = 0; // l'indice du container dans CONTAINERS

        // on regarde si ce type de container existe déjà
        if (strlist_inList(env->CONTAINERS, container_types[i].name)) {
            index = strlist_index(env->CONTAINERS, container_types[i].name);
        }
        else {
            index = env->CONTAINERS->len;
            strlist_append(env->CONTAINERS, strdup(container_types[i].name));
            nelist_append(env->ATTRIBUTES, NEO_VOID);
        }

        *container_types[i].container_assoc_index = index; // on associe le type de container à son indice dans CONTAINERS

        // crée la NeList des attributs
        NeList* fields = nelist_create(container_types[i].nb_fields);
        for (int j = 0 ; j < container_types[i].nb_fields ; j++) {
            fields->tab[j] = neo_str_create(strdup(container_types[i].fields[j]));
        }

        // et on met la liste d'attributs dans le tableau d'attributs global
        if (!neo_is_void(env->ATTRIBUTES->tab[index]) && !nelist_equal(neo_to_list(env->ATTRIBUTES->tab[index]), fields)) {
            // si on met pas ce garde fou, on peut overrider le type de container en silence
            nelist_destroy(fields);
            neon_fail(116, neo_new_str_create(container_types[i].name));
            return;
        }

        neobject_destroy(env->ATTRIBUTES->tab[index]);
        env->ATTRIBUTES->tab[index] = gc_extern_neo_list_convert(fields);
    }

    

    for (int i = 0 ; i < NBGRAPHICFUNC ; i++) {
        Var var = get_var((char*)get_graphicfunction_name(i));
        replace_var(var, get_graphicfunction(i));
    }
    return;

}