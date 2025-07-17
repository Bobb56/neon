#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


#include "headers.h"





//creation du tableau de pointeurs generiques a liberer en cas d’erreur


extern ptrlist allocptr;



//déclaration des variables globales
extern int error;
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
extern strlist NOMS;
extern intlist TYPES;
extern intlist ADRESSES;
extern strlist VARIABLES;
extern listlist LISTES;

extern strlist NOMSBUILTINSFONC;



extern char* (*OPFONC[NBOPERATEURS])(char*,unsigned,char*,unsigned,int*);
extern char* (*BUILTINSFONC[NBBUILTINFONC])(strlist*, intlist*, int*);



extern unsigned nbTempList;





Tree* tree_create(char* label1, long double label2, long double type)
{
    Tree* newTree = err_malloc(sizeof(Tree));
    newTree->label1 = label1;
    newTree->label2 = label2;
    newTree->type = type;
    
    // création de la liste de fils
    newTree->capacity = 0;

    Tree** tmp = err_malloc(sizeof(Tree*));
    
    if (tmp == NULL)
        quitError(ERROR_REALLOC);
    
    newTree->sons=tmp;
    newTree->nbSons = 0;
    


    return newTree;
}




void tree_appendSon(Tree* tree, Tree* son)
{
  Tree** tmp;
  if (pow(2, tree->capacity)==tree->nbSons)
  {
    tree->capacity++;
    
    //réallocation
    tmp = err_malloc(pow(2, tree->capacity)*sizeof(Tree*));
    
    if (tmp == NULL)
        quitError(ERROR_REALLOC);
    
    for (unsigned i=0 ; i<tree->nbSons ; i++)
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
    if (tree->label1 != NULL)
    {
        err_free(tree->label1);
    }
    
    if (tree->nbSons > 0)
    {
        for (unsigned i=0 ; i<tree->nbSons ; i++)
        {
            tree_destroy(tree->sons[i]);
        }
    }
    err_free(tree->sons);
    err_free(tree);
    return ;
}








void tree_removeSon(Tree* tree, unsigned index)
{
  if (index >= tree->nbSons)
  {
    quitError(ERROR_OUT_OF_RANGE);
  }
  
  tree_destroy(tree->sons[index]);
  
  for (unsigned i = index ; i < tree->nbSons -1; i++)//décale tous les éléments à partir de celui à supprimer
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











char* fonctionArgs(char* fonction, char* set)
{
    unsigned i=0;
    while (fonction[i] != set[0])
        i++;
    
    i++;
    
    /*
    unsigned nbPar = 0;
    while (fonction[i] != set[1] || nbPar !=0)
    {
        if (fonction[i] == set[0])
            nbPar++;
        else if (fonction[i] == set[1])
            nbPar--;
        i++;
        
    }
    */
    
    if (fonction[i] == set[1]) // fonction sans arguments
        return NULL;
    else
        return sub(fonction, i, strlen(fonction)-1);
}




char* fonctionName(char* fonction, char* set)
{
    unsigned i=0;
    while (fonction[i] != set[0])
        i++;
    
    return sub(fonction, 0, i);
}






char* expressionFromStatement(char* statement)
{
    unsigned i=0;
    while (statement[i] != '(')
        i++;
    
    i++;
    unsigned deb = i;
    
    unsigned nbPar = 0;
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
    unsigned i=0;
    while (statement[i] != '{')
        i++;
    
    return sub(statement, i+1, strlen(statement)-1);
}








void calcPriorites(strlist* lexemes, intlist* nature, intlist* ordre)
{  
  // assignement de l'ordre des calculs
  
  *ordre=intlist_create(lexemes->len);
  for(unsigned i=0;i<ordre->len;i++)
    ordre->tab[i]=0;
  
  unsigned numero = 1;
  
  
  //prise en compte des parentheses
  unsigned fin,deb,i;
  while (strlist_count(lexemes,"(")!=0 && strlist_count(lexemes,")")!=0)
  {
    i=0;
    while (strcmp(")",lexemes->tab[i])!=0) {i++;}
    fin=i;
    while (strcmp("(",lexemes->tab[i])!=0) {i--;}
    deb=i;
    
    for (unsigned rang=1 ; rang<intlist_max(&PRIORITE)+1;rang++)
    {
      for (unsigned i=deb;i<fin;i++)
      {
        if (nature->tab[i]==TYPE_OPERATOR[0])
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
  for (unsigned rang=1 ; rang<intlist_max(&PRIORITE)+1;rang++)
  {
    for (unsigned i=0;i<lexemes->len;i++)
    {
      if (nature->tab[i]==TYPE_OPERATOR[0])
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
        
        if (types->tab[0] == TYPE_VARIABLE[0] || types->tab[0] == TYPE_BOOL[0] || types->tab[0] == TYPE_CONST[0] || types->tab[0] == TYPE_NONE[0])
        {
            
            tree->label1 = copy(tokens->tab[0]);
            tree->label2 = 0;
            tree->type = types->tab[0];

        }
        
        if (types->tab[0] == TYPE_FONCTION[0])
        {
            
            // ou alors on a un seul argument et il faut l'ajouter en tant que fils, ou alors on a deux arguments et il faut juste changer les caractéristiques de l'arbre à virgule
            
            char* args = fonctionArgs(tokens->tab[0], "()");
            
            
            if (args != NULL)
            {
                strlist tokens2;
                intlist types2;
                cut(&tokens2, &types2, args, false);
                intlist ordre2;
                calcPriorites(&tokens2, &types2, &ordre2);
                unsigned nbVirgules = intlist_inList(&types2, TYPE_VIRGULE[0]);
                
                if (nbVirgules > 0)
                {
                    createExpressionTreeAux(tree, &tokens2, &types2, &ordre2);
                    err_free(tree->label1);
                }
                else
                {
                    Tree* fils = tree_create(NULL, 0, 0);
                    createExpressionTreeAux(fils, &tokens2, &types2, &ordre2);
                    tree_appendSon(tree, fils);
                }
                
                strlist_destroy(&tokens2);
                err_free(types2.tab);
                err_free(ordre2.tab);
                err_free(args);
            }
            
            tree->label1 = fonctionName(tokens->tab[0], "()");
            tree->label2 = (long double)strlist_index(&NOMSBUILTINSFONC, tree->label1);
            tree->type = TYPE_FONCTION[0];
            
            
            
            
            
        }
        
        if (types->tab[0] == TYPE_NUMBER[0])
        {
            tree->label1 = copy(tokens->tab[0]);
            tree->label2 = strToNb(tokens->tab[0]);
            tree->type = TYPE_NUMBER[0];
        }
        
        if (types->tab[0] == TYPE_LISTINDEX[0])
        {
            char* args = fonctionArgs(tokens->tab[0], "[]");
            
            Tree* fils = tree_create(NULL, 0, 0);
            createExpressionTree(fils, args);
            tree_appendSon(tree, fils);
            tree->label1 = fonctionName(tokens->tab[0], "[]");
            tree->label2 = 0;
            tree->type = TYPE_LISTINDEX[0];
            err_free(args);
        }
        
        if (types->tab[0] == TYPE_STRING[0])
        {
            tree->label1 = traitementString(tokens->tab[0]);
            tree->label2 = (long double) strlen(tokens->tab[0] - 2);
            tree->type = TYPE_STRING[0];
        }
        
        if (types->tab[0] == TYPE_LIST[0])
        {
            char* l2 = sub(tokens->tab[0], 1, strlen(tokens->tab[0])-1);
            
            if (strcmp(l2, "")==0)
            {
                tree->nbSons = 0;
            }
            else
            {
                Tree* tree2 = tree_create(NULL, 0, 0);
                createExpressionTree(tree2, l2);
                
                if (tree2->type != TYPE_VIRGULE[0]) // ca veut dire que la liste ne contient qu'un seul element au maximum, donc que l'arbre n'est pas un arbre virgule
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
                    tree->label1 = tree2->label1;
                    tree->label2 = tree2->label2;
                }
            }
            
            err_free(l2);
            
            
            // détermination d'un nom pour la liste littérale
            char* numList=nbToStr((double)nbTempList);
            char* nom = addStr("--templist",numList);
            err_free(numList) ; nbTempList++;
            tree->label1 = nom;
            //tree->label2 = (long double)tree->nbSons;
            tree->type = TYPE_TEMPLIST[0];
            
            //verification de l'arbre
            // printf("label : %s, type : %c, nbSons : %u\n", tree->label1, tree->type, tree->nbSons);
            //printf("label fils1 : %s, type fils1 : %c, nbSons fils1 : %u\n", tree->sons[0]->label1, tree->sons[0]->type, tree->sons[0]->nbSons);
            //printf("label fils2 : %s, type fils2 : %c, nbSons fils2 : %u\n", tree->sons[1]->label1, tree->sons[1]->type, tree->sons[1]->nbSons);
            //printf("label fils3 : %s, type fils3 : %c, nbSons fils3 : %u\n", tree->sons[2]->label1, tree->sons[2]->type, tree->sons[2]->nbSons);
        }
        
        
    }
    else
    {
        
        unsigned nbVirgules = intlist_count(types, TYPE_VIRGULE[0]);
        
        if (nbVirgules > 0)
        {
            
            
            
            tree->label1 = copy(",");
            tree->type = TYPE_VIRGULE[0];
            
            Tree* arbre;
            unsigned index, len;
            strlist sous_tokens = strlist_create(0);
            intlist sous_types = intlist_create(0);
            intlist sous_ordre = intlist_create(0);
            
            
            for (unsigned j=0 ; j <= nbVirgules ; j++)// boucle qui parcourt toutes les virgules
            {
                
                if (j==nbVirgules)
                    index = tokens->len;
                else
                    index = intlist_index(types, TYPE_VIRGULE[0]);
                
                for (unsigned l=0 ; l<index ; l++)// on crée des listes qui vont contenir les éléments à gauche de la première virgule
                {
                    
                    strlist_append(&sous_tokens, copy(tokens->tab[l]));
                    intlist_append(&sous_types, types->tab[l]);
                    intlist_append(&sous_ordre, ordre->tab[l]);
                }
                
                
                arbre = tree_create(NULL, 0, 0);
                createExpressionTreeAux(arbre, &sous_tokens, &sous_types, &sous_ordre);// puis met ces éléments dans un arbre
                tree_appendSon(tree, arbre);// et on ajoute cet arbre à tree.
                
                
                strlist_destroy(&sous_tokens);//on détruit les listes intermédiaires
                err_free(sous_types.tab);
                err_free(sous_ordre.tab);
                sous_tokens = strlist_create(0);
                sous_types = intlist_create(0);
                sous_ordre = intlist_create(0);
                
                
                len = tokens->len;
                for (unsigned m=0 ; m <= index && m<len ; m++) // supprime tous les éléments à gauche de la première virgule ainsi que la virgule elle-même
                {
                    
                    strlist_remove(tokens, 0, true);
                    intlist_remove(types, 0);
                    intlist_remove(ordre, 0);
                }
                
            }
            
            strlist_destroy(&sous_tokens);
            err_free(sous_types.tab);
            err_free(sous_ordre.tab);
        }
        else
        {
            // dans le cas où il y a des opérateurs dedans
            unsigned numOp=intlist_max(ordre);
            
            if (numOp != 0)
            {
                
                unsigned index=intlist_index(ordre,numOp);
                
                unsigned placeOp = strlist_index(&OPERATEURS,tokens->tab[index]);
                
                unsigned typeOperande=OPERANDES.tab[placeOp];
                
                tree->label1 = copy(tokens->tab[index]);
                tree->label2 = (long double)placeOp;
                tree->type = TYPE_OPERATOR[0];
                
                if (typeOperande == RIGHT_LEFT)
                {
                    Tree* filsg = tree_create(NULL, 0, 0); Tree* filsd = tree_create(NULL, 0, 0);
                    // il va falloir séparer en deux l'expression, et recalculer l'arbre des expressions sous jacentes
                    
                    strlist tokensGauche = (strlist){.tab = tokens->tab, .len = index};
                    intlist typesGauche = (intlist){.tab = types->tab, .len = index};
                    intlist ordreGauche = (intlist){.tab = ordre->tab, .len = index};
                    
                    strlist tokensDroite = (strlist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                    intlist typesDroite = (intlist){.tab = types->tab+index+1, .len = types->len-index-1};
                    intlist ordreDroite = (intlist){.tab = ordre->tab+index+1, .len = ordre->len-index-1};
                    
                    
                    createExpressionTreeAux(filsg, &tokensGauche, &typesGauche, &ordreGauche);
                    createExpressionTreeAux(filsd, &tokensDroite, &typesDroite, &ordreDroite);
                    
                    tree_appendSon(tree, filsg);
                    tree_appendSon(tree, filsd);
                    
                    
                }
                
                if (typeOperande == VARLEFT) // variable à gauche comme -- ou ++
                {
                    
                    Tree* fils = tree_create(NULL, 0, 0);
                    
                    strlist ntokens = (strlist) {.tab = tokens->tab, .len = tokens->len-1};
                    intlist ntypes = (intlist) {.tab = types->tab, .len = types->len-1};
                    intlist nordre = (intlist) {.tab = ordre->tab, .len = ordre->len-1};
                    
                    
                    createExpressionTreeAux(fils, &ntokens, &ntypes, &nordre);
                    tree_appendSon(tree, fils);
                    
                    
                }
                
                if (typeOperande == VAR_RIGHT)
                {
                    Tree* filsg = tree_create(NULL, 0, 0); Tree* filsd = tree_create(NULL, 0, 0);
                    // il va falloir séparer en deux l'expression, et recalculer l'arbre des expressions sous jacentes
                    
                    strlist tokensGauche = (strlist) {.tab = (char*[1]) {copy(tokens->tab[0])}, .len = 1};
                    intlist typesGauche = (intlist) {.tab = (int[1]) {types->tab[0]}, .len = 1};
                    intlist ordreGauche = (intlist) {.tab = (int[1]) {ordre->tab[0]}, .len =1};
                    
                    
                    strlist tokensDroite = (strlist){.tab = tokens->tab+index+1, .len = tokens->len-index-1};
                    intlist typesDroite = (intlist){.tab = types->tab+index+1, .len = types->len-index-1};
                    intlist ordreDroite = (intlist){.tab = ordre->tab+index+1, .len = ordre->len-index-1};
                    
                    
                    createExpressionTreeAux(filsd, &tokensDroite, &typesDroite, &ordreDroite);
                    createExpressionTreeAux(filsg, &tokensGauche, &typesGauche, &ordreGauche);
                    
                    tree_appendSon(tree, filsg);
                    tree_appendSon(tree, filsd);
                    
                    err_free(tokensGauche.tab[0]);
                    
                }
                
                if (typeOperande == VARRIGHT || typeOperande == RIGHT)
                {
                    
                    Tree* fils = tree_create(NULL, 0, 0);
                    
                    
                    strlist ntokens = (strlist) {.tab = tokens->tab+1, .len = tokens->len-1};
                    intlist ntypes = (intlist) {.tab = types->tab+1, .len = types->len-1};
                    intlist nordre = (intlist) {.tab = ordre->tab+1, .len = ordre->len-1};
                    
                    
                    createExpressionTreeAux(fils, &ntokens, &ntypes, &nordre);
                    tree_appendSon(tree, fils);
                    
                }
            }
            
        
        }
        
    }

}























void createExpressionTree(Tree* tree, char* string)
{
    strlist tokens;
    intlist types;
    
    cut(&tokens, &types, string, false);
    
    intlist ordre;
    calcPriorites(&tokens, &types, &ordre);
    
    
    createExpressionTreeAux(tree, &tokens, &types, &ordre);
    

    strlist_destroy(&tokens);
    err_free(types.tab);
    err_free(ordre.tab);

    
    return;
    
}






void createStatementIEWFTree(Tree* tree, char* statement, int type)
{
    char* expression = expressionFromStatement(statement);
    char* block = blockFromStatement(statement);
    
    Tree* fils1 = tree_create(NULL, 0, 0);
    createExpressionTree(fils1, expression);
    
    Tree* fils2 = tree_create(NULL, 0, 0);
    createSyntaxTree(fils2, block);
    fils2->type = TYPE_BLOCK[0];
    
    tree_appendSon(tree, fils1);
    tree_appendSon(tree, fils2);
    
    err_free(expression);
    err_free(block);
    tree->type = type;
    
    
    return ;
}






void createStatementElseTree(Tree* tree, char* statement)
{
    char* block = blockFromStatement(statement);
    
    createSyntaxTree(tree, block);
    err_free(block);
    tree->type = TYPE_STATEMENTELSE[0];
    return ;
}




void cutConditionBlock(strlist* tokens, intlist* types, char* conditionblock)
{
    cut(tokens, types, conditionblock, false);
    
    char* newStr, *nom;
    
    for (unsigned i=0 ; i < tokens->len ; i++)
    {
        //tokens->tab[i] est forcement un blockline
        nom = nomBlockLine(tokens->tab[i]);
        
        newStr = addStr(tokens->tab[i], tokens->tab[i+1]);
        strlist_remove(tokens, i+1, true);intlist_remove(types, i+1);
        err_free(tokens->tab[i]);
        tokens->tab[i] = newStr;
        
        if (strcmp(nom, "if")==0)
        {
            types->tab[i] = TYPE_STATEMENTIF[0];
        }
        if (strcmp(nom, "elif")==0)
        {
            types->tab[i] = TYPE_STATEMENTELIF[0];
        }
        if (strcmp(nom, "else")==0)
        {
            types->tab[i] = TYPE_STATEMENTELSE[0];
        }
        
        err_free(nom);
        
        
    }
    
    
    
}









void createConditionBlockTree(Tree* tree, char* conditionblock)
{
    strlist tokens;
    intlist types;
    
    cutConditionBlock(&tokens, &types, conditionblock);// fonction qui découpe les conditionblock en statements, en donnant leurs types
    
    
    for (unsigned i=0 ; i<tokens.len ; i++)
    {
        if (types.tab[i] == TYPE_STATEMENTELSE[0])
        {
            Tree* fils = tree_create(NULL, 0, 0);
            createStatementElseTree(fils, tokens.tab[i]);
            tree_appendSon(tree, fils);
        }
        else
        {
            Tree* fils = tree_create(NULL, 0, 0);
            createStatementIEWFTree(fils, tokens.tab[i], types.tab[i]);
            tree_appendSon(tree, fils);
        }
    }
    
    tree->type = TYPE_CONDITIONBLOCK[0];
    
    strlist_destroy(&tokens);
    err_free(types.tab);
    
}










void createSyntaxTree(Tree* tree, char* program)
{
    // tree est un pointeur vers un Tree, dans lequel on va mettre l'arbre de syntaxe. Ce pointeur peut avoir une valeur complètement arbitraire, puisqu'il est initialisé dans la fonction.
    // program est une chaine de caractères contenant tout le programme.
    // la fonction calcule l'arbre correspondant à la chaine de caractères récursivement, à l'aide de cut.
    strlist tokens;
    intlist types;
    

    cut(&tokens, &types, program, true);
	
    
    for (unsigned index=0 ; index < tokens.len ; index++)
    {
        
        if (types.tab[index] == TYPE_CONDITIONBLOCK[0])
        {
            Tree * fils = tree_create(NULL, 0, 0);
            createConditionBlockTree(fils, tokens.tab[index]);
            tree_appendSon(tree, fils);
        }
        
        if (types.tab[index] == TYPE_EXPRESSION[0])
        {
            Tree * fils = tree_create(NULL, 0, 0);
            createExpressionTree(fils, tokens.tab[index]);
            tree_appendSon(tree, fils);
        }
        
        if (types.tab[index] == TYPE_STATEMENTFOR[0] || types.tab[index] == TYPE_STATEMENTWHILE[0])
        {
            Tree * fils = tree_create(NULL, 0, 0);
            createStatementIEWFTree(fils, tokens.tab[index], types.tab[index]);
            tree_appendSon(tree, fils);
        }
        
        
    }
    tree->type = TYPE_SYNTAXTREE[0];
    strlist_destroy(&tokens);
    
    err_free(types.tab);
    
    return ;
}

