#include "headers/objects.h"
#include "headers/trees.h"
#define NEON_SOURCE_ID 26

#include "headers/errors.h"
#include "headers/interpcontext.h"
#include "headers/neonio.h"
#include <string.h>

InterpContext* InterpContext_create(void) {
    InterpContext* interp_context = neon_malloc(sizeof(InterpContext));
    interp_context->pointer = 0;
    interp_context->n_rets = 0;
    interp_context->capacity = 8;
    interp_context->stack = neon_malloc((1 << interp_context->capacity) * sizeof(struct InterpContextItem));
    interp_context->len = 0;
    return interp_context;
}



void InterpContext_aff(InterpContext* interp_context) {
    for (int i=interp_context->len-1 ; i >= 0 ; i--) {
        struct InterpContextItem* item = &interp_context->stack[i];
        if (item->type == Object) {
            printString("Object: ");
            neobject_aff(item->obj);
        }
        else {
            printString("Code: ");
            printInt(item->code);
        }
        newLine();
    }
}



void InterpContext_destroy(InterpContext* interp_context) {
    for (size_t i=0 ; i < interp_context->len ; i++) {
        if (interp_context->stack[i].type == Object) {
            neobject_destroy(interp_context->stack[i].obj);
        }
    }
    neon_free(interp_context->stack);
    neon_free(interp_context);
}

// The object `obj` can be destroyed; InterpContext_push uses a copy of it
void InterpContext_push(InterpContext* interp_context, struct InterpContextItem item) {
    if (interp_context->len + 1 >= (size_t)(1 << interp_context->capacity)) {
        interp_context->capacity += 1;
        interp_context->stack = neon_realloc(interp_context->stack, (1 << interp_context->capacity) * sizeof(struct InterpContextItem));
        if (interp_context->stack == NULL) {
            neon_fail(12, NO_ARGS);
            return;
        }
    }
    interp_context->stack[interp_context->len] = item;
    interp_context->len += 1;

    // If we modify the interp context, we stop reading from it
    // When we push things on the context, it means that we ARE the time
    // We are creating/extending the time, we are the present and the future
    // So it would make no sense to also read from the past
    interp_context->pointer = 0;
}

void InterpContext_push_obj(InterpContext* interp_context, TreeBufferIndex tree, NeObj obj) {
    struct InterpContextItem item = (struct InterpContextItem) {
        .type = Object,
        .tree = tree,
        .obj = neo_copy(obj)
    };
    InterpContext_push(interp_context, item);
}

void InterpContext_push_ret_code(InterpContext* interp_context, TreeBufferIndex tree, int code) {
    struct InterpContextItem item = (struct InterpContextItem) {
        .type = Code,
        .tree = tree,
        .code = code
    };
    InterpContext_push(interp_context, item);
}


struct InterpContextItem* InterpContext_get_value(InterpContext* interp_context, TreeBufferIndex tree) {
    if (interp_context->pointer < interp_context->len) {
        struct InterpContextItem* item = &interp_context->stack[interp_context->pointer];

        if (item->tree == tree) {
            interp_context->pointer += 1;
            return item;
        }
    }

    return NULL;
}

// Prepares an InterpContext to be restored
void InterpContext_reset(InterpContext* interp_context) {
    interp_context->pointer = 0;
    interp_context->n_rets = 0;
}

size_t InterpContext_before_call(InterpContext* interp_context) {
    // Track calls and returns to have information about the execution context
    // So that we can delete useless contexts
    interp_context->n_rets = 0;

    return interp_context->len;
}

void InterpContext_after_call(InterpContext* interp_context) {
    interp_context->n_rets ++;
}


void InterpContext_stack_obj(InterpContext* interp_context, NeObj obj, TreeBufferIndex tree, size_t last_context) {
    // If there are two returns in sequence, we can replace all previous values by the one we get
    if (interp_context->n_rets == 2) {
        interp_context->n_rets = 1;

        // Destroy all upper contexts and the objects it contains
        for (int i = interp_context->len-1 ; i >= (int)last_context ; i--) {
            if (interp_context->stack[i].type == Object) {
                neobject_destroy(interp_context->stack[i].obj);
            }
        }
        interp_context->len = last_context;
        
    }
    InterpContext_push_obj(interp_context, tree, obj);
}


void InterpContext_stack_ret_code(InterpContext* interp_context, int code, TreeBufferIndex tree, size_t last_context) {
    // If there are two returns in sequence, we can replace all previous values by the one we get
    if (interp_context->n_rets == 2) {
        interp_context->n_rets = 1;

        // Destroy all upper contexts and the objects it contains
        for (int i = interp_context->len-1 ; i >= (int)last_context ; i--) {
            if (interp_context->stack[i].type == Object) {
                neobject_destroy(interp_context->stack[i].obj);
            }
        }
        interp_context->len = last_context;
    }
    InterpContext_push_ret_code(interp_context, tree, code);
}

// Deletes `steps` values from the top of the context to go back in the context
void InterpContext_back(InterpContext* interp_context, size_t steps) {
    if (steps >= interp_context->len) {
        interp_context->len = 0;
    }
    else {
        interp_context->len -= steps;
    }
}