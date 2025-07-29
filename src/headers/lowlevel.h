#ifndef LOWLEVEL_H
#define LOWLEVEL_H

#include "runtime.h"
#include "objects.h"
#include "processcycle.h"

void reset_stack_and_registers(void);
void save_stack_and_registers(void);
void switch_registers(Process* process, Process* prev);
void* get_stack(void);

#endif