#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "objects.h"

#define ARG(n)                  args->tab[n]

#define NBGRAPHICFUNC           8

struct ContainersAssoc {
    int Rect;
    int Circle;
    int Triangle;
    int Polygon;
    int Ellipse;
    int FloodFill;
    int Line;
    int Text;
    int Point;
};

struct ContainerType {
    char* name;
    char** fields;
    int nb_fields;
    int* container_assoc_index;
};


#define NEON_PALETTE_COLOR(r,g,b)       (r << 5 | g << 2 | b)
#define NEON_PALETTE_BLACK              NEON_PALETTE_COLOR(0,0,0)
#define NEON_PALETTE_WHITE              NEON_PALETTE_COLOR(7,7,3)
#define NEON_PALETTE_GREEN              NEON_PALETTE_COLOR(0,5,0)
#define NEON_PALETTE_RED                NEON_PALETTE_COLOR(7,0,0)
#define NEON_PALETTE_BLUE               NEON_PALETTE_COLOR(0,0,3)
#define NEON_PALETTE_PURPLE             NEON_PALETTE_COLOR(7,0,3)


int neon_getKey(void);
void draw_nelist(NeList* list);
void draw_obj(NeObj obj);
void set_neon_palette(void);

NeObj draw(NeList* args);
NeObj getKey(NeList* args);
NeObj setPixel(NeList* args);
NeObj getPixel(NeList* args);
NeObj setTextTransparentColor(NeList* args);
NeObj getTextWidth(NeList* args);
NeObj rgb(NeList* args);
NeObj menu(NeList* args);


void init_graphicmodule(NeonEnv* env);
NeObj call_graphicfunction(int id, NeList* list);
const char* get_graphicfunction_name(int id);
NeObj get_graphicfunction(int id);

#endif