#ifndef LOWLEVEL_H
#define LOWLEVEL_H

#include "runtime.h"
#include "objects.h"
#include "processcycle.h"

NeObj* reset_stack_and_registers(void);
void switch_registers(Process* process, Process* prev, uint8_t flags);
void* get_stack(void);

#endif