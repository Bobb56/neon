#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "headers/neon.h"
#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/dynarrays.h"
#include "headers/parser.h"
#include "headers/runtime.h"
#include "headers/strings.h"
#include "headers/syntaxtrees.h"




/*
PROCéDé : 
La fonction cut (ou parse) ne sera appelée que deux fois dans cette bibliothèque,
au niveau de createExpressionTree et createSyntaxTree.

Tout le reste ne sera que lecture de ce qui a déjà été calculé auparavant.

Il va falloir changer la manière dont on traite les données renvoyées par cut : ce ne sont plus simplement deux listes :
une d'entiers et une de chaines de caractères, mais un tableau d'ast

Parfois, il va falloir prendre en compte plus de cas qu'auparavant : notamment sur les indices de liste, de chaines de caractères
et les appels de fonction. En l'occurence, il va falloir s'occupper du truc de gauche, et ne pas supposer que c'est une simple
variable.

ETAPES :
Traiter chaque nouvel appel à cut en le transformant en appel à parse
Modifier partout la méthode d'analyse des données renvoyées par cut, en méthode associée à parse
Ajouter les nouveaux traitements pour les indices de liste et les appels de fonction
*/










Tree* tree_create(char* label1, int label2, uint8_t type)
{
    Tree* newTree = malloc(sizeof(Tree));

    if (newTree == NULL)
    {
        global_env->CODE_ERROR = 12;
        return NULL;
    }
    
    newTree->label1 = label1;
    newTree->label2 = label2;
    newTree->type = type;
    
    // création de la liste de fils
    newTree->capacity = 0;

    Tree** tmp = malloc(sizeof(Tree*));
    
    if (tmp == NULL)
    {
        global_env->CODE_ERROR = 12;
        free(newTree);
        return NULL;
    }
    
    newTree->sons=tmp;
    newTree->nbSons = 0;
    newTree->data = NEO_VOID;


    return newTree;
}




void tree_appendSon(Tree* tree, Tree* son)
{
  Tree** tmp;

  if (pow(2, tree->capacity) == tree->nbSons)
  {
    tree->capacity++;
    
    //réallocation
    tmp = malloc(pow(2, tree->capacity)*sizeof(Tree*));

    if (tmp == NULL) {
        global_env->CODE_ERROR = 12;
        return;
    }
    
    
    for (int i=0 ; i<tree->nbSons ; i++)
    {
        tmp[i] = tree->sons[i];
    }
    
    free(tree->sons);
    
    tree->sons = tmp;//affectation du pointeur de tmp vers tree.tab
  }

  tree->sons[tree->nbSons]=son;
  tree->nbSons++;
  return;
}


_Bool tree_isEmpty(Tree* tree) {
    return tree->type == TYPE_SYNTAXTREE && tree->nbSons == 0;
}




void tree_destroy(Tree* tree)
{
    if (tree != NULL) {
        if (tree->label1 != NULL)
            free(tree->label1);
            
        if (tree->nbSons > 0)
        {
            for (int i=0 ; i<tree->nbSons ; i++)
                tree_destroy(tree->sons[i]);
        }
        free(tree->sons);
        
        if (tree->type != TYPE_VARIABLE && tree->type != TYPE_LISTINDEX && tree->type != TYPE_FONCTION)
            neobject_destroy(tree->data);
        
        free(tree);
    }
}








void tree_removeSon(Tree* tree, int index)
{
  if (index >= tree->nbSons)
  {
    global_env->CODE_ERROR = 13;
    return ;
  }
  
  tree_destroy(tree->sons[index]);
  
  for (int i = index ; i < tree->nbSons -1; i++)//décale tous les éléments à partir de celui à supprimer
    tree->sons[i]=tree->sons[i+1];
  
  Tree **tmp;
  
  if (pow(2, tree->capacity-1)==tree->nbSons-1)
  {
    tree->capacity--;
    tmp = realloc(tree->sons, pow(2, tree->capacity)*sizeof(Tree));//réalloue un nouveau pointeur de la bonne taille
    tree->sons = tmp;
  }
  tree->nbSons--;//décrémentation de la longueur
}







Tree* tree_copy(Tree* tree)
{
    // on suppose ici que Adresses et Noms sont déjà initialisés avec les fonctions built-ins
    
    Tree* tree2 = tree_create(NULL, 0, TYPE_SYNTAXTREE);
    if (global_env->CODE_ERROR != 0)
        return NULL;

    tree2->type = tree->type;
    tree2->label1 = (tree->label1 == NULL) ? NULL : strdup(tree->label1);
    tree2->label2 = tree->label2;
    tree2->variable = tree->variable;
    
    tree2->data = neo_copy(tree->data);

    for (int i = 0 ; i < tree->nbSons ; i++)
    {
        tree_appendSon(tree2, tree_copy(tree->sons[i]));
    }

    return tree2;
}







void createExpressionTreeAux(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset)
{

    //printf("createExpressionTreeAux :\n");
    //strlist_aff(tokens);
    //ast_aff(ast, tokens->len);

    int count = ast_typeCountAst(ast, tokens->len, TYPE_ENDOFLINE, offset);
    int real_length = ast_length(ast, tokens->len, offset);
    
    // tous les tokens sont des retours à la ligne
    if (count == real_length) {
        global_env->LINENUMBER = lines->tab[offset];
        global_env->CODE_ERROR = 30;
        return ;
    }
    // il y a un seul token qui n'est pas un retour à la ligne
    else if (count == real_length - 1) // traitement des cas de base de la fonction récursive
    {
        if (real_length > 1) { // on enlève tous les retours à la ligne

            int i = 0;
            while (ast[i]->type == TYPE_ENDOFLINE) i++;

            strlist tokens2 = (strlist) {.tab = tokens->tab + i, .len = ast[i]->fin - offset - i + 1};

            createExpressionTreeAux(tree, ast + i, &tokens2, lines, offset + i);
            return;
        }

        tree->line = lines->tab[offset]; // cet arbre se situe à la ligne [line] du fichier

        if (ast[0]->type == TYPE_VARIABLE)
        {
            // dans ce cas, la champ tree->data de l'arbre va servir à stocker directement l'adresse de la variable. On pourra donc y accéder directement depuis l'arbre de la même manière qu'on le ferait dans global_env->ADRESSES, au lieu de mettre l'adresse dans une autre NeObject, ce qui obligerait à passer dans une dimension de plus de NeObject.
            //il va falloir ajouter à global_env->NOMS le nom de la variable, allouer un NeObj et le mettre comme feuille et dans global_env->ADRESSES
            // le fait de manipuler des NeObj permet de pouvoir modifier le contenu (l'objet en lui-même, mais sans son adresse. Plus de liberté)
            // ajout de la variable :

            // d'abord on vérifie si la variable existe déjà, sinon, on va en créer une copie inutile et qui va entrer en conflit avec la précédente, bref : c'est pas bien !            
            tree->variable = get_var(tokens->tab[0]);
            tree->data = NEO_VOID;
            tree->type = TYPE_VARIABLE;

        }

        else if (ast[0]->type == TYPE_BOOL)
        {
            tree->data = neo_bool_create(strToBool(tokens->tab[0]));// création d'un NeObject correspondant à la valeur contenue dans la feuille
            tree->type = TYPE_BOOL;
        }

        else if (ast[0]->type == TYPE_EXCEPTION)
        {
            // créer une exception
            int index = strlist_index(global_env->EXCEPTIONS, tokens->tab[0]);
            tree->data = neo_exception_create(index);
            tree->type = TYPE_EXCEPTION;
        }


        else if (ast[0]->type == TYPE_CONST)
        {
            if (strcmp(tokens->tab[0], get_none()) == 0) { // TYPE_NONE
                tree->data = neo_none_create();
                tree->type = TYPE_NONE;
            }
            else if (strcmp(tokens->tab[0], get_infinity()) == 0) {
                tree->data = neo_double_create(INFINITY);
                tree->type = TYPE_DOUBLE;
            }
            else if (strcmp(tokens->tab[0], get_nan()) == 0) {
                tree->data = neo_double_create(NAN);
                tree->type = TYPE_DOUBLE;
            }
            else if (strcmp(tokens->tab[0], get_pi()) == 0) {
                tree->data = neo_double_create(PI);
                tree->type = TYPE_DOUBLE;
            }
        }
        
        else if (ast[0]->type == TYPE_FONCTION)
        {

            // ou alors on a un seul argument et il faut l'ajouter en tant que fils, ou alors on a deux arguments et il faut juste changer les caractéristiques de l'arbre à virgule
            ast_pop(ast[0]); // découvre quel est le nom de la fonction
            int lenNomFonc = ast[0]->fin + 1 - offset;


            // on va créer l'arbre pour les arguments
            // on fabrique un nouvel ast et un nouveau tokens contenant uniquement les arguments
            Ast** argsAst;
            int argsAst_offset;
            strlist argsTok;

            if (tokens->len - 1 == ast[0]->fin + 2 - offset) { // si l'indice de la parenthèse fermante est égal à l'indice de la parenthèse ouvrante + 1
                argsAst = NULL;
                argsTok.tab = NULL;
                argsTok.len = 0;
            }
            else {
                argsAst = ast + lenNomFonc + 1;
                argsAst_offset = lenNomFonc + 1;
                argsTok.tab = tokens->tab + lenNomFonc + 1;
                argsTok.len = tokens->len - lenNomFonc - 2;
            }


            if (argsAst != NULL && strlist_ast_inList(&argsTok, argsAst, ":", TYPE_OPERATOR, offset + lenNomFonc + 1))
            {
                char* nomFonc = tokens->tab[0];
                // ce n'est pas une fonction mais un constructeur de container, on réutilise le tableau
                
                // argsTok contient maintenant la liste des tokens des 'arguments' de l'objet
                
                int i = 0; // indice de tok

                while (i < argsTok.len - 2)
                {
                    while (argsAst[i]->type == TYPE_ENDOFLINE) i++;

                    Tree* t1 = tree_create(NULL, 0, TYPE_VIRGULE);

                    if (global_env->CODE_ERROR != 0) {
                        return ;
                    }

                    if (argsAst[i]->type != TYPE_VARIABLE || strcmp(argsTok.tab[i+1],":") != 0) // le champ n'a pas un nom correct ou n'est pas suivi de :
                    {
                        global_env->CODE_ERROR = 87;
                        global_env->LINENUMBER = lines->tab[offset + argsAst_offset + i];
                        tree_destroy(t1);
                        return ;
                    }

                    t1->label1 = strdup(argsTok.tab[i]); // nom du champ


                    i+=2; // on saute le ':'. On a le droit car la condition de la boucle while le permet

                    // calcule l'arbre du truc à droite du ':'
                    int k = i;
                    while (k < argsTok.len && argsAst[k]->type != TYPE_VIRGULE) // saute à l'argument suivant
                        k = argsAst[k]->fin - offset - argsAst_offset + 1; // l'offset de argsAst est lenNomFonc + 1

                    // prend la partie après la virgule, la première partie est 1 seul token que l'on prend au début de la boucle en ajoutant un de plus à i
                    strlist tok2 = (strlist) {.tab = argsTok.tab + i, .len = k-i};


                    Tree* t = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                    if (global_env->CODE_ERROR != 0)
                    {
                        tree_destroy(t1);
                        return ;
                    }

                    createExpressionTreeAux(t, argsAst + i, &tok2, lines, offset + argsAst_offset + i);

                    if (global_env->CODE_ERROR != 0)
                    {
                        tree_destroy(t1);
                        tree_destroy(t);
                        return ;
                    }

                    tree_appendSon(t1, t); // l'arbre représentant l'expression est fils d'un arbre qui contient juste le nom de l'attribut
                    tree_appendSon(tree, t1); // l'arbre est encore fils de l'arbre container

                    i=k+1;
                }

                if (i+2 >= argsTok.len && i < argsTok.len) // c'est que la définition du container est incorrecte
                {
                    global_env->CODE_ERROR = 87;
                    global_env->LINENUMBER = lines->tab[offset + argsAst_offset + i];
                    return ;
                }

                int index = strlist_index(global_env->CONTAINERS, nomFonc);

                if (global_env->CODE_ERROR == 18) // si le type n'existe pas déjà
                {
                    global_env->CODE_ERROR = 0;

                    // il faut vérifier qu'il n'y a pas déjà le même nom d'attribut
                    for (int i=0 ; i < tree->nbSons ; i++)
                    {
                        for (int j=0 ; j < tree->nbSons ; j++)
                        {
                            if (i != j && strcmp(tree->sons[i]->label1, tree->sons[j]->label1) == 0) //doublon
                            {
                                global_env->CODE_ERROR = 86;
                                global_env->LINENUMBER = tree->sons[i]->line;
                                return ;
                            }
                        }
                    }

                    // on est bon, donc on crée le type
                    tree->label2 = global_env->CONTAINERS->len;
                    strlist_append(global_env->CONTAINERS, strdup(nomFonc)); // ajout du nom de container
                    NeList* attr_names = nelist_create(tree->nbSons); // liste des noms d'attributs

                    for (int i = 0 ; i < tree->nbSons ; i++) // création de la liste des attributs
                        attr_names->tab[i] = neo_str_create(strdup(tree->sons[i]->label1));
                    
                    // on transforme la liste de noms en NeObj, mais sans l'ajouter dans la liste d'objets du Mark & Sweep
                    // en effet, c'est un objet à part, qui ne fait pas partie du système d'objets de Neon
                    // les seuls objets qui doivent être dans la liste d'objets sont des objets accessibles à un moment
                    // ou à un autre depuis global_env->ADRESSES
                    nelist_append(global_env->ATTRIBUTES, gc_extern_neo_list_convert(attr_names)); // ajout de la liste

                }
                else // on vérifie que les champs sont bien comme ils ont été définis
                {
                    tree->label2 = index;

                    NeList* noms = neo_to_list(global_env->ATTRIBUTES->tab[index]); // liste des noms définis

                    if (noms->len != tree->nbSons)
                    {
                        global_env->CODE_ERROR = 83;
                        global_env->LINENUMBER = tree->line;
                        return ;
                    }

                    // on va vérifier qu'il y a tous les champs, et s'ils ne sont pas dans l'ordre, on les remet dans l'ordre
                    Tree* tree2 = tree_create(NULL, 0, TYPE_SYNTAXTREE);

                    for (int i = 0 ; i < noms->len ; i++) // on fixe un champ 'officiel', et on cherche où est le champ correspondant dans tree
                    {
                        // on vérifie si tree->sons[i]->label1 est dans noms
                        _Bool bo = false;
                        int k = 0;
                        for (; k < tree->nbSons ; k++)
                        {
                            if (strcmp(neo_to_string(noms->tab[i]), tree->sons[k]->label1) == 0) // le champ existe bien
                            {
                                bo = true;
                                break;
                            }
                        }

                        if (!bo)
                        {
                            global_env->CODE_ERROR = 83;
                            global_env->LINENUMBER = tree->line;
                            free(tree2->sons);
                            free(tree2);
                            return ;
                        }

                        // ajout de la branche au nouvel arbre réorganisé
                        tree_appendSon(tree2, tree->sons[k]);
                    }
                    // petit échange
                    free(tree->sons);
                    tree->sons = tree2->sons;
                    // libération de ce qui reste de tree2
                    free(tree2);
                }

                tree->label1 = NULL;
                tree->type = TYPE_CONTAINER;
            }
            else
            {

                // création de l'arbre pour la fonction en elle-même
                strlist tokens2 = (strlist) {.tab = tokens->tab, .len = lenNomFonc};
                Tree* fonction = tree_create(NULL, 0, TYPE_SYNTAXTREE);

                createExpressionTreeAux(fonction, ast, &tokens2, lines, offset);

                if (global_env->CODE_ERROR != 0) {
                    return;
                }

                tree_appendSon(tree, fonction); // premier fils : la fonction en elle-même

                Tree* argsTree = tree_create(NULL, 0, TYPE_SYNTAXTREE);
            
                if ((argsTok.len == 1 && argsAst[0]->type != TYPE_ENDOFLINE) || argsTok.len > 1) // s'il y a des arguments
                {
                    _Bool virgules = ast_typeInAst(argsAst, argsTok.len, TYPE_VIRGULE, offset + lenNomFonc + 1); // l'offset est de lenNomFonc + 1
                    
                    if (virgules)
                    {
                        createExpressionTreeAux(argsTree, argsAst, &argsTok, lines, offset + lenNomFonc + 1);
                        if (global_env->CODE_ERROR != 0)
                        {
                            return;
                        }
                        
                        free(argsTree->label1);
                        argsTree->label1 = NULL;
                    }
                    else
                    {
                        Tree* fils = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                        if (global_env->CODE_ERROR != 0)
                        {
                            return;
                        }
                        createExpressionTreeAux(fils, argsAst, &argsTok, lines, offset + lenNomFonc + 1);
                        if (global_env->CODE_ERROR != 0)
                        {
                            tree_destroy(fils);
                            return;
                        }
                        tree_appendSon(argsTree, fils);
                    }
                }

                tree_appendSon(tree, argsTree);

                tree->label1 = NULL;
                tree->data = NEO_VOID;
                tree->type = TYPE_FONCTION;
            }
        }
        
        else if (ast[0]->type == TYPE_INTEGER)
        {
            tree->data = neo_integer_create(str_to_int(tokens->tab[0]));// création d'un NeObject correspondant à la valeur contenue dans la feuille
            tree->type = TYPE_INTEGER;
        }

        else if (ast[0]->type == TYPE_DOUBLE)
        {
            tree->data = neo_double_create(str_to_double(tokens->tab[0]));// création d'un NeObject correspondant à la valeur contenue dans la feuille
            tree->type = TYPE_DOUBLE;
        }
        
        else if (ast[0]->type == TYPE_LISTINDEX) // un arbre listindex a deux fils : le fils gauche contient l'arbre correspondant à la liste et le fils droit l'arbre de l'index
        {
            // on fabrique un nouveau tokens et un nouveau AST pour l'expression entre crochets
            ast_pop(ast[0]);

            int lenNomInd = ast[0]->fin + 1 - offset;

            if (tokens->len - 1 == ast[0]->fin + 2 - offset) { // pas d'arguments
                global_env->CODE_ERROR = 31;
                global_env->LINENUMBER = lines->tab[offset];
                return;
            }

            Ast** indAst = ast + ast[0]->fin - offset + 2;
            
            strlist indTok;
            indTok.tab = tokens->tab + ast[0]->fin + 2 - offset;
            indTok.len = tokens->len - ast[0]->fin - 3 + offset;


            Tree* expr = tree_create(NULL, 0, TYPE_SYNTAXTREE);
            createExpressionTreeAux(expr, indAst, &indTok, lines, offset + lenNomInd + 1);

            if (global_env->CODE_ERROR != 0) {
                return;
            }

            // appel récursif pour le nom de la liste
            Tree* nomListe = tree_create(NULL, 0, TYPE_SYNTAXTREE);

            strlist nomTok;
            nomTok.tab = tokens->tab;
            nomTok.len = lenNomInd;

            createExpressionTreeAux(nomListe, ast, &nomTok, lines, offset);
            
            if (global_env->CODE_ERROR != 0) {
                return;
            }

            tree_appendSon(tree, nomListe);
            tree_appendSon(tree, expr);

            tree->type = TYPE_LISTINDEX;

        }
        
        else if (ast[0]->type == TYPE_STRING)
        {
            char* intermediaire = sub(tokens->tab[0],1,strlen(tokens->tab[0])-1); // on enlève les guillemets
            tree->data = neo_str_create(traitementString(intermediaire));
            tree->type = TYPE_STRING;
            free(intermediaire);
        }
        
        else if (ast[0]->type == TYPE_LIST)
        {
            ast_pop(ast[0]);
            // on crée un nouveau token et un nouveau AST

            Ast** listeAst = ast + 1;
            strlist listeTok;

            listeTok.tab = tokens->tab + 1;
            listeTok.len = tokens->len - 2;

            if (listeTok.len > 0)
            {
                Tree* fils = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                createExpressionTreeAux(fils, listeAst, &listeTok, lines, offset + 1);

                if (global_env->CODE_ERROR != 0) {
                    tree_destroy(fils);
                    return;
                }

                if (fils->type != TYPE_VIRGULE) {
                    tree_appendSon(tree, fils);
                } else {
                    free(tree->sons);
                    tree->sons = fils->sons;
                    tree->capacity = fils->capacity;
                    tree->nbSons = fils->nbSons;
                    free(fils->label1);
                    tree->label2 = fils->label2;
                    tree->data = fils->data;
                    tree->variable = fils->variable;
                    free(fils);
                }
            }
            
            tree->label1 = NULL;
            
            tree->type = TYPE_LIST;
            tree->data = NEO_VOID;
            
        }
        else if (ast[0]->type == TYPE_EXPRESSION) {
            // sous-expression prioritaires, entourées ou non de parenthèses
            // on doit donc effectuer un appel récursif dessus en enlevant l'étage EXPRESSION et en enlevant un étage de parenthèses
            ast_pop(ast[0]);

            // expression entourée de parenthèses, on les enlève elles ne servent à rien
            if (strcmp(tokens->tab[0], "(") == 0 && strcmp(tokens->tab[tokens->len - 1], ")") == 0)
            {
                strlist tokensAux = {.tab = tokens->tab + 1, .len = tokens->len - 2};
                createExpressionTreeAux(tree, ast + 1, &tokensAux, lines, offset + 1);
            }
            else
            {
                createExpressionTreeAux(tree, ast, tokens, lines, offset);
            }
        }
        else if (ast[0]->type != TYPE_PARENTHESE1 && ast[0]->type != TYPE_PARENTHESE2 && ast[0]->type != TYPE_ENDOFLINE) { // si c'est des parenthèses ou un retour à la ligne, on ignore
            if (strcmp("...", tokens->tab[0]) == 0)
                global_env->CODE_ERROR = 97;
            else {
                global_env->CODE_ERROR = 32;
            }
            global_env->LINENUMBER = lines->tab[offset];

            return ;
        }
        
    }
    else if (tokens->len > 1)
    {
        
        int nbVirgules = ast_typeCountAst(ast, tokens->len, TYPE_VIRGULE, offset);
        
        if (nbVirgules > 0)
        {

            tree->data = NEO_VOID;
            
            
            tree->label1 = strdup(",");
            tree->type = TYPE_VIRGULE;
            tree->line = lines->tab[offset];
            
            Tree* arbre;
            int index = 0, len;
            strlist sous_tokens;            
            
            for (int j=0 ; j <= nbVirgules ; j++)// boucle qui parcourt toutes les virgules
            {
                int debut = index;
                // avance index jusqu'à la prochaine virgule ou la fin de l'ast
                while (index < tokens->len && ast[index]->type != TYPE_VIRGULE)
                    index = ast[index]->fin + 1 - offset;


                // index va un coup trop loin

                arbre = tree_create(NULL, 0, TYPE_SYNTAXTREE);

                sous_tokens.tab = tokens->tab + debut;
                sous_tokens.len = index - debut;
                createExpressionTreeAux(arbre, ast + debut, &sous_tokens, lines, offset + debut);

                if (global_env->CODE_ERROR != 0) {
                    tree_destroy(arbre);
                    free(tree->label1);
                    tree->label1 = NULL;
                    return;
                }

                tree_appendSon(tree, arbre);

                index ++;
                
            }
        }
        else
        {
            // dans le cas où il y a des opérateurs dedans

            // on cherche l'indice de l'opérateur le moins prioritaire de l'AST
            int index = ast_minOp(ast, tokens, offset);

            if (index < 0) {
                global_env->CODE_ERROR = 30;
                global_env->LINENUMBER = lines->tab[offset];
                return;
            }
            
            if (strcmp(tokens->tab[index], ":") == 0) // erreur : l'opérateur : n'est pas un opérateur normal
            {
                global_env->CODE_ERROR = 92;
                global_env->LINENUMBER = lines->tab[offset + index];
                return;
            }
            
            int placeOp = get_operator_index(tokens->tab[index]);
            
            int typeOperande = get_type_operande_index(placeOp);
            
            tree->label1 = NULL;
            tree->label2 = placeOp;
            tree->type = TYPE_OPERATOR;
            tree->line = lines->tab[offset + index];


            if (typeOperande & RIGHT_LEFT)
            {

                // si c'est un opérateur '.', on va changer la structure de l'arbre, donc c'est une opération différente
                if (tree->label2 == 30) // si c'est un point
                {
                    free(tree->label1);
                    tree->label1 = NULL;

                    strlist tokensGauche = (strlist) {.tab = tokens->tab, .len = index};
                    Ast** astGauche = ast;
                    
                    strlist tokensDroite = (strlist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                    Ast** astDroite = ast + index + 1;

                    createExpressionTreeAux(tree, astDroite, &tokensDroite, lines, offset + index + 1);


                    if (global_env->CODE_ERROR != 0)
                        return ;


                    Tree* fils = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                    if (global_env->CODE_ERROR != 0)
                        return;

                    createExpressionTreeAux(fils, astGauche, &tokensGauche, lines, offset);


                    if (global_env->CODE_ERROR != 0)
                    {
                        tree_destroy(fils);
                        return;
                    }
                    
                    
                    // on va rajouter en premier argument de la fonction l'opérande de gauche
                    if (tree->type != TYPE_FONCTION)
                    {
                        tree_destroy(fils);
                        global_env->CODE_ERROR = 72;
                        global_env->LINENUMBER = tree->line;

                        return;
                    }

                    tree_appendSon(tree->sons[1], NULL); // augmentation de la taille du tableau
                    // décalage des éléments
                    for (int i = tree->sons[1]->nbSons - 1 ; i > 0 ; i--)
                        tree->sons[1]->sons[i] = tree->sons[1]->sons[i-1];

                    tree->sons[1]->sons[0] = fils; // la premier élément est la variable

                    return;
                }
                else if (tree->label2 == 34) // opérateur >>
                {
                    free(tree->label1);
                    tree->label1 = NULL;
                    tree->label2 = 0;


                    // maintenant, il se peut que le membre de droite soit de la forme fonc(truc) ou encore tab[truc]

                    if (ast[index+1]->type != TYPE_VARIABLE)
                    {
                        // dans ce cas, on veut réorganiser les priorités pour évaluer d'abord la fonction et l'index avant
                        // le container

                        // comme l'opérateur est le plus prioritaire, on sait que la partie de gauche commence en zéro

                        // on met entre parenthèses (container>>field)
                        ast_push(ast[0]);
                        ast[0]->fin = index + 1 + offset;
                        ast[0]->type = TYPE_EXPRESSION;


                        // on sauvegarde les constructions syntaxiques au dessus de l'identificateur juste à droite
                        intlist stack_sov_typ = intlist_create(0);
                        intlist stack_sov_fin = intlist_create(0);

                        while (ast[index+1]->type != TYPE_VARIABLE) {
                            intlist_append(&stack_sov_typ, ast[index+1]->type);
                            intlist_append(&stack_sov_fin, ast[index+1]->fin);
                            int r = ast_pop(ast[index+1]);

                            if (r == -1) { // cet ast n'arrivera jamais à un type variable
                                free(stack_sov_fin.tab);
                                free(stack_sov_typ.tab);
                                global_env->CODE_ERROR = 82;
                                global_env->LINENUMBER = lines->tab[offset + index + 1];
                                return;
                            }
                        }

                        // on remet tout ça mais directement sur le container>>field (par dessus l'expression)
                        while (stack_sov_typ.len > 0) {
                            ast_push(ast[0]);
                            ast[0]->fin = stack_sov_fin.tab[stack_sov_fin.len - 1];
                            ast[0]->type = stack_sov_typ.tab[stack_sov_typ.len - 1];
                            intlist_remove(&stack_sov_fin, stack_sov_fin.len - 1);
                            intlist_remove(&stack_sov_typ, stack_sov_typ.len - 1);
                        }

                        // on se ramènera plus tard au cas de base
                        createExpressionTreeAux(tree, ast, tokens, lines, offset);

                        free(stack_sov_fin.tab);
                        free(stack_sov_typ.tab);
                    }
                    else {
                        strlist tokensGauche = (strlist){.tab = tokens->tab, .len = index};
                        
                        Tree* t = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                        if (global_env->CODE_ERROR != 0)
                        {
                            return ;
                        }
                        
                        createExpressionTreeAux(t, ast, &tokensGauche, lines, offset);

                        if (global_env->CODE_ERROR != 0)
                        {
                            tree_destroy(t);
                            return ;
                        }

                        tree->label1 = strdup(tokens->tab[index + 1]); // le nom du champ
                        tree_appendSon(tree, t);

                        tree->label2 = -1; // ce champ sera changé pendant l'exécution
                        tree->type = TYPE_ATTRIBUTE;
                    }
                    return;
                }
                else
                {

                    Tree* filsg = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                    if (global_env->CODE_ERROR != 0)
                    {
                        free(tree->label1);
                        tree->label1 = NULL;
                        return;
                    }

                    Tree* filsd = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                    if (global_env->CODE_ERROR != 0)
                    {
                        free(tree->label1);
                        tree->label1 = NULL;
                        tree_destroy(filsg);
                        return;
                    }
                    
                    // il va falloir séparer en deux l'expression, et recalculer l'arbre des expressions sous jacentes
                    
                    strlist tokensGauche = (strlist){.tab = tokens->tab, .len = index};
                    
                    strlist tokensDroite = (strlist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                    
                    createExpressionTreeAux(filsg, ast, &tokensGauche, lines, offset);

                    if (global_env->CODE_ERROR != 0)
                    {
                        tree_destroy(filsg);
                        tree_destroy(filsd);
                        return;
                    }
                    createExpressionTreeAux(filsd, ast + index + 1, &tokensDroite, lines, offset + index + 1);
                    if (global_env->CODE_ERROR != 0)
                    {
                        tree_destroy(filsg);
                        tree_destroy(filsd);
                        return;
                    }
                    
                    tree_appendSon(tree, filsg);
                    tree_appendSon(tree, filsd);
                }
                
            }
            
            else if (typeOperande & VARLEFT) // variable à gauche comme -- ou ++
            {
                
                Tree* fils = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                if (global_env->CODE_ERROR != 0)
                    return;
                
                strlist ntokens = (strlist) {.tab = tokens->tab, .len = tokens->len-1};
                
                
                createExpressionTreeAux(fils, ast, &ntokens, lines, offset);
                if (global_env->CODE_ERROR != 0)
                {
                    tree_destroy(fils);
                    free(tree->label1);
                    tree->label1 = NULL;
                    return ;
                }
                tree_appendSon(tree, fils);
                
                
            }
            
            else if (typeOperande & VAR_RIGHT) // cas où il y a potentiellement un opérateur point : '.'
            {
            
                Tree* filsg = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                if (global_env->CODE_ERROR != 0)
                {
                    free(tree->label1);
                    tree->label1 = NULL;
                    return;
                }

                Tree* filsd = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                
                if (global_env->CODE_ERROR != 0)
                {
                    tree_destroy(filsg);
                    free(tree->label1);
                    tree->label1 = NULL;
                    return;
                }
                // il va falloir séparer en deux l'expression, et recalculer l'arbre des expressions sous jacentes
                
                strlist tokensGauche = (strlist){.tab = tokens->tab, .len = index};
                
                
                strlist tokensDroite = (strlist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};

                
                createExpressionTreeAux(filsd, ast + index + 1, &tokensDroite, lines, offset + index + 1);
                
                if (global_env->CODE_ERROR != 0)
                {
                    tree_destroy(filsg);
                    tree_destroy(filsd);
                    return;
                }
                createExpressionTreeAux(filsg, ast, &tokensGauche, lines, offset);
                
                if (global_env->CODE_ERROR != 0)
                {
                    free(tree->label1);
                    tree->label1 = NULL;
                    tree_destroy(filsg);
                    tree_destroy(filsd);
                    return;
                }

                
                tree_appendSon(tree, filsg);
                tree_appendSon(tree, filsd);
                
            }



            else if (typeOperande & LEFT_VAR) // cas où il y a potentiellement un opérateur point : '.'
            {
            
                Tree* filsg = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                if (global_env->CODE_ERROR != 0)
                {
                    free(tree->label1);
                    tree->label1 = NULL;
                    return;
                }

                Tree* filsd = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                
                if (global_env->CODE_ERROR != 0)
                {
                    tree_destroy(filsg);
                    free(tree->label1);
                    tree->label1 = NULL;
                    return;
                }
                // il va falloir séparer en deux l'expression, et recalculer l'arbre des expressions sous jacentes
                
                strlist tokensGauche = (strlist){.tab = tokens->tab, .len = index};
                
                strlist tokensDroite = (strlist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                
                createExpressionTreeAux(filsd, ast + index + 1, &tokensDroite, lines, offset + index + 1);
                
                if (global_env->CODE_ERROR != 0)
                {
                    free(tree->label1);
                    tree->label1 = NULL;
                    tree_destroy(filsg);
                    tree_destroy(filsd);
                    return;
                }
                createExpressionTreeAux(filsg, ast, &tokensGauche, lines, offset);
                
                if (global_env->CODE_ERROR != 0)
                {
                    free(tree->label1);
                    tree->label1 = NULL;
                    tree_destroy(filsg);
                    tree_destroy(filsd);
                    return;
                }

                
                tree_appendSon(tree, filsg);
                tree_appendSon(tree, filsd);
                
            }


            else if (typeOperande & VAR_VAR) // cas où il y a potentiellement un opérateur point : '.'
            {
            
                Tree* filsg = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                if (global_env->CODE_ERROR != 0)
                {
                    free(tree->label1);
                    tree->label1 = NULL;
                    return;
                }

                Tree* filsd = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                
                if (global_env->CODE_ERROR != 0)
                {
                    tree_destroy(filsg);
                    free(tree->label1);
                    tree->label1 = NULL;
                    return;
                }
                // il va falloir séparer en deux l'expression, et recalculer l'arbre des expressions sous jacentes
                
                strlist tokensGauche = (strlist){.tab = tokens->tab, .len = index};
                
                strlist tokensDroite = (strlist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                
                createExpressionTreeAux(filsd, ast + index + 1, &tokensDroite, lines, offset + index + 1);
                
                if (global_env->CODE_ERROR != 0)
                {
                    free(tree->label1);
                    tree->label1 = NULL;
                    tree_destroy(filsg);
                    tree_destroy(filsd);
                    return;
                }
                createExpressionTreeAux(filsg, ast, &tokensGauche, lines, offset);
                
                if (global_env->CODE_ERROR != 0)
                {
                    free(tree->label1);
                    tree->label1 = NULL;
                    tree_destroy(filsg);
                    tree_destroy(filsd);
                    return;
                }

                
                tree_appendSon(tree, filsg);
                tree_appendSon(tree, filsd);
                
            }


            
            else if (typeOperande & VARRIGHT || typeOperande & RIGHT)
            {
                
                
                Tree* fils = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                if (global_env->CODE_ERROR != 0)
                    return;
                
                
                strlist ntokens = (strlist) {.tab = tokens->tab+1, .len = tokens->len-1};
                
                
                createExpressionTreeAux(fils, ast + 1, &ntokens, lines, offset + 1);
                if (global_env->CODE_ERROR != 0)
                {
                    tree_destroy(fils);
                    return;
                }

                if (tree->label2 == 39 && fils->type != TYPE_FONCTION) { // pour parallel
                    tree_destroy(fils);
                    global_env->CODE_ERROR = 100;
                    global_env->LINENUMBER = fils->line;
                    return;
                }


                tree_appendSon(tree, fils);
                
            }

            else if (typeOperande == 0) {
                // cas des opérateurs inertes comme ... : il ne doit pas être en dehors d'une définition de fonction
                global_env->CODE_ERROR = 97;
                global_env->LINENUMBER = lines->tab[offset];
                return;
                
            }
            
        }
        
    }
}











void createExpressionTree(Tree* tree, char* string)
{
    strlist* tokens = strlist_create(0);
    intlist types = intlist_create(0);
    intlist lines = intlist_create(0);
    Ast** ast;

    //printf("Début cut\n");

    cut(tokens, &types, string, true, &lines);

    if (global_env->CODE_ERROR != 0) {
        free(types.tab);
        free(lines.tab);
        strlist_destroy(tokens, true);
        return;
    }

    ast = ast_create(&types);

    //printf("Debut parse\n");
        
    parse(tokens, types, ast, &lines, 0);

    if (global_env->CODE_ERROR != 0) {
        free(types.tab);
        free(lines.tab);
        ast_destroy(ast, tokens->len);
        strlist_destroy(tokens, true);
        return;
    }

    statements(&types, tokens, ast, &lines, 0);
    
    createExpressionTreeAux(tree, ast, tokens, &lines, 0);
    tree->label1 = NULL;


    ast_destroy(ast, tokens->len);

    strlist_destroy(tokens, true);
    free(types.tab);
    free(lines.tab);

    return;
    
}






void affExpr(Tree* tree)
{
    if (tree->type == TYPE_OPERATOR)
    {
        int gramm = get_type_operande_index(get_operator_index(tree->label1));

        if (strcmp(tree->label1, "_") == 0)
        {
            free(tree->label1);
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
        NeObj fonction = eval(tree->sons[0]);

        char* nom = global_env->NOMS->tab[nelist_index2(global_env->ADRESSES, fonction)];
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

        neobject_destroy(fonction);
    }

    if (tree->type == TYPE_VARIABLE)
    {
        char* nom = global_env->NOMS->tab[nelist_index(global_env->ADRESSES, tree->data)];
        printString(nom);
    }

    if (tree->type == TYPE_NONE || tree->type == TYPE_BOOL || tree->type == TYPE_CONST || tree->type == TYPE_EXCEPTION || tree->type == TYPE_INTEGER || tree->type == TYPE_DOUBLE)
    {
        neobject_aff(tree->data);
    }

    if (tree->type == TYPE_STRING)
    {
        char* string = neo_to_string(tree->data);
        //char* str2 = traitementStringInverse(string);
        printString("\"");
        printString(string);
        printString("\"");
        //free(str2);
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
        printString(global_env->CONTAINERS->tab[tree->label2]);
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







/*
On suppose que le bloc qu'on reçoit n'a pas été poppé de sa condition de bloc
*/
void createStatementIEWFTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset, int type)
{
    ast_pop(ast[0]); // on pop le bloc global

    strlist blocToks = (strlist) {.tab = tokens->tab + ast[0]->fin - offset + 2, .len = tokens->len - (ast[0]->fin - offset + 1) - 2};
    Ast** blocAst = ast + ast[0]->fin - offset + 2;

    strlist exprToks = (strlist) {.tab = tokens->tab + 2, .len = ast[0]->fin - offset - 2};
    Ast** exprAst = ast + 2;

    
    Tree* fils1 = tree_create(NULL, 0, TYPE_SYNTAXTREE);
    if (global_env->CODE_ERROR != 0)
    {
        return;
    }
    createExpressionTreeAux(fils1, exprAst, &exprToks, lines, offset + 2);

    
    Tree* fils2 = tree_create(NULL, 0, TYPE_SYNTAXTREE);
    if (global_env->CODE_ERROR != 0)
    {
        tree_destroy(fils1);
        return;
    }
    
    createSyntaxTreeAux(fils2, blocAst, &blocToks, lines, offset + ast[0]->fin - offset + 2);

    if (global_env->CODE_ERROR != 0)
    {
        tree_destroy(fils1);
        tree_destroy(fils2);
        return;
    }
    
    fils2->type = TYPE_BLOCK;
    
    tree_appendSon(tree, fils1);
    tree_appendSon(tree, fils2);

    tree->type = type;
    tree->line = lines->tab[offset];
    tree->data = NEO_VOID;

    return ;
}






void createStatementElseTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset)
{
    ast_pop(ast[0]);

    strlist bloc = (strlist) {.tab = tokens->tab + 2, .len = tokens->len - 3};
    
    createSyntaxTreeAux(tree, ast + 2, &bloc, lines, offset + 2);

    //s'il y a une erreur, la fonction va quitter normalement et global_env->CODE_ERROR va faire le reste
    
    tree->type = TYPE_STATEMENTELSE;
    tree->data = NEO_VOID;
    tree->line = lines->tab[offset];
    return ;
}








void createConditionBlockTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset)
{
    ast_pop(ast[0]);

    int i = 0, index2 = 0;
    
    while (i < tokens->len)
    {
        index2 = ast[i]->fin - offset + 1; // on regarde maintenant où on va sauter après

        if (ast[i]->type == TYPE_STATEMENTELSE)
        {
            Tree* fils = tree_create(NULL, 0, TYPE_SYNTAXTREE);
            if (global_env->CODE_ERROR != 0)
            {
                printString("b20\n");
                return;
            }

            strlist elseToks = (strlist) {.tab = tokens->tab + i, .len = ast[i]->fin - offset + 1 - i};

            createStatementElseTree(fils, ast + i, &elseToks, lines, offset + i);

            if (global_env->CODE_ERROR != 0)
            {
                printString("b21\n");
                tree_destroy(fils);
                return;
            }

            tree_appendSon(tree, fils);
        }
        else if (ast[i]->type == TYPE_STATEMENTIF || ast[i]->type == TYPE_STATEMENTELIF)
        {
            Tree* fils = tree_create(NULL, 0, TYPE_SYNTAXTREE);
            if (global_env->CODE_ERROR != 0)
            {
                printString("b22\n");
                return;
            }

            strlist blocToks = (strlist) {.tab = tokens->tab + i, .len = ast[i]->fin - offset + 1 - i};

            createStatementIEWFTree(fils, ast + i, &blocToks, lines, offset + i, ast[i]->type);
            if (global_env->CODE_ERROR != 0)
            {
                printString("b23\n");
                tree_destroy(fils);
                return;
            }
            tree_appendSon(tree, fils);
        }

        i = index2;

    }
    
    tree->type = TYPE_CONDITIONBLOCK;
    tree->data = NEO_VOID;
    tree->line = lines->tab[offset];
    
}




void createFunctionTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset, _Bool isMethod)
{

    int i = 0;
    while (strcmp(tokens->tab[i], "{") != 0) i++;
    // ici, i a la valeur de l'index de la première accolade

    strlist codeTok = (strlist) {.tab = tokens->tab + i + 1, .len = tokens->len - i - 2};

    Tree* syntaxTree = tree_create(NULL, 0, TYPE_SYNTAXTREE);
    if (global_env->CODE_ERROR != 0) {
        return;
    }

    createSyntaxTreeAux(syntaxTree, ast + i + 1, &codeTok, lines, offset + i + 1);


    if (global_env->CODE_ERROR != 0)
    {
        tree_destroy(syntaxTree);
        return;
    }

    //ajout de l'arbre de la fonction à la liste des fonctions
    tree_appendSon(global_env->FONCTIONS, syntaxTree);

    if (global_env->CODE_ERROR != 0) {
        tree_destroy(syntaxTree);
        return;
    }

    char* name = tokens->tab[1];

    strlist argsTok = (strlist) {.tab = tokens->tab + 3, .len = i - 4};
    Ast** argsAst = ast + 3;
    int args_offset = offset + 3;

    // majorant du nombre d'arguments
    int size_liste = strlist_count(&argsTok, ",") + 1;
    Var* liste = malloc(sizeof(Var)*size_liste); // tableau qui va contenir toutes les variables à affecter
    int liste_index = 0;

    int nbOptArgs = 0;
    _Bool unlimited_arguments = false;
    Tree* opt_args = tree_create(NULL, 0, TYPE_SYNTAXTREE); // même si la fonction n'a pas d'arguments elle a un arbre

    if (global_env->CODE_ERROR != 0) {
        free(liste);
        return;
    }

    if (argsTok.len > 0) // si la fonction a des arguments
    {
        // il faut maintenant définir chaque variable dont le nom est dans tokens
        
        for (i = 0 ; i < argsTok.len ; i = argsAst[i]->fin - args_offset + 1)
        {
            if (strcmp(argsTok.tab[i], ",") != 0) // si ce n'est pas une virgule, c'est une variable (enfin on vérifie)
            {

                if (argsAst[i]->type == TYPE_VARIABLE)
                {

                    liste[liste_index++] = get_var(argsTok.tab[i]);

                    // il faut vérifier si c'est un argument optionnel ou pas

                    if (ast_index(argsAst, i, args_offset) < ast_length(argsAst, argsTok.len, args_offset) - 2 && strcmp(argsTok.tab[argsAst[i]->fin - args_offset + 1], ":=") == 0) // argument optionnel
                    {
                        if (unlimited_arguments)
                            nbOptArgs++;

                        // dans ce cas, on évalue l'expression et on l'ajoute à l'arbre opt_args
                        int j = i;
                        while (j < argsTok.len && strcmp(argsTok.tab[j], ",") != 0)
                            j = argsAst[j]->fin - args_offset + 1; // va à la fin de l'expression

                        // création d'une sous liste, puis calcul de l'arbre de cette sous liste
                        strlist tokensarg = (strlist) {.tab = argsTok.tab + i+2, .len = j-i-2};
                        
                        Tree* exp = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                        createExpressionTreeAux(exp, argsAst + i + 2, &tokensarg, lines, args_offset + i + 2);

                        if (global_env->CODE_ERROR != 0) {
                            tree_destroy(opt_args);
                            free(liste);
                            return ;
                        }

                        tree_appendSon(opt_args, exp); // on ajoute l'expression aux arguments optionnels

                        i = ast_prec(argsAst, j, args_offset);
                    }
                    else if (ast_index(argsAst, i, args_offset) < ast_length(argsAst, argsTok.len, args_offset) - 1 && strcmp(argsTok.tab[i+1], ",") != 0) {
                        // erreur de syntaxe
                        global_env->CODE_ERROR = 96;
                        global_env->LINENUMBER = lines->tab[offset + args_offset + i];
                        tree_destroy(opt_args);
                        free(liste);
                        return ;
                    }
                    else { // argument non optionnel
                        if (unlimited_arguments) {
                            // erreur : on ne peut pas mettre d'arguments obligatoires après ...
                            global_env->CODE_ERROR = 96;
                            global_env->LINENUMBER = lines->tab[offset + args_offset + i];
                            tree_destroy(opt_args);
                            free(liste);
                            return ;
                        }
                        tree_appendSon(opt_args, NULL);
                    }
                }
                else if (strcmp(argsTok.tab[i], "...") == 0) {
                    // nombre infini de paramètres
                    // s'il y a encore des choses après, c'est une erreur de syntaxe
                    
                    // la fonction prend un nombre illimité de paramètres
                    unlimited_arguments = true;
                }
                else {
                    global_env->CODE_ERROR = 96;
                    global_env->LINENUMBER = lines->tab[offset + args_offset + i];
                    tree_destroy(opt_args);
                    free(liste);
                    return ;
                }
            }
        }
    }

    // création de la fonction partielle servant de base pour définir des fonctions

    if (isMethod) { // pour être une méthode il faut avoir au moins un argument
        if (liste_index == 0) {
            global_env->CODE_ERROR = 110;
            return;
        }
        else
            tree->data = userFuncCreate(liste, syntaxTree, liste_index, unlimited_arguments, nbOptArgs, NULL, TYPE_USERMETHOD); // objet destiné à être dans l'arbre
    }
    else {
        tree->data = userFuncCreate(liste, syntaxTree, liste_index, unlimited_arguments, nbOptArgs, NULL, TYPE_USERFUNC); // objet destiné à être dans l'arbre
    }
    

    tree_appendSon(tree, opt_args);
    
    tree->label1 = strdup(name);
    tree->line = lines->tab[offset];
    tree->type = TYPE_FUNCTIONDEF;

    return ;
}





void createSyntaxTreeAux(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset) {

    _Bool expression = false;
    int exprStart = 0;

    int index = 0, index2 = 0;

    printString("b15 ");
    printInt(ast[0]->type);
    newLine();

    printString("Code d'erreur : ");
    printInt(global_env->CODE_ERROR);
    newLine();

    while (index < tokens->len) {

        index2 = ast[index]->fin - offset + 1; // on regarde où on va sauter, et on saute à cet endroit après
        // c'est important, car on modifie ast[index] dans le code qui suit

        

        if (ast[index]->type == TYPE_TRYEXCEPT) {
            ast_pop(ast[index]);
            // on commence par le bloc try

            // on a donc forcément 4 objets dans tokens2
            Tree* try_except = tree_create(NULL, 0, TYPE_SYNTAXTREE);
            Tree* try_tree = tree_create(NULL, 0, TYPE_SYNTAXTREE);
            

            // try
            strlist tryTok = (strlist) {.tab = tokens->tab + index + 2, .len = ast[index]->fin - offset - index - 2};

            createSyntaxTreeAux(try_tree, ast + index + 2, &tryTok, lines, offset + index + 2);

            if (global_env->CODE_ERROR != 0)
            {
                tree_destroy(try_except);
                tree_destroy(try_tree);
                return ;
            }

            try_tree->type = TYPE_STATEMENTTRY;
            tree_appendSon(try_except, try_tree);


            //except
            int i = ast[index]->fin - offset + 1;
            // on se déplace jusqu'au premier except
            while (i < index2 && ast[i]->type != TYPE_STATEMENTEXCEPT)
                i = ast[i]->fin - offset + 1;

            while (i < index2) {
                Tree* except_tree = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                Tree* except_expr = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                Tree* except_bloc = tree_create(NULL, 0, TYPE_SYNTAXTREE);

                int fin = ast[i]->fin - offset;
                ast_pop(ast[i]); // on est sur le blockline

                strlist blocToks = (strlist) {.tab = tokens->tab + ast[i]->fin - offset + 2, .len = fin - (ast[i]->fin - offset + 2)};
                
                createSyntaxTreeAux(except_bloc, ast + ast[i]->fin - offset + 2, &blocToks, lines, offset + ast[i]->fin - offset + 2);
                
                if (global_env->CODE_ERROR != 0)
                {
                    tree_destroy(try_except);
                    tree_destroy(except_tree);
                    tree_destroy(except_expr);
                    tree_destroy(except_bloc);
                    return ;
                }

                strlist exprToks = (strlist) {.tab = tokens->tab + i + 2, .len = ast[i]->fin - offset - i - 2};

                if (exprToks.len > 0) // s'il y a au moins un argument
                {
                    createExpressionTreeAux(except_expr, ast + i + 2, &exprToks, lines, offset + i + 2);

                    if (global_env->CODE_ERROR != 0)
                    {
                        tree_destroy(try_except);
                        tree_destroy(except_tree);
                        tree_destroy(except_expr);
                        tree_destroy(except_bloc);
                        return;
                    }


                    if (except_expr->type != TYPE_VIRGULE) // il faut augmenter encore en profondeur
                    {
                        Tree* t = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                        tree_appendSon(t, except_expr);
                        tree_appendSon(except_tree, t);
                    }
                    else
                    {
                        tree_appendSon(except_tree, except_expr);
                    }
                }
                else
                {
                    tree_appendSon(except_tree, except_expr);
                }
                
                tree_appendSon(except_tree, except_bloc);
                except_tree->type = TYPE_STATEMENTEXCEPT;
                tree_appendSon(try_except, except_tree);



                // on se déplace jusqu'au prochain except
                while (i < index2 && ast[i]->type != TYPE_STATEMENTEXCEPT)
                    i = ast[i]->fin - offset + 1;

            }

            try_except->type = TYPE_TRYEXCEPT;
            
            tree_appendSon(tree, try_except);
        }
        else if (ast[index]->type == TYPE_ATOMICBLOCK) {

            Tree* corps = tree_create(NULL, 0, TYPE_SYNTAXTREE);

            if (global_env->CODE_ERROR != 0) {
                return;
            }
            
            strlist blocTok = (strlist) {.tab = tokens->tab + index + 2, .len = ast[index]->fin - offset - index - 2};

            createSyntaxTreeAux(corps, ast + index + 2, &blocTok, lines, offset + index + 2);

            if (global_env->CODE_ERROR != 0) {
                tree_destroy(corps);
                return ;
            }

            corps->type = TYPE_ATOMICBLOCK;
            tree_appendSon(tree, corps);
        }
        else if (ast[index]->type == TYPE_BLOCKWORD1LINE) {
            char* name = tokens->tab[index];
            strlist argsTok = (strlist) {.tab = tokens->tab + index + 2, .len = ast[index]->fin - offset - index - 2};
            Ast** argsAst = ast + index + 2;

            Tree* fils = tree_create(NULL, 0, TYPE_SYNTAXTREE);

            if (global_env->CODE_ERROR != 0) {
                return;
            }
            
            if (argsTok.len > 0) // s'il y a au moins un argument
            {
                // création de l'arbre des arguments

                _Bool virgules = ast_typeInAst(argsAst, argsTok.len, TYPE_VIRGULE, offset + index + 2);
                
                if (virgules)
                {
                    createExpressionTreeAux(fils, argsAst, &argsTok, lines, offset + index + 2);
                    if (global_env->CODE_ERROR != 0)
                    {
                        tree_destroy(fils);
                        return;
                    }
                }
                else
                {
                    Tree* fils2 = tree_create(NULL, 0, TYPE_SYNTAXTREE);
                    if (global_env->CODE_ERROR != 0)
                    {
                        tree_destroy(fils);
                        return;
                    }
                    
                    createExpressionTreeAux(fils2, argsAst, &argsTok, lines, offset + index + 2);
                    if (global_env->CODE_ERROR != 0)
                    {
                        tree_destroy(fils);
                        tree_destroy(fils2);
                        return;
                    }
                    tree_appendSon(fils, fils2);
                }
            }
            
            fils->label2 = get_blockword1Line_index(name) + 1;

            fils->type = TYPE_BLOCKWORD1LINE;
            
            tree_appendSon(tree, fils);
        }
        else if (ast[index]->type == TYPE_FUNCTIONDEF || ast[index]->type == TYPE_METHODDEF) {
            Tree* fils = tree_create(NULL, 0, TYPE_SYNTAXTREE);
            if (global_env->CODE_ERROR != 0) {
                return;
            }

            strlist stru = (strlist) {.tab = tokens->tab + index, .len = ast[index]->fin - offset + 1 - index};
            
            createFunctionTree(fils, ast + index, &stru, lines, offset + index, ast[index]->type == TYPE_METHODDEF);

            if (global_env->CODE_ERROR != 0)
            {
                tree_destroy(fils);
                return;
            }

            tree_appendSon(tree, fils);
        }
        else if (ast[index]->type == TYPE_KEYWORD) {
            Tree * fils = tree_create(NULL, get_lkeywords_index(tokens->tab[index]) + 1, TYPE_KEYWORD);
            if (global_env->CODE_ERROR != 0)
            {
                return;
            }
            
            tree_appendSon(tree, fils);
        }
        else if (ast[index]->type == TYPE_STATEMENTFOR || ast[index]->type == TYPE_STATEMENTFOREACH || ast[index]->type == TYPE_STATEMENTWHILE)  {
            Tree * fils = tree_create(NULL, 0, TYPE_SYNTAXTREE);

            if (global_env->CODE_ERROR != 0)
                return;

            strlist stru = (strlist) {.tab = tokens->tab + index, .len = ast[index]->fin - offset + 1 - index};
            
            createStatementIEWFTree(fils, ast + index, &stru, lines, offset + index, ast[index]->type);

            if (global_env->CODE_ERROR != 0) {
                tree_destroy(fils);
                return;
            }

            tree_appendSon(tree, fils);
        }
        else if (ast[index]->type == TYPE_CONDITIONBLOCK) {
            
            strlist conBlock = (strlist) {.tab = tokens->tab + index, .len = ast[index]->fin - offset - index + 1};
            Tree* st = tree_create(NULL, 0, TYPE_SYNTAXTREE);

            createConditionBlockTree(st, ast + index, &conBlock, lines, offset + index);

            if (global_env->CODE_ERROR != 0) {
                tree_destroy(st);
                printString("b10\n");
                return;
            }

            tree_appendSon(tree, st);
        }
        else if (!expression && ast[index]->type != TYPE_ENDOFLINE) { // this is of course the beginning of an expression
            expression = true;
            exprStart = index;
        }

        else if (expression && ast[index]->type == TYPE_ENDOFLINE) { // si l'expression est terminée, on la compute
            
            // compute an expression between exprStart and index
            strlist exprToks = (strlist) {.tab = tokens->tab + exprStart, .len = index - exprStart};
            Tree* expr = tree_create(NULL, 0, TYPE_SYNTAXTREE);

            createExpressionTreeAux(expr, ast + exprStart, &exprToks, lines, offset + exprStart);

            if (global_env->CODE_ERROR != 0) {
                tree_destroy(expr);
                printString("b11\n");
                return;
            }

            tree_appendSon(tree, expr);

            expression = false;
        }
        
        // pour permettre de commencer et finir des expressions durant le même tour dans le seul cas d'une expression tout à la fin
        if (expression && index2 >= tokens->len) { // on sait qu'au prochain tour on sera partis, donc on prend tout le reste
            
            strlist exprToks = (strlist) {.tab = tokens->tab + exprStart, .len = tokens->len - exprStart};
            Tree* expr = tree_create(NULL, 0, TYPE_SYNTAXTREE);

            createExpressionTreeAux(expr, ast + exprStart, &exprToks, lines, offset + exprStart);

            if (global_env->CODE_ERROR != 0) {
                tree_destroy(expr);
                printString("b12\n");
                return;
            }

            tree_appendSon(tree, expr);

            expression = false;
        }

        index = index2;

    }

    tree->type = TYPE_SYNTAXTREE;
    tree->data = NEO_VOID;
    tree->line = lines->tab[offset];

}







/*
Cette fonction prend en argument une chaine de caractères représentant un programme, et un pointeur vers un arbre initialisé
Elle construit dans ce pointeur l'arbre syntaxique associé à la chaine de caractères
*/
void createSyntaxTree(Tree* tree, char* program)
{
    strlist* tokens = strlist_create(0);
    intlist types = intlist_create(0);
    intlist lines = intlist_create(0);
    Ast** ast;

    cut(tokens, &types, program, true, &lines);

    if (global_env->CODE_ERROR != 0) {
        free(types.tab);
        free(lines.tab);
        strlist_destroy(tokens, true);
        return;
    }

    ast = ast_create(&types);

    parse(tokens, types, ast, &lines, 0);

    if (global_env->CODE_ERROR != 0) {
        free(types.tab);
        free(lines.tab);
        ast_destroy(ast, tokens->len);
        strlist_destroy(tokens, true);
        return;
    }


    statements(&types, tokens, ast, &lines, 0);

    if (global_env->CODE_ERROR != 0) {
        free(types.tab);
        free(lines.tab);
        ast_destroy(ast, tokens->len);
        strlist_destroy(tokens, true);
        return;
    }

    printString("b0\n");
    createSyntaxTreeAux(tree, ast, tokens, &lines, 0);
    printString("b100\n");

    ast_destroy(ast, tokens->len);
    strlist_destroy(tokens, true);
    free(types.tab);
    free(lines.tab);

}


