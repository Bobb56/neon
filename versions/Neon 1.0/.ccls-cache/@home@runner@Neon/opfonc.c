#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


#include "headers.h"





//creation du tableau de pointeurs generiques a liberer en cas d’erreur


extern ptrlist allocptr;



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
extern strlist NOMS;
extern intlist TYPES;
extern intlist ADRESSES;
extern strlist VARIABLES;
extern listlist LISTES;

extern strlist NOMSBUILTINSFONC;



extern char* (*OPFONC[NBOPERATEURS])(char*,unsigned,char*,unsigned,int*);
extern char* (*BUILTINSFONC[NBBUILTINFONC])(strlist*, intlist*, int*);



extern unsigned nbTempList;





//definition des fonctions attribuees aux operateurs

char* _add(char* _op1,unsigned _nat1,char* _op2,unsigned _nat2,int* typeRetour)
{
  if (_nat1==TYPE_NUMBER[0])
  {
    if (_nat2==TYPE_NUMBER[0])
    {
      *typeRetour=TYPE_NUMBER[0];
      return nbToStr(strToNb(_op1)+strToNb(_op2));
    }
    else
    {
      quitError("Erreur : types non supportes pour l addition.");
    }
  }
  else if (_nat1==TYPE_STRING[0])
  {
    if (_nat2==TYPE_STRING[0])
    {
      char* sousch1=sub(_op1,0,strlen(_op1)-1);
      char* sousch2=sub(_op2,1,strlen(_op2));
      char* res=addStr(sousch1,sousch2);
      err_free(sousch1);err_free(sousch2);
      *typeRetour=TYPE_STRING[0];
      return res;
    }
    else
    {
      quitError("Erreur : types non supportes pour l addition.");
    }
  }
  else
  {
    quitError("Erreur : types non supportes pour l addition.");
  }
  return "Erreur";
}






char* _sub(char* _op1,unsigned _nat1,char* _op2,unsigned _nat2,int* typeRetour)
{
  if (_nat1==TYPE_NUMBER[0] && _nat2==TYPE_NUMBER[0])
  {
    *typeRetour=TYPE_NUMBER[0];
    return nbToStr(strToNb(_op1)-strToNb(_op2));
  }
  else if (_nat1==TYPE_STRING[0] && _nat2==TYPE_NUMBER[0])
  {
    *typeRetour=TYPE_STRING[0];
    if (strlen(_op1)<3) {quitError(ERROR_OUT_OF_RANGE);}
    char* sousch = sub(_op1,0,strlen(_op1)-1-strToNb(_op2));
    char* retour = addStr(sousch,"\"");
    err_free(sousch);
    return retour;
  }
  else if (_nat1==TYPE_STRING[0] && _nat2==TYPE_STRING[0])
  {
    *typeRetour=TYPE_STRING[0];
    char *sousch=sub(_op1,1,strlen(_op1)-1);
    char *sousch2=sub(_op2,1,strlen(_op2)-1);
    
    char* remplacement=replace(sousch,sousch2,"");
    
    char *retour1=addStr("\"",remplacement);
    char *retour2=addStr(retour1,"\"");
    
    err_free(sousch);err_free(remplacement);err_free(retour1);err_free(sousch2);
    return retour2;
    
  }
  else
  {
    quitError("Erreur : types non supportes pour la soustraction.");
  }
  return "Erreur";
}






char* _div(char* _op1,unsigned _nat1,char* _op2,unsigned _nat2,int* typeRetour)
{
  if (_nat1==TYPE_NUMBER[0] && _nat2==TYPE_NUMBER[0])
  {
    *typeRetour=TYPE_NUMBER[0];
    return nbToStr(strToNb(_op1)/strToNb(_op2));
  }
  else
  {
    quitError("Erreur : types non supportes pour la division.");
  }
  return "Erreur";
}






char* _mul(char* _op1,unsigned _nat1,char* _op2,unsigned _nat2,int* typeRetour)
{
  if (_nat1==TYPE_NUMBER[0] && _nat2==TYPE_NUMBER[0])
  {
    *typeRetour=TYPE_NUMBER[0];
    return nbToStr(strToNb(_op1)*strToNb(_op2));
  }
  else if ((_nat1==TYPE_NUMBER[0] && _nat2==TYPE_STRING[0]) || (_nat1==TYPE_STRING[0] && _nat2==TYPE_NUMBER[0]))
  {
    char* multip;char* tmp;char* sousch;char* retour;
    unsigned times=0;
    if (_nat1==TYPE_STRING[0])
    {
      sousch=sub(_op1,1,strlen(_op1)-1);
      multip=sub(_op1,0,strlen(_op1)-1);
      times=(unsigned)strToNb(_op2);
    }
    else
    {
      sousch=sub(_op2,1,strlen(_op2)-1);
      multip=sub(_op2,0,strlen(_op2)-1);
      times=(unsigned)strToNb(_op1);
    }
    
    
    
    for (unsigned i=1;i<times;i++)
    {
      tmp=addStr(multip,sousch);
      err_free(multip);
      multip=tmp;
    }
    *typeRetour=TYPE_STRING[0];
    retour = addStr(multip,"\"");
    
    err_free(multip);
    err_free(sousch);
    
    return retour;
  }
  else
  {
    quitError("Erreur : types non supportes pour la multiplication.");
  }
  return "Erreur";
}





char* _mod(char* _op1,unsigned _nat1,char* _op2,unsigned _nat2,int* typeRetour)
{
  if (_nat1==TYPE_NUMBER[0] && _nat2==TYPE_NUMBER[0])
  {
    *typeRetour=TYPE_NUMBER[0];
    return nbToStr((long long int)strToNb(_op1)%(long long int)strToNb(_op2));
  }
  else
  {
    quitError("Erreur : types non supportes pour le modulo.");
  }
  return "Erreur";
}





char* _eucl(char* _op1,unsigned _nat1,char* _op2,unsigned _nat2,int* typeRetour)
{
  if (_nat1==TYPE_NUMBER[0] && _nat2==TYPE_NUMBER[0])
  {
    double nb1=strToNb(_op1);
    double nb2=strToNb(_op2);
    *typeRetour=TYPE_NUMBER[0];
    return nbToStr((nb1-(long long int)nb1%(long long int)nb2)/nb2);
  }
  else if ((_nat1==TYPE_STRING[0] && _nat2==TYPE_NUMBER[0]) || (_nat2==TYPE_STRING[0] && _nat1==TYPE_NUMBER[0]))
  {
    *typeRetour=TYPE_STRING[0];
    char* retour;char* chaine;char* sousch;
    long int times;
    if (_nat1==TYPE_STRING[0])
    {
      times=strToNb(_op2);
      chaine=_op1;
    }
    else
    {
      times=strToNb(_op1);
      chaine=_op2;
    }
    unsigned longueur=strlen(chaine)-2;
    unsigned nouvLongueur=(longueur-longueur%times)/times;
    sousch = sub(chaine,0,nouvLongueur+1);
    retour=addStr(sousch, "\"");
    err_free(sousch);
    return retour;
  }
  else
  {
    quitError("Erreur : types non supportes pour la division euclidienne.");
  }
  return "Erreur";
}





char* _and(char* op1,unsigned nat1,char* op2,unsigned nat2,int* typeRetour)
{
  if ((nat1 != TYPE_BOOL[0] && nat1 != TYPE_NUMBER[0]) || (nat2 != TYPE_BOOL[0] && nat2 != TYPE_NUMBER[0]))
    quitError("Types non supportes pour 'et'.\n");

  _Bool nb1,nb2,res;
  
  if (strcmp(op1,"0")==0 || strcmp(op1,"False")==0){nb1=0;} else {nb1=1;}
  if (strcmp(op2,"0")==0 || strcmp(op2,"False")==0){nb2=0;} else {nb2=1;}
  
  res=nb1 && nb2;
  
  if (res)
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("True");
  }
  else
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("False");
  }
  return "Erreur";
}





char* _or(char* op1,unsigned nat1,char* op2,unsigned nat2,int* typeRetour)
{
  if ((nat1 != TYPE_BOOL[0] && nat1 != TYPE_NUMBER[0]) || (nat2 != TYPE_BOOL[0] && nat2 != TYPE_NUMBER[0]))
    quitError("Types non supportes pour 'ou'.\n");
  _Bool nb1,nb2,res;
  if (strcmp(op1,"0")==0 || strcmp(op1,"False")==0){nb1=0;} else {nb1=1;}
  if (strcmp(op2,"0")==0 || strcmp(op2,"False")==0){nb2=0;} else {nb2=1;}
  
  res=nb1 || nb2;
  
  if (res)
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("True");
  }
  else
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("False");
  }
  return "Erreur";
}






char* _xor(char* op1,unsigned nat1,char* op2,unsigned nat2,int* typeRetour)
{
  if ((nat1 != TYPE_BOOL[0] && nat1 != TYPE_NUMBER[0]) || (nat2 != TYPE_BOOL[0] && nat2 != TYPE_NUMBER[0]))
    quitError("Types non supportes pour 'xor'.\n");
  _Bool nb1,nb2,res;
  if (strcmp(op1,"0")==0 || strcmp(op1,"False")==0){nb1=0;} else {nb1=1;}
  if (strcmp(op2,"0")==0 || strcmp(op2,"False")==0){nb2=0;} else {nb2=1;}
  
  res=nb1 ^ nb2;
  
  if (res)
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("True");
  }
  else
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("False");
  }
  return "Erreur";
}




char* _pow(char* _op1,unsigned _nat1,char* _op2,unsigned _nat2,int* typeRetour)
{
  if (_nat1==TYPE_NUMBER[0] && _nat2==TYPE_NUMBER[0])
  {
    *typeRetour=TYPE_NUMBER[0];
    return nbToStr(pow(strToNb(_op1),strToNb(_op2)));
  }
  else
  {
    quitError("Erreur : types non supportes pour la puissance.");
  }
  return "Erreur";
}







char* _equal(char* _op1,unsigned _nat1,char* _op2,unsigned _nat2,int* typeRetour)
{
  if (_nat1==TYPE_NUMBER[0])
  {
    if (_nat2==TYPE_NUMBER[0])
    {
      _Bool res=strToNb(_op1)==strToNb(_op2);
      
      if (res)
      {
        *typeRetour=TYPE_BOOL[0];
        return copy("True");
      }
      else
      {
        *typeRetour=TYPE_BOOL[0];
        return copy("False");
      }
      
    }
    else
    {
      quitError("Erreur : types non supportes pour l egalite.");
    }
  }
  else if (_nat1==TYPE_STRING[0])
  {
    if (_nat2==TYPE_STRING[0])
    {
      _Bool res=strcmp(_op1,_op2);
      
      
      if (res)
      {
        *typeRetour=TYPE_BOOL[0];
        return copy("False");
      }
      else
      {
        *typeRetour=TYPE_BOOL[0];
        return copy("True");
      }
    }
    else
    {
      quitError("Erreur : types non supportes pour l egalite.");
    }
  }
  else
  {
    quitError("Erreur : types non supportes pour l egalite.");
  }
  return "Erreur";
}





char* _notEq(char* _op1,unsigned _nat1,char* _op2,unsigned _nat2,int* typeRetour)
{
  if (_nat1==TYPE_NUMBER[0])
  {
    if (_nat2==TYPE_NUMBER[0])
    {
      _Bool res=strToNb(_op1)!=strToNb(_op2);
      
      if (res)
      {
        *typeRetour=TYPE_BOOL[0];
        return copy("True");
      }
      else
      {
        *typeRetour=TYPE_BOOL[0];
        return copy("False");
      }
      
    }
    else
    {
      quitError("Erreur : types non supportes pour l egalite.");
    }
  }
  else if (_nat1==TYPE_STRING[0])
  {
    if (_nat2==TYPE_STRING[0])
    {
      _Bool res=strcmp(_op1,_op2);
      
      
      if (res)
      {
        *typeRetour=TYPE_BOOL[0];
        return copy("True");
      }
      else
      {
        *typeRetour=TYPE_BOOL[0];
        return copy("False");
      }
    }
    else
    {
      quitError("Erreur : types non supportes pour l egalite.");
    }
  }
  else
  {
    quitError("Erreur : types non supportes pour l egalite.");
  }
  return "Erreur";
}







char* _infEqual(char* op1,unsigned nat1,char* op2,unsigned nat2,int* typeRetour)
{
  double nb1,nb2;
  _Bool res;
  nb1=strToNb(op1);
  nb2=strToNb(op2);
  
  res=nb1<=nb2;
  
  if (res)
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("True");
  }
  else
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("False");
  }
  return "Erreur";
}





char* _supEqual(char* op1,unsigned nat1,char* op2,unsigned nat2,int* typeRetour)
{
  double nb1,nb2;
  _Bool res;
  nb1=strToNb(op1);
  nb2=strToNb(op2);
  
  res=nb1>=nb2;
  
  if (res)
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("True");
  }
  else
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("False");
  }
  return "Erreur";
}






char* _inf(char* op1,unsigned nat1,char* op2,unsigned nat2,int* typeRetour)
{
  double nb1,nb2;
  _Bool res;
  nb1=strToNb(op1);
  nb2=strToNb(op2);
  
  res=nb1<nb2;
  
  if (res)
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("True");
  }
  else
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("False");
  }
  return "Erreur";
}





char* _sup(char* op1,unsigned nat1,char* op2,unsigned nat2,int* typeRetour)
{
  double nb1,nb2;
  _Bool res;
  nb1=strToNb(op1);
  nb2=strToNb(op2);
  
  res=nb1>nb2;
  
  if (res)
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("True");
  }
  else
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("False");
  }
  return "Erreur";
}





char* _affect(char* op1, unsigned nat1, char* op2, unsigned nat2,int* typeRetour)
{
  // op1 représente le nom de la variable et op2 la valeur à affecter
  
  
  
  if (!(isVariable(op1) || nat1==TYPE_LISTINDEX[0]))
  {
    quitError("Erreur : impossible d affecter une valeur a autre chose qu une variable ou un index de liste.");
  }
  
  _Bool inList = strlist_inList(&NOMS,op1);
  
  
  if (inList || nat1==TYPE_LISTINDEX[0])// si la variable existe deja on la supprime avant
  {
    
    if (nat1==TYPE_LISTINDEX[0])
    {
      char* nomListe=nameListIndex(op1);
      unsigned index=indexListIndex(op1);
      
      unsigned indexListe=strlist_index(&NOMS, nomListe);
      
      err_free(nomListe);
    
      err_free(LISTES.tab[ADRESSES.tab[indexListe]].tab[index]);
    }
    else if (nat1==TYPE_VARIABLE[0])
    {
      unsigned indexVar = strlist_index(&NOMS,op1);
      unsigned typeVar=TYPES.tab[indexVar];
      
      if (typeVar==TYPE_TEMPLIST[0] || typeVar==TYPE_LIST[0])
      {
        listlist_remove(&LISTES,ADRESSES.tab[indexVar]);
      }
      else
      {
        strlist_remove(&VARIABLES,ADRESSES.tab[indexVar],true);
      }
      
      intlist_remove(&TYPES,indexVar);
      intlist_remove(&ADRESSES,indexVar);
      strlist_remove(&NOMS,indexVar,1);
      
      //algorithme qui decale les adresses des varables car on a supprimé une variable
      for (unsigned k=indexVar;k<ADRESSES.len;k++)
      {
        if ((TYPES.tab[k] != TYPE_LIST[0] && TYPES.tab[k] != TYPE_TEMPLIST[0] && typeVar != TYPE_LIST[0] && typeVar != TYPE_TEMPLIST[0]) || (TYPES.tab[k]==TYPE_LIST[0] && typeVar==TYPE_TEMPLIST[0]) || (TYPES.tab[k]==TYPE_TEMPLIST[0] && typeVar==TYPE_LIST[0]))//si la variable est le type de variable supprimée
        {
          ADRESSES.tab[k]--;//on baisse l’adresse de 1
          
        }
          
      }
    }
    
  }


  

  if (nat2==TYPE_TEMPLIST[0] || nat2==TYPE_LIST[0])
  {
    
    unsigned indexList=strlist_index(&NOMS,op2);//index du nom de la liste temporaire
    
    
    
    unsigned longueur=LISTES.tab[ADRESSES.tab[indexList]].len;//longueur de la liste à copier
    
    strlist_append(&NOMS,copy(op1));
    intlist_append(&TYPES,TYPE_LIST[0]);
    intlist_append(&ADRESSES,LISTES.len);
    
    strlist liste=strlist_create(0);
    
    for (unsigned i=0;i<longueur;i++)
    {
      strlist_append(&liste,copy(LISTES.tab[ADRESSES.tab[indexList]].tab[i]));
    }
    
    listlist_append(&LISTES, &liste);
    
          
  }
  else
  {
    if (nat1==TYPE_LISTINDEX[0])//affecte un élément de liste
    {
      char* nomListe=nameListIndex(op1);
      
      unsigned index=indexListIndex(op1);
      
      unsigned indexListe=strlist_index(&NOMS, nomListe);
      
      err_free(nomListe);
      
      if (nat2==TYPE_TEMPLIST[0])
      {
        quitError("Erreur : Une liste ne peut pas contenir une autre liste.");
      }
      
      
      LISTES.tab[ADRESSES.tab[indexListe]].tab[index]=copy(op2);
      
    }
    else //variable normale
    {
      intlist_append(&TYPES,nat2);

      intlist_append(&ADRESSES,VARIABLES.len);
      strlist_append(&VARIABLES,copy(op2));
      strlist_append(&NOMS,copy(op1));
      
    }
    
  }
      
  *typeRetour=nat2;
  char* retour=copy(op2);
  
  
  
  return retour;
}



char* _goIn(char* op2, unsigned nat2, char* op1, unsigned nat1,int* typeRetour)
{
  char* nomVar=sub(op1,1,strlen(op1)-1);
  
  if (isVariable(nomVar))
  {
    nat1=TYPE_VARIABLE[0];
  }
  else if (isListIndex(nomVar))
  {
    nat1=TYPE_LISTINDEX[0];
  }
  else
  {
    quitError("Erreur : impossible d affecter une valeur a autre chose qu une variable ou un index de liste.");
  }

  char* retour = _affect(nomVar,nat1,op2,nat2,typeRetour);
  err_free(nomVar);
  return retour;
}












char* _addEqual(char* op1, unsigned nat1, char* op2, unsigned nat2,int* typeRetour)
{
  unsigned index = strlist_index(&NOMS,op1);
  char* op_1=copy(VARIABLES.tab[ADRESSES.tab[index]]);
  
  err_free(VARIABLES.tab[ADRESSES.tab[index]]);
  VARIABLES.tab[ADRESSES.tab[index]]=_add(op_1,TYPES.tab[index],op2,nat2,typeRetour);
  err_free(op_1);
  
  TYPES.tab[index]=*typeRetour;
  
  char* retour=copy(VARIABLES.tab[ADRESSES.tab[index]]);
  
  
  
  return retour;
}



char* _subEqual(char* op1, unsigned nat1, char* op2, unsigned nat2,int* typeRetour)
{
  
  unsigned index = strlist_index(&NOMS,op1);
  char* op_1=copy(VARIABLES.tab[ADRESSES.tab[index]]);
  
  err_free(VARIABLES.tab[ADRESSES.tab[index]]);
  VARIABLES.tab[ADRESSES.tab[index]]=_sub(op_1,TYPES.tab[index],op2,nat2,typeRetour);
  err_free(op_1);
  
  TYPES.tab[index]=*typeRetour;
  
  char* retour=copy(VARIABLES.tab[ADRESSES.tab[index]]);
  return retour;
}



char* _mulEqual(char* op1, unsigned nat1, char* op2, unsigned nat2,int* typeRetour)
{
  
  unsigned index = strlist_index(&NOMS,op1);
  char* op_1=copy(VARIABLES.tab[ADRESSES.tab[index]]);
  
  err_free(VARIABLES.tab[ADRESSES.tab[index]]);
  VARIABLES.tab[ADRESSES.tab[index]]=_mul(op_1,TYPES.tab[index],op2,nat2,typeRetour);
  err_free(op_1);
  
  TYPES.tab[index]=*typeRetour;
  
  char* retour=copy(VARIABLES.tab[ADRESSES.tab[index]]);
  return retour;
}



char* _divEqual(char* op1, unsigned nat1, char* op2, unsigned nat2,int* typeRetour)
{
  
  unsigned index = strlist_index(&NOMS,op1);
  char* op_1=copy(VARIABLES.tab[ADRESSES.tab[index]]);
  
  err_free(VARIABLES.tab[ADRESSES.tab[index]]);
  VARIABLES.tab[ADRESSES.tab[index]]=_div(op_1,TYPES.tab[index],op2,nat2,typeRetour);
  err_free(op_1);
  
  TYPES.tab[index]=*typeRetour;
  
  char* retour=copy(VARIABLES.tab[ADRESSES.tab[index]]);
  return retour;
}



char* _incr(char* op1, unsigned nat1, char* op2, unsigned nat2,int* typeRetour)
{
  if (nat1 == TYPE_VARIABLE[0])
  {
      unsigned index = strlist_index(&NOMS,op1);
      
      char* op_1=copy(VARIABLES.tab[ADRESSES.tab[index]]);
      
      err_free(VARIABLES.tab[ADRESSES.tab[index]]);
      VARIABLES.tab[ADRESSES.tab[index]]=_add(op_1,TYPES.tab[index],op2,nat2,typeRetour);
      err_free(op_1);
      TYPES.tab[index]=*typeRetour;
      
      char* retour=copy(VARIABLES.tab[ADRESSES.tab[index]]);
      
      return retour;
  }
  else if (nat1 == TYPE_LISTINDEX[0])
  {
      // il faut récupérer le nom de la liste
      char* nomListe = nameListIndex(op1);
      unsigned index = indexListIndex(op1);
      
      unsigned index2 = strlist_index(&NOMS,nomListe);
      
      char* op_1=copy(LISTES.tab[ADRESSES.tab[index2]].tab[index]); // copie de l'element de la liste
      int nat_1 = typeListItem(op_1); // type de l'element de la liste
      
      
      err_free(LISTES.tab[ADRESSES.tab[index2]].tab[index]);
      LISTES.tab[ADRESSES.tab[index2]].tab[index] = _add(op_1,nat_1,op2,nat2,typeRetour);
      err_free(op_1);
      
      char* retour=copy(LISTES.tab[ADRESSES.tab[index2]].tab[index]);
      
      err_free(nomListe);
      
      return retour;
  }
  else
    quitError("Types non supportes pour l'incrementation\n");
  
  
}



char* _decr(char* op1, unsigned nat1, char* op2, unsigned nat2,int* typeRetour)
{
  if (nat1 == TYPE_VARIABLE[0])
  {
      unsigned index = strlist_index(&NOMS,op1);
      
      char* op_1=copy(VARIABLES.tab[ADRESSES.tab[index]]);
      
      err_free(VARIABLES.tab[ADRESSES.tab[index]]);
      
      VARIABLES.tab[ADRESSES.tab[index]]=_sub(op_1,TYPES.tab[index],op2,nat2,typeRetour);
      err_free(op_1);
      TYPES.tab[index]=*typeRetour;
      
      char* retour=copy(VARIABLES.tab[ADRESSES.tab[index]]);
      
      return retour;
  }
  else if (nat1 == TYPE_LISTINDEX[0])
  {
      
      // il faut récupérer le nom de la liste
      char* nomListe = nameListIndex(op1);
      unsigned index = indexListIndex(op1);
      
      unsigned index2 = strlist_index(&NOMS,nomListe);
      
      char* op_1=copy(LISTES.tab[ADRESSES.tab[index2]].tab[index]); // copie de l'element de la liste
      int nat_1 = typeListItem(op_1); // type de l'element de la liste
      
      
      err_free(LISTES.tab[ADRESSES.tab[index2]].tab[index]);
      LISTES.tab[ADRESSES.tab[index2]].tab[index] = _sub(op_1,nat_1,op2,nat2,typeRetour);
      err_free(op_1);
      
      char* retour=copy(LISTES.tab[ADRESSES.tab[index2]].tab[index]);
      
      err_free(nomListe);
      
      return retour;
      
  }
  else
    quitError("Types non supportes pour la decrementation\n");

}




char* _not(char* op1,unsigned nat1,char* op2,unsigned nat2,int* typeRetour)
{
  _Bool nb1,res;
  if (strcmp(op1,"0")==0 || strcmp(op1,"False")==0){nb1=0;} else {nb1=1;}
  
  res=!nb1;
  
  if (res)
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("True");
  }
  else
  {
    *typeRetour=TYPE_BOOL[0];
    return copy("False");
  }
}





char* _ref(char* op1,unsigned nat1,char* op2,unsigned nat2,int* typeRetour)
{
  *typeRetour=TYPE_STRING[0];
  char* chaine1=addStr("\"",op1);
  char* retour=addStr(chaine1,"\"");
  err_free(chaine1);
  return retour;
}




char* _deref(char* op1,unsigned nat1,char* op2,unsigned nat2,int* typeRetour)
{
  char* nom=sub(op1,1,strlen(op1)-1);
  
  unsigned index = strlist_index(&NOMS,nom);
  
  *typeRetour=TYPES.tab[index];
  char* retour = copy(VARIABLES.tab[ADRESSES.tab[index]]);
  err_free(nom);
  return retour;
}




char* _minus(char* op1,unsigned nat1,char* op2,unsigned nat2,int* typeRetour)
{
    
    if (nat1 == TYPE_NUMBER[0])
    {
        *typeRetour = TYPE_NUMBER[0];
        return addStr("-", op1);
        
    }
    else
        quitError("Types non supportes pour l'operateur '-'\n");

}