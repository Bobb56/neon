#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "headers.c"

#ifdef WASM
    extern char* INPUTMESSAGE;
#endif


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
    
        for (int i=0 ; i< args->len ; i++)
        {
            if (args->tab[i]->type == TYPE_STRING)
                printString(neo_to_string(args->tab[i]));
            else
                neobject_aff(args->tab[i]);
            
            printString(" ");
        
        }
        printBack();
    
    #endif
    
    char *entree=input("");
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
  char* retour = type(args->tab[0]->type);
  
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
            printString("Built-in function ");
        else if (fun->nbArgsMeth != 0 && fun->type == TYPE_BUILTINFUNC)
            printString("Built-in method ");
        else if (fun->type == TYPE_USERFUNC)
            printString("Function ");
        else if (fun->type == TYPE_USERMETHOD)
            printString("Method ");

        printString(NOMS->tab[nelist_index(ADRESSES,args->tab[0])]);
        newLine();
        
        if (fun->nbArgs == -1)
        {
            printString("Illimited number of arguments of type : ");
            printString(type(fun->typeArgs[0]));
        }
        else
        {
            printString("Needs ");
            printInt(fun->nbArgs);
            printString(" argument(s) of type : ");

            for (int i = 0 ; i < fun->nbArgs ; i++)
            {
                printString(type(fun->typeArgs[i]));
                if (i + 1 < fun->nbArgs)
                    printString(", ");
            }
            
        }

        printString(".");
        newLine();

        printString("The returned type is : ");
        printString(type(fun->typeRetour));
        printString(".");
        newLine();
        
        if (fun->help == NULL)
            printString("No help available for this function.");
        else
        {
            printString(fun->help);
            newLine();
        }
    }
    return neo_none_create();
}




NeObject* _randint_(NeList* args)
{
    double max = number_toDouble(neo_to_nb(args->tab[0]));
    if (max != (int)max)
    {
        CODE_ERROR = 64;// la fonction randint prend en argument un entier positif
        return NULL;
    }

    double result = rand()%(int)max;

    return neo_nb_create(number_fromDouble(result));
}


NeObject* _quitError_(NeList* args)
{
    setColor(RED);
    printString(neo_to_string(args->tab[0]));
    setColor(WHITE);

    CODE_ERROR = 1;
    
    return NULL;
}



NeObject* _time_(NeList* args)
{
    time_t temps = time(NULL);
    double res = (double)temps;
    return neo_nb_create(number_fromDouble(res));
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
    return e;
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