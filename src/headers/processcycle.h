#ifndef PROCESSCYCLE_H
#define PROCESSCYCLE_H

#include <stdint.h>
#include "syntaxtrees.h"



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

ProcessCycle* processCycle_create(void);
ProcessCycle* processCycle_remove(ProcessCycle* pc);
ProcessCycle* loadNextLivingProcess(ProcessCycle* pc);
void unloadCurrentProcess(Process* p);
Process* processCycle_add(ProcessCycle* pc, Tree* tree, int id, bool delete_tree, bool isInitialized);
bool processCycle_isActive(ProcessCycle* cycle);
bool processCycle_isEmpty(ProcessCycle* pc);
void processCycle_clean(ProcessCycle* cycle);
void process_preRemove(Process* p);

void save_later(ptrlist* variables_a_sauvegarder, Var var);
void switchGlobalLocalVariables(ptrlist* varsToSave);
void partialRestore(ptrlist* varsToSave, ptrlist* sov_vars_to_save);
int create_new_process(Tree* tree, bool delete_tree, bool isInitialized);

#endif