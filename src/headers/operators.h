#ifndef OPERATORS_H
#define OPERATORS_H

#include <stdlib.h>

#include "errors.h"
#include "objects.h"
#include "constants.h"

#define NBOPERATEURS 40

NeObj _and(TreeBuffer* tb, TreeBufferIndex tree1, TreeBufferIndex tree2);
NeObj _or(TreeBuffer* tb, TreeBufferIndex tree1, TreeBufferIndex tree2);
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


extern void* operators_functions[NBOPERATEURS];


#endif