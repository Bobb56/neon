#define NEON_SOURCE_ID 18

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
#include "headers/nativefunctions.h"

#include <stdint.h>
#include <string.h>


/*
TODO
- Il faut écrire la structure des types de Containers utilisés en en-tête du fichier, ainsi que les noms de variables utilisés et les types d'exceptions utilisés

Procédé lors de la lecture de l'en-tête :
Containers :
------------
Fabrication d'une liste d'association type de container du fichier -> type de container de l'environnement
En créant les types de containers nécessaires si inexistants


Variables :
------------
Fabrication d'une liste d'association variable du fichier -> variable de l'environnement
En créant les nouvelles variables avec les bons noms si besoin

Exceptions :
------------
Fabrication d'une liste d'association code exception du fichier -> code exception de l'environnement
En créant les nouvelles exceptions si besoin

TODO : Il faut que dans les fonctions d'écriture finale des objets, au même niveau que celui où on remplace les pointeurs internes des objets par des indices de la table des pointeurs, on remplace les indices de variables, les exceptions et les types de containers par leurs indices dans la table des containers, variables, exceptions


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





bool hasPointer(NeObj obj) {
    return (NEO_TYPE(obj) & HEAP_ALLOCATED) && NEO_TYPE(obj) != TYPE_PROMISE;
}


PointerType getPointerType(NeObj neo) {
    switch (NEO_TYPE(neo)) {
        case TYPE_STRING: return StringPtr;
        case TYPE_BUILTINFUNC: return FunctionPtr;
        case TYPE_LIST: return NeListPtr;
        case TYPE_CONTAINER: return ContainerPtr;
        case TYPE_USERFUNC:
        case TYPE_PARTIALFUNC: // Les partial func utilisent le même code pour leur sérialisation
            return UserFuncPtr;
        default:
            neon_internal_error();
            return -1;
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
        neon_fail(122, NO_ARGS);
        return 0;
    }

    // On prend garde à ne pas lire plus que la taille d'un intptr_t
    if (size > sizeof(intptr_t))
        size = sizeof(intptr_t);

    // Lecture des octets de l'entier dans le buffer
    uint8_t buffer[sizeof(intptr_t)] = {0};
    if (!NeStream_read(stream, buffer, size)) {
        neon_fail(122, NO_ARGS);
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


void write_string_value(NeStream stream, char* string) {
    int size = strlen(string) + 1;
    write_number_value(stream, size);
    NeStream_write(stream, string, size);
}

char* read_string_value(NeStream stream) {
    int size = read_number_value(stream);
    return_on_error(NULL);

    // Lecture de la chaîne de caractères
    char* string = neon_malloc(size);
    return_on_error(NULL);
    
    if (!NeStream_read(stream, string, size)) {
        neon_free(string);
        neon_fail(122, NO_ARGS);
        return NULL;
    }
    return string;
}


// Écrit le header contenant les informations sur les types de containers de containersTable
void write_containers_header(NeStream stream, intlist* containersTable) {
    // Écriture de la taille de la table
    write_number_value(stream, containersTable->len);

    // Écriture de chaque élément de la table
    for (int i=0 ; i < containersTable->len ; i++) {
        int container_type = containersTable->tab[i];
        NeList* attributes = neo_to_list(global_env->ATTRIBUTES->tab[container_type]);

        // Écriture du nom du container
        write_string_value(stream, global_env->CONTAINERS->tab[container_type]);

        // Écriture du nombre de champs du container
        write_number_value(stream, attributes->len);

        // Écriture de chaque nom de champ du container
        for (int j=0 ; j < attributes->len ; j++) {
            write_string_value(stream, neo_to_string(attributes->tab[j]));
        }
    }
}

// Lit le header des containers, met à jour les containers de l'environnement courant et écrit la containersTable pour faire correspondre les types de containers de la table des containers, et ceux de l'environnement
void read_containers_header(NeStream stream, intlist* containersTable) {
    int tableSize = read_number_value(stream);

    for (int i=0 ; i < tableSize ; i++) {
        char* containerName = read_string_value(stream);
        int nb_fields = read_number_value(stream);

        // On regarde si ce type de container existe déjà
        int container_index = strlist_index(global_env->CONTAINERS, containerName);
        // Le type de container n'existe pas, donc on va le créer
        if (container_index == -1) {
            neon_reset_error();
            // Ajout du type de container à la table des containers
            intlist_append(containersTable, global_env->CONTAINERS->len);

            // Ajout du nom du container
            strlist_append(global_env->CONTAINERS, containerName);

            // Création de la liste d'attributs
            NeList* attributes = nelist_create(nb_fields);
            for (int j=0 ; j < nb_fields ; j++) {
                attributes->tab[j] = neo_str_create(read_string_value(stream));
            }

            // Ajout de la liste d'attributs aux attributs de ce container
            nelist_append(global_env->ATTRIBUTES, gc_extern_neo_list_convert(attributes));
        }
        else if (nb_fields != neo_list_len(global_env->ATTRIBUTES->tab[container_index])) {
            neon_fail(32, NO_ARGS);
            neon_free(containerName);
            return;
        }
        else {
            // Vérification de la correspondance exacte des champs
            for (int j=0 ; j < nb_fields ; j++) {
                char* field = read_string_value(stream);

                bool field_is_present = strcmp(neo_to_string(neo_list_nth(global_env->ATTRIBUTES->tab[container_index], j)), field) == 0;
                neon_free(field);

                if (!field_is_present) {
                    neon_fail(32, NO_ARGS);
                    neon_free(containerName);
                    return;
                }
            }

            neon_free(containerName);
            // Tous les champs sont présents et il y a le même nombre de champs donc les containers correspondent
            // Donc on ajoute l'indice du container déjà existant dans la table des containers
            intlist_append(containersTable, container_index);
        }
    }
}






void write_exceptions_header(NeStream stream, intlist* exceptionsTable) {
    // Écriture de la taille de la table
    write_number_value(stream, exceptionsTable->len);
    
    for (int i=0 ; i < exceptionsTable->len ; i++) {
        int exceptionCode = exceptionsTable->tab[i];

        // Écriture du nom de l'exception
        write_string_value(stream, global_env->EXCEPTIONS->tab[exceptionCode]);
    }
}


void read_exceptions_header(NeStream stream, intlist* exceptionsTable) {
    int tableSize = read_number_value(stream);

    for (int i=0 ; i < tableSize ; i++) {
        char* exception = read_string_value(stream);
        return_on_error();

        int exception_index = strlist_index(global_env->EXCEPTIONS, exception);

        // L'exception n'existe pas encore
        if (exception_index == -1) {
            neon_reset_error();
            // Ajout de la nouvelle exception à la table des exceptions
            intlist_append(exceptionsTable, global_env->EXCEPTIONS->len);
            // Ajout de la nouvelle exception à l'environnement
            strlist_append(global_env->EXCEPTIONS, exception);
        }
        else {
            neon_free(exception);
            // Mise à jour de la table d'exceptions
            intlist_append(exceptionsTable, exception_index);
        }
    }
}


void write_variables_header(NeStream stream, intlist* varsTable) {
    // Écriture de la taille de la table
    write_number_value(stream, varsTable->len);

    for (int i=0 ; i < varsTable->len ; i++) {
        Var var = varsTable->tab[i];
        // Écriture du nom de la variable
        write_string_value(stream, global_env->NOMS->tab[var]);
    }
}



void read_variables_header(NeStream stream, intlist* variablesTable) {
    int tableSize = read_number_value(stream);

    for (int i=0 ; i < tableSize ; i++) {
        char* varName = read_string_value(stream);

        int var_index = strlist_index(global_env->NOMS, varName);

        // La variable n'existe pas encore
        if (var_index == -1) {
            neon_reset_error();
            // Ajout de la nouvelle variable à la table des variables
            intlist_append(variablesTable, global_env->NOMS->len);

            // Ajout de la nouvelle variable à l'environnement
            strlist_append(global_env->NOMS, varName);
            nelist_append(global_env->ADRESSES, neo_empty_create());
        }
        else {
            neon_free(varName);
            // Mise à jour de la table des variables
            intlist_append(variablesTable, var_index);
        }
    }
}



/*
Cette fonction parcourt un objet et ajoute à la table des pointeurs tous les pointeurs internes de cet objet
*/
void update_ptr_table_obj(NeObj obj, intptrlist* ptrTable, intlist* typesTable, intlist* containers, intlist* vars, intlist* expt) {
    if (hasPointer(obj) && intptrlist_index(ptrTable, obj.refc_ptr) == -1) {
        intlist_append(typesTable, getPointerType(obj));
        intptrlist_append(ptrTable, obj.refc_ptr);

        if (NEO_TYPE(obj) == TYPE_LIST) {
            update_ptr_table_list(obj.nelist, ptrTable, typesTable, containers, vars, expt);
        }
        else if (NEO_TYPE(obj) == TYPE_CONTAINER) {
            // Mise à jour de la liste des containers utilisés
            if (!intlist_inList(containers, obj.container->type))
                intlist_append(containers, obj.container->type);

            // Ajout du pointeur de la nelist
            intlist_append(typesTable, ContainerNeListPtr);
            intptrlist_append(ptrTable, obj.container->data);
            update_ptr_table_list(obj.container->data, ptrTable, typesTable, containers, vars, expt);
        }
        else if (NEO_TYPE(obj) == TYPE_USERFUNC || NEO_TYPE(obj) == TYPE_PARTIALFUNC) {
            // Mise à jour de la liste des variables utilisées
            for (int i=0 ; i < obj.userfunc->nbArgs ; i++) {
                if (!intlist_inList(vars, obj.userfunc->args[i]))
                    intlist_append(vars, obj.userfunc->args[i]);
            }

            // Ajout du TreeBuffer à la liste des pointeurs
            TreeBuffer* tb = obj.userfunc->tree_buffer;
            if (intptrlist_index(ptrTable, tb) == -1) {
                intlist_append(typesTable, TreeBufferPtr);
                intptrlist_append(ptrTable, tb);
            }
            
            // Update the pointer table with some pointers that are not stored directly in the TreeBuffer (the NeObjects for instance)
            void* args[] = {
                (void*)ptrTable,
                (void*)typesTable,
                (void*)containers,
                (void*)vars,
                (void*)expt
            };
            TreeBuffer_iter(tb, NeTree_update_ptr_table, args);

            // Ajout des arguments optionnels
            // Si on est sur une partialfunc, opt_args est à NULL
            NeList* opt_args = obj.userfunc->opt_args;
            if (opt_args != NULL && intptrlist_index(ptrTable, opt_args) == -1) {
                intlist_append(typesTable, UserFuncOptArgsPtr);
                intptrlist_append(ptrTable, opt_args);
                update_ptr_table_list(opt_args, ptrTable, typesTable, containers, vars, expt);
            }
        }
    }
    else if (NEO_TYPE(obj) == TYPE_EXCEPTION) {
        // Mise à jour de la liste des exceptions utilisées
        int code = get_exception_code(obj);
        if (!intlist_inList(expt, code))
            intlist_append(expt, code);
    }
}

void update_ptr_table_list(NeList* list, intptrlist* ptrTable, intlist* typesTable, intlist* containers, intlist* vars, intlist* expt) {
    for (int i=0 ; i < list->len ; i++) {
        update_ptr_table_obj(list->tab[i], ptrTable, typesTable, containers, vars, expt);
    }
}


/*
Cette fonction ajoute à la table des pointeurs tous les pointeurs internes d'un TreeBuffer
*/
void NeTree_update_ptr_table(TreeBuffer* tb, TreeBufferIndex tree, void* args) {
    intptrlist* ptrTable = ((void**)args)[0];
    intlist* typesTable = ((void**)args)[1];
    intlist* containers = ((void**)args)[2];
    intlist* vars = ((void**)args)[3];
    intlist* expt = ((void**)args)[4];

    if (TREE_ISVOID(tree))
        return;
    
    switch (TREE_TYPE(tb, tree)) {
        case TypeConst:
            update_ptr_table_obj(treeConst(tb, tree)->obj, ptrTable, typesTable, containers, vars, expt);
            break;
        
        case TypeVariable:
            if (!intlist_inList(vars, treeVar(tb, tree)->var))
                intlist_append(vars, treeVar(tb, tree)->var);
            break;
        
        case TypeFunctiondef:
            intptrlist_append(ptrTable, treeFDef(tb, tree)->name);
            intlist_append(typesTable, CharStarPtr);

            update_ptr_table_obj(treeFDef(tb, tree)->object, ptrTable, typesTable, containers, vars, expt);
            break;
        
        case TypeFunctioncall:
            update_ptr_table_obj(treeFCall(tb, tree)->function_obj, ptrTable, typesTable, containers, vars, expt);
            break;
        
        case TypeAttribute:
            intptrlist_append(ptrTable, treeAttr(tb, tree)->name);
            intlist_append(typesTable, CharStarPtr);
            break;
        
        case TypeContainerLit:
            if (!intlist_inList(containers, treeContLit(tb, tree)->container_type))
                intlist_append(containers, treeContLit(tb, tree)->container_type);
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



/*
Écrit un NeObj dans le stream en remplaçant la valeur du pointeur par son indice dans la table s'il s'agit d'un pointeur
La représentation ne correspond pas à la représentation réelle d'un NeObj en mémoire (elle peut être plus courte)
*/


// Modifie les champs internes du NeObj pour pouvoir être sérialisé (remplacement des pointeurs par des indices, et rescaling des indices d'exceptions pour correspondre à ceux qui seront écrits dans le fichier)
NeObj process_neobj_for_serialization(NeObj obj, intptrlist* ptrTable, intlist* exceptions) {
    if (hasPointer(obj)) {
        obj.integer = intptrlist_index(ptrTable, obj.refc_ptr);
    }
    else if (NEO_TYPE(obj) == TYPE_EXCEPTION) {
        // On n'écrit pas directement le type d'exception, mais son indice dans la table d'exceptions
        int index = intlist_index(exceptions, get_exception_code(obj));
        neon_assert(index >= 0, NEO_VOID);
        obj.integer = index;
    }

    return obj;
}

// Fonction inverse
NeObj process_neobj_for_deserialization(NeObj obj, intptrlist* ptrTable, intlist* exceptions) {
    if (hasPointer(obj)) {
        obj.refc_ptr = ptrTable->tab[obj.integer];
    }
    else if (NEO_TYPE(obj) == TYPE_EXCEPTION) {
        obj.integer = exceptions->tab[obj.integer];
    }

    return obj;
}



void serialize_partial_neobject_opt(NeStream stream, NeObj neo, intptrlist* ptrTable, intlist* exceptions) {
    NeObj processed_neo = process_neobj_for_serialization(neo, ptrTable, exceptions);
    NeStream_write(stream, &processed_neo.type, 1);
    write_number_value(stream, processed_neo.integer);
}


/*
Écrit un NeObj dans le stream en remplaçant la valeur du pointeur par son indice dans la table s'il s'agit d'un pointeur
L'objet écrit correspond exactement à un NeObj
*/
void serialize_partial_neobject(NeStream stream, NeObj neo, intptrlist* ptrTable, intlist* exceptions) {
    NeObj processed_neo = process_neobj_for_serialization(neo, ptrTable, exceptions);
    NeStream_write(stream, &processed_neo.type, 1);
    NeStream_write(stream, &processed_neo.integer, sizeof(intptr_t));
}




// Fonction qui écrit entièrement le bloc de données d'un TreeBuffer, en remmplaçant les pointeurs internes par les indices dans la table
// Cette fonction écrit exactement tb->size octets, donc aucune compression des indices de la table
void serialize_treebuffer_block(NeStream stream, TreeBuffer* tb, intptrlist* ptrTable, intlist* containers, intlist* vars, intlist* expt) {
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
                
                copy->obj = process_neobj_for_serialization(copy->obj, ptrTable, expt);

                NeStream_write(stream, copy, sizeof(struct ConstObj));
                index += sizeof(struct ConstObj);
                break;
            }

            case TypeVariable: {
                struct Variable* tree = treeVar(tb, index);
                memcpy(buffer, tree, sizeof(struct Variable));
                struct Variable* copy = buffer;
                
                copy->var = intlist_index(vars, copy->var);

                NeStream_write(stream, copy, sizeof(struct Variable));
                index += sizeof(struct Variable);
                break;
            }

            case TypeFunctiondef: {
                struct FunctionDef* tree = treeFDef(tb, index);
                memcpy(buffer, tree, sizeof(struct FunctionDef));
                struct FunctionDef* copy = buffer;

                copy->name = (char*)(intptr_t)intptrlist_index(ptrTable, copy->name);
                copy->object = process_neobj_for_serialization(copy->object, ptrTable, expt);

                NeStream_write(stream, copy, sizeof(struct FunctionDef));
                index += sizeof(struct FunctionDef);
                break;
            }

            case TypeFunctioncall: {
                struct FunctionCall* tree = treeFCall(tb, index);
                memcpy(buffer, tree, sizeof(struct FunctionCall));
                struct FunctionCall* copy = buffer;

                copy->function_obj = process_neobj_for_serialization(copy->function_obj, ptrTable, expt);

                NeStream_write(stream, copy, sizeof(struct FunctionCall));
                index += sizeof(struct FunctionCall);
                break;
            }

            case TypeAttribute: {
                struct Attribute* tree = treeAttr(tb, index);
                memcpy(buffer, tree, sizeof(struct Attribute));
                struct Attribute* copy = buffer;

                copy->name = (char*)(intptr_t)intptrlist_index(ptrTable, copy->name);
                copy->index = -1; // On invalide l'indice de l'attribut et le type de container

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

            case TypeContainerLit: {
                struct ContainerLit* tree = treeContLit(tb, index);
                memcpy(buffer, tree, sizeof(struct ContainerLit));
                struct ContainerLit* copy = buffer;

                // On rescale le type de container avec celui qui est écrit dans la table
                copy->container_type = intlist_index(containers, copy->container_type);

                NeStream_write(stream, copy, sizeof(struct ContainerLit));
                index += sizeof(struct ContainerLit);
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
Number : Taille de la table
{
    1 octet : Type de pointeur
    ? octets : [DONNEES]
} x nombre de pointeurs
*/

// Écriture de la table des pointeurs en remplaçant les pointeurs internes par les indices dans la ptrTable
void serialize_all_pointers(NeStream stream, intptrlist* ptrTable, intlist* typesTable, intlist* containers, intlist* vars, intlist* exceptions) {
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

            case CharStarPtr: {
                // Écriture de la taille des données du pointeur
                write_string_value(stream, ptr.charstar);
                break;
            }

            case StringPtr: {
                // Écriture de la taille des données du pointeur
                write_string_value(stream, ptr.string->string);
                break;
            }

            case NeListPtr: {
                // Écriture du champ len de la NeList
                write_number_value(stream, ptr.nelist->len);

                // Écriture de chaque NeObject
                for (int j = 0 ; j < ptr.nelist->len ; j++) {
                    serialize_partial_neobject_opt(stream, ptr.nelist->tab[j], ptrTable, exceptions);
                }
                break;
            }

            case ContainerPtr: {
                // Écriture du type de Container
                int ctnr_index = intlist_index(containers, ptr.container->type);
                neon_assert(ctnr_index >= 0,);
                write_number_value(stream, ctnr_index);

                // Écriture de l'indice de la NeList
                intptr_t list_index = intptrlist_index(ptrTable, ptr.container->data);
                write_number_value(stream, list_index);
                break;
            }

            case UserFuncPtr: {
                write_number_value(stream, ptr.userfunc->isMethod);
                write_number_value(stream, ptr.userfunc->nbArgs);
                write_number_value(stream, ptr.userfunc->nbOptArgs);
                write_number_value(stream, ptr.userfunc->unlimited_arguments);

                // Écriture des variables arguments
                for (int i=0 ; i < ptr.userfunc->nbArgs ; i++) {
                    int variable_index = intlist_index(vars, ptr.userfunc->args[i]);
                    write_number_value(stream, variable_index);
                }

                if (ptr.userfunc->doc == NULL) {
                    write_number_value(stream, -1);
                }
                else {
                    write_string_value(stream, ptr.userfunc->doc);
                }
                
                NeStream_write(stream, &ptr.userfunc->code, sizeof(TreeBufferIndex));
                write_number_value(stream, intptrlist_index(ptrTable, ptr.userfunc->tree_buffer));
                // Dans le cas où on sérialise une PartialFunc, on écrit un -1
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
                serialize_treebuffer_block(stream, ptr.treebuffer, ptrTable, containers, vars, exceptions);
                break;
            }

            case FunctionPtr: {
                // Écriture de l'id de la fonction
                write_number_value(stream, ptr.function->id);

                // Écriture du module de la fonction
                write_number_value(stream, ptr.function->module);
                break;
            }
        }
    }
}




/*
Fonction pour entièrement sérialiser un objet Neon
*/
void neobject_serialize(NeStream stream, NeObj neo) {
    // Table qui stocke quels pointeurs ont été sérialisés
    intptrlist ptrTable = intptrlist_create(0);

    // Table qui stocke les types des pointeurs de la table ptrTable
    intlist typesTable = intlist_create(0);

    // Table qui stocke tous les types de containers utilisés par l'objet à sérialiser
    intlist containersTable = intlist_create(0);

    // Table qui stocke toutes les variables de l'environnement actuel utilisées par l'objet à sérialiser
    intlist varsTable = intlist_create(0);

    // Table qui stocke toutes les exceptions utilisées par l'objet à sérialiser
    intlist exceptionsTable = intlist_create(0);

    // On construit la table de tous les pointeurs (les sommets du graphe des objets)
    update_ptr_table_obj(neo, &ptrTable, &typesTable, &containersTable, &varsTable, &exceptionsTable);

    // Écriture des headers
    write_variables_header(stream, &varsTable);
    write_containers_header(stream, &containersTable);
    write_exceptions_header(stream, &exceptionsTable);

    // On écrit chaque objet (chaque sommet dans le graphe des objets) en remplaçant les références à d'autres objets par les indices de ces objets dans la table des pointeurs
    serialize_all_pointers(stream, &ptrTable, &typesTable, &containersTable, &varsTable, &exceptionsTable);
    serialize_partial_neobject_opt(stream, neo, &ptrTable, &exceptionsTable);

    neon_free(varsTable.tab);
    neon_free(containersTable.tab);
    neon_free(exceptionsTable.tab);
    neon_free(ptrTable.tab);
    neon_free(typesTable.tab);
}




/*
Fonction inverse de serialize_partial_neobject
*/
NeObj read_partial_neobject(NeStream stream) {
    NeObj obj = NEO_VOID;
    if (!NeStream_read(stream, &obj.type, 1)) {
        neon_fail(122, NO_ARGS);
        return NEO_VOID;
    }
    obj.integer = read_number_value(stream);
    return obj;
}


// On désérialise tous les objets de la table des pointeurs, mais en gardant les références aux autres objets comme des indices dans la table des pointeurs
void read_all_pointers(NeStream stream, intptrlist* ptrTable, intlist* typesTable, intlist* containers, intlist* vars, intlist* expt) {
    int tableSize = read_number_value(stream);
    return_on_error();

    for (int i = 0 ; i < tableSize ; i++) {
        PointerType ptrType;
        if (!NeStream_read(stream, &ptrType, 1)) {
            neon_fail(122, NO_ARGS);
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
                    neon_fail(122, NO_ARGS);
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
                    neon_fail(122, NO_ARGS);
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
                int type = containers->tab[read_number_value(stream)];
                return_on_error();
                intptr_t index = read_number_value(stream);
                return_on_error();
                Container* c = container_create(type, (void*)index);
                c->refc = 0; // Lors de sa création le container n'est accessible depuis nulle part. Plus tard on la relie à d'autres objets, et là on augmente le compteur de références

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
                    neon_fail(122, NO_ARGS);
                    return;
                }

                
                // Lecture des données (size octets)
                void* pointer = neon_malloc(n_blocks * block_size);
                return_on_error();

                if (!NeStream_read(stream, pointer, size)) {
                    neon_fail(122, NO_ARGS);
                    neon_free(pointer);
                    return;
                }

                
                TreeBuffer* tb = neon_malloc(sizeof(TreeBuffer));

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
                    args[i] = vars->tab[read_number_value(stream)];
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
                        neon_fail(122, NO_ARGS);
                        neon_free(args);
                        neon_free(doc);
                        return;
                    }
                }

                TreeBufferIndex code;
                if (!NeStream_read(stream, &code, sizeof(TreeBufferIndex))) {
                    neon_fail(122, NO_ARGS);
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

                // Dans le cas où on est sur une partial func, on lit un -1
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

                intptrlist_append(ptrTable, fun);
                intlist_append(typesTable, UserFuncPtr);
                break;
            }

            case FunctionPtr: {
                int id = read_number_value(stream);
                return_on_error();

                int module = read_number_value(stream);
                return_on_error();

                NeObj fun = get_function(id, module);
                *(fun.refc_ptr) = 0;

                intptrlist_append(ptrTable, neo_to_function(fun));
                intlist_append(typesTable, FunctionPtr);
                break;
            }

            default: {
                neon_fail(122, NO_ARGS);
                return;
            }
        }
    }
}

void solve_pointers_list(NeList* list, intptrlist* ptrTable, intlist* typesTable, intlist* containers, intlist* vars, intlist* expt) {
    for (int i=0 ; i < list->len ; i++) {
        solve_pointers_aux(&list->tab[i], ptrTable, typesTable, containers, vars, expt);
    }
}


void solve_pointers_treebuffer(TreeBuffer* tb, intptrlist* ptrTable, intlist* typesTable, intlist* containers, intlist* vars, intlist* expt) {
    TreeBufferIndex index = 0;
    while (index < tb->size) {
        switch (BYTE(tb, index)) {

            case TypeTreeList: {
                index += sizeof(uint8_t) + sizeof(uint16_t) + sizeof(TreeBufferIndex) * treelistLength(tb, index);
                break;
            }

            case TypeVariable: {
                struct Variable* tree = treeVar(tb, index);
                tree->var = vars->tab[tree->var];
                index += sizeof(struct Variable);
                break;
            }


            case TypeConst: {
                struct ConstObj* tree = treeConst(tb, index);
                solve_pointers_aux(&tree->obj, ptrTable, typesTable, containers, vars, expt);
                index += sizeof(struct ConstObj);
                break;
            }

            case TypeFunctiondef: {
                struct FunctionDef* tree = treeFDef(tb, index);
                tree->name = (char*)ptrTable->tab[(intptr_t)tree->name];
                solve_pointers_aux(&tree->object, ptrTable, typesTable, containers, vars, expt);
                index += sizeof(struct FunctionDef);
                break;
            }

            case TypeFunctioncall: {
                struct FunctionCall* tree = treeFCall(tb, index);
                solve_pointers_aux(&tree->function_obj, ptrTable, typesTable, containers, vars, expt);
                index += sizeof(struct FunctionCall);
                break;
            }

            case TypeAttribute: {
                struct Attribute* tree = treeAttr(tb, index);
                tree->name = (char*)ptrTable->tab[(intptr_t)tree->name];
                index += sizeof(struct Attribute);
                break;
            }

            case TypeContainerLit: {
                struct ContainerLit* tree = treeContLit(tb, index);
                tree->container_type = containers->tab[tree->container_type];
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
Restitue tous les pointeurs dans les objets, met à jour les compteurs de références, et ajoute les objets finaux au Garbage Collector
*/
void solve_pointers_aux(NeObj* neo, intptrlist* ptrTable, intlist* typesTable, intlist* containers, intlist* vars, intlist* expt) {
    if (hasPointer(*neo)) {
        *neo = process_neobj_for_deserialization(*neo, ptrTable, expt);
        // Incrémentation du compteur de références du pointeur que l'on vient d'écrire
        (*neo->refc_ptr) += 1;

        // L'objet est désormais complet

        if (NEO_TYPE(*neo) == TYPE_LIST && !ismarked(*neo)) {
            mark(*neo);
            solve_pointers_list(neo->nelist, ptrTable, typesTable, containers, vars, expt);
            // Ajout de l'objet au Garbage Collector
            neo_list_convert(neo->nelist);
        }
        else if (NEO_TYPE(*neo) == TYPE_CONTAINER && !ismarked(*neo)) {
            mark(*neo);
            neo->container->data = (NeList*)ptrTable->tab[(intptr_t)neo->container->data];
            solve_pointers_list(neo->container->data, ptrTable, typesTable, containers, vars, expt);
            // Ajout de l'objet au Garbage Collector
            neo_container_convert(neo->container);
        }
        else if ((NEO_TYPE(*neo) == TYPE_USERFUNC || NEO_TYPE(*neo) == TYPE_PARTIALFUNC) && !ismarked(*neo)) {
            mark(*neo);

            // Si on est sur une partial func, opt_args est à -1
            if ((intptr_t)neo->userfunc->opt_args != -1) {
                neo->userfunc->opt_args = (NeList*)ptrTable->tab[(intptr_t)neo->userfunc->opt_args];
                solve_pointers_list(neo->userfunc->opt_args, ptrTable, typesTable, containers, vars, expt);
            }
            else
                neo->userfunc->opt_args = NULL;

            neo->userfunc->tree_buffer = (TreeBuffer*)ptrTable->tab[(intptr_t)neo->userfunc->tree_buffer];

            // On se sert du champ locked pour savoir si on a résolu les pointeurs internes du TreeBuffer
            if (!neo->userfunc->tree_buffer->locked) {
                neo->userfunc->tree_buffer->locked = true;
                solve_pointers_treebuffer(neo->userfunc->tree_buffer, ptrTable, typesTable, containers, vars, expt);
                ptrlist_append(global_env->TREEBUFFERS, neo->userfunc->tree_buffer);
                move_treebuffer_to_side_memory(neo->userfunc->tree_buffer);
            }

            // Ajout de l'objet au Garbage Collector
            if (NEO_TYPE(*neo) == TYPE_USERFUNC)
                neo_userfunc_convert(neo->userfunc);
        }
    }
    else if (NEO_TYPE(*neo) == TYPE_PROMISE) { // Vérification que la promesse est valide
        int index = intptrlist_index(&global_env->PROMISES_CNT, neo->refc_ptr);
        if (index == -1) {
            neon_fail(122, NO_ARGS);
            return;
        }
        else {
            // On incrémente le nombre de promesses du processus associé
            *(neo->refc_ptr) += 1;
        }
    }
}


void solve_pointers(NeObj *neo, intptrlist *ptrTable, intlist *typesTable, intlist* containers, intlist* vars, intlist* expt) {
    solve_pointers_aux(neo, ptrTable, typesTable, containers, vars, expt);
    // Démarque les objets marqués par solve_pointers_aux
    recursive_unmark(*neo);
}



// Supprime un pointeur de la table des pointeurs
void free_pointer(PointerUnion ptr, PointerType ptrType) {
    switch (ptrType) {
        case CharStarPtr:
            neon_free(ptr.charstar);
            return;
        
        case StringPtr:
            neon_free(ptr.string->string);
            neon_free(ptr.string);
            return;
        
        case NeListPtr:
            neon_free(ptr.nelist->tab);
            neon_free(ptr.nelist);
            return;
        
        case ContainerPtr:
            neon_free(ptr.container);
            return;
        
        case TreeBufferPtr:
            neon_free(ptr.treebuffer->pointer);
            neon_free(ptr.treebuffer);
            return;
        
        case UserFuncPtr:
            neon_free(ptr.userfunc->args);
            if (ptr.userfunc->doc != NULL)
                neon_free(ptr.userfunc->doc);
            neon_free(ptr.userfunc);
            return;
        
        case FunctionPtr:
            function_destroy(ptr.function);
            return;
    }
}


// Libère tous les objets de la table des pointeurs
void free_all_pointers(intptrlist* ptrTable, intlist* typesTable) {
    for (int i=0 ; i < ptrTable->len ; i++) {
        free_pointer((PointerUnion)(void*)ptrTable->tab[i], typesTable->tab[i]);
    }
}



// Fonction pour désérialiser entièrement un objet
NeObj neobject_deserialize(NeStream stream) {
    // Table qui stocke quels pointeurs ont été sérialisés
    intptrlist ptrTable = intptrlist_create(0);
    // Table qui stocke les types des pointeurs de la table ptrTable
    intlist typesTable = intlist_create(0);

    // Table qui stocke tous les types de containers utilisés par l'objet à sérialiser
    intlist containersTable = intlist_create(0);

    // Table qui stocke toutes les variables de l'environnement actuel utilisées par l'objet à sérialiser
    intlist varsTable = intlist_create(0);

    // Table qui stocke toutes les exceptions utilisées par l'objet à sérialiser
    intlist exceptionsTable = intlist_create(0);

    // Lecture des headers
    read_variables_header(stream, &varsTable);
    read_containers_header(stream, &containersTable);
    read_exceptions_header(stream, &exceptionsTable);

    // Lecture de tous les pointeurs (les sommets du graphe)
    read_all_pointers(stream, &ptrTable, &typesTable, &containersTable, &varsTable, &exceptionsTable);
    if_error goto handle_error;
    
    NeObj obj = read_partial_neobject(stream);
    if_error goto handle_error;

    // Remplacement de tous les indices dans la table des pointeurs par les réels pointeurs
    solve_pointers(&obj, &ptrTable, &typesTable, &containersTable, &varsTable, &exceptionsTable);

    neon_free(ptrTable.tab);
    neon_free(typesTable.tab);
    neon_free(exceptionsTable.tab);
    neon_free(varsTable.tab);
    neon_free(containersTable.tab);
    return obj;

handle_error:
    free_all_pointers(&ptrTable, &typesTable);
    neon_free(ptrTable.tab);
    neon_free(typesTable.tab);
    neon_free(exceptionsTable.tab);
    neon_free(varsTable.tab);
    neon_free(containersTable.tab);
    return NEO_VOID;
}
