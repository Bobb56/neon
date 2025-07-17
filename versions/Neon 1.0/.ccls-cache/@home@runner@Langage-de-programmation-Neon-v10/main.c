#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "headers.h"






/*
Bugs détectés : dans l'autre version



modifications apportées : fonction startMessage, prototype et prototype de terminal
changement du look : affichage du type, et changement des caractères affichés avant d'entrer une commande
ajout de la fonction builtins _help_
-> ajout des codes couleurs pour l'affichage
-> ajout de la variable error qui indique si on est dans le terminal ou en train d'exécuter un fichier.
-> modification de la fonction quitError

pour plus tard : faire en sorte que les fonctions builtins soient des variables qui aient leur propre type, et permettre ainsi le remplacement de fonctions builtins par des fonctions programmées par l'utilisateur.
-> lors de la création de l'arbre, lorsque l'arbre arrive sur une variable, il va chercher dans ADRESSES si elle n'existe déjà pas, et si elle existe, il va juste effectuer une copie du pointeur.
=> ainsi, les fonctions ne sont que des NeObjects qui contiennent en interne si elles sont builtins ou pas
-> si on décide de modifier une fonction builtins en en définissant une autre du même nom, alors ca va modifier dans le NeObject indiqué par NOMS et ADRESSES, et donc aussi modifier aussi le NeObject pointé par l'arbre. Il n'y aura aucun moyen de restaurer la fonction à part relancer Neon.

De cette manière, toutes les fonctions sont des NeObjects stockées dans ADRESSES, qui peuvent être modifiées. les fonctions built-ins sont chargées dans ADRESSES au début de l'exécution
la structure interne d'un NeObject fonction serait une structure {buil_ins_ou_pas, pointeur_vers_la fonction, pointeur_vers_l_aide_de_la_fonction}

=> de cette manière, les appels à des fonctions seront plus rapides, car les pointeurs de fonctions utilisés seront directement dans l'arbre (même optimisation qu'avec les variables normales d'ailleurs)

*/






//creation du tableau de pointeurs generiques a liberer en cas d’erreur


 ptrlist allocptr;



//déclaration des variables globales
int error;
strlist acceptedChars;
listlist syntax;
strlist sousop;
intlist gramm1;
strlist operateurs3;
strlist operateurs1;
strlist operateurs2;
strlist blockwords;
strlist boolean;
strlist keywords;
strlist lkeywords;
strlist constant;


strlist OPERATEURS;
intlist PRIORITE;
intlist OPERANDES;


//stockage des variables
strlist NOMS;
intlist TYPES;
intlist ADRESSES;
strlist VARIABLES;
listlist LISTES;

strlist NOMSBUILTINSFONC;



unsigned nbTempList;





//créer le tableau de fonctions
char* (*OPFONC[NBOPERATEURS])(char*,unsigned,char*,unsigned,int*)={_and,_or,_xor,_add,_mul,_sub,_div,_pow,_equal,_notEq,_infEqual,_supEqual,_inf,_sup,_affect,_addEqual,_subEqual,_mulEqual,_divEqual,_incr,_decr,_not,_mod,_eucl,_ref,_goIn,_deref, _minus};


char* (*BUILTINSFONC[NBBUILTINFONC])(strlist*, intlist*, int*)={_print_,_input_,_nbr_,_str_,_len_,_substring_,_exit_,_append_,_remove_,_insert_,_type_, _reverse_, _eval_,_help_,_clear_};








void treeToList(Tree* tree, strlist* tokens, intlist* types) // fonction qui renvoie une liste des fils de l'arbre calculés avec eval
{
    // tokens et types doivent être des listes initialisées
    
    strlist_resize(tokens, tree->nbSons, false);
    intlist_resize(types, tree->nbSons);
    
    char* str, *newStr;
    int typeRetour;
    
    for (unsigned i=0 ; i < tree->nbSons ; i++)
    {
        
        str = eval(tree->sons[i], &typeRetour);
        
        types->tab[i] = typeRetour;

        tokens->tab[i] = str;
    }
    return;
}




unsigned indexListIndex(char* listindex)
{
  unsigned i = 0;
  while (listindex[i]!='[')
  {
    i++;
  }
  char* indexstr=sub(listindex,i+1,strlen(listindex)-1);
  
  unsigned index=strToNb(indexstr);
  err_free(indexstr);
  return index;
}




unsigned typeListItem(char* item)
{
    if (item[0] == '"')
        return TYPE_STRING[0];
    else if (strcmp(item, "True")==0 || strcmp(item, "False")==0)
        return TYPE_BOOL[0];
    else
        return TYPE_NUMBER[0];
    
}




char* nameListIndex(char* listindex)
{
  unsigned i = 0;
  while (listindex[i]!='[')
  {
    i++;
  }
  return sub(listindex,0,i);
}







char* evalAux(Tree* tree, int* typeRetour, _Bool calcListIndex)
{


    // s'il n'y a qu'un seul type et que c'est un opérateur, on va calculer l'expression, des types fils à travers l'opérateur
    if (tree->type == TYPE_OPERATOR[0])
    {
        // il faut envoyer les opérandes calculées à la fonction opérateur
        if (tree->nbSons == 1) // operateur unaire
        {
            int nat1 ; char *op1 ;
            
            // récupération de la grammaire de l'opérateur
            if (OPERANDES.tab[(unsigned)tree->label2] == VARRIGHT || OPERANDES.tab[(unsigned)tree->label2] == VARLEFT)
            {
                
                op1 = evalAux(tree->sons[0], &nat1, false); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
            }
            else
            {
                
                op1 = evalAux(tree->sons[0], &nat1, true);
            }
            
            char* retour = OPFONC[(unsigned)tree->label2](op1, nat1, "1", TYPE_NUMBER[0], typeRetour);
            
            err_free(op1);
            
            return retour;
        }
        else if (tree->nbSons == 2) // operateur binaire
        {
            int nat1, nat2 ; char *op1, *op2;
            
            
            if (OPERANDES.tab[(unsigned)tree->label2] == VAR_RIGHT)
            {

                op1 = evalAux(tree->sons[0], &nat1, false); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur          
            }
            else
            {
                
                op1 = evalAux(tree->sons[0], &nat1, true);
            }
            
            op2 = evalAux(tree->sons[1], &nat2, true);

            char* retour = OPFONC[(unsigned)tree->label2](op1, nat1, op2, nat2, typeRetour);

            err_free(op1); err_free(op2);
            
            return retour;
        }
        
    }
    
    // s'il n'y a qu'un seul élément final, on le calcule (traiterOperande)
    
    if (tree->type == TYPE_NUMBER[0])
    {
        *typeRetour = TYPE_NUMBER[0];
        return copy(tree->label1);
    }
    
    
    if (tree->type == TYPE_VARIABLE[0])
    {
        if (calcListIndex)
        {
            unsigned index = strlist_index(&NOMS, tree->label1);
            *typeRetour = TYPES.tab[index];
            
            if (TYPES.tab[index] == TYPE_LIST[0])
            {
                return copy(tree->label1);
            }
            else
            {
                return copy(VARIABLES.tab[ADRESSES.tab[index]]);
            }
        }
        else
        {
            *typeRetour = TYPE_VARIABLE[0];
            return copy(tree->label1);
        }

    }
    
    if (tree->type == TYPE_TEMPLIST[0])
    {
        if (calcListIndex)
        {
            strlist tokens = strlist_create(0);
            intlist types = intlist_create(0);
            
            treeToList(tree, &tokens, &types);
			
			
            strlist_append(&NOMS, copy(tree->label1)); // ajoute le nom de liste temporaire aux NOMS
            intlist_append(&TYPES, TYPE_LIST[0]); // ajoute le type de la liste
            listlist_append(&LISTES, &tokens); // ajoute tokens aux LISTES
            intlist_append(&ADRESSES, LISTES.len -1); // ajoute aux adresses la place de tokens dans LISTES
			
			
            err_free(types.tab);
            
            *typeRetour = TYPE_LIST[0];
						
            return copy(tree->label1);
        }
        else
        {
            *typeRetour = TYPE_VARIABLE[0];
            return copy(tree->label1);
        }
    }
    
    if (tree->type == TYPE_FONCTION[0])
    {

        strlist tokens = strlist_create(0);
        intlist types = intlist_create(0);
        
        treeToList(tree, &tokens, &types);
        

        
        char* retour = BUILTINSFONC[(unsigned)tree->label2](&tokens, &types, typeRetour);
        
        strlist_destroy(&tokens);err_free(types.tab);
        
        return retour;
    }
    
    if (tree->type == TYPE_LISTINDEX[0])
    {
        // il faut au préalable calculer l'index
        int tr;
        
        char* index = evalAux(tree->sons[0], &tr ,true);

        if (calcListIndex)
        {
            unsigned index2 = strToNb(index);
            unsigned index3 = strlist_index(&NOMS, tree->label1);
            *typeRetour = typeListItem(LISTES.tab[ADRESSES.tab[index3]].tab[index2]);
            err_free(index);
            return copy(LISTES.tab[ADRESSES.tab[index3]].tab[index2]);
        }
        else
        {
            *typeRetour = TYPE_LISTINDEX[0];
            char* tempstr = addStr(tree->label1, "[");
            char* tempstr2 = addStr(tempstr, index);
            char* tempstr3 = addStr(tempstr2, "]");
            err_free(tempstr);err_free(tempstr2);err_free(index);
            
            return tempstr3;
        }

    }
    
    
    if (tree->type == TYPE_STRING[0])
    {

        *typeRetour = TYPE_STRING[0];
        char* str = copy(tree->label1);
        str[0] = '"';str[strlen(str)-1] = '"';
        return str;
    }
    
    if (tree->type == TYPE_BOOL[0])
    {
        *typeRetour = TYPE_BOOL[0];
        return copy(tree->label1);

    }
    
    if (tree->type == TYPE_NONE[0])
    {
        *typeRetour = TYPE_NONE[0];
        return copy("None");
    }

}












char* eval(Tree* tree, int* typeRetour)
{
    
    char* retour = evalAux(tree, typeRetour, true);

    return retour;
}



_Bool isTrue(Tree* tree)
{
    int tr;
    char* expr = eval(tree, &tr);
    
    if (tr == TYPE_BOOL[0])
    {
        _Bool ret = strcmp(expr, "True") == 0;
        err_free(expr);
        return ret;
    }
    else if (tr == TYPE_NUMBER[0])
    {
        _Bool ret = !(strcmp(expr, "0")==0);
        err_free(expr);
        return ret;
    }
    else
    {
        quitError("Erreur : Une expression booleenne est attendue.\n");
    }
}




void execConditionBlock(Tree* tree)
{
    
    if (isTrue(tree->sons[0]->sons[0]))
    {
        
        exec(tree->sons[0]->sons[1]);

        return ;
    }
    
    _Bool elif = false;
    for (unsigned i=1 ; i < tree->nbSons - 1 ; i++)
    {
        if (isTrue(tree->sons[i]->sons[0]))
        {
            
            exec(tree->sons[i]->sons[1]);

            elif = true;
            break;
        }
    }
    
    if (!elif && tree->sons[tree->nbSons - 1]->type == TYPE_STATEMENTELSE[0])
    {
        
        exec(tree->sons[tree->nbSons - 1]);

    }
    
    return ;
    
}















void exec(Tree* tree)
{
    for (unsigned inst=0 ; inst < tree->nbSons ; inst++)
    {
      
        if (tree->sons[inst]->type == TYPE_CONDITIONBLOCK[0])
        {
            execConditionBlock(tree->sons[inst]);
        }
        else if (tree->sons[inst]->type == TYPE_STATEMENTWHILE[0])
        {

            while (isTrue(tree->sons[inst]->sons[0]))
            {
                
                exec(tree->sons[inst]->sons[1]);

            }
        }
        
        else if (tree->sons[inst]->type == TYPE_STATEMENTFOR[0])
        {
            int type;
            char* var = tree->sons[inst]->sons[0]->sons[0]->label1; // variable à incrémenter lors de la boucle
            char* valeur = eval(tree->sons[inst]->sons[0]->sons[1], &type);//valeur de départ de la boucle
            int tr;
            
            err_free(_affect(var, TYPE_VARIABLE[0], valeur, type, &tr));
            
            unsigned index = ADRESSES.tab[strlist_index(&NOMS, var)]; // index du nom de la variable dans NOMS, TYPES, VARIABLES...
            char* maxstr = eval(tree->sons[inst]->sons[0]->sons[2], &tr); // borne supérieure des valeurs atteintes par la variable
            long double max = strToNb(maxstr);
            
            err_free(maxstr);
            
            err_free(valeur);
            
            while (strToNb(VARIABLES.tab[ADRESSES.tab[strlist_index(&NOMS, var)]]) < max) // while (var < max)
            {
                
                exec(tree->sons[inst]->sons[1]); // exécution du corps de la boucle
                err_free(_addEqual(var, TYPE_VARIABLE[0], "1", TYPE_NUMBER[0],&tr)); // incrémentation de la variable
                
            }
            
            
        }
        
        else // expression
        {
            int tr;
            err_free(eval(tree->sons[inst], &tr)); // sinon, évaluation de l'expression, en la libérant juste après

        }
        
    }
    
}










void printRes(char* res, int type)
{
    _Bool aff=0;//cette variable indique si quelque chose  été affiché durant le print et va servir pour savoir si on crée un retour à la ligne à la fin
  

    if (type == TYPE_NUMBER[0] || type == TYPE_BOOL[0] || type == TYPE_STRING[0])
    {
        // affichage du type
        printf ("\033[32m");
        printf("-type : <");
        int tr;
        strlist temp1 = (strlist) {.tab = (char*[1]) {res},.len = 1,.capacity = 2};
        intlist temp2 = (intlist) {.tab = (int[1]) {type},.len = 1,.capacity = 2};
        char* typeRep = _type_(&temp1,&temp2,&tr);

        char* typeRep2 = sub(typeRep,1,strlen(typeRep)-1);
        
        printf("%s> = ", typeRep2);

        err_free(typeRep);
        err_free(typeRep2);
        printf("\033[00m");//arret de la couleur
        printf("%s",res);
        
        aff=1;
    }


    if (type==TYPE_TEMPLIST[0] || type==TYPE_LIST[0])
    {

      printf ("\033[32m");
      printf("-type : <");

      int tr;
      strlist temp1 = (strlist) {.tab = (char*[1]) {res},.len = 1,.capacity = 2};
      intlist temp2 = (intlist) {.tab = (int[1]) {type},.len = 1,.capacity = 2};
      char* typeRep = _type_(&temp1,&temp2,&tr);
      
      char* typeRep2 = sub(typeRep,1,strlen(typeRep)-1);
    
      printf("%s> = ", typeRep2);
      printf("\033[00m");
      err_free(typeRep2);
      err_free(typeRep);
      
      unsigned indexListe=strlist_index(&NOMS, res);
      strlist list = LISTES.tab[ADRESSES.tab[indexListe]];
      if (list.len == 0)
      {
        printf("[]");
      }
      else
      {
        printf("[");
        for ( unsigned i = 0 ; i < list.len -1 ; i++)
          printf("%s, ",list.tab[i]);//affiche l'élément


        printf("%s]",list.tab[list.len-1]);//dernier élément
      }

      aff=1;
    }

    if (aff)
      printf(" ");
    
    if (aff)
      printf("\n");
}



void startMessage(void)
{
    printf(START_MESSAGE);
    printf("Neon v1.0 compile le ");
    printf(__DATE__);
    printf(" a ");
    printf(__TIME__);
    printf(".\n");
    return;
}





void terminal (void)
{
    
    char* exp, *res;Tree* tree; int typeRetour;
        
        while (true)
        {
            printf("Nombre de pointeurs alloues : %u\n",ptrlist_len(&allocptr));
            exp = input(SEQUENCE_ENTREE);

            if (exp == NULL) // si l'utilisateur n'a rien ecrit
                continue;
            
            tree = tree_create(NULL, 0, 0);

            createSyntaxTree(tree, exp);

            
            // s'il n'y a qu'une expression, alors, on affiche le résultat de l'expression
            if (tree->nbSons == 1 && tree->sons[0]->type != TYPE_CONDITIONBLOCK[0] && tree->sons[0]->type != TYPE_STATEMENTFOR[0] && tree->sons[0]->type != TYPE_STATEMENTWHILE[0])
            {
                res = eval(tree->sons[0], &typeRetour);

                printRes(res, typeRetour);
                
                err_free(res);

            }
            else if (tree->nbSons > 0)
                exec(tree);
            
            
            
            // on supprime les templist
            for (unsigned i=0 ; i<NOMS.len ; i++)
            {
                if (NOMS.tab[i][0] == '-') // c'est une templist parce que son nom commence par "--"
                {
                    listlist_remove(&LISTES, ADRESSES.tab[i]);
					// on vient de supprimer une liste de LISTES, il faut donc décaler toutes les adresses des listes situées après
					for (unsigned j=i+1 ; j < ADRESSES.len ; j++)
					{
						ADRESSES.tab[j]--;
					}
                    intlist_remove(&TYPES, i);
                    strlist_remove(&NOMS, i, true);
					intlist_remove(&ADRESSES,i);
                }
            }
            err_free(exp);
            tree_destroy(tree);
            
                        
        }


    return ;
}







void execFile(char* filename)
{
    
    FILE* fichier = fopen(filename, "rt");//lit le fichier
    if (fichier == NULL)
        quitError("Erreur lors de l'ouverture du fichier.\n");
    
    //on regarde la position actuelle de la tête de fichier

    fpos_t pos;
    fgetpos(fichier, &pos);

  
    // calcul de la longueur du programme
    unsigned longueur = 0;
    while (fgetc(fichier)!=EOF)
    {
        longueur++;
    }
    
    fsetpos(fichier, &pos);//remet la tête de fichier au début
    char* program=err_malloc(longueur+1);// crée le tableau de caractères qui va contenir le programme


    char car;
    unsigned index = 0;
    for (unsigned index = 0 ; (car=fgetc(fichier))!=EOF ; index++)
    {
        program[index] = car;
    }
    program[longueur] = '\0';
    
    fclose(fichier);
    
    // exécution du fichier
    Tree* tree = tree_create(NULL, 0, 0);
    createSyntaxTree(tree, program);
    exec(tree);
    tree_destroy(tree);
    err_free(program);
    
    return ;
}














int main (int argc, char* argv[])
{
    //définition de acceptedChars
    acceptedChars = (strlist) {
        .tab = (char*[28]) {"\"", "'", "+","*","-","/","<",">","=","%","&",":","!", ",", ";", "\n", "#", "$", "[", "]", "(", ")", "{", "}", "\\", ".", "_", " "},
        .len = 28
    };
    
    
    
    //définition de syntax
    syntax = (listlist) {
        .tab = (strlist[]) {
            (strlist) {.tab = (char*[]) {TYPE_FONCTION,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (strlist) {.tab = (char*[]) {TYPE_LISTINDEX,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (strlist) {.tab = (char*[]) {TYPE_OPERATOR,TYPE_PARENTHESE1, TYPE_NUMBER,TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_FONCTION, TYPE_BOOL, TYPE_LISTINDEX, TYPE_ENDOFLINE,  TYPE_OPERATOR, TYPE_PARENTHESE2, TYPE_CONST, TYPE_VIRGULE, TYPE_NONE} , .len = 15},//suivant la grammaire de chaque operateur
            (strlist) {.tab = (char*[]) {TYPE_STRING,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (strlist) {.tab = (char*[]) {TYPE_NUMBER,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_CROCHET} , .len = 6}, // suivant la grammaire de l’operateur
            (strlist) {.tab = (char*[]) {TYPE_LIST,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (strlist) {.tab = (char*[]) {TYPE_BLOCK,TYPE_ENDOFLINE, TYPE_BLOCKLINE, TYPE_KEYWORD} , .len = 4},
            (strlist) {.tab = (char*[]) {TYPE_BOOL,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (strlist) {.tab = (char*[]) {TYPE_CONST,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (strlist) {.tab = (char*[]) {TYPE_VARIABLE,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (strlist) {.tab = (char*[]) {TYPE_BLOCKLINE,TYPE_ENDOFLINE, TYPE_BLOCK} , .len = 3},
            (strlist) {.tab = (char*[]) {TYPE_ENDOFLINE, TYPE_VIRGULE, TYPE_NUMBER, TYPE_PARENTHESE1, TYPE_VARIABLE, TYPE_LISTINDEX, TYPE_BOOL, TYPE_OPERATOR, TYPE_LIST, TYPE_STRING, TYPE_BLOCKLINE, TYPE_BLOCK, TYPE_FONCTION, TYPE_PARENTHESE2, TYPE_KEYWORD, TYPE_CONST, TYPE_NONE} , .len = 17}, // suivant la grammaire de l’operateur
            (strlist) {.tab = (char*[]) {TYPE_PARENTHESE1,TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_VIRGULE, TYPE_PARENTHESE1} , .len = 5},
            (strlist) {.tab = (char*[]) {TYPE_PARENTHESE2,TYPE_OPERATOR, TYPE_NUMBER, TYPE_STRING, TYPE_LIST, TYPE_FONCTION, TYPE_LISTINDEX, TYPE_VARIABLE, TYPE_PARENTHESE2} , .len = 9},
            (strlist) {.tab = (char*[]) {TYPE_VIRGULE, TYPE_NUMBER,TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_FONCTION, TYPE_BOOL, TYPE_LISTINDEX, TYPE_ENDOFLINE, TYPE_OPERATOR, TYPE_CONST, TYPE_NONE} , .len = 12},
            (strlist) {.tab = (char*[]) {TYPE_KEYWORD,TYPE_ENDOFLINE, TYPE_BLOCK} , .len = 3},
            (strlist) {.tab = (char*[]) {TYPE_NONE,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            },
        .len = 17
    };
    
    
    //définition de sousop
    sousop = (strlist) {
        .tab = (char*[11]) {"+","*","-","/","<",">","=","%","&",":","!"},
        .len = 11
    };
    
    
    // liste qui contient les opérandes prises par les opérateurs, correspond a operateurs3[]
    gramm1 = (intlist) {
        .tab = (int[28]) {RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,VARRIGHT, RIGHT},
        .len = 28
    };
    
    
    //définition de operateurs3
    operateurs3 = (strlist) {
        .tab = (char*[28]) {"and","or","xor","+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","not","%","//","&","->",":", "_"},
        .len = 28
    };
    
    
    //définition de operateurs1
    // opérateurs composés de caractères de sousop
    operateurs1 = (strlist) {
        .tab = (char*[23]) {"+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","%","//","&","->",":"},
        .len = 23
    };
    
    
    // opérateurs mots
    operateurs2 = (strlist) {.tab = (char*[4]) {"and","or","xor","not"}, .len = 4};


    //mots de blocs d'instructions
    blockwords = (strlist) {.tab = (char*[4]) {"if","while","for", "elif"}, .len = 4};
    
    keywords = (strlist) {.tab = (char*[3]) {"else", "continue", "break"}, .len = 3};
    
    lkeywords = (strlist) {.tab = (char*[2]) {"continue", "break"}, .len = 2};
    
    
    boolean = (strlist) {.tab = (char*[2]) {"True","False"}, .len = 2};
    constant = (strlist) {.tab = (char*[1]) {"None"}, .len = 1};
    
    
    
    
    
    OPERATEURS=(strlist) {(char*[NBOPERATEURS]) {"and","or","xor","+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","not","%","//","&","->",":", "_"}, NBOPERATEURS};
    
    
    
    PRIORITE= (intlist) {(int[28]) {7,7,7,4,3,4,3,2,5,5,5,5,5,5,8,8,8,8,8,2,2,6,3,3,1,8,1,3}, 28};
    
    OPERANDES=(intlist) {(int[28]){RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,RIGHT, RIGHT},28};

    
    NOMSBUILTINSFONC=(strlist){(char*[NBBUILTINFONC]){"print","input","nbr","str","len","sub","exit","append","remove","insert","type", "reverse", "eval","help","clear"},NBBUILTINFONC};



    
    //stockage des variables
    NOMS=strlist_create(0);
    TYPES=intlist_create(0);
    ADRESSES=intlist_create(0);
    VARIABLES=strlist_create(0);
    LISTES=listlist_create(0);
    
    unsigned nbTempList=0; //nombre de listes temporaires, ça sert à les nommer en les créant 
    

    //error contient 0 lors de son premier passage ici
    //on lui met 1 lorsqu'elle passe par le terminal
    //on la met à deux lorsque l'on exécute un fichier
    error = 0;
    
    
    if (argc >= 2)
    {
        error = 2;
        strlist args = strlist_create(argc - 2);
        for (unsigned i=2 ; i < argc ; i++)
        {
            args.tab[i-2] = sandwich(argv[i], '"');
        }
        strlist_append(&NOMS, copy("__args__"));
        intlist_append(&TYPES, TYPE_LIST[0]);
        intlist_append(&ADRESSES, LISTES.len);
        
        listlist_append(&LISTES, &args);

        
        execFile(argv[1]);

        strlist_destroy(&VARIABLES);
        strlist_destroy(&NOMS);
        listlist_destroy(&LISTES);
        err_free(TYPES.tab);err_free(ADRESSES.tab);
        
        freeAllPtr();
        
    }
    else
    {
        startMessage();
        error=1;
        terminal();
    }
    
    
    return 0;
}
