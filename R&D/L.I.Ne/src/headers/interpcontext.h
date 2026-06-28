#ifndef INTERPCONTEXT_H
#define INTERPCONTEXT_H

#include "trees.h"
#include <stddef.h>
#include <stdint.h>

#define INTERP_CONTEXT_ITEM_VOID    ((struct InterpContextItem){0})

struct InterpContextItem {
    enum {
        Object,
        Code
    } type;
    TreeBufferIndex tree;
    union {
        NeObj obj;
        int code;
    };
};

typedef struct InterpContext {
    size_t len;
    uint8_t capacity;
    struct InterpContextItem* stack; 
    uint8_t n_rets;
    size_t pointer;
} InterpContext;




InterpContext* InterpContext_create(void);
void InterpContext_aff(InterpContext* interp_context);
void InterpContext_destroy(InterpContext* interp_context);
void InterpContext_push_obj(InterpContext* interp_context, TreeBufferIndex tree, NeObj obj);
void InterpContext_push_ret_code(InterpContext* interp_context, TreeBufferIndex tree, int code);
struct InterpContextItem* InterpContext_get_value(InterpContext* interp_context, TreeBufferIndex tree);
size_t InterpContext_before_call(InterpContext* interp_context);
void InterpContext_after_call(InterpContext* interp_context);
void InterpContext_stack_obj(InterpContext* interp_context, NeObj obj, TreeBufferIndex tree, size_t last_context);
void InterpContext_stack_ret_code(InterpContext* interp_context, int code, TreeBufferIndex tree, size_t last_context);
void InterpContext_reset(InterpContext* interp_context);
void InterpContext_back(InterpContext* interp_context, size_t steps);

#endif



/*
// token-paste helpers
#define CAT2(a,b) a##b
#define CAT(a,b)  CAT2(a,b)

#define DEFINE_CONTEXT(id, ...)         \
    struct CAT(_context_struct_, id) {  \
        __VA_ARGS__                     \
    }

#define make_context(id, ...)       (struct CAT(_context_struct_, id)) {__VA_ARGS__}


#define InterpContext_push_struct(interp_context, struct)  \
    InterpContext_push(interp_context, (char*)&struct, sizeof(struct))



#define InterpContext_pop_struct(interp_context, struct)  \
    InterpContext_push(interp_context, (char*)&struct, sizeof(struct))



#define STRUCT_AFFECT_1(a)                      .a=a
#define STRUCT_AFFECT_2(a,b)                    .a=a,.b=b
#define STRUCT_AFFECT_3(a,b,c)                  .a=a,.b=b,.c=c
#define STRUCT_AFFECT_4(a,b,c,d)                .a=a,.b=b,.c=c,.d=d
#define STRUCT_AFFECT_5(a,b,c,d,e)              .a=a,.b=b,.c=c,.d=d,.e=e
#define STRUCT_AFFECT_6(a,b,c,d,e,f)            .a a,.b=b,.c=c,.d=d,.e=e,.f=f
#define STRUCT_AFFECT_7(a,b,c,d,e,f,g)          .a a,.b=b,.c=c,.d=d,.e=e,.f=f,g=g
#define STRUCT_AFFECT_8(a,b,c,d,e,f,g,h)        .a a,.b=b,.c=c,.d=d,.e=e,.f=f,g=g,.h=h
#define STRUCT_AFFECT_9(a,b,c,d,e,f,g,h,i)      .a a,.b=b,.c=c,.d=d,.e=e,.f=f,g=g,.h=h,.i=i
#define STRUCT_AFFECT_10(a,b,c,d,e,f,g,h,i,j)   .a a,.b=b,.c=c,.d=d,.e=e,.f=f,g=g,.h=h,.i=i,.j=j


#define SAVE_CONTEXT(id, nb_args, ...)                                                      \
    struct CAT(_context_struct_, id) temp_struct = (struct CAT(_context_struct_, id)) {     \
        CAT(STRUCT_AFFECT_, nb_args)(__VA_ARGS__)                                           \
    };                                                                                      \
    InterpContext_push_struct(interp_context, temp_struct)


#define RESTORE_CONTEXT(id, nb_args, ...)                                                   \
    if (false) {                                                                            \
        CAT(_context_label, id):;                                                           \
        struct CAT(_context_struct_, id) temp_struct = (struct CAT(_context_struct_, id)) { \
            CAT(STRUCT_AFFECT_, nb_args)(__VA_ARGS__)                                       \
        };                                                                                   \
    }

*/

/*
How to use those macros ?
---------------------------
// If we are restoring a context, we can jump into that macro
// and restore local vars that are computed before the macro
// to simulate the fact that we already computed them
RESTORE_CONTEXT(id, nb_args, ...);

NeObj ret = eval(tb, tree);

if_error {
    // Here we push the context that we want to find in the RESTORE_CONTEXT above
    PUSH_CONTEXT(id, ...);
    return NEO_VOID;
}

In what conditions do we need to save/restore the context ?
-----------------------------------------------------------
The goal of restoring the context is to simulate the fact that
we already computed the local variables before

So we need to restore the context if there are
previous local variables computed before.
We can restrict this condition to the computations that
could take time, e.g. calls that may call eval_aux again

And we need to save the context each time we need to restore it
*/