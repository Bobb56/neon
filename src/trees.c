#include <stdint.h>
#include <stdlib.h>
#include "headers/trees.h"
#include "headers/dynarrays.h"
#include "headers/errors.h"
#include "headers/neon.h"
#include "headers/objects.h"
#include <string.h>
#include "headers/neonio.h"

/*
Comment fonctionnent les structures de données d'arbres ?
Afin de maximiser la localité mémoire et la compacité des données, les données associées à un même arbre sont
toutes regroupées au sein du même TreeBuffer : un gros buffer dans lequel on alloue tous les sous-arbres et les
TreeList.
Ainsi, quand on demande la transformation d'un fichier source en arbre syntaxique, un TreeBuffer est créé, et
tous les arbres sont alloués dedans, à l'exception des arbres associés aux fonctions définies et aux expressions
d'appel de fonction pour le lancement de fonction en parallèle. Ces derniers sont alloués dans des TreeBuffers
persistants libérés lors de la destruction d'un environnement. Il sont chaînés dans env->TREEBUFFERS

Ce buffer n'a aucune fonctionnalité de désallocation à part sa suppression totale, donc on alloue dedans uniquement
des zones mémoire qui n'auront pas besoin d'être supprimées ou modifiées avant la suppression définitive
du TreeBuffer.
Cela engendre un fonctionnement un peu plus subtil des TreeList.
Lorsque qu'un type d'arbre contient des TreeList, on alloue lors de sa création un objet TreeListTemp qui peut
être redimensionné, modifié, etc. Une fois que ce TreeListTemp a atteint sa forme finale, on peut appeler
la fonction TreeListTemp_dump(TreeBuffer*, struct TreeListTemp*, struct TreeList*) qui va recopier la
TreeListTemp dans la TreeList indiquée en argument tout en supprimant la TreeListTemp.

Si un programme utilise uniquement ces fonctions pour créer des TreeBuffers :
- createSyntaxTree
- createExpressionTree
- TreeBuffer_persistent_expr
- TreeBuffer_persistent_syntaxtree
alors les TreeBuffers sont garantis comme ne pouvant plus être modifiés après
Chacune de ces fonctions verrouille le TreeBuffer juste avant de le renvoyer.
Cela permet d'optimiser l'accès aux sous-arbres lors de l'évaluation
*/


/*
int TreeBuffer_init(TreeBuffer* tb) {
    static uint8_t fileno = 0; // compte le nombre de fichiers créés
    fileno++;

    strcpy(tb->name, "NRDAT"); // Neon Runtime Data
    uint8_t n = fileno;
    for (int i = 7 ; i >= 5 ; i--) {
        tb->name[i] = n%10 + '0';
        n = (n-n%10)/10;
    }
    tb->name[8] = '\0';

    tb->handle = ti_Open(tb->name, "w");
    
    if (tb->handle == 0) {
        return -1;
    }

    tb->pointer = ti_GetVATPtr(tb->handle);
    tb->size = 0;

    TreeListTemp_init(&tb->remember);

    return 0;
}


TreeBufferIndex TreeBuffer_alloc(TreeBuffer* tb, int size) {
    TreeBufferIndex pointer = tb->size;
    uint8_t zero = 0;
    ti_Write(&zero, 1, size, tb->handle);
    tb->pointer = ti_GetVATPtr(tb->handle);

    tb->size += size;
    return pointer;
}

void TreeBuffer_destroy(TreeBuffer* tb, TreeBufferIndex entry_point) {
    // Parcours de tout le TreeBuffer pour libérer tous les pointeurs dedans
    if (!TREE_ISVOID(entry_point))
        NeTree_destroy(tb, entry_point);
    
    // On supprime les arbres sur lesquels personne ne pointait
    TreeListTemp_destroy(tb, &tb->remember);

    // Et on supprime les données en elles-même
    ti_Close(tb->handle);
    ti_Delete(tb->name);
}

*/


int TreeBuffer_init(TreeBuffer* tb) {
    tb->entry_point = TREE_VOID;
    tb->size = 0;
    tb->n_blocks = 1;
    tb->block_size = 512;
    tb->pointer = neon_malloc(tb->block_size * tb->n_blocks);
    memset(tb->pointer, 0xff, tb->block_size * tb->n_blocks);

    if (tb->pointer == NULL)
        return -1;

    tb->locked = false;
    return 0;
}


TreeBufferIndex TreeBuffer_alloc(TreeBuffer* tb, int size) {
    neon_assert(!tb->locked, TREE_VOID);

    TreeBufferIndex pointer = tb->size;
    while (tb->size + size > tb->block_size * tb->n_blocks) {
        tb->n_blocks++;
        void* tmp = neon_realloc(tb->pointer, tb->n_blocks * tb->block_size);

        if (tmp == NULL) {
            global_env->CODE_ERROR = 12;
            return TREE_VOID;
        }

        tb->pointer = tmp;
    }
    tb->size += size;
    return pointer;
}

void TreeBuffer_destroy(TreeBuffer* tb) {
    // Parcours de tout le TreeBuffer pour libérer tous les pointeurs dedans
    if (!TREE_ISVOID(tb->entry_point))
        NeTree_destroy(tb, tb->entry_point);
    
    // On supprime les arbres sur lesquels personne ne pointait
    free(tb->pointer);
}

void TreeBuffer_delete_all(ptrlist* tree_buffers) {
    ptrlist* ptr = tree_buffers;
    while (ptr != NULL && (ptr->tete != NULL || ptr->queue != NULL)) {
        TreeBuffer_destroy(ptr->tete);
        ptr = ptr->queue;
    }
}





void TreeList_destroy(TreeBuffer* tb, struct TreeList* treelist) {
    TreeBufferIndex* array_ptr = tb->pointer + treelist->indices;
    for (int i = 0 ; i < treelist->length ; i++) {
        NeTree_destroy(tb, treelistGet(tb, *treelist)[i]);
    }
}

void NeTree_destroy(TreeBuffer* tb, TreeBufferIndex tree) {
    if (TREE_ISVOID(tree))
        return;

    switch (TREE_TYPE(tb, tree)) {
        case TypeBinaryOp:
            NeTree_destroy(tb, treeBinOp(tb, tree)->left);
            NeTree_destroy(tb, treeBinOp(tb, tree)->right);
            break;
        
        case TypeUnaryOp:
            NeTree_destroy(tb, treeUnOp(tb, tree)->expr);
            break;

        case TypeConst:
            neobject_destroy(treeConst(tb, tree)->obj);
            break;
        
        case TypeFor:
        case TypeForeach:
            TreeList_destroy(tb, &treeFor(tb, tree)->params);
            NeTree_destroy(tb, treeFor(tb, tree)->block);
            break;

        case TypeWhile:
        case TypeIf:
        case TypeElif:
            NeTree_destroy(tb, treeIEW(tb, tree)->expression);
            NeTree_destroy(tb, treeIEW(tb, tree)->code);
            break;
        
        case TypeAtomic:
        case TypeConditionblock:
        case TypeSyntaxtree:
        case TypeElse:
        case TypeList:
            TreeList_destroy(tb, &treeSntxTree(tb, tree)->treelist);
            break;
        
        case TypeTryExcept:
            NeTree_destroy(tb, treeTE(tb, tree)->try_tree);
            TreeList_destroy(tb, &treeTE(tb, tree)->except_blocks);
            break;
        
        case TypeFunctiondef:
            neon_free(treeFDef(tb, tree)->name);
            neobject_destroy(treeFDef(tb, tree)->object);
            TreeList_destroy(tb, &treeFDef(tb, tree)->args);
            break;
        
        case TypeListindex:
            NeTree_destroy(tb, treeLstIndx(tb, tree)->index);
            NeTree_destroy(tb, treeLstIndx(tb, tree)->object);
            break;
        
        case TypeFunctioncall:
            TreeList_destroy(tb, &treeFCall(tb, tree)->args);
            NeTree_destroy(tb, treeFCall(tb, tree)->function);
            if (!neo_is_void(treeFCall(tb, tree)->function_obj))
                neobject_destroy(treeFCall(tb, tree)->function_obj);
            break;
        
        case TypeAttribute:
            if (treeAttr(tb, tree)->name != NULL)
                neon_free(treeAttr(tb, tree)->name);
            NeTree_destroy(tb, treeAttr(tb, tree)->object);
            break;
        
        case TypeKWParam:
            TreeList_destroy(tb, &treeKWParam(tb, tree)->params);
            break;
        
        case TypeContainerLit:
            TreeList_destroy(tb, &treeContLit(tb, tree)->attributes);
            break;
        
        case TypeAttributeLit:
            neon_free(treeAttrLit(tb, tree)->name);
            NeTree_destroy(tb, treeAttrLit(tb, tree)->expr);
            break;
        
        case TypeExceptBlock:
            NeTree_destroy(tb, treeExpt(tb, tree)->block);
            TreeList_destroy(tb, &treeExpt(tb, tree)->exceptions);
            break;
        
        case TypeKeyword:
        case TypeVariable:
        case TypeParallelCall:
            break;
    }
}






void TreeListTemp_destroy(TreeBuffer* tb, struct TreeListTemp* list) {
    for (int i = 0 ; i < list->len ; i++)
        NeTree_destroy(tb, list->trees[i]);
    neon_free(list->trees);
}


size_t type_size(TreeType type) {
    switch (type) {
        case TypeBinaryOp:
            return sizeof(struct BinaryOp);
        case TypeUnaryOp:
            return sizeof(struct UnaryOp);
        case TypeConst:
            return sizeof(struct ConstObj);
        case TypeFor:
        case TypeForeach:
            return sizeof(struct StatementFor);
        case TypeWhile:
        case TypeIf:
        case TypeElif:
            return sizeof(struct StatementIEW);        
        case TypeElse:
        case TypeSyntaxtree:
        case TypeAtomic:
        case TypeConditionblock:
        case TypeList:
            return sizeof(struct SyntaxTree);
        case TypeTryExcept:
            return sizeof(struct TryExcept);
        case TypeFunctiondef:
            return sizeof(struct FunctionDef);
        case TypeVariable:
            return sizeof(struct Variable);
        case TypeListindex:
            return sizeof(struct ListIndex);
        case TypeFunctioncall:
            return sizeof(struct FunctionCall);
        case TypeAttribute:
            return sizeof(struct Attribute);
        case TypeKeyword:
            return sizeof(struct Keyword);
        case TypeKWParam:
            return sizeof(struct KWParam);
        case TypeContainerLit:
            return sizeof(struct ContainerLit);
        case TypeAttributeLit:
            return sizeof(struct AttributeLit);
        case TypeExceptBlock:
            return sizeof(struct ExceptBlock);
        case TypeParallelCall:
            return sizeof(struct ParallelCall);
    }
}






TreeBufferIndex NeTree_create(TreeBuffer* tb, TreeType type, int line) {
    TreeBufferIndex tree = TreeBuffer_alloc(tb, type_size(type));
    return_on_error(TREE_VOID);

    TREE_LINE(tb, tree) = line;
    TREE_TYPE(tb,tree) = type;
    return tree;
}





bool NeTree_isexpr(TreeBuffer* tb, TreeBufferIndex tree) {
    intlist expressions = {
        .tab = (int[]) {
                            TypeBinaryOp,
                            TypeUnaryOp,
                            TypeConst,
                            TypeVariable,
                            TypeListindex,
                            TypeFunctioncall,
                            TypeAttribute,
                            TypeList,
                            TypeContainerLit,
                            TypeAttributeLit
                        },
        .len = 10
    };
    return intlist_inList(&expressions, TREE_TYPE(tb,tree));
}





void TreeListTemp_init(struct TreeListTemp* tree_list)
{
    tree_list->trees = NULL;
    tree_list->len = 0;
}

void TreeListTemp_append(struct TreeListTemp* tree_list, TreeBufferIndex tree) {
    TreeBufferIndex* ptr;
    if (tree_list->len == 0) {
        ptr = neon_malloc(sizeof(TreeBufferIndex));
    }
    else {
        ptr = neon_realloc(tree_list->trees, (tree_list->len + 1) * sizeof(TreeBufferIndex));
    }

    if (ptr == NULL) {
        global_env->CODE_ERROR = 12;
        return;
    }

    tree_list->trees = ptr;
    tree_list->trees[tree_list->len++] = tree;
}

void TreeListTemp_insert(struct TreeListTemp* tree_list, TreeBufferIndex tree, int index) {
    if (tree_list->len == 0) {
        tree_list->trees = neon_malloc(sizeof(TreeBufferIndex));
    }
    else {
        tree_list->trees = neon_realloc(tree_list->trees, (tree_list->len + 1) * sizeof(TreeBufferIndex));
    }

    tree_list->len++;

    for (int i = tree_list->len - 1 ; i > index ; i--) {
        tree_list->trees[i] = tree_list->trees[i-1];
    }

    tree_list->trees[index] = tree;
}



/*
Cette fonction recopie le TreeListTemp dans le TreeBuffer et initialise avec cela le TreeList*
Elle libère le TreeListTemp
*/
struct TreeList TreeListTemp_dump(TreeBuffer* tb, struct TreeListTemp* temp_list) {
    // alloue un espace de la bonne taille et récupère l'indice dans le TreeBuffer
    struct TreeList list;

    list.indices = TreeBuffer_alloc(tb, temp_list->len * sizeof(TreeBufferIndex));

    if_error {
        return list;
    }

    // on le considère comme un tableau de TreeBufferIndex
    TreeBufferIndex* array_ptr = treelistGet(tb, list);

    // copie le temp_list dans la liste contenue dans le TreeBuffer
    for (int i = 0 ; i < temp_list->len ; i++) {
        array_ptr[i] = temp_list->trees[i];
    }

    list.length = temp_list->len;

    // libère la liste temporaire
    neon_free(temp_list->trees);
    return list;
}


TreeBufferIndex NeTree_make_unaryOp(TreeBuffer* tb, int op, TreeBufferIndex expr, int line) {
    TreeBufferIndex tree = NeTree_create(tb, TypeUnaryOp, line);

    return_on_error(TREE_VOID);

    treeUnOp(tb, tree)->op = op;
    treeUnOp(tb, tree)->expr = expr;
    return tree;
}


TreeBufferIndex NeTree_make_parallel_call(TreeBuffer* tb, TreeBuffer* expr_buffer, TreeBufferIndex expr, int line) {
    TreeBufferIndex tree = NeTree_create(tb, TypeParallelCall, line);
    return_on_error(TREE_VOID);

    treeParCall(tb, tree)->expr = expr;
    treeParCall(tb, tree)->expr_buffer = expr_buffer;
    return tree;
}


TreeBufferIndex NeTree_make_binaryOp(TreeBuffer* tb, int op, TreeBufferIndex left, TreeBufferIndex right, int line) {
    TreeBufferIndex tree = NeTree_create(tb, TypeBinaryOp, line);
    return_on_error(TREE_VOID);

    treeBinOp(tb, tree)->op = op;
    treeBinOp(tb, tree)->left = left;
    treeBinOp(tb, tree)->right = right;
    return tree;
}

TreeBufferIndex NeTree_make_variable(TreeBuffer* tb, Var variable, int line) {
    TreeBufferIndex tree = NeTree_create(tb, TypeVariable, line);
    return_on_error(TREE_VOID);

    treeVar(tb, tree)->var = variable;
    return tree;
}

TreeBufferIndex NeTree_make_const(TreeBuffer* tb, NeObj const_obj, int line) {
    TreeBufferIndex tree = NeTree_create(tb, TypeConst, line);
    return_on_error(TREE_VOID);

    treeConst(tb, tree)->obj = const_obj;
    return tree;
}



TreeBufferIndex NeTree_make_fcall(TreeBuffer* tb, int line) {
    TreeBufferIndex tree = NeTree_create(tb, TypeFunctioncall, line);
    return_on_error(TREE_VOID);

    treeFCall(tb, tree)->function_obj = NEO_VOID;
    return tree;
}



TreeBufferIndex NeTree_make_functiondef(TreeBuffer* tb, char* name, struct TreeListTemp args, NeObj object, int line) {
    TreeBufferIndex tree = NeTree_create(tb, TypeFunctiondef, line);
    return_on_error(TREE_VOID);

    TREELIST_AFFECT(
        treeFDef(tb, tree)->args,
        TreeListTemp_dump(tb, &args)
    );
    

    treeFDef(tb, tree)->object = object;
    treeFDef(tb, tree)->name = name;
    return tree;
}


TreeBufferIndex NeTree_make_attribute(TreeBuffer* tb, TreeBufferIndex object, char* name, int line) {
    TreeBufferIndex tree = NeTree_create(tb, TypeAttribute, line);
    return_on_error(TREE_VOID);

    treeAttr(tb, tree)->last_cont_type = -1;
    treeAttr(tb, tree)->index = -1;
    treeAttr(tb, tree)->name = name;
    treeAttr(tb, tree)->object = object;
    return tree;
}

TreeBufferIndex NeTree_make_IEWF_tree(TreeBuffer* tb, TreeBufferIndex expr, TreeBufferIndex block, TreeType type, int line) {
    TreeBufferIndex tree = NeTree_create(tb, type, line);
    return_on_error(TREE_VOID);

    treeIEW(tb, tree)->expression = expr;
    treeIEW(tb, tree)->code = block;
    return tree;
}

TreeBufferIndex NeTree_make_except_block(TreeBuffer* tb, struct TreeListTemp exceptions, TreeBufferIndex block, int line) {
    TreeBufferIndex tree = NeTree_create(tb, TypeExceptBlock, line);
    return_on_error(TREE_VOID);

    TREELIST_AFFECT(
        treeExpt(tb, tree)->exceptions,
        TreeListTemp_dump(tb, &exceptions)
    );

    treeExpt(tb, tree)->block = block;
    return tree;
}


TreeBufferIndex NeTree_make_tryexcept(TreeBuffer* tb, TreeBufferIndex try_tree, struct TreeListTemp except_blocks, int line) {
    TreeBufferIndex tryexcept = NeTree_create(tb, TypeTryExcept, line);
    return_on_error(TREE_VOID);

    TREELIST_AFFECT(
        treeTE(tb,tryexcept)->except_blocks,
        TreeListTemp_dump(tb, &except_blocks)
    );

    treeTE(tb,tryexcept)->try_tree = try_tree;
    return tryexcept;
}

TreeBufferIndex NeTree_make_kwparam(TreeBuffer* tb, struct TreeListTemp params, int code, int line) {
    TreeBufferIndex tree = NeTree_create(tb, TypeKWParam, line);
    return_on_error(TREE_VOID);

    treeKWParam(tb, tree)->code = code;
    
    TREELIST_AFFECT(
        treeKWParam(tb, tree)->params,
        TreeListTemp_dump(tb, &params)
    )

    return tree;
}


TreeBufferIndex NeTree_make_for_tree(TreeBuffer* tb, struct TreeListTemp params, TreeBufferIndex block, int line, TreeType type) {
    TreeBufferIndex tree = NeTree_create(tb, type, line);
    return_on_error(TREE_VOID);

    TREELIST_AFFECT(
        treeFor(tb, tree)->params,
        TreeListTemp_dump(tb, &params)
    );

    treeFor(tb, tree)->block = block;
    return tree;
}