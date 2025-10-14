#ifndef LOWLEVEL_H
#define LOWLEVEL_H

#include "constants.h"
#include "runtime.h"
#include "objects.h"
#include "processcycle.h"

void reset_stack_and_registers(Process* process);
void save_stack_and_registers(Process* process);
void switch_registers(Process* process, Process* prev);
void* get_stack(void);

#ifdef TI_EZ80
void* floodfill_switch_stack(void* new_stack);
#endif

#endif