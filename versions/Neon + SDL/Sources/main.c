#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "headers.c"

#if defined(LINUX) || defined(WINDOWS11) || defined(WINDOWS10)
    #include <SDL2/SDL.h>
#endif


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
> optimiser les objets de type container avec une liste globale contenant les noms des champs
> vérifier les champs entrés dans les containers
> changer la définition de type avec un mot-clé typedef (typedef(Cmplx, Re, Im) au lieu de typedef("Cmplx", "Re", "Im"))
> vérifier les priorités de tous les opérateurs


Procédure pour ajouter une fonction built-in :
1) Programmer cette fonction selon le prototype NeObject* _maFonction_ (NeList* args)
4) Ajouter 1 à la constante pré-processeur NBBUILTINFONC définie au début de neon.h
3) Ajouter le prototype de cette fonction tout à la fin de neon.h
4) Ajouter la fonction dans le tableau de fonctions au début de ce fichier
5) Compléter les informations sur la fonction dans neonInit
*/

#if defined(LINUX) || defined(WINDOWS11) || defined(WINDOWS10)
SDL_Window *WINDOW = NULL; // fenêtre principale utilisée par le programme lorsque les graphiques sont utilisés
SDL_Renderer *RENDERER = NULL;
int MAX_WIDTH = 0, MAX_HEIGHT = 0; // taille réelle de l'écran
#endif

char* EXCEPTION = NULL; // exception levée par l'utilisateur, à afficher par printError

int NAME = 0; // adresse du NeObject représentant le nom actuel de fichier. A ne pas utiliser, il y a à la place la fonction updateFileName

int CODE_ERROR = 0; // contient le code de l'erreur survenue lors d'un appel de fonction
strlist ERRORS_MESSAGES;

ptrlist* VAR_LOC; // c'est une pile de pile qui contiendra dans chaque contexte les variables à supprimer


NeObject* RETURN_VALUE = NULL; // contient la valeur de retour de la fonction si le programme a été interrompu par un return ()

strlist* CONTAINERS; // à un indice du tableau, associe le nom du type de container
NeList* ATTRIBUTES;

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


NeObject* (*BUILTINSFONC[NBBUILTINFONC])(NeList*)={_print_,_input_,_nbr_,_str_,_len_,_substring_,_exit_,_append_,_remove_,_insert_,_type_, _reverse_, _eval_,_clear_,_help_, _randint_,_quitError_, _time_, _assert_, _output_, _chr_, _ord_, _list_comp_, _create_exception_, _exception_, _int_, _index_, _replace_, _count_, _list_, _sort_asc_, _sort_desc_, _sin_, _cos_, _tan_, _deg_, _rad_, _sqrt_, _ln_, _exp_, _log_, _log2_, _round_, _abs_, _ceil_, _floor_, _readFile_, _writeFile_, _enableGraphics_, _disableGraphics_, _sleep_, _setPixel_, _refreshScreen_, _getScreenSize_};

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
                op1 = evalAux(tree->sons[0], false); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
                if (CODE_ERROR != 0)
                    return NULL;
                
                op2 = evalAux(tree->sons[1], true);
            }
            else if (OPERANDES.tab[(int)number_toDouble(tree->label2)] == LEFT_VAR)
            {
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

            if (index >= list->len)
            {
                CODE_ERROR = 82;
                neobject_destroy(neo, calcListIndex);
                return NULL;
            }

            number_destroy(tree->label2); // il faut faire attention : tree_destroy va systématiquement chercher à supprimer label2
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

        if (tree->sons[inst]->type == TYPE_TRYEXCEPT)
        {
            Tree* tree_temp = tree->sons[inst];
            // exécution du try
            int var = exec(tree->sons[inst]->sons[0]);

            if (CODE_ERROR != 0)
            {
                
                // on va parcourir la liste des exceptions jusqu'à en trouver une qui corresponde
                _Bool bo = true;
                for (int i = 0 ; i < tree_temp->sons[1]->sons[0]->nbSons ; i++)
                {
                    if (tree_temp->sons[1]->sons[0]->sons[i]->type != TYPE_EXCEPTION)
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

            return var;
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

                    #ifndef TI83PCE
                        char* nomFichier = addStr(neo_to_string(nom), ".ne");

                        updateFileName(strdup(neo_to_string(nom)));

                        execFile(nomFichier);
                        err_free(nomFichier);
                    #else
                        updateFileName(strdup(nom));
                        execFile(neo_to_string(nom));
                    #endif

                    updateFileName(strdup("__main__"));

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
        
    }
    return 0;
}










void printRes(NeObject* res)
{
    if (res->type != TYPE_NONE)
    {
        printString(" (");
        setColor(GREEN);
        printString(type(res));
        setColor(WHITE);
        printString(") = ");
        neobject_aff(res);
        newLine();
    }
    return;
}





void startMessage(void)
{
    setColor(GREEN);
    printString("Welcome to Neon");
    setColor(WHITE);
    newLine();
    
    printString("Neon v");
    printString(VERSION);
    printString(" compiled on ");
    printString(__TIMESTAMP__);
    printString(".");
    newLine();
    
    printString("Discuss on the official Discord server : ");
    printString(INVITATION);
    printString(" or contact me at raphael@langage-neon.org");
    newLine();
    
    printString("Visit ");
    setColor(GREEN); printString("https://langage-neon.org"); setColor(WHITE);
    printString(" for more information");
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
                    tree_destroy(tree);
                    printError(CODE_ERROR);
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
        printError(CODE_ERROR);

    tree_destroy(tree);
    
    CODE_ERROR=0;
    return ;
}




void defineVariables(void)
{

    // valeurs spéciales mathématiques
    #ifndef TI83PCE
        strlist_append(NOMS, strdup("Infinity"));
        nelist_append(ADRESSES, neo_nb_create(number_fromDouble(INFINITY)));
    
        strlist_append(NOMS, strdup("NegInfinity"));
        nelist_append(ADRESSES, neo_nb_create(number_fromDouble(-INFINITY)));
    
        strlist_append(NOMS, strdup("NaN"));
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

    strlist_append(NOMS, strdup("__platform__"));
    nelist_append(ADRESSES, neo_str_create(strdup(p)));

    strlist_append(NOMS, strdup("__name__"));
    nelist_append(ADRESSES, NULL);

    NAME = ADRESSES->len - 1; // l'adresse de __name__ à modifier


    // pi
    nelist_append(ADRESSES, neo_nb_create(number_fromDouble(PI)));
    strlist_append(NOMS, strdup("pi"));


}












void neonInit(void)
{

    #if defined(LINUX) || defined(WINDOWS11) || defined(WINDOWS10)
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        setColor(RED);
        printString("Error: unable to initialize graphics system\n");
        CODE_ERROR = 74;
        setColor(WHITE);
        return ;
    }
    #endif

    //définition de acceptedChars
    static const char* acceptedChars_tab[31] = {"\"", "'", "+","*","-","/","<",">","=","%","&","@","!", ",", ";", "\n", "#", "$", "[", "]", "(", ")", "{", "}", "\\", ".", "_", " ", "\t", ".", ":"};
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

    int types_fin_temp[19] = {TYPE_FONCTION, TYPE_PARENTHESE2, TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_LISTINDEX, TYPE_NUMBER, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_BOOL, TYPE_KEYWORD, TYPE_CONST, TYPE_WORD, TYPE_COMMENT, TYPE_CROCHET, TYPE_EXPRESSION, TYPE_NONE, TYPE_EXCEPTION, TYPE_BLOCK};
    intlist_copy(&types_fin, types_fin_temp, 19);


    //définition de sousop
    static const char* sousop_temp[] = {"+","*","-","/","<",">","=","%","&","@","!", ".",":"};
    strlist_copy(&sousop, sousop_temp, 13);


    // liste qui contient les opérandes prises par les opérateurs, correspond a operateurs3
    int gramm1_temp[NBOPERATEURS] = {RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,RIGHT, RIGHT, VARRIGHT, LEFT_VAR, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT};
    intlist_copy(&gramm1, gramm1_temp, NBOPERATEURS);



    //définition de operateurs3
    static const char* operateurs3_temp[NBOPERATEURS] = {"and","or","xor","+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","not","%","//","&","<-","@", "_", "del", "->", ".", "EE", "=>", "in", ">>", ":"};
    strlist_copy(&operateurs3, operateurs3_temp, NBOPERATEURS);


    //définition de operateurs1
    // opérateurs composés de caractères de sousop
    static const char* operateurs1_temp[] = {"+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","%","//","&","<-","@", "->", ".", "=>", ">>", ":"};
    strlist_copy(&operateurs1, operateurs1_temp, 28);


    // opérateurs mots
    static const char* operateurs2_temp[] = {"and","or","xor","not", "del", "EE", "in"};
    strlist_copy(&operateurs2, operateurs2_temp, 7);



    static const char* OPERATEURS_temp[NBOPERATEURS] = {"and","or","xor","+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","not","%","//","&","<-","@", "_", "del", "->", ".", "EE", "=>", "in", ">>", ":"};
    strlist_copy(&OPERATEURS, OPERATEURS_temp, NBOPERATEURS);


    static const int PRIORITE_temp[NBOPERATEURS] = {8, 8, 8, 5, 4, 5, 4, 3, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 9, 3, 3, 7, 4, 4, 2, 9, 2, 4, 9, 9, 2, 4, 8, 6, 1, 9};
    intlist_copy(&PRIORITE, PRIORITE_temp, NBOPERATEURS);

    static const int OPERANDES_temp[NBOPERATEURS] = {RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,RIGHT, RIGHT, VARRIGHT, LEFT_VAR, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT};
    intlist_copy(&OPERANDES, OPERANDES_temp, NBOPERATEURS);


    // ------------------

    // défnit les tokens de délimitation de blocks

    //mots de blocs d'instructions
    static const char* blockwords_temp[] = {"if","while","for", "ei", "return", "import", "local", "tr", "expt"};
    strlist_copy(&blockwords, blockwords_temp, 9);

    static const char* blockwords1Line_temp[] = {"return", "import", "local"};
    strlist_copy(&blockwords1Line, blockwords1Line_temp, 3);

    static const char* keywordFunction_temp[] = {"function", "method"};
    strlist_copy(&keywordFunction, keywordFunction_temp, 2);

    static const char* keywords_temp[] = {"es", "continue", "break", "pass", "tr"};
    strlist_copy(&keywords, keywords_temp, 5);

    static const char* lkeywords_temp[] = {"continue", "break", "pass"};
    strlist_copy(&lkeywords, lkeywords_temp, 3);


    static const char* boolean_temp[] = {"True","False"};
    strlist_copy(&boolean, boolean_temp, 2);

    static const char* exceptions_temp[] = {
        "SyntaxError",
        "FileNotFound",
        "UnmeasurableObject",
        "UndefinedVariable",
        "IncorrectFunctionCall",
        "MemoryError",
        "NonIndexableObject",
        "IncorrectListIndex",
        "OutOfRange",
        "IncorrectType",
        "DivisionByZero",
        "UnknownError",
        "AssertionFailed",
        "DefinitionError",
        "GraphicError"
    };
    // ici il faut s'assurer que capacity a la bonne valeur
    exceptions.tab = err_malloc(sizeof(char*)*16);
    exceptions.len = 15;
    exceptions.capacity = 4;
    for (int i = 0 ; i < 15 ; i++)
        exceptions.tab[i] = strdup(exceptions_temp[i]);
    

    static const char* constant_temp[] = {"None"};
    strlist_copy(&constant, constant_temp, 1);




    /*----- Préparation des fonctions ------*/
    static const char* NOMSBUILTINSFONC_temp[NBBUILTINFONC] = {"print","input","nbr","str","len","sub","exit","append","remove","insert","type", "reverse", "eval","clear","help", "randint", "failwith", "time", "assert", "output", "chr", "ord", "list_comp", "create_exception", "exception", "int", "index", "replace", "count", "list", "sort_asc", "sort_desc", "sin", "cos", "tan", "deg", "rad", "sqrt", "ln", "exp", "log", "log2", "round", "abs", "ceil", "floor", "readFile", "writeFile", "enableGraphics", "disableGraphics", "sleep", "setPixel", "refreshScreen", "getScreenSize"};
    strlist_copy(&NOMSBUILTINSFONC, NOMSBUILTINSFONC_temp, NBBUILTINFONC);

    static const int typesRetour[NBBUILTINFONC] = {TYPE_NONE, TYPE_STRING, TYPE_NUMBER, TYPE_STRING, TYPE_NUMBER, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_STRING, TYPE_STRING, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NUMBER, TYPE_NONE, TYPE_NUMBER, TYPE_NONE, TYPE_NONE, TYPE_STRING, TYPE_NUMBER, TYPE_LIST, TYPE_EXCEPTION, TYPE_NONE, TYPE_NUMBER, TYPE_NUMBER, TYPE_STRING, TYPE_NUMBER, TYPE_LIST, TYPE_NONE, TYPE_NONE, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_LIST};

    static const char* helpbuiltinsfonc[NBBUILTINFONC] = {
        "Displays arguments in the terminal",
        "Displays the given argument and prompts the user to input in the terminal. Returns the entered value.",
        "Converts a string of characters into a number.",
        "Converts a number into a string of characters.",
        "Returns the length of an object.",
        "Performs substring extraction. sub(\"hello\", 1, 4) returns \"onj\".",
        "Exits the Neon interpreter.",
        "Adds an element to a list.",
        "Removes an element from a list.",
        "Inserts an element into a list.",
        "Returns the type of an object.",
        "Reverses the characters in a string of characters.",
        "Evaluates the given string of characters and returns the result.",
        "Clears the terminal.",
        "Displays help information related to a function.",
        "Returns a random integer between two integers.",
        "Displays the provided text in the terminal and exits the program.",
        "Returns the time elapsed since 1970 in seconds.",
        "Stops the program if at least 1 assertion has failed.",
        "Puts the given arguments on the terminal without spaces or \\n.",
        "Returns the ASCII character associated to the number.",
        "Returns the ASCII value of the given character.",
        "Arguments : index, start, end, step, condition, value\nEvaluates value for all indices between start and end according to step, and adds it to the list if condition is true.\nindex must be the name of the variable used, and condition and value must be string expressions.",
        "Creates an Exception object with the given name.",
        "Exits the program with the given exception.",
        "Takes the integer part of a number.",
        "Returns the index of an element in a list.",
        "replace(str, str1, str2) returns a string in which all occurrences of str1 have been replaced by str2 in str.",
        "Counts the number of occurrences in a list or a string.",
        "Separates every character of a string in a list.",
        "Sorts a list in ascending order.",
        "Sorts a list in descending order.",
        "sin: Sine function",
        "cos: Cosine function",
        "tan: Tangent function",
        "deg: Convert angle from radians to degrees",
        "rad: Convert angle from degrees to radians",
        "sqrt: Square root function",
        "ln: Natural logarithm (base e)",
        "exp: Exponential function (e^x)",
        "log: Logarithm (base 10)",
        "log2: Logarithm (base 2)",
        "round: Round to the nearest decimal number with the given precision",
        "abs: Absolute value",
        "ceil: Ceiling function (smallest integer greater than or equal to)",
        "floor: Floor function (largest integer less than or equal to)",
        "Returns the content of the file whose name was given",
        "Writes the given string in the file whose name was given. The syntax is writeFile(name, content)",
        "Begins to draw in a window",
        "Kills the graphic window",
        "Sleeps during x milliseconds",
        "Sets the pixel at coordinates with the given color. Syntax is setColor(x,y,r,g,b)"
        "Call this function after calling graphics functions to make them take effect",
        "Returns in a list the width and the height of the screen"
    };


    //indique le nombre de premiers arguments à ne pas copier
    static const int typesbuiltinsfonc[NBBUILTINFONC] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // le nombre d'arguments "méthode"


    static const int nbargs[NBBUILTINFONC] = {-1,-1,1,1,1,3,0,2,2,3,1,1,1,0,1,2,1,0,-1,-1,1,1,6,1,1,1,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,2,0,0,1,5,0,0}; // nombre d'arguments en tout

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
        (int[]){TYPE_STRING, TYPE_STRING},
        NULL,
        NULL,
        (int[]){TYPE_NUMBER},
        (int[]){TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER, TYPE_NUMBER},
        NULL,
        NULL
    };

    /* ----- fin préparation des fonctions ------*/


    // tableau des messages d'erreur :

    static const char* ERRORS_MESSAGES_temp[NB_ERRORS] = {
        "",// ne doit pas être utilisé
        "",// ne doit pas être utilisé
        "Multiple decimal points in the same number",
        "File doesn't exist",
        "This object has no length",
        "Use of an undefined variable",
        "Too many arguments in user-defined function call",
        "Insufficient arguments in user-defined function call",
        "Call to an undefined function",
        "Call to an invalid function",
        "Starting value of the for loop is not a number",
        "Incorrect function definition",
        "Memory error",
        "Incorrect index for subtree deletion",
        "Invalid arguments provided in function call",
        "This object is not indexable",
        "List index is not a number",
        "Attempt to index an undefined list",
        "List index out of range",
        "Undetermined error, likely a syntax issue",
        "Unevaluable condition due to incorrect type",
        "Multiple arguments for return statement",
        "'For' loop variant is not a variable",
        "Incorrect built-in function name in function call",
        "Unknown operator",
        "Unknown character",
        "String, list, or other structure not terminated",
        "Unclosed string at the end of line",
        "Incompatible keyword used in an expression",
        "Block instruction line not followed by an instruction block (e.g., a lone if() statement)",
        "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
        "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
        "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
        "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
        "Lone block of instructions",
        "Incorrect arrangement of if/elif/else blocks",
        "Incorrect number of arguments when calling a built-in function",
        "Index out of range for inserting into a NeList",
        "Index out of range for deleting from a NeList",
        "Index out of range to access in a NeList",
        "Unsupported types for addition",
        "Unsupported types for subtraction",
        "Unsupported types for division",
        "Unsupported types for multiplication",
        "Unsupported types for modulo",
        "Euclidean division by zero",
        "Unsupported types for Euclidean division",
        "Unsupported types for 'and' operator",
        "Unsupported types for 'or' operator",
        "Unsupported types for 'xor' operator",
        "Unsupported types for exponentiation",
        "Unsupported types for equality tests",
        "Unsupported types for <=",
        "Unsupported types for >=",
        "Unsupported types for <",
        "Unsupported types for >",
        "Attempt to assign the value of an undefined variable to another variable",
        "Improper use of the '->' operator",
        "Unsupported types for 'not' operator",
        "Referencing an undefined variable",
        "Dereferencing something other than a string",
        "Unsupported types for unary minus operator",
        "Unsupported types for reverse function",
        "Attempt to use help on a non-function variable",
        "The randint function must have two positive integers a and b as arguments, with a < b",
        "Pointer is not allocated",
        "Undefined error",
        "Can not open file",
        "More closing parentheses, curved brackets or square brackets than opening ones",
        "local must have at least one argument",
        "local can only be used in a function or in a method",
        "Assertion failed",
        "The '.' operator is only for functions or methods and can only be applied on a variable",
        "Incorrect hexadecimal or binary number",
        "", // code 74 spécial pour les appels d'exceptions
        "Unsupported types for implication.",
        "except block not preceded by a try block.",
        "try block not followed by an except block",
        "except expects exceptions",
        "in can only be calculated with a list",
        "'>>' is only for containers",
        "A container field is set but the container is not used",
        "Unknown container field",
        "Same container name but not same fields",
        "Containers must contain at least one field",
        "Unknown container type",
        "Two different fields cannot have the same name in a container",
        "Incorrect container definition",
        "Object not in list",
        "Cannot execute a method on a litteral object",
        "That list cannot be sorted",
        "Cannot call a function while dereferencing a container field",
        "Invalid use of ':'",
        "Error while initializing graphics",
        "Please use enableGraphics() after using graphical functions"
    };

    strlist_copy(&ERRORS_MESSAGES, ERRORS_MESSAGES_temp, NB_ERRORS);

    static const int exceptions_err_temp[NB_ERRORS] = {
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
        0,
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
        0,
        0,
        0,
        9,
        0,
        0,
        0,
        9,
        0,
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
        0,
        14,
        14
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


    #if defined(LINUX) || defined(WINDOWS11) || defined(WINDOWS10)
    if (WINDOW != NULL)
        SDL_DestroyWindow(WINDOW);

    SDL_Quit();
    #endif

}




#ifdef TI83PCE
    int main(void)
#else
    int main (int argc, char* argv[])
#endif
{

    //stockage des variables
    neonInit();

    if (CODE_ERROR != 0)
        return 0;


    // définition de la liste des arguments
    NeObject* l = neo_list_create(0);
    updateFileName(strdup("__main__")); // nom du fichier actuel

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

            updateFileName(strdup("__main__"));

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
