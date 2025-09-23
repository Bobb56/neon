#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "objects.h"

#define ARG(n)      args->tab[n]

struct ContainersAssoc {
    int Rect, FilledRect;
    int Circle, FilledCircle;
    int Line;
    int Text;
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


#endif