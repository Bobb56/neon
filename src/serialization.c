#include "headers/serialization.h"
#include "headers/constants.h"
#include "headers/dynarrays.h"
#include "headers/neobj.h"
#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/errors.h"
#include "headers/sidememory.h"
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
            return UserFuncPtr;
        default: return -1;
    }
}



// Fonctions pour lire/écrire des entiers de manière indépendante de l'endianness de manière légèrement compressée

void write_number_value(NeStream stream, intptr_t signed_value) {
    // Cast sans changer la représentation
    uintptr_t value = *(uintptr_t*)&signed_value;

    uint8_t buffer[sizeof(intptr_t)] = {0};
    // Écriture de l'entier octet par octet dans un buffer de manière indépendante de l'endianness
    int i = 0;
    while (value != 0) {
        buffer[i] = value & 0xff; // On récupère les 8 bits de poids faible
        value = value >> 8; // On enlève les 8 bits de poids faible
        i++;
    }

    // Calcul du nombre d'octets utilisés
    uint8_t size = sizeof(intptr_t);
    while (size >= 1 && buffer[size-1] == 0) size--;

    // Écriture du nombre d'octets utilisés
    NeStream_write(stream, &size, 1);

    // Écriture du bon nombre d'octets dans le buffer
    NeStream_write(stream, buffer, size);
}




intptr_t read_number_value(NeStream stream) {
    // On récupère le nombre d'octets sur lequel est stocké l'entier
    uint8_t size;
    if (!NeStream_read(stream, &size, 1)) {
        global_env->CODE_ERROR = 122;
        return 0;
    }

    // On prend garde à ne pas lire plus que la taille d'un intptr_t
    if (size > sizeof(intptr_t))
        size = sizeof(intptr_t);

    // Lecture des octets de l'entier dans le buffer
    uint8_t buffer[sizeof(intptr_t)] = {0};
    if (!NeStream_read(stream, buffer, size)) {
        global_env->CODE_ERROR = 122;
        return 0;
    }
    
    // Reconstitution de l'entier de manière indépendante de l'endianness
    uintptr_t value = 0;
    for (int i = size-1 ; i >= 0 ; i--) {
        value = (value << 8) | buffer[i];
    }

    // Cast sans changer la représentation
    return *(intptr_t*)&value;
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
        else if (NEO_TYPE(obj) == TYPE_USERFUNC) {
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
                intlist_append(typesTable, NeListPtr | GC_EXTERN);
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




void serialize_partial_neobject_opt(NeStream stream, NeObj neo, intptrlist* ptrTable) {
    NeStream_write(stream, &neo.type, 1);
    if (NEO_TYPE(neo) & HEAP_ALLOCATED) {
        intptr_t index = intptrlist_index(ptrTable, neo.refc_ptr);
        write_number_value(stream, index);
    }
    else {
        write_number_value(stream, neo.integer);
    }
}


void serialize_partial_neobject(NeStream stream, NeObj neo, intptrlist* ptrTable) {
    NeStream_write(stream, &neo.type, 1);
    if (NEO_TYPE(neo) & HEAP_ALLOCATED) {
        neo.integer = intptrlist_index(ptrTable, neo.refc_ptr);
    }
    NeStream_write(stream, &neo.integer, sizeof(intptr_t));
}





// Cette fonction écrit exactement tb->size octets, donc aucune compression des indices de la table
void serialize_treebuffer_block(NeStream stream, TreeBuffer* tb, intptrlist* ptrTable) {
    void* buffer = neon_malloc(100); // Plus grand que n'importe quelle structure d'arbre (taille max atteinte avec FunctionDef sur Linux x86 = 40 octets)
    TreeBufferIndex index = 0;
    while (index < tb->size) {
        switch (BYTE(tb, index)) {
            case TypeTreeList: {
                // Calcul de la taille totale du bloc de la TreeList
                int treelist_size = sizeof(uint8_t) + sizeof(uint16_t) + sizeof(TreeBufferIndex) * treelistLength(tb, index);
                // Écriture de la TreeList telle quelle
                NeStream_write(stream, tb->pointer + index, treelist_size);
                index += treelist_size;
                break;
            }

            case TypeConst: {
                struct ConstObj* tree = treeConst(tb, index);
                memcpy(buffer, tree, sizeof(struct ConstObj));
                struct ConstObj* copy = buffer;
                
                if (NEO_TYPE(copy->obj) & HEAP_ALLOCATED) {
                    copy->obj.integer = intptrlist_index(ptrTable, copy->obj.refc_ptr);
                }

                NeStream_write(stream, copy, sizeof(struct ConstObj));
                index += sizeof(struct ConstObj);
                break;
            }

            case TypeFunctiondef: {
                struct FunctionDef* tree = treeFDef(tb, index);
                memcpy(buffer, tree, sizeof(struct FunctionDef));
                struct FunctionDef* copy = buffer;

                copy->name = (char*)(intptr_t)intptrlist_index(ptrTable, copy->name);
                if (NEO_TYPE(copy->object) & HEAP_ALLOCATED) {
                    copy->object.integer = intptrlist_index(ptrTable, copy->object.refc_ptr);
                }

                NeStream_write(stream, copy, sizeof(struct FunctionDef));
                index += sizeof(struct FunctionDef);
                break;
            }

            case TypeFunctioncall: {
                struct FunctionCall* tree = treeFCall(tb, index);
                memcpy(buffer, tree, sizeof(struct FunctionCall));
                struct FunctionCall* copy = buffer;

                if (NEO_TYPE(copy->function_obj) & HEAP_ALLOCATED) {
                    copy->function_obj.integer = intptrlist_index(ptrTable, copy->function_obj.refc_ptr);
                }

                NeStream_write(stream, copy, sizeof(struct FunctionCall));
                index += sizeof(struct FunctionCall);
                break;
            }

            case TypeAttribute: {
                struct Attribute* tree = treeAttr(tb, index);
                memcpy(buffer, tree, sizeof(struct Attribute));
                struct Attribute* copy = buffer;

                copy->name = (char*)(intptr_t)intptrlist_index(ptrTable, copy->name);

                NeStream_write(stream, copy, sizeof(struct Attribute));
                index += sizeof(struct Attribute);
                break;
            }

            case TypeAttributeLit: {
                struct AttributeLit* tree = treeAttrLit(tb, index);
                memcpy(buffer, tree, sizeof(struct AttributeLit));
                struct AttributeLit* copy = buffer;

                copy->name = (char*)(intptr_t)intptrlist_index(ptrTable, copy->name);

                NeStream_write(stream, copy, sizeof(struct AttributeLit));
                index += sizeof(struct AttributeLit);
                break;
            }

            case TypeParallelCall: {
                struct ParallelCall* tree = treeParCall(tb, index);
                memcpy(buffer, tree, sizeof(struct ParallelCall));
                struct ParallelCall* copy = buffer;

                copy->expr_buffer = (TreeBuffer*)(intptr_t)intptrlist_index(ptrTable, copy->expr_buffer);

                NeStream_write(stream, copy, sizeof(struct ParallelCall));
                index += sizeof(struct ParallelCall);
                break;
            }


            default: {
                int tree_size = type_size(TREE_TYPE(tb, index));
                NeStream_write(stream, tb->pointer + index, tree_size);
                index += tree_size;
                break;
            }
        }
    }
    neon_free(buffer);
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
                    serialize_partial_neobject_opt(stream, ptr.nelist->tab[j], ptrTable);
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

            case UserFuncPtr: {
                write_number_value(stream, ptr.userfunc->isMethod);
                write_number_value(stream, ptr.userfunc->nbArgs);
                write_number_value(stream, ptr.userfunc->nbOptArgs);
                write_number_value(stream, ptr.userfunc->unlimited_arguments);
                for (int i=0 ; i < ptr.userfunc->nbArgs ; i++) {
                    write_number_value(stream, ptr.userfunc->args[i]);
                }

                if (ptr.userfunc->doc == NULL) {
                    write_number_value(stream, -1);
                }
                else {
                    int size = strlen(ptr.userfunc->doc) + 1;
                    write_number_value(stream, size);
                    NeStream_write(stream, ptr.userfunc->doc, size);
                }
                
                NeStream_write(stream, &ptr.userfunc->code, sizeof(TreeBufferIndex));
                write_number_value(stream, intptrlist_index(ptrTable, ptr.userfunc->tree_buffer));
                write_number_value(stream, intptrlist_index(ptrTable, ptr.userfunc->opt_args));
                break;
            }

            case TreeBufferPtr: {
                // Écriture du nombre de blocs
                write_number_value(stream, ptr.treebuffer->n_blocks);
                
                // Écriture de la taille de chaque bloc
                write_number_value(stream, ptr.treebuffer->block_size);

                // Écriture de la taille du TreeBuffer
                write_number_value(stream, ptr.treebuffer->size);

                // Écriture de l'entry point
                NeStream_write(stream, &ptr.treebuffer->entry_point, sizeof(TreeBufferIndex));

                // Écriture des données (size octets)
                serialize_treebuffer_block(stream, ptr.treebuffer, ptrTable);
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
    serialize_partial_neobject_opt(stream, neo, &ptrTable);

    neon_free(ptrTable.tab);
    neon_free(typesTable.tab);
}





NeObj read_partial_neobject(NeStream stream) {
    NeObj obj = NEO_VOID;
    if (!NeStream_read(stream, &obj.type, 1)) {
        global_env->CODE_ERROR = 122;
        return NEO_VOID;
    }
    obj.integer = read_number_value(stream);
    return obj;
}


// On désérialise tous les objets, mais en gardant les références aux autres objets comme des indices dans la table des pointeurs
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

            case CharStarPtr: {
                // Lecture de la taille de la chaîne de caractères
                int size = read_number_value(stream);
                return_on_error();

                // Lecture de la chaîne de caractères
                char* string = neon_malloc(size);
                if (!NeStream_read(stream, string, size)) {
                    neon_free(string);
                    global_env->CODE_ERROR = 122;
                    return;
                }

                intptrlist_append(ptrTable, string);
                intlist_append(typesTable, CharStarPtr);
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

            case TreeBufferPtr: {
                // Lecture du nombre de blocs
                int n_blocks = read_number_value(stream);
                return_on_error();

                // Lecture de la taille de chaque bloc
                int block_size = read_number_value(stream);
                return_on_error();

                // Lecture de la taille du TreeBuffer
                int size = read_number_value(stream);
                return_on_error();


                // Lecture de l'entry point
                TreeBufferIndex entry_point;
                if (!NeStream_read(stream, &entry_point, sizeof(TreeBufferIndex))) {
                    global_env->CODE_ERROR = 122;
                    return;
                }

                
                // Lecture des données (size octets)
                void* pointer = neon_malloc(n_blocks * block_size);
                return_on_error();

                if (!NeStream_read(stream, pointer, size)) {
                    global_env->CODE_ERROR = 122;
                    neon_free(pointer);
                    return;
                }

                
                TreeBuffer* tb = neon_malloc(sizeof(TreeBuffer));
                ptrlist_append(global_env->TREEBUFFERS, tb);

                if_error {
                    neon_free(pointer);
                    return;
                }

                tb->n_blocks = n_blocks;
                tb->block_size = block_size;
                tb->size = size;
                tb->side_memory = false;
                tb->locked = false; // On se sert du champ locked pour savoir si on a résolu les pointeurs internes du TreeBuffer
                tb->pointer = pointer;
                tb->entry_point = entry_point;
                
                intptrlist_append(ptrTable, tb);
                intlist_append(typesTable, TreeBufferPtr);
                break;
            }

            case UserFuncPtr: {
                bool isMethod = read_number_value(stream);
                return_on_error();

                int nbArgs = read_number_value(stream);
                return_on_error();

                int nbOptArgs = read_number_value(stream);
                return_on_error();

                bool unlimited_arguments = read_number_value(stream);
                return_on_error();

                // Lecture des arguments
                Var* args = neon_malloc(sizeof(Var) * nbArgs);
                return_on_error();

                for (int i=0 ; i < nbArgs ; i++) {
                    args[i] = read_number_value(stream);
                    if_error {
                        neon_free(args);
                        return;
                    }
                }

                // Lecture de la doc de la fonction
                int size = read_number_value(stream);
                if_error {
                    neon_free(args);
                    return;
                }

                char* doc = NULL;

                if (size != -1) {
                    doc = neon_malloc(sizeof(char) * size);
                    if_error {
                        neon_free(args);
                        return;
                    }

                    if (!NeStream_read(stream, doc, size)) {
                        global_env->CODE_ERROR = 122;
                        neon_free(args);
                        neon_free(doc);
                        return;
                    }
                }

                TreeBufferIndex code;
                if (!NeStream_read(stream, &code, sizeof(TreeBufferIndex))) {
                    global_env->CODE_ERROR = 122;
                    neon_free(args);
                    neon_free(doc);
                    return;
                }

                intptr_t tree_buffer = read_number_value(stream);
                if_error {
                    neon_free(doc);
                    neon_free(args);
                    return;
                }

                intptr_t opt_args = read_number_value(stream);
                if_error {
                    neon_free(doc);
                    neon_free(args);
                    return;
                }

                UserFunc* fun = neon_malloc(sizeof(UserFunc));
                if_error {
                    neon_free(doc);
                    neon_free(args);
                    return;
                }

                fun->isMethod = isMethod;
                fun->runningInstances = 0;
                fun->myCopy = NULL;
                fun->next = NEO_VOID;
                fun->prev = NEO_VOID;
                fun->args = args;
                fun->nbArgs = nbArgs;
                fun->nbOptArgs = nbOptArgs;
                fun->opt_args = (void*)opt_args;
                fun->code = code;
                fun->doc = doc;
                fun->unlimited_arguments = unlimited_arguments;
                fun->tree_buffer = (void*)tree_buffer;
                fun->refc = 0;

                // Ajout de la fonction au garbage collector
                neo_userfunc_convert(fun);

                intptrlist_append(ptrTable, fun);
                intlist_append(typesTable, UserFuncPtr);
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
        solve_pointers_aux(&list->tab[i], ptrTable, typesTable);
    }
}


void solve_pointers_treebuffer(TreeBuffer* tb, intptrlist* ptrTable, intlist* typesTable) {
    TreeBufferIndex index = 0;
    while (index < tb->size) {
        switch (BYTE(tb, index)) {

            case TypeTreeList: {
                index += sizeof(uint8_t) + sizeof(uint16_t) + sizeof(TreeBufferIndex) * treelistLength(tb, index);
                break;
            }


            case TypeConst: {
                struct ConstObj* tree = treeConst(tb, index);
                solve_pointers_aux(&tree->obj, ptrTable, typesTable);
                index += sizeof(struct ConstObj);
                break;
            }

            case TypeFunctiondef: {
                struct FunctionDef* tree = treeFDef(tb, index);
                tree->name = (char*)ptrTable->tab[(intptr_t)tree->name];
                solve_pointers_aux(&tree->object, ptrTable, typesTable);
                index += sizeof(struct FunctionDef);
                break;
            }

            case TypeFunctioncall: {
                struct FunctionCall* tree = treeFCall(tb, index);
                solve_pointers_aux(&tree->function_obj, ptrTable, typesTable);
                index += sizeof(struct FunctionCall);
                break;
            }

            case TypeAttribute: {
                struct Attribute* tree = treeAttr(tb, index);
                tree->name = (char*)ptrTable->tab[(intptr_t)tree->name];
                index += sizeof(struct Attribute);
                break;
            }

            case TypeAttributeLit: {
                struct AttributeLit* tree = treeAttrLit(tb, index);
                tree->name = (char*)ptrTable->tab[(intptr_t)tree->name];
                index += sizeof(struct AttributeLit);
                break;
            }

            case TypeParallelCall: {
                struct ParallelCall* tree = treeParCall(tb, index);
                tree->expr_buffer = (TreeBuffer*)ptrTable->tab[(intptr_t)tree->expr_buffer];
                index += sizeof(struct ParallelCall);
                break;
            }

            default: {
                index += type_size(TREE_TYPE(tb, index));
                break;
            }
        }
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
        else if ((NEO_TYPE(*neo) == TYPE_USERFUNC) && !ismarked(*neo)) {
            mark(*neo);
            neo->userfunc->opt_args = (NeList*)ptrTable->tab[(intptr_t)neo->userfunc->opt_args];
            solve_pointers_list(neo->userfunc->opt_args, ptrTable, typesTable);
            neo->userfunc->tree_buffer = (TreeBuffer*)ptrTable->tab[(intptr_t)neo->userfunc->tree_buffer];

            // On se sert du champ locked pour savoir si on a résolu les pointeurs internes du TreeBuffer
            if (!neo->userfunc->tree_buffer->locked) {
                neo->userfunc->tree_buffer->locked = true;
                solve_pointers_treebuffer(neo->userfunc->tree_buffer, ptrTable, typesTable);
                move_treebuffer_to_side_memory(neo->userfunc->tree_buffer);
            }
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