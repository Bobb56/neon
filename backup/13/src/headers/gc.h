#ifndef GC_H
#define GC_H

#include "constants.h"
#include "objects.h"

void print_objects_list(void);
void gc_free_objects_list(void);
void gc_add_deep_object(const NeObj neo);
void gc_remove_container(Container* c);
void gc_remove_nelist(NeList* l);
void neobject_partial_destroy(NeObj neo);
void nelist_partial_destroy(NeList* neo);
void gc_nelist_mark(NeList* list);
void gc_mark(NeObj obj);
void gc_mark_and_sweep(void);
void gc_final_sweep(void);
void print_objects_list(void);


#endif