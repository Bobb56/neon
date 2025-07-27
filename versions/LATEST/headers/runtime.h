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


typedef enum {
    Running,
    Finished,
    Uninitialized
} ProcessState;


typedef struct Process
{
    uint8_t registers[64]; // sert à faire les échanges de registres

    // l'état du processus est à Uninitialized si jamais la pile de ce processus n'a pas été appelé dans eval_prolog de manière normale
    // lorsque switch_registers restaure un processus dont ce champ est à Uninitialized, il appelle directement launch_process qui appelle à son tour eval_prolog de manière normale
    // ainsi quand un processus créé avec ce flag termine, il revient automatiquement à launch_process qui le supprime proprement et continue l'exécution
    ProcessState state;
    
    Tree* arg_tree;
    NeObj* arg_obj;
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

ProcessCycle* processCycle_remove(ProcessCycle* pc);

int exec_aux(Tree* tree);
void exec(Tree* tree);
bool isTrue(Tree* tree);
void updateFileName(char* name);
ProcessCycle* processCycle_create(void);
NeObj* eval_prolog(NeObj* obj, Tree* tree);
NeObj eval_aux(Tree* tree);
void eval_epilog(void);
NeObj callUserFunc(UserFunc* fun, NeList* args, NeObj neo_local_args);
void initRuntime(void);
void exitRuntime(void);


#endif