#ifndef BUILTINFUNCTIONS_H
#define BUILTINFUNCTIONS_H

#include "constants.h"
#include "objects.h"

#define NBBUILTINFUNC 54

#define ARG(n)      args->tab[n]
NeObj _print_(NeList* args);
NeObj _input_(NeList* args);
NeObj _nbr_(NeList* args);
NeObj _str_(NeList* args);
NeObj _len_(NeList* args);
NeObj _substring_(NeList* args);
NeObj _exit_(NeList* args);
NeObj _append_(NeList* args);
NeObj _remove_(NeList* args);
NeObj _insert_(NeList* args);
NeObj _type_(NeList* args);
NeObj _reverse_(NeList* args);
NeObj _eval_(NeList* args);
NeObj _clear_(NeList* args);
NeObj _help_(NeList* args);
NeObj _randint_(NeList* args);
NeObj _failwith_(NeList* args);
NeObj _time_(NeList* args);
NeObj _assert_(NeList* args);
NeObj _output_(NeList* args);
NeObj _chr_(NeList* args);
NeObj _ord_(NeList* args);
NeObj _list_comp_(NeList* args);
NeObj _create_exception_(NeList* args);
NeObj _raise_(NeList* args);
NeObj _int_(NeList* args);
NeObj _index_(NeList* args);
NeObj _replace_(NeList* args);
NeObj _count_(NeList* args);
NeObj _list_(NeList* args);
NeObj _sort_asc_(NeList* args);
NeObj _sort_desc_(NeList* args);
NeObj _sin_(NeList* args);
NeObj _cos_(NeList* args);
NeObj _tan_(NeList* args);
NeObj _deg_(NeList* args);
NeObj _rad_(NeList* args);
NeObj _sqrt_(NeList* args);
NeObj _ln_(NeList* args);
NeObj _exp_(NeList* args);
NeObj _log_(NeList* args);
NeObj _log2_(NeList* args);
NeObj _round_(NeList* args);
NeObj _abs_(NeList* args);
NeObj _ceil_(NeList* args);
NeObj _floor_(NeList* args);
NeObj _writeFile_(NeList* args);
NeObj _readFile_(NeList* args);
NeObj _setFunctionDoc_(NeList* args);
NeObj _setAtomicTime_(NeList* args);
NeObj _copy_(NeList* args);
NeObj _load_namespace_(NeList* args);
NeObj _gc_(NeList* args);
NeObj _setColor_(NeList* args);

#endif