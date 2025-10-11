#ifndef NEOBJ_H
#define NEOBJ_H

#include <stdint.h>

// définition de la structure de NeObj
typedef struct Function Function;
typedef struct UserFunc UserFunc;
typedef struct Container Container;
typedef struct NeList NeList;
typedef struct String String;


typedef struct NeObj
{
    union {
        int* refc_ptr; // ce champ permet d'accéder de manière directe au premier champ de chacune des structures

        Container* container;
        NeList* nelist;
        Function* function;
        UserFunc* userfunc;
        String* string;
        double floating;
        intptr_t integer;
    };
    uint8_t type;
} NeObj;



typedef int Var;

#endif