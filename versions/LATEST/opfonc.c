#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "headers.h"

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
extern strlist neon_boolean;
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

extern strlist* CONTAINERS;

extern NeObj (*OPFONC[NBOPERATEURS])(NeObj,NeObj);
extern NeObj (*BUILTINSFONC[NBBUILTINFONC])(NeList*);



extern int error;





//definition des fonctions attribuees aux operateurs

NeObj _add(NeObj _op1, NeObj _op2)
{
  if (NEO_TYPE(_op1) == TYPE_CONTAINER || NEO_TYPE(_op2) == TYPE_CONTAINER) {
    NeObj ret = callOverloadedBinaryOperator(_op1, _op2, "add");
    if (CODE_ERROR != 0) {
      return NEO_VOID;
    }
    return ret;
  }


  else if (NEO_TYPE(_op1)==TYPE_INTEGER && NEO_TYPE(_op2)==TYPE_INTEGER)
  {
    return neo_integer_create(neo_to_integer(_op1) + neo_to_integer(_op2));
  }


  else if (NEO_TYPE(_op1)==TYPE_DOUBLE && NEO_TYPE(_op2)==TYPE_DOUBLE)
  {
    return neo_double_create(neo_to_double(_op1) + neo_to_double(_op2));
  }
  else if (NEO_TYPE(_op1)==TYPE_INTEGER && NEO_TYPE(_op2)==TYPE_DOUBLE)
  {
    return neo_double_create((double)neo_to_integer(_op1) + neo_to_double(_op2));
  }
  else if (NEO_TYPE(_op2)==TYPE_INTEGER && NEO_TYPE(_op1)==TYPE_DOUBLE)
  {
    return neo_double_create((double)neo_to_integer(_op2) + neo_to_double(_op1));
  }



  else if (NEO_TYPE(_op1)==TYPE_STRING)
  {
    if (NEO_TYPE(_op2)==TYPE_STRING)
    {
      return neo_str_create(addStr(neo_to_string(_op1),neo_to_string(_op2)));
    }
    else
    {
      // Erreur : types non supportes pour l addition
      CODE_ERROR = 40;
      return NEO_VOID;
    }
  }
  else if (NEO_TYPE(_op1) == TYPE_LIST)
  {
      if (NEO_TYPE(_op2) == TYPE_LIST)
      {
          // fusion de deux listes
          NeList* list = nelist_create(neo_list_len(_op1) + neo_list_len(_op2));
          for (int i = 0 ; i < neo_list_len(_op1) ; i++)
              list->tab[i] = neo_copy(neo_list_nth(_op1, i));

          for (int i = neo_list_len(_op1) ; i < list->len ; i++)
              list->tab[i] = neo_copy(neo_list_nth(_op2, i - neo_list_len(_op1)));

          return neo_list_convert(list);
      }
      else
      {
        // Erreur : types non supportes pour l addition
        CODE_ERROR = 40;
        return NEO_VOID;
      }
  }
  else
  {
    // Erreur : types non supportes pour l addition
    CODE_ERROR = 40;
    return NEO_VOID;
  }
  return NEO_VOID;
}






NeObj _sub(NeObj _op1, NeObj _op2)
{
  if (NEO_TYPE(_op1) == TYPE_CONTAINER || NEO_TYPE(_op2) == TYPE_CONTAINER) {
    NeObj ret = callOverloadedBinaryOperator(_op1, _op2, "sub");
    if (CODE_ERROR != 0) {
      return NEO_VOID;
    }
    return ret;
  }
  

  else if (NEO_TYPE(_op1)==TYPE_INTEGER && NEO_TYPE(_op2)==TYPE_INTEGER)
  {
    return neo_integer_create(neo_to_integer(_op1) - neo_to_integer(_op2));
  }


  else if (NEO_TYPE(_op1)==TYPE_DOUBLE && NEO_TYPE(_op2)==TYPE_DOUBLE)
  {
    return neo_double_create(neo_to_double(_op1) - neo_to_double(_op2));
  }
  else if (NEO_TYPE(_op1)==TYPE_INTEGER && NEO_TYPE(_op2)==TYPE_DOUBLE)
  {
    return neo_double_create((double)neo_to_integer(_op1) - neo_to_double(_op2));
  }
  else if (NEO_TYPE(_op2)==TYPE_INTEGER && NEO_TYPE(_op1)==TYPE_DOUBLE)
  {
    return neo_double_create((double)neo_to_integer(_op2) - neo_to_double(_op1));
  }


  else if (NEO_TYPE(_op1)==TYPE_STRING && NEO_TYPE(_op2)==TYPE_INTEGER)
  {
    // on enlève le nombre de caractères à la fin de la chaîne
    char* retour = sub(neo_to_string(_op1), 0, strlen(neo_to_string(_op1)) - neo_to_integer(_op2));
    if (CODE_ERROR != 0)
        return NEO_VOID;
    
    return neo_str_create(retour);
  }
  else if (NEO_TYPE(_op1)==TYPE_STRING && NEO_TYPE(_op2)==TYPE_STRING)
  {

    return neo_str_create(replace(neo_to_string(_op1),neo_to_string(_op2),""));
    
  }
  else
  {
    // Erreur : types non supportes pour la soustraction
    CODE_ERROR = 41;
    return NEO_VOID;
  }
  return NEO_VOID;
}






NeObj _div(NeObj _op1, NeObj _op2)
{
  if (NEO_TYPE(_op1) == TYPE_CONTAINER || NEO_TYPE(_op2) == TYPE_CONTAINER) {
    NeObj ret = callOverloadedBinaryOperator(_op1, _op2, "div");
    if (CODE_ERROR != 0) {
      return NEO_VOID;
    }
    return ret;
  }
  else if ((NEO_TYPE(_op1)==TYPE_INTEGER || NEO_TYPE(_op1)==TYPE_DOUBLE) && (NEO_TYPE(_op2)==TYPE_INTEGER || NEO_TYPE(_op2)==TYPE_DOUBLE))
  {
    double op1 = (NEO_TYPE(_op1) == TYPE_INTEGER) ? (double)neo_to_integer(_op1) : neo_to_double(_op1);
    double op2 = (NEO_TYPE(_op2) == TYPE_INTEGER) ? (double)neo_to_integer(_op2) : neo_to_double(_op2);
    return neo_double_create(op1/op2);
  }
  else
  {
    // Erreur : types non supportes pour la division
    CODE_ERROR = 42;
    return NEO_VOID;
  }
  return NEO_VOID;
}






NeObj _mul(NeObj _op1, NeObj _op2)
{
  // gère les multiplications avec les booléens
  if (NEO_TYPE(_op1) == TYPE_CONTAINER || NEO_TYPE(_op2) == TYPE_CONTAINER) {
    NeObj ret = callOverloadedBinaryOperator(_op1, _op2, "mul");
    if (CODE_ERROR != 0) {
      return NEO_VOID;
    }
    return ret;
  }
  else if (NEO_TYPE(_op1) == TYPE_BOOL && neo_to_bool(_op1)) {
    return neo_copy(_op2);
  }
  else if (NEO_TYPE(_op2) == TYPE_BOOL && neo_to_bool(_op2)) {
    return neo_copy(_op1);
  }
  else if (NEO_TYPE(_op1) == TYPE_BOOL) { // _op1 est un False
    if (NEO_TYPE(_op2) == TYPE_LIST)
      return neo_list_create(0);
    if (NEO_TYPE(_op2) == TYPE_INTEGER)
      return neo_integer_create(0);
    if (NEO_TYPE(_op2) == TYPE_DOUBLE)
      return neo_double_create(0);
    if (NEO_TYPE(_op2) == TYPE_STRING)
      return neo_str_create(strdup(""));
  }
  else if (NEO_TYPE(_op2) == TYPE_BOOL) { // _op2 est un False
    if (NEO_TYPE(_op1) == TYPE_LIST)
      return neo_list_create(0);
    if (NEO_TYPE(_op1) == TYPE_INTEGER)
      return neo_integer_create(0);
    if (NEO_TYPE(_op1) == TYPE_DOUBLE)
      return neo_double_create(0);
    if (NEO_TYPE(_op1) == TYPE_STRING)
      return neo_str_create(strdup(""));
  }


  // multiplications avec les autres types

  else if (NEO_TYPE(_op1)==TYPE_INTEGER && NEO_TYPE(_op2)==TYPE_INTEGER)
  {
    return neo_integer_create(neo_to_integer(_op1) * neo_to_integer(_op2));
  }


  else if (NEO_TYPE(_op1)==TYPE_DOUBLE && NEO_TYPE(_op2)==TYPE_DOUBLE)
  {
    return neo_double_create(neo_to_double(_op1) * neo_to_double(_op2));
  }
  else if (NEO_TYPE(_op1)==TYPE_INTEGER && NEO_TYPE(_op2)==TYPE_DOUBLE)
  {
    return neo_double_create((double)neo_to_integer(_op1) * neo_to_double(_op2));
  }
  else if (NEO_TYPE(_op2)==TYPE_INTEGER && NEO_TYPE(_op1)==TYPE_DOUBLE)
  {
    return neo_double_create((double)neo_to_integer(_op2) * neo_to_double(_op1));
  }


  else if ((NEO_TYPE(_op1)==TYPE_INTEGER && NEO_TYPE(_op2)==TYPE_STRING) || (NEO_TYPE(_op1)==TYPE_STRING && NEO_TYPE(_op2)==TYPE_INTEGER))
  {
    char* sousch;
    int times=0;

    if (NEO_TYPE(_op1)==TYPE_STRING)
    {
      sousch=neo_to_string(_op1);
      times=neo_to_integer(_op2);
    }
    else
    {
      sousch=neo_to_string(_op2);
      times=neo_to_integer(_op1);
    }
    
    int len_sousch = strlen(sousch);
    char* multip = err_malloc(sizeof(char*)*(times*len_sousch+1));

    for (int i=0 ; i < times*len_sousch ; i++)
    {
      multip[i] = sousch[i%len_sousch];
    }
    
    multip[times*len_sousch] = '\0';

    return neo_str_create(multip);
  }
  else if ((NEO_TYPE(_op1)==TYPE_INTEGER && NEO_TYPE(_op2)==TYPE_LIST) || (NEO_TYPE(_op2)==TYPE_INTEGER && NEO_TYPE(_op1)==TYPE_LIST)) // multiplication de liste
  {
    long int times;
    NeList* list;
    if (NEO_TYPE(_op2)==TYPE_INTEGER)
    {
        times = neo_to_integer(_op2);
        list = neo_to_list(_op1);
    }
    else
    {
        times = neo_to_integer(_op1);
        list = neo_to_list(_op2);
    }
    
    // on va faire list * times
    NeObj neres = neo_list_create(times*list->len);
    NeList* res = neo_to_list(neres);
    
    for (int i=0 ; i < times*list->len ; i++)
    {
    	res->tab[i] = neo_copy(list->tab[i%list->len]);
    }
    
    return neres;
  }
  else
  {
    CODE_ERROR = 43; //unsupported types for mul
    return NEO_VOID;
  }
  return NEO_VOID;
}





NeObj _mod(NeObj _op1, NeObj _op2)
{
  if (NEO_TYPE(_op1) == TYPE_CONTAINER || NEO_TYPE(_op2) == TYPE_CONTAINER) {
    NeObj ret = callOverloadedBinaryOperator(_op1, _op2, "mod");
    if (CODE_ERROR != 0) {
      return NEO_VOID;
    }
    return ret;
  }
  else if (NEO_TYPE(_op1)==TYPE_INTEGER && NEO_TYPE(_op2)==TYPE_INTEGER)
  {
    if (neo_to_integer(_op2) == 0)
    {
        CODE_ERROR = 44;
        return NEO_VOID;
    }
    return neo_integer_create(neo_to_integer(_op1) % neo_to_integer(_op2));
  }
  else
  {
    CODE_ERROR = 44; // unsupported types
    return NEO_VOID;
  }
  return NEO_VOID;
}





NeObj _eucl(NeObj _op1, NeObj _op2)
{

  if (NEO_TYPE(_op2) == TYPE_INTEGER && neo_to_integer(_op2) == 0)
  {
      CODE_ERROR = 45;
      return NEO_VOID;
  }
  
  if (NEO_TYPE(_op1) == TYPE_CONTAINER || NEO_TYPE(_op2) == TYPE_CONTAINER) {
    NeObj ret = callOverloadedBinaryOperator(_op1, _op2, "eucl");
    if (CODE_ERROR != 0) {
      return NEO_VOID;
    }
    return ret;
  }
  else if (NEO_TYPE(_op1)==TYPE_INTEGER && NEO_TYPE(_op2)==TYPE_INTEGER)
  {
    int a = neo_to_integer(_op1);
    int b = neo_to_integer(_op2);
    return neo_integer_create((a - (a%b))/b);
  }
  else if ((NEO_TYPE(_op1)==TYPE_STRING && NEO_TYPE(_op2)==TYPE_INTEGER) || (NEO_TYPE(_op2)==TYPE_STRING && NEO_TYPE(_op1)==TYPE_INTEGER))
  {
    char* chaine;char* sousch;
    int times;
    if (NEO_TYPE(_op1)==TYPE_STRING)
    {
      times=neo_to_integer(_op2);
      chaine=neo_to_string(_op1);
    }
    else
    {
      times=neo_to_integer(_op1);
      chaine=neo_to_string(_op2);
    }
    int longueur=strlen(chaine);
    int nouvLongueur=(longueur-longueur%times)/times;
    sousch = sub(chaine,0,nouvLongueur);
    return neo_str_create(sousch);
  }
  else if (NEO_TYPE(_op1) == TYPE_CONTAINER || NEO_TYPE(_op2) == TYPE_CONTAINER) {
    NeObj ret = callOverloadedBinaryOperator(_op1, _op2, "eucl");
    if (CODE_ERROR != 0) {
      return NEO_VOID;
    }
    return ret;
  }
  else
  {
    CODE_ERROR = 46;
    return NEO_VOID;
  }
  return NEO_VOID;
}





NeObj _and(Tree* tree1, Tree* tree2)
{
  bool b = isTrue(tree1) && CODE_ERROR == 0 && isTrue(tree2);

  if (CODE_ERROR != 0)
    return NEO_VOID;
  else
    return neo_bool_create(b);
}





NeObj _or(Tree* tree1, Tree* tree2)
{
  bool b = isTrue(tree1) || CODE_ERROR != 0 || isTrue(tree2);

  if (CODE_ERROR != 0)
    return NEO_VOID;
  else
    return neo_bool_create(b);
}






NeObj _xor(NeObj op1, NeObj op2)
{
  if ((NEO_TYPE(op1) != TYPE_BOOL && NEO_TYPE(op1) != TYPE_INTEGER) || (NEO_TYPE(op2) != TYPE_BOOL && NEO_TYPE(op2) != TYPE_INTEGER))
  {
      CODE_ERROR = 49;
      return NEO_VOID;
  }
  
  bool nb1,nb2;
  nb1 = NEO_TYPE(op1) == TYPE_INTEGER ? neo_to_integer(op1) != 0 : neo_to_bool(op1);
  nb2 = NEO_TYPE(op2) == TYPE_INTEGER ? neo_to_integer(op2) != 0 : neo_to_bool(op2);
    
  return neo_bool_create(nb1 ^ nb2);
}




NeObj _pow(NeObj _op1, NeObj _op2)
{
  if (NEO_TYPE(_op1) == TYPE_CONTAINER || NEO_TYPE(_op2) == TYPE_CONTAINER) {
    NeObj ret = callOverloadedBinaryOperator(_op1, _op2, "pow");
    if (CODE_ERROR != 0) {
      return NEO_VOID;
    }
    return ret;
  }
  else if ((NEO_TYPE(_op1)==TYPE_INTEGER || NEO_TYPE(_op1)==TYPE_DOUBLE) && (NEO_TYPE(_op2)==TYPE_INTEGER || NEO_TYPE(_op2)==TYPE_DOUBLE))
  {
    double op1 = (NEO_TYPE(_op1) == TYPE_INTEGER) ? (double)neo_to_integer(_op1) : neo_to_double(_op1);
    double op2 = (NEO_TYPE(_op2) == TYPE_INTEGER) ? (double)neo_to_integer(_op2) : neo_to_double(_op2);
    return neo_double_create(pow(op1, op2));
  }
  else
  {
      CODE_ERROR = 50;
      return NEO_VOID;
  }
  return NEO_VOID;
}



NeObj _equal(NeObj op1, NeObj op2)
{
    return neo_bool_create(neo_equal(op1,op2));
}




NeObj _notEq(NeObj _op1, NeObj _op2)
{
    return neo_bool_create(!neo_equal(_op1,_op2));
}







NeObj _infEqual(NeObj op1, NeObj op2)
{
    bool res = neo_compare(op1, op2) <= 0;
    if (CODE_ERROR != 0)
    {
        CODE_ERROR = 52;
        return NEO_VOID;
    }
    
    return neo_bool_create(res);
}





NeObj _supEqual(NeObj op1, NeObj op2)
{
    bool res = neo_compare(op1, op2) >= 0;
    if (CODE_ERROR != 0)
    {
        CODE_ERROR = 53;
        return NEO_VOID;
    }
    
    return neo_bool_create(res);
}






NeObj _inf(NeObj op1, NeObj op2)
{
    bool res = neo_compare(op1, op2) < 0;
    if (CODE_ERROR != 0)
    {
        CODE_ERROR = 54;
        return NEO_VOID;
    }
    
    return neo_bool_create(res);
}





NeObj _sup(NeObj op1, NeObj op2)
{
    bool res = neo_compare(op1, op2) > 0;
    if (CODE_ERROR != 0)
    {
        CODE_ERROR = 55;
        return NEO_VOID;
    }
    
    return neo_bool_create(res);
}




NeObj _affect(NeObj op2, NeObj* op1)
{
    // ici, on affecte à une variable qui existe déjà forcément, mais n'a peut-être pas de valeur
    // op1 est un NeObject, on va modifier directement le NeObject, via son pointeur
    // op1 représente le nom de la variable et op2 la valeur à affecter
    //neobject_destroy(op1);
    if (NEO_TYPE(op2) == TYPE_EMPTY)
    {
        CODE_ERROR = 56; // unedfined var
        return NEO_VOID;
    }
    else
    {
        neobject_destroy(*op1);
        *op1 = neo_copy(op2);
        return neo_copy(*op1);
    }
    return NEO_VOID;
}







NeObj _affectNone(NeObj* op1, NeObj op2)
{
    // ici, on affecte à une variable qui existe déjà forcément, mais n'a peut-être pas de valeur
    // op1 est un NeObject, on va modifier directement le NeObject, via son pointeur
    // op1 représente le nom de la variable et op2 la valeur à affecter
    //neobject_destroy(op1);
    if (NEO_TYPE(op2) == TYPE_EMPTY)
    {
        CODE_ERROR = 56; // unedfined var
        return NEO_VOID;
    }
    else
    {
        neobject_destroy(*op1);
        *op1 = neo_copy(op2);
        return neo_none_create();
    }
    return NEO_VOID;
}








void _affect2(NeObj* op1, NeObj op2)
{
    // ici, on affecte à une variable qui existe déjà forcément, mais n'a peut-être pas de valeur
    // op1 est un NeObject, on va modifier directement le NeObject, via son pointeur
    // op1 représente le nom de la variable et op2 la valeur à affecter
    //neobject_destroy(op1);
    if (NEO_TYPE(op2) == TYPE_EMPTY)
    {
        CODE_ERROR = 56;//undefined var
        return ;
    }
    else
    {
        neobject_destroy(*op1);
        *op1 = neo_copy(op2);
        return;
    }
    return;
}




NeObj _goIn(NeObj op2, NeObj op1)
{

    if (NEO_TYPE(op2) == TYPE_STRING)
    {
        char* nomVar=neo_to_string(op2); // attention nomVar peut contenir un index de liste
        
        NeObj* var = NULL;
        
        if (!strlist_inList(NOMS,nomVar)) // création d'une nouvelle variable
        {
            strlist_append(NOMS,strdup(nomVar));
            nelist_append(ADRESSES, neo_empty_create()); // variable ajoutée
            var = &ADRESSES->tab[ADRESSES->len-1];
        }
        else
        {
            int index = strlist_index(NOMS, nomVar);
            var = &ADRESSES->tab[index];            
        }
    
        // ADRESSES->tab[index] contient donc le NeObject dans lequel on doit mettre la valeur
        NeObj res = _affect(op1, var);
        if (CODE_ERROR != 0)
            return NEO_VOID;
        
        return res;
    
    
    }
    else
    {
      // Erreur : Vous devez procéder comme suit valeur ->  \"nom_variable\"
      CODE_ERROR = 57;
    }

    return NEO_VOID;
}












NeObj _addEqual(NeObj* op1, NeObj op2)
{
    NeObj add = _add(*op1,op2);
    if (CODE_ERROR != 0)
        return NEO_VOID;
    NeObj res = _affect(add, op1);
    if (CODE_ERROR != 0)
        return NEO_VOID;
    neobject_destroy(add);
    return res;
}




NeObj _subEqual(NeObj* op1, NeObj op2)
{
    NeObj sub = _sub(*op1,op2);
    if (CODE_ERROR != 0)
        return NEO_VOID;
    NeObj res = _affect(sub, op1);
    if (CODE_ERROR != 0)
        return NEO_VOID;
    neobject_destroy(sub);
    return res;
}



NeObj _mulEqual(NeObj* op1, NeObj op2)
{
    NeObj mul = _mul(*op1,op2);
    if (CODE_ERROR != 0)
        return NEO_VOID;
    NeObj res = _affect(mul, op1);
    if (CODE_ERROR != 0)
        return NEO_VOID;
    neobject_destroy(mul);
    return res;
}



NeObj _divEqual(NeObj* op1, NeObj op2)
{
    NeObj div = _div(*op1,op2);
    if (CODE_ERROR != 0)
        return NEO_VOID;
    NeObj res = _affect(div, op1);
    if (CODE_ERROR != 0)
        return NEO_VOID;
    neobject_destroy(div);
    return res;
}



NeObj _incr(NeObj* op1)
{
    NeObj new;
    if (NEO_TYPE((*op1)) == TYPE_INTEGER) {
        new = neo_integer_create(neo_to_integer(*op1) + 1);
    }
    else if (NEO_TYPE((*op1)) == TYPE_DOUBLE) {
        new = neo_double_create(neo_to_double(*op1) + (double)1);
    }
    else {
        CODE_ERROR = 40;
        return NEO_VOID;
    }

    _affect2(op1, new);
    return new;
}

void _incr2(NeObj* op1, int incr) {
    NeObj new;
    if (NEO_TYPE((*op1)) == TYPE_INTEGER) {
        new = neo_integer_create(neo_to_integer(*op1) + 1);
    }
    else if (NEO_TYPE((*op1)) == TYPE_DOUBLE) {
        new = neo_double_create(neo_to_double(*op1) + (double)1);
    }
    else {
        CODE_ERROR = 40;
        return ;
    }

    neobject_destroy(*op1);
    *op1 = new;
    return;
}


NeObj _decr(NeObj* op1)
{
    NeObj new;
    if (NEO_TYPE((*op1)) == TYPE_INTEGER) {
        new = neo_integer_create(neo_to_integer(*op1) - 1);
    }
    else if (NEO_TYPE((*op1)) == TYPE_DOUBLE) {
        new = neo_double_create(neo_to_double(*op1) - (double)1);
    }
    else {
        CODE_ERROR = 40;
        return NEO_VOID;
    }

    _affect2(op1, new);
    return new;
}




NeObj _not(NeObj op1)
{
  if (NEO_TYPE(op1) != TYPE_BOOL && NEO_TYPE(op1) != TYPE_INTEGER)
  {
        CODE_ERROR = 58;
        return NEO_VOID;
  }

  bool nb1;
  
  nb1 = NEO_TYPE(op1) == TYPE_INTEGER ? neo_to_integer(op1) != 0 : neo_to_bool(op1);
    
  return neo_bool_create(!nb1);
}





NeObj _ref(NeObj op1)
{
    if (NEO_TYPE(op1) == TYPE_EMPTY)
    {
        CODE_ERROR = 59;//undefined var
        return NEO_VOID;
    }
    int index = nelist_index(ADRESSES,op1);
    return neo_str_create(strdup(NOMS->tab[index]));
}








NeObj _minus(NeObj op1)
{
    
    if (NEO_TYPE(op1) == TYPE_INTEGER)
    {
        return neo_integer_create(- neo_to_integer(op1));
    }
    else if (NEO_TYPE(op1) == TYPE_DOUBLE)
    {
        return neo_double_create(- neo_to_double(op1));
    }
    else if (NEO_TYPE(op1) == TYPE_CONTAINER)
    {
        NeObj ret = callOverloadedUnaryOperator(op1, "minus");
        if (CODE_ERROR != 0) {
          return NEO_VOID;
        }
        return ret;
    }
    else
    {
        CODE_ERROR = 61; // types non supportés par l'opérateur moins (unaire)
        return NEO_VOID;
    }

    return NEO_VOID;

}

NeObj _del(NeObj* op1)
{
    neobject_destroy(*op1);
    *op1 = neo_empty_create();
    return neo_none_create();
}


NeObj _exponent(NeObj op1, NeObj op2)
{
  if ((NEO_TYPE(op1)==TYPE_INTEGER || NEO_TYPE(op1)==TYPE_DOUBLE) && (NEO_TYPE(op2)==TYPE_INTEGER || NEO_TYPE(op2)==TYPE_DOUBLE))
  {
    double _op1 = (NEO_TYPE(op1) == TYPE_INTEGER) ? (double)neo_to_integer(op1) : neo_to_double(op1);
    double _op2 = (NEO_TYPE(op2) == TYPE_INTEGER) ? (double)neo_to_integer(op2) : neo_to_double(op2);
    return neo_double_create(_op1 * pow(10.f, _op2));
  }
  else
  {
      CODE_ERROR = 50;
      return NEO_VOID;
  }
  return NEO_VOID;
}


NeObj _implique(NeObj op1, NeObj op2)
{
    if (NEO_TYPE(op2) != TYPE_BOOL && NEO_TYPE(op1) != TYPE_BOOL)
    {
        CODE_ERROR=75;
        return NEO_VOID;
    }
    else
    {
        return (neo_bool_create(!neo_to_bool(op1) || neo_to_bool(op2)));
    }
}


NeObj _in(NeObj op1, NeObj op2)
{

    if (NEO_TYPE(op1) == TYPE_CONTAINER || NEO_TYPE(op2) == TYPE_CONTAINER) {
        NeObj ret = callOverloadedBinaryOperator(op1, op2, "in");
        if (CODE_ERROR != 0) {
          return NEO_VOID;
        }
        return ret;
    }

    if (NEO_TYPE(op2) != TYPE_LIST)
    {
        CODE_ERROR = 79;
        return NEO_VOID;
    }
    else
    {
        return neo_bool_create(nelist_inList2(neo_to_list(op2), op1));
    }
}

NeObj _swap(NeObj* op1, NeObj* op2)
{
    NeObj temp = *op1;
    *op2 = *op1;
    *op1 = temp;
    return neo_none_create();
}


NeObj _deref(NeObj op1) {
    if (NEO_TYPE(op1) != TYPE_STRING) {
        CODE_ERROR = 60; // ceci n'est pas une chaine de caractères
        return NEO_VOID;
    }

    char* nom=neo_to_string(op1);

    if(!strlist_inList(NOMS, nom))
    {
        CODE_ERROR = 5;
        return NEO_VOID;
    }
        
    int index = strlist_index(NOMS,nom);    
    return neo_copy(ADRESSES->tab[index]);
}