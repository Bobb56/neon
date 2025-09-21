#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "headers.c"

extern int CODE_ERROR;

extern Tree* FONCTIONS;

extern strlist* CONTAINERS;
extern NeList* ATTRIBUTES;

//déclaration des variables globales à cut
extern strlist acceptedChars;
extern listlist syntax;
extern strlist sousop;
extern intlist gramm1;
extern strlist operateurs3;
extern strlist operateurs1;
extern strlist operateurs2;
extern strlist blockwords;
extern strlist blockwords1Line;
extern strlist boolean;
extern strlist exceptions;
extern strlist keywords;
extern strlist lkeywords;
extern strlist constant;


extern strlist OPERATEURS;
extern intlist PRIORITE;
extern intlist OPERANDES;

extern void* PTRERROR;

//stockage des variables
extern strlist* NOMS;
extern NeList* ADRESSES;

extern strlist NOMSBUILTINSFONC;
extern strlist HELPBUILTINSFONC;
extern intlist TYPESBUILTINSFONC;

extern NeObject* (*OPFONC[NBOPERATEURS])(NeObject*,NeObject*);
extern NeObject* (*BUILTINSFONC[NBBUILTINFONC])(NeList*);



extern int error;





Tree* tree_create(char* label1, Number label2, char type)
{
    Tree* newTree = err_malloc(sizeof(Tree));

    if (newTree == NULL)
    {
        CODE_ERROR = 12;
        return NULL;
    }
    
    newTree->label1 = label1;
    newTree->label2 = label2;
    newTree->type = type;
    
    // création de la liste de fils
    newTree->capacity = 0;

    Tree** tmp = err_malloc(0);
    
    if (tmp == NULL)
    {
        CODE_ERROR = 12;
        err_free(newTree);
        return NULL;
    }
    
    newTree->sons=tmp;
    newTree->nbSons = 0;
    newTree->data = NULL;


    return newTree;
}




void tree_appendSon(Tree* tree, Tree* son)
{
  Tree** tmp;
  if (pow(2, tree->capacity) >= tree->nbSons)
  {
    tree->capacity++;
    
    //réallocation
    tmp = err_malloc(pow(2, tree->capacity)*sizeof(Tree*));
    
    
    for (int i=0 ; i<tree->nbSons ; i++)
    {
        tmp[i] = tree->sons[i];
    }
    
    err_free(tree->sons);
    
    
    tree->sons = tmp;//affectation du pointeur de tmp vers tree.tab
  }

  tree->sons[tree->nbSons]=son;
  tree->nbSons++;
  return;
}







void tree_destroy(Tree* tree)
{

    //printf("type arbre supprime : %c    %s\n", tree->type, tree->label1);

    if (tree->label1 != NULL)
        err_free(tree->label1);
    
    number_destroy(tree->label2);
    
    if (tree->nbSons > 0)
    {
        for (int i=0 ; i<tree->nbSons ; i++)
            tree_destroy(tree->sons[i]);
    }
    err_free(tree->sons);
    
    if (tree->data != NULL && tree->type != TYPE_VARIABLE && tree->type != TYPE_LISTINDEX && tree->type != TYPE_FONCTION && tree->type != TYPE_FUNCTIONDEF)
        neobject_destroy(tree->data,true);
    else if (tree->type == TYPE_FUNCTIONDEF)
    {
        UserFunc* fun = tree->data->data;
        err_free(fun->args);
        err_free(fun);
        err_free(tree->data);
    }
    
    err_free(tree);
    return ;
}








void tree_removeSon(Tree* tree, int index)
{
  if (index >= tree->nbSons)
  {
    CODE_ERROR = 13;
    return ;
  }
  
  tree_destroy(tree->sons[index]);
  
  for (int i = index ; i < tree->nbSons -1; i++)//décale tous les éléments à partir de celui à supprimer
    tree->sons[i]=tree->sons[i+1];
  
  Tree **tmp;
  
  if (pow(2, tree->capacity-1)==tree->nbSons-1)
  {
    tree->capacity--;
    tmp = err_realloc(tree->sons, pow(2, tree->capacity)*sizeof(Tree));//réalloue un nouveau pointeur de la bonne taille
    tree->sons = tmp;
  }
  tree->nbSons--;//décrémentation de la longueur
}







Tree* tree_copy(Tree* tree)
{
    // on suppose ici que Adresses et Noms sont déjà initialisés avec les fonctions built-ins
    
    Tree* tree2 = tree_create(NULL, number_default(), 0);
    if (CODE_ERROR != 0)
        return NULL;

    tree2->type = tree->type;
    tree2->label1 = (tree->label1 == NULL) ? NULL : strdup(tree->label1);
    tree2->label2 = tree->label2;
    
    if (tree->data == NULL)
        tree2->data = NULL;
    else
    {
        if (tree->type == TYPE_VARIABLE || tree->type == TYPE_FONCTION)
        {
            // il faut vérifier s'il n'existe pas déjà un objet du même nom, auquel cas il faudra lui faire prendre la même adresse
            tree2->data = tree->data;
        }
        else
            tree2->data = neo_copy(tree->data);
    }

    for (int i = 0 ; i < tree->nbSons ; i++)
    {
        tree_appendSon(tree2, tree_copy(tree->sons[i]));
    }

    return tree2;
}






char* fonctionArgs(char* fonction, char* set)
{
    int len = strlen(fonction);
    int sov = 0;
    int nbPar = 0;
    int nbCro = 0;
    _Bool str1 = false;
    _Bool str2 = false;
    _Bool precId = false; // indique si le caractère précédent appartenait à un identifier pour les '
    int i = 0;

    while (i < len)
    {

        if ((str1 || str2) && fonction[i] == '\\')
            i+=2;
        
        if (!str1 && !str2) // si l'on n'est pas une chaine de caractères
        {

            if (isalpha(fonction[i]) || fonction[i] == '_' || precId && isalnum(fonction[i]))
                precId = true;
            else if (fonction[i] != '\'')
                precId = false;

            if (!precId && fonction[i] == '\'')
                str2 = true;

            else if (fonction[i] == '"')
                str1 = true;

            // parenthèses & crochets
            
            if (fonction[i] == '(')
                nbPar ++;
            else if (fonction[i] == ')')
                nbPar --;

            if (fonction[i] == '[')
                nbCro ++;
            else if (fonction[i] == ']')
                nbCro --;



            // on cherche si on change de [...][...], mais il ne faut pas être dans une chaine de caractères
            // l'update de sov ne peut pas se faire dans une chaine de caractères
            if (set[0] == '(')
            {
                if (nbPar == 1 && fonction[i] == '(')
                    sov = i;

            }
            else if (set[0] == '[')
            {
                if (nbCro == 1 && fonction[i] == '[')
                    sov = i;
            }

        }
        else if (str2)
        {
            if (fonction[i] == '\'')
                str2 = false;
        }
        else if (str1)
        {
            if (fonction[i] == '"')
                str1 = false;
        }


        i++;
    }


    if (len-sov <= 2)
        return NULL;
    else
        return sub(fonction, sov+1, len-1);
}



char* fonctionName(char* fonction, char* set)
{
    int len = strlen(fonction);
    int sov = 0;
    int nbPar = 0;
    int nbCro = 0;
    _Bool str1 = false;
    _Bool str2 = false;
    _Bool precId = false; // indique si le caractère précédent appartenait à un identifier pour les '
    int i = 0;

    while (i < len)
    {

        if ((str1 || str2) && fonction[i] == '\\')
            i+=2;
        
        if (!str1 && !str2) // si l'on n'est pas une chaine de caractères
        {

            if (isalpha(fonction[i]) || fonction[i] == '_' || precId && isalnum(fonction[i]))
                precId = true;
            else if (fonction[i] != '\'')
                precId = false;

            if (!precId && fonction[i] == '\'')
                str2 = true;

            else if (fonction[i] == '"')
                str1 = true;

            // parenthèses & crochets
            
            if (fonction[i] == '(')
                nbPar ++;
            else if (fonction[i] == ')')
                nbPar --;

            if (fonction[i] == '[')
                nbCro ++;
            else if (fonction[i] == ']')
                nbCro --;




            // on cherche si on change de [...][...], mais il ne faut pas être dans une chaine de caractères
            // l'update de sov ne peut pas se faire dans une chaine de caractères
            if (set[0] == '(')
            {
                if (nbPar == 1 && fonction[i] == '(')
                    sov = i;

            }
            else if (set[0] == '[')
            {
                if (nbCro == 1 && fonction[i] == '[')
                    sov = i;
            }

        }
        else if (str2)
        {
            if (fonction[i] == '\'')
                str2 = false;
        }
        else if (str1)
        {
            if (fonction[i] == '"')
                str1 = false;
        }


        i++;
    }

    return sub(fonction, 0, sov);
}










char* expressionFromStatement(char* statement)
{
    int i=0;
    while (statement[i] != '(')
        i++;
    
    i++;
    int deb = i;
    
    int nbPar = 0;
    while (statement[i] != ')' || nbPar !=0)
    {
        if (statement[i] == '(')
            nbPar++;
        else if (statement[i] == ')')
            nbPar--;
        i++;
        
    }
    
    return sub(statement, deb, i);
}




char* blockFromStatement(char* statement)
{    
    int i=0;
    while (statement[i] != '{')
        i++;
    
    return sub(statement, i+1, strlen(statement)-1);
}








void calcPriorites(strlist* lexemes, intlist* nature, intlist* ordre)
{  
  // assignement de l'ordre des calculs
  
  *ordre=intlist_create(lexemes->len);
  for(int i=0;i<ordre->len;i++)
    ordre->tab[i]=0;
  
  int numero = 1;
  
  
  //prise en compte des parentheses
  int fin,deb,i;
  while (strlist_count(lexemes,"(")!=0 && strlist_count(lexemes,")")!=0)
  {
    i=0;
    while (strcmp(")",lexemes->tab[i])!=0) {i++;}
    fin=i;
    while (strcmp("(",lexemes->tab[i])!=0) {i--;}
    deb=i;
    
    for (int rang=1 ; rang<intlist_max(&PRIORITE)+1;rang++)
    {
      for (int i=deb;i<fin;i++)
      {
        if (nature->tab[i]==TYPE_OPERATOR)
        {
          if (PRIORITE.tab[strlist_index(&OPERATEURS, lexemes->tab[i])]==rang && ordre->tab[i]==0)
          {
            ordre->tab[i]=numero;
            numero++;
          }
        }
      }
    }
    
    strlist_remove(lexemes,fin,1);strlist_remove(lexemes,deb,1);
    intlist_remove(nature,fin);intlist_remove(nature,deb);
    intlist_remove(ordre,fin);intlist_remove(ordre,deb);
    
  }
  
  
  //termine sans les parentheses
  for (int rang=1 ; rang<intlist_max(&PRIORITE)+1;rang++)
  {
    for (int i=0;i<lexemes->len;i++)
    {
      if (nature->tab[i]==TYPE_OPERATOR)
      {
        if (PRIORITE.tab[strlist_index(&OPERATEURS, lexemes->tab[i])]==rang && ordre->tab[i]==0)
        {
          ordre->tab[i]=numero;
          numero++;
        }
      }
    }
  }
}












void createExpressionTreeAux(Tree* tree, strlist* tokens, intlist* types, intlist* ordre)
{
    
    if (tokens->len == 1) // traitement des cas de base de la fonction récursive
    {
        
        
        if (types->tab[0] == TYPE_VARIABLE)
        {
            // dans ce cas, la champ tree->data de l'arbre va servir à stocker directement l'adresse de la variable. On pourra donc y accéder directement depuis l'arbre de la même manière qu'on le ferait dans ADRESSES, au lieu de mettre l'adresse dans une autre NeObject, ce qui obligerait à passer dans une dimension de plus de NeObject.
            //il va falloir ajouter à NOMS le nom de la variable, allouer un NeObject* et le mettre comme feuille et dans ADRESSES
            // le fait de manipuler des NeObject* permet de pouvoir modifier le contenu (l'objet en lui-même, mais sans son adresse. Plus de liberté)
            // ajout de la variable :

            // d'abord on vérifie si la variable existe déjà, sinon, on va en créer une copie inutile et qui va entrer en conflit avec la précédente, bref : c'est pas bien !
            if (strlist_inList(NOMS,tokens->tab[0]))
            {
                tree->data = ADRESSES->tab[strlist_index(NOMS,tokens->tab[0])];
            }
            else
            {
                strlist_append(NOMS,strdup(tokens->tab[0])); // on ajoute le nom à la liste des noms
                tree->data = neobject_create(TYPE_EMPTY); // création de l'adresse du NeObject, mais celui-ci est symbolique pour l'instant : on ne sait pas encore ce qu'on va mettre dedans
                nelist_append(ADRESSES,tree->data); // on ajoute l'objet à la liste des variables
                // donc normalement, l'index du nom de la variable et de son adresse dans ADRESSES sont les mêmes
            }
            tree->type = TYPE_VARIABLE;

        }

        if (types->tab[0] == TYPE_BOOL)
        {
            tree->data = neo_bool_create(strToBool(tokens->tab[0]));// création d'un NeObject correspondant à la valeur contenue dans la feuille
            tree->type = TYPE_BOOL;
        }

        if (types->tab[0] == TYPE_EXCEPTION)
        {
            // créer une exception
            int index = strlist_index(&exceptions, tokens->tab[0]);
            tree->data = neo_exception_create(index);
            tree->type = TYPE_EXCEPTION;
        }


        if (types->tab[0] == TYPE_CONST)
        {
            tree->data = neo_const_create(strdup(tokens->tab[0]));// création d'un NeObject correspondant à la valeur contenue dans la feuille
            tree->type = TYPE_CONST;
        }


        if (types->tab[0] == TYPE_NONE)
        {
            tree->data = neo_none_create();// création d'un NeObject correspondant à la valeur contenue dans la feuille
            tree->type = TYPE_NONE;
        }
        
        if (types->tab[0] == TYPE_FONCTION)
        {
            
            // ou alors on a un seul argument et il faut l'ajouter en tant que fils, ou alors on a deux arguments et il faut juste changer les caractéristiques de l'arbre à virgule
            char* nomFonc = fonctionName(tokens->tab[0], "()");
            char* args = fonctionArgs(tokens->tab[0], "()");

            strlist* tok;intlist typ;

            if (args != NULL) // fonction sans arguments, on saute direct en bas
            {
                tok = strlist_create(0);
                cut(tok, &typ, args, false); // découpee en une liste d'arguments

                if (CODE_ERROR != 0)
                {
                    err_free(args);
                    err_free(nomFonc);
                    strlist_destroy(tok,true);
                    err_free(typ.tab);
                    return ;
                }
            }


            if (args != NULL && strlist_inList(tok, ":"))
            {
                // ce n'est pas une fonction mais un constructeur de container, on réutilise le tableau
                
                // tok contient maintenant une liste des 'arguments' de l'objet

                int i = 0; // indice de tok

                while (i < tok->len - 2)
                {
                    Tree* t1 = tree_create(NULL, number_default(), TYPE_VIRGULE);

                    if (CODE_ERROR != 0)
                    {
                        err_free(args);
                        err_free(nomFonc);
                        strlist_destroy(tok,true);
                        err_free(typ.tab);
                        return ;
                    }

                    if (typ.tab[i] != TYPE_VARIABLE || strcmp(tok->tab[i+1],":") != 0) // le champ n'a pas un nom correct ou n'est pas suivi de :
                    {
                        CODE_ERROR = 87;
                        err_free(args);
                        err_free(nomFonc);
                        tree_destroy(t1);
                        strlist_destroy(tok,true);
                        err_free(typ.tab);
                        return ;
                    }

                    t1->label1 = strdup(tok->tab[i]); // nom du champ


                    i+=2; // on saute le ':'. On a le droit car la condition de la boucle while le permet

                    // calcule l'arbre du truc à droite du ':'
                    int k = i;
                    while (k < typ.len && typ.tab[k] != TYPE_VIRGULE) k++; // saute à l'argument suivant

                    // prend la partie après la virgule, la première partie est 1 seul token que l'on prend au début de la boucle en ajoutant un de plus à i
                    strlist tok2 = (strlist) {.tab = tok->tab + i, .len = k-i};
                    intlist typ2 = (intlist) {.tab = typ.tab + i, .len = k-i};
                    intlist ordre;
                    calcPriorites(&tok2, &typ2, &ordre);

                    Tree* t = tree_create(NULL, number_default(), 0);
                    if (CODE_ERROR != 0)
                    {
                        err_free(args);
                        err_free(nomFonc);
                        strlist_destroy(tok,true);
                        err_free(typ.tab);
                        tree_destroy(t1);
                        err_free(ordre.tab);
                        return ;
                    }

                    createExpressionTreeAux(t, &tok2, &typ2, &ordre);

                    err_free(ordre.tab);

                    if (CODE_ERROR != 0)
                    {
                        err_free(args);
                        err_free(nomFonc);
                        strlist_destroy(tok,true);
                        err_free(typ.tab);
                        tree_destroy(t1);tree_destroy(t);
                        return ;
                    }

                    tree_appendSon(t1, t); // l'arbre représentant l'expression est fils d'un arbre qui contient juste le nom de l'attribut
                    tree_appendSon(tree, t1); // l'arbre est encore fils de l'arbre container

                    i=k+1;
                }

                if (i+2 >= tok->len && i < tok->len) // c'est que la définition du container est incorrecte
                {
                    CODE_ERROR = 87;
                    err_free(args);
                    err_free(nomFonc);
                    strlist_destroy(tok,true);
                    err_free(typ.tab);
                    return ;
                }

                strlist_destroy(tok, true);
                err_free(typ.tab);

                int index = strlist_index(CONTAINERS, nomFonc);

                if (CODE_ERROR == 18) // si le type n'existe pas déjà
                {
                    CODE_ERROR = 0;

                    // il faut vérifier qu'il n'y a pas déjà le même nom d'attribut
                    for (int i=0 ; i<tree->nbSons ; i++)
                    {
                        for (int j=0 ; j<tree->nbSons ; j++)
                        {
                            if (i != j && strcmp(tree->sons[i]->label1, tree->sons[j]->label1) == 0) //doublon
                            {
                                CODE_ERROR = 86;
                                err_free(args);
                                err_free(nomFonc);
                                return ;
                            }
                        }
                    }

                    // on est bon, donc on crée le type
                    tree->label2 = number_fromDouble((double)CONTAINERS->len);
                    strlist_append(CONTAINERS, nomFonc); // ajout du nom de container
                    NeList* attr_names = nelist_create(tree->nbSons); // liste des noms d'attributs

                    for (int i = 0 ; i < tree->nbSons ; i++) // création de la liste des attributs
                        attr_names->tab[i] = neo_str_create(strdup(tree->sons[i]->label1));
                    
                    nelist_append(ATTRIBUTES, neo_list_convert(attr_names)); // ajout de la liste

                }
                else // on vérifie que les champs sont bien comme ils ont été définis
                {
                    tree->label2 = number_fromDouble((double)index);

                    NeList* noms = (NeList*)(ATTRIBUTES->tab[index]->data); // liste des noms définis

                    if (noms->len != tree->nbSons)
                    {
                        CODE_ERROR = 83;
                        err_free(args);
                        err_free(nomFonc);
                        return ;
                    }

                    // on va vérifier qu'il y a tous les champs, et s'ils ne sont pas dans l'ordre, on les remet dans l'ordre
                    Tree* tree2 = tree_create(NULL, number_default(), 0);

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
                            CODE_ERROR = 83;

                            number_destroy(tree2->label2);
                            err_free(tree2->sons);
                            err_free(tree2);

                            err_free(args);
                            err_free(nomFonc);
                            return ;
                        }

                        // ajout de la branche au nouvel arbre réorganisé
                        tree_appendSon(tree2, tree->sons[k]);
                    }
                    // petit échange
                    err_free(tree->sons);
                    tree->sons = tree2->sons;
                    // libération de ce qui reste de tree2
                    number_destroy(tree2->label2);
                    err_free(tree2);

                    err_free(nomFonc);
                }

                tree->label1 = NULL;
                tree->type = TYPE_CONTAINER;

                err_free(args);

            }
            else
            {

                if (args != NULL)
                {
                    strlist_destroy(tok, true);
                    err_free(typ.tab);
                }

                // c'est bel et bien une fonction
            
                if (args != NULL)
                {
                    strlist* tokens2 = strlist_create(0);
                    intlist types2;
                    cut(tokens2, &types2, args, false);
                    
                    if (CODE_ERROR != 0)
                    {
                        strlist_destroy(tokens2, true);
                        err_free(types2.tab);
                        err_free(args);
                        return;
                    }
                    
                    intlist ordre2;
                    calcPriorites(tokens2, &types2, &ordre2);
                    int nbVirgules = intlist_inList(&types2, TYPE_VIRGULE);
                    
                    if (nbVirgules > 0)
                    {
                        createExpressionTreeAux(tree, tokens2, &types2, &ordre2);
                        if (CODE_ERROR != 0)
                        {
                            strlist_destroy(tokens2, true);
                            err_free(args);
                            return;
                        }
                        
                        err_free(tree->label1);
                    }
                    else
                    {
                        Tree* fils = tree_create(NULL, number_default(), 0);
                        if (CODE_ERROR != 0)
                        {
                            strlist_destroy(tokens2, true);
                            err_free(args);
                            return;
                        }
                        createExpressionTreeAux(fils, tokens2, &types2, &ordre2);
                        if (CODE_ERROR != 0)
                        {
                            tree_destroy(fils);
                            strlist_destroy(tokens2, true);
                            err_free(args);
                            return;
                        }
                        tree_appendSon(tree, fils);
                    }
                    strlist_destroy(tokens2, true);
                    err_free(types2.tab);
                    err_free(ordre2.tab);
                    err_free(args);
                }

                // importation du pointeur de la fonction

                //il faut au préalable évaluer le nom de la fonction : en effet, ça peut être un index de liste, une autre fonction, ...
                strlist* tok = strlist_create(0);
                intlist typ;

                cut(tok, &typ, nomFonc, false);
                if (CODE_ERROR != 0)
                {
                    err_free(nomFonc);
                    strlist_destroy(tok, true);
                    err_free(typ.tab);
                    return;
                }
                
                if (tok->len != 1)
                {
                    CODE_ERROR = 23;
                    err_free(nomFonc);
                    strlist_destroy(tok, true);
                    return ;
                }

                
                Tree* tree_aux = tree_create(NULL, number_default(), 0);
                if (CODE_ERROR != 0)
                {
                    err_free(nomFonc);
                    strlist_destroy(tok, true);
                    return;
                }
                
                createExpressionTree(tree_aux, tok->tab[0]);

                if (CODE_ERROR != 0)
                {
                    tree_destroy(tree_aux);
                    err_free(nomFonc);
                    strlist_destroy(tok, true);
                    return;
                }
                
                // on stocke le nom de la fonction
                // grâce au fait qu'on a calculé l'arbre avec le nom de la variable dedans, la variable a été créée
                tree->data = evalAux(tree_aux, false); // on sous entend que la fonction portant ce nom existe déjà, car elle a été créée lorsque l'on a appellé createExpressionTreeAux sur le nom de la variable

                if (CODE_ERROR != 0)
                {
                    tree_destroy(tree_aux);
                    err_free(nomFonc);
                    strlist_destroy(tok, true);
                    return;
                }

                err_free(nomFonc);
                tree_destroy(tree_aux);
                strlist_destroy(tok, true);
                err_free(typ.tab);
                

                tree->label1 = NULL;
                tree->type = TYPE_FONCTION;
            }
        }
        
        if (types->tab[0] == TYPE_NUMBER)
        {
            tree->data = neo_nb_create(number_fromStr(tokens->tab[0]));// création d'un NeObject correspondant à la valeur contenue dans la feuille
            tree->type = TYPE_NUMBER;
        }
        
        if (types->tab[0] == TYPE_LISTINDEX) // un arbre listindex a deux fils : le fils gauche contient l'arbre correspondant à la liste et le fils droit l'arbre de l'index
        {
            char* name = fonctionName(tokens->tab[0], "[]");
            
            
            Tree* fils2 = tree_create(NULL, number_default(), 0);
            if (CODE_ERROR != 0)
            {
                err_free(name);
                return;
            }
            
            createExpressionTree(fils2, name);

            if (CODE_ERROR != 0)
            {
                err_free(name);
                tree_destroy(fils2);
                return;
            }
            
            tree_appendSon(tree, fils2);

            char* args = fonctionArgs(tokens->tab[0], "[]");


            Tree* fils = tree_create(NULL, number_default(), 0);
            if (CODE_ERROR != 0)
            {
                err_free(name);
                err_free(args);
                return;
            }
            
            createExpressionTree(fils, args);
            
            if (CODE_ERROR != 0)
            {
                err_free(name);
                err_free(args);
                tree_destroy(fils);
                return;
            }
            
            
            tree_appendSon(tree, fils);

            tree->label2 = number_default();
            tree->type = TYPE_LISTINDEX;
            
            err_free(name);
            err_free(args);
            

        }
        
        if (types->tab[0] == TYPE_STRING)
        {
            char* intermediaire = sub(tokens->tab[0],1,strlen(tokens->tab[0])-1); // on enlève les guillemets
            tree->data = neo_str_create(traitementString(intermediaire));
            tree->type = TYPE_STRING;
            err_free(intermediaire);
        }
        
        if (types->tab[0] == TYPE_LIST)
        {
            char* l2 = sub(tokens->tab[0], 1, strlen(tokens->tab[0])-1);

            
            if (strcmp(l2, "")==0)
            {
                tree->nbSons = 0;
            }
            else
            {
                Tree* tree2 = tree_create(NULL, number_default(), 0);
                if (CODE_ERROR != 0)
                {
                    err_free(l2);
                    return;
                }
                
                createExpressionTree(tree2, l2);

                if (CODE_ERROR != 0)
                {
                    err_free(l2);
                    tree_destroy(tree2);
                    return;
                }
                
                if (tree2->type != TYPE_VIRGULE) // ca veut dire que la liste ne contient qu'un seul element au maximum, donc que l'arbre n'est pas un arbre virgule
                {
                    tree_appendSon(tree, tree2);
                }
                else
                {
                    //err_free(tree);
                    err_free(tree->sons);
                    tree->sons = tree2->sons;
                    tree->capacity = tree2->capacity;
                    tree->nbSons = tree2->nbSons;
                    err_free(tree2->label1);
                    tree->label2 = tree2->label2;
                    tree->data = tree2->data;
                    err_free(tree2);
                }
                
            }
            
            err_free(l2);
            
            
            tree->label1 = NULL;
            
            tree->type = TYPE_LIST;
            tree->data = NULL;
            
        }
        
    }
    else if (tokens->len > 1)
    {
        
        int nbVirgules = intlist_count(types, TYPE_VIRGULE);
        
        if (nbVirgules > 0)
        {

            tree->data = NULL;
            
            
            tree->label1 = strdup(",");
            tree->type = TYPE_VIRGULE;
            
            Tree* arbre;
            int index, len;
            strlist* sous_tokens = strlist_create(0);
            intlist sous_types = intlist_create(0);
            intlist sous_ordre = intlist_create(0);
            
            
            for (int j=0 ; j <= nbVirgules ; j++)// boucle qui parcourt toutes les virgules
            {
                
                if (j==nbVirgules)
                    index = tokens->len;
                else
                    index = intlist_index(types, TYPE_VIRGULE);
                
                for (int l=0 ; l<index ; l++)// on crée des listes qui vont contenir les éléments à gauche de la première virgule
                {
                    strlist_append(sous_tokens, strdup(tokens->tab[l]));
                    intlist_append(&sous_types, types->tab[l]);
                    intlist_append(&sous_ordre, ordre->tab[l]);
                }
                
                
                arbre = tree_create(NULL, number_default(), 0);
                if (CODE_ERROR != 0)
                {
                    err_free(tree->label1);
                    tree->label1 = NULL;
                    strlist_destroy(sous_tokens, true);
                    err_free(sous_types.tab);
                    err_free(sous_ordre.tab);
                    
                    return;
                }
                
                createExpressionTreeAux(arbre, sous_tokens, &sous_types, &sous_ordre);// puis met ces éléments dans un arbre
                if (CODE_ERROR != 0)
                {
                    err_free(tree->label1);
                    tree->label1 = NULL;
                    strlist_destroy(sous_tokens, true);
                    err_free(sous_types.tab);
                    err_free(sous_ordre.tab);
                    tree_destroy(arbre);
                    return;
                }
                tree_appendSon(tree, arbre);// et on ajoute cet arbre à tree.
                
                
                strlist_destroy(sous_tokens, true);//on détruit les listes intermédiaires
                err_free(sous_types.tab);
                err_free(sous_ordre.tab);
                sous_tokens = strlist_create(0);
                sous_types = intlist_create(0);
                sous_ordre = intlist_create(0);
                
                
                len = tokens->len;
                for (int m=0 ; m <= index && m<len ; m++) // supprime tous les éléments à gauche de la première virgule ainsi que la virgule elle-même
                {
                    
                    strlist_remove(tokens, 0, true);
                    intlist_remove(types, 0);
                    intlist_remove(ordre, 0);
                }
                
            }
            
            strlist_destroy(sous_tokens, true);
            err_free(sous_types.tab);
            err_free(sous_ordre.tab);
        }
        else
        {
            
            // dans le cas où il y a des opérateurs dedans
            int numOp=intlist_max(ordre);
            
            if (numOp != 0)
            {
                tree->data = NULL;
                int index=intlist_index(ordre,numOp);

                if (strcmp(tokens->tab[index], ":")==0) // erreur : l'opérateur : n'est pas un opérateur normal
                {
                    CODE_ERROR = 92;
                    return ;
                }
                
                int placeOp = strlist_index(&OPERATEURS,tokens->tab[index]);
                
                int typeOperande=OPERANDES.tab[placeOp];
                
                tree->label1 = strdup(tokens->tab[index]);

                number_destroy(tree->label2);
                tree->label2 = number_fromDouble((double)placeOp);

                tree->type = TYPE_OPERATOR;
                
                if (typeOperande == RIGHT_LEFT)
                {

                    // si c'est un opérateur '.', on va changer la structure de l'arbre, donc c'est une opération différente
                    if ((long int)number_toDouble(tree->label2) == 30) // si c'est un point
                    {
                        err_free(tree->label1);
                        tree->label1 = NULL;

                        strlist tokensGauche = (strlist) {.tab = tokens->tab, .len = index};
                        intlist typesGauche = (intlist) {.tab = types->tab, .len = index};
                        intlist ordreGauche = (intlist) {.tab = ordre->tab, .len = index};
                        
                        strlist tokensDroite = (strlist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                        intlist typesDroite = (intlist){.tab = types->tab+index+1, .len = types->len-index-1};
                        intlist ordreDroite = (intlist){.tab = ordre->tab+index+1, .len = ordre->len-index-1};


                        createExpressionTreeAux(tree, &tokensDroite, &typesDroite, &ordreDroite);


                        if (CODE_ERROR != 0)
                            return ;


                        Tree* fils = tree_create(NULL, number_default(), 0);
                        if (CODE_ERROR != 0)
                            return;

                        createExpressionTreeAux(fils, &tokensGauche, &typesGauche, &ordreGauche);


                        if (CODE_ERROR != 0)
                        {
                            tree_destroy(fils);
                            return;
                        }
                        
                        
                        // on va rajouter en premier argument de la fonction l'opérande de gauche
                        if (tree->type != TYPE_FONCTION)
                        {
                            tree_destroy(fils);
                            CODE_ERROR = 72;

                            return;
                        }

                        tree_appendSon(tree, NULL); // augmentation de la taille du tableau
                        // décalage des éléments
                        for (int i = tree->nbSons - 1 ; i > 0 ; i--)
                            tree->sons[i] = tree->sons[i-1];

                        tree->sons[0] = fils; // la premier élément est la variable

                        
                    }
                    else if ((long int)number_toDouble(tree->label2) == 34) // opérateur >>
                    {
                        // tokens contient ["a", ">>", "b"]
                        err_free(tree->label1);
                        number_destroy(tree->label2);
                        tree->label2 = number_default();


                        // maintenant, il se peut que le membre de droite soit de la forme fonc(truc) ou encore tab[truc]

                        if (types->tab[types->len-1] == TYPE_LISTINDEX)
                        {
                            tree->label1 = NULL;
                            char* s1 = addStr(tokens->tab[0],tokens->tab[1]);
                            s1 = addStr2(s1,tokens->tab[2]);
                            strlist tok0 = (strlist) {.tab = (char*[]){s1}, .len = 1};
                            intlist typ0 = (intlist) {.tab = (int[]){TYPE_LISTINDEX}, .len = 1};
                            intlist ordre0 = (intlist) {.tab = (int[]){0}, .len = 1};

                            // on joue sur le fait que foncName et foncArgs interprète correctement "a>>tab[i]"
                            createExpressionTreeAux(tree, &tok0, &typ0, &ordre0);

                            err_free(s1);
                            return ;
                        }
                        else if (types->tab[types->len-1] == TYPE_FONCTION)
                        {
                            tree->label1 = NULL;
                            CODE_ERROR = 91;
                            return;
                        }

                        strlist tokensGauche = (strlist){.tab = tokens->tab, .len = index};
                        intlist typesGauche = (intlist){.tab = types->tab, .len = index};
                        intlist ordreGauche = (intlist){.tab = ordre->tab, .len = index};
                        
                        Tree* t = tree_create(NULL, number_default(), 0);
                        if (CODE_ERROR != 0)
                        {
                            return ;
                        }
                        
                        createExpressionTreeAux(t, &tokensGauche, &typesGauche, &ordreGauche);

                        if (CODE_ERROR != 0)
                        {
                            tree_destroy(t);
                            return ;
                        }

                        

                        tree->label1 = strdup(tokens->tab[tokens->len-1]); // à la condition qu'il y en a bien 3 dans la liste
                        tree_appendSon(tree, t);

                        tree->type = TYPE_ATTRIBUTE;
                    }
                    else
                    {
                        Tree* filsg = tree_create(NULL, number_default(), 0);
                        if (CODE_ERROR != 0)
                        {
                            err_free(tree->label1);
                            tree->label1 = NULL;
                            number_destroy(tree->label2);
                            return;
                        }
    
                        Tree* filsd = tree_create(NULL, number_default(), 0);
                        if (CODE_ERROR != 0)
                        {
                            err_free(tree->label1);
                            tree->label1 = NULL;
                            number_destroy(tree->label2);
                            tree_destroy(filsg);
                            return;
                        }
                        
                        // il va falloir séparer en deux l'expression, et recalculer l'arbre des expressions sous jacentes
                        
                        strlist tokensGauche = (strlist){.tab = tokens->tab, .len = index};
                        intlist typesGauche = (intlist){.tab = types->tab, .len = index};
                        intlist ordreGauche = (intlist){.tab = ordre->tab, .len = index};
                        
                        strlist tokensDroite = (strlist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                        intlist typesDroite = (intlist){.tab = types->tab+index+1, .len = types->len-index-1};
                        intlist ordreDroite = (intlist){.tab = ordre->tab+index+1, .len = ordre->len-index-1};
    
    
                        
                        createExpressionTreeAux(filsg, &tokensGauche, &typesGauche, &ordreGauche);
                        if (CODE_ERROR != 0)
                        {
                            tree_destroy(filsg);
                            tree_destroy(filsd);
                            return;
                        }
                        createExpressionTreeAux(filsd, &tokensDroite, &typesDroite, &ordreDroite);
                        if (CODE_ERROR != 0)
                        {
                            tree_destroy(filsg);
                            tree_destroy(filsd);
                            return;
                        }
                        
                        tree_appendSon(tree, filsg);
                        tree_appendSon(tree, filsd);
                    }
                    
                }
                
                if (typeOperande == VARLEFT) // variable à gauche comme -- ou ++
                {
                    
                    Tree* fils = tree_create(NULL, number_default(), 0);
                    if (CODE_ERROR != 0)
                        return;
                    
                    strlist ntokens = (strlist) {.tab = tokens->tab, .len = tokens->len-1};
                    intlist ntypes = (intlist) {.tab = types->tab, .len = types->len-1};
                    intlist nordre = (intlist) {.tab = ordre->tab, .len = ordre->len-1};
                    
                    
                    createExpressionTreeAux(fils, &ntokens, &ntypes, &nordre);
                    if (CODE_ERROR != 0)
                    {
                        tree_destroy(fils);
                        number_destroy(tree->label2);
                        err_free(tree->label1);
                        tree->label1 = NULL;
                        return ;
                    }
                    tree_appendSon(tree, fils);
                    
                    
                }
                
                if (typeOperande == VAR_RIGHT) // cas où il y a potentiellement un opérateur point : '.'
                {
              
                    Tree* filsg = tree_create(NULL, number_default(), 0);
                    if (CODE_ERROR != 0)
                    {
                        number_destroy(tree->label2);
                        err_free(tree->label1);
                        tree->label1 = NULL;
                        return;
                    }

                    Tree* filsd = tree_create(NULL, number_default(), 0);
                    
                    if (CODE_ERROR != 0)
                    {
                        tree_destroy(filsg);
                        number_destroy(tree->label2);
                        err_free(tree->label1);
                        tree->label1 = NULL;
                        return;
                    }
                    // il va falloir séparer en deux l'expression, et recalculer l'arbre des expressions sous jacentes
                    
                    strlist tokensGauche = (strlist){.tab = tokens->tab, .len = index};
                    intlist typesGauche = (intlist){.tab = types->tab, .len = index};
                    intlist ordreGauche = (intlist){.tab = ordre->tab, .len = index};
                    
                    
                    strlist tokensDroite = (strlist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                    intlist typesDroite = (intlist){.tab = types->tab+index+1, .len = types->len-index-1};
                    intlist ordreDroite = (intlist){.tab = ordre->tab+index+1, .len = ordre->len-index-1};

                    
                    createExpressionTreeAux(filsd, &tokensDroite, &typesDroite, &ordreDroite);
                    
                    if (CODE_ERROR != 0)
                    {
                        tree_destroy(filsg);
                        tree_destroy(filsd);
                        return;
                    }
                    createExpressionTreeAux(filsg, &tokensGauche, &typesGauche, &ordreGauche);
                    
                    if (CODE_ERROR != 0)
                    {
                        number_destroy(tree->label2);
                        err_free(tree->label1);
                        tree->label1 = NULL;
                        tree_destroy(filsg);
                        tree_destroy(filsd);
                        return;
                    }

                    
                    tree_appendSon(tree, filsg);
                    tree_appendSon(tree, filsd);
                    
                }



                if (typeOperande == LEFT_VAR) // cas où il y a potentiellement un opérateur point : '.'
                {
              
                    Tree* filsg = tree_create(NULL, number_default(), 0);
                    if (CODE_ERROR != 0)
                    {
                        number_destroy(tree->label2);
                        err_free(tree->label1);
                        tree->label1 = NULL;
                        return;
                    }

                    Tree* filsd = tree_create(NULL, number_default(), 0);
                    
                    if (CODE_ERROR != 0)
                    {
                        tree_destroy(filsg);
                        number_destroy(tree->label2);
                        err_free(tree->label1);
                        tree->label1 = NULL;
                        return;
                    }
                    // il va falloir séparer en deux l'expression, et recalculer l'arbre des expressions sous jacentes
                    
                    strlist tokensGauche = (strlist){.tab = tokens->tab, .len = index};
                    intlist typesGauche = (intlist){.tab = types->tab, .len = index};
                    intlist ordreGauche = (intlist){.tab = ordre->tab, .len = index};
                    
                    strlist tokensDroite = (strlist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                    intlist typesDroite = (intlist){.tab = types->tab+index+1, .len = types->len-index-1};
                    intlist ordreDroite = (intlist){.tab = ordre->tab+index+1, .len = ordre->len-index-1};
                    
                    createExpressionTreeAux(filsd, &tokensDroite, &typesDroite, &ordreDroite);
                    
                    if (CODE_ERROR != 0)
                    {
                        number_destroy(tree->label2);
                        err_free(tree->label1);
                        tree->label1 = NULL;
                        tree_destroy(filsg);
                        tree_destroy(filsd);
                        return;
                    }
                    createExpressionTreeAux(filsg, &tokensGauche, &typesGauche, &ordreGauche);
                    
                    if (CODE_ERROR != 0)
                    {
                        number_destroy(tree->label2);
                        err_free(tree->label1);
                        tree->label1 = NULL;
                        tree_destroy(filsg);
                        tree_destroy(filsd);
                        return;
                    }

                    
                    tree_appendSon(tree, filsg);
                    tree_appendSon(tree, filsd);
                    
                }


                
                if (typeOperande == VARRIGHT || typeOperande == RIGHT)
                {
                    
                    Tree* fils = tree_create(NULL, number_default(), 0);
                    if (CODE_ERROR != 0)
                        return;
                    
                    
                    strlist ntokens = (strlist) {.tab = tokens->tab+1, .len = tokens->len-1};
                    intlist ntypes = (intlist) {.tab = types->tab+1, .len = types->len-1};
                    intlist nordre = (intlist) {.tab = ordre->tab+1, .len = ordre->len-1};
                    
                    
                    createExpressionTreeAux(fils, &ntokens, &ntypes, &nordre);
                    if (CODE_ERROR != 0)
                    {
                        tree_destroy(fils);
                        return;
                    }
                    tree_appendSon(tree, fils);
                    
                }
            }
            
            
        }
        
    }
}











void createExpressionTree(Tree* tree, char* string)
{
    strlist* tokens = strlist_create(0);
    intlist types;
    cut(tokens, &types, string, false);
    if (CODE_ERROR != 0)
    {
        strlist_destroy(tokens, true);
        err_free(types.tab);
        return;
    }
    intlist ordre;
    calcPriorites(tokens, &types, &ordre);
    createExpressionTreeAux(tree, tokens, &types, &ordre);
    
    if (CODE_ERROR != 0)
    {
        strlist_destroy(tokens, true);
        err_free(types.tab);
        err_free(ordre.tab);
        return;
    }
    
    strlist_destroy(tokens, true);
    err_free(types.tab);
    err_free(ordre.tab);

    
    return;
    
}






void createStatementIEWFTree(Tree* tree, char* statement, int type)
{
    char* expression = expressionFromStatement(statement);
    char* block = blockFromStatement(statement);

    
    Tree* fils1 = tree_create(NULL, number_default(), 0);
    if (CODE_ERROR != 0)
    {
        err_free(expression);
        err_free(block);
        return;
    }
    createExpressionTree(fils1, expression);

    
    Tree* fils2 = tree_create(NULL, number_default(), 0);
    if (CODE_ERROR != 0)
    {
        tree_destroy(fils1);
        err_free(expression);
        err_free(block);
        return;
    }
    
    createSyntaxTree(fils2, block);
    if (CODE_ERROR != 0)
    {
        tree_destroy(fils1);
        tree_destroy(fils2);
        err_free(expression);
        err_free(block);
        return;
    }
    
    fils2->type = TYPE_BLOCK;
    
    tree_appendSon(tree, fils1);
    tree_appendSon(tree, fils2);

    
    err_free(expression);
    err_free(block);
    tree->type = type;
    tree->data = NULL;

    
    return ;
}






void createStatementElseTree(Tree* tree, char* statement)
{
    char* block = blockFromStatement(statement);
    
    createSyntaxTree(tree, block);

    //s'il y a une erreur, la fonction va quitter normalement et CODE_ERROR va faire le reste
    
    err_free(block);
    tree->type = TYPE_STATEMENTELSE;
    tree->data = NULL;
    return ;
}




void cutConditionBlock(strlist* tokens, intlist* types, char* conditionblock)
{
    cut(tokens, types, conditionblock, false);
    if (CODE_ERROR != 0)
        return;
    
    char* newStr, *nom;
    
    for (int i=0 ; i < tokens->len ; i++)
    {
        //tokens->tab[i] est forcement un blockline
        nom = nomBlockLine(tokens->tab[i]);
        
        newStr = addStr(tokens->tab[i], tokens->tab[i+1]);
        strlist_remove(tokens, i+1, true);intlist_remove(types, i+1);
        err_free(tokens->tab[i]);
        tokens->tab[i] = newStr;
        
        if (strcmp(nom, "if")==0)
        {
            types->tab[i] = TYPE_STATEMENTIF;
        }
        if (strcmp(nom, "ei")==0)
        {
            types->tab[i] = TYPE_STATEMENTELIF;
        }
        if (strcmp(nom, "es")==0)
        {
            types->tab[i] = TYPE_STATEMENTELSE;
        }
        
        err_free(nom);
        
        
    }
    
    
    
}









void createConditionBlockTree(Tree* tree, char* conditionblock)
{
    strlist* tokens = strlist_create(0);
    intlist types;
    
    cutConditionBlock(tokens, &types, conditionblock);// fonction qui découpe les conditionblock en statements, en donnant leurs types
    
    if (CODE_ERROR != 0)
    {
        strlist_destroy(tokens, true);
        err_free(types.tab);
        return;
    }
    
    
    for (int i=0 ; i<tokens->len ; i++)
    {
        if (types.tab[i] == TYPE_STATEMENTELSE)
        {
            Tree* fils = tree_create(NULL, number_default(), 0);
            if (CODE_ERROR != 0)
            {
                strlist_destroy(tokens, true);
                err_free(types.tab);
                return;
            }
            createStatementElseTree(fils, tokens->tab[i]);

            if (CODE_ERROR != 0)
            {
                tree_destroy(fils);
                strlist_destroy(tokens, true);
                err_free(types.tab);
                return;
            }

            tree_appendSon(tree, fils);
        }
        else
        {
            Tree* fils = tree_create(NULL, number_default(), 0);
            if (CODE_ERROR != 0)
            {
                strlist_destroy(tokens, true);
                err_free(types.tab);
                return;
            }
            createStatementIEWFTree(fils, tokens->tab[i], types.tab[i]);
            if (CODE_ERROR != 0)
            {
                strlist_destroy(tokens, true);
                err_free(types.tab);
                tree_destroy(fils);
                return;
            }
            tree_appendSon(tree, fils);
        }
    }
    
    tree->type = TYPE_CONDITIONBLOCK;
    
    strlist_destroy(tokens, true);
    err_free(types.tab);
    tree->data = NULL;
    
}




void createFunctionTree(Tree* tree, char* string, _Bool isMethod)
{
    int len = strlen(string);

    char* args = NULL; // va contenir le nom de la fonction avec les arguments
    char* code = NULL; // va contenir le code dans les accolades

    int i = 0;
    _Bool gui = false;
    while (string[i] != '{')
    {
        
        if (string[i] == '"')
            gui = !gui;
        
        i++;
    }
    // ici, i a la valeur de l'index de la première accolade

    args = sub(string, 0, i);
    code = sub(string, i+1, len - 1);

    Tree* syntaxTree = tree_create(NULL, number_default(), 0);
    if (CODE_ERROR != 0)
    {
        err_free(args);
        err_free(code);
        return;
    }

    createSyntaxTree(syntaxTree, code);
    if (CODE_ERROR != 0)
    {
        err_free(args);
        err_free(code);
        tree_destroy(syntaxTree);
        return;
    }
    

    
    err_free(code);

    //ajout de l'arbre de la fonction à la liste des fonctions
    tree_appendSon(FONCTIONS, syntaxTree);

    char* args2 = fonctionArgs(args, "()");
    char* name = fonctionName(args, "()");
    
    err_free(args);

    NeObject** tab = NULL; // tableau qui va contenir toutes les variables à affecter

    int nbArgs = 0;

    if (args2 != NULL) // si la fonction a des arguments
    {
        strlist* tokens = strlist_create(0);
        intlist types;
        cut(tokens, &types, args2, false);
        if (CODE_ERROR != 0)
        {
            strlist_destroy(tokens, true);
            err_free(types.tab);
            tree_destroy(syntaxTree);
            err_free(args2);
            err_free(name);
            return;
        }

        // suppression des virgules
        i=0;
        while (i < tokens->len)
        {
            if (strcmp(tokens->tab[i], ",") == 0)
                strlist_remove(tokens, i, true);
            else
                i++;
        }
        err_free(types.tab);
        err_free(args2);

        // il faut maintenant définir chaque variable dont le nom est dans tokens
        tab = err_malloc(sizeof(NeObject*) * tokens->len);
        nbArgs = tokens->len;
        
        
        for (i = 0 ; i < tokens -> len ; i++)
        {
            if (strlist_inList(NOMS,tokens->tab[i]))
            {
                tab[i] = ADRESSES->tab[strlist_index(NOMS,tokens->tab[i])];
            }
            else
            {
                strlist_append(NOMS,strdup(tokens->tab[i])); // on ajoute le nom à la liste des noms
                tab[i] = neobject_create(TYPE_EMPTY); // création de l'adresse du NeObject, mais celui-ci est symbolique pour l'instant : on ne sait pas encore ce qu'on va mettre dedans
                nelist_append(ADRESSES,tab[i]); // on ajoute l'objet à la liste des variables
            }
        }

        strlist_destroy(tokens, true);
        
    }

    // on dispose maintenant du tableau de variables et de l'arbre de code
    NeObject* neo = userFuncCreate(tab, syntaxTree, nbArgs); // objet destiné à être dans l'arbre
    if (isMethod)
        neo->type = TYPE_USERMETHOD;

    
    
    if (strlist_inList(NOMS,name))
    {
        int index = strlist_index(NOMS,name);
        _affect2(ADRESSES->tab[index], neo);
    }
    else
    {
        strlist_append(NOMS,strdup(name)); // on ajoute le nom à la liste des noms
        nelist_append(ADRESSES,neo_copy(neo)); // on ajoute l'objet à la liste des variables
    }
    
    tree->data = neo;
    tree->label1 = name;
    tree->type = TYPE_FUNCTIONDEF;

    return ;
}










void createSyntaxTree(Tree* tree, char* program)
{
    // tree est un pointeur vers un Tree, dans lequel on va mettre l'arbre de syntaxe. Ce pointeur peut avoir une valeur complètement arbitraire, puisqu'il est initialisé dans la fonction.
    // program est une chaine de caractères contenant tout le programme.
    // la fonction calcule l'arbre correspondant à la chaine de caractères récursivement, à l'aide de cut.
    strlist* tokens = strlist_create(0);
    intlist types;

    cut(tokens, &types, program, true);
    if (CODE_ERROR != 0)
    {
        err_free(types.tab);
        strlist_destroy(tokens, true);
        return;
    }
    
    for (int index=0 ; index < tokens->len ; index++)
    {

        if (types.tab[index] == TYPE_TRYEXCEPT)
        {
            strlist* tokens2 = strlist_create(0);
            intlist types2;
            cut(tokens2, &types2, tokens->tab[index], false);

            // on a donc forcément 4 objets dans tokens2
            Tree* try_except = tree_create(NULL, number_default(), 0);
            Tree* try_tree = tree_create(NULL, number_default(), 0);
            Tree* except_tree = tree_create(NULL, number_default(), 0);
            Tree* except_expr = tree_create(NULL, number_default(), 0);
            Tree* except_bloc = tree_create(NULL, number_default(), 0);

            // try
            char* bloc = blockFromStatement(tokens2->tab[1]);
            createSyntaxTree(try_tree, bloc);
            err_free(bloc);

            if (CODE_ERROR != 0)
            {
                tree_destroy(try_except);tree_destroy(try_tree);tree_destroy(except_tree);tree_destroy(except_expr);tree_destroy(except_bloc);
                strlist_destroy(tokens2, true);
                err_free(types2.tab);
                return ;
            }
            try_tree->type = TYPE_STATEMENTTRY;
            tree_appendSon(try_except, try_tree);

            //except
            bloc = blockFromStatement(tokens2->tab[3]);
            createSyntaxTree(except_bloc, bloc);
            err_free(bloc);
            
            if (CODE_ERROR != 0)
            {
                tree_destroy(try_except);tree_destroy(try_tree);tree_destroy(except_tree);tree_destroy(except_expr);tree_destroy(except_bloc);
                strlist_destroy(tokens2, true);
                err_free(types2.tab);
                return ;
            }

            char* expr = fonctionArgs(tokens2->tab[2], "()");


            if (expr != NULL)
            {
                createExpressionTree(except_expr, expr);
                err_free(expr);

                if (CODE_ERROR != 0)
                {
                    tree_destroy(try_except);tree_destroy(try_tree);tree_destroy(except_tree);tree_destroy(except_expr);tree_destroy(except_bloc);
                    strlist_destroy(tokens2, true);
                    err_free(types2.tab);
                    return;
                }


                if (except_expr->type != TYPE_VIRGULE) // il faut augmenter encore en profondeur
                {
                    Tree* t = tree_create(NULL, number_default(), 0);
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

            try_except->type = TYPE_TRYEXCEPT;
            
            tree_appendSon(tree, try_except);

            strlist_destroy(tokens2, true);
            err_free(types2.tab);
        }

        if (types.tab[index] == TYPE_BLOCKWORD1LINE)
        {
            char* name = fonctionName(tokens->tab[index], "()");
            char* arg = fonctionArgs(tokens->tab[index], "()");
            Tree* fils = tree_create(NULL, number_default(), 0);
            if (CODE_ERROR != 0)
            {
                err_free(name);err_free(arg);
                err_free(types.tab);
                strlist_destroy(tokens, true);
                return;
            }
            
            if (arg != NULL) // s'il y a au moins un argument
            {
                // création de l'arbre des arguments
                strlist* tokens2 = strlist_create(0);
                intlist types2;
                cut(tokens2, &types2, arg, false);
                if (CODE_ERROR != 0)
                {
                    strlist_destroy(tokens2, true);
                    err_free(types2.tab);
                    err_free(types.tab);
                    strlist_destroy(tokens, true);
                    err_free(name);err_free(arg);
                    tree_destroy(fils);
                    return;
                }
                
                intlist ordre2;
                calcPriorites(tokens2, &types2, &ordre2);
                int nbVirgules = intlist_inList(&types2, TYPE_VIRGULE);
                
                if (nbVirgules > 0)
                {
                    createExpressionTreeAux(fils, tokens2, &types2, &ordre2);
                    if (CODE_ERROR != 0)
                    {
                        strlist_destroy(tokens2, true);
                        err_free(types2.tab);
                        err_free(types.tab);
                        strlist_destroy(tokens, true);
                        err_free(name);err_free(arg);
                        tree_destroy(fils);
                        return;
                    }
                }
                else
                {
                    Tree* fils2 = tree_create(NULL, number_default(), 0);
                    if (CODE_ERROR != 0)
                    {
                        strlist_destroy(tokens2, true);
                        err_free(types2.tab);
                        err_free(types.tab);
                        strlist_destroy(tokens, true);
                        err_free(name);err_free(arg);
                        tree_destroy(fils);
                        return;
                    }
                    
                    createExpressionTreeAux(fils2, tokens2, &types2, &ordre2);
                    if (CODE_ERROR != 0)
                    {
                        strlist_destroy(tokens2, true);
                        err_free(types2.tab);
                        err_free(types.tab);
                        strlist_destroy(tokens, true);
                        err_free(name);err_free(arg);
                        tree_destroy(fils);
                        tree_destroy(fils2);
                        return;
                    }
                    tree_appendSon(fils, fils2);
                }
                strlist_destroy(tokens2, true);
                err_free(types2.tab);
                err_free(ordre2.tab);
            }

            fils->type = TYPE_BLOCKWORD1LINE;
            fils->label2 = number_fromDouble((double)(strlist_index(&blockwords1Line, name) + 1));

            err_free(name);
            err_free(arg);
            
            tree_appendSon(tree, fils);

        }
        
        else if (types.tab[index] == TYPE_FUNCTIONDEF || types.tab[index] == TYPE_METHODDEF)
        {
            Tree* fils = tree_create(NULL, number_default(), 0);
            if (CODE_ERROR != 0)
            {
                err_free(types.tab);
                strlist_destroy(tokens, true);
                return;
            }
            
            createFunctionTree(fils, tokens->tab[index], types.tab[index] == TYPE_METHODDEF);
            if (CODE_ERROR != 0)
            {
                err_free(types.tab);
                strlist_destroy(tokens, true);
                tree_destroy(fils);
                return;
            }

            tree_appendSon(tree, fils);
        }

        else if (types.tab[index] == TYPE_KEYWORD)
        {
            Tree * fils = tree_create(NULL, number_fromDouble((double)(strlist_index(&lkeywords, tokens->tab[index]) + 1)), TYPE_KEYWORD);
            if (CODE_ERROR != 0)
            {
                err_free(types.tab);
                strlist_destroy(tokens, true);
                return;
            }
            
            tree_appendSon(tree, fils);
        }

        
        if (types.tab[index] == TYPE_CONDITIONBLOCK)
        {
            Tree * fils = tree_create(NULL, number_default(), 0);
            if (CODE_ERROR != 0)
            {
                err_free(types.tab);
                strlist_destroy(tokens, true);
                return;
            }
            
            createConditionBlockTree(fils, tokens->tab[index]);
            if (CODE_ERROR != 0)
            {
                err_free(types.tab);
                strlist_destroy(tokens, true);
                tree_destroy(fils);
                return;
            }
            tree_appendSon(tree, fils);
        }
        
        if (types.tab[index] == TYPE_EXPRESSION)
        {
            Tree * fils = tree_create(NULL, number_default(), 0);
            if (CODE_ERROR != 0)
            {
                err_free(types.tab);
                strlist_destroy(tokens, true);
                return;
            }
            
            createExpressionTree(fils, tokens->tab[index]);
            if (CODE_ERROR != 0)
            {
                err_free(types.tab);
                strlist_destroy(tokens, true);
                tree_destroy(fils);
                return;
            }
            
            tree_appendSon(tree, fils);
        }
        
        if (types.tab[index] == TYPE_STATEMENTFOR || types.tab[index] == TYPE_STATEMENTWHILE)
        {
            Tree * fils = tree_create(NULL, number_default(), 0);
            if (CODE_ERROR != 0)
            {
                err_free(types.tab);
                strlist_destroy(tokens, true);
                return;
            }
            
            createStatementIEWFTree(fils, tokens->tab[index], types.tab[index]);
            if (CODE_ERROR != 0)
            {
                err_free(types.tab);
                strlist_destroy(tokens, true);
                tree_destroy(fils);
                return;
            }
            tree_appendSon(tree, fils);
        }
        
    }
    
    
    tree->type = TYPE_SYNTAXTREE;
    tree->data = NULL;
    strlist_destroy(tokens, true);
    
    err_free(types.tab);
    
    
    return ;
}

