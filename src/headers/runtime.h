#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>

#include "constants.h"
#include "objects.h"
#include "dynarrays.h"




typedef struct NeSave
{
    NeObj object;
    Var var;
} NeSave;





typedef struct Process
{
    uint8_t registers[64]; // sert à faire les échanges de registres
    Tree* arg_tree;
    Tree* original_call; // pointeur vers l'arbre original (le premier argument) de ce processus. Sert à libérer les arbres temporaires que l'on crée pour lancer des promesses
    void* stack; // ceci est un pointeur sur l'adresse de début de la pile (le haut de la pile), qui servira à la libérer
    ptrlist* var_loc; // les variables locales créés depuis le lancement du processus
    int id;
    ptrlist* varsToSave;
} Process;

typedef struct ProcessCycle
{
    Process* process;
    struct ProcessCycle* next;
    struct ProcessCycle* prev;
} ProcessCycle;




bool neoIsTrue(NeObj neo);
bool isTrue(Tree* tree);
NeObj eval(Tree* tree);
NeObj* get_address(Tree* tree);
void local(Var var, ptrlist* var_loc);
void newContext(ptrlist* var_loc);
void deleteContext(ptrlist* var_loc);
int exec_aux(Tree* tree);
void exec(Tree* tree);
bool isTrue(Tree* tree);
void updateFileName(char* name);
ProcessCycle* processCycle_create(void);
NeObj eval_prolog(Tree* tree);
NeObj eval_aux(Tree* tree);
NeObj eval_epilog(void);
NeObj callUserFunc(UserFunc* fun, NeList* args, NeObj neo_local_args);
void initRuntime(void);
void exitRuntime(void);



#endif