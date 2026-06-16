#ifndef SIDEMEMORY_H
#define SIDEMEMORY_H

#include "dynarrays.h"
#include "parser.h"
#include "errors.h"
#include "trees.h"

#ifdef TI_EZ80
    #define INITIAL_BASE_POINTER()      (void*)0xd52c00 // (0xd40000 + 320*240)
    #define BUFFER_SIZE                 (320*240)
#elif defined(MINIMAL_LIBC)
    #define BUFFER_SIZE                 (1 << 19)
#else
    #define BUFFER_SIZE                 (1 << 24)
#endif

void side_memory_init(void);
void side_memory_exit(void);
void side_memory_start(void);
void side_memory_end(void);
void move_all_treebuffers(void);
void move_treebuffer_to_side_memory(TreeBuffer* tb);
void copy_intlist_to_side_memory(intlist* list);
void copy_toklist_to_side_memory(toklist* list);
Ast** copy_ast_to_side_memory(Ast** ast, size_t length);
Ast** ast_create(intlist* typeTok);
void ast_push(Ast* ast);
void ast_push_check(Ast** ast, size_t fin, int type);
int ast_pop(Ast* ast);
void ast_destroy(Ast ** ast, size_t length);
char* side_memory_sandwich(char* string, char car);
char* side_memory_addStr(char* str1, char* str2);
#endif