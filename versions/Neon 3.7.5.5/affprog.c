#include "headers.c"
#include <string.h>



//creation du tableau de pointeurs generiques a liberer en cas d’erreur

int indent = 0;



//déclaration des variables globales à cut
extern strlist acceptedChars;
extern listlist syntax;
extern strlist sousop;
extern intlist gramm1;
extern strlist operateurs3;
extern strlist operateurs1;
extern strlist operateurs2;
extern strlist blockwords;
extern strlist neon_boolean;
extern strlist keywords;
extern strlist lkeywords;
extern strlist constant;


extern strlist OPERATEURS;
extern intlist PRIORITE;
extern intlist OPERANDES;


//stockage des variables
extern strlist* NOMS;
extern intlist TYPES;
extern NeList* ADRESSES;
extern strlist VARIABLES;
extern listlist LISTES;

extern strlist NOMSBUILTINSFONC;
extern strlist HELPBUILTINSFONC;
extern intlist TYPESBUILTINSFONC;

extern NeObject* (*OPFONC[NBOPERATEURS])(NeObject*,NeObject*);
extern NeObject* (*BUILTINSFONC[NBBUILTINFONC])(NeList*);

extern strlist* CONTAINERS;
extern NeList* ATTRIBUTES;

extern int CODE_ERROR;



void affLine(char* file, int line) {
    setColor(RED);
    printString(" # ");
    setColor(WHITE);

    if (file != NULL) {
        printString("Error happened in file ");
        setColor(GREEN); printString(file); setColor(WHITE);
        printString(" at line ");

        Number n = number_fromDouble((double)line);
        setColor(GREEN); printNumber(n); setColor(WHITE);
        number_destroy(n);

        printString(" :");
        newLine();

        setColor(RED);
        printString(" # ");
        setColor(WHITE);

        char* program = openFile(file);
        int compt = 1, i = 0;

        // cherche le début de la ligne
        while (compt < line && program[i] != '\0') {
            if (program[i] == '\n')
                compt++;
            i++;
        }

        // passe tous les espaces du début
        while (program[i] == ' ') i++;

        // marque la fin de la ligne
        int debut = i;
        while (program[i] != '\0' && program[i] != '\n')
            i++;

        program[i] = '\0';

        printString(program + debut);
        err_free(program);
    }
    else {
        printString("Error happened in standard input");
    }
    newLine();
}








void affExpr(Tree* tree)
{
    if (tree->type == TYPE_OPERATOR)
    {
        int gramm = gramm1.tab[strlist_index(&operateurs3,tree->label1)];

        if (strcmp(tree->label1, "_") == 0)
        {
            err_free(tree->label1);
            tree->label1 = strdup("-");
        }
        
        if (gramm & VARLEFT)
        {
            affExpr(tree->sons[0]);
            printString(" ");
            printString(tree->label1);
            printString(" ");
        }


        if (gramm & VARRIGHT || gramm & RIGHT)
        {
            printString(" ");
            printString(tree->label1);
            printString(" ");
            affExpr(tree->sons[0]);
        }

        if (gramm & VAR_RIGHT || gramm & RIGHT_LEFT)
        {
            affExpr(tree->sons[0]);
            printString(" ");
            printString(tree->label1);
            printString(" ");
            affExpr(tree->sons[1]);
        }

        
    }


    if (tree->type == TYPE_FONCTION)
    {
        NeObject* fonction = eval(tree->sons[0]);

        char* nom = NOMS->tab[nelist_index2(ADRESSES, fonction)];
        printString(nom);
        printString("(");

        for (int i = 0 ; i + 1 < tree->sons[0]->nbSons ; i++)
        {
            affExpr(tree->sons[0]->sons[i]);
            printString(", ");
        }
        if (tree->sons[0]->nbSons > 0)
            affExpr(tree->sons[0]->sons[tree->sons[0]->nbSons-1]);


        printString(")");

        neobject_destroy(fonction, true);
    }

    if (tree->type == TYPE_VARIABLE)
    {
        char* nom = NOMS->tab[nelist_index(ADRESSES, tree->data)];
        printString(nom);
    }

    if (tree->type == TYPE_NONE || tree->type == TYPE_BOOL || tree->type == TYPE_CONST || tree->type == TYPE_EXCEPTION || tree->type == TYPE_NUMBER)
    {
        neobject_aff(tree->data);
    }

    if (tree->type == TYPE_STRING)
    {
        char* string = (char*)tree->data->data.ptr;
        //char* str2 = traitementStringInverse(string);
        printString("\"");
        printString(string);
        printString("\"");
        //err_free(str2);
    }


    if (tree->type == TYPE_LISTINDEX)
    {
        affExpr(tree->sons[0]);
        printString("[");
        affExpr(tree->sons[1]);
        printString("]");
    }

    if (tree->type == TYPE_LIST)
    {
        if (tree->nbSons == 0)
        {
            printString("[]");
        }
        else
        {
            printString("[");
            for (int i=0;i<tree->nbSons-1;i++)
            {
                affExpr(tree->sons[i]);
                printString(", ");
            }
            affExpr(tree->sons[tree->nbSons - 1]);
            printString("]");
        }
        
    }

    if (tree->type == TYPE_CONTAINER)
    {
        //affichage du type
        printString(CONTAINERS->tab[tree->label2]);
        printString("(");

        // boucle pour afficher chaque nom de champ et chaque expression associée
        for (int i = 0 ; i < tree->nbSons ; i++)
        {
            printString(tree->sons[i]->label1);
            printString(": ");
            affExpr(tree->sons[i]->sons[0]);

            if (i < tree->nbSons - 1)
                printString(", ");
        }
        printString(")");

    }


    if (tree->type == TYPE_ATTRIBUTE)
    {
        affExpr(tree->sons[0]);
        printString(">>");


        // normalement dans tous les cas on est censé vérifier la conditino
        // en effet, l'arbre garde toujours le nom de l'attribut, sinon c'est trop risqué de le récupérer par évaluation arrivé ici
        // par exemple le container peut être une variable locale qui a disparu lors de l'affichage de l'erreur
        if (tree->label1 != NULL) // si ça se trouve le neobject a pas été initialisé, auquel cas on peut directement lire la chaine de caractères
            printString(tree->label1);
        else
        {
            printString("unknown_attribute");
        }

    }

    return ;
}