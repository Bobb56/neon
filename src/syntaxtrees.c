#include <string.h>
#include <math.h>
#include "headers/constants.h"
#include "headers/neon.h"
#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/dynarrays.h"
#include "headers/parser.h"
#include "headers/runtime.h"
#include "headers/strings.h"
#include "headers/syntaxtrees.h"
#include "headers/trees.h"
#include "headers/errors.h"

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


void createVirguleTree(struct TreeList* tree_list, Ast** ast, toklist* tokens, intlist* lines, int offset) {
    int nbVirgules = ast_typeCountAst(ast, tokens->len, TYPE_VIRGULE, offset);
        
    if (nbVirgules > 0) {
        int index = 0;
        toklist sous_tokens;
        
        for (int j=0 ; j <= nbVirgules ; j++)// boucle qui parcourt toutes les virgules
        {
            int debut = index;
            // avance index jusqu'à la prochaine virgule ou la fin de l'ast
            while (index < tokens->len && ast[index]->type != TYPE_VIRGULE)
                index = ast[index]->fin + 1 - offset;


            // index va un coup trop loin

            sous_tokens.tab = tokens->tab + debut;
            sous_tokens.len = index - debut;
            NeTree arbre = createExpressionTreeAux(ast + debut, &sous_tokens, lines, offset + debut);

            return_on_error();

            TreeList_append(tree_list, arbre);

            return_on_error();

            index ++;
            
        }
    }
    else { // il y a soit une expression normale, soit rien tu tout
        int count = ast_typeCountAst(ast, tokens->len, TYPE_ENDOFLINE, offset);
        int real_length = ast_length(ast, tokens->len, offset);

        // si il y a des choses quand même
        if (count < real_length) {
            NeTree arbre = createExpressionTreeAux(ast, tokens, lines, offset);
            TreeList_append(tree_list, arbre);
            return_on_error();
        }
    }
}





NeTree createExpressionTreeAux(Ast** ast, toklist* tokens, intlist* lines, int offset)
{

    //printf("createExpressionTreeAux :\n");
    //toklist_aff(tokens);
    //ast_aff(ast, tokens->len);

    int count = ast_typeCountAst(ast, tokens->len, TYPE_ENDOFLINE, offset);
    int real_length = ast_length(ast, tokens->len, offset);
    
    // tous les tokens sont des retours à la ligne
    if (count == real_length) {
        printString("z0\n");
        toklist_aff(tokens);
        ast_aff(ast, tokens->len);
        global_env->LINENUMBER = lines->tab[offset];
        global_env->CODE_ERROR = 30;
        return TREE_VOID;
    }

    // il y a un seul token qui n'est pas un retour à la ligne
    else if (count == real_length - 1) // traitement des cas de base de la fonction récursive
    {
        if (real_length > 1) { // on enlève tous les retours à la ligne

            int i = 0;
            while (ast[i]->type == TYPE_ENDOFLINE) i++;

            toklist tokens2 = (toklist) {.tab = tokens->tab + i, .len = ast[i]->fin - offset - i + 1};

            return createExpressionTreeAux(ast + i, &tokens2, lines, offset + i);
        }

        if (ast[0]->type == TYPE_VARIABLE) {
            char sov = stringize(tokens->tab[0]);
            NeTree T = NeTree_make_variable(get_var(tokens->tab[0].debut), lines->tab[offset]);
            unstringize(tokens->tab[0], sov);
            return T;
        }

        else if (ast[0]->type == TYPE_BOOL) {
            char sov = stringize(tokens->tab[0]);
            NeTree T = NeTree_make_const(neo_bool_create(strToBool(tokens->tab[0].debut)), lines->tab[offset]);
            unstringize(tokens->tab[0], sov);
            return T;
        }

        else if (ast[0]->type == TYPE_EXCEPTION) {
            // créer une exception
            char sov = stringize(tokens->tab[0]);
            int index = strlist_index(global_env->EXCEPTIONS, tokens->tab[0].debut);
            unstringize(tokens->tab[0], sov);
            return NeTree_make_const(neo_exception_create(index), lines->tab[offset]);
        }


        else if (ast[0]->type == TYPE_CONST) {
            NeObj obj = NEO_VOID;
            if (tokeq(tokens->tab[0], get_none())) { // TYPE_NONE
                obj = neo_none_create();
            }
            else if (tokeq(tokens->tab[0], get_infinity())) {
                obj = neo_double_create(INFINITY);
            }
            else if (tokeq(tokens->tab[0], get_nan())) {
                obj = neo_double_create(NAN);
            }
            else if (tokeq(tokens->tab[0], get_pi())) {
                obj = neo_double_create(PI);
            }
            return NeTree_make_const(obj, lines->tab[offset]); // cet arbre se situe à la ligne [line] du fichier
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
            toklist argsTok;

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


            if (argsAst != NULL && toklist_ast_inList(&argsTok, argsAst, ":", TYPE_OPERATOR, offset + lenNomFonc + 1))
            {
                NeTree tree = NeTree_make_containerlit(lines->tab[offset]);

                return_on_error(TREE_VOID);

                Token nomFonc = tokens->tab[0];
                // ce n'est pas une fonction mais un constructeur de container, on réutilise le tableau
                
                // argsTok contient maintenant la liste des tokens des 'arguments' de l'objet
                
                int i = 0; // indice de tok

                while (i < argsTok.len - 2)
                {
                    while (argsAst[i]->type == TYPE_ENDOFLINE) i++;

                    NeTree t1 = NeTree_create(TypeAttributeLit, lines->tab[offset + argsAst_offset + i]);

                    if_error {
                        NeTree_destroy(tree);
                        return TREE_VOID;
                    }

                    if (argsAst[i]->type != TYPE_VARIABLE || !tokeq(argsTok.tab[i+1],":")) // le champ n'a pas un nom correct ou n'est pas suivi de :
                    {
                        global_env->CODE_ERROR = 87;
                        global_env->LINENUMBER = lines->tab[offset + argsAst_offset + i];
                        NeTree_destroy(t1);
                        NeTree_destroy(tree);
                        return TREE_VOID;
                    }

                    t1.attribute_lit->name = tokdup(argsTok.tab[i]); // nom du champ


                    i+=2; // on saute le ':'. On a le droit car la condition de la boucle while le permet

                    // calcule l'arbre du truc à droite du ':'
                    int k = i;
                    while (k < argsTok.len && argsAst[k]->type != TYPE_VIRGULE) // saute à l'argument suivant
                        k = argsAst[k]->fin - offset - argsAst_offset + 1; // l'offset de argsAst est lenNomFonc + 1

                    // prend la partie après la virgule, la première partie est 1 seul token que l'on prend au début de la boucle en ajoutant un de plus à i
                    toklist tok2 = (toklist) {.tab = argsTok.tab + i, .len = k-i};

                    t1.attribute_lit->expr = createExpressionTreeAux(argsAst + i, &tok2, lines, offset + argsAst_offset + i);

                    if_error
                    {
                        NeTree_destroy(t1);
                        NeTree_destroy(tree);
                        return TREE_VOID;
                    }

                    NeTree_containerlit_add_attribute(&tree, t1); // l'arbre est encore fils de l'arbre container

                    i=k+1;
                }

                if (i+2 >= argsTok.len && i < argsTok.len) // c'est que la définition du container est incorrecte
                {
                    global_env->CODE_ERROR = 87;
                    global_env->LINENUMBER = lines->tab[offset + argsAst_offset + i];
                    NeTree_destroy(tree);
                    return TREE_VOID;
                }

                int index = strlist_token_index(global_env->CONTAINERS, nomFonc);

                if (global_env->CODE_ERROR == 18) // si le type n'existe pas déjà
                {
                    global_env->CODE_ERROR = 0;

                    // il faut vérifier qu'il n'y a pas déjà le même nom d'attribut
                    for (int i=0 ; i < tree.container_lit->attributes.len ; i++)
                    {
                        for (int j=0 ; j < tree.container_lit->attributes.len ; j++)
                        {
                            if (i != j &&   strcmp(tree.container_lit->attributes.trees[i].attribute_lit->name,
                                                   tree.container_lit->attributes.trees[j].attribute_lit->name
                                            ) == 0) //doublon
                            {
                                NeTree_destroy(tree);
                                global_env->CODE_ERROR = 86;
                                global_env->LINENUMBER = tree.container_lit->attributes.trees[i].attribute_lit->line;
                                return TREE_VOID;
                            }
                        }
                    }

                    // on est bon, donc on crée le type
                    tree.container_lit->container_type = global_env->CONTAINERS->len;
                    strlist_append(global_env->CONTAINERS, tokdup(nomFonc)); // ajout du nom de container
                    NeList* attr_names = nelist_create(tree.container_lit->attributes.len); // liste des noms d'attributs

                    for (int i = 0 ; i < tree.container_lit->attributes.len ; i++) // création de la liste des attributs
                        attr_names->tab[i] = neo_str_create(strdup(tree.container_lit->attributes.trees[i].attribute_lit->name));
                    
                    // on transforme la liste de noms en NeObj, mais sans l'ajouter dans la liste d'objets du Mark & Sweep
                    // en effet, c'est un objet à part, qui ne fait pas partie du système d'objets de Neon
                    // les seuls objets qui doivent être dans la liste d'objets sont des objets accessibles à un moment
                    // ou à un autre depuis global_env->ADRESSES
                    nelist_append(global_env->ATTRIBUTES, gc_extern_neo_list_convert(attr_names)); // ajout de la liste

                }
                else // on vérifie que les champs sont bien comme ils ont été définis
                {
                    tree.container_lit->container_type = index;

                    NeList* noms = neo_to_list(global_env->ATTRIBUTES->tab[index]); // liste des noms définis

                    if (noms->len != tree.container_lit->attributes.len)
                    {
                        NeTree_destroy(tree);
                        global_env->CODE_ERROR = 83;
                        global_env->LINENUMBER = tree.container_lit->line;
                        return TREE_VOID;
                    }

                    // on va vérifier qu'il y a tous les champs, et s'ils ne sont pas dans l'ordre, on les remet dans l'ordre

                    struct TreeList attributes = tree.container_lit->attributes; // on extirpe le tableau de tree
                    TreeList_init(&tree.container_lit->attributes); // on reset le tableau de tree

                    for (int i = 0 ; i < noms->len ; i++) // on fixe un champ 'officiel', et on cherche où est le champ correspondant dans tree
                    {
                        // on vérifie si tree->sons[i]->label1 est dans noms
                        _Bool bo = false;
                        int k = 0;
                        for (; k < attributes.len ; k++)
                        {
                            if (strcmp(neo_to_string(noms->tab[i]), attributes.trees[k].attribute_lit->name) == 0) // le champ existe bien
                            {
                                bo = true;
                                break;
                            }
                        }

                        if (!bo)
                        {
                            global_env->CODE_ERROR = 83;
                            global_env->LINENUMBER = tree.container_lit->line;
                            NeTree_destroy(tree);
                            if (attributes.trees != NULL)
                                neon_free(attributes.trees);
                            return TREE_VOID;
                        }

                        // ajout de la branche au nouvel arbre réorganisé
                        NeTree_containerlit_add_attribute(&tree, attributes.trees[k]);
                    }
                    if (attributes.trees != NULL)
                        neon_free(attributes.trees);
                }

                return tree;
            }
            else
            {
                NeTree tree = NeTree_make_fcall(lines->tab[offset]);

                return_on_error(TREE_VOID);

                // création de l'arbre pour la fonction en elle-même
                toklist tokens2 = (toklist) {.tab = tokens->tab, .len = lenNomFonc};
                NeTree fonction = createExpressionTreeAux(ast, &tokens2, lines, offset);

                if_error {
                    NeTree_destroy(tree);
                    return TREE_VOID;
                }

                tree.fcall->function = fonction;

            
                if ((argsTok.len == 1 && argsAst[0]->type != TYPE_ENDOFLINE) || argsTok.len > 1) // s'il y a des arguments
                {
                    createVirguleTree(&tree.fcall->args, argsAst, &argsTok, lines, offset + lenNomFonc + 1);
                }

                return tree;
            }
        }
        
        else if (ast[0]->type == TYPE_INTEGER)
        {
            char sov = stringize(tokens->tab[0]);
            intptr_t integer = str_to_int(tokens->tab[0].debut);
            unstringize(tokens->tab[0], sov);

            return_on_error(TREE_VOID);

            NeTree T = NeTree_make_const(neo_integer_create(integer), lines->tab[offset]);
            return T;
        }

        else if (ast[0]->type == TYPE_DOUBLE)
        {
            char sov = stringize(tokens->tab[0]);
            NeTree T = NeTree_make_const(neo_double_create(str_to_double(tokens->tab[0].debut)), lines->tab[offset]);
            unstringize(tokens->tab[0], sov);
            return T;
        }
        
        else if (ast[0]->type == TYPE_LISTINDEX) // un arbre listindex a deux fils : le fils gauche contient l'arbre correspondant à la liste et le fils droit l'arbre de l'index
        {
            // on fabrique un nouveau tokens et un nouveau AST pour l'expression entre crochets
            ast_pop(ast[0]);

            int lenNomInd = ast[0]->fin + 1 - offset;

            if (tokens->len - 1 == ast[0]->fin + 2 - offset) { // pas d'arguments
                global_env->CODE_ERROR = 31;
                global_env->LINENUMBER = lines->tab[offset];
                return TREE_VOID;
            }

            Ast** indAst = ast + ast[0]->fin - offset + 2;
            
            toklist indTok;
            indTok.tab = tokens->tab + ast[0]->fin + 2 - offset;
            indTok.len = tokens->len - ast[0]->fin - 3 + offset;

            NeTree tree = NeTree_create(TypeListindex, lines->tab[offset]);

            tree.listindex->index = createExpressionTreeAux(indAst, &indTok, lines, offset + lenNomInd + 1);

            if_error {
                NeTree_destroy(tree);
                return TREE_VOID;
            }

            // appel récursif pour le nom de la liste
            toklist nomTok;
            nomTok.tab = tokens->tab;
            nomTok.len = lenNomInd;

            tree.listindex->object = createExpressionTreeAux(ast, &nomTok, lines, offset);
            
            if_error {
                NeTree_destroy(tree);
                return TREE_VOID;
            }

            return tree;
        }
        
        else if (ast[0]->type == TYPE_STRING)
        {
            char* intermediaire = sub(tokens->tab[0].debut,1, tokens->tab[0].len - 1); // on enlève les guillemets
            NeTree tree = NeTree_make_const(neo_str_create(traitementString(intermediaire)), lines->tab[offset]);
            neon_free(intermediaire);

            return tree;
        }
        
        else if (ast[0]->type == TYPE_LIST)
        {
            ast_pop(ast[0]);
            // on crée un nouveau token et un nouveau AST

            Ast** listeAst = ast + 1;
            toklist listeTok;

            listeTok.tab = tokens->tab + 1;
            listeTok.len = tokens->len - 2;

            NeTree tree = NeTree_make_syntaxtree(TypeList, lines->tab[offset]);

            return_on_error(TREE_VOID);

            if (listeTok.len > 0)
            {
                createVirguleTree(&tree.syntaxtree->treelist, listeAst, &listeTok, lines, offset + 1);

                return_on_error(TREE_VOID);
            }
            
            return tree;
            
        }
        else if (ast[0]->type == TYPE_EXPRESSION) {
            // sous-expression prioritaires, entourées ou non de parenthèses
            // on doit donc effectuer un appel récursif dessus en enlevant l'étage EXPRESSION et en enlevant un étage de parenthèses
            ast_pop(ast[0]);

            // expression entourée de parenthèses, on les enlève elles ne servent à rien
            if (tokeq(tokens->tab[0], "(") && tokeq(tokens->tab[tokens->len - 1], ")"))
            {
                toklist tokensAux = {.tab = tokens->tab + 1, .len = tokens->len - 2};
                return createExpressionTreeAux(ast + 1, &tokensAux, lines, offset + 1);
            }
            else
            {
                return createExpressionTreeAux(ast, tokens, lines, offset);
            }
        }
        else if (ast[0]->type != TYPE_PARENTHESE1 && ast[0]->type != TYPE_PARENTHESE2 && ast[0]->type != TYPE_ENDOFLINE) { // si c'est des parenthèses ou un retour à la ligne, on ignore
            if (tokeq(tokens->tab[0], "..."))
                global_env->CODE_ERROR = 97;
            else {
                global_env->CODE_ERROR = 32;
            }
            global_env->LINENUMBER = lines->tab[offset];

            return TREE_VOID;
        }
        else {
            global_env->CODE_ERROR = 19;
            return TREE_VOID;
        }
        
    }
    else if (tokens->len > 1)
    {
        
        int nbVirgules = ast_typeCountAst(ast, tokens->len, TYPE_VIRGULE, offset);
        
        if (nbVirgules > 0)
        {
            printString("Une expression seule contenant des virgules n'est pas une expression valide\n");
            return TREE_VOID;
        }
        else
        {
            // dans le cas où il y a des opérateurs dedans

            // on cherche l'indice de l'opérateur le moins prioritaire de l'AST
            int index = ast_minOp(ast, tokens, offset);

            if (index < 0) {
                global_env->CODE_ERROR = 30;
                global_env->LINENUMBER = lines->tab[offset];
                return TREE_VOID;
            }
            
            if (tokeq(tokens->tab[index], ":")) // erreur : l'opérateur : n'est pas un opérateur normal
            {
                global_env->CODE_ERROR = 92;
                global_env->LINENUMBER = lines->tab[offset + index];
                return TREE_VOID;
            }
            
            int operator_index = get_operator_index(tokens->tab[index]);
            
            int typeOperande = get_type_operande_index(operator_index);


            if (is_binary(operator_index)) {

                // si c'est un opérateur '.', on va changer la structure de l'arbre, donc c'est une opération différente
                if (operator_index == 30) // si c'est un point
                {
                    toklist tokensGauche = (toklist) {.tab = tokens->tab, .len = index};
                    Ast** astGauche = ast;
                    
                    toklist tokensDroite = (toklist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                    Ast** astDroite = ast + index + 1;

                    NeTree tree = createExpressionTreeAux(astDroite, &tokensDroite, lines, offset + index + 1);

                    return_on_error(TREE_VOID);

                    NeTree arg0 = createExpressionTreeAux(astGauche, &tokensGauche, lines, offset);


                    if_error {
                        NeTree_destroy(tree);
                        return TREE_VOID;
                    }
                    
                    
                    // on va rajouter en premier argument de la fonction l'opérande de gauche
                    if (TREE_TYPE(tree) != TypeFunctioncall)
                    {
                        NeTree_destroy(arg0);
                        NeTree_destroy(tree);
                        global_env->CODE_ERROR = 72;
                        global_env->LINENUMBER = lines->tab[offset];
                        return TREE_VOID;
                    }

                    TreeList_insert(&tree.fcall->args, arg0, 0);

                    if_error {
                        NeTree_destroy(arg0);
                        NeTree_destroy(tree);
                        return TREE_VOID;
                    }

                    return tree;
                }
                else if (operator_index == 34) // opérateur >>
                {

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
                                neon_free(stack_sov_fin.tab);
                                neon_free(stack_sov_typ.tab);
                                global_env->CODE_ERROR = 82;
                                global_env->LINENUMBER = lines->tab[offset + index + 1];
                                return TREE_VOID;
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
                        NeTree tree = createExpressionTreeAux(ast, tokens, lines, offset);

                        neon_free(stack_sov_fin.tab);
                        neon_free(stack_sov_typ.tab);

                        return tree;
                    }
                    else {
                        toklist tokensGauche = (toklist){.tab = tokens->tab, .len = index};
                        
                        
                        // création de l'arbre de l'objet dont on prend l'attribut
                        NeTree obj_tree = createExpressionTreeAux(ast, &tokensGauche, lines, offset);

                        return_on_error(TREE_VOID);

                        char* name = tokdup(tokens->tab[index + 1]); // le nom du champ

                        return NeTree_make_attribute(obj_tree, name, lines->tab[offset]);

                    }
                }
                else
                {

                    // il va falloir séparer en deux l'expression, et recalculer l'arbre des expressions sous jacentes
                    
                    toklist tokensGauche = (toklist){.tab = tokens->tab, .len = index};
                    
                    toklist tokensDroite = (toklist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                    
                    NeTree left = createExpressionTreeAux(ast, &tokensGauche, lines, offset);

                    return_on_error(TREE_VOID);

                    NeTree right = createExpressionTreeAux(ast + index + 1, &tokensDroite, lines, offset + index + 1);
                    
                    if_error
                    {
                        NeTree_destroy(left);
                        return TREE_VOID;
                    }

                    NeTree ret = NeTree_make_binaryOp(operator_index, left, right, lines->tab[offset]);

                    return ret;
                }
            }
            
            else if (typeOperande & VARRIGHT || typeOperande & RIGHT) // variable à gauche comme -- ou ++
            {

                toklist ntokens = (toklist) {.tab = tokens->tab+1, .len = tokens->len-1};
                
                NeTree fils = createExpressionTreeAux(ast+1, &ntokens, lines, offset+1);

                return_on_error(TREE_VOID);

                if (operator_index == 39 && TREE_TYPE(fils) != TypeFunctioncall) { // pour parallel
                    global_env->CODE_ERROR = 100;
                    global_env->LINENUMBER = lines->tab[offset];
                    return TREE_VOID;
                }

                return NeTree_make_unaryOp(operator_index, fils, lines->tab[offset]);
                
            }

            else if (typeOperande & VARLEFT) {
                
                toklist ntokens = (toklist) {.tab = tokens->tab, .len = tokens->len-1};
                
                
                NeTree fils = createExpressionTreeAux(ast, &ntokens, lines, offset);

                return_on_error(TREE_VOID);

                return NeTree_make_unaryOp(operator_index, fils, lines->tab[offset]);
            }
            

            else if (typeOperande == 0) {
                // cas des opérateurs inertes comme ... : il ne doit pas être en dehors d'une définition de fonction
                global_env->CODE_ERROR = 97;
                global_env->LINENUMBER = lines->tab[offset];
                return TREE_VOID;
            }

            else {
                global_env->CODE_ERROR = 19;
                return TREE_VOID;
            }
            
        }
        
    }
    else {
        global_env->CODE_ERROR = 19;
        return TREE_VOID;
    }

}








NeTree createExpressionTree(char* string)
{
    toklist* tokens = toklist_create(0);
    intlist types = intlist_create(0);
    intlist lines = intlist_create(0);
    Ast** ast;

    //printf("Début cut\n");

    cut(tokens, &types, string, true, &lines);

    if_error {
        neon_free(types.tab);
        neon_free(lines.tab);
        toklist_destroy(tokens);
        return TREE_VOID;
    }

    ast = ast_create(&types);

    //printf("Debut parse\n");
        
    parse(tokens, types, ast, &lines, 0);

    if_error {
        neon_free(types.tab);
        neon_free(lines.tab);
        ast_destroy(ast, tokens->len);
        toklist_destroy(tokens);
        return TREE_VOID;
    }

    statements(&types, tokens, ast, &lines, 0);
    
    NeTree tree = createExpressionTreeAux(ast, tokens, &lines, 0);

    ast_destroy(ast, tokens->len);

    toklist_destroy(tokens);
    neon_free(types.tab);
    neon_free(lines.tab);

    return tree;
    
}






void affExpr(NeTree tree)
{
    switch (TREE_TYPE(tree)) {
    
        case TypeBinaryOp:
        {
            affExpr(tree.binary_op->left);
            printString(" ");
            printString(get_operator_string(tree.binary_op->op));
            printString(" ");
            affExpr(tree.binary_op->right);
            return;
        }

        case TypeUnaryOp:
        {

            char* operator_string;
            if (tree.unary_op->op == 27)
                operator_string = "-";
            else {
                operator_string = get_operator_string(tree.unary_op->op);
            }
            
            if (tree.unary_op->op & VARLEFT)
            {
                affExpr(tree.unary_op->expr);
                printString(operator_string);
            }
            else {
                printString(operator_string);
                affExpr(tree.unary_op->expr);
            }

            return;
        }


        case TypeFunctioncall:
        {
            NeObj fonction = eval(tree.fcall->function);

            char* nom = global_env->NOMS->tab[nelist_index2(global_env->ADRESSES, fonction)];
            printString(nom);
            printString("(");

            for (int i = 0 ; i + 1 < tree.fcall->args.len ; i++)
            {
                affExpr(tree.fcall->args.trees[i]);
                printString(", ");
            }
            if (tree.fcall->args.len > 0)
                affExpr(tree.fcall->args.trees[tree.fcall->args.len - 1]);


            printString(")");

            neobject_destroy(fonction);
            return;
        }

        case TypeVariable:
        {
            printString(get_name(tree.variable->var));
            return;
        }

        case TypeConst:
        {
            if (NEO_TYPE(tree.const_obj->obj) == TYPE_STRING) {
                char* string = neo_to_string(tree.const_obj->obj);
                //char* str2 = traitementStringInverse(string);
                printString("\"");
                printString(string);
                printString("\"");
            }
            else
                neobject_aff(tree.const_obj->obj);
            
            return;
        }


        case TypeListindex:
        {
            affExpr(tree.listindex->object);
            printString("[");
            affExpr(tree.listindex->index);
            printString("]");
            return;
        }

        case TypeList:
        {
            if (tree.syntaxtree->treelist.len == 0)
            {
                printString("[]");
            }
            else
            {
                printString("[");
                for (int i = 0 ; i < tree.syntaxtree->treelist.len - 1 ; i++)
                {
                    affExpr(tree.syntaxtree->treelist.trees[i]);
                    printString(", ");
                }
                affExpr(tree.syntaxtree->treelist.trees[tree.syntaxtree->treelist.len]);
                printString("]");
            }
            return;
        }

        case TypeContainerLit:
        {
            //affichage du type
            printString(global_env->CONTAINERS->tab[tree.container_lit->container_type]);
            printString("(");

            // boucle pour afficher chaque nom de champ et chaque expression associée
            for (int i = 0 ; i < tree.container_lit->attributes.len ; i++)
            {
                printString(tree.container_lit->attributes.trees[i].attribute_lit->name);
                printString(": ");
                affExpr(tree.container_lit->attributes.trees[i].attribute_lit->expr);

                if (i < tree.container_lit->attributes.len - 1)
                    printString(", ");
            }
            printString(")");

            return;

        }


        case TypeAttribute:
        {
            affExpr(tree.attribute->object);

            printString(">>");

            if (tree.attribute->name != NULL)
                printString(tree.attribute->name);
            else if (tree.attribute->index != -1 && tree.attribute->last_cont_type != -1) {
                NeList* attr_list = global_env->ATTRIBUTES->tab[tree.attribute->last_cont_type].nelist;
                printString(neo_to_string(attr_list->tab[tree.attribute->index]));
            }
            else {
                printString("<Unknown container type>");
            }

            return;

        }

        default:
            printString("<Not an expression>");
    }

    return ;
}







/*
On suppose que le bloc qu'on reçoit n'a pas été poppé de sa condition de bloc
*/
NeTree createStatementIEWTree(Ast** ast, toklist* tokens, intlist* lines, int offset, TreeType type)
{
    ast_pop(ast[0]); // on pop le bloc global

    toklist blocToks = (toklist) {.tab = tokens->tab + ast[0]->fin - offset + 2, .len = tokens->len - (ast[0]->fin - offset + 1) - 2};
    Ast** blocAst = ast + ast[0]->fin - offset + 2;

    toklist exprToks = (toklist) {.tab = tokens->tab + 2, .len = ast[0]->fin - offset - 2};
    Ast** exprAst = ast + 2;

    
    NeTree expr_tree = createExpressionTreeAux(exprAst, &exprToks, lines, offset + 2);

    return_on_error(TREE_VOID);
    
    NeTree block_tree = createSyntaxTreeAux(blocAst, &blocToks, lines, offset + ast[0]->fin - offset + 2);

    if_error
    {
        NeTree_destroy(expr_tree);
        return TREE_VOID;
    }

    return NeTree_make_IEWF_tree(expr_tree, block_tree, type, lines->tab[offset]);
}






NeTree createStatementForTree(Ast** ast, toklist* tokens, intlist* lines, int offset, TreeType type)
{
    ast_pop(ast[0]); // on pop le bloc global

    toklist blocToks = (toklist) {.tab = tokens->tab + ast[0]->fin - offset + 2, .len = tokens->len - (ast[0]->fin - offset + 1) - 2};
    Ast** blocAst = ast + ast[0]->fin - offset + 2;

    toklist exprToks = (toklist) {.tab = tokens->tab + 2, .len = ast[0]->fin - offset - 2};
    Ast** exprAst = ast + 2;

    struct TreeList expr_tree;
    TreeList_init(&expr_tree);

    createVirguleTree(&expr_tree, exprAst, &exprToks, lines, offset + 2);

    if_error {
        TreeList_destroy(&expr_tree);
        return TREE_VOID;
    }
    
    NeTree block_tree = createSyntaxTreeAux(blocAst, &blocToks, lines, offset + ast[0]->fin - offset + 2);

    if_error
    {
        TreeList_destroy(&expr_tree);
        return TREE_VOID;
    }
    

    return NeTree_make_for_tree(expr_tree, block_tree, lines->tab[offset], type);
}





NeTree createStatementElseTree(Ast** ast, toklist* tokens, intlist* lines, int offset)
{
    ast_pop(ast[0]);
    toklist bloc = (toklist) {.tab = tokens->tab + 2, .len = tokens->len - 3};
    
    NeTree tree = createSyntaxTreeAux(ast + 2, &bloc, lines, offset + 2);
    tree.general_info->type = TypeElse;

    return tree;
}








NeTree createConditionBlockTree(Ast** ast, toklist* tokens, intlist* lines, int offset)
{
    ast_pop(ast[0]);

    int i = 0, index2 = 0;

    NeTree tree = NeTree_make_syntaxtree(TypeConditionblock, lines->tab[offset]);

    return_on_error(TREE_VOID);
    
    while (i < tokens->len) 
    {
        index2 = ast[i]->fin - offset + 1; // on regarde maintenant où on va sauter après

        if (ast[i]->type == TYPE_STATEMENTELSE)
        {

            toklist elseToks = (toklist) {.tab = tokens->tab + i, .len = ast[i]->fin - offset + 1 - i};

            NeTree fils = createStatementElseTree(ast + i, &elseToks, lines, offset + i);

            if_error {
                NeTree_destroy(tree);
                return TREE_VOID;
            }

            NeTree_add_syntaxtree(&tree, fils);
        }
        else if (ast[i]->type == TYPE_STATEMENTIF || ast[i]->type == TYPE_STATEMENTELIF)
        {
            toklist blocToks = (toklist) {.tab = tokens->tab + i, .len = ast[i]->fin - offset + 1 - i};

            TreeType type = (ast[i]->type == TYPE_STATEMENTIF) ? TypeIf : TypeElif;

            NeTree fils = createStatementIEWTree(ast + i, &blocToks, lines, offset + i, type);
            
            if_error {
                NeTree_destroy(tree);
                return TREE_VOID;
            }

            NeTree_add_syntaxtree(&tree, fils);

        }

        i = index2;

    }
    
    return tree;
}




NeTree createFunctionTree(Ast** ast, toklist* tokens, intlist* lines, int offset, _Bool isMethod)
{
    int i = 0;
    while (!tokeq(tokens->tab[i], "{")) i++;
    // ici, i a la valeur de l'index de la première accolade

    toklist codeTok = (toklist) {.tab = tokens->tab + i + 1, .len = tokens->len - i - 2};

    NeTree syntaxTree = createSyntaxTreeAux(ast + i + 1, &codeTok, lines, offset + i + 1);

    return_on_error(TREE_VOID);

    //ajout de l'arbre de la fonction à la liste des fonctions
    TreeList_append(&global_env->FONCTIONS, syntaxTree);

    if_error {
        NeTree_destroy(syntaxTree);
        return TREE_VOID;
    }

    Token name = tokens->tab[1];

    toklist argsTok = (toklist) {.tab = tokens->tab + 3, .len = i - 4};


    Ast** argsAst = ast + 3;
    int args_offset = offset + 3;

    // majorant du nombre d'arguments
    int size_liste = toklist_count(&argsTok, ",") + 1;
    Var* liste = neon_malloc(sizeof(Var)*size_liste); // tableau qui va contenir toutes les variables à affecter
    int liste_index = 0;

    int nbOptArgs = 0;
    _Bool unlimited_arguments = false;
    
    // création des arguments de la fonction
    struct TreeList opt_args;
    TreeList_init(&opt_args);
    

    if (argsTok.len > 0) // si la fonction a des arguments
    {
        // il faut maintenant définir chaque variable dont le nom est dans tokens
        
        for (i = 0 ; i < argsTok.len ; i = argsAst[i]->fin - args_offset + 1)
        {
            if (!tokeq(argsTok.tab[i], ",")) // si ce n'est pas une virgule, c'est une variable (enfin on vérifie)
            {
                if (argsAst[i]->type == TYPE_VARIABLE)
                {
                    char sov = stringize(argsTok.tab[i]);
                    liste[liste_index++] = get_var(argsTok.tab[i].debut);
                    unstringize(argsTok.tab[i], sov);

                    // il faut vérifier si c'est un argument optionnel ou pas

                    if (ast_index(argsAst, i, args_offset) < ast_length(argsAst, argsTok.len, args_offset) - 2 && tokeq(argsTok.tab[argsAst[i]->fin - args_offset + 1], ":=")) // argument optionnel
                    {
                        if (unlimited_arguments)
                            nbOptArgs++;

                        // dans ce cas, on évalue l'expression et on l'ajoute à la liste opt_args
                        int j = i;
                        while (j < argsTok.len && !tokeq(argsTok.tab[j], ","))
                            j = argsAst[j]->fin - args_offset + 1; // va à la fin de l'expression

                        // création d'une sous liste, puis calcul de l'arbre de cette sous liste
                        toklist tokensarg = (toklist) {.tab = argsTok.tab + i+2, .len = j-i-2};
                        
                        NeTree exp = createExpressionTreeAux(argsAst + i + 2, &tokensarg, lines, args_offset + i + 2);

                        if_error {
                            TreeList_destroy(&opt_args);
                            neon_free(liste);
                            return TREE_VOID;
                        }

                        TreeList_append(&opt_args, exp); // on ajoute l'expression aux arguments optionnels

                        if_error {
                            TreeList_destroy(&opt_args);
                            neon_free(liste);
                            return TREE_VOID;
                        }

                        i = ast_prec(argsAst, j, args_offset);
                    }
                    else if (ast_index(argsAst, i, args_offset) < ast_length(argsAst, argsTok.len, args_offset) - 1 && !tokeq(argsTok.tab[i+1], ",")) {
                        // erreur de syntaxe
                        global_env->CODE_ERROR = 96;
                        global_env->LINENUMBER = lines->tab[offset + args_offset + i];
                        TreeList_destroy(&opt_args);
                        neon_free(liste);
                        return TREE_VOID;
                    }
                    else { // argument non optionnel
                        if (unlimited_arguments) {
                            // erreur : on ne peut pas mettre d'arguments obligatoires après ...
                            global_env->CODE_ERROR = 96;
                            global_env->LINENUMBER = lines->tab[offset + args_offset + i];
                            TreeList_destroy(&opt_args);
                            neon_free(liste);
                            return TREE_VOID;
                        }
                        TreeList_append(&opt_args, TREE_VOID);
                        if_error {
                            TreeList_destroy(&opt_args);
                            neon_free(liste);
                            return TREE_VOID;
                        }
                    }
                }
                else if (tokeq(argsTok.tab[i], "...")) {
                    // nombre infini de paramètres
                    // s'il y a encore des choses après, c'est une erreur de syntaxe
                    
                    // la fonction prend un nombre illimité de paramètres
                    unlimited_arguments = true;
                }
                else {
                    global_env->CODE_ERROR = 96;
                    global_env->LINENUMBER = lines->tab[offset + args_offset + i];
                    TreeList_destroy(&opt_args);
                    neon_free(liste);
                    return TREE_VOID;
                }
            }
        }
    }

    // création de la fonction partielle servant de base pour définir des fonctions
    NeObj partial_func;

    if (isMethod) { // pour être une méthode il faut avoir au moins un argument
        if (liste_index == 0) {
            global_env->CODE_ERROR = 110;
            return TREE_VOID;
        }
        else
            partial_func = userFuncCreate(liste, syntaxTree, liste_index, unlimited_arguments, nbOptArgs, NULL, TYPE_USERMETHOD); // objet destiné à être dans l'arbre
    }
    else {
        partial_func = userFuncCreate(liste, syntaxTree, liste_index, unlimited_arguments, nbOptArgs, NULL, TYPE_USERFUNC); // objet destiné à être dans l'arbre
    }
    
    return NeTree_make_functiondef(tokdup(name), opt_args, partial_func, lines->tab[offset]);
}





NeTree createSyntaxTreeAux(Ast** ast, toklist* tokens, intlist* lines, int offset) {

    _Bool expression = false;
    int exprStart = 0;

    int index = 0, index2 = 0;

    NeTree tree = NeTree_make_syntaxtree(TypeSyntaxtree, lines->tab[offset]);

    return_on_error(TREE_VOID);

    while (index < tokens->len) {

        index2 = ast[index]->fin - offset + 1; // on regarde où on va sauter, et on saute à cet endroit après
        // c'est important, car on modifie ast[index] dans le code qui suit

        

        if (ast[index]->type == TYPE_TRYEXCEPT) {
            ast_pop(ast[index]);
            // on commence par le bloc try
            

            // try
            toklist tryTok = (toklist) {.tab = tokens->tab + index + 2, .len = ast[index]->fin - offset - index - 2};

            NeTree try_tree = createSyntaxTreeAux(ast + index + 2, &tryTok, lines, offset + index + 2);

            if_error {
                NeTree_destroy(tree);
                return TREE_VOID;
            }

            struct TreeList except_blocks;
            TreeList_init(&except_blocks);

            //except
            int i = ast[index]->fin - offset + 1;
            // on se déplace jusqu'au premier except
            while (i < index2 && ast[i]->type != TYPE_STATEMENTEXCEPT)
                i = ast[i]->fin - offset + 1;

            while (i < index2) {

                int fin = ast[i]->fin - offset;
                ast_pop(ast[i]); // on est sur le blockline

                toklist blocToks = (toklist) {.tab = tokens->tab + ast[i]->fin - offset + 2, .len = fin - (ast[i]->fin - offset + 2)};
                
                NeTree except_bloc = createSyntaxTreeAux(ast + ast[i]->fin - offset + 2, &blocToks, lines, offset + ast[i]->fin - offset + 2);
                
                if_error
                {
                    NeTree_destroy(tree);
                    TreeList_destroy(&except_blocks);
                    NeTree_destroy(try_tree);
                    return TREE_VOID;
                }

                toklist exprToks = (toklist) {.tab = tokens->tab + i + 2, .len = ast[i]->fin - offset - i - 2};

                struct TreeList except_expr;
                TreeList_init(&except_expr);

                if (exprToks.len > 0) // s'il y a au moins un argument
                {
                    createVirguleTree(&except_expr, ast + i + 2, &exprToks, lines, offset + i + 2);

                    if_error
                    {
                        NeTree_destroy(tree);
                        TreeList_destroy(&except_blocks);
                        TreeList_destroy(&except_expr);
                        NeTree_destroy(try_tree);
                        NeTree_destroy(except_bloc);
                        return TREE_VOID;
                    }

                }

                NeTree except = NeTree_make_except_block(except_expr, except_bloc, lines->tab[offset + i]);
                
                if_error {
                    NeTree_destroy(tree);
                    TreeList_destroy(&except_blocks);
                    TreeList_destroy(&except_expr);
                    NeTree_destroy(try_tree);
                    NeTree_destroy(except_bloc);
                    return TREE_VOID;
                }
                
                TreeList_append(&except_blocks, except);
                
                if_error {
                    NeTree_destroy(tree);
                    TreeList_destroy(&except_blocks);
                    NeTree_destroy(try_tree);
                    NeTree_destroy(except);
                    return TREE_VOID;
                }

                // on se déplace jusqu'au prochain except
                while (i < index2 && ast[i]->type != TYPE_STATEMENTEXCEPT)
                    i = ast[i]->fin - offset + 1;
            }

            NeTree try_except = NeTree_make_tryexcept(try_tree, except_blocks, lines->tab[offset + index]);

            if_error {
                NeTree_destroy(tree);
                TreeList_destroy(&except_blocks);
                NeTree_destroy(try_tree);
                return TREE_VOID;
            }

            NeTree_add_syntaxtree(&tree, try_except);
        }
        else if (ast[index]->type == TYPE_ATOMICBLOCK) {
            
            toklist blocTok = (toklist) {.tab = tokens->tab + index + 2, .len = ast[index]->fin - offset - index - 2};

            NeTree body = createSyntaxTreeAux(ast + index + 2, &blocTok, lines, offset + index + 2);

            if_error {
                NeTree_destroy(tree);
                return TREE_VOID;
            }

            body.general_info->type = TypeAtomic;
            NeTree_add_syntaxtree(&tree, body);
        }
        else if (ast[index]->type == TYPE_BLOCKWORD1LINE) {
            Token name = tokens->tab[index];
            toklist argsTok = (toklist) {.tab = tokens->tab + index + 2, .len = ast[index]->fin - offset - index - 2};
            Ast** argsAst = ast + index + 2;

            struct TreeList params;
            TreeList_init(&params);

            createVirguleTree(&params, argsAst, &argsTok, lines, offset + index + 2);

            if_error {
                NeTree_destroy(tree);
                TreeList_destroy(&params);
                return TREE_VOID;
            }
            
            int code = get_blockword1Line_index(name) + 1;

            NeTree fils = NeTree_make_kwparam(params, code, lines->tab[offset + index]);

            if_error {
                NeTree_destroy(tree);
                TreeList_destroy(&params);
                return TREE_VOID;
            }
            
            NeTree_add_syntaxtree(&tree, fils);
        }
        else if (ast[index]->type == TYPE_FUNCTIONDEF || ast[index]->type == TYPE_METHODDEF) {

            toklist stru = (toklist) {.tab = tokens->tab + index, .len = ast[index]->fin - offset + 1 - index};
            
            NeTree fils = createFunctionTree(ast + index, &stru, lines, offset + index, ast[index]->type == TYPE_METHODDEF);

            if_error {
                NeTree_destroy(tree);
                return TREE_VOID;
            }

            NeTree_add_syntaxtree(&tree, fils);
        }
        else if (ast[index]->type == TYPE_KEYWORD) {
            NeTree fils = NeTree_create(TypeKeyword, lines->tab[offset + index]);
            fils.keyword->code = get_lkeywords_index(tokens->tab[index]) + 1;
            
            NeTree_add_syntaxtree(&tree, fils);
        }
        else if (ast[index]->type == TYPE_STATEMENTFOR || ast[index]->type == TYPE_STATEMENTFOREACH)  {

            toklist stru = (toklist) {.tab = tokens->tab + index, .len = ast[index]->fin - offset + 1 - index};

            TreeType type = (ast[index]->type == TYPE_STATEMENTFOR) ? TypeFor : TypeForeach;
            
            NeTree fils = createStatementForTree(ast + index, &stru, lines, offset + index, type);

            if_error {
                NeTree_destroy(tree);
                return TREE_VOID;
            }

            NeTree_add_syntaxtree(&tree, fils);
        }
        else if (ast[index]->type == TYPE_STATEMENTWHILE)  {

            toklist stru = (toklist) {.tab = tokens->tab + index, .len = ast[index]->fin - offset + 1 - index};
            
            NeTree fils = createStatementIEWTree(ast + index, &stru, lines, offset + index, TypeWhile);

            if_error {
                NeTree_destroy(tree);
                return TREE_VOID;
            }

            NeTree_add_syntaxtree(&tree, fils);
        }
        else if (ast[index]->type == TYPE_CONDITIONBLOCK) {
            
            toklist conBlock = (toklist) {.tab = tokens->tab + index, .len = ast[index]->fin - offset - index + 1};

            NeTree st = createConditionBlockTree(ast + index, &conBlock, lines, offset + index);

            if_error {
                NeTree_destroy(tree);
                return TREE_VOID;
            }

            NeTree_add_syntaxtree(&tree, st);
        }
        else if (!expression && ast[index]->type != TYPE_ENDOFLINE) { // this is of course the beginning of an expression
            expression = true;
            exprStart = index;
        }

        else if (expression && ast[index]->type == TYPE_ENDOFLINE) { // si l'expression est terminée, on la compute
            
            // compute an expression between exprStart and index
            toklist exprToks = (toklist) {.tab = tokens->tab + exprStart, .len = index - exprStart};

            NeTree expr = createExpressionTreeAux(ast + exprStart, &exprToks, lines, offset + exprStart);

            if_error {
                NeTree_destroy(tree);
                return TREE_VOID;
            }

            NeTree_add_syntaxtree(&tree, expr);

            expression = false;
        }
        
        // pour permettre de commencer et finir des expressions durant le même tour dans le seul cas d'une expression tout à la fin
        if (expression && index2 >= tokens->len && global_env->CODE_ERROR==0) { // on sait qu'au prochain tour on sera partis, donc on prend tout le reste
            
            toklist exprToks = (toklist) {.tab = tokens->tab + exprStart, .len = tokens->len - exprStart};

            NeTree expr = createExpressionTreeAux(ast + exprStart, &exprToks, lines, offset + exprStart);

            if_error {
                NeTree_destroy(tree);
                return TREE_VOID;
            }

            NeTree_add_syntaxtree(&tree, expr);

            expression = false;
        }

        if_error {
            NeTree_destroy(tree);
            return TREE_VOID;
        }

        index = index2;

    }

    return tree;

}







/*
Cette fonction prend en argument une chaine de caractères représentant un programme, et un pointeur vers un arbre initialisé
Elle construit dans ce pointeur l'arbre syntaxique associé à la chaine de caractères
*/
NeTree createSyntaxTree(char* program)
{
    toklist* tokens = toklist_create(0);
    intlist types = intlist_create(0);
    intlist lines = intlist_create(0);
    Ast** ast;

    cut(tokens, &types, program, true, &lines);

    if_error {
        neon_free(types.tab);
        neon_free(lines.tab);
        toklist_destroy(tokens);
        return TREE_VOID;
    }

    ast = ast_create(&types);

    if (ast == NULL) {
        global_env->CODE_ERROR = 12;
        neon_free(types.tab);
        neon_free(lines.tab);
        toklist_destroy(tokens);
        return TREE_VOID;
    }

    parse(tokens, types, ast, &lines, 0);

    if_error {
        neon_free(types.tab);
        neon_free(lines.tab);
        ast_destroy(ast, tokens->len);
        toklist_destroy(tokens);
        return TREE_VOID;
    }

    statements(&types, tokens, ast, &lines, 0);

    if_error {
        neon_free(types.tab);
        neon_free(lines.tab);
        ast_destroy(ast, tokens->len);
        toklist_destroy(tokens);
        return TREE_VOID;
    }

    NeTree tree = createSyntaxTreeAux(ast, tokens, &lines, 0);

    ast_destroy(ast, tokens->len);
    toklist_destroy(tokens);
    neon_free(types.tab);
    neon_free(lines.tab);

    return tree;
}