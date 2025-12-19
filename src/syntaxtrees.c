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

int debug = 0;

void createVirguleTree(TreeBuffer* tb, struct TreeListTemp* tree_list, Ast** ast, toklist* tokens, intlist* lines, int offset) {
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
            
            TreeBufferIndex arbre = createExpressionTreeAux(tb, ast + debut, &sous_tokens, lines, offset + debut);
            return_on_error();

            TreeListTemp_append(tree_list, arbre);

            return_on_error();

            index ++;
            
        }
    }
    else { // il y a soit une expression normale, soit rien tu tout
        int count = ast_typeCountAst(ast, tokens->len, TYPE_ENDOFLINE, offset);
        int real_length = ast_length(ast, tokens->len, offset);

        // si il y a des choses quand même
        if (count < real_length) {
            TreeBufferIndex arbre = createExpressionTreeAux(tb, ast, tokens, lines, offset);
            TreeListTemp_append(tree_list, arbre);
            return_on_error();
        }
    }
}





TreeBufferIndex createExpressionTreeAux(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset)
{
    int count = ast_typeCountAst(ast, tokens->len, TYPE_ENDOFLINE, offset);
    int real_length = ast_length(ast, tokens->len, offset);
    
    // tous les tokens sont des retours à la ligne
    if (count == real_length) {
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

            return createExpressionTreeAux(tb, ast + i, &tokens2, lines, offset + i);
        }

        if (ast[0]->type == TYPE_VARIABLE) {
            char sov = stringize(tokens->tab[0]);
            TreeBufferIndex T = NeTree_make_variable(tb, get_var(tokens->tab[0].debut), lines->tab[offset]);
            unstringize(tokens->tab[0], sov);
            return T;
        }

        else if (ast[0]->type == TYPE_BOOL) {
            char sov = stringize(tokens->tab[0]);
            TreeBufferIndex T = NeTree_make_const(tb, neo_bool_create(strToBool(tokens->tab[0].debut)), lines->tab[offset]);
            unstringize(tokens->tab[0], sov);
            return T;
        }

        else if (ast[0]->type == TYPE_EXCEPTION) {
            // créer une exception
            char sov = stringize(tokens->tab[0]);
            int index = strlist_index(global_env->EXCEPTIONS, tokens->tab[0].debut);
            unstringize(tokens->tab[0], sov);
            return NeTree_make_const(tb, neo_exception_create(index), lines->tab[offset]);
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
            return NeTree_make_const(tb, obj, lines->tab[offset]); // cet arbre se situe à la ligne [line] du fichier
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
                TreeBufferIndex tree = NeTree_create(tb, TypeContainerLit, lines->tab[offset]);

                struct TreeListTemp temptreelist;
                TreeListTemp_init(&temptreelist);

                return_on_error(TREE_VOID);

                Token nomFonc = tokens->tab[0];
                // ce n'est pas une fonction mais un constructeur de container, on réutilise le tableau
                
                // argsTok contient maintenant la liste des tokens des 'arguments' de l'objet
                
                int i = 0; // indice de tok

                while (i < argsTok.len - 2)
                {
                    while (argsAst[i]->type == TYPE_ENDOFLINE) i++;

                    TreeBufferIndex t1 = NeTree_create(tb, TypeAttributeLit, lines->tab[offset + argsAst_offset + i]);

                    if_error {
                        TreeListTemp_destroy(tb, &temptreelist);
                        return TREE_VOID;
                    }

                    if (argsAst[i]->type != TYPE_VARIABLE || !tokeq(argsTok.tab[i+1],":")) // le champ n'a pas un nom correct ou n'est pas suivi de :
                    {
                        global_env->CODE_ERROR = 87;
                        global_env->LINENUMBER = lines->tab[offset + argsAst_offset + i];
                        NeTree_destroy(tb, t1);
                        TreeListTemp_destroy(tb, &temptreelist);
                        return TREE_VOID;
                    }

                    treeAttrLit(tb, t1)->name = tokdup(argsTok.tab[i]); // nom du champ


                    i+=2; // on saute le ':'. On a le droit car la condition de la boucle while le permet

                    // calcule l'arbre du truc à droite du ':'
                    int k = i;
                    while (k < argsTok.len && argsAst[k]->type != TYPE_VIRGULE) // saute à l'argument suivant
                        k = argsAst[k]->fin - offset - argsAst_offset + 1; // l'offset de argsAst est lenNomFonc + 1

                    // prend la partie après la virgule, la première partie est 1 seul token que l'on prend au début de la boucle en ajoutant un de plus à i
                    toklist tok2 = (toklist) {.tab = argsTok.tab + i, .len = k-i};

                    TREE_AFFECT(
                        treeAttrLit(tb, t1)->expr,
                        createExpressionTreeAux(tb, argsAst + i, &tok2, lines, offset + argsAst_offset + i)
                    );

                    if_error {
                        NeTree_destroy(tb, t1);
                        TreeListTemp_destroy(tb, &temptreelist);
                        return TREE_VOID;
                    }

                    TreeListTemp_append(&temptreelist, t1); // l'arbre est encore fils de l'arbre container

                    i=k+1;
                }

                if (i+2 >= argsTok.len && i < argsTok.len) // c'est que la définition du container est incorrecte
                {
                    global_env->CODE_ERROR = 87;
                    global_env->LINENUMBER = lines->tab[offset + argsAst_offset + i];
                    return TREE_VOID;
                }

                int index = strlist_token_index(global_env->CONTAINERS, nomFonc);

                if (global_env->CODE_ERROR == 18) // si le type n'existe pas déjà
                {
                    global_env->CODE_ERROR = 0;

                    // il faut vérifier qu'il n'y a pas déjà le même nom d'attribut
                    for (int i=0 ; i < temptreelist.len ; i++)
                    {
                        for (int j=0 ; j < temptreelist.len ; j++)
                        {
                            if (i != j &&   strcmp(treeAttrLit(tb, temptreelist.trees[i])->name,
                                                   treeAttrLit(tb, temptreelist.trees[j])->name
                                            ) == 0) //doublon
                            {
                                global_env->CODE_ERROR = 86;
                                global_env->LINENUMBER = treeAttrLit(tb, temptreelist.trees[i])->line;
                                TreeListTemp_destroy(tb, &temptreelist);
                                return TREE_VOID;
                            }
                        }
                    }

                    // on est bon, donc on crée le type
                    treeContLit(tb, tree)->container_type = global_env->CONTAINERS->len;
                    strlist_append(global_env->CONTAINERS, tokdup(nomFonc)); // ajout du nom de container
                    NeList* attr_names = nelist_create(temptreelist.len); // liste des noms d'attributs

                    for (int i = 0 ; i < temptreelist.len ; i++) // création de la liste des attributs
                        attr_names->tab[i] = neo_str_create(strdup(treeAttrLit(tb, temptreelist.trees[i])->name));
                    
                    // on transforme la liste de noms en NeObj, mais sans l'ajouter dans la liste d'objets du Mark & Sweep
                    // en effet, c'est un objet à part, qui ne fait pas partie du système d'objets de Neon
                    // les seuls objets qui doivent être dans la liste d'objets sont des objets accessibles à un moment
                    // ou à un autre depuis global_env->ADRESSES
                    nelist_append(global_env->ATTRIBUTES, gc_extern_neo_list_convert(attr_names)); // ajout de la liste

                }
                else // on vérifie que les champs sont bien comme ils ont été définis
                {
                    treeContLit(tb, tree)->container_type = index;

                    NeList* noms = neo_to_list(global_env->ATTRIBUTES->tab[index]); // liste des noms définis

                    if (noms->len != temptreelist.len)
                    {
                        global_env->CODE_ERROR = 83;
                        global_env->LINENUMBER = treeContLit(tb, tree)->line;
                        TreeListTemp_destroy(tb, &temptreelist);
                        return TREE_VOID;
                    }

                    // on va vérifier qu'il y a tous les champs, et s'ils ne sont pas dans l'ordre, on les remet dans l'ordre

                    struct TreeListTemp attributes = temptreelist; // on extirpe le tableau de tree
                    TreeListTemp_init(&temptreelist); // on reset le tableau de tree

                    for (int i = 0 ; i < noms->len ; i++) // on fixe un champ 'officiel', et on cherche où est le champ correspondant dans tree
                    {
                        // on vérifie si tree->sons[i]->label1 est dans noms
                        _Bool bo = false;
                        int k = 0;
                        for (; k < attributes.len ; k++)
                        {
                            if (strcmp(neo_to_string(noms->tab[i]), treeAttrLit(tb, attributes.trees[k])->name) == 0) // le champ existe bien
                            {
                                bo = true;
                                break;
                            }
                        }

                        if (!bo)
                        {
                            global_env->CODE_ERROR = 83;
                            global_env->LINENUMBER = treeContLit(tb, tree) ->line;
                            TreeListTemp_destroy(tb, &temptreelist);
                            if (attributes.trees != NULL)
                                neon_free(attributes.trees);
                            return TREE_VOID;
                        }

                        // ajout de la branche au nouvel arbre réorganisé
                        TreeListTemp_append(&temptreelist, attributes.trees[k]);
                    }
                    if (attributes.trees != NULL)
                        neon_free(attributes.trees);
                }

                TREELIST_AFFECT(
                    treeContLit(tb, tree)->attributes,
                    TreeListTemp_dump(tb, &temptreelist)
                );

                return tree;
            }
            else
            {
                TreeBufferIndex tree = NeTree_make_fcall(tb, lines->tab[offset]);

                return_on_error(TREE_VOID);

                // création de l'arbre pour la fonction en elle-même
                toklist tokens2 = (toklist) {.tab = tokens->tab, .len = lenNomFonc};
                TreeBufferIndex fonction = createExpressionTreeAux(tb, ast, &tokens2, lines, offset);

                if_error {
                    return TREE_VOID;
                }

                treeFCall(tb, tree)->function = fonction;

                struct TreeListTemp temptreelist; TreeListTemp_init(&temptreelist);
            
                if ((argsTok.len == 1 && argsAst[0]->type != TYPE_ENDOFLINE) || argsTok.len > 1) // s'il y a des arguments
                {
                    createVirguleTree(tb, &temptreelist, argsAst, &argsTok, lines, offset + lenNomFonc + 1);
                    if_error {
                        TreeListTemp_destroy(tb, &temptreelist);
                        return TREE_VOID;
                    }
                }

                TREELIST_AFFECT(
                    treeFCall(tb, tree)->args,
                    TreeListTemp_dump(tb, &temptreelist)
                );

                return tree;
            }
        }
        
        else if (ast[0]->type == TYPE_INTEGER)
        {
            char sov = stringize(tokens->tab[0]);
            intptr_t integer = str_to_int(tokens->tab[0].debut);
            unstringize(tokens->tab[0], sov);

            return_on_error(TREE_VOID);

            TreeBufferIndex T = NeTree_make_const(tb, neo_integer_create(integer), lines->tab[offset]);
            return T;
        }

        else if (ast[0]->type == TYPE_DOUBLE)
        {
            char sov = stringize(tokens->tab[0]);
            TreeBufferIndex T = NeTree_make_const(tb, neo_double_create(str_to_double(tokens->tab[0].debut)), lines->tab[offset]);
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

            TreeBufferIndex tree = NeTree_create(tb, TypeListindex, lines->tab[offset]);

            // on est obligé de faire une affectation différée, sinon le pointeur évalué à gauche n'est plus valide une fois
            // createExpressionTreeAux appelée
            TREE_AFFECT(
                treeLstIndx(tb, tree)->index,
                createExpressionTreeAux(tb, indAst, &indTok, lines, offset + lenNomInd + 1)
            );

            if_error {
                return TREE_VOID;
            }


            // appel récursif pour le nom de la liste
            toklist nomTok;
            nomTok.tab = tokens->tab;
            nomTok.len = lenNomInd;

            TREE_AFFECT(
                treeLstIndx(tb, tree)->object,
                createExpressionTreeAux(tb, ast, &nomTok, lines, offset);
            );

            return tree;
        }
        
        else if (ast[0]->type == TYPE_STRING)
        {
            char* intermediaire = sub(tokens->tab[0].debut,1, tokens->tab[0].len - 1); // on enlève les guillemets
            TreeBufferIndex tree = NeTree_make_const(tb, neo_str_create(traitementString(intermediaire)), lines->tab[offset]);
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

            TreeBufferIndex tree = NeTree_create(tb, TypeList, lines->tab[offset]);

            return_on_error(TREE_VOID);

            struct TreeListTemp temptreelist; TreeListTemp_init(&temptreelist);

            if (listeTok.len > 0)
            {
                createVirguleTree(tb, &temptreelist, listeAst, &listeTok, lines, offset + 1);
                
                if_error {
                    TreeListTemp_destroy(tb, &temptreelist);
                    return TREE_VOID;
                }
            }

            TREELIST_AFFECT(
                treeSntxTree(tb, tree)->treelist,
                TreeListTemp_dump(tb, &temptreelist)
            );
            
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
                return createExpressionTreeAux(tb, ast + 1, &tokensAux, lines, offset + 1);
            }
            else
            {
                return createExpressionTreeAux(tb, ast, tokens, lines, offset);
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

                    TreeBufferIndex tree = createExpressionTreeAux(tb, astDroite, &tokensDroite, lines, offset + index + 1);

                    return_on_error(TREE_VOID);

                    TreeBufferIndex arg0 = createExpressionTreeAux(tb, astGauche, &tokensGauche, lines, offset);


                    if_error {
                        NeTree_destroy(tb, tree);
                        return TREE_VOID;
                    }
                    
                    
                    // on va rajouter en premier argument de la fonction l'opérande de gauche
                    if (TREE_TYPE(tb, tree) != TypeFunctioncall)
                    {
                        NeTree_destroy(tb, tree);
                        NeTree_destroy(tb, arg0);
                        global_env->CODE_ERROR = 72;
                        global_env->LINENUMBER = lines->tab[offset];
                        return TREE_VOID;
                    }

                    // on alloue une nouveau TreeList dans le TreeBuffer sans libérer l'ancien
                    TreeBufferIndex new_treelist = TreeBuffer_alloc(tb, sizeof(TreeBufferIndex) * (treeFCall(tb, tree)->args.length + 1));
                    TreeBufferIndex* array_ptr = tb->pointer + new_treelist;
                    array_ptr[0] = arg0; // le premier argument est l'objet à gauche
                    // et on copie les autres arguments
                    for (int i = 0 ; i < treeFCall(tb, tree)->args.length ; i++) {
                        array_ptr[i+1] = treelistGet(tb, treeFCall(tb, tree)->args)[i];
                    }
                    // et on remplace le TreeList
                    treeFCall(tb, tree)->args.indices = new_treelist;
                    treeFCall(tb, tree)->args.length++;

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
                        TreeBufferIndex tree = createExpressionTreeAux(tb, ast, tokens, lines, offset);

                        neon_free(stack_sov_fin.tab);
                        neon_free(stack_sov_typ.tab);

                        return tree;
                    }
                    else {
                        toklist tokensGauche = (toklist){.tab = tokens->tab, .len = index};
                        
                        
                        // création de l'arbre de l'objet dont on prend l'attribut
                        TreeBufferIndex obj_tree = createExpressionTreeAux(tb, ast, &tokensGauche, lines, offset);

                        return_on_error(TREE_VOID);

                        char* name = tokdup(tokens->tab[index + 1]); // le nom du champ

                        return NeTree_make_attribute(tb, obj_tree, name, lines->tab[offset]);
                    }
                }
                else
                {

                    // il va falloir séparer en deux l'expression, et recalculer l'arbre des expressions sous jacentes
                    
                    toklist tokensGauche = (toklist){.tab = tokens->tab, .len = index};
                    
                    toklist tokensDroite = (toklist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                    
                    TreeBufferIndex left = createExpressionTreeAux(tb, ast, &tokensGauche, lines, offset);
                    return_on_error(TREE_VOID);

                    TreeBufferIndex right = createExpressionTreeAux(tb, ast + index + 1, &tokensDroite, lines, offset + index + 1);
                    
                    if_error {
                        NeTree_destroy(tb, left);
                        return TREE_VOID;
                    }

                    TreeBufferIndex ret = NeTree_make_binaryOp(tb, operator_index, left, right, lines->tab[offset]);

                    return ret;
                }
            }
            
            else if (typeOperande & VARRIGHT || typeOperande & RIGHT) // variable à gauche comme -- ou ++
            {

                toklist ntokens = (toklist) {.tab = tokens->tab+1, .len = tokens->len-1};

                TreeBufferIndex fils;

                // dans le cas où l'on est en train de créer l'arbre associé à l'opérateur parallel,
                // on crée le fils dans global_env->FONCTIONS car l'expression du lancement de la fonction
                // en parallèle, une fois ajoutée dans le nouveau processus en attente d'être démarré pourra
                // si ça se trouve être exécutée après la suppression du buffer associé au fichier dans
                // lequel on lance la fonction en parallèle
                if (operator_index == 39) { // opérateur parallel
                    TreeBuffer* func_buf = &global_env->FONCTIONS;
                
                    fils = createExpressionTreeAux(func_buf, ast+1, &ntokens, lines, offset+1);
                    return_on_error(TREE_VOID);

                    TreeBuffer_remember(func_buf, fils);

                    if (TREE_TYPE(func_buf, fils) != TypeFunctioncall) { // pour parallel
                        global_env->CODE_ERROR = 100;
                        global_env->LINENUMBER = lines->tab[offset];
                        return TREE_VOID;
                    }

                    return NeTree_make_parallel_call(tb, fils, lines->tab[offset]);
                }
                else {
                    fils = createExpressionTreeAux(tb, ast+1, &ntokens, lines, offset+1);
                    return NeTree_make_unaryOp(tb, operator_index, fils, lines->tab[offset]);
                }
            }

            else if (typeOperande & VARLEFT) {
                
                toklist ntokens = (toklist) {.tab = tokens->tab, .len = tokens->len-1};
                
                
                TreeBufferIndex fils = createExpressionTreeAux(tb, ast, &ntokens, lines, offset);

                return_on_error(TREE_VOID);

                return NeTree_make_unaryOp(tb, operator_index, fils, lines->tab[offset]);
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







// Cette fonction libère dès qu'elle n'en n'a plus besoin la chaîne de caractères donnée en argument
TreeBuffer createExpressionTree(char* string, bool free_string)
{

    toklist* tokens = toklist_create(0);
    intlist types = intlist_create(0);
    intlist lines = intlist_create(0);
    Ast** ast;

    //printf("Début cut\n");

    cut(tokens, &types, string, true, &lines, free_string);

    if_error {
        neon_free(types.tab);
        neon_free(lines.tab);
        toklist_destroy(tokens);
        return (TreeBuffer){0};
    }

    ast = ast_create(&types);

    //printf("Debut parse\n");
        
    parse(tokens, types, ast, &lines, 0);

    if_error {
        neon_free(types.tab);
        neon_free(lines.tab);
        ast_destroy(ast, tokens->len);
        toklist_destroy(tokens);
        return (TreeBuffer){0};
    }

    statements(&types, tokens, ast, &lines, 0);
    
    TreeBuffer tb;
    TreeBuffer_init(&tb);
    tb.entry_point = createExpressionTreeAux(&tb, ast, tokens, &lines, 0);

    if_error {
        TreeBuffer_destroy(&tb);
    }

    ast_destroy(ast, tokens->len);

    toklist_destroy(tokens);
    neon_free(types.tab);
    neon_free(lines.tab);

    return tb;
    
}






void affExpr(TreeBuffer* tb, TreeBufferIndex tree)
{
    switch (TREE_TYPE(tb, tree)) {
    
        case TypeBinaryOp:
        {
            affExpr(tb, treeBinOp(tb, tree)->left);
            printString(" ");
            printString(get_operator_string(treeBinOp(tb, tree)->op));
            printString(" ");
            affExpr(tb, treeBinOp(tb, tree)->right);
            return;
        }

        case TypeUnaryOp:
        {

            char* operator_string;
            if (treeUnOp(tb, tree)->op == 27)
                operator_string = "-";
            else {
                operator_string = get_operator_string(treeUnOp(tb, tree)->op);
            }
            
            if (treeUnOp(tb, tree)->op & VARLEFT)
            {
                affExpr(tb, treeUnOp(tb, tree)->expr);
                printString(operator_string);
            }
            else {
                printString(operator_string);
                affExpr(tb, treeUnOp(tb, tree)->expr);
            }

            return;
        }


        case TypeFunctioncall:
        {
            NeObj fonction = eval(tb, treeFCall(tb, tree)->function);

            char* nom = global_env->NOMS->tab[nelist_index2(global_env->ADRESSES, fonction)];
            printString(nom);
            printString("(");

            for (int i = 0 ; i + 1 < treeFCall(tb, tree)->args.length ; i++)
            {
                affExpr(tb, treelistGet(tb, treeFCall(tb, tree)->args)[i]);
                printString(", ");
            }
            if (treeFCall(tb, tree)->args.length > 0)
                affExpr(tb, treelistGet(tb, treeFCall(tb, tree)->args)[treeFCall(tb, tree)->args.length - 1]);


            printString(")");

            neobject_destroy(fonction);
            return;
        }

        case TypeVariable:
        {
            printString(get_name(treeVar(tb, tree)->var));
            return;
        }

        case TypeConst:
        {
            if (NEO_TYPE(treeConst(tb, tree)->obj) == TYPE_STRING) {
                char* string = neo_to_string(treeConst(tb, tree)->obj);
                //char* str2 = traitementStringInverse(string);
                printString("\"");
                printString(string);
                printString("\"");
            }
            else
                neobject_aff(treeConst(tb, tree)->obj);
            
            return;
        }


        case TypeListindex:
        {
            affExpr(tb, treeLstIndx(tb, tree)->object);
            printString("[");
            affExpr(tb, treeLstIndx(tb, tree)->index);
            printString("]");
            return;
        }

        case TypeList:
        {
            if (treeSntxTree(tb, tree)->treelist.length == 0)
            {
                printString("[]");
            }
            else
            {
                printString("[");
                for (int i = 0 ; i < treeSntxTree(tb, tree)->treelist.length - 1 ; i++)
                {
                    affExpr(tb, treelistGet(tb, treeSntxTree(tb, tree)->treelist)[i]);
                    printString(", ");
                }
                affExpr(tb, treelistGet(tb, treeSntxTree(tb, tree)->treelist)[treeSntxTree(tb, tree)->treelist.length]);
                printString("]");
            }
            return;
        }

        case TypeContainerLit:
        {
            //affichage du type
            printString(global_env->CONTAINERS->tab[treeContLit(tb, tree) ->container_type]);
            printString("(");

            // boucle pour afficher chaque nom de champ et chaque expression associée
            for (int i = 0 ; i < treeContLit(tb, tree) ->attributes.length ; i++)
            {
                printString(treeAttrLit(tb, treelistGet(tb, treeContLit(tb, tree)->attributes)[i])->name);
                printString(": ");
                affExpr(tb, treeAttrLit(tb, treelistGet(tb, treeContLit(tb, tree)->attributes)[i])->expr);

                if (i < treeContLit(tb, tree)->attributes.length - 1)
                    printString(", ");
            }
            printString(")");

            return;

        }


        case TypeAttribute:
        {
            affExpr(tb, treeAttr(tb, tree)->object);

            printString(">>");

            if (treeAttr(tb, tree)->name != NULL)
                printString(treeAttr(tb, tree)->name);
            else if (treeAttr(tb, tree)->index != -1 && treeAttr(tb, tree)->last_cont_type != -1) {
                NeList* attr_list = global_env->ATTRIBUTES->tab[treeAttr(tb, tree)->last_cont_type].nelist;
                printString(neo_to_string(attr_list->tab[treeAttr(tb, tree)->index]));
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
TreeBufferIndex createStatementIEWTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset, TreeType type)
{
    ast_pop(ast[0]); // on pop le bloc global

    toklist blocToks = (toklist) {.tab = tokens->tab + ast[0]->fin - offset + 2, .len = tokens->len - (ast[0]->fin - offset + 1) - 2};
    Ast** blocAst = ast + ast[0]->fin - offset + 2;

    toklist exprToks = (toklist) {.tab = tokens->tab + 2, .len = ast[0]->fin - offset - 2};
    Ast** exprAst = ast + 2;

    
    TreeBufferIndex expr_tree = createExpressionTreeAux(tb, exprAst, &exprToks, lines, offset + 2);

    return_on_error(TREE_VOID);
    
    TreeBufferIndex block_tree = createSyntaxTreeAux(tb, blocAst, &blocToks, lines, offset + ast[0]->fin - offset + 2);

    if_error
    {
        NeTree_destroy(tb, expr_tree);
        return TREE_VOID;
    }

    return NeTree_make_IEWF_tree(tb, expr_tree, block_tree, type, lines->tab[offset]);
}






TreeBufferIndex createStatementForTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset, TreeType type)
{
    ast_pop(ast[0]); // on pop le bloc global

    toklist blocToks = (toklist) {.tab = tokens->tab + ast[0]->fin - offset + 2, .len = tokens->len - (ast[0]->fin - offset + 1) - 2};
    Ast** blocAst = ast + ast[0]->fin - offset + 2;

    toklist exprToks = (toklist) {.tab = tokens->tab + 2, .len = ast[0]->fin - offset - 2};
    Ast** exprAst = ast + 2;

    struct TreeListTemp expr_tree;
    TreeListTemp_init(&expr_tree);

    createVirguleTree(tb, &expr_tree, exprAst, &exprToks, lines, offset + 2);

    if_error {
        TreeListTemp_destroy(tb, &expr_tree);
        return TREE_VOID;
    }
    
    TreeBufferIndex block_tree = createSyntaxTreeAux(tb, blocAst, &blocToks, lines, offset + ast[0]->fin - offset + 2);

    if_error
    {
        TreeListTemp_destroy(tb, &expr_tree);
        return TREE_VOID;
    }
    

    return NeTree_make_for_tree(tb, expr_tree, block_tree, lines->tab[offset], type);
}





TreeBufferIndex createStatementElseTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset)
{
    ast_pop(ast[0]);
    toklist bloc = (toklist) {.tab = tokens->tab + 2, .len = tokens->len - 3};
    
    TreeBufferIndex tree = createSyntaxTreeAux(tb, ast + 2, &bloc, lines, offset + 2);
    TREE_TYPE(tb, tree) = TypeElse;

    return tree;
}








TreeBufferIndex createConditionBlockTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset)
{
    ast_pop(ast[0]);

    int i = 0, index2 = 0;

    TreeBufferIndex tree = NeTree_create(tb, TypeConditionblock, lines->tab[offset]);
    struct TreeListTemp temptreelist; TreeListTemp_init(&temptreelist);

    return_on_error(TREE_VOID);
    
    while (i < tokens->len) 
    {
        index2 = ast[i]->fin - offset + 1; // on regarde maintenant où on va sauter après

        if (ast[i]->type == TYPE_STATEMENTELSE)
        {

            toklist elseToks = (toklist) {.tab = tokens->tab + i, .len = ast[i]->fin - offset + 1 - i};

            TreeBufferIndex fils = createStatementElseTree(tb, ast + i, &elseToks, lines, offset + i);

            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                return TREE_VOID;
            }

            TreeListTemp_append(&temptreelist, fils);
        }
        else if (ast[i]->type == TYPE_STATEMENTIF || ast[i]->type == TYPE_STATEMENTELIF)
        {
            toklist blocToks = (toklist) {.tab = tokens->tab + i, .len = ast[i]->fin - offset + 1 - i};

            TreeType type = (ast[i]->type == TYPE_STATEMENTIF) ? TypeIf : TypeElif;

            TreeBufferIndex fils = createStatementIEWTree(tb, ast + i, &blocToks, lines, offset + i, type);
            
            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                return TREE_VOID;
            }

            TreeListTemp_append(&temptreelist, fils);
        }

        i = index2;
    }

    TREELIST_AFFECT(
        treeSntxTree(tb, tree)->treelist,
        TreeListTemp_dump(tb, &temptreelist)
    );
    
    return tree;
}




TreeBufferIndex createFunctionTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset, _Bool isMethod)
{
    int i = 0;
    while (!tokeq(tokens->tab[i], "{")) i++;
    // ici, i a la valeur de l'index de la première accolade

    toklist codeTok = (toklist) {.tab = tokens->tab + i + 1, .len = tokens->len - i - 2};

    // on alloue l'arbre syntaxTree dans le TreeBuffer global des fonctions
    TreeBufferIndex syntaxTree = createSyntaxTreeAux(&global_env->FONCTIONS, ast + i + 1, &codeTok, lines, offset + i + 1);

    if_error {
        NeTree_destroy(&global_env->FONCTIONS, syntaxTree);
        return TREE_VOID;
    }

    TreeBuffer_remember(&global_env->FONCTIONS, syntaxTree);

    if_error {
        NeTree_destroy(&global_env->FONCTIONS, syntaxTree);
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
    struct TreeListTemp opt_args;
    TreeListTemp_init(&opt_args);
    

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
                        
                        TreeBufferIndex exp = createExpressionTreeAux(tb, argsAst + i + 2, &tokensarg, lines, args_offset + i + 2);

                        if_error {
                            TreeListTemp_destroy(tb, &opt_args);
                            neon_free(liste);
                            return TREE_VOID;
                        }

                        TreeListTemp_append(&opt_args, exp); // on ajoute l'expression aux arguments optionnels

                        if_error {
                            TreeListTemp_destroy(tb, &opt_args);
                            neon_free(liste);
                            return TREE_VOID;
                        }

                        i = ast_prec(argsAst, j, args_offset);
                    }
                    else if (ast_index(argsAst, i, args_offset) < ast_length(argsAst, argsTok.len, args_offset) - 1 && !tokeq(argsTok.tab[i+1], ",")) {
                        // erreur de syntaxe
                        global_env->CODE_ERROR = 96;
                        global_env->LINENUMBER = lines->tab[offset + args_offset + i];
                        TreeListTemp_destroy(tb, &opt_args);
                        neon_free(liste);
                        return TREE_VOID;
                    }
                    else { // argument non optionnel
                        if (unlimited_arguments) {
                            // erreur : on ne peut pas mettre d'arguments obligatoires après ...
                            global_env->CODE_ERROR = 96;
                            global_env->LINENUMBER = lines->tab[offset + args_offset + i];
                            TreeListTemp_destroy(tb, &opt_args);
                            neon_free(liste);
                            return TREE_VOID;
                        }
                        TreeListTemp_append(&opt_args, TREE_VOID);
                        if_error {
                            TreeListTemp_destroy(tb, &opt_args);
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
                    TreeListTemp_destroy(tb, &opt_args);
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

    return NeTree_make_functiondef(tb, tokdup(name), opt_args, partial_func, lines->tab[offset]);
}





TreeBufferIndex createSyntaxTreeAux(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset) {
    /*printString("-----createSyntaxTreeAux------\n");
    toklist_aff(tokens);
    neon_pause("----\n");*/
    bool expression = false;
    int exprStart = 0;

    int index = 0, index2 = 0;

    TreeBufferIndex tree = NeTree_create(tb, TypeSyntaxtree, lines->tab[offset]);
    struct TreeListTemp temptreelist; TreeListTemp_init(&temptreelist);

    return_on_error(TREE_VOID);

    while (index < tokens->len) {

        index2 = ast[index]->fin - offset + 1; // on regarde où on va sauter, et on saute à cet endroit après
        // c'est important, car on modifie ast[index] dans le code qui suit        

        if (ast[index]->type == TYPE_TRYEXCEPT) {
            ast_pop(ast[index]);
            // on commence par le bloc try
            

            // try
            toklist tryTok = (toklist) {.tab = tokens->tab + index + 2, .len = ast[index]->fin - offset - index - 2};

            TreeBufferIndex try_tree = createSyntaxTreeAux(tb, ast + index + 2, &tryTok, lines, offset + index + 2);

            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                return TREE_VOID;
            }

            struct TreeListTemp except_blocks;
            TreeListTemp_init(&except_blocks);

            //except
            int i = ast[index]->fin - offset + 1;
            // on se déplace jusqu'au premier except
            while (i < index2 && ast[i]->type != TYPE_STATEMENTEXCEPT)
                i = ast[i]->fin - offset + 1;

            while (i < index2) {

                int fin = ast[i]->fin - offset;
                ast_pop(ast[i]); // on est sur le blockline

                toklist blocToks = (toklist) {.tab = tokens->tab + ast[i]->fin - offset + 2, .len = fin - (ast[i]->fin - offset + 2)};
                
                TreeBufferIndex except_bloc = createSyntaxTreeAux(tb, ast + ast[i]->fin - offset + 2, &blocToks, lines, offset + ast[i]->fin - offset + 2);
                
                if_error
                {
                    NeTree_destroy(tb, try_tree);
                    TreeListTemp_destroy(tb, &temptreelist);
                    TreeListTemp_destroy(tb, &except_blocks);
                    return TREE_VOID;
                }

                toklist exprToks = (toklist) {.tab = tokens->tab + i + 2, .len = ast[i]->fin - offset - i - 2};

                struct TreeListTemp except_expr;
                TreeListTemp_init(&except_expr);

                if (exprToks.len > 0) // s'il y a au moins un argument
                {
                    createVirguleTree(tb, &except_expr, ast + i + 2, &exprToks, lines, offset + i + 2);

                    if_error
                    {
                        TreeListTemp_destroy(tb, &temptreelist);
                        TreeListTemp_destroy(tb, &except_blocks);
                        TreeListTemp_destroy(tb, &except_expr);
                        NeTree_destroy(tb, try_tree);
                        NeTree_destroy(tb, except_bloc);
                        return TREE_VOID;
                    }

                }

                TreeBufferIndex except = NeTree_make_except_block(tb, except_expr, except_bloc, lines->tab[offset + i]);
                
                if_error {
                    TreeListTemp_destroy(tb, &temptreelist);
                    TreeListTemp_destroy(tb, &except_blocks);
                    TreeListTemp_destroy(tb, &except_expr);
                    NeTree_destroy(tb, try_tree);
                    NeTree_destroy(tb, except_bloc);
                    return TREE_VOID;
                }
                
                TreeListTemp_append(&except_blocks, except);
                
                if_error {
                    TreeListTemp_destroy(tb, &temptreelist);
                    TreeListTemp_destroy(tb, &except_blocks);
                    NeTree_destroy(tb, try_tree);
                    NeTree_destroy(tb, except);
                    return TREE_VOID;
                }

                // on se déplace jusqu'au prochain except
                while (i < index2 && ast[i]->type != TYPE_STATEMENTEXCEPT)
                    i = ast[i]->fin - offset + 1;
            }

            TreeBufferIndex try_except = NeTree_make_tryexcept(tb, try_tree, except_blocks, lines->tab[offset + index]);

            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                TreeListTemp_destroy(tb, &except_blocks);
                NeTree_destroy(tb, try_tree);
                return TREE_VOID;
            }

            TreeListTemp_append(&temptreelist, try_except);
        }
        else if (ast[index]->type == TYPE_ATOMICBLOCK) {
            
            toklist blocTok = (toklist) {.tab = tokens->tab + index + 2, .len = ast[index]->fin - offset - index - 2};

            TreeBufferIndex body = createSyntaxTreeAux(tb, ast + index + 2, &blocTok, lines, offset + index + 2);

            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                return TREE_VOID;
            }

            TREE_TYPE(tb, body) = TypeAtomic;
            TreeListTemp_append(&temptreelist, body);
        }
        else if (ast[index]->type == TYPE_BLOCKWORD1LINE) {
            Token name = tokens->tab[index];
            toklist argsTok = (toklist) {.tab = tokens->tab + index + 2, .len = ast[index]->fin - offset - index - 2};
            Ast** argsAst = ast + index + 2;

            struct TreeListTemp params;
            TreeListTemp_init(&params);

            createVirguleTree(tb, &params, argsAst, &argsTok, lines, offset + index + 2);

            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                TreeListTemp_destroy(tb, &params);
                return TREE_VOID;
            }
            
            int code = get_blockword1Line_index(name) + 1;

            TreeBufferIndex fils = NeTree_make_kwparam(tb, params, code, lines->tab[offset + index]);

            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                TreeListTemp_destroy(tb, &params);
                return TREE_VOID;
            }
            
            TreeListTemp_append(&temptreelist, fils);
        }
        else if (ast[index]->type == TYPE_FUNCTIONDEF || ast[index]->type == TYPE_METHODDEF) {

            toklist stru = (toklist) {.tab = tokens->tab + index, .len = ast[index]->fin - offset + 1 - index};
            
            TreeBufferIndex fils = createFunctionTree(tb, ast + index, &stru, lines, offset + index, ast[index]->type == TYPE_METHODDEF);

            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                return TREE_VOID;
            }

            TreeListTemp_append(&temptreelist, fils);
        }
        else if (ast[index]->type == TYPE_KEYWORD) {
            TreeBufferIndex fils = NeTree_create(tb, TypeKeyword, lines->tab[offset + index]);
            treeKW(tb, fils)->code = get_lkeywords_index(tokens->tab[index]) + 1;
            
            TreeListTemp_append(&temptreelist, fils);
        }
        else if (ast[index]->type == TYPE_STATEMENTFOR || ast[index]->type == TYPE_STATEMENTFOREACH)  {

            toklist stru = (toklist) {.tab = tokens->tab + index, .len = ast[index]->fin - offset + 1 - index};

            TreeType type = (ast[index]->type == TYPE_STATEMENTFOR) ? TypeFor : TypeForeach;
            
            TreeBufferIndex fils = createStatementForTree(tb, ast + index, &stru, lines, offset + index, type);

            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                return TREE_VOID;
            }

            TreeListTemp_append(&temptreelist, fils);
        }
        else if (ast[index]->type == TYPE_STATEMENTWHILE)  {

            toklist stru = (toklist) {.tab = tokens->tab + index, .len = ast[index]->fin - offset + 1 - index};
            
            TreeBufferIndex fils = createStatementIEWTree(tb, ast + index, &stru, lines, offset + index, TypeWhile);

            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                return TREE_VOID;
            }

            TreeListTemp_append(&temptreelist, fils);
        }
        else if (ast[index]->type == TYPE_CONDITIONBLOCK) {
            
            toklist conBlock = (toklist) {.tab = tokens->tab + index, .len = ast[index]->fin - offset - index + 1};

            TreeBufferIndex st = createConditionBlockTree(tb, ast + index, &conBlock, lines, offset + index);

            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                return TREE_VOID;
            }

            TreeListTemp_append(&temptreelist, st);
        }
        else if (!expression && ast[index]->type != TYPE_ENDOFLINE) { // this is of course the beginning of an expression
            expression = true;
            exprStart = index;
        }

        else if (expression && ast[index]->type == TYPE_ENDOFLINE) { // si l'expression est terminée, on la compute
            
            // compute an expression between exprStart and index
            toklist exprToks = (toklist) {.tab = tokens->tab + exprStart, .len = index - exprStart};

            TreeBufferIndex expr = createExpressionTreeAux(tb, ast + exprStart, &exprToks, lines, offset + exprStart);
            
            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                return TREE_VOID;
            }

            TreeListTemp_append(&temptreelist, expr);

            expression = false;
        }
        
        // pour permettre de commencer et finir des expressions durant le même tour dans le seul cas d'une expression tout à la fin
        if (expression && index2 >= tokens->len && global_env->CODE_ERROR==0) { // on sait qu'au prochain tour on sera partis, donc on prend tout le reste
            
            toklist exprToks = (toklist) {.tab = tokens->tab + exprStart, .len = tokens->len - exprStart};

            TreeBufferIndex expr = createExpressionTreeAux(tb, ast + exprStart, &exprToks, lines, offset + exprStart);

            if_error {
                TreeListTemp_destroy(tb, &temptreelist);
                return TREE_VOID;
            }

            TreeListTemp_append(&temptreelist, expr);

            expression = false;
        }

        if_error {
            TreeListTemp_destroy(tb, &temptreelist);
            return TREE_VOID;
        }

        index = index2;

    }

    TREELIST_AFFECT(
        treeSntxTree(tb, tree)->treelist,
        TreeListTemp_dump(tb, &temptreelist)
    );

    return tree;
}







/*
Cette fonction prend en argument une chaine de caractères représentant un programme
et un booléen indiquant si on peut libérer la chaîne de caractères dès qu'on en n'a plus besoin
Elle renvoie un TreeBuffer prêt à être exécuté
*/
TreeBuffer createSyntaxTree(char* program, bool free_after)
{

    toklist* tokens = toklist_create(0);
    intlist types = intlist_create(0);
    intlist lines = intlist_create(0);
    Ast** ast;

    cut(tokens, &types, program, true, &lines, free_after);

    if_error {
        neon_free(types.tab);
        neon_free(lines.tab);
        toklist_destroy(tokens);
        return (TreeBuffer){0};
    }

    //mem_stat printString("Taille tokens : ");
    //mem_stat printInt((1<< tokens->capacity)*sizeof(Token));
    //mem_stat newLine();
    //mem_stat printString("Taille types : ");
    //mem_stat printInt((1<< types.capacity)*sizeof(int));
    //mem_stat newLine();
    //mem_stat printString("Taille lines : ");
    //mem_stat printInt((1<< lines.capacity)*sizeof(int));
    //mem_stat newLine();

    ast = ast_create(&types);

    //mem_stat printString("Taille ast : ");
    //mem_stat printInt((types.len * sizeof(Ast*)));
    //mem_stat newLine();

    if (ast == NULL) {
        global_env->CODE_ERROR = 12;
        neon_free(types.tab);
        neon_free(lines.tab);
        toklist_destroy(tokens);
        return (TreeBuffer){0};
    }

    parse(tokens, types, ast, &lines, 0);

    if_error {
        neon_free(types.tab);
        neon_free(lines.tab);
        ast_destroy(ast, tokens->len);
        toklist_destroy(tokens);
        return (TreeBuffer){0};
    }

    statements(&types, tokens, ast, &lines, 0);

    if_error {
        neon_free(types.tab);
        neon_free(lines.tab);
        ast_destroy(ast, tokens->len);
        toklist_destroy(tokens);
        return (TreeBuffer){0};
    }

    TreeBuffer tb;
    TreeBuffer_init(&tb);
    tb.entry_point = createSyntaxTreeAux(&tb, ast, tokens, &lines, 0);

    if_error {
        TreeBuffer_destroy(&tb);
    }

    //mem_stat printString("taille buffer : "); printInt(tb->block_size * tb->n_blocks);newLine();
    //mem_stat printString("taille fonctions : "); printInt(global_env->FONCTIONS.block_size * global_env->FONCTIONS.n_blocks);newLine(); neon_pause("");

    ast_destroy(ast, tokens->len);
    toklist_destroy(tokens);
    neon_free(types.tab);
    neon_free(lines.tab);

    return tb;
}