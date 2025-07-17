%{
#include "ast.h"
#include "expr.tab.h"  /* Inclure l'en-tête généré par Bison pour le type YYSTYPE */
#include <stdlib.h>    /* Pour atoi */
%}

%%

[0-9]+          { yylval.number = atoi(yytext); return NUMBER; }
[\t\n ]+        ; /* Ignore les espaces et les nouvelles lignes */
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULTIPLY; }
"/"             { return DIVIDE; }
"("             { return LPAREN; }
")"             { return RPAREN; }
.               { printf("Unknown character: %s\n", yytext); }

%%

int yywrap() { return 1; }
