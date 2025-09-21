// bibliothèque générale de structures des données Neon


#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "headers.c"


extern int CODE_ERROR;

extern strlist* CONTAINERS;
extern NeList* ATTRIBUTES;

extern intlist TYPESBUILTINSFONC;
extern strlist NOMSBUILTINSFONC;
extern NeObject* (*BUILTINSFONC[NBBUILTINFONC])(NeList*);
extern NeList* ADRESSES;
extern strlist HELPBUILTINSFONC;
extern strlist* NOMS;
extern intlist NBARGS;

extern strlist exceptions;

/*
Correspondances NeObject data void* avec le type du NeObject

number -> Number*
bool -> _Bool*
string -> char*
list -> NeList*




TESTS :
    
    // création de 3 types de base
    NeObject* neo1 = neo_str_create(strdup("coucou"));
    NeObject* neo2 = neo_nb_create(12.7);
    NeObject* neo3 = neo_bool_create(strToBool("True"));
    
    //creation de 3 autres types de base
    NeObject* neo4 = neo_str_create(strdup("chat"));
    NeObject* neo5 = neo_nb_create(-1.234);
    NeObject* neo6 = neo_bool_create(strToBool("False"));
    
    //ajout des 3 premiers types de base dans une liste
    NeObject* neo7 = neo_list_create(0);
    neo_list_append(neo7, neo1);
    neo_list_append(neo7, neo2);
    neo_list_append(neo7, neo3);
    
    //ajout des 3 autres types de base dans une deuxième liste
    NeObject* neo8 = neo_list_create(0);
    neo_list_append(neo8, neo4);
    neo_list_append(neo8, neo5);
    neo_list_append(neo8, neo6);
    
    // création d'une troisième liste dans laquelle on met les deux précédentes
    NeObject* neo9 = neo_list_create(0);
    neo_list_append(neo9, neo7);
    neo_list_append(neo9, neo8);
    
    
    
    
    neobject_aff(neo9);
    printf("\n");
    neobject_aff(neo_list_nth(neo9,0));
    printf("\n");
    neobject_aff(neo_list_nth(neo9,1));
    
    printf("\n");
    
    neobject_destroy(neo9);


*/




NeObject* neobject_create(char type)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = NULL;
    neo->type = type;
    
    return neo;
}



NeObject* neo_container_create(int type, NeList* data)
{
    Container* c = err_malloc(sizeof(Container));
    c->type = type;
    c->data = data;

    NeObject* neo = neobject_create(TYPE_CONTAINER);
    neo->data = (void*)c;
    return neo;
}



NeObject* neo_nb_create(Number number)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = err_malloc(sizeof(Number));
    neo->type = TYPE_NUMBER;
    *((Number*)neo->data) = number;
    return neo;
}



NeObject* neo_bool_create(_Bool boolean)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = err_malloc(sizeof(_Bool*));
    neo->type = TYPE_BOOL;
    *((_Bool*)neo->data) = boolean;
    return neo;
}


_Bool neo_is_true(NeObject* neo)
{
    return *(_Bool*)neo->data;
}


NeObject* neo_fun_create(int nbArgsMeth, NeObject * (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = function_create(nbArgsMeth,ptr, help, nbArgs, typeArgs, typeRetour);
    neo->type = TYPE_FONCTION;
    return neo;
}



Function* function_create(int nbArgsMeth, NeObject * (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour)
{
    Function* fun = err_malloc(sizeof(Function));
    fun->type = TYPE_BUILTINFUNC;
    fun->ptr = ptr;
    fun->nbArgs = nbArgs;
    fun->help = help;
    fun->nbArgsMeth = nbArgsMeth;
    fun->typeRetour = typeRetour;

    // copie de typeArgs dans le tas
    if (nbArgs == -1)
    {
        fun->typeArgs = err_malloc(sizeof(int));
        fun->typeArgs[0] = typeArgs[0];
    }
    else
    {
        fun->typeArgs = err_malloc(sizeof(int)*nbArgs);
        for (int i = 0 ; i < nbArgs ; i++)
            fun->typeArgs[i] = typeArgs[i];
    }
    
    return fun;
}


NeObject* functionCall(NeObject* fun, NeList* args)
{
    Function* f = fun->data;
    if (f->nbArgs != -1 && f->nbArgs != args->len)
    {
        CODE_ERROR = 36;// nombre d'arguments invalide pour appeler cette fonction
        return 0;
    }
    NeObject* (*ptr) (NeList*) = f->ptr;
    NeObject* ret = ptr(args);
    return ret;
}




void compFunc(const int* typesbuiltinsfonc, const char** helpbuiltinsfonc, const int* nbargs, const int** typesArgs, const int* typesRetour)
{
    NeObject* func;
    for (int i = 0 ; i < NBBUILTINFONC ; i++)
    {
        func = neo_fun_create(typesbuiltinsfonc[i], BUILTINSFONC[i], helpbuiltinsfonc[i], nbargs[i], typesArgs[i], typesRetour[i]);
        strlist_append(NOMS, strdup(NOMSBUILTINSFONC.tab[i]));
        nelist_append(ADRESSES, func);
    }
    return;
}




_Bool funcArgsCheck(Function* fun, NeList* args)
{

    if (fun->nbArgs == -1)
    {
        for (int i=0 ; i < args->len ; i++)
        {
            if (fun->typeArgs[0] != -1 && (int)args->tab[i]->type != fun->typeArgs[0])
            {
                return false;
            }
        }
    }
    else
    {
        if (fun->nbArgs != args->len)
            return false;
        
        for (int i=0 ; i < args->len ; i++)
        {
            if (fun->typeArgs[i] != -1 && args->tab[i]->type != fun->typeArgs[i])
                return false;
        }
    }

    return true;
}



NeObject* userFuncCreate(NeObject** args, Tree* code, int nbArgs)
{
    UserFunc* fun = err_malloc(sizeof(UserFunc));
    fun->args = args;
    fun->code = code;
    fun->nbArgs = nbArgs;

    NeObject* neo = neobject_create(TYPE_USERFUNC);
    neo->data = (void*)fun;
    
    return neo;
}








NeObject* neo_str_create(char* string) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = string;
    neo->type = TYPE_STRING;
    return neo;
}



NeObject* neo_exception_create(int index)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    // détermine la place de la chaine dans le tableau exception
    neo->data = err_malloc(sizeof(int));
    *(int*)neo->data = index;
    neo->type = TYPE_EXCEPTION;
    return neo;
}




NeObject* neo_const_create(char* string) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = string;
    neo->type = TYPE_CONST;
    return neo;
}



NeObject* neo_none_create(void) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = NULL;
    neo->type = TYPE_NONE;
    return neo;
}


NeObject* neo_copy(NeObject* neo)
{
    
    if (neo->type == TYPE_NUMBER)
    {
        return neo_nb_create(number_copy(neo_to_nb(neo)));
    }

    else if (neo->type == TYPE_CONST)
    {
        return neo_const_create(strdup(neo_to_const(neo)));
    }

    else if (neo->type == TYPE_STRING)
    {
        return neo_str_create(strdup(neo_to_string(neo)));
    }

    else if (neo->type == TYPE_NONE)
    {
        return neo_none_create();
    }

    else if (neo->type == TYPE_BOOL)
    {
        return neo_bool_create(neo_to_bool(neo));
    }

    else if (neo->type == TYPE_EXCEPTION)
    {
        return neo_exception_create(*(int*)neo->data);
    }
    
    else if (neo->type == TYPE_LIST)
    {
        return neo_list_convert(nelist_copy((NeList*)neo->data));
    }
    else if (neo->type == TYPE_FONCTION)
    {
        Function* fun = neo->data;
        NeObject* ret = neo_fun_create(fun->nbArgsMeth, fun->ptr, fun->help, fun->nbArgs, fun->typeArgs, fun->typeRetour);
        return ret;
        
    }
    else if (neo->type == TYPE_USERFUNC || neo->type == TYPE_USERMETHOD)
    {
        UserFunc* fun = neo->data;
        NeObject** args = err_malloc(sizeof(NeObject) * fun->nbArgs);
        
        for (int i = 0 ; i < fun->nbArgs ; i++)
            args[i] = fun->args[i];

        NeObject* ret = userFuncCreate(args, fun->code, fun->nbArgs);
        ret->type = neo->type;
        
        return ret;
    }
    else if (neo->type == TYPE_CONTAINER)
    {
        Container* c = (Container*)neo->data;
        NeList* data = nelist_copy(c->data);
        
        return neo_container_create(c->type, data);
    }
    else if (neo->type == TYPE_EMPTY)
    {
        return neobject_create(TYPE_EMPTY);
    }
    
    return NULL;
}



Number neo_to_nb(NeObject* neo)
{
  return *((Number*)neo->data);
}


_Bool neo_to_bool(NeObject* neo)
{
  return *((_Bool*)neo->data);
}



char* neo_to_string(NeObject* neo)
{
  return (char*)neo->data;
}


char* neo_to_const(NeObject* neo)
{
  return (char*)neo->data;
}



char* neo_to_none(NeObject* neo)
{
  return (char*)neo->data;
}


NeObject* neo_list_create(int len)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = nelist_create(len);
    neo->type = TYPE_LIST;
    return neo;
}




NeObject* neo_list_convert(NeList* list)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = list;
    neo->type = TYPE_LIST;
    return neo;
}




void neobject_destroy(NeObject* neo, _Bool bo)
{
    if (neo != NULL)
    {
        if (neo->data != NULL) // on supprime neo->data
        {
            if (neo->type == TYPE_NUMBER)
            {
                number_destroy(neo_to_nb(neo));
                err_free(neo->data);
            }
            else if (neo->type == TYPE_LIST)
            {
                nelist_destroy((NeList*)neo->data,true);
            }
            else if (neo->type == TYPE_FONCTION)
            {
                Function* fun = neo->data;
                if (fun->type == TYPE_BUILTINFUNC)
                    err_free(fun->typeArgs);
                
                err_free(fun);
            }
    
            else if (neo->type == TYPE_USERFUNC || neo->type == TYPE_USERMETHOD)
            {
                UserFunc* fun = neo->data;
                
                err_free(fun->args);
                err_free(fun);
            }

            else if (neo->type == TYPE_CONTAINER)
            {
                Container* c = (Container*)neo->data;
                nelist_destroy(c->data, true);
                err_free(c);
            }
            else if (neo->data != NULL)
            {
                err_free(neo->data);
            }
    
            neo->data = NULL;
        }
        
        if (bo) // on supprime neo
        {
            err_free(neo);
        }
    }
    return;
}



void neobject_aff(NeObject* neo)
{
    
    if (neo->type == TYPE_NUMBER)
    {
        number_aff(neo_to_nb(neo));
    }
    else if (neo->type == TYPE_BOOL)
    {
        if (*((_Bool*)neo->data))
            printString("Vrai");
        else
            printString("Faux");
    }
    else if (neo->type == TYPE_STRING)
    {
        printString("\"");
        printString((char*)neo->data);
        printString("\"");
    }
    else if (neo->type == TYPE_LIST)
    {
        nelist_aff((NeList*)neo->data);
    }
    else if (neo->type == TYPE_CONST)
    {
        printString(neo_to_string(neo));
    }
    else if (neo->type == TYPE_EXCEPTION)
    {
        printString(exceptions.tab[*(int*)neo->data]);
    }
    else if (neo->type == TYPE_NONE)
    {
        printString("Rien");
    }
    else if (neo->type == TYPE_FONCTION)
    {
        Function* fun = neo->data;
        
        if (fun->nbArgsMeth == 0 && fun->type == TYPE_BUILTINFUNC)
            printString("<fonction de base>");
        else if (fun->nbArgsMeth != 0 && fun->type == TYPE_BUILTINFUNC)
            printString("<methode de base>");
    }

    else if (neo->type == TYPE_USERFUNC)
    {
        printString("<fonction>");
    }

    else if (neo->type == TYPE_USERMETHOD)
    {
        printString("<methode>");
    }
    else if (neo->type == TYPE_CONTAINER)
    {
        Container* c = (Container*)neo->data;
        NeList* list = (NeList*)(ATTRIBUTES->tab[c->type]->data);

        printString(CONTAINERS->tab[c->type]);
        printString("(");
        for (int i=0 ; i < c->data->len ; i++)
        {
            printString(neo_to_string(list->tab[i]));
            printString(": ");
            neobject_aff(c->data->tab[i]);
            if (i < c->data->len - 1)
                printString(", ");
        }
        printString(")");
    }

}



char* neobject_str(NeObject* neo)
{

    if (neo->type == TYPE_NUMBER)
    {
        return number_toStr(neo_to_nb(neo));
    }
    else if (neo->type == TYPE_BOOL)
    {
        if (*((_Bool*)neo->data))
            return strdup("Vrai");
        else
            return strdup("Faux");
    }
    else if (neo->type == TYPE_STRING)
    {
        char* str1 = addStr("\"",(char*)neo->data);
        char* str2 = addStr(str1,"\"");
        err_free(str1);
        return str2;
    }
    else if (neo->type == TYPE_LIST)
    {
        return nelist_str((NeList*)neo->data);
    }
    else if (neo->type == TYPE_CONST)
    {
        return strdup(neo_to_string(neo));
    }
    else if (neo->type == TYPE_EXCEPTION)
    {
        return strdup(exceptions.tab[*(int*)neo->data]);
    }
    else if (neo->type == TYPE_NONE)
    {
        return strdup("Rien");
    }
    else if (neo->type == TYPE_FONCTION)
    {        
        Function* fun = neo->data;

        if (fun->nbArgsMeth == 0 && fun->type == TYPE_BUILTINFUNC)
            return strdup("<fonction de base>");
        else if (fun->nbArgsMeth != 0 && fun->type == TYPE_BUILTINFUNC)
            return strdup("<methode de base>");
    }

    else if (neo->type == TYPE_USERFUNC)
    {
        return strdup("<fonction>");
    }

    else if (neo->type == TYPE_USERMETHOD)
    {
        return strdup("<methode>");
    }

    else if (neo->type == TYPE_CONTAINER)
    {
        Container* c = (Container*)neo->data;
        NeList* list = (NeList*)(ATTRIBUTES->tab[c->type]->data);

        char* str1 = strdup(CONTAINERS->tab[c->type]);
        str1 = addStr2(str1, "(");
        for (int i=0 ; i < c->data->len ; i++)
        {
            str1 = addStr2(str1, neo_to_string(list->tab[i]));
            str1 = addStr2(str1, ": ");

            char* s = neobject_str(c->data->tab[i]);
            str1 = addStr2(str1, s);
            err_free(s);

            if (i < c->data->len - 1)
                str1 = addStr2(str1, ", ");
        }

        return addStr2(str1, ")");;
    }

    return NULL;
}


_Bool nelist_equal(NeList* l1, NeList* l2)
{
    _Bool bo = l1->len==l2->len;
    for (int i=0 ; bo && i < l1->len ; i++)
        bo = bo && neo_equal(l1->tab[i], l2->tab[i]);
    
    return bo;
}



_Bool neo_equal(NeObject* _op1, NeObject* _op2)
{
    if (_op1->type==TYPE_NUMBER)
    {
        if (_op2->type==TYPE_NUMBER)
        {
            return number_equal(neo_to_nb(_op1), neo_to_nb(_op2));
        }
        else
        {
            return false;
        }
    }

    else if (_op1->type==TYPE_STRING)
    {
        if (_op2->type==TYPE_STRING)
        {
            return strcmp(neo_to_string(_op1),neo_to_string(_op2)) == 0;
        }
        else
        {
            return false;
        }
    }

    else if (_op1->type==TYPE_BOOL)
    {
        if (_op2->type==TYPE_BOOL)
        {
            return neo_to_bool(_op1)==neo_to_bool(_op2);
        }
        else
        {
            return false;
        }
    }
  
    else if (_op1->type==TYPE_NONE && _op2->type==TYPE_NONE)
        return true;
  
    else if (_op1->type==TYPE_EXCEPTION)
    {
        if (_op2->type==TYPE_EXCEPTION)
        {
            return *(int*)_op1->data == *(int*)_op2->data;
        }
        else
        {
            return false;
        }
    }

    else if (_op1->type == TYPE_FONCTION)
    {
        if (_op2->type == TYPE_FONCTION)
        {
            Function* f1=_op1->data;
            Function* f2=_op2->data;
            return f1->ptr==f2->ptr;
        }
        else
        {
            return false;
        }
    }

    else if (_op1->type == TYPE_USERFUNC || _op1->type == TYPE_USERMETHOD)
    {
        if (_op2->type == TYPE_USERFUNC || _op2->type == TYPE_USERMETHOD)
        {
            UserFunc* f1=_op1->data;
            UserFunc* f2=_op2->data;
            return f1->code==f2->code;
        }
        else
        {
            return false;
        }
    }

    else if (_op1->type == TYPE_LIST)
    {
        if (_op2->type == TYPE_LIST)
        {
            return nelist_equal(_op1->data, _op2->data);
        }
        else
            return false;
    }

    else if (_op1 -> type == TYPE_CONTAINER)
    {
        if (_op2 -> type == TYPE_CONTAINER)
        {
            Container* c1 = _op1->data;
            Container* c2 = _op2->data;
            return c1->type == c2->type && nelist_equal(c1->data, c2->data);
        }
        else
            return false;
    }
    return false; // dans tous les autres cas
  
}






int neo_compare(NeObject* a, NeObject* b)
{
    if (a->type == TYPE_NUMBER && b->type == TYPE_NUMBER)
    {
        return number_compare(neo_to_nb(a), neo_to_nb(b));
    }
    else if (a->type == TYPE_STRING && b->type == TYPE_STRING)
    {
        return strcmp(neo_to_string(a), neo_to_string(b));
    }
    else
    {
        //erreur
        CODE_ERROR = 90;
        return 0;
    }
}


int neo_compare2(NeObject* a, NeObject* b)
{
    int c = neo_compare(b,a);
    if (CODE_ERROR != 0)
        return 0;
    else
        return c;
}




void neo_list_append(NeObject* neo, NeObject* ptr)
{
    nelist_append((NeList*)neo->data, ptr);
    return;
}




void neo_list_remove(NeObject* neo, int index)
{
    nelist_remove((NeList*)neo->data, index);
    return;
}



void neo_list_insert(NeObject* neo, NeObject* ptr,int index)
{
    nelist_insert((NeList*)neo->data, ptr, index);
    return;
}



NeObject* neo_list_nth(NeObject* neo, int index)
{
  return ((NeList*)neo->data)->tab[index];
}


int neo_list_len(NeObject* neo)
{
  return ((NeList*)neo->data)->len;
}


//nelist

NeList* nelist_create(int len)
{
  NeList* list = err_malloc(sizeof(NeList));//crée la structure
  
  list->capacity = 0;
  while (pow(2, list->capacity) < len)
    list->capacity++;
  
  list->tab=err_malloc(pow(2, list->capacity)*sizeof(NeObject*));//initialise le tableau de longueur len avec de zéros
  
  err_memset(list->tab,0,len);
  list->len=len;//initialise la bonne longueur
  return list;//retourne la structure
}



void nelist_append(NeList* list, NeObject* ptr)//ajoute un élément à la fin de la liste
{
    NeObject** tmp;

    if (pow(2, list->capacity)==list->len)
    {
        list->capacity++;
        tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(NeObject*));//réallocation de list.tab
        list->tab = tmp;//affectation du pointeur de tmp vers list.tab
    }

    list->tab[list->len]=ptr;//affecte ptr au dernier élément
    list->len++;//incrémente la longueur
}






void nelist_insert(NeList* list,NeObject* neo, int index)//ajoute un élément à la place indiquée
{
  if (index > list->len)
  {
        CODE_ERROR = 37;//out of range
        return ;
  }
  
  NeObject** tmp;
  
  if (pow(2, list->capacity)==list->len)
  {
    list->capacity++;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(NeObject*));//réallocation de list.tab
    list->tab = tmp;//affectation du pointeur de tmp vers list.tab
  }
  
  for (int i = list->len ; i > index; i--)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i-1];
  
  list->tab[index]=neo;

  list->len++;//incrémente la longueur
}






void nelist_remove(NeList* list,int index)
{


  if (index >= list->len)
  {
        CODE_ERROR = 38;//out of range
        return ;
  }
  
  
  
  neobject_destroy(list->tab[index],true);
  
  for (int i = index ; i < list->len -1; i++)//décale tous les éléments à partir de celui à supprimer
    list->tab[i]=list->tab[i+1];
    
  NeObject** tmp;
  
  if (pow(2, list->capacity-1)==list->len-1)
  {
    list->capacity--;
    tmp = err_realloc(list->tab, pow(2, list->capacity)*sizeof(NeObject*));//réalloue un nouveau pointeur de la bonne taille
    list->tab = tmp;
  }
  
  
  list->len--;
  
}




int nelist_index(NeList* liste, NeObject* neo)
{
    
    for (int i=0 ; i < liste->len ; i++)
    {
        if (liste->tab[i] == neo)
        {
            return i;
        }
    }
    CODE_ERROR = 39; // cet objet n'existe pas
    
    return -1;
}






int nelist_index2(NeList* l, NeObject* neo)
{
    for (int i = 0; i < l->len ; i++)
    {
        if (neo_equal(neo, l->tab[i]))
            return i;
    }
    CODE_ERROR = 88;
    return -1;
}






void nelist_aff(NeList* liste)
{
  if (liste->len == 0)
  {
      printString("[]");
  }
  else
  {
    printString("[");
    for (int i=0;i<liste->len-1;i++)
    {
      neobject_aff(liste->tab[i]);
      printString(", ");
    }
    neobject_aff(liste->tab[liste->len-1]);
    printString("]");
  }
}



char* nelist_str(NeList* list)
{
    if (list->len == 0)
    {
        return strdup("[]");
    }
    else
    {
        char* str1 = strdup("["), * str2, *temp;
        for (int i=0 ; i < list->len - 1 ; i++)
        {
            temp = neobject_str(list->tab[i]);
            str2 = addStr(str1,temp);
            err_free(temp);err_free(str1);
            str1 = addStr(str2,", ");
            err_free(str2);
        }
        temp = neobject_str(list->tab[list->len - 1]);
        str2 = addStr(str1,temp);
        err_free(str1);err_free(temp);
        str1 = addStr(str2, "]");
        err_free(str2);
        return str1;
    }
    return strdup("Erreur17");
}






void nelist_destroy(NeList* list,_Bool bo)
{
  for (int i=0;i<list->len;i++)
  {
      neobject_destroy(list->tab[i],true);
  }
  err_free(list->tab);

  if (bo)
    err_free(list);
}


char* type(NeObject* neo)
{
    if (neo->type == TYPE_BOOL)
        return "Booleen";

    if (neo->type == TYPE_STRING)
        return "Chaine";

    if (neo->type == TYPE_NUMBER)
        return "Nombre";

    if (neo->type == TYPE_FONCTION)
        return "Fonction de base";

    if (neo->type == TYPE_LIST)
        return "Liste";

    if (neo->type == TYPE_USERFUNC)
        return "Fonction";

    if (neo->type == TYPE_USERMETHOD)
        return "Methode";

    if (neo->type == TYPE_EXCEPTION)
        return "Erreur";
    
    if (neo->type == TYPE_CONTAINER)
    {
        Container* c = (Container*)neo->data;
        return CONTAINERS->tab[c->type];
    }

    if (neo->type == -1)
        return "N'importe quel type";

    return "Rien";

}


_Bool nelist_inList(NeList* list, NeObject* neo)
{
    _Bool bo = false;
    
    for (int i = 0 ; i < list->len ; i++)
    {
        if (neo_equal(list->tab[i], neo))
        {
            bo = true;
            break;
        }
    }

    return bo;
}


NeList* nelist_copy(NeList* l)
{
    //copie de deux listes, partie récursive de la fonction
    
    NeList* liste = nelist_create(l->len); //nouvelle liste
    for (int i=0 ; i < l->len ; i++)
    {
        liste->tab[i] = neo_copy(l->tab[i]); // on ajoute à la liste liste une copie de l'élément i de la liste neo
    }
    return liste;
}
