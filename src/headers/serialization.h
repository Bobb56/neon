#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "neobj.h"
#include "objects.h"
#include "neonio.h"

typedef uint8_t PointerType;
#define GC_EXTERN               128
#define IGNORE_GC_PROPERTY      127
#define StringPtr               0
#define FunctionPtr             1
#define TreeBufferPtr           2
#define NeListPtr               3
#define ContainerPtr            4
#define UserFuncPtr             5
#define CharStarPtr             6
#define ContainerNeListPtr      (NeListPtr | GC_EXTERN)
#define UserFuncOptArgsPtr      (NeListPtr | GC_EXTERN)

typedef union {
    String* string;
    Function* function;
    TreeBuffer* treebuffer;
    NeList* nelist;
    Container* container;
    UserFunc* userfunc;
    NeObj* neobjarray;
    char* charstar;
    void* pointer;
} PointerUnion;

void write_number_value2(NeStream stream, intptr_t value);
intptr_t read_number_value2(NeStream stream);

NeObj neobject_deserialize(NeStream stream);
void neobject_serialize(NeStream stream, NeObj neo);

void NeTree_update_ptr_table(TreeBuffer* tb, TreeBufferIndex tree, void* args);
void update_ptr_table_list(NeList* list, intptrlist* ptrTable, intlist* typesTable);
void solve_pointers_aux(NeObj* neo, intptrlist* ptrTable, intlist* typesTable);
void solve_pointers(NeObj* neo, intptrlist* ptrTable, intlist* typesTable);
#endif