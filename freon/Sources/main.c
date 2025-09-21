#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "headers.c"

#ifdef WASM
    #include <emscripten/emscripten.h>
    char* INPUTMESSAGE = NULL;
#endif



#ifdef TI83PCE
    #include "nio_ce.h"
    nio_console console;
#endif


/*
Bugs détectés :
_____
Avancement et choses à faire :
> vérifier les priorités de tous les opérateurs


Procédure pour ajouter une fonction built-in :
1) Programmer cette fonction selon le prototype NeObject* _maFonction_ (NeList* args)
4) Ajouter 1 à la constante pré-processeur NBBUILTINFONC définie au début de neon.h
3) Ajouter le prototype de cette fonction tout à la fin de neon.h
4) Ajouter la fonction dans le tableau de fonctions au début de ce fichier
5) Compléter les informations sur la fonction dans neonInit
*/


void* PTRERROR = NULL; // pointeur qui contient soit un pointeur d'arbre soit une chaine de caractères à propos de l'endroit où est apparue une erreur

char* EXCEPTION = NULL; // exception levée par l'utilisateur, à afficher par printError

int NAME = 0; // adresse du NeObject représentant le nom actuel de fichier. A ne pas utiliser, il y a à la place la fonction updateFileName

int CODE_ERROR = 0; // contient le code de l'erreur survenue lors d'un appel de fonction
strlist ERRORS_MESSAGES;

ptrlist* VAR_LOC; // c'est une pile de pile qui contiendra dans chaque contexte les variables à supprimer


NeObject* RETURN_VALUE = NULL; // contient la valeur de retour de la fonction si le programme a été interrompu par un return ()

strlist* CONTAINERS; // à un indice du tableau, associe le nom du type de container
NeList* ATTRIBUTES; // contient pour chaque type de container, une liste des noms de champs

Tree* FONCTIONS; // cet arbre va contenir en fils tous les arbres de fonction des fonctions et des méthodes définies par l'utilisateur

ptrlist* allocptr; // stocke tous les pointeurs alloués depuis le début
ptrlist* freeptr; // stocke tous les pointeurs libérés depuis le début

strlist* NOMS; // contient les noms des variables associés à leurs adresses mémoire dans ADRESSSES
NeList* ADRESSES;


//définition de acceptedChars
strlist acceptedChars ;

// definit les types de tokens composes et indique par quel type de token compose ils peuvent etre precedes
listlist syntax;
intlist types_debut;
intlist types_fin;

//définition de sousop
strlist sousop ;

// liste qui contient les opérandes prises par les opérateurs, correspond a operateurs3[]
intlist gramm1 ;

//définition de operateurs3
strlist operateurs3;

//définition de operateurs1
// opérateurs composés de caractères de sousop
strlist operateurs1;

// opérateurs mots
strlist operateurs2;

//mots de blocs d'instructions
strlist blockwords;
strlist blockwords1Line;
strlist keywordFunction;
strlist keywords;
strlist lkeywords;
strlist boolean;
strlist exceptions;
strlist constant;
strlist OPERATEURS;
intlist PRIORITE;
intlist OPERANDES;
intlist exceptions_err;

strlist NOMSBUILTINSFONC;





//créer le tableau de fonctions
NeObject* (*OPFONC[NBOPERATEURS])(NeObject*,NeObject*)={_and,_or,_xor,_add,_mul,_sub,_div,_pow,_equal,_notEq,_infEqual,_supEqual,_inf,_sup,_affectNone,_addEqual,_subEqual,_mulEqual,_divEqual,_incr,_decr,_not,_mod,_eucl,_ref,_goIn,_deref, _minus, _del, _affect, NULL, _exponent, _implique, _in, NULL};


NeObject* (*BUILTINSFONC[NBBUILTINFONC])(NeList*)={_print_,_input_,_nbr_,_str_,_len_,_substring_,_exit_,_append_,_remove_,_insert_,_type_, _reverse_, _eval_,_clear_,_help_, _randint_,_quitError_, _time_, _assert_, _output_, _chr_, _ord_, _list_comp_, _create_exception_, _exception_, _int_, _index_, _replace_, _count_, _list_, _sort_asc_, _sort_desc_, _sin_, _cos_, _tan_, _deg_, _rad_, _sqrt_, _ln_, _exp_, _log_, _log2_, _round_, _abs_, _ceil_, _floor_, _readFile_, _writeFile_};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////




void updateFileName(char* name)
{
    neobject_destroy(ADRESSES->tab[NAME], true);
    ADRESSES->tab[NAME] = neo_str_create(name);

    return ;
}








void treeToList(Tree* tree, NeObject* list, int nbArgsMeth) // fonction qui renvoie une liste des fils de l'arbre calculés avec eval
{
    // list doit être un NeObject provenant de la fonction neo_list_create(la_bonne_taille)
    // tokens et types doivent être des listes initialisées
    NeList* l = list->data;
    int i=0;
    
    for (; (i < nbArgsMeth && i < tree->nbSons) || (nbArgsMeth == -1 && i < tree->nbSons) ; i++)
    {
        l->tab[i] = evalAux(tree->sons[i], false);

        if (CODE_ERROR != 0)
            return;
        
        if (l->tab[i]->type == TYPE_EMPTY)
        {
            CODE_ERROR = 5;
            return ;
        }
        
    }

    for (;i < tree->nbSons ; i++)
    {
        l->tab[i] = eval(tree->sons[i]);
        
        if (CODE_ERROR != 0)
        {
            // si y a eu un problème dans l'évaluation d'un argument, on doit libérer toute la liste créée jusqu'alors
            for (int j = nbArgsMeth ; j <= i ; j++) // on ne détruit que les éléments que l'on a copiés
                neobject_destroy(l->tab[j],true);
            
            return;
        }
    }
    
    return;
}




int indexListIndex(char* listindex)
{
  int i = 0;
  while (listindex[i]!='[')
  {
    i++;
  }
  char* indexstr=sub(listindex,i+1,strlen(listindex)-1);
  
  int index=atoi(indexstr);
  err_free(indexstr);
  return index;
}









NeObject* evalAux(Tree* tree, _Bool calcListIndex)
{

    //if (!calcListIndex && tree->type != TYPE_VARIABLE && tree->type != TYPE_LISTINDEX)
    //    quitError("Vous ne pouvez rien affecter à cet objet.");
    
    // s'il n'y a qu'un seul type et que c'est un opérateur, on va calculer l'expression, des types fils à travers l'opérateur
    
    
    

    if (tree->type == TYPE_OPERATOR)
    {

        // il faut envoyer les opérandes calculées à la fonction opérateur
        if (tree->nbSons == 1) // operateur unaire
        {
            NeObject * op1;
            // récupération de la grammaire de l'opérateur
            if (OPERANDES.tab[(int)number_toDouble(tree->label2)] == VARRIGHT || OPERANDES.tab[(int)number_toDouble(tree->label2)] == VARLEFT)
            {
                PTRERROR = tree;

                op1 = evalAux(tree->sons[0], false); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
                
                if (CODE_ERROR != 0)
                    return NULL;
                
            }
            else
            {
                op1 = evalAux(tree->sons[0], true);

                if (CODE_ERROR != 0)
                    return NULL;
            }

            NeObject* un = neo_nb_create(number_fromDouble(1));
            NeObject* retour = OPFONC[(int)number_toDouble(tree->label2)](op1,un);


            if (CODE_ERROR != 0)
            {
                neobject_destroy(un, true);
                if (OPERANDES.tab[(int)number_toDouble(tree->label2)] != VARRIGHT && OPERANDES.tab[(int)number_toDouble(tree->label2)] != VARLEFT)
                    neobject_destroy(op1, true);
                
                return NULL;
            }
            
            
            if (OPERANDES.tab[(int)number_toDouble(tree->label2)] != VARRIGHT && OPERANDES.tab[(int)number_toDouble(tree->label2)] != VARLEFT)  // on ne libère op1 que si op1 n'est pas directement d'adresse d'une variable
                neobject_destroy(op1,true);

            
            neobject_destroy(un,true);
            
            
            return retour;
        }
        else if (tree->nbSons == 2) // operateur binaire
        {
            NeObject *op1, *op2;
            
            
            if (OPERANDES.tab[(int)number_toDouble(tree->label2)] == VAR_RIGHT)
            {

                PTRERROR = tree;

                op1 = evalAux(tree->sons[0], false); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
                if (CODE_ERROR != 0)
                    return NULL;
                
                op2 = evalAux(tree->sons[1], true);
            }
            else if (OPERANDES.tab[(int)number_toDouble(tree->label2)] == LEFT_VAR)
            {

                PTRERROR = tree;

                op1 = evalAux(tree->sons[0], true);
                if (CODE_ERROR != 0)
                    return NULL;
                
                op2 = evalAux(tree->sons[1], false); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
            }
            else
            {
                op1 = evalAux(tree->sons[0], true);

                if (CODE_ERROR != 0)
                {
                    neobject_destroy(op1, true);
                    return NULL;
                }

                op2 = evalAux(tree->sons[1], true);
            }


            if (CODE_ERROR != 0)
            {
                if (OPERANDES.tab[(int)number_toDouble(tree->label2)] != VAR_RIGHT)
                    neobject_destroy(op1, true);
                return NULL;
            }

            NeObject* retour = OPFONC[(long int)number_toDouble(tree->label2)](op1, op2);


            if (CODE_ERROR != 0)
            {
                if (OPERANDES.tab[(int)number_toDouble(tree->label2)] != VAR_RIGHT)
                    neobject_destroy(op1, true);
                neobject_destroy(op2, true);
                return NULL;
            }

            if (OPERANDES.tab[(int)number_toDouble(tree->label2)] != VAR_RIGHT) // on ne libère op1 que si op1 n'est pas directement d'adresse d'une variable
                neobject_destroy(op1,true);
            
            if (OPERANDES.tab[(int)number_toDouble(tree->label2)] != LEFT_VAR)
                neobject_destroy(op2,true);
            
            return retour;
        }
        
    }
    
    // s'il n'y a qu'un seul élément final, on le calcule (traiterOperande)
    
    if (tree->type == TYPE_NUMBER)
    {
        if (!calcListIndex)
        {
            CODE_ERROR = 89;
            return NULL;
        }
        else
            return neo_copy(tree->data);
    }
    
    
    if (tree->type == TYPE_VARIABLE) // référence à la fonction createExpressionTree, la feuille contient : le nom de la variable, son adresse directement dans tree->data
    {
        if (calcListIndex)
        {
            if (tree->data->type == TYPE_EMPTY)
            {
                CODE_ERROR = 5;
                return NULL;
            }
            else
                return neo_copy(tree->data);
        }
        else
        {
            return tree->data;
        }
        

    }
    
    if (tree->type == TYPE_LIST)
    {
        if (!calcListIndex) // erreur car fuite mémoire
        {
            CODE_ERROR = 89;
            return NULL;
        }

        // donc les enfants de tree sont les éléments de la liste
        NeObject* liste = neo_list_create(tree->nbSons);
        treeToList(tree,liste,0);

        if (CODE_ERROR != 0)
        {
            err_free(((NeList*)liste->data)->tab);
            err_free(liste->data);
            err_free(liste);
            
            return NULL;
        }

        
        return liste;
    }
    
    if (tree->type == TYPE_FONCTION)
    {

        PTRERROR = tree;
        
        if (tree->data->type == TYPE_FONCTION)
        {
            
            NeObject* args = neo_list_create(tree->nbSons);
            
            Function* fun = tree->data->data;
            treeToList(tree, args, fun->nbArgsMeth);

            if (CODE_ERROR != 0)
            {
                err_free(((NeList*)(args->data))->tab);
                err_free(args->data);
                err_free(args);
                
                return NULL;
            }
    
            if (!funcArgsCheck(fun, args->data))
            {
                if (fun->nbArgsMeth != -1)
                {
                    for (int i=fun->nbArgsMeth ; i < neo_list_len(args) ; i++) // on ne détruit que les éléments que l'on a copiés
                    {
                        neobject_destroy(neo_list_nth(args,i),true);
                    }
                }
    
                err_free(((NeList*)(args->data))->tab);
                err_free(args->data);
                err_free(args);
                
                CODE_ERROR = 14;
                return NULL;
            }
    
            
            NeObject* retour = functionCall(tree->data,(NeList*)args->data);
            
            if (CODE_ERROR != 0)
            {
                if (fun->nbArgsMeth != -1)
                {
                    for (int i=fun->nbArgsMeth ; i < neo_list_len(args) ; i++) // on ne détruit que les éléments que l'on a copiés
                    {
                      neobject_destroy(neo_list_nth(args,i),true);
                    }
                }
    
                err_free(((NeList*)(args->data))->tab);
                err_free(args->data);
                err_free(args);
                
                return NULL;
            }
            
            if (fun->nbArgsMeth != -1)
            {
                for (int i=fun->nbArgsMeth ; i < neo_list_len(args) ; i++) // on ne détruit que les éléments que l'on a copiés
                {
                  neobject_destroy(neo_list_nth(args,i),true);
                }
            }
    
    
            err_free(((NeList*)(args->data))->tab);
            err_free(args->data);
            err_free(args);


            return retour;
        }

        else if (tree->data->type == TYPE_USERFUNC || tree->data->type == TYPE_USERMETHOD)
        {
            
            // calcul des arguments et affectation des arguments
            NeObject* args = neo_list_create(tree->nbSons);

            UserFunc* fun = tree->data->data;

            if (tree->data->type == TYPE_USERMETHOD)
                treeToList(tree, args, 1);
            else
                treeToList(tree, args, 0);


            if (CODE_ERROR != 0)
            {
                err_free(((NeList*)(args->data))->tab);
                err_free(args->data);
                err_free(args);
                return NULL;
            }


            if (fun->nbArgs < neo_list_len(args)) // erreur
            {
                CODE_ERROR = 6;
                if (tree->data->type == TYPE_USERMETHOD)
                    ((NeList*)args->data)->tab[0] = neo_none_create();
                neobject_destroy(args, true);
                return NULL;
            }
            else if (fun->nbArgs > neo_list_len(args)) // erreur
            {
                CODE_ERROR = 7;
                if (tree->data->type == TYPE_USERMETHOD)
                    ((NeList*)args->data)->tab[0] = neo_none_create();
                neobject_destroy(args, true);
                return NULL;
            }

            // ouvre un nouveau contexte pour sauvegarder les variables locales de cet appel
            newContext();
            

            if (tree->data->type == TYPE_USERMETHOD)
            {
                // affectation de la variable de méthode
                if (fun->nbArgs > 0)
                {
                    NeObject* c = neo_copy(neo_list_nth(args, 0)); // on crée une copie de l'argument
                    // on empile la valeur actuelle de l'argument pour avoir champ libre dans cette fonction
                    
                    local(fun->args[0]);

                    fun->args[0]->data = c->data;
                    fun->args[0]->type = c->type;
                    err_free(c);
                }
                
                for (int i = 1 ; i < fun->nbArgs ; i++) // affectation des arguments
                {
                    NeObject* temp = neo_copy(neo_list_nth(args, i));
                    
                    local(fun->args[i]);
                    
                    fun->args[i]->data = temp->data;
                    fun->args[i]->type = temp->type;

                    err_free(temp);
                    
                    if (CODE_ERROR != 0) // erreur : libération des arguments affectés
                    {
                        
                        // libérer la sauvegarde des variables
                        deleteContext();
                        
                        ((NeList*)args->data)->tab[0] = neo_none_create();
                        neobject_destroy(args, true);
                        // libération des arguments
                        for (int j = 1 ; j < i ; j++)
                            neobject_destroy(fun->args[j], false);

                        return NULL;
                    }
                }
            }
            else
            {
                for (int i = 0 ; i < fun->nbArgs ; i++) // affectation des arguments
                {
                    NeObject* temp = neo_copy(neo_list_nth(args, i));
                    
                    local(fun->args[i]);
                    
                    fun->args[i]->data = temp->data;
                    fun->args[i]->type = temp->type;

                    err_free(temp);
                    
                    if (CODE_ERROR != 0) // erreur : libération des arguments affectés
                    {
                        // libération des sauvegardes de variables
                        deleteContext();
                        
                        neobject_destroy(args, true);
                        // libération des arguments
                        for (int j = 0 ; j < i ; j++)
                            neobject_destroy(fun->args[j], false);
                        
                        return NULL;
                    }
                    
                }
            }


            // exécution de la fonction
            int var = exec(fun->code);


            if (CODE_ERROR != 0) // erreur
            {
                for (int i = 0 ; i < fun->nbArgs ; i++)
                    neobject_destroy(fun->args[i], false);

                if (tree->data->type == TYPE_USERMETHOD)
                    ((NeList*)args->data)->tab[0] = neo_none_create();
                neobject_destroy(args, true);

                return NULL;
            }

            // sauvegarde des valeurs obtenues à la fin de la méthode pour que le paramètre puisse éventuellement récupérer son ancienne valeur
            if (tree->data->type == TYPE_USERMETHOD && fun->nbArgs > 0)
            {

                // sauvegarde uniquement le data, et on l'enlève de fun->args[0]
                void* s = fun->args[0]->data;
                int t = fun->args[0]->type;
                
                fun->args[0]->data = NULL;

                deleteContext(); // supprime le contexte : restaure les variables d'avant


                neobject_destroy(((NeList*)args->data)->tab[0], false);
                
                // on modifie list[0]
                ((NeList*)args->data)->tab[0]->data = s;
                ((NeList*)args->data)->tab[0]->type = t;

                ((NeList*)args->data)->tab[0] = NULL; // neobject au pif

            }
            else
                deleteContext(); // réaffecte les anciennes valeurs des variables qui ont été mises en local

            
            neobject_destroy(args, true);
            
            if (var == EXIT_RETURN)
                return RETURN_VALUE;
            else
                return neo_none_create();
            
        }

        else if (tree->data->type == TYPE_EMPTY)
        {
            CODE_ERROR = 8;
            return NULL;
        }
        else
        {
            CODE_ERROR = 9;
            return NULL;
        }
        
    }
    
    if (tree->type == TYPE_LISTINDEX)
    {
        // il faut au préalable calculer l'index
        // tree->data contient un pointeur vers la liste concernée
        NeObject* obj = evalAux(tree->sons[0],false);
        if (CODE_ERROR != 0)
            return NULL;

        
        if (obj->type != TYPE_LIST)
        {
            CODE_ERROR = 15;
            return NULL;
        }

        
        NeObject* index = evalAux(tree->sons[1] ,true);

        if (CODE_ERROR != 0)
        {
            neobject_destroy(index, true);
            return NULL;
        }

        
        if (index->type != TYPE_NUMBER)
        {
            neobject_destroy(index, true);
            CODE_ERROR = 16;
            return NULL;
        }
        
        long int index2 = (long int)number_toDouble(neo_to_nb(index));
        neobject_destroy(index,true);
        

        if (obj->type == TYPE_EMPTY)
        {
            CODE_ERROR = 17;
            return NULL;
        }
        else if (index2 >= neo_list_len(obj) || index2 < 0)
        {
            CODE_ERROR = 18;
            return NULL;
        }
        
        if (calcListIndex)
        {
            NeObject* retour = neo_copy(neo_list_nth(obj,index2));
            return retour;
        }
        else
        {
            // dans ce cas-là, on va devoir retourner directement le pointeur de l'élément
            return neo_list_nth(obj,index2); // au lieu de faire une copie, on renvoie directement le bon pointeur
        }

    }

    if (tree->type == TYPE_CONTAINER) // évalue un noeud correspondant à un container (littéral)
    {

        if (!calcListIndex)
        {
            CODE_ERROR = 81; // affectation d'un champ de container littéral : illégal car -> fuite mémoire
            return NULL;
        }

        NeList* val = nelist_create(tree->nbSons);
        

        for (int i = 0 ; i < tree->nbSons ; i++)
        {
            val->tab[i] = eval(tree->sons[i]->sons[0]);

            if (CODE_ERROR != 0)
            {
                for (int k=0 ; k < i ; k++) // libération de tous les éléments alloués jusqu'a i exclus
                {
                    neobject_destroy(val->tab[k], true);
                    return NULL;
                }
            }
        }
        NeObject* ret = neo_container_create((int)number_toDouble(tree->label2), val);
        
        return ret;
    }
    
    if (tree->type == TYPE_ATTRIBUTE)
    {
        NeObject* neo = evalAux(tree->sons[0], calcListIndex); // vérifier que le fils existe bien

        if (CODE_ERROR != 0)
            return NULL;

        if (neo->type != TYPE_CONTAINER)
        {
            // Erreur : essaie d'accéder à un champ d'une variable qui n'est pas un container
            neobject_destroy(neo, calcListIndex);
            CODE_ERROR = 80;
            return NULL;
        }


        // va chercher la valeur
        NeObject* ret;

        Container* c = (Container*)neo->data;

        if (tree->label1 == NULL) // ça veut dire qu'on sait exactement où chercher la valeur
        {
            //printf("passage optimisé\n");
            int index = (long int)number_toDouble(tree->label2);
            ret = (calcListIndex) ? neo_copy(c->data->tab[index]) : c->data->tab[index];
        }
        else
        {
            //printf("passage complexe\n");
            // il faut au préalable calculer l'index de où est la valeur dans le tableau
            NeList* list = (NeList*)(ATTRIBUTES->tab[c->type]->data);
            int index = 0;
            for (; index < list->len && strcmp(neo_to_string(list->tab[index]), tree->label1) != 0 ; index++)

            if (index == list->len - 1 && strcmp(neo_to_string(list->tab[index]), tree->label1) != 0)
            {
                CODE_ERROR = 82;
                neobject_destroy(neo, calcListIndex);
                return NULL;
            }

            tree->label2 = number_fromDouble((double)index);
            err_free(tree->label1) ; tree->label1 = NULL;
            ret = (calcListIndex) ? neo_copy(c->data->tab[index]) : c->data->tab[index];
        }

        if (calcListIndex)
        {
            neobject_destroy(neo, true);
        }
        
        return ret;
    }
    
    if (tree->type == TYPE_STRING)
    {
        if (calcListIndex)
            return neo_copy(tree->data);
        else
        {
            CODE_ERROR = 89;
            return NULL;
        }
    }
    
    if (tree->type == TYPE_BOOL)
    {
        if (calcListIndex)
            return neo_copy(tree->data);
        else
        {
            CODE_ERROR = 89;
            return NULL;
        }
    }

    if (tree->type == TYPE_EXCEPTION)
    {
        if (calcListIndex)
            return neo_copy(tree->data);
        else
        {
            CODE_ERROR = 89;
            return NULL;
        }
    }
    
    if (tree->type == TYPE_NONE)
    {
        if (calcListIndex)
            return neo_copy(tree->data);
        else
        {
            CODE_ERROR = 89;
            return NULL;
        }
    }

    if (tree->type == 0)
        return neo_none_create();
    
    //printf("type : %c\n", tree->type);

    CODE_ERROR = 19;
    return NULL;
}



void newContext(void)
{
    // création d'un nouveau contexte
    ptrlist* ctxt = ptrlist_create();
    ptrlist_append(VAR_LOC, ctxt);
}


void deleteContext(void)
{
    //suppression du contexte
    ptrlist* ctxt2 = (ptrlist*)VAR_LOC->tete;

    
        if (ctxt2->tete == NULL) // aucun élément
        {
            err_free(ctxt2);
        }
    
        else if (ctxt2->queue == NULL) // un seul élément
        {
            //-------------
            NeSave* ns = ctxt2->tete;
            neobject_destroy(ns->object, false);
            ns->object->data = ns->data;
            ns->object->type = ns->type;
            err_free(ns);
            //-------------

            err_free(ctxt2);
        }
        else // plusieurs éléments
        {
            ptrlist* temp = NULL;
            ptrlist* ptr = ctxt2;
            while (ptr->queue != NULL)
            {
                //-------------
                NeSave* ns = ptr->tete;
                neobject_destroy(ns->object, false);
                ns->object->data = ns->data;
                ns->object->type = ns->type;
                err_free(ns);
                //-------------
                
                temp = ptr->queue;
                err_free(ptr);
                ptr = temp;
            }
            //-------------
            NeSave* ns = ptr->tete;
            neobject_destroy(ns->object, false);
            ns->object->data = ns->data;
            ns->object->type = ns->type;
            err_free(ns);
            //-------------
            
            err_free(ptr);
        }

        ptrlist_remove(VAR_LOC, ctxt2, true); // enlève le pointeur

    return ;
}




NeObject* eval(Tree* tree)
{
    NeObject* retour = evalAux(tree, true);
    if (CODE_ERROR != 0)
    {
        neobject_destroy(retour, true);
        return NULL;
    }
    return retour;
}



_Bool isTrue(Tree* tree)
{
    NeObject* expr = eval(tree);

    if (CODE_ERROR != 0)
    {
        neobject_destroy(expr, true);
        return NULL;
    }
    
    if (expr->type == TYPE_BOOL)
    {
        _Bool res = neo_to_bool(expr);
        neobject_destroy(expr,true);
        return res;
    }
    else if (expr->type == TYPE_NUMBER)
    {
        _Bool res = (int)number_toDouble(neo_to_nb(expr)) != 0;
        neobject_destroy(expr,true);
        return res;
    }
    else
    {
        neobject_destroy(expr,true);
        CODE_ERROR = 20;
    }
    return false;
}




int execConditionBlock(Tree* tree)
{

    int bloc = 0;
    while (bloc < tree->nbSons)
    {
        PTRERROR = tree->sons[bloc];

        if (isTrue(tree->sons[bloc]->sons[0]))
        {
            if (CODE_ERROR != 0)
                return 0;
            
            int var = exec(tree->sons[bloc]->sons[1]);

            
            if (CODE_ERROR != 0)
                return 0;

            if (var != 0)
                return var;
            
            bloc++;
            while (bloc < tree->nbSons && tree->sons[bloc]->type != TYPE_STATEMENTIF)
                bloc++;

            if (bloc == tree->nbSons)
                return 0;
            else
                continue;
        }
        
        bloc++;

        PTRERROR = tree->sons[bloc];

        _Bool elif = false;
        while (bloc < tree->nbSons && tree->sons[bloc]->type == TYPE_STATEMENTELIF)
        {
            if (isTrue(tree->sons[bloc]->sons[0]) && !elif)
            {
                int var = exec(tree->sons[bloc]->sons[1]);

                if (CODE_ERROR != 0)
                    return 0;
                
                if (var != 0)
                    return var;
                
                elif = true;
            }
            bloc++;
        }

        PTRERROR = tree->sons[bloc];
        
        if (bloc < tree->nbSons && tree->sons[bloc]->type == TYPE_STATEMENTELSE) // s'il y a un bloc else
        {
            if (!elif) // si on a le droit d'y aller
            {
                int var = exec(tree->sons[tree->nbSons - 1]);

                if (CODE_ERROR != 0)
                    return 0;
                
                if (var != 0)
                    return var;
            }
            bloc++;
        }
    }
    
    return 0;
    
}





void local(NeObject* var)
{
    // sauvegarde de l'object actuel
    NeSave* ns = err_malloc(sizeof(NeSave));
    ns->object = var;
    ns->data = var->data;
    ns->type = var->type;

    // réinitialisation de la variable
    var->data = NULL;
    var->type = TYPE_EMPTY;

    //ajout de la sauvegarde au contexte actuel
    
    ptrlist_append((ptrlist*)(VAR_LOC->tete), (void*)ns);
}









int exec(Tree* tree)
{

    for (int inst=0 ; inst < tree->nbSons ; inst++)
    {
        PTRERROR = tree->sons[inst];

        if (tree->sons[inst]->type == TYPE_TRYEXCEPT)
        {
            Tree* tree_temp = tree->sons[inst];
            // exécution du try
            int var = exec(tree_temp->sons[0]);

            if (CODE_ERROR > 1) // exit() n'est pas considéré comme une erreur
            {
                // on va parcourir la liste des exceptions jusqu'à en trouver une qui corresponde
                _Bool bo = true;
                for (int i = 0 ; i < tree_temp->sons[1]->sons[0]->nbSons ; i++)
                {
                    PTRERROR = tree->sons[inst];

                    /*
                    PREVIOUS :
                    Tree* obj = tree_temp->sons[1]->sons[0]->sons[i];

                    if (!(obj->type == TYPE_EXCEPTION || (obj->type == TYPE_VARIABLE && ((NeObject*)obj->data)->type == TYPE_EXCEPTION)))
          
                    */


                    NeObject* obj = tree_temp->sons[1]->sons[0]->sons[i]->data;

                    if (obj->type != TYPE_EXCEPTION)
                    {
                        CODE_ERROR = 78;
                        return var;
                    }
                    else
                    {
                        int code = *((int*)tree_temp->sons[1]->sons[0]->sons[i]->data->data);
                        if (exceptions_err.tab[CODE_ERROR] == code || (CODE_ERROR == 74 && EXCEPTION != NULL && strcmp(EXCEPTION, exceptions.tab[code])==0)) // l'erreur correspond
                        {
                            if (CODE_ERROR == 74)
                                EXCEPTION = NULL;
                            
                            bo = true;
                            break;
                        }
                        else
                            bo = false;
                    }
                }

                if (bo) // exécution du except
                {

                    CODE_ERROR = 0; // du coup c'est bon on repart (la police a arrêté le programme, a vérifié ses papiers, et le programme est reparti)
                    
                    var = exec(tree_temp->sons[1]->sons[1]);

                    if (CODE_ERROR != 0)
                        return var;
                }
            }

        }

        
        
        else if (tree->sons[inst]->type == TYPE_BLOCKWORD1LINE)
        {
            if ((int)number_toDouble(tree->sons[inst]->label2) == RETURN) // action de return
            {
                if (tree->sons[inst]->nbSons > 1)
                {
                    CODE_ERROR = 21;
                    return 0;
                }
                

                RETURN_VALUE = (tree->sons[inst]->nbSons == 0) ? neo_none_create() : eval(tree->sons[inst]->sons[0]);

                if (CODE_ERROR != 0)
                    return 0;
                
                return EXIT_RETURN;
                
            }

            
            else if ((int)number_toDouble(tree->sons[inst]->label2) == IMPORT) // action de import
            {
                for (int i = 0 ; i < tree->sons[inst]->nbSons ; i++)
                {
                    NeObject* nom = eval(tree->sons[inst]->sons[i]);

                    if (CODE_ERROR != 0)
                        return 0;

                    char* nomAct = strdup(neo_to_string(ADRESSES->tab[NAME])); // pour restaurer le nom de fichier actuel

                    #ifndef TI83PCE
                        char* nomFichier = addStr(neo_to_string(nom), ".ne");

                        updateFileName(strdup(neo_to_string(nom)));

                        execFile(nomFichier);
                        err_free(nomFichier);
                    #else
                        updateFileName(strdup(nom));
                        execFile(neo_to_string(nom));
                    #endif

                    updateFileName(nomAct);

                    neobject_destroy(nom, true);

                }
            }

            if ((int)number_toDouble(tree->sons[inst]->label2) == LOCAL)
            {
                if (tree->sons[inst]->nbSons == 0) // il faut au moins un argument
                {
                    CODE_ERROR = 69;
                    return 0;
                }
                else if (VAR_LOC->tete == NULL)
                {
                    CODE_ERROR = 70;
                    return 0;
                }

                for (int i = 0 ; i < tree->sons[inst]->nbSons ; i++) // pour toutes les variables
                {
                    // va traiter la variable comme étant locale
                    local(tree->sons[inst]->sons[i]->data);
                }
                
            }
        }

        
        
        else if (tree->sons[inst]->type == TYPE_KEYWORD)
        {
            if ((int)number_toDouble(tree->sons[inst]->label2) != PASS)
                return (int)number_toDouble(tree->sons[inst]->label2);
        }

        
      
        else if (tree->sons[inst]->type == TYPE_CONDITIONBLOCK)
        {
            int var = execConditionBlock(tree->sons[inst]);

            if (CODE_ERROR != 0)
                return 0;

            
            if (var != 0) // break, continue, return...
                return var;
        }

        

        else if (tree->sons[inst]->type == TYPE_STATEMENTWHILE)
        {
            int var = 0;
            while (isTrue(tree->sons[inst]->sons[0]))
            {
                var = exec(tree->sons[inst]->sons[1]);
                
                if (CODE_ERROR != 0)
                    return 0;
                
                if (var != 0)
                    break;
                
            }
            
            if (var == EXIT_RETURN) // en sortant d'une boucle, on n'a pas de break ou continue
                return var;
        }
        
        

        else if (tree->sons[inst]->type == TYPE_STATEMENTFOR)
        {
            if (tree->sons[inst]->sons[0]->sons[0]->type != TYPE_VARIABLE)
            {
                CODE_ERROR = 22;
                return 0;
            }
            
            NeObject* var = tree->sons[inst]->sons[0]->sons[0]->data; // variable à incrémenter lors de la boucle
            NeObject* valeur = eval(tree->sons[inst]->sons[0]->sons[1]);//valeur de départ de la boucle

            
            if (CODE_ERROR != 0)
            {
                neobject_destroy(valeur, true);
                return 0;
            }
            
            if (valeur -> type != TYPE_NUMBER)
            {
                neobject_destroy(valeur, true);
                CODE_ERROR = 10;
                return 0;
            }
            
            _affect2(var, valeur);


            NeObject* tempMax = eval(tree->sons[inst]->sons[0]->sons[2]);


            if (CODE_ERROR != 0)
            {
                neobject_destroy(tempMax, true);
                neobject_destroy(valeur, true);
                return 0;
            }
            
            Number max = number_copy(neo_to_nb(tempMax)); // borne supérieure des valeurs atteintes par la variable

            neobject_destroy(tempMax,true);
            neobject_destroy(valeur,true);


            NeObject* un = neo_nb_create(number_fromDouble(1));

            int ret = 0;
            
            while (number_inf(neo_to_nb(var), max))
            {
                ret = exec(tree->sons[inst]->sons[1]);

                if (CODE_ERROR != 0)
                {
                    neobject_destroy(un, true);
                    return 0;
                }

                if (ret != 0)
                    break;
                
                neobject_destroy(_addEqual(var, un),true); // incrémentation de la variable
            }
            neobject_destroy(un,true);
            number_destroy(max);

            if (ret == EXIT_RETURN)
                return ret;
        }

        

        else if (tree->sons[inst]->type == TYPE_FUNCTIONDEF)
        {
            // définition de fonction en cours d'exécution
            int index = strlist_index(NOMS, tree->sons[inst]->label1);
            _affect2(ADRESSES->tab[index], tree->sons[inst]->data);

        }
        
        else // expression
        {
            neobject_destroy(eval(tree->sons[inst]),true); // sinon, évaluation de l'expression, en la libérant juste après
        }

        if (CODE_ERROR != 0) // ben oui sinon les erreurs ne seront pas captées au bon endroit
            return 0;
        

        
    }
    return 0;
}










void printRes(NeObject* res)
{
    if (res->type != TYPE_NONE)
    {
        printString(" ");
        setColor(GREEN);
        printString(type(res));
        setColor(WHITE);
        printString(" : ");
        neobject_aff(res);
        newLine();
    }
    return;
}





void startMessage(void)
{
    setColor(GREEN);
    printString("Bienvenue dans Freon");
    setColor(WHITE);
    newLine();
    
    printString("Freon v");
    printString(VERSION);
    printString(" construit à partir de Neon v");
    printString(BASE_VERSION);
    
    /*
    printString(" compilé le ");
    printString(__TIMESTAMP__);
    printString(".");
    */
    newLine();
    
    printString("Plus d'informations sur ");
    setColor(GREEN); printString("https://www.langage-neon.org/freon"); setColor(WHITE);
    newLine();

    #ifdef EXPERIMENTAL
        newLine();
        setColor(RED);
        printString("/!\\ THIS IS AN EXPERIMENTAL VERSION. IT MAY NOT WORK. /!\\");
        newLine();newLine();
        setColor(WHITE);
    #endif

    return;
}






#ifdef WASM
    void EMSCRIPTEN_KEEPALIVE execCode(char* code)
    {
        
    	if (strcmp(code,"")==0) // si l'utilisateur n'a rien ecrit
    	    return;
    
    	Tree* tree = tree_create(NULL, 0, 0);
    	createSyntaxTree(tree, code);
        if (CODE_ERROR != 0)
        {
            printError(CODE_ERROR);
            CODE_ERROR = 0;
            tree_destroy(tree);
            return;
        }
    	// s'il n'y a qu'une expression, alors, on affiche le résultat de l'expression
    	if (tree->nbSons == 1 && tree->sons[0]->type != TYPE_TRYEXCEPT && tree->sons[0]->type != TYPE_CONDITIONBLOCK && tree->sons[0]->type != TYPE_STATEMENTFOR && tree->sons[0]->type != TYPE_STATEMENTWHILE && tree->sons[0]->type != TYPE_KEYWORD && tree->sons[0]->type != TYPE_FUNCTIONDEF && tree->sons[0]->type != TYPE_BLOCKWORD1LINE)
        {
    		NeObject* res = eval(tree->sons[0]);
            if (CODE_ERROR > 1)
                printError(CODE_ERROR);
            else if (CODE_ERROR == 1)
                neobject_destroy(res, true);
            else
            {
                printRes(res);
                neobject_destroy(res,true);
            }
    	}
    	else if (tree->nbSons > 0)
    	{
    		exec(tree);
            if (CODE_ERROR > 1)
                printError(CODE_ERROR);
    	}
        CODE_ERROR = 0;
    	tree_destroy(tree);
        return ;
    }




    char* EMSCRIPTEN_KEEPALIVE inputMessage(void)
    {
        return INPUTMESSAGE;
    }

#endif







void terminal (void)
{
    
    
    char* exp;
    NeObject* res ; Tree* tree;
        
        while (true)
        {

            // printf("Nombre de pointeurs alloués : %d\n", ptrlist_len(&allocptr));
            // printf("pointeurs alloués : ");ptrlist_aff(&allocptr);

            CODE_ERROR = 0; // réinitialise les erreurs
            PTRERROR = NULL; // réinitialise le pointeur d'erreur

            exp = inputCode(SEQUENCE_ENTREE);
            
            if (CODE_ERROR > 1)
            {
                printError(CODE_ERROR);
                CODE_ERROR = 0;
                continue;
            }
            else if (CODE_ERROR == 1)
            {
                return;
            }

            
            if (strcmp(exp,"")==0) // si l'utilisateur n'a rien ecrit
            {
                err_free(exp);
                continue;
            }

            
            
            tree = tree_create(NULL, number_default(), 0);

            if (CODE_ERROR != 0)
            {
                printError(CODE_ERROR);
                err_free(exp);
                continue;
            }

            
            createSyntaxTree(tree, exp);
            err_free(exp);

            if (CODE_ERROR > 1)
            {
                printError(CODE_ERROR);
                tree_destroy(tree);
                continue;
            }
            
            // s'il n'y a qu'une expression, alors, on affiche le résultat de l'expression
            if (tree->nbSons == 1 && tree->sons[0]->type != TYPE_TRYEXCEPT && tree->sons[0]->type != TYPE_CONDITIONBLOCK && tree->sons[0]->type != TYPE_STATEMENTFOR && tree->sons[0]->type != TYPE_STATEMENTWHILE && tree->sons[0]->type != TYPE_KEYWORD && tree->sons[0]->type != TYPE_FUNCTIONDEF && tree->sons[0]->type != TYPE_BLOCKWORD1LINE)
            {
                
                res = eval(tree->sons[0]);


                if (CODE_ERROR > 1)
                {
                    if (PTRERROR == NULL)
                        PTRERROR = tree;

                    printError(CODE_ERROR);
                    tree_destroy(tree);
                    continue;
                }
                else if (CODE_ERROR == 1) // quitte le terminal
                {
                    tree_destroy(tree);
                    return ;
                }
                
                printRes(res);
                neobject_destroy(res,true);
            }
            else if (tree->nbSons > 0)
            {
                exec(tree);
                if (CODE_ERROR > 1)
                {
                    printError(CODE_ERROR);
                    tree_destroy(tree);
                    continue;
                }
                else if (CODE_ERROR == 1) // quitte le terminal
                {
                    tree_destroy(tree);
                    return ;
                }
            }
            tree_destroy(tree);
            
            /*
            strlist* tokens = strlist_create(0);
            intlist types = intlist_create(0);
            cut(tokens, &types, exp, true);
            strlist_aff(tokens);
            intlist_aff(&types);
            strlist_destroy(tokens, true);
            err_free(types.tab);
            */
            
        }


    return ;
}







void execFile(char* filename)
{
    
    char* program = openFile(filename); // fonction dépendant du système cible
    
    if (CODE_ERROR != 0)
        printError(CODE_ERROR);
    CODE_ERROR = 0;

    if (program == NULL)
        return ;
    
    // exécution du fichier
    Tree* tree = tree_create(NULL, 0, 0);
    createSyntaxTree(tree, program);
    err_free(program);

    if (CODE_ERROR != 0)
    {
        printError(CODE_ERROR);
        CODE_ERROR=0;
        tree_destroy(tree);
        return;
    }

    
    exec(tree);
    
    if (CODE_ERROR > 1)
    {
        printError(CODE_ERROR);
    }

    tree_destroy(tree);
    
    CODE_ERROR=0;
    return ;
}




void defineVariables(void)
{

    // valeurs spéciales mathématiques
    #ifndef TI83PCE
        strlist_append(NOMS, strdup("Infini"));
        nelist_append(ADRESSES, neo_nb_create(number_fromDouble(INFINITY)));
    
        strlist_append(NOMS, strdup("PasUnNombre"));
        nelist_append(ADRESSES, neo_nb_create(number_fromDouble(NAN)));
    #endif


    // définition de la variable de version
    strlist_append(NOMS, strdup("__version__"));
    nelist_append(ADRESSES, neo_str_create(strdup(VERSION)));

    // définition de la variable de plateforme
    #ifdef LINUX
        char* p = "linux";
    #endif
    #ifdef WINDOWS11
        char* p = "windows11";
    #endif
    #ifdef WINDOWS10
        char* p = "windows10";
    #endif
    #ifdef TI83PCE
        char* p = "ti83pce";
    #endif
    #ifdef WASM
        char* p = "wasm";
    #endif

    strlist_append(NOMS, strdup("__plateforme__"));
    nelist_append(ADRESSES, neo_str_create(strdup(p)));

    strlist_append(NOMS, strdup("__nom__"));
    nelist_append(ADRESSES, NULL);

    NAME = ADRESSES->len - 1; // l'adresse de __nom__ à modifier


    // pi
    nelist_append(ADRESSES, neo_nb_create(number_fromDouble(PI)));
    strlist_append(NOMS, strdup("pi"));


}












void neonInit(void)
{

    //définition de acceptedChars
    const char* acceptedChars_tab[31] = {"\"", "'", "+","*","-","/","<",">","=","%","&","@","!", ",", ";", "\n", "#", "$", "[", "]", "(", ")", "{", "}", "\\", ".", "_", " ", "\t", ".", ":"};
    strlist_copy(&acceptedChars, acceptedChars_tab, 31);





    // definit les types de tokens composes et indique par quel type de token compose ils peuvent etre precedes

    // les tableaux à l'intérieur ne sont pas des const, car sinon ça pose un problème avec le type strlist. Ce n'est pas grave, j'enlèverais ce tableau bientôt pour mettre quelque chose de moins "lourd"
    const intlist syntax_tab[20] = {
            (intlist) {.tab = (int[]) {TYPE_FONCTION,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_KEYWORDFUNCTION} , .len = 6}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_LISTINDEX,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_OPERATOR,TYPE_PARENTHESE1, TYPE_NUMBER,TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_FONCTION, TYPE_BOOL, TYPE_LISTINDEX, TYPE_ENDOFLINE,  TYPE_OPERATOR, TYPE_PARENTHESE2, TYPE_CONST, TYPE_VIRGULE, TYPE_NONE, TYPE_EXCEPTION} , .len = 16},//suivant la grammaire de chaque operateur
            (intlist) {.tab = (int[]) {TYPE_STRING,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_NUMBER,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_CROCHET} , .len = 6}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_LIST,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_BLOCK,TYPE_ENDOFLINE, TYPE_BLOCKLINE, TYPE_KEYWORD, TYPE_FONCTION} , .len = 5},
            (intlist) {.tab = (int[]) {TYPE_BOOL,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE,TYPE_PARENTHESE2} , .len = 6}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_CONST,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_VARIABLE,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_BLOCKLINE,TYPE_ENDOFLINE, TYPE_BLOCK} , .len = 3},
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_VIRGULE, TYPE_NUMBER, TYPE_PARENTHESE1, TYPE_VARIABLE, TYPE_LISTINDEX, TYPE_BOOL, TYPE_OPERATOR, TYPE_LIST, TYPE_STRING, TYPE_BLOCKLINE, TYPE_BLOCK, TYPE_FONCTION, TYPE_PARENTHESE2, TYPE_KEYWORD, TYPE_CONST, TYPE_NONE, TYPE_BLOCKWORD1LINE, TYPE_EXCEPTION} , .len = 19}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_VIRGULE, TYPE_PARENTHESE1} , .len = 5},
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE2,TYPE_OPERATOR, TYPE_NUMBER, TYPE_STRING, TYPE_LIST, TYPE_FONCTION, TYPE_LISTINDEX, TYPE_VARIABLE, TYPE_PARENTHESE2,TYPE_BOOL} , .len = 10},
            (intlist) {.tab = (int[]) {TYPE_VIRGULE, TYPE_NUMBER,TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_FONCTION, TYPE_BOOL, TYPE_LISTINDEX, TYPE_ENDOFLINE, TYPE_OPERATOR, TYPE_CONST, TYPE_NONE, TYPE_EXCEPTION} , .len = 13},
            (intlist) {.tab = (int[]) {TYPE_KEYWORD,TYPE_ENDOFLINE, TYPE_BLOCK} , .len = 3},
            (intlist) {.tab = (int[]) {TYPE_NONE,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_BLOCKWORD1LINE, TYPE_ENDOFLINE}, .len = 2},
            (intlist) {.tab = (int[]) {TYPE_KEYWORDFUNCTION, TYPE_ENDOFLINE}, .len = 2},
            (intlist) {.tab = (int[]) {TYPE_EXCEPTION,TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE}, .len = 5}
        };


    listlist_copy(&syntax, syntax_tab, 20);

    // types acceptés en début d'entrée

    
    int types_debut_temp[21] = {TYPE_FONCTION, TYPE_PARENTHESE1, TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_LISTINDEX, TYPE_NUMBER, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_BOOL, TYPE_BLOCKLINE, TYPE_KEYWORD, TYPE_CONST, TYPE_WORD, TYPE_COMMENT, TYPE_CROCHET, TYPE_BLOCKWORD, TYPE_BLOCKWORD1LINE, TYPE_EXPRESSION, TYPE_NONE, TYPE_EXCEPTION};
    intlist_copy(&types_debut, types_debut_temp, 21);

    int types_fin_temp[20] = {TYPE_FONCTION, TYPE_PARENTHESE2, TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_LISTINDEX, TYPE_NUMBER, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_BOOL, TYPE_KEYWORD, TYPE_CONST, TYPE_WORD, TYPE_COMMENT, TYPE_CROCHET, TYPE_EXPRESSION, TYPE_NONE, TYPE_EXCEPTION, TYPE_BLOCK, TYPE_BLOCKWORD1LINE};
    intlist_copy(&types_fin, types_fin_temp, 20);


    //définition de sousop
    const char* sousop_temp[] = {"+","*","-","/","<",">","=","%","&","@","!", ".",":"};
    strlist_copy(&sousop, sousop_temp, 13);


    // liste qui contient les opérandes prises par les opérateurs, correspond a operateurs3
    int gramm1_temp[NBOPERATEURS] = {RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,RIGHT, RIGHT, VARRIGHT, LEFT_VAR, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT};
    intlist_copy(&gramm1, gramm1_temp, NBOPERATEURS);



    //définition de operateurs3
    const char* operateurs3_temp[NBOPERATEURS] = {"et","ou","oux","+","*","-","/","**","=","!=","<=",">=","<",">","<-","+=","-=","*=","/=","++","--","non","mod","//","&",":=","@", "_", "suppr", "->", ".", "EE", "=>", "dans", ">>", ":"};
    strlist_copy(&operateurs3, operateurs3_temp, NBOPERATEURS);


    //définition de operateurs1
    // opérateurs composés de caractères de sousop
    const char* operateurs1_temp[] = {"+","*","-","/","**","=","!=","<=",">=","<",">","<-","+=","-=","*=","/=","++","--","//","&",":=","@", "->", ".", "=>", ">>", ":"};
    strlist_copy(&operateurs1, operateurs1_temp, 27);


    // opérateurs mots
    const char* operateurs2_temp[] = {"et","ou","oux","non", "suppr", "EE", "dans", "mod"};
    strlist_copy(&operateurs2, operateurs2_temp, 8);



    const char* OPERATEURS_temp[NBOPERATEURS] = {"et","ou","oux","+","*","-","/","^","=","!=","<=",">=","<",">","<-","+=","-=","*=","/=","++","--","non","mod","//","&",":=","@", "_", "suppr", "->", ".", "EE", "=>", "in", ">>", ":"};
    strlist_copy(&OPERATEURS, OPERATEURS_temp, NBOPERATEURS);


    const int PRIORITE_temp[NBOPERATEURS] = {8, 8, 8, 5, 4, 5, 4, 3, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 9, 3, 3, 7, 4, 4, 2, 9, 2, 4, 9, 9, 2, 4, 8, 6, 1, 9};
    intlist_copy(&PRIORITE, PRIORITE_temp, NBOPERATEURS);

    const int OPERANDES_temp[NBOPERATEURS] = {RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,RIGHT, RIGHT, VARRIGHT, LEFT_VAR, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT};
    intlist_copy(&OPERANDES, OPERANDES_temp, NBOPERATEURS);


    // ------------------

    // défnit les tokens de délimitation de blocks

    //mots de blocs d'instructions
    const char* blockwords_temp[] = {"Si","TantQue","Pour", "ei", "Renvoyer", "Importer", "Locale", "tr", "expt"};
    strlist_copy(&blockwords, blockwords_temp, 9);

    const char* blockwords1Line_temp[] = {"Renvoyer", "Importer", "Locale"};
    strlist_copy(&blockwords1Line, blockwords1Line_temp, 3);

    const char* keywordFunction_temp[] = {"Fonction", "Methode"};
    strlist_copy(&keywordFunction, keywordFunction_temp, 2);

    const char* keywords_temp[] = {"es", "Continuer", "Sortir", "NeRienFaire", "tr"};
    strlist_copy(&keywords, keywords_temp, 5);

    const char* lkeywords_temp[] = {"Continuer", "Sortir", "NeRienFaire"};
    strlist_copy(&lkeywords, lkeywords_temp, 3);


    const char* boolean_temp[] = {"Vrai","Faux"};
    strlist_copy(&boolean, boolean_temp, 2);

    const char* exceptions_temp[] = {
        "ErreurDeSyntaxe",
        "FichierInexistant",
        "ObjetNonMesurable",
        "VariableNonDefinie",
        "AppelDeFonctionIncorrect",
        "ErreurMemoire",
        "ObjetNonIndexable",
        "IndexDeListeIncorrect",
        "HorsLimites",
        "TypeIncorrect",
        "DivisionParZero",
        "ErreurInconnue",
        "AssertionEchouee",
        "ErreurDeDefinition"
    };
    // ici il faut s'assurer que capacity a la bonne valeur
    exceptions.tab = err_malloc(sizeof(char*)*16);
    exceptions.len = 14;
    exceptions.capacity = 4;
    for (int i = 0 ; i < 14 ; i++)
        exceptions.tab[i] = strdup(exceptions_temp[i]);
    

    const char* constant_temp[] = {"Rien"};
    strlist_copy(&constant, constant_temp, 1);




    /*----- Préparation des fonctions ------*/
    const char* NOMSBUILTINSFONC_temp[NBBUILTINFONC] = {"Afficher","Demander","nbr","chaine","longueur","souschaine","Quitter","Ajouter","Enlever","Inserer","type", "retourner", "evaluer","Effacer","aide", "entAleat", "QuitterAvec", "temps", "Verifier", "Ecrire", "car", "ascii", "compDeListe", "CreerErreur", "Erreur", "ent", "indice", "remplacer", "compter", "lister", "Trier_croiss", "Trier_decroiss", "sin", "cos", "tan", "deg", "rad", "racine", "ln", "exp", "log", "log2", "arrondir", "abs", "partEntSup", "partEntInf", "lireFichier", "EcrireFichier"};
    strlist_copy(&NOMSBUILTINSFONC, NOMSBUILTINSFONC_temp, NBBUILTINFONC);

    const int typesRetour[NBBUILTINFONC] = {TYPE_NONE, TYPE_STRING, TYPE_NUMBER, TYPE_STRING, TYPE_NUMBER, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_STRING, TYPE_STRING, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NUMBER, TYPE_NONE, TYPE_NUMBER, TYPE_NONE, TYPE_NONE, TYPE_STRING, TYPE_NUMBER, TYPE_LIST, TYPE_EXCEPTION, TYPE_NONE, TYPE_NUMBER, TYPE_NUMBER, TYPE_STRING, TYPE_NUMBER, TYPE_LIST, TYPE_NONE, TYPE_NONE, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_STRING, TYPE_NONE};

    const char* helpbuiltinsfonc[NBBUILTINFONC] = {
        "Affiche les arguments dans le terminal",
        "Affiche l'argument donné et invite l'utilisateur à entrer une valeur dans le terminal. Renvoie la valeur saisie.",
        "Convertit une chaîne de caractères en un nombre.",
        "Convertit un nombre en une chaîne de caractères.",
        "Renvoie la longueur d'un objet.",
        "Effectue une extraction de sous-chaîne. sub(\"hello\", 1, 4) renvoie \"onj\".",
        "Quitte l'interpréteur Neon.",
        "Ajoute un élément à une liste.",
        "Supprime un élément d'une liste.",
        "Insère un élément dans une liste.",
        "Renvoie le type d'un objet.",
        "Inverse les caractères d'une chaîne de caractères.",
        "Évalue la chaîne de caractères donnée et renvoie le résultat.",
        "Efface le terminal.",
        "Affiche des informations d'aide relatives à une fonction.",
        "Renvoie un nombre entier aléatoire entre 0 et <argument>, exclus.",
        "Affiche le texte fourni dans le terminal et quitte le programme.",
        "Renvoie le temps écoulé depuis 1970 en secondes.",
        "Arrête le programme si au moins 1 assertion a échoué.",
        "Affiche les arguments donnés dans le terminal sans espaces ni \\n.",
        "Renvoie le caractère ASCII associé au nombre.",
        "Renvoie la valeur ASCII du caractère donné.",
        "Arguments : index, start, end, step, condition, value\nÉvalue value pour tous les indices entre start et end selon step, et l'ajoute à la liste si condition est vraie.\nindex doit être le nom de la variable utilisée, et condition et value doivent être des expressions de chaîne de caractères.",
        "Crée un objet Erreur avec le nom donné.",
        "Quitte le programme avec l'erreur donnée.",
        "Prend la partie entière d'un nombre",
        "Renvoie l'indice d'un élément dans une liste.",
        "remplace(chn, chn1, chn2) renvoie une chaine de caractères dans laquelle toutes les occurences de chn1 ont été remplacées par chn2 dans chn.",
        "Compte le nombre d'occurences dans une liste ou une chaine de caractères.",
        "Sépare tous les caractères dans une chaine de caractères.",
        "Trie une liste par ordre croissant.",
        "Trie une liste par ordre décroissant.",
        "sin: Fonction sinus",
        "cos: Fonction cosinus",
        "tan: Fonction tangente",
        "deg: Convertit un angle en radians en degrés",
        "rad: Convertit un angle en degrés en radians",
        "racine: Fonction racine carrée",
        "ln: Logarithme népérien",
        "exp: Fonction exponentielle",
        "log: Logarithme (base 10)",
        "log2: Logarithme (base 2)",
        "arrondi: Arrondis à la décimale la plus proche, et avec la précision donnée",
        "abs: Valeur absolue",
        "partEntSup: Partie entière supérieure",
        "partEntInf: Partie entière inférieure",
        "Renvoie le contenu du fichier donné",
        "Ecrit la chaine de caractère donnée dans le fichier donné. La syntaxe est EcritFichier(nom, contenu)"
    };


    //indique le nombre de premiers arguments à ne pas copier
    const int typesbuiltinsfonc[NBBUILTINFONC] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // le nombre d'arguments "méthode"


    const int nbargs[NBBUILTINFONC] = {-1,-1,1,1,1,3,0,2,2,3,1,1,1,0,1,2,1,0,-1,-1,1,1,6,1,1,1,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,2}; // nombre d'arguments en tout

    // les fonctions ayant un nombre illimité d'arguments ne doivent avoir qu'un seul élément dans typeArgs
    const int* typesArgs[NBBUILTINFONC] = {
        (int[]){-1},
        (int[]){-1},
        (int[]){TYPE_STRING},
        (int[]){-1},
        (int[]){-1},
        (int[]){TYPE_STRING, TYPE_NUMBER, TYPE_NUMBER},
        NULL,
        (int[]){TYPE_LIST, -1},
        (int[]){TYPE_LIST, TYPE_NUMBER},
        (int[]){TYPE_LIST, -1, TYPE_NUMBER},
        (int[]){-1},
        (int[]){TYPE_STRING},
        (int[]){TYPE_STRING},
        NULL,
        (int[]){TYPE_FONCTION},
        (int[]){TYPE_NUMBER, TYPE_NUMBER},
        (int[]){TYPE_STRING},
        NULL,
        (int[]){TYPE_BOOL},
        (int[]){-1},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_STRING},
        (int[]){TYPE_STRING, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_STRING, TYPE_STRING},
        (int[]){TYPE_STRING},
        (int[]){TYPE_EXCEPTION},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_LIST, -1},
        (int[]){TYPE_STRING, TYPE_STRING, TYPE_STRING},
        (int[]){-1, -1},
        (int[]){TYPE_STRING},
        (int[]){TYPE_LIST},
        (int[]){TYPE_LIST},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER, TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_STRING},
        (int[]){TYPE_STRING, TYPE_STRING}
    };

    /* ----- fin préparation des fonctions ------*/


    // tableau des messages d'erreur :

    const char* ERRORS_MESSAGES_temp[NB_ERRORS] = {
        "", // Ne doit pas être utilisé
        "", // Ne doit pas être utilisé
        "Multiples points décimaux dans le même nombre",
        "Le fichier n'existe pas",
        "Cet objet n'a pas de longueur",
        "Utilisation d'une variable non définie",
        "Trop d'arguments dans l'appel de fonction définie par l'utilisateur",
        "Nombre insuffisant d'arguments dans l'appel de fonction définie par l'utilisateur",
        "Appel d'une fonction non définie",
        "Appel à une fonction invalide",
        "La valeur de départ de la boucle for n'est pas un nombre",
        "Définition incorrecte de fonction",
        "Erreur de mémoire",
        "Index incorrect pour la suppression d'un sous-arbre",
        "Arguments invalides fournis dans l'appel de fonction",
        "Cet objet n'est pas indexable",
        "L'index de la liste n'est pas un nombre",
        "Tentative d'indexer une liste non définie",
        "Index de liste hors de portée",
        "Erreur indéterminée, probablement un problème de syntaxe",
        "Condition non évaluable en raison d'un type incorrect",
        "Arguments multiples pour l'instruction return",
        "La variable de la boucle for n'est pas une variable",
        "Nom incorrect de fonction intégrée dans l'appel de fonction",
        "Opérateur inconnu",
        "Caractère inconnu",
        "Chaîne, liste ou autre structure non terminée",
        "Chaîne non fermée à la fin de la ligne",
        "Mot-clé incompatible utilisé dans une expression",
        "Ligne d'instruction de bloc non suivie par un bloc d'instruction (par exemple, une simple déclaration Si())",
        "Utilisation incorrecte d'un opérateur; types incompatibles utilisés consécutivement ou un type inconnu/incompatible",
        "Utilisation incorrecte d'un opérateur; types incompatibles utilisés consécutivement ou un type inconnu/incompatible",
        "Utilisation incorrecte d'un opérateur; types incompatibles utilisés consécutivement ou un type inconnu/incompatible",
        "Utilisation incorrecte d'un opérateur; types incompatibles utilisés consécutivement ou un type inconnu/incompatible",
        "Bloc unique d'instructions",
        "Arrangement incorrect des blocs Si/SinonSi/Sinon",
        "Nombre incorrect d'arguments lors de l'appel d'une fonction intégrée",
        "Index hors de portée pour l'insertion dans une NeList",
        "Index hors de portée pour la suppression d'une NeList",
        "Index hors de portée pour l'accès à une NeList",
        "Types non pris en charge pour l'addition",
        "Types non pris en charge pour la soustraction",
        "Types non pris en charge pour la division",
        "Types non pris en charge pour la multiplication",
        "Types non pris en charge pour le modulo",
        "Division euclidienne par zéro",
        "Types non pris en charge pour la division euclidienne",
        "Types non pris en charge pour l'opérateur 'et'",
        "Types non pris en charge pour l'opérateur 'ou'",
        "Types non pris en charge pour l'opérateur 'ou'",
        "Types non pris en charge pour l'exponentiation",
        "Types non pris en charge pour les tests d'égalité",
        "Types non pris en charge pour <=",
        "Types non pris en charge pour >=",
        "Types non pris en charge pour <",
        "Types non pris en charge pour >",
        "Tentative d'assignation de la valeur d'une variable non définie à une autre variable",
        "Utilisation incorrecte de l'opérateur '->'",
        "Types non pris en charge pour l'opérateur 'non'",
        "Référence à une variable non définie",
        "Déréférencement d'autre chose qu'une chaîne de caractères",
        "Types non pris en charge pour l'opérateur unaire moins",
        "Types non pris en charge pour la fonction reverse",
        "Tentative d'utilisation de l'aide sur une variable non fonctionnelle",
        "La fonction entAleat nécessite un argument entier positif",
        "Le pointeur n'est pas alloué",
        "Erreur indéfinie",
        "Impossible d'ouvrir le fichier",
        "Plus de parenthèses fermantes, d'accolades ou de crochets que d'ouvrants",
        "locale doit avoir au moins un argument",
        "locale ne peut être utilisé que dans une fonction ou une méthode",
        "Échec de l'assertion",
        "L'opérateur '.' est réservé aux fonctions ou méthodes et ne peut être appliqué qu'à une variable",
        "Nombre hexadécimal ou binaire incorrect",
        "", // code 74 spécial pour les appels d'exceptions
        "Types non pris en charge pour l'implication.",
        "Bloc except non précédé par un bloc try.",
        "Bloc Essayer non suivi d'un bloc SiErreur",
        "SiErreur attend des exceptions",
        "dans ne peut être calculé qu'avec une liste",
        "'>>' ne doit etre utilise que pour les conteneurs",
        "Le champ du conteneur a ete affecte alors que le conteneur n'est pas utilise",
        "Ce champ de conteneur n'existe pas",
        "Deux conteneurs ont le meme nom mais pas les memes champs",
        "Un conteneur doit contenir au moins un champ",
        "Type de conteneur inconnu",
        "Un conteneur ne peut pas contenir deux champs differents possedant le meme nom",
        "Conteneur defini de maniere incorrecte",
        "L'objet l'est pas dans la liste",
        "Impossible d'executer une methode sur un objet litteral",
        "Cette liste ne peut pas etre triee",
        "Impossible d'appeler une fonction pendant le déréférencement d'un champ de conteneur",
        "Mauvaise utilisation de ':'"
    };

    strlist_copy(&ERRORS_MESSAGES, ERRORS_MESSAGES_temp, NB_ERRORS);

    const int exceptions_err_temp[NB_ERRORS] = {
        0,// ne doit pas être utilisé
        0,// ne doit pas être utilisé
        0,
        1,
        2,
        3,
        4,
        4,
        3,
        4,
        0,
        0,
        5,
        11,
        4,
        6,
        7,
        3,
        8,
        0,
        9,
        0,
        9,
        4,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        4,
        8,
        8,
        8,
        9,
        9,
        9,
        9,
        9,
        10,
        9,
        9,
        9,
        9,
        9,
        9,
        9,
        9,
        9,
        9,
        3,
        9,
        9,
        3,
        9,
        9,
        9,
        9,
        9,
        5,
        11,
        1,
        0,
        0,
        9,
        12,
        9,
        0,
        0,
        9,
        0,
        0,
        9,
        9,
        9,
        5,
        3,
        13,
        13,
        13,
        13,
        0,
        8,
        5,
        9,
        0,
        0
    };

    intlist_copy(&exceptions_err, exceptions_err_temp, NB_ERRORS);



    // ----- fin messages d'erreur

    CONTAINERS = strlist_create(0);
    ATTRIBUTES = nelist_create(0);
    

    VAR_LOC = ptrlist_create();

    FONCTIONS = tree_create(NULL, number_default(), 0);
    ADRESSES = nelist_create(0);
    NOMS = strlist_create(0);
    compFunc(typesbuiltinsfonc, helpbuiltinsfonc, nbargs, typesArgs, typesRetour);

    srand(time(NULL));

    // définition des variables de base

    defineVariables();

    return;
}


void neonExit(void)
{
    //destruction de tout ce qui a été alloué.

    err_free(VAR_LOC);

    // il faut destroy tout
    strlist_destroy(&acceptedChars, false);
    listlist_destroy(&syntax);
    strlist_destroy(&sousop, false) ;
    err_free(gramm1.tab) ;
    err_free(types_debut.tab);
    err_free(types_fin.tab);
    strlist_destroy(&operateurs3, false);
    strlist_destroy(&operateurs1, false);
    strlist_destroy(&operateurs2, false);
    strlist_destroy(&blockwords, false);
    strlist_destroy(&blockwords1Line, false);
    strlist_destroy(&keywords, false);
    strlist_destroy(&lkeywords, false);
    strlist_destroy(&keywordFunction, false);
    strlist_destroy(&boolean, false);
    strlist_destroy(&exceptions, false);
    strlist_destroy(&constant, false);
    strlist_destroy(&OPERATEURS, false);
    err_free(PRIORITE.tab);
    err_free(OPERANDES.tab);
    err_free(exceptions_err.tab);
    strlist_destroy(&NOMSBUILTINSFONC, false);
    strlist_destroy(&ERRORS_MESSAGES, false);

    tree_destroy(FONCTIONS);

    strlist_destroy(CONTAINERS, true);
    nelist_destroy(ATTRIBUTES, true);

    strlist_destroy(NOMS, true);
    nelist_destroy(ADRESSES,true);

}




#ifdef TI83PCE
    int main(void)
#else
    int main (int argc, char* argv[])
#endif
{

    //stockage des variables
    neonInit();


    // définition de la liste des arguments
    NeObject* l = neo_list_create(0);
    updateFileName(strdup("__principal__")); // nom du fichier actuel

    #ifdef TI83PCE
        // récupération du nom de fichier si existant
        nio_init(&console, NIO_MAX_COLS, NIO_MAX_ROWS, 0, 0, NIO_COLOR_BLACK, NIO_COLOR_WHITE, true);
        nio_set_default(&console);

        char* filename = argsAns();

        nelist_append(ADRESSES,l);
        strlist_append(NOMS,strdup("__args__"));

        if (filename == NULL) // lance le terminal
        {
            startMessage();
            terminal();
        }
        else
        {
            execFile(filename);
            err_free(filename);
        }
    #else
        #ifndef WASM

            // ajout des argument dans le tableau contenant les arguments du programme
            for (int i = 2 ; i < argc ; i++)
                neo_list_append(l,neo_str_create(strdup(argv[i])));

            nelist_append(ADRESSES,l);
            strlist_append(NOMS,strdup("__args__"));

            updateFileName(strdup("__principal__"));

            if (argc >= 2)
            {
                execFile(argv[1]);
            }
            else
            {
                startMessage();
                terminal();
            }

            neonExit();
        #endif
    #endif


    #ifdef TI83PCE
        nio_free(&console);
    #endif

    return 0;
}
