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
extern strlist boolean;
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

extern int error;



void printIndent(void)
{
    for (int i = 0 ; i < indent ; i++)
        printString("    ");
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
        
        if (gramm == VARLEFT)
        {
            affExpr(tree->sons[0]);
            printString(" ");
            printString(tree->label1);
            printString(" ");
        }


        if (gramm == VARRIGHT || gramm == RIGHT)
        {
            printString(" ");
            printString(tree->label1);
            printString(" ");
            affExpr(tree->sons[0]);
        }

        if (gramm == VAR_RIGHT || gramm == RIGHT_LEFT)
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
        char* nom = NOMS->tab[nelist_index(ADRESSES, tree->data)];
        printString(nom);
        printString("(");

        for (int i = 0 ; i + 1 < tree->nbSons ; i++)
        {
            affExpr(tree->sons[i]);
            printString(", ");
        }
        if (tree->nbSons > 0)
            affExpr(tree->sons[tree->nbSons-1]);


        printString(")");
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
        char* string = (char*)tree->data->data;
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
        printString(CONTAINERS->tab[(int)number_toDouble(tree->label2)]);
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


        if (tree->label1 != NULL) // si ça se trouve le neobject a pas été initialisé, auquel cas on peut directement lire la chaine de caractères
            printString(tree->label1);
        else
        {
            NeObject* neo = eval(tree->sons[0]);
            Container* c = neo->data;
            int index = (long int)number_toDouble(tree->label2);
            NeList* list = ATTRIBUTES->tab[c->type]->data;
            printString(neo_to_string(list->tab[index]));
        }

    }

    return ;
}





void affFor(Tree* tree)
{
    printIndent();
    printString("Pour (");
    affExpr(tree->sons[0]->sons[0]);
    printString(", ");
    affExpr(tree->sons[0]->sons[1]);
    printString(", ");
    affExpr(tree->sons[0]->sons[2]);
    printString(") faire");
    newLine();
    /*

    indent++;
    for (int i = 0 ; i < tree->sons[1]->nbSons ; i++)
    {
        affProgram(tree->sons[1]->sons[i]);
    }
    indent--;
    printIndent();
    printString("end");
    newLine();
    */
    
}




void affIf(Tree* tree)
{
    printIndent();
    printString("Si (");
    affExpr(tree->sons[0]);
    printString(") alors");
    /*newLine();

    indent++;
    for (int i = 0 ; i < tree->sons[1]->nbSons ; i++)
    {
        affProgram(tree->sons[1]->sons[i]);
    }
    indent--;*/
}



void affElif(Tree* tree)
{
    printIndent();
    printString("SinonSi (");
    affExpr(tree->sons[0]);
    printString(") alors");
    /*newLine();

    indent++;
    for (int i = 0 ; i < tree->sons[1]->nbSons ; i++)
    {
        affProgram(tree->sons[1]->sons[i]);
    }
    indent--;*/
}


void affElse(Tree* tree)
{
    printIndent();
    printString("Sinon");
    /*newLine();

    indent++;
    for (int i = 0 ; i < tree->nbSons ; i++)
    {
        affProgram(tree->sons[i]);
    }
    indent--;*/
}



void affWhile(Tree* tree)
{
    printIndent();
    printString("TantQue (");
    affExpr(tree->sons[0]);
    printString(") faire");
    newLine();
    /*
    indent++;
    for (int i = 0 ; i < tree->sons[1]->nbSons ; i++)
    {
        affProgram(tree->sons[1]->sons[i]);
    }
    indent--;
    printIndent();
    printString("end");
    newLine();*/
}




void affConBlock(Tree* tree)
{
    _Bool plusieursIfs = false;

    for (int i=0 ; i < tree->nbSons ; i++)
    {
        if (tree->sons[i]->type == TYPE_STATEMENTIF)
        {
            if (plusieursIfs)
            {
                printIndent();
                printString("Fin");
                newLine();
            }

            affIf(tree->sons[i]);
            plusieursIfs = true;
        }

        if (tree->sons[i]->type == TYPE_STATEMENTELIF)
        {
            affElif(tree->sons[i]);
        }

        if (tree->sons[i]->type == TYPE_STATEMENTELSE)
        {
            affElse(tree->sons[i]);
        }
    }

    printIndent();
    printString("Fin");
    newLine();
}



void affProgram(Tree* tree)
{
    if (tree->type == TYPE_STATEMENTIF)
    {
        affIf(tree);
    }
    if (tree->type == TYPE_STATEMENTELIF)
    {
        affElif(tree);
    }
    if (tree->type == TYPE_STATEMENTELSE)
    {
        affElse(tree);
    }
    if (tree->type == TYPE_KEYWORD)
    {
        printIndent();
        printString(lkeywords.tab[(int)number_toDouble(tree->label2) - 1]);
        newLine();
    }
    else if (tree->type == TYPE_TRYEXCEPT)
    {
        printString("\nEssayer");
        newLine();

        indent ++;
        for (int i = 0 ; i < tree->sons[0]->nbSons ; i++)
        {
            affProgram(tree->sons[0]->sons[i]);
        }
        indent --;

        printString("SiErreur (");
        for (int i = 0 ; i < tree->sons[1]->sons[0]->nbSons ; i++)
        {
            affExpr(tree->sons[1]->sons[0]->sons[i]);
            if (i < tree->sons[1]->sons[0]->nbSons - 1)
                printString(", ");
        }
        printString(") faire");
        /*newLine();

        indent ++;
        for (int i = 0 ; i < tree->sons[1]->sons[1]->nbSons ; i++)
        {
            affProgram(tree->sons[1]->sons[1]->sons[i]);
        }
        indent --;

        printString("end");*/
        newLine();

    }

    else if (tree->type == TYPE_FUNCTIONDEF)
    {
        UserFunc* fun = tree->data->data;
        printIndent();

        if (tree->data->type == TYPE_USERFUNC)
            printString("Fonction ");
        else
            printString("Methode ");

        printString(tree->label1);
        printString("(");
        for (int i = 0 ; i < fun->nbArgs ; i++)
        {
            // affichage de la variable fun->args[i]
            char* nom = NOMS->tab[nelist_index(ADRESSES, fun->args[i])];
            printString(nom);

            if (i < fun->nbArgs - 1)
                printString(", ");
        }
        printString(") faire");
        /*newLine();

        // affichage du code
        indent++;
        affProgram(fun->code);
        indent--;
        printIndent();
        printString("end");
        newLine();*/

    }


    else if (tree->type == TYPE_STATEMENTFOR)
    {
        affFor(tree);
    }

    else if (tree->type == TYPE_STATEMENTWHILE)
    {
        affWhile(tree);
    }

    else if (tree->type == TYPE_CONDITIONBLOCK)
    {
        affConBlock(tree);
    }

    else if (tree->type == TYPE_SYNTAXTREE)
    {
        for (int i = 0 ; i < tree->nbSons ; i++)
        {
            affProgram(tree->sons[i]);
        }
    }
    else if (tree->type == TYPE_BLOCKWORD1LINE)
    {
        printIndent();
        if ((int)number_toDouble(tree->label2) == RETURN)
        {
            printString("Renvoyer (");
        }
        if ((int)number_toDouble(tree->label2) == IMPORT)
        {
            printString("Importer(");
        }
        if ((int)number_toDouble(tree->label2) == LOCAL)
        {
            printString("locale (");
        }
        for (int i = 0 ; i + 1 < tree->nbSons ; i++)
        {
            affExpr(tree->sons[i]);
            printString(", ");
        }
        
        if (tree->nbSons > 0)
            affExpr(tree->sons[tree->nbSons-1]);
        
        printString(")");
        newLine();
    }
    else
    {
        printIndent();
        affExpr(tree);
        newLine();
    }
    
}
