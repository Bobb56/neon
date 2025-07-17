// ast.h
#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    char type;      // Type de noeud: '+', '-', '*', '/' ou 'n' pour les nombres
    int value;      // Valeur si le noeud est un nombre
    struct Node *left;
    struct Node *right;
} Node;

Node* createNode(char type, int value, Node* left, Node* right);

void printAST(Node* node, int depth);

void freeAST(Node* node);

#endif // AST_H
