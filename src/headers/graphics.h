#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "objects.h"

#define ARG(n)      args->tab[n]

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

void initGraphics(void);
void draw_nelist(NeList* list);
void draw_obj(NeObj obj);
NeObj draw(NeList* args);
NeObj getKey(NeList* args);
NeObj setPixel(NeList* args);
NeObj getPixel(NeList* args);
NeObj setTextTransparentColor(NeList* args);
NeObj getTextWidth(NeList* args);

#endif