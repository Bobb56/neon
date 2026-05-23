#include "headers/serialization.h"
#include "headers/constants.h"
#include "headers/dynarrays.h"
#include "headers/neobj.h"
#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/errors.h"
#include "headers/trees.h"
#include "headers/neon.h"

#include <stdint.h>
#include <string.h>


/*
TODO
- Il faut écrire la structure des types de Containers utilisés en en-tête du fichier
- Écrire plein d'autres trucs en en-tête du fichier
- Ne pas oublier de bien gérer le compteur de références et le garbage collector lors de la désérialisation

*/


/*
Procédé de la sérialisation :
-----------------------------
- Parcours complet de tout l'objet, construction d'une table d'association numéro/pointeur
- Sérialisation de chaque pointeur de la table


Procédé de la désérialisation :
-------------------------------
- Désérialisation itérative de tous les pointeurs et reconstruction de la table des pointeurs
- Parcours de tout l'objet pour restaurer les pointeurs dedans
*/


PointerType getPointerType(NeObj neo) {
    switch (NEO_TYPE(neo)) {
        case TYPE_STRING: return StringPtr;
        case TYPE_BUILTINFUNC: return FunctionPtr;
        case TYPE_LIST: return NeListPtr;
        case TYPE_CONTAINER: return ContainerPtr;
        case TYPE_USERFUNC:
        case TYPE_USERMETHOD:
            return UserFuncPtr;
        default: return -1;
    }
}

void write_number_value(NeStream stream, intptr_t value) {
    uint8_t size = strnlen((char*)&value, sizeof(intptr_t));
    NeStream_write(stream, &size, 1);
    NeStream_write(stream, &value, size);
}

int min(int a, int b) {
    if (a < b)
        return a;
    else
        return b;
}



intptr_t read_number_value(NeStream stream) {
    uint8_t size;
    intptr_t value = 0;
    if (NeStream_read(stream, &size, 1) && NeStream_read(stream, &value, min(size, sizeof(intptr_t)))) {
        return value;
    }
    else {
        global_env->CODE_ERROR = 122;
        return 0;
    }
}





void update_ptr_table_obj(NeObj obj, intptrlist* ptrTable, intlist* typesTable) {
    if ((NEO_TYPE(obj) & HEAP_ALLOCATED) && intptrlist_index(ptrTable, obj.refc_ptr) == -1) {
        intlist_append(typesTable, getPointerType(obj));
        intptrlist_append(ptrTable, obj.refc_ptr);

        if (NEO_TYPE(obj) == TYPE_LIST) {
            update_ptr_table_list(obj.nelist, ptrTable, typesTable);
        }
        else if (NEO_TYPE(obj) == TYPE_CONTAINER) {
            intlist_append(typesTable, ContainerNeListPtr);
            intptrlist_append(ptrTable, obj.container->data);
            update_ptr_table_list(obj.container->data, ptrTable, typesTable);
        }
        else if (NEO_TYPE(obj) == TYPE_USERFUNC || NEO_TYPE(obj) == TYPE_USERMETHOD) {
            // Ajout du TreeBuffer
            TreeBuffer* tb = obj.userfunc->tree_buffer;
            if (intptrlist_index(ptrTable, tb) == -1) {
                intlist_append(typesTable, TreeBufferPtr);
                intptrlist_append(ptrTable, tb);
            }
            
            // Update the pointer table with some pointers that are not stored directly in the TreeBuffer (the NeObjects for instance)
            void* args[] = {(void*)ptrTable, (void*)typesTable};
            TreeBuffer_iter(tb, NeTree_update_ptr_table, args);

            // Ajout des arguments optionnels
            NeList* opt_args = obj.userfunc->opt_args;
            if (intptrlist_index(ptrTable, opt_args) == -1) {
                intlist_append(typesTable, NeListPtr);
                intptrlist_append(ptrTable, opt_args);
                update_ptr_table_list(opt_args, ptrTable, typesTable);
            }
        }
    }
}

void update_ptr_table_list(NeList* list, intptrlist* ptrTable, intlist* typesTable) {
    for (int i=0 ; i < list->len ; i++) {
        update_ptr_table_obj(list->tab[i], ptrTable, typesTable);
    }
}



void NeTree_update_ptr_table(TreeBuffer* tb, TreeBufferIndex tree, void* args) {
    intptrlist* ptrTable = ((void**)args)[0];
    intlist* typesTable = ((void**)args)[1];

    if (TREE_ISVOID(tree))
        return;
    
    switch (TREE_TYPE(tb, tree)) {
        case TypeConst:
            update_ptr_table_obj(treeConst(tb, tree)->obj, ptrTable, typesTable);
            break;
        
        case TypeFunctiondef:
            intptrlist_append(ptrTable, treeFDef(tb, tree)->name);
            intlist_append(typesTable, CharStarPtr);

            update_ptr_table_obj(treeFDef(tb, tree)->object, ptrTable, typesTable);
            break;
        
        case TypeFunctioncall:
            if (!neo_is_void(treeFCall(tb, tree)->function_obj))
                update_ptr_table_obj(treeFCall(tb, tree)->function_obj, ptrTable, typesTable);
            break;
        
        case TypeAttribute:
            intptrlist_append(ptrTable, treeAttr(tb, tree)->name);
            intlist_append(typesTable, CharStarPtr);
            break;
        
        case TypeAttributeLit:
            intptrlist_append(ptrTable, treeAttrLit(tb, tree)->name);
            intlist_append(typesTable, CharStarPtr);
            break;
            
        case TypeParallelCall:
            intptrlist_append(ptrTable, treeParCall(tb, tree)->expr_buffer);
            intlist_append(typesTable, TreeBufferPtr);
            break;
    }
}



// Pour sérialiser l'arbre, on crée un buffer de la taille de l'arbre, on modifie les champs qu'on veut, puis on écrit le buffer dans le fichier
void NeTree_serialize(NeStream stream, uint8_t** pointer, intptrlist* ptrTable, intlist* typesTable) {
    
}




void serialize_partial_neobject(NeStream stream, NeObj neo, intptrlist* ptrTable) {
    NeStream_write(stream, &neo.type, 1);
    if (NEO_TYPE(neo) & HEAP_ALLOCATED) {
        intptr_t index = intptrlist_index(ptrTable, neo.refc_ptr);
        write_number_value(stream, index);
    }
    else {
        write_number_value(stream, neo.integer);
    }
}

/*
Format :
{
    1 octet : Type de pointeur
    2 Octets : Taille des données (X)
    X octets : [DONNEES]
} x nombre de pointeurs
*/

// Écriture de tous les pointeurs en remplaçant les pointeurs internes par les indices dans la ptrTable
void serialize_all_pointers(NeStream stream, intptrlist* ptrTable, intlist* typesTable) {
    // Écriture de la taille de la table
    write_number_value(stream, ptrTable->len);

    // Écriture de chaque pointeur de la table
    for (int i=0 ; i < ptrTable->len ; i++) {
        PointerType ptrType = typesTable->tab[i];
        PointerUnion ptr = (PointerUnion){.pointer = ptrTable->tab[i]};
        // Écriture du type des données
        NeStream_write(stream, &ptrType, 1);
        
        // Écriture des données du pointeur, différente pour chaque type de pointeur
        switch (ptrType & IGNORE_GC_PROPERTY) {
            case StringPtr:
            case CharStarPtr: {
                // Écriture de la taille des données du pointeur
                int size = strlen(ptr.string->string) + 1;
                write_number_value(stream, size);
                NeStream_write(stream, ptr.string->string, size);
                break;
            }

            case NeListPtr: {
                // Écriture du champ len de la NeList
                write_number_value(stream, ptr.nelist->len);

                // Écriture de chaque NeObject
                for (int j = 0 ; j < ptr.nelist->len ; j++) {
                    serialize_partial_neobject(stream, ptr.nelist->tab[j], ptrTable);
                }
                break;
            }

            case ContainerPtr: {
                // Écriture du type de Container
                write_number_value(stream, ptr.container->type);

                // Écriture de l'indice de la NeList
                intptr_t index = intptrlist_index(ptrTable, ptr.container->data);
                write_number_value(stream, index);
                break;
            }

            case TreeBufferPtr: {
                // Écriture du nombre de blocs
                write_number_value(stream, ptr.treebuffer->n_blocks);
                
                // Écriture de la taille de chaque bloc
                write_number_value(stream, ptr.treebuffer->block_size);

                // Écriture de la taille du TreeBuffer
                write_number_value(stream, ptr.treebuffer->size);

                // Écriture des données (size octets)
                
                
            }
        }
    }
}






void neobject_serialize(NeStream stream, NeObj neo) {
    // Table qui stocke quels pointeurs ont été sérialisés
    intptrlist ptrTable = intptrlist_create(0);
    // Table qui stocke les types des pointeurs de la table ptrTable
    intlist typesTable = intlist_create(0);
    update_ptr_table_obj(neo, &ptrTable, &typesTable);

    serialize_all_pointers(stream, &ptrTable, &typesTable);
    serialize_partial_neobject(stream, neo, &ptrTable);

    neon_free(ptrTable.tab);
    neon_free(typesTable.tab);
}





NeObj read_partial_neobject(NeStream stream) {
    NeObj obj;
    if (!NeStream_read(stream, &obj.type, 1)) {
        global_env->CODE_ERROR = 122;
        return NEO_VOID;
    }
    obj.integer = read_number_value(stream);
    return obj;
}


void read_all_pointers(NeStream stream, intptrlist* ptrTable, intlist* typesTable) {
    int tableSize = read_number_value(stream);
    return_on_error();

    for (int i = 0 ; i < tableSize ; i++) {
        PointerType ptrType;
        if (!NeStream_read(stream, &ptrType, 1)) {
            global_env->CODE_ERROR = 122;
            return;
        }

        switch (ptrType & IGNORE_GC_PROPERTY) {
            case StringPtr: {
                String* string = neon_malloc(sizeof(String));
                string->refc = 0; // Lors de sa création la chaîne n'est accessible depuis nulle part. Plus tard on la relie à d'autres objets, et là on augmente le compteur de références
                
                // Lecture de la taille de la chaîne de caractères
                int size = read_number_value(stream);
                return_on_error();

                // Lecture de la chaîne de caractères
                string->string = neon_malloc(size);
                if (!NeStream_read(stream, string->string, size)) {
                    string_destroy(string);
                    global_env->CODE_ERROR = 122;
                    return;
                }

                intptrlist_append(ptrTable, string);
                intlist_append(typesTable, StringPtr);
                break;
            }

            case NeListPtr: {
                int length = read_number_value(stream);
                return_on_error();

                NeList* list = nelist_create(length);
                list->refc = 0; // Lors de sa création la liste n'est accessible depuis nulle part. Plus tard on la relie à d'autres objets, et là on augmente le compteur de références

                // Ajoute la liste au garbage collector
                if (!(ptrType & GC_EXTERN))
                    neo_list_convert(list);
                
                int j=0;
                for (; j < length ; j++) {
                    list->tab[j] = read_partial_neobject(stream);
                    if_error {
                        neon_free(list->tab);
                        neon_free(list);
                        return;
                    }
                }

                intptrlist_append(ptrTable, list);
                intlist_append(typesTable, NeListPtr);
                break;
            }

            case ContainerPtr: {
                int type = read_number_value(stream);
                return_on_error();
                intptr_t index = read_number_value(stream);
                return_on_error();
                Container* c = container_create(type, (void*)index);
                c->refc = 0; // Lors de sa création le container n'est accessible depuis nulle part. Plus tard on la relie à d'autres objets, et là on augmente le compteur de références

                // Ajoute le container au garbage collector
                neo_container_convert(c);

                intptrlist_append(ptrTable, c);
                intlist_append(typesTable, ContainerPtr);
                break;
            }

            default: {
                global_env->CODE_ERROR = 122;
                return;
            }
        }
    }
}

void solve_pointers_list(NeList* list, intptrlist* ptrTable, intlist* typesTable) {
    for (int i=0 ; i < list->len ; i++) {
        solve_pointers(&list->tab[i], ptrTable, typesTable);
    }
}


/*
Restitue tous les pointeurs dans les objets et met à jour les compteurs de références
*/
void solve_pointers_aux(NeObj* neo, intptrlist* ptrTable, intlist* typesTable) {

    if (NEO_TYPE(*neo) & HEAP_ALLOCATED) {
        neo->refc_ptr = ptrTable->tab[neo->integer];
        // Incrémentation du compteur de références du pointeur que l'on vient d'écrire
        (*neo->refc_ptr) += 1;

        // L'objet est désormais complet

        if (NEO_TYPE(*neo) == TYPE_LIST && !ismarked(*neo)) {
            mark(*neo);
            solve_pointers_list(neo->nelist, ptrTable, typesTable);
        }
        else if (NEO_TYPE(*neo) == TYPE_CONTAINER && !ismarked(*neo)) {
            mark(*neo);
            neo->container->data = (NeList*)ptrTable->tab[(intptr_t)neo->container->data];
            solve_pointers_list(neo->container->data, ptrTable, typesTable);
        }
    }
}


void solve_pointers(NeObj *neo, intptrlist *ptrTable, intlist *typesTable) {
    solve_pointers_aux(neo, ptrTable, typesTable);
    // Démarque les objets marqués par solve_pointers_aux
    recursive_unmark(*neo);
}



NeObj neobject_deserialize(NeStream stream) {
    NeObj obj = NEO_VOID;

    // Table qui stocke quels pointeurs ont été sérialisés
    intptrlist ptrTable = intptrlist_create(0);
    // Table qui stocke les types des pointeurs de la table ptrTable
    intlist typesTable = intlist_create(0);
    read_all_pointers(stream, &ptrTable, &typesTable);
    if_error goto handle_error;
    
    obj = read_partial_neobject(stream);
    if_error goto handle_error;

    // Remplacement de tous les indices dans la table des pointeurs par les réels pointeurs
    solve_pointers(&obj, &ptrTable, &typesTable);

handle_error:
    neon_free(ptrTable.tab);
    neon_free(typesTable.tab);

    return obj;
}