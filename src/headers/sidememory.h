#ifndef SIDEMEMORY_H
#define SIDEMEMORY_H

#include "dynarrays.h"
#include "parser.h"

#define BUFFER_SIZE         (1 << 24)

void init_side_memory(void);
void deinit_side_memory(void);
void copy_intlist_to_side_memory(intlist* list);
void copy_toklist_to_side_memory(toklist* list);
Ast** copy_ast_to_side_memory(Ast** ast, int length);
Ast** ast_create(intlist* typeTok);
void ast_push(Ast* ast);
void ast_push_check(Ast** ast, int fin, int type);
int ast_pop(Ast* ast);
void ast_destroy(Ast ** ast, int length);
char* side_memory_sandwich(char* string, char car);
char* side_memory_addStr(char* str1, char* str2);
#endif