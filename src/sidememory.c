// bibliothèque pour sauvegarder des objets inertes afin de conserver le plus de heap possible pour les objets en construction
// on y copie toutes les données temporaires nécéssaires à la création d'un TreeBuffer une fois qu'elles sont readonly
#include "headers/sidememory.h"
#include "headers/dynarrays.h"
#include "headers/errors.h"
#include "headers/neonio.h"
#include "headers/parser.h"
#include "headers/neon.h"
#include <stdlib.h>
#include <string.h>

static void* buffer = NULL;
static void* pointer = NULL;

void init_side_memory(void) {
    buffer = malloc(BUFFER_SIZE);
    pointer = buffer;
}


void deinit_side_memory(void) {
    neon_free(buffer);
}


void* side_memory_alloc(int size) {
    void* alloc_area = pointer;
    pointer += size;
    return alloc_area;
}



void copy_intlist_to_side_memory(intlist* list) {
    int* new_tab = side_memory_alloc(sizeof(int) * list->len);
    memcpy(new_tab, list->tab, sizeof(int) * list->len);
    neon_free(list->tab);
    list->tab = new_tab;
}

void copy_toklist_to_side_memory(toklist* list) {
    Token* new_tab = side_memory_alloc(sizeof(Token) * list->len);
    memcpy(new_tab, list->tab, sizeof(Token) * list->len);
    neon_free(list->tab);
    list->tab = new_tab;
}

Ast* copy_ast(Ast* ast) {
    Ast* new_ast = side_memory_alloc(sizeof(Ast));
    memcpy(new_ast, ast, sizeof(Ast));
    return new_ast;
}

Ast* copy_ast_chain(Ast* root) {

    if (root == NULL)
        return NULL;
    else {
        Ast* next = copy_ast_chain(root->suiv);
        Ast* copy = copy_ast(root);
        copy->suiv = next;
        neon_free(root);
        return copy;
    }
}

Ast** copy_ast_to_side_memory(Ast** ast, int length) {
    Ast** new_ast = side_memory_alloc(sizeof(Ast*) * length);

    for (int i=0 ; i < length ; i++) {
        new_ast[i] = copy_ast_chain(ast[i]);
    }
    neon_free(ast);
    return new_ast;
}



///////////// DÉFINITION DE LA STRUCTURE AST //////////////


Ast** ast_create(intlist* typeTok) {
    Ast** ast = side_memory_alloc(sizeof(Ast*) * typeTok->len);

    for (int i = 0 ; i < typeTok->len ; i++) {
        ast[i] = side_memory_alloc(sizeof(Ast));

        ast[i]->type = typeTok->tab[i];
        ast[i]->fin = i;
        ast[i]->suiv = NULL;
    }
    return ast;
}



void ast_push(Ast* ast) {
    Ast* ast2 = side_memory_alloc(sizeof(Ast));

    ast2->fin = ast->fin;
    ast2->type = ast->type;
    ast2->suiv = ast->suiv;
    // on a fait une copie du chaînon

    ast->suiv = ast2;
}


void ast_push_check(Ast** ast, int fin, int type) {
    //if (ast[0]->fin != fin || ast[0]->type != type)
    //{
        ast_push(ast[0]);

        if (global_env->CODE_ERROR == 0) {
            ast[0]->type = type;
            ast[0]->fin = fin;
        }
    //}
}



// Ces fonctions permettent de travailler sur un Ast dans la mémoire figée
int ast_pop(Ast* ast) {
    if (ast -> suiv == NULL) {
        return -1;
    }

    Ast* sov = ast->suiv;
    ast->fin = sov->fin;
    ast->type = sov->type;
    ast->suiv = sov->suiv;
    return 0;
}

/*
void ast_destroy(Ast ** ast, int length) {
    for (int i = 0 ; i < length ; i++) {
        while (ast[i]->suiv != NULL)
            ast_pop(ast[i]);
        neon_free(ast[i]);
    }
    neon_free(ast);
}
*/




char* side_memory_sandwich(char* string, char car)
{

    /* fonction qui ajoute le caractère car avant et après la chaine de caractères string*/
    int len = strlen(string);
    char* newStr = side_memory_alloc(len + 3);

    if (newStr == NULL)
        return NULL;
  
    newStr[0]=car;
    newStr[len+1]=car;
    newStr[len+2]='\0';
    for (int i = 0 ; i < len ; i++)
        newStr[i+1] = string[i];
    

    return newStr;
}




char* side_memory_addStr(char* str1, char* str2)// concatène deux chaines de caractères
{
  char* newStr = side_memory_alloc(strlen(str1) + strlen(str2) + 1);

  if (newStr == NULL)
    return NULL;
  
  strcpy(newStr,str1);
  strcat(newStr,str2);

  return newStr;
}
