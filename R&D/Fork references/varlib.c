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


*/



NeObject* neobject_create(char type)
{
    NeObject* neo = err_malloc(sizeof(NeObject));
    neo->data = NULL;
    neo->type = type;
    neo->refc = NULL;
    
    return neo;
}


void incr_count(NeObject* neo)
{
    *(neo->refc) = *(neo->refc) + 1;
}

void decr_count(NeObject* neo)
{
    *(neo->refc) = *(neo->refc) - 1;
}

void init_count(NeObject* neo)
{
    neo->refc = err_malloc(sizeof(unsigned));
    *(neo->refc) = 1;
}


/*
void update_count(NeObject* neo, unigned* refc) // change l'adresse du compteur en libérant le pointeur précédent
{
    if (neo->refc != NULL)
        err_free(neo->refc)

    neo->refc = refc;
}
*/


NeObject* neo_container_create(int type, NeList* data)
{
    Container* c = err_malloc(sizeof(Container));
    c->type = type;
    c->data = data;

    NeObject* neo = neobject_create(TYPE_CONTAINER);
    init_count(neo); // initialise le compteur de références de l'objet à 1
    neo->data = (void*)c;
    return neo;
}



NeObject* neo_nb_create(Number number)
{
    NeObject* neo = neobject_create(TYPE_NUMBER);
    init_count(neo);
    neo->data = err_malloc(sizeof(Number));
    *((Number*)neo->data) = number;
    return neo;
}



NeObject* neo_bool_create(_Bool boolean)
{
    NeObject* neo = neobject_create(TYPE_BOOL);
    init_count(neo);
    neo->data = err_malloc(sizeof(_Bool*));
    *((_Bool*)neo->data) = boolean;
    return neo;
}


_Bool neo_is_true(NeObject* neo)
{
    return *(_Bool*)neo->data;
}


NeObject* neo_fun_create(int nbArgsMeth, NeObject * (*ptr)(NeList *), const char* help, int nbArgs, const int* typeArgs, int typeRetour)
{
    NeObject* neo = neobject_create(TYPE_FONCTION);
    init_count(neo);
    neo->data = function_create(nbArgsMeth,ptr, help, nbArgs, typeArgs, typeRetour);
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
    init_count(neo);
    neo->data = (void*)fun;
    
    return neo;
}








NeObject* neo_str_create(char* string) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObject* neo = neobject_create(TYPE_STRING);
    init_count(neo);
    neo->data = string;
    return neo;
}



NeObject* neo_exception_create(int index)
{
    NeObject* neo = neobject_create(TYPE_EXCEPTION);
    init_count(neo);
    // détermine la place de la chaine dans le tableau exception
    neo->data = err_malloc(sizeof(int));
    *(int*)neo->data = index;
    return neo;
}




NeObject* neo_const_create(char* string) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObject* neo = neobject_create(TYPE_CONST);
    init_count(neo);
    neo->data = string;
    return neo;
}



NeObject* neo_none_create(void) // attention, la chaine de caractères passée en argument va être mise dans le NeObject directement sans être copiée. Donc elle doit être dans le tas, et ne pas être libérée par l'extérieur
{
    NeObject* neo = neobject_create(TYPE_NONE);
    init_count(neo);
    return neo;
}




NeObject* neo_copy(NeObject* neo)
{
    NeObject* neo2 = neobject_create(neo->type);
    neo2->data = neo->data;
    neo2->refc = neo->refc; // le compteur de références est le même car c'est le même neo->data. Ainsi si un autre est libéré, ce compteur sera lui aussi modifié
    incr_count(neo); // augmente le compteur pour toutes les variables
    return neo2;
}


NeObject* neo_dup(NeObject* neo)
{
    void** writeAdress = NULL, *identicalAdress = NULL;
    return neo_dup_aux(neo, &writeAdress, &identicalAdress);
}






NeObject* neo_dup_aux(NeObject* neo, void*** writeAdress, void** identicalAdress)
{
    /*
    EXPLICATION DU FONCTIONNEMENT DE CETTE FONCTION :
    On copie l'objet récursivement, et lorsque l'on rencontre un pointeur pour la deuxième fois, on retourne l'adresse d'écriture, et l'endroit qui avait rencontré le pointeur pour la première fois va écrire à la bonne adresse
    
    */

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
        unsigned sov = *(neo->refc);
        *(neo->refc) = (unsigned)-1;

        NeList* cop = nelist_copy_aux((NeList*)neo->data, writeAdress, identicalAdress);

        NeObject* neo2 = neo_list_convert(cop);


        if (*identicalAdress == (void*)neo->data) // la fonction de copie a rencontré nous et elle savait pas quel pointeur mettre
        {
            **writeAdress = (void*)neo_copy(neo2); // on a modifié le dernier pointeur
            *identicalAdress = NULL;
        }
        // sinon, le contexte précédent recevra toujours writeAdress et identicalAdress

        *(neo->refc) = sov;

        return neo2;
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

        unsigned sov = *(neo->refc);
        *(neo->refc) = (unsigned)-1;

        NeList* cop = nelist_copy_aux(c->data, writeAdress, identicalAdress);

        NeObject* neo2 = neo_container_create(c->type, cop);

        if (*identicalAdress == (void*)neo->data) // la fonction de copie a rencontré nous et elle savait pas quel pointeur mettre
        {
            **writeAdress = (void*)neo_copy(neo2); // on a modifié le dernier pointeur
            *identicalAdress = NULL;
        }
        // sinon, le contexte précédent recevra toujours writeAdress et identicalAdress

        *(neo->refc) = sov;

        return neo2;
    }
    else if (neo->type == TYPE_EMPTY)
    {
        NeObject* neo = neobject_create(TYPE_EMPTY);
        init_count(neo);
        return neo;
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
    NeObject* neo = neobject_create(TYPE_LIST);
    init_count(neo);
    neo->data = nelist_create(len);
    return neo;
}




NeObject* neo_list_convert(NeList* list)
{
    NeObject* neo = neobject_create(TYPE_LIST);
    init_count(neo);
    neo->data = list;
    return neo;
}





_Bool neobject_destroy(NeObject* neo, _Bool bo) // renvoie si l'objet pointait sur le même NeObject* que lui-même
{
    return neobject_destroy_aux(neo, bo, neo, true);
}



// il faut pas que vérifier que l'objet pointe pas sur lui-même, mais aussi qu'un objet plus large pointe pas dessus, car sinon on lui demandera


_Bool neobject_destroy_aux(NeObject* neo, _Bool bo, NeObject* baseObj, _Bool externCall) // décrémente le pointeur, supprime éventuellement tout l'objet et supprime éventuellement l'enveloppe
{
    _Bool idref = false;
    if (neo != NULL && neo->type != TYPE_EMPTY && neo->refc != NULL && *(neo->refc) != (unsigned)-1) // pour pouvoir libérer correctement les objets se référant à eux mêmes
    {
        int autoref = neobject_autoref(neo, &idref);
        // les autoref fonctionnent lorsqu'un objet pointe sur lui-même tout court : on peut y accéder par les variables ou par lui-même
        // lorsqu'un objet se contient lui-même mais à travers un objet accessible de l'extérieur, il faudrait aussi supprimer cette référence extérieure

        // la première condition est là pour éviter de parcourir des objets de ADRESSES déjà libérés lorsque l'appel à neobject_destroy est fait lors de la libération finale de ADRESSES
        if (neo->refc != NULL && (*(neo->refc) == 1 || *(neo->refc) - autoref == 1)) // c'est le moment de réellement supprimer l'objet : on était le dernier endroit à encore pointer dessus
        {
            // expérimental
            for (int k = 0 ; k < ADRESSES->len ; k++)
            {
                if (ADRESSES->tab[k] != NULL && ADRESSES->tab[k] != neo && neo->refc == ADRESSES->tab[k]->refc) // on peut accéder à cet objet par l'extérieur, on supprime donc ce trajet
                {
                    ADRESSES->tab[k]->data = NULL; // on supprime pas vraiment car c'est le même data que neo, et on va supprimer neo maintenant
                    ADRESSES->tab[k]->refc = NULL;
                    ADRESSES->tab[k]->type = TYPE_EMPTY;
                }
            }
            
            if (neo->data != NULL) // on supprime neo->data
            {
                if (neo->type == TYPE_NUMBER)
                {
                    number_destroy(neo_to_nb(neo));
                    err_free(neo->data);
                }
                else if (neo->type == TYPE_LIST)
                {
                    unsigned sov = *(neo->refc);
                    *(neo->refc) = (unsigned)-1;
                    nelist_destroy_aux((NeList*)neo->data,true, baseObj); // la fonction a supprimé nous, donc pas le choix : il faut tester si neo->refc existe toujours avant de vouloir le modifier
                    *(neo->refc) = sov;
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
                    unsigned sov = *(neo->refc);
                    *(neo->refc) = (unsigned)-1;
                    nelist_destroy_aux(c->data, true, baseObj);

                    *(neo->refc) = sov;
                    err_free(c);
                }
                else if (neo->data != NULL)
                {
                    err_free(neo->data);
                }

                neo->data = NULL;
            }

            err_free(neo->refc); // le seul moment où l'on libère le compteur de références est le moment où plus aucun objet ne pointe dessus
        }
        else if (neo->refc != NULL)
        {
            decr_count(neo);
        }
    }

    // ce truc s'exécute quoi qu'il arrive, même si on reconnait qu'on est sur une même variable qu'avant car on rappelle que même si les refc sont identiques, les enveloppes sont TOUJOURS différentes
    if (bo && neo != NULL && (neo != baseObj || externCall)) // par contre peu importe la valeur de refc, l'enveloppe est systématiquement supprimée (sauf si elle est réutilisée)
    {       // si l'objet existe ailleurs, refc est conservé, sinon il aura été libéré
        err_free(neo);
    }
    
    return idref;
}










void neobject_aff(NeObject* neo)
{
    if (neo->refc != NULL && *(neo->refc) == (unsigned)-1)
    {
        setColor(RED);
        printString(" * ");
        setColor(WHITE);
        return;
    }

    // quand on affiche un objet on l'affiche et juste avant l'appel récursif on met *(neo->refc) à -1
    if (neo->type == TYPE_NUMBER)
    {
        number_aff(neo_to_nb(neo));
    }
    else if (neo->type == TYPE_BOOL)
    {
        if (*((_Bool*)neo->data))
            printString("True");
        else
            printString("False");
    }
    else if (neo->type == TYPE_STRING)
    {
        printString("\"");
        printString((char*)neo->data);
        printString("\"");
    }
    else if (neo->type == TYPE_LIST)
    {
        unsigned sov = *(neo->refc); // sauvegarde de neo->refc pour le mettre à -1 pour l'affichage récursif
        // on se prépare à l'appel récursif
        *(neo->refc) = (unsigned)-1;

        nelist_aff((NeList*)neo->data);

        *(neo->refc) = sov;
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
        printString("None");
    }
    else if (neo->type == TYPE_FONCTION)
    {
        Function* fun = neo->data;
        
        if (fun->nbArgsMeth == 0 && fun->type == TYPE_BUILTINFUNC)
            printString("<built-in function>");
        else if (fun->nbArgsMeth != 0 && fun->type == TYPE_BUILTINFUNC)
            printString("<built-in method>");
    }

    else if (neo->type == TYPE_USERFUNC)
    {
        printString("<function>");
    }

    else if (neo->type == TYPE_USERMETHOD)
    {
        printString("<method>");
    }
    else if (neo->type == TYPE_CONTAINER)
    {
        unsigned sov = *(neo->refc); // sauvegarde de neo->refc

        Container* c = (Container*)neo->data;
        NeList* list = (NeList*)(ATTRIBUTES->tab[c->type]->data);

        printString(CONTAINERS->tab[c->type]);
        printString("(");
        for (int i=0 ; i < c->data->len ; i++)
        {
            printString(neo_to_string(list->tab[i]));
            printString(": ");
            // on se prépare avant l'appel récursif
            *(neo->refc) = (unsigned)-1;

            neobject_aff(c->data->tab[i]);
            if (i < c->data->len - 1)
                printString(", ");
        }
        printString(")");

        *(neo->refc) = sov;
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
            return strdup("True");
        else
            return strdup("False");
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
        return strdup("None");
    }
    else if (neo->type == TYPE_FONCTION)
    {        
        Function* fun = neo->data;

        if (fun->nbArgsMeth == 0 && fun->type == TYPE_BUILTINFUNC)
            return strdup("<built-in function>");
        else if (fun->nbArgsMeth != 0 && fun->type == TYPE_BUILTINFUNC)
            return strdup("<built-in method>");
    }

    else if (neo->type == TYPE_USERFUNC)
    {
        return strdup("<function>");
    }

    else if (neo->type == TYPE_USERMETHOD)
    {
        return strdup("<method>");
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


_Bool nelist_equal(NeList* l1, NeList* l2) // pas de stack overflow en parcourant une listes infinies
{
    _Bool bo = l1->len==l2->len;
    for (int i=0 ; bo && i < l1->len ; i++)
    {
        bo = bo && neo_equal(l1->tab[i], l2->tab[i]);
    }
    
    return bo;
}



_Bool neo_equal(NeObject* _op1, NeObject* _op2)
{
    if (*(_op1->refc) == (unsigned)-1 && *(_op2->refc) == (unsigned)-1) // on a déjà comparé ces deux-là
        return true;

    else if (_op1->type != _op2->type)
    {
        return false;
    }
    
    else if (_op1->type==TYPE_NUMBER)
    {
        return number_equal(neo_to_nb(_op1), neo_to_nb(_op2));
    }

    else if (_op1->type==TYPE_STRING)
    {
        return strcmp(neo_to_string(_op1),neo_to_string(_op2)) == 0;
    }

    else if (_op1->type==TYPE_BOOL)
    {
        return neo_to_bool(_op1)==neo_to_bool(_op2);
    }
  
    else if (_op1->type==TYPE_NONE)
        return true;
  
    else if (_op1->type==TYPE_EXCEPTION)
    {
        return *(int*)_op1->data == *(int*)_op2->data;
    }

    else if (_op1->type == TYPE_FONCTION)
    {
        Function* f1=_op1->data;
        Function* f2=_op2->data;

        return f1->ptr==f2->ptr;
    }

    else if (_op1->type == TYPE_USERFUNC || _op1->type == TYPE_USERMETHOD)
    {

        UserFunc* f1=_op1->data;
        UserFunc* f2=_op2->data;

        return f1->code==f2->code;
    }

    else if (_op1->type == TYPE_LIST)
    {
        unsigned sov1 = *(_op1->refc);
        unsigned sov2 = *(_op2->refc);
        *(_op1->refc) = (unsigned)-1;
        *(_op2->refc) = (unsigned)-1;

        _Bool eq = nelist_equal(_op1->data, _op2->data);

        *(_op1->refc) = sov1;
        *(_op2->refc) = sov2;

        return eq;
    }

    else if (_op1 -> type == TYPE_CONTAINER)
    {
        Container* c1 = _op1->data;
        Container* c2 = _op2->data;

        unsigned sov1 = *(_op1->refc);
        unsigned sov2 = *(_op2->refc);
        *(_op1->refc) = (unsigned)-1;
        *(_op2->refc) = (unsigned)-1;

        _Bool eq = (c1->type == c2->type && nelist_equal(c1->data, c2->data));

        *(_op1->refc) = sov1;
        *(_op2->refc) = sov2;

        return eq;
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






void nelist_destroy_aux(NeList* list,_Bool bo, NeObject* baseObj)
{
    for (int i=0;i<list->len;i++)
    {
        neobject_destroy_aux(list->tab[i],true, baseObj, false);
        list->tab[i] = NULL; // pour que quand on libère ADRESSES à la fin, la fonction neobject_destroy n'aille pas refouiller dans ADRESSES
    }
    err_free(list->tab);

    if (bo)
    {
        err_free(list);
    }
}




void nelist_destroy(NeList* list, _Bool bo)
{
    nelist_destroy_aux(list, bo, NULL);
}




char* type(NeObject* neo)
{
    if (neo->type == TYPE_BOOL)
        return "Bool";

    if (neo->type == TYPE_STRING)
        return "String";

    if (neo->type == TYPE_NUMBER)
        return "Number";

    if (neo->type == TYPE_FONCTION)
        return "Built-in function";

    if (neo->type == TYPE_LIST)
        return "List";

    if (neo->type == TYPE_USERFUNC)
        return "Function";

    if (neo->type == TYPE_USERMETHOD)
        return "Method";

    if (neo->type == TYPE_EXCEPTION)
        return "Exception";
    
    if (neo->type == TYPE_CONTAINER)
    {
        Container* c = (Container*)neo->data;
        return CONTAINERS->tab[c->type];
    }

    if (neo->type == -1)
        return "any type";

    return "NoneType";

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
    void** writeAdress = NULL, *identicalAdress = NULL;
    return nelist_copy_aux(l, &writeAdress, &identicalAdress);
}





NeList* nelist_copy_aux(NeList* l, void *** writeAdress, void** identicalAdress)
{
    //copie de deux listes, partie récursive de la fonction
    void ** wa2 = NULL , *ia2 = NULL;
    
    NeList* liste = nelist_create(l->len); //nouvelle liste
    for (int i=0 ; i < l->len ; i++)
    {
        if (*(l->tab[i]->refc) == (unsigned)-1 && wa2 == NULL && ia2 == NULL) // on a déjà copié cet objet
        {
            wa2 = (void*)&liste->tab[i]; // le précédent contexte (ayant déjà copié l->tab[i] écrira ici)
            ia2 = (void*)l->tab[i]->data;// on prend comme référence de comparaison le champ data
            *(l->tab[i]->refc) = (unsigned)-2; // comme ça on ne reviendra pas ici si on retrouve ce pointeur
        }
        else
        {
            liste->tab[i] = neo_dup_aux(l->tab[i], writeAdress, identicalAdress); // on ajoute à la liste liste une copie de l'élément i de la liste neo
            
            if (wa2 == NULL && ia2 == NULL && *writeAdress != NULL && *identicalAdress != NULL) // premièr appel récursif dans lequel on a rencontré un pointeur déjà rencontré
            {
                wa2 = *writeAdress ; ia2 = *identicalAdress;
            }
        }
    }

    *writeAdress = wa2;
    *identicalAdress = ia2;

    return liste;
}





int neobject_autoref_aux(NeObject* neo, NeObject* comp, _Bool* idref) // fonction auxiliaire qui ne modifie pas neo->refc
{
    if (neo == comp && idref != NULL) // on a rencontré le pointeur de base identique
        *idref = true;


    if (neo == NULL || neo->type == TYPE_EMPTY || neo->refc == NULL || neo->data == NULL)
        return 0;
    else if (*(neo->refc) == (unsigned)-2) // cas de base, attention pour combiner des calculs d'autoref et des neobject_destroy, il faut pas la même marque pour les deux
        return 1;
    else if (*(neo->refc) == (unsigned)-3) // on est déjà passé par ce pointeur mais ce n'est pas le pointeur recherché
        return 0;
    else // on n'est jamais passé par là
    {
        unsigned sov = *(neo->refc);
        *(neo->refc) = (unsigned)-3; // pour savoir quand on revient à un pointeur déjà parcouru qui n'est pas le pointeur recherché

        int somme = 0;

        if (neo->type == TYPE_LIST) // on additionne les autoref de chacun des indices de liste
        {
            NeList* list = (NeList*)neo->data;
            for (int i = 0 ; i < list->len ; i++)
            {
                somme += neobject_autoref_aux(list->tab[i], comp, idref);
            }
        }
        else if (neo->type == TYPE_CONTAINER) // on additionne les autoref de chacun des champs
        {
            Container* c = (Container*)neo->data;
            for (int i = 0 ; i < c->data->len ; i++)
            {
                somme += neobject_autoref_aux(c->data->tab[i], comp, idref);
            }
        }
        

        *(neo->refc) = sov;

        return somme;
    }
}



int neobject_autoref(NeObject* neo, _Bool* idref) // fonction qui calcule combien de fois neo fait-il référence à lui-même
{
    // le deuxième argument va permettre d'également calculer si l'objet fait directement référence à lui-même
    unsigned sov = *(neo->refc);
    *(neo->refc) = (unsigned)-2; // pour savoir quand on revient à nous-même
    
    int somme = 0;

    if (neo->type == TYPE_LIST) // on additionne les autoref de chacun des indices de liste
    {
        NeList* list = (NeList*)neo->data;
        for (int i = 0 ; i < list->len ; i++)
        {
            somme += neobject_autoref_aux(list->tab[i], neo, idref);
        }
    }
    else if (neo->type == TYPE_CONTAINER) // on additionne les autoref de chacun des champs
    {
        Container* c = (Container*)neo->data;
        for (int i = 0 ; i < c->data->len ; i++)
        {
            somme += neobject_autoref_aux(c->data->tab[i], neo, idref); // envoi sur une structure non initialisée correctement
        }
    }

    *(neo->refc) = sov;
    return somme;
}