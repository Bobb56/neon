#ifndef OPERATORS_H
#define OPERATORS_H

#include <stdlib.h>

#include "objects.h"
#include "constants.h"

#define NBOPERATEURS 40

NeObj _and(Tree* tree1, Tree* tree2);
NeObj _or(Tree* tree1, Tree* tree2);
NeObj _xor(NeObj op1, NeObj op2);
NeObj _add(NeObj op1, NeObj op2);
NeObj _mul(NeObj op1, NeObj op2);
NeObj _sub(NeObj op1, NeObj op2);
NeObj _div(NeObj op1, NeObj op2);
NeObj _pow(NeObj op1, NeObj op2);
NeObj _equal(NeObj op1, NeObj op2);
NeObj _notEq(NeObj op1, NeObj op2);
NeObj _infEqual(NeObj op1, NeObj op2);
NeObj _supEqual(NeObj op1, NeObj op2);
NeObj _inf(NeObj op1, NeObj op2);
NeObj _sup(NeObj op1, NeObj op2);
NeObj _affect(NeObj op1, NeObj* op2);
NeObj _affectNone(NeObj* op1, NeObj op2);
void _affect2(NeObj* op1, NeObj op2);
NeObj _addEqual(NeObj* op1, NeObj op2);
NeObj _subEqual(NeObj* op1, NeObj op2);
NeObj _mulEqual(NeObj* op1, NeObj op2);
NeObj _divEqual(NeObj* op1, NeObj op2);
NeObj _incr(NeObj* op1);
void _incr2(NeObj* op1, int incr);
NeObj _decr(NeObj* op1);
NeObj _not(NeObj op1);
NeObj _mod(NeObj op1, NeObj op2);
NeObj _eucl(NeObj op1, NeObj op2);
NeObj _ref(NeObj* op1);
NeObj _deref(NeObj op1);
NeObj _goIn(NeObj op1, NeObj op2);
NeObj _minus(NeObj op1);
NeObj _del(NeObj* op1);
NeObj _exponent(NeObj op1, NeObj op2);
NeObj _implique(NeObj op1, NeObj op2);
NeObj _in(NeObj op1, NeObj op2);
NeObj _swap(NeObj* op1, NeObj* op2);



static void* operators_functions[NBOPERATEURS] = {
    (void*)_and,
    (void*)_or,
    (void*)_xor,
    (void*)_add,
    (void*)_mul,
    (void*)_sub,
    (void*)_div,
    (void*)_pow,
    (void*)_equal,
    (void*)_notEq,
    (void*)_infEqual,
    (void*)_supEqual,
    (void*)_inf,
    (void*)_sup,
    (void*)_affectNone,
    (void*)_addEqual,
    (void*)_subEqual,
    (void*)_mulEqual,
    (void*)_divEqual,
    (void*)_incr,
    (void*)_decr,
    (void*)_not,
    (void*)_mod,
    (void*)_eucl,
    (void*)_ref,
    (void*)_goIn,
    (void*)_deref,
    (void*)_minus,
    (void*)_del,
    (void*)_affect,
    NULL,
    (void*)_exponent,
    (void*)_implique,
    (void*)_in,
    NULL,
    NULL,
    (void*)_swap,
    NULL,
    NULL,
    NULL
};



#endif