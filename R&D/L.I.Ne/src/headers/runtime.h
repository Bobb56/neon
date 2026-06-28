#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>

#include "constants.h"
#include "interpcontext.h"
#include "objects.h"
#include "dynarrays.h"
#include "trees.h"

#define ADD_STACK_SIZE(n)   volatile char dummy[n] = {0} ; (void)dummy

#define MAY_INTERRUPT()     if (global_env->atomic_counter == 0) {                      \
                                global_env->atomic_counter = global_env->ATOMIC_TIME;   \
                                interrupt();                                            \
                            }                                                           \
                            global_env->atomic_counter--

#define tb_eval(tb, interp_context)                 eval(tb, (tb)->entry_point, interp_context)
#define tb_exec(tb)                                 exec(tb, (tb)->entry_point)
#define tb_launch_execution(tb, interp_context)     launch_execution(tb, (tb)->entry_point, interp_context)

void stop_execution(void);
bool neoIsTrue(NeObj neo);
bool isTrue(TreeBuffer* tb, TreeBufferIndex tree);
NeObj launch_evaluation(TreeBuffer* tb, TreeBufferIndex tree, InterpContext* interp_context);
NeObj* get_address(TreeBuffer* tb, TreeBufferIndex tree);
int exec(TreeBuffer* tb, TreeBufferIndex tree);
void launch_execution(TreeBuffer* tb, TreeBufferIndex tree, InterpContext* interp_context);
bool isTrue(TreeBuffer* tb, TreeBufferIndex tree);
void update__name__(char* name);
NeObj eval(TreeBuffer* tb, TreeBufferIndex tree);
NeObj callUserFunc(UserFunc* fun, Var* variables, NeObj* values, int variable_index, NeObj local_args, NeObj* self_address);
void initRuntime(InterpContext* interp_context);
void exitRuntime(void);


#endif