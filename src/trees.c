#include "headers/trees.h"
#include "headers/errors.h"




void TreeList_init(struct TreeList* tree_list)
{
    tree_list->trees = NULL;
    tree_list->len = 0;
}

void TreeList_append(struct TreeList* tree_list, NeTree* tree) {
    if (tree_list->len == 0) {
        tree_list->trees = neon_malloc(sizeof(Tree*));
    }
    else {
        tree_list->trees = neon_realloc(tree_list->trees, tree_list->len + 1);
    }

    tree_list->trees[tree_list->len++] = tree;
}


NeTree make_unaryOp(int op, NeTree* expr) {
    NeTree tree;
    tree.unary_op = neon_malloc(sizeof(struct UnaryOp));
    tree.unary_op->type = TypeUnaryOp;
    tree.unary_op->op = op;
    tree.unary_op->expr = expr;
    return tree;
}

NeTree make_binaryOp(int op, NeTree* left, NeTree* right) {
    NeTree tree;
    tree.binary_op = neon_malloc(sizeof(struct BinaryOp));
    tree.binary_op->type = TypeBinaryOp;
    tree.binary_op->op = op;
    tree.binary_op->left = left;
    tree.binary_op->right = right;
    return tree;
}
