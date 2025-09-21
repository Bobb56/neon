#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


#include "headers.c"

extern int CODE_ERROR;


extern ptrlist* VAR_LOC;


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





//definition des fonctions attribuees aux operateurs

NeObject* _add(NeObject* _op1, NeObject* _op2)
{
  
  
  if (_op1->type==TYPE_NUMBER)
  {
    if (_op2->type==TYPE_NUMBER)
    {
      return neo_nb_create(number_add(neo_to_nb(_op1),neo_to_nb(_op2)));
    }
    else
    {
      // Erreur : types non supportes pour l addition
      CODE_ERROR = 40;
      return NULL;
    }
  }
  else if (_op1->type==TYPE_STRING)
  {
    if (_op2->type==TYPE_STRING)
    {
      return neo_str_create(addStr(neo_to_string(_op1),neo_to_string(_op2)));
    }
    else
    {
      // Erreur : types non supportes pour l addition
      CODE_ERROR = 40;
      return NULL;
    }
  }
  else if (_op1->type == TYPE_LIST)
  {
      if (_op2->type == TYPE_LIST)
      {
          // fusion de deux listes
          NeList* list = nelist_create(neo_list_len(_op1) + neo_list_len(_op2));
          for (int i = 0 ; i < neo_list_len(_op1) ; i++)
              list->tab[i] = neo_copy(neo_list_nth(_op1, i));

          for (int i = neo_list_len(_op1) ; i < list->len ; i++)
              list->tab[i] = neo_copy(neo_list_nth(_op2, i - neo_list_len(_op1)));

          NeObject* neo = neobject_create(TYPE_LIST);
          neo->data = list;
          return neo;
      }
      else
      {
        // Erreur : types non supportes pour l addition
        CODE_ERROR = 40;
        return NULL;
      }
  }
  else
  {
    // Erreur : types non supportes pour l addition
    CODE_ERROR = 40;
    return NULL;
  }
  return NULL;
}






NeObject* _sub(NeObject* _op1, NeObject* _op2)
{
  if (_op1->type==TYPE_NUMBER && _op2->type==TYPE_NUMBER)
  {
    return neo_nb_create(number_sub(neo_to_nb(_op1), neo_to_nb(_op2)));
  }
  else if (_op1->type==TYPE_STRING && _op2->type==TYPE_NUMBER)
  {
    // on enlève le nombre de caractères à la fin de la chaîne
    char* retour = sub(neo_to_string(_op1), 0, strlen(neo_to_string(_op1)) - (unsigned long)number_toDouble(neo_to_nb(_op2)));
    if (CODE_ERROR != 0)
        return NULL;
    
    return neo_str_create(retour);
  }
  else if (_op1->type==TYPE_STRING && _op2->type==TYPE_STRING)
  {

    return neo_str_create(replace(neo_to_string(_op1),neo_to_string(_op2),""));
    
  }
  else
  {
    // Erreur : types non supportes pour la soustraction
    CODE_ERROR = 41;
    return NULL;
  }
  return NULL;
}






NeObject* _div(NeObject* _op1, NeObject* _op2)
{
  if (_op1->type==TYPE_NUMBER && _op2->type==TYPE_NUMBER)
  {
    return neo_nb_create(number_div(neo_to_nb(_op1), neo_to_nb(_op2)));
  }
  else
  {
    // Erreur : types non supportes pour la division
    CODE_ERROR = 42;
    return NULL;
  }
  return NULL;
}






NeObject* _mul(NeObject* _op1, NeObject* _op2)
{
  if (_op1->type==TYPE_NUMBER && _op2->type==TYPE_NUMBER)
  {
    return neo_nb_create(number_mul(neo_to_nb(_op1), neo_to_nb(_op2)));
  }
  else if ((_op1->type==TYPE_NUMBER && _op2->type==TYPE_STRING) || (_op1->type==TYPE_STRING && _op2->type==TYPE_NUMBER))
  {
    char* multip;char* tmp;char* sousch;
    int times=0;
    if (_op1->type==TYPE_STRING)
    {
      sousch=neo_to_string(_op1);
      multip=strdup(sousch);
      times=(long int)number_toDouble(neo_to_nb(_op2));
    }
    else
    {
      sousch=neo_to_string(_op2);
      multip=strdup(sousch);
      times=(long int)number_toDouble(neo_to_nb(_op1));
    }
    

    for (int i=1;i<times;i++)
    {
      tmp=addStr(multip,sousch);
      err_free(multip);
      multip=tmp;
    }
    


    return neo_str_create(multip);
  }
  else if ((_op1->type==TYPE_NUMBER && _op2->type==TYPE_LIST) || (_op2->type==TYPE_NUMBER && _op1->type==TYPE_LIST)) // multiplication de liste
  {
    long int times;
    NeList* list;
    if (_op2->type==TYPE_NUMBER)
    {
        times = (long int) number_toDouble(*(Number*)_op2->data);
        list=_op1->data;
    }
    else
    {
        times = (long int) number_toDouble(*(Number*)_op1->data);
        list=_op2->data;
    }
    
    // on va faire list * times
    NeObject* neres = neo_list_create(times*list->len);
    NeList* res=neres->data;
    
    for (int i=0 ; i < times ; i++)
    {
    	for (int j=0 ; j < list->len ; j++)
        	res->tab[i*list->len + j] = neo_copy(list->tab[j]);
    }
    
    return neres;
  }
  else
  {
    CODE_ERROR = 43; //unsupported types for mul
    return NULL;
  }
  return NULL;
}





NeObject* _mod(NeObject* _op1, NeObject* _op2)
{
  if (_op1->type==TYPE_NUMBER && _op2->type==TYPE_NUMBER)
  {
    if (number_toDouble(neo_to_nb(_op2)) == 0)
    {
        CODE_ERROR = 44;
        return NULL;
    }
    return neo_nb_create(number_mod(neo_to_nb(_op1), neo_to_nb(_op2)));
  }
  else
  {
    CODE_ERROR = 44; // unsupported types
    return NULL;
  }
  return NULL;
}





NeObject* _eucl(NeObject* _op1, NeObject* _op2)
{

    if (_op2->type == TYPE_NUMBER && number_toDouble(neo_to_nb(_op2)) == 0)
    {
        CODE_ERROR = 45;
        return NULL;
    }
    
  if (_op1->type==TYPE_NUMBER && _op2->type==TYPE_NUMBER)
  {
    Number nb1 = neo_to_nb(_op1);
    Number nb2 = neo_to_nb(_op2);
    
    return neo_nb_create(number_eucl(nb1,nb2));
  }
  else if ((_op1->type==TYPE_STRING && _op2->type==TYPE_NUMBER) || (_op2->type==TYPE_STRING && _op1->type==TYPE_NUMBER))
  {
    char* chaine;char* sousch;
    long int times;
    if (_op1->type==TYPE_STRING)
    {
      times=(long int)number_toDouble(neo_to_nb(_op2));
      chaine=neo_to_string(_op1);
    }
    else
    {
      times=(long int)number_toDouble(neo_to_nb(_op1));
      chaine=neo_to_string(_op2);
    }
    int longueur=strlen(chaine);
    int nouvLongueur=(longueur-longueur%times)/times;
    sousch = sub(chaine,0,nouvLongueur);
    return neo_str_create(sousch);
  }
  else
  {
    CODE_ERROR = 46;
    return NULL;
  }
  return NULL;
}





NeObject* _and(NeObject* op1, NeObject* op2)
{
  if ((op1->type != TYPE_BOOL && op1->type != TYPE_NUMBER) || (op2->type != TYPE_BOOL && op2->type != TYPE_NUMBER))
  {
        CODE_ERROR = 47;
        return NULL;
  }

  _Bool nb1,nb2;
  
  nb1 = op1->type == TYPE_NUMBER ? number_toDouble(neo_to_nb(op1)) != 0 : neo_to_bool(op1);
  nb2 = op2->type == TYPE_NUMBER ? number_toDouble(neo_to_nb(op2)) != 0 : neo_to_bool(op2);
    
  return neo_bool_create(nb1 && nb2);
}





NeObject* _or(NeObject* op1, NeObject* op2)
{
  if ((op1->type != TYPE_BOOL && op1->type != TYPE_NUMBER) || (op2->type != TYPE_BOOL && op2->type != TYPE_NUMBER))
  {
      CODE_ERROR = 48;
      return NULL;
  }
  
  _Bool nb1,nb2;
  nb1 = op1->type == TYPE_NUMBER ? number_toDouble(neo_to_nb(op1)) != 0 : neo_to_bool(op1);
  nb2 = op2->type == TYPE_NUMBER ? number_toDouble(neo_to_nb(op2)) != 0 : neo_to_bool(op2);
    
  return neo_bool_create(nb1 || nb2);
}






NeObject* _xor(NeObject* op1, NeObject* op2)
{
  if ((op1->type != TYPE_BOOL && op1->type != TYPE_NUMBER) || (op2->type != TYPE_BOOL && op2->type != TYPE_NUMBER))
  {
      CODE_ERROR = 49;
      return NULL;
  }
  
  _Bool nb1,nb2;
  nb1 = op1->type == TYPE_NUMBER ? number_toDouble(neo_to_nb(op1)) != 0 : neo_to_bool(op1);
  nb2 = op2->type == TYPE_NUMBER ? number_toDouble(neo_to_nb(op2)) != 0 : neo_to_bool(op2);
    
  return neo_bool_create(nb1 ^ nb2);
}




NeObject* _pow(NeObject* _op1, NeObject* _op2)
{
  if (_op1->type==TYPE_NUMBER && _op2->type==TYPE_NUMBER)
  {
    return neo_nb_create(number_pow(neo_to_nb(_op1),neo_to_nb(_op2)));
  }
  else
  {
      CODE_ERROR = 50;
      return NULL;
  }
  return NULL;
}



NeObject *_equal(NeObject* op1, NeObject* op2)
{
    return neo_bool_create(neo_equal(op1,op2));
}




NeObject* _notEq(NeObject* _op1, NeObject* _op2)
{
    return neo_bool_create(!neo_equal(_op1,_op2));
}







NeObject* _infEqual(NeObject* op1, NeObject* op2)
{
    _Bool res = neo_compare(op1, op2) <= 0;
    if (CODE_ERROR != 0)
    {
        CODE_ERROR = 52;
        return NULL;
    }
    
    return neo_bool_create(res);
}





NeObject* _supEqual(NeObject* op1, NeObject* op2)
{
    _Bool res = neo_compare(op1, op2) >= 0;
    if (CODE_ERROR != 0)
    {
        CODE_ERROR = 53;
        return NULL;
    }
    
    return neo_bool_create(res);
}






NeObject* _inf(NeObject* op1, NeObject* op2)
{
    _Bool res = neo_compare(op1, op2) < 0;
    if (CODE_ERROR != 0)
    {
        CODE_ERROR = 54;
        return NULL;
    }
    
    return neo_bool_create(res);
}





NeObject* _sup(NeObject* op1, NeObject* op2)
{
    _Bool res = neo_compare(op1, op2) > 0;
    if (CODE_ERROR != 0)
    {
        CODE_ERROR = 55;
        return NULL;
    }
    
    return neo_bool_create(res);
}




NeObject* _affect(NeObject* op2, NeObject* op1)
{
    // ici, on affecte à une variable qui existe déjà forcément, mais n'a peut-être pas de valeur
    // op1 est un NeObject, on va modifier directement le NeObject, via son pointeur
    // op1 représente le nom de la variable et op2 la valeur à affecter
    //neobject_destroy(op1);
    if (op2->type == TYPE_EMPTY)
    {
        CODE_ERROR = 56; // unedfined var
        return NULL;
    }
    else
    {
        neobject_destroy(op1,false);
        NeObject* affect = neo_copy(op2);
        op1->type = affect->type;
        op1->data = affect->data;
        err_free(affect);
        return neo_copy(op1);
    }
    return NULL;
}







NeObject* _affectNone(NeObject* op1, NeObject* op2)
{
    // ici, on affecte à une variable qui existe déjà forcément, mais n'a peut-être pas de valeur
    // op1 est un NeObject, on va modifier directement le NeObject, via son pointeur
    // op1 représente le nom de la variable et op2 la valeur à affecter
    //neobject_destroy(op1);
    if (op2->type == TYPE_EMPTY)
    {
        CODE_ERROR = 56; // unedfined var
        return NULL;
    }
    else
    {
        neobject_destroy(op1,false);
        NeObject* affect = neo_copy(op2);
        op1->type = affect->type;
        op1->data = affect->data;
        
        err_free(affect);
        return neo_none_create();
    }
    return NULL;
}








void _affect2(NeObject* op1, NeObject* op2)
{
    // ici, on affecte à une variable qui existe déjà forcément, mais n'a peut-être pas de valeur
    // op1 est un NeObject, on va modifier directement le NeObject, via son pointeur
    // op1 représente le nom de la variable et op2 la valeur à affecter
    //neobject_destroy(op1);
    if (op2->type == TYPE_EMPTY)
    {
        CODE_ERROR = 56;//undefined var
        return ;
    }
    else
    {
        neobject_destroy(op1,false);
        NeObject* affect = neo_copy(op2);
        op1->type = affect->type;
        op1->data = affect->data;
        err_free(affect);
        return;
    }
    return;
}




NeObject* _goIn(NeObject* op2, NeObject* op1)
{

    if (op2->type == TYPE_STRING)
    {
        char* nomVar=neo_to_string(op2); // attention nomVar peut contenir un index de liste
        
        NeObject* var = NULL;
        
        if (!strlist_inList(NOMS,nomVar))
        {
            var = neobject_create(TYPE_EMPTY); // création d'une nouvelle variable
            strlist_append(NOMS,strdup(nomVar));
            nelist_append(ADRESSES, var); // variable ajoutée
        }
        else
        {
            Tree* tree = tree_create(NULL,0,0);
            createExpressionTree(tree, nomVar);
            var = evalAux(tree,false); // var contient désormais le pointeur vers le NeObject à modifier
            tree_destroy(tree);
            
        }
    
        // ADRESSES->tab[index] contient donc le NeObject dans lequel on doit mettre la valeur
        NeObject* res = _affect(op1, var);
        if (CODE_ERROR != 0)
            return NULL;
        
        return res;
    
    
    }
    else
    {
      // Erreur : Vous devez procéder comme suit valeur ->  \"nom_variable\"
      CODE_ERROR = 57;
    }

    return NULL;
}












NeObject* _addEqual(NeObject* op1, NeObject* op2)
{
    NeObject* add = _add(op1,op2);
    if (CODE_ERROR != 0)
        return NULL;
    NeObject* res = _affect(add, op1);
    if (CODE_ERROR != 0)
        return NULL;
    neobject_destroy(add,true);
    return res;
}




NeObject* _subEqual(NeObject* op1, NeObject* op2)
{
    NeObject* sub = _sub(op1,op2);
    if (CODE_ERROR != 0)
        return NULL;
    NeObject* res = _affect(sub, op1);
    if (CODE_ERROR != 0)
        return NULL;
    neobject_destroy(sub,true);
    return res;
}



NeObject* _mulEqual(NeObject* op1, NeObject* op2)
{
    NeObject* mul = _mul(op1,op2);
    if (CODE_ERROR != 0)
        return NULL;
    NeObject* res = _affect(mul, op1);
    if (CODE_ERROR != 0)
        return NULL;
    neobject_destroy(mul,true);
    return res;
}



NeObject* _divEqual(NeObject* op1, NeObject* op2)
{
    NeObject* div = _div(op1,op2);
    if (CODE_ERROR != 0)
        return NULL;
    NeObject* res = _affect(div, op1);
    if (CODE_ERROR != 0)
        return NULL;
    neobject_destroy(div,true);
    return res;
}



NeObject* _incr(NeObject* op1, NeObject* op2)
{
    NeObject* res = _addEqual(op1,op2);
    if (CODE_ERROR != 0)
        return NULL;
    return res;
}



NeObject* _decr(NeObject* op1, NeObject* op2)
{
  NeObject* res = _subEqual(op1,op2);
    if (CODE_ERROR != 0)
        return NULL;
    return res;
}




NeObject* _not(NeObject* op1, NeObject* op2)
{
  if (op1->type != TYPE_BOOL && op1->type != TYPE_NUMBER)
  {
        CODE_ERROR = 58;
        return NULL;
  }

  _Bool nb1;
  
  nb1 = op1->type == TYPE_NUMBER ? number_toDouble(neo_to_nb(op1)) != 0 : neo_to_bool(op1);
    
  return neo_bool_create(!nb1);
}





NeObject* _ref(NeObject* op1, NeObject* op2)
{
    if (op1->type == TYPE_EMPTY)
    {
        CODE_ERROR = 59;//undefined var
        return NULL;
    }
    int index = nelist_index(ADRESSES,op1);
    return neo_str_create(strdup(NOMS->tab[index]));
}




NeObject* _deref(NeObject* op1, NeObject* op2)
{
  if (op1->type != TYPE_STRING)
  {
        CODE_ERROR = 60; // ceci n'est pas une chaine de caractères
        return NULL;
  }
  char* nom=neo_to_string(op1);

  if(!strlist_inList(NOMS, nom))
  {
      CODE_ERROR = 5;
      return NULL;
  }
    
  int index = strlist_index(NOMS,nom);
  
  return neo_copy(ADRESSES->tab[index]);
}




NeObject* _minus(NeObject* op1, NeObject* op2)
{
    
    if (op1->type == TYPE_NUMBER)
    {
        return neo_nb_create(number_minus(neo_to_nb(op1)));
    }
    else
    {
        CODE_ERROR = 61; // types non supportés par l'opérateur moins (unaire)
        return NULL;
    }

    return NULL;

}

NeObject* _del(NeObject* op1, NeObject* op2)
{
    neobject_destroy(op1, false);
    op1->type = TYPE_EMPTY;
    op1->data = NULL;
    return neo_none_create();
}


NeObject* _exponent(NeObject* op1, NeObject* op2)
{
    if (op1->type != TYPE_NUMBER || op2->type != TYPE_NUMBER)
    {
        CODE_ERROR = 50;
        return NULL ;
    }
    
    Number n1 = number_fromDouble((double)10);
    Number n2 = number_pow(n1, neo_to_nb(op2));
    Number n3 = number_mul(neo_to_nb(op1), n2);
    number_destroy(n1);number_destroy(n2);
    return neo_nb_create(n3);
}


NeObject* _implique(NeObject* op1, NeObject* op2)
{
    if (op2->type != TYPE_BOOL && op1->type != TYPE_BOOL)
    {
        CODE_ERROR=75;
        return NULL;
    }
    else
    {
        return (neo_bool_create(!neo_to_bool(op1) || neo_to_bool(op2)));
    }
}


NeObject* _in(NeObject* op1, NeObject* op2)
{
    if (op2->type != TYPE_LIST)
    {
        CODE_ERROR = 79;
        return NULL;
    }
    else
    {
        return neo_bool_create(nelist_inList((NeList*)op2->data, op1));
    }
}