#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "neobj.h"
#include "objects.h"
#include "neonio.h"

typedef uint8_t PointerType;
#define GC_EXTERN               128
#define WITHOUT_GC_PROPERTY     127
#define StringPtr               0
#define FunctionPtr             1
#define TreeBufferPtr           2
#define NeListPtr               3
#define ContainerPtr            4
#define UserFuncPtr             5
#define ContainerNeListPtr      (NeListPtr | GC_EXTERN)

typedef union {
    String* string;
    Function* function;
    TreeBuffer* treebuffer;
    NeList* nelist;
    Container* container;
    UserFunc* userfunc;
    NeObj* neobjarray;
    void* pointer;
} PointerUnion;

#define MAX_VALUE(bits)             ((uintptr_t)1 << bits)

NeObj neobject_deserialize(NeStream stream);
void neobject_serialize(NeStream stream, NeObj neo);

void update_ptr_table_list(NeList* list, intptrlist* ptrTable, intlist* typesTable);
void solve_pointers(NeObj* neo, intptrlist* ptrTable, intlist* typesTable);
#endif