#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "headers.c"
#include <stdio.h>


#ifdef WASM
    extern char* INPUTMESSAGE;
#endif

extern strlist* CONTAINERS;
extern NeList* ATTRIBUTES;

extern char* EXCEPTION;

extern strlist ERRORS_MESSAGES;
extern intlist exceptions_err;

extern int CODE_ERROR;


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
extern strlist exceptions;

extern strlist keywords;
extern strlist lkeywords;
extern strlist constant;


extern strlist OPERATEURS;
extern intlist PRIORITE;
extern intlist OPERANDES;


//stockage des variables
extern strlist* NOMS;
extern NeList* ADRESSES;

extern strlist NOMSBUILTINSFONC;
extern strlist HELPBUILTINSFONC;
extern intlist TYPESBUILTINSFONC;

extern NeObject* (*OPFONC[NBOPERATEURS])(NeObject*,NeObject*);
extern NeObject* (*BUILTINSFONC[NBBUILTINFONC])(NeList*);



extern int error;




NeObject* _print_(NeList* args)
{
  for (int i=0 ; i< args->len ; i++)
  {
      if (args->tab[i]->type == TYPE_STRING)
          printString(neo_to_string(args->tab[i]));
      else
        neobject_aff(args->tab[i]);
      printString(" ");
  }
  newLine();
  
  return neo_none_create();
}



NeObject* _input_(NeList* args)
{
    
    #ifdef WASM
        if (INPUTMESSAGE != NULL)
            err_free(INPUTMESSAGE);
        
        if (args->len == 0)
        {
            INPUTMESSAGE = strdup("");
        }
        else
        {
            char* str1 = strdup(""), * str2, *temp;
            for (unsigned i=0 ; i < args->len ; i++)
            {
                temp = (args->tab[i]->type == TYPE_STRING) ? strdup(neo_to_string(args->tab[i])) : neobject_str(args->tab[i]);
                str2 = addStr(str1,temp);
                err_free(temp);err_free(str1);
                str1 = addStr(str2," ");
                err_free(str2);
            }
            
            INPUTMESSAGE = str1;
        }
    #else

        #ifndef LINUX
    
            for (int i=0 ; i< args->len ; i++)
            {
                if (args->tab[i]->type == TYPE_STRING)
                    printString(neo_to_string(args->tab[i]));
                else
                    neobject_aff(args->tab[i]);
                
                
                if (i < args->len - 1)
                    printString(" ");
            
            }

            char *entree=input("");

        #else
            // à cause de linenoise, il faut mettre tout le texte dans une seule chaine de caractères
            char* chaine = strdup("");

            for (int i=0 ; i< args->len ; i++)
            {
                if (args->tab[i]->type == TYPE_STRING)
                    chaine = addStr2(chaine, neo_to_string(args->tab[i]));
                else
                    chaine = addStr2(chaine,neobject_str(args->tab[i]));
                
                
                if (i < args->len - 1)
                    chaine = addStr2(chaine, " ");
            }

            char* entree = input(chaine);
            err_free(chaine);

        #endif
    
    #endif
    
    return neo_str_create(entree);
}




NeObject* _nbr_(NeList* args)
{
    Number ret = number_fromStr(neo_to_string(args->tab[0]));
    if (CODE_ERROR != 0)
        return NULL;
    
    return neo_nb_create(ret);
}



NeObject* _str_(NeList* args)
{
    return neo_str_create(neobject_str(args->tab[0]));
}


NeObject* _len_(NeList* args)
{
  NeObject* retour;
  if (args->tab[0]->type==TYPE_LIST)
  {
    retour = neo_nb_create(number_fromDouble((double)neo_list_len(args->tab[0])));
  }
  else if (args->tab[0]->type==TYPE_STRING)
  {
    retour = neo_nb_create(number_fromDouble((double)strlen(neo_to_string(args->tab[0]))));
  }
  else
  {
    CODE_ERROR = 4; // cet objet n'a pas de longueur
    return NULL;
  }
  
  return retour;
  
}


NeObject* _substring_(NeList* args)
{
    char* substring = sub(neo_to_string(args->tab[0]),(int)number_toDouble(neo_to_nb(args->tab[1])),(int)number_toDouble(neo_to_nb(args->tab[2])));
    return neo_str_create(substring);
}



NeObject* _exit_(NeList* args)
{
  CODE_ERROR = 1;
  return NULL;
}



NeObject* _append_(NeList* args)
{
    neo_list_append(args->tab[0], neo_copy(args->tab[1]));
    return neo_none_create();
}







NeObject* _remove_(NeList* args)
{
    neo_list_remove(args->tab[0], (int long)number_toDouble(neo_to_nb(args->tab[1])));
    return neo_none_create();
}





NeObject* _insert_(NeList* args)
{
    neo_list_insert(args->tab[0], neo_copy(args->tab[1]), (int long)number_toDouble(neo_to_nb(args->tab[2])));
    return neo_none_create();
}




NeObject* _type_(NeList* args)
{
  char* retour = type(args->tab[0]);
  
  return neo_str_create(strdup(retour));
  
}


NeObject* _reverse_(NeList* args)
{
    if (args->tab[0]->type == TYPE_STRING)
    {
        char* chaine = neo_to_string(args->tab[0]);
        int len = strlen(chaine);
        
        char* retour = err_malloc((len+1)*sizeof(char));
        
        for (int i=0 ; i<len ; i++)
        {
            retour[i] = chaine[len-i-1];
        }
        retour[len] = '\0';
        
        
        return neo_str_create(retour);
    }
    else
    {
        CODE_ERROR = 62;//unsupported types for reverse
        return NULL;
    }
}


NeObject* _eval_(NeList* args)
{
    Tree* tree = tree_create(NULL, number_default(), 0);
    
    char* exp = neo_to_string(args->tab[0]);
    
    createExpressionTree(tree, exp);
    NeObject* res = eval(tree);
    
    
    tree_destroy(tree);
    
    return res;
}



NeObject* _clear_(NeList* args)
{
  clearConsole();
  return neo_none_create();
}


NeObject* _help_(NeList* args)
{
    if (args->tab[0]->type != TYPE_FONCTION)
    {
        CODE_ERROR = 63;//erreur : ceci n'est pas une fonction
        return NULL;
    }
    else
    {
        Function* fun = args->tab[0]->data;
        if (fun->nbArgsMeth == 0 && fun->type == TYPE_BUILTINFUNC)
            printString("Fonction de base ");
        else if (fun->nbArgsMeth != 0 && fun->type == TYPE_BUILTINFUNC)
            printString("Methode de base ");
        else if (fun->type == TYPE_USERFUNC)
            printString("Fonction ");
        else if (fun->type == TYPE_USERMETHOD)
            printString("Methode ");

        printString(NOMS->tab[nelist_index2(ADRESSES,args->tab[0])]);
        newLine();
        
        if (fun->nbArgs == -1)
        {
            printString("Nombre illimite d'arguments de type : ");

            NeObject neo = (NeObject) {.type = fun->typeArgs[0], .data = NULL};
            printString(type(&neo));
        }
        else
        {
            printString("Necessite ");
            printInt(fun->nbArgs);
            printString(" argument(s) du type : ");

            for (int i = 0 ; i < fun->nbArgs ; i++)
            {
                NeObject neo = (NeObject) {.type = fun->typeArgs[i], .data = NULL};
                printString(type(&neo));

                if (i + 1 < fun->nbArgs)
                    printString(", ");
            }
            
        }

        printString(".");
        newLine();

        printString("Le type de retour est : ");

        NeObject neo = (NeObject) {.type = fun->typeRetour, .data = NULL};
        printString(type(&neo));

        printString(".");
        newLine();
        
        if (fun->help == NULL)
            printString("Aucune aide n'est disponible pour cet objet.");
        else
        {
            printString((char*)fun->help);
            newLine();
        }
    }
    return neo_none_create();
}




NeObject* _randint_(NeList* args)
{
    return neo_nb_create(number_randint(neo_to_nb(args->tab[0]), neo_to_nb(args->tab[1])));
}


NeObject* _quitError_(NeList* args)
{
    setColor(RED);
    printString(neo_to_string(args->tab[0]));newLine();
    setColor(WHITE);

    CODE_ERROR = 1;
    
    return NULL;
}



NeObject* _time_(NeList* args)
{
    #ifndef TI83PCE
        time_t temps = time(NULL);
        double res = (double)temps;
        return neo_nb_create(number_fromDouble(res));
    #else
        long int t = rtc_Time();
        return neo_nb_create(number_fromDouble((double)t));
    #endif
}

NeObject* _assert_(NeList* args)
{
    for (int i = 0 ; i < args->len ; i++)
    {
        if (!neo_is_true(args->tab[i]))
        {
            CODE_ERROR = 71;
            return NULL;
        }
    }
    return neo_none_create();
}



NeObject* _output_(NeList* args)
{
  for (int i=0 ; i< args->len ; i++)
  {
      if (args->tab[i]->type == TYPE_STRING)
          printString(neo_to_string(args->tab[i]));
      else
        neobject_aff(args->tab[i]);
  }
  return neo_none_create();
}


NeObject* _chr_(NeList* args)
{
    Number x = neo_to_nb(args->tab[0]);
    long int n = (long int)number_toDouble(x);
    char* c = err_malloc(sizeof(char) * 2);
    c[0] = (char)n;
    c[1] = '\0';
    return neo_str_create(c);
}

NeObject* _ord_(NeList* args)
{
    char* c = neo_to_string(args->tab[0]);
    long int n = (long int)c[0];

    Number x = number_fromDouble((double)n);
    return neo_nb_create(x);
}



NeObject* _list_comp_(NeList* args)
{
    /* args : indice, debut, fin, pas, condition, valeur */
    char* nom=neo_to_string(args->tab[0]);

    int index;
    NeObject* i;

    if(!strlist_inList(NOMS, nom))
    {
        strlist_append(NOMS, strdup("i"));
        i = neobject_create(TYPE_EMPTY);

        index = ADRESSES->len;
        
        nelist_append(ADRESSES, i);
    }
    else
    {
        index = strlist_index(NOMS,nom);
        i = ADRESSES->tab[index];
    }
    
    _affect2(i, args->tab[1]); // valeur de debut

    Tree* cond = tree_create(NULL, number_default(),0);
    Tree* val = tree_create(NULL, number_default(),0);

    createExpressionTree(cond, neo_to_string(args->tab[4]));

    if (CODE_ERROR != 0)
    {
        tree_destroy(cond);
        tree_destroy(val);
    }

    createExpressionTree(val, neo_to_string(args->tab[5]));

    if (CODE_ERROR != 0)
    {
        tree_destroy(cond);
        tree_destroy(val);
    }

    NeObject* liste = neo_list_create(0);
    NeObject* x;

    while (number_inf(neo_to_nb(i), neo_to_nb(args->tab[2])))
    {
        if (isTrue(cond))
            neo_list_append(liste, eval(val));

        // incrémentation de la variable d'indice
        x = _add(i, args->tab[3]);
        neobject_destroy(i, false);
        i->data = x->data;
        err_free(x);
    }

    tree_destroy(cond);
    tree_destroy(val);
    
    return liste;
}


NeObject *_create_exception_(NeList* args)
{
    strlist_append(&exceptions, strdup(neo_to_string(args->tab[0])));
    NeObject* e = neo_exception_create(exceptions.len - 1);

    if (strlist_inList(NOMS,neo_to_string(args->tab[0])))
    {
        int index = strlist_index(NOMS,neo_to_string(args->tab[0]));
        _affect2(ADRESSES->tab[index], e);
        neobject_destroy(e, true);
    }
    else
    {
        strlist_append(NOMS,strdup(neo_to_string(args->tab[0]))); // on ajoute le nom à la liste des noms
        nelist_append(ADRESSES,e); // on ajoute l'objet à la liste des variables
    }

    return neo_none_create();
}


NeObject* _exception_(NeList* args)
{
    EXCEPTION = exceptions.tab[*(int*)(args->tab[0]->data)]; // exception qui sera affichée par printError
    CODE_ERROR = 74;
    return NULL;
}


NeObject * _int_(NeList* args)
{
    Number x = *(Number*)args->tab[0]->data;
    int n = (long int)number_toDouble(x);
    return neo_nb_create(number_fromDouble((double)n));
}


NeObject* _index_(NeList* args)
{
    int i = nelist_index2((NeList*)args->tab[0]->data, args->tab[1]);

    if (i >= 0)
        return neo_nb_create(number_fromDouble((double)i));
    
    return NULL;
}


NeObject* _replace_(NeList* args)
{
    char* str = replace(
        neo_to_string(args->tab[0]),
        neo_to_string(args->tab[1]),
        neo_to_string(args->tab[2])
    );

    return neo_str_create(str);
}


NeObject* _count_(NeList* args)
{
    if (args->tab[0]->type == TYPE_LIST)
    {
        int count = 0;
        NeList* l = args->tab[0]->data;
        for (int i = 0; i < l->len ; i++)
        {
            if (neo_equal(args->tab[1], l->tab[i]))
                count++;
        }
        return neo_nb_create(number_fromDouble((double)count));
    }
    else if (args->tab[0]->type == TYPE_STRING)
    {
        return neo_nb_create(number_fromDouble((double)count(
            neo_to_string(args->tab[0]),
            neo_to_string(args->tab[1])
        )));
    }
    else
    {
        CODE_ERROR = 14;
        return NULL;
    }
}


NeObject* _list_(NeList* args)
{
    char* s = neo_to_string(args->tab[0]);
    NeList* l = nelist_create(strlen(s));
    for (int i=0 ; i < l->len ; i++)
        l->tab[i] = neo_str_create(charToString(s[i]));
    
    return neo_list_convert(l);
}




void swap(NeObject **a, NeObject **b) {
    NeObject* tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

// Fonction pour partitionner le tableau en utilisant le dernier élément comme pivot
int partition(NeObject** arr, int low, int high, int (*compare) (NeObject*,NeObject*)) {
    NeObject* pivot = arr[high];  // Choix du pivot
    int i = (low - 1);  // Index du plus petit élément

    for (int j = low; j <= high - 1; j++) {
        // Si l'élément courant est plus petit que le pivot
        if (compare(arr[j], pivot) < 0) {
            i++;  // Incrémenter l'index du plus petit élément
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}



// Fonction principale de tri rapide
void quickSort(NeObject* arr[], int low, int high, int (*compare) (NeObject*,NeObject*)) {
    if (low < high) {
        // Obtention de l'index de partition
        int pi = partition(arr, low, high, compare);

        // Trier les éléments avant et après la partition
        quickSort(arr, low, pi - 1, compare);
        quickSort(arr, pi + 1, high, compare);
    }
}


NeObject* _sort_asc_(NeList* args)
{
    NeList* l = args->tab[0]->data;

    quickSort(l->tab, 0, l->len - 1, neo_compare);

    if (CODE_ERROR != 0)
        return NULL;
    
    return neo_none_create();
}



NeObject* _sort_desc_(NeList* args)
{
    NeList* l = args->tab[0]->data;

    quickSort(l->tab, 0, l->len - 1, neo_compare2);

    if (CODE_ERROR != 0)
        return NULL;
    
    return neo_none_create();
}


NeObject* _sin_(NeList* args)
{
    return neo_nb_create(number_sin(neo_to_nb(args->tab[0])));
}


NeObject* _cos_(NeList* args)
{
    return neo_nb_create(number_cos(neo_to_nb(args->tab[0])));
}


NeObject* _tan_(NeList* args)
{
    return neo_nb_create(number_tan(neo_to_nb(args->tab[0])));
}


NeObject* _deg_(NeList* args)
{
    return neo_nb_create(number_deg(neo_to_nb(args->tab[0])));
}


NeObject* _rad_(NeList* args)
{
    return neo_nb_create(number_rad(neo_to_nb(args->tab[0])));
}


NeObject* _sqrt_(NeList* args)
{
    return neo_nb_create(number_sqrt(neo_to_nb(args->tab[0])));
}


NeObject* _ln_(NeList* args)
{
    return neo_nb_create(number_ln(neo_to_nb(args->tab[0])));
}


NeObject* _exp_(NeList* args)
{
    return neo_nb_create(number_exp(neo_to_nb(args->tab[0])));
}


NeObject* _log_(NeList* args)
{
    return neo_nb_create(number_log(neo_to_nb(args->tab[0])));
}


NeObject* _log2_(NeList* args)
{
    return neo_nb_create(number_log2(neo_to_nb(args->tab[0])));
}


NeObject* _round_(NeList* args)
{
    return neo_nb_create(number_round(neo_to_nb(args->tab[0]), neo_to_nb(args->tab[1])));
}


NeObject* _abs_(NeList* args)
{
    return neo_nb_create(number_abs(neo_to_nb(args->tab[0])));
}


NeObject* _ceil_(NeList* args)
{
    return neo_nb_create(number_ceil(neo_to_nb(args->tab[0])));
}


NeObject* _floor_(NeList* args)
{
    return neo_nb_create(number_floor(neo_to_nb(args->tab[0])));
}



NeObject* _readFile_(NeList* args)
{
    char* res = openFile(neo_to_string(args->tab[0]));

    if (CODE_ERROR != 0)
        return NULL;
    
    return neo_str_create(res);
}



NeObject* _writeFile_(NeList* args)
{
    writeFile(neo_to_string(args->tab[0]),neo_to_string(args->tab[1]));

    if (CODE_ERROR != 0)
        return NULL;
    
    return neo_none_create();
}
