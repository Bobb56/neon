%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "expr.tab.h"


Node* createNode(char type, int value, Node* left, Node* right) {
    Node *node = (Node*) malloc(sizeof(Node));
    node->type = type;
    node->value = value;
    node->left = left;
    node->right = right;
    return node;
}

void printAST(Node* node, int depth) {
    printf("affichage\n");
    if (node == NULL) return;

    for (int i = 0; i < depth; i++) printf("  "); // indentation
    if (node->type == 'n') printf("%d\n", node->value);
    else printf("%c\n", node->type);

    printAST(node->left, depth + 1);
    printAST(node->right, depth + 1);
}

void freeAST(Node* node) {
    if (node == NULL) return;
    freeAST(node->left);
    freeAST(node->right);
    free(node);
}


int yylex();
void yyerror(const char* s) { printf("Error: %s\n", s); }

%}

%union {
    int number;
    Node* node;
}

%token <number> NUMBER
%token PLUS MINUS MULTIPLY DIVIDE LPAREN RPAREN

%type <node> expr term factor

%%

input:
    expr { printAST($1, 0); freeAST($1); }
    ;

expr:
    expr PLUS term   { $$ = createNode('+', 0, $1, $3); }
    | expr MINUS term { $$ = createNode('-', 0, $1, $3); }
    | term           { $$ = $1; }
    ;

term:
    term MULTIPLY factor { $$ = createNode('*', 0, $1, $3); }
    | term DIVIDE factor { $$ = createNode('/', 0, $1, $3); }
    | factor             { $$ = $1; }
    ;

factor:
    NUMBER           { $$ = createNode('n', $1, NULL, NULL); }
    | LPAREN expr RPAREN { $$ = $2; }
    ;

%%

int main() {
    printf("Enter an expression: ");
    yyparse();
    return 0;
}
