#ifndef LOWLEVEL_H
#define LOWLEVEL_H

#include "runtime.h"
#include "objects.h"

NeObj reset_stack_and_registers(void);
void switch_registers(Process* process, Process* prev, int save_stack, int load_stack);
NeObj eval_epilog_unaligned(void);

#endif