#ifndef OBJECTS_H
#define OBJECTS_H

#include <stdint.h>
#include <stdbool.h>

#include "constants.h"
#include "trees.h"


#include "neobj.h"


struct String
{
    int refc;
    char* string;
};



struct NeList
{
    int refc;
    NeObj* tab; // tableau de pointeurs de NeObjects
    int len;
    int capacity;
    struct NeList* myCopy;
    NeObj next;
    NeObj prev;
};

struct Container
{
    int refc;
    int type;
    NeList* data;
    struct Container* myCopy;
    NeObj next;
    NeObj prev;
};



struct Function
{
    int refc;
    NeObj (*ptr) (NeList*);
    const char* help;
    int nbArgs;
    int* typeArgs;
    int typeRetour;
};


struct UserFunc
{
    int refc;
    Var* args;
    int nbArgs;
    bool unlimited_arguments;
    int nbOptArgs; // nombre d'arguments vraiment, vraiment optionnels (par définition, ceux qui sont après ...)
    TreeBuffer* tree_buffer;
    TreeBufferIndex code;
    NeList* opt_args;// valeurs par défaut des arguments optionnels
    char* doc;
};


typedef struct NeonEnv NeonEnv;



#define NEO_TYPE(neo)                       neo.type
#define NEO_VOID                            ((NeObj) {.type = TYPE_EMPTY})
#define NEO_SPECIAL(code)                   ((NeObj) {.type = TYPE_EMPTY, .integer = code})
#define IS_NEO_SPECIAL_CODE(neo, code)      (neo.type == TYPE_EMPTY && neo.integer == code)

#define neobject_destroy(neo)               general_neobject_destroy(neo, false)
#define gc_extern_neobject_destroy(neo)     general_neobject_destroy(neo, true)
#define nelist_destroy(list)                general_nelist_destroy(list, false)
#define gc_extern_nelist_destroy(list)                general_nelist_destroy(list, true)

bool neo_is_void(NeObj neo);
bool neo_exact_equal(NeObj a, NeObj b);

void variable_append(NeonEnv* env, char* name, NeObj value);
void free_var(Var var);
void replace_var(Var var, NeObj object);
void set_var(Var var, NeObj object);
NeObj* get_absolute_address(Var var);
NeObj get_var_value(Var var);
Var get_var(char* name);
Var get_var_from_addr(NeObj* obj);
char* get_name(Var var);
UserFunc* neo_to_userfunc(NeObj neo);
Function* neo_to_function(NeObj neo);
NeObj neo_empty_create(void);
void var_reset(NeObj* neo);
Container* container_create(int type, NeList* data);
NeObj neo_container_create(int type, NeList* data);
Container* neo_to_container(NeObj);
NeObj gc_extern_neo_container_convert(Container* c);
NeObj neo_container_convert(Container* c);
int get_field_index(Container* c, char* name);
NeObj* get_container_field_addr(Container* c, int index);
NeObj get_container_field(Container* c, int index);
void container_destroy(Container* c);
NeObj neo_promise_create(int id);
int get_promise_id(NeObj promise);
void general_neobject_destroy(NeObj neo, bool gc_extern);
void neobject_aff(NeObj neo);
NeList* nelist_create(int len);
void nelist_append(NeList* list, NeObj ptr);//ajoute un élément à la fin de la liste
NeObj* nelist_nth_addr(NeList* list, int index);
NeObj nelist_nth(NeList* list, int index);
NeList* nelist_reverse(NeList* list);
void nelist_remove(NeList* list,int index);
void nelist_aff(NeList* liste);
void general_nelist_destroy(NeList* list, bool gc_extern);
void nelist_destroy_until(NeList *list, int index_max);
NeObj neo_integer_create(intptr_t number);
NeObj neo_double_create(double number);
intptr_t randint(intptr_t min, intptr_t max);
intptr_t neo_to_integer(NeObj neo);
double neo_to_double(NeObj neo);
bool is_number(NeObj obj);
NeObj neo_bool_create(bool neon_boolean);
bool neo_is_true(NeObj neo);
NeObj neo_str_create(char* string);
NeObj neo_list_create(int len);
void neo_list_append(NeObj neo, NeObj ptr);
NeObj neo_list_nth(NeObj neo, int index);
NeObj neo_const_create(char* string);
bool neo_to_bool(NeObj neo);
char* neo_to_string(NeObj neo);
char* neo_to_const(NeObj neo);
NeObj neo_none_create(void);
NeObj neo_copy(NeObj neo);
NeObj neo_dup(NeObj neo);
NeObj neo_deepcopy(NeObj neo);
NeList* nelist_dup(NeList*);
int neo_list_len(NeObj neo);
int nelist_index(NeList* liste, NeObj neo);
int nelist_index2(NeList* l, NeObj neo);
void nelist_insert(NeList* list,NeObj neo, int index);
void neo_list_insert(NeObj neo, NeObj ptr, int index);
void neo_list_remove(NeObj neo, int index);
NeList* neo_to_list(NeObj neo);
char* neobject_str(NeObj);
char* nelist_str(NeList*);
NeObj neo_list_convert(NeList* list);
NeObj gc_extern_neo_list_convert(NeList* list);
Function* function_create(NeObj (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour);
NeObj neo_fun_create(NeObj (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour);
bool funcArgsCheck(Function* fun, NeList* args);
NeObj functionCall(NeObj fun, NeList* args);
char* type(NeObj neo);
NeObj userFuncCreate(Var* args, TreeBuffer* tree_buffer, TreeBufferIndex code, int nbArgs, bool unlimited_arguments, int nbOptArgs, NeList* opt_args, uint8_t type);
NeObj userFuncDefine(NeObj obj, NeList* opt_args);
NeObj neo_exception_create(int index);
int get_exception_code(NeObj exception);
bool neo_equal(NeObj _op1, NeObj _op2);
bool nelist_inList(NeList* list, NeObj neo);
bool nelist_inList2(NeList* list, NeObj neo);
bool nelist_equal(NeList* l1, NeList* l2);
int neo_compare(NeObj a, NeObj b); // ordre croissant
int neo_compare2(NeObj a, NeObj b); // ordre décroissant
void neo_list_free(NeObj list);
void mark(NeObj neo);
void unmark(NeObj neo);
void recursive_unmark(NeObj neo);
bool ismarked(NeObj neo);
void update_if_promise(NeObj* promise);
char* var_name(NeObj obj);
NeObj callOverloadedBinaryOperator(NeObj op1, NeObj op2, char* opname);
NeObj callOverloadedUnaryOperator(NeObj op1, char* opname);

#endif