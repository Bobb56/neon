#include "headers/neonio.h"
#define NEON_SOURCE_ID 19

/*
Cette bibliothèque gère une deuxième zone mémoire utilisée pour désengorger le heap
Cette mémoire est divisée en deux zones : la zone permanente, suivie de la zone temporaire.

|       Zone permanente         |       Zone temporaire -->
^                               ^                         ^
initial_base_pointer            base_pointer              pointer

La zone temporaire s'utilise par sessions : on appelle side_memory_start() pour commencer à allouer dedans et on appelle side_memory_end() lorsque l'on a terminé. Pour allouer dans la zone temporaire si une session est en cours, on appelle side_memory_alloc()
Entre deux sessions d'utilisation de la mémoire temporaire, on peut écrire dans la mémoire permanente. Ce qui est écrit dans la mémoire permanente ne sera supprimé qu'à la sortie de l'interpréteur Neon. Pour allouer dans la zone permanente si aucune session n'est en cours, on utilise side_memory_hard_alloc()
*/

#include "headers/sidememory.h"
#include "headers/dynarrays.h"
#include "headers/errors.h"
#include "headers/parser.h"
#include "headers/neon.h"
#include "headers/trees.h"
#include <string.h>

#ifdef TI_EZ80
#include <ti/vars.h>
#include <graphx.h>
#endif

static int buffer_size = 0;
static void* base_pointer = NULL;
static void* pointer = NULL;
static void* initial_base_pointer = NULL;
static bool open = false;

// Un mécanisme qui permet de stocker de manière permanente des TreeBuffers dans la side memory
static ptrlist* pending_treebuffers = NULL;

// À appeler au chargement de l'interpréteur Neon
void side_memory_init(void) {
    #ifdef TI_EZ80
        buffer_size = BUFFER_SIZE;
        initial_base_pointer = INITIAL_BASE_POINTER();
        //buffer_size = os_MemChk(&initial_base_pointer);
    #else
        initial_base_pointer = neon_malloc(BUFFER_SIZE);
        buffer_size = BUFFER_SIZE;
    #endif
    base_pointer = initial_base_pointer;

    pending_treebuffers = ptrlist_create();
}

// À appeler avant de sortir de l'interpréteur Neon
void side_memory_exit(void) {
    #ifndef TI_EZ80
        neon_free(initial_base_pointer);
    #endif
    ptrlist_destroy(pending_treebuffers, false, true);
}


// Début d'une session d'utilisation de la side memory
void side_memory_start(void) {
    #ifdef TI_EZ80
        if (gfx_vbuffer != INITIAL_BASE_POINTER()) {
            gfx_BlitScreen();
            gfx_SwapDraw();
        }
    #endif
    pointer = base_pointer;
    open = true;
}

// Fin d'une session d'utilisation de la side memory
void side_memory_end(void) {
    open = false;
    move_all_treebuffers();
    pointer = base_pointer;
}


// Cette fonction doit toujours renvoyer des pointeurs alignés
// On suppose que le début du buffer est déjà bien aligné, on se charge seulement à l'allocation
// de conserver l'alignement
// On maintient ici un alignement à 8 octets
void* side_memory_alloc(size_t size) {
    // Extension de la taille de la zone à allouer pour qu'elle soit alignée à 8 octets
    size = align8(size);

    void* alloc_area = pointer;
    if (pointer + size >= initial_base_pointer + buffer_size) {
        neon_fail(12, NO_ARGS);
        return NULL;
    }
    pointer += size;
    return alloc_area;
}

// Alloue définitivement de la mémoire dans la side memory
// Part du principe que la side memory ne contient aucune donnée temporaire
void* side_memory_hard_alloc(size_t size) {
    neon_assert(!open, NULL);

    void* alloc_area = base_pointer;
    if (base_pointer + size >= initial_base_pointer + buffer_size) {
        neon_fail(12, NO_ARGS);
        return NULL;
    }
    base_pointer += size;

    if (base_pointer > pointer)
        pointer = base_pointer;

    return alloc_area;
}


void move_treebuffer_to_side_memory(TreeBuffer* tb) {
    UNUSED_PARAMETER(tb);

    //ptrlist_append(pending_treebuffers, tb);
}

void move_treebuffer(TreeBuffer* tb) {
    void* new_pointer = side_memory_hard_alloc(tb->block_size * tb->n_blocks);
    return_on_error();
    memcpy(new_pointer, tb->pointer, tb->block_size * tb->n_blocks);
    neon_free(tb->pointer);
    tb->pointer = new_pointer;
    tb->side_memory = true;
}

void move_all_treebuffers(void) {
    while (!ptrlist_isEmpty(pending_treebuffers)) {
        TreeBuffer* tb = ptrlist_pop(pending_treebuffers);
        move_treebuffer(tb);
        return_on_error();
    }
}


void copy_intlist_to_side_memory(intlist* list) {
    int* new_tab = side_memory_alloc(sizeof(int) * list->len);
    return_on_error();
    memcpy(new_tab, list->tab, sizeof(int) * list->len);
    neon_free(list->tab);
    list->tab = new_tab;
}

void copy_toklist_to_side_memory(toklist* list) {
    Token* new_tab = side_memory_alloc(sizeof(Token) * list->len);
    return_on_error();
    memcpy(new_tab, list->tab, sizeof(Token) * list->len);
    neon_free(list->tab);
    list->tab = new_tab;
}

Ast* copy_ast(Ast* ast) {
    Ast* new_ast = side_memory_alloc(sizeof(Ast));
    return_on_error(NULL);
    memcpy(new_ast, ast, sizeof(Ast));
    return new_ast;
}

Ast* copy_ast_chain(Ast* root) {
    if (root == NULL)
        return NULL;
    else {
        Ast* next = copy_ast_chain(root->suiv);
        return_on_error(NULL);
        Ast* copy = copy_ast(root);
        return_on_error(NULL);
        copy->suiv = next;
        neon_free(root);
        return copy;
    }
}

Ast** copy_ast_to_side_memory(Ast** ast, size_t length) {
    void* pointer_save = pointer; // On sauvegarde le pointeur avant de faire des modifs au cas où ça échoue
    Ast** new_ast = side_memory_alloc(sizeof(Ast*) * length);
    return_on_error(NULL);

    for (size_t i=0 ; i < length ; i++) {
        new_ast[i] = copy_ast_chain(ast[i]);
        if_error {
            pointer = pointer_save; // On restaure le pointeur à l'état d'arrivée
            return NULL;
        }
    }
    neon_free(ast);
    return new_ast;
}



///////////// DÉFINITION DE LA STRUCTURE AST //////////////


Ast** ast_create(intlist* typeTok) {
    void* pointer_save = pointer;
    
    Ast** ast = side_memory_alloc(sizeof(Ast*) * typeTok->len);
    return_on_error(NULL);

    for (size_t i = 0 ; i < typeTok->len ; i++) {
        ast[i] = side_memory_alloc(sizeof(Ast));
        return_on_error(NULL);

        if_error {
            pointer = pointer_save;
            return NULL;
        }

        ast[i]->type = typeTok->tab[i];
        ast[i]->fin = i;
        ast[i]->suiv = NULL;
    }
    return ast;
}



void ast_push(Ast* ast) {
    Ast* ast2 = side_memory_alloc(sizeof(Ast));
    return_on_error();

    ast2->fin = ast->fin;
    ast2->type = ast->type;
    ast2->suiv = ast->suiv;
    // on a fait une copie du chaînon

    ast->suiv = ast2;
}


void ast_push_check(Ast** ast, size_t fin, int type) {
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
    size_t len = strlen(string);
    char* newStr = side_memory_alloc(len + 3);
    return_on_error(NULL);
  
    newStr[0]=car;
    newStr[len+1]=car;
    newStr[len+2]='\0';
    for (size_t i = 0 ; i < len ; i++)
        newStr[i+1] = string[i];
    

    return newStr;
}




char* side_memory_addStr(char* str1, char* str2)// concatène deux chaines de caractères
{
  char* newStr = side_memory_alloc(strlen(str1) + strlen(str2) + 1);
  return_on_error(NULL);
  
  strcpy(newStr,str1);
  strcat(newStr,str2);

  return newStr;
}
