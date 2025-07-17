#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


#include "headers.h"



/*

Documentation des erreurs de syntaxe :

Erreur de syntaxe1 : Chaine de caractère non fermée en fin de ligne
Erreur de syntaxe2 : Opérateur inconnu
Erreur de syntaxe3 et 3.5 : Caractère inconnu
Erreur de syntaxe4 : Unité lexicale quelconque non terminée
Erreur de syntaxe5 : Mauvaise utilisation d'un opérateur, deux types à la suite non compatibles, ou utilisation d'un type inconnu/incompatible
Erreur de syntaxe6 : Mauvais agencement de blocs if/elif/else
Erreur de syntaxe7 : Nombre contenant plusieurs virgules décimales
Erreur de syntaxe8 : Mot clé incompatible utilisé dans une expression
Erreur de syntaxe9 : Ligne de bloc d'instructions non suivi par un bloc d'instructions (ex: un if() tout seul)
Erreur de syntaxe10 : Bloc d'instructions tout seul
Erreur de syntaxe11 : Mauvais agencement de blocs if/elif/else


*/








//creation du tableau de pointeurs generiques a liberer en cas d’erreur


extern ptrlist allocptr;



//déclaration des variables globales
extern int error;
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





/*---------------------------------- FONCTIONS SPECIFIQUES A L ANCIEN LIBTOK -------------------------------------*/




//definition des fonctions de grammaire
_Bool isVariable(char* str)
{
  if (strlist_inList(&OPERATEURS,str) || strlist_inList(&keywords,str) || strlist_inList(&blockwords,str))
  {
    return 0;
  }


  if (strlen(str)>0)
  {
    if (isalpha(str[0]) || str[0]=='_')
    {
      for (unsigned i=1 ; i<strlen(str) ; i++)
      {
        if (!isalnum(str[i]) && str[i]!='_')
        {
          return 0;
        }
      }
      
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return 0;
  }
}






_Bool isNumber(char* str)
{
  if (count(str,"-")>1 || count(str,".")>1 || (count(str,"-")==1 && str[0]!='-') || str[0]=='.')
  {
    return 0;
  }
  
  for (unsigned i=0 ; i<strlen(str) ; i++)
  {
    if (!isdigit(str[i]) && str[i]!='.' && str[i]!='-')
    {
      return 0;
    }
  }
  return 1;
}




_Bool isBool(char* str)
{
  for (unsigned i=0 ; i<boolean.len ; i++)
  {
    if (strlen(boolean.tab[i])>=strlen(str))
    {
      char* sousch=sub(boolean.tab[i],0,strlen(str));
      if (strcmp(str,sousch)==0)
      {
        err_free(sousch);
        return 1;
      }
      err_free(sousch);
    }
  }
  return 0;
}






_Bool isOperator(char* str)
{
  for (unsigned i=0 ; i<OPERATEURS.len ; i++)
  {
    if (strlen(OPERATEURS.tab[i])>=strlen(str))
    {
      char* sousch=sub(OPERATEURS.tab[i],0,strlen(str));
      if (strcmp(str,sousch)==0)
      {
        err_free(sousch);
        return 1;
      }
      err_free(sousch);
    }
  }
  return 0;
}





_Bool isBlank(char* str)
{
  for (unsigned i=0 ; i<strlen(str) ; i++)
  {
    if (str[i]!=' ')
    {
      return 0;
    }
  }
  return 1;
}



_Bool isKeyword(char* str)
{
  // on fait un tour une fois pour tester dans les keywords
  for (unsigned i=0 ; i<keywords.len ; i++)
  {
    if (strlen(keywords.tab[i])>=strlen(str))
    {
      char* sousch=sub(keywords.tab[i],0,strlen(str));
      if (strcmp(str,sousch)==0)
      {
        err_free(sousch);
        return 1;
      }
      err_free(sousch);
    }
  }
  
  // puis un autre tour dans les blockwords
  for (unsigned i=0 ; i<blockwords.len ; i++)
  {
    if (strlen(blockwords.tab[i])>=strlen(str))
    {
      char* sousch=sub(blockwords.tab[i],0,strlen(str));
      if (strcmp(str,sousch)==0)
      {
        err_free(sousch);
        return 1;
      }
      err_free(sousch);
    }
  }
  
  
  
  
  
  
  return 0;
}




_Bool isFonction(char* str)
{
  if (count(str,"(")==0)
  {
    return isVariable(str);
  }
  else
  {
    unsigned i=1;
    
    while (str[i-1]!='(')
    {
      i++;
    }
    
    char* str1=sub(str,0,i-1);
    
    if (!isVariable(str1) || (count(str,"(")==count(str,")") && str[strlen(str)-1]!=')') || count(str,")")>count(str,"("))
    {
      err_free(str1);
      return 0;
    }
    else
    {
      err_free(str1);
      return 1;
    }    
  }
}




_Bool isListIndex(char* str)
{
  if (count(str,"[")==0)
  {
    return isVariable(str);
  }
  else
  {
    unsigned i=1;
    while (str[i-1]!='[')
    {
      i++;
    }
    
    char* strtmp=sub(str,0,i-1);
    char* str1=replace(strtmp," ","");
    err_free(strtmp);
    
    if (!isVariable(str1) || (count(str,"[")==count(str,"]") && str[strlen(str)-1]!=']') || count(str,"]")>count(str,"["))
    {
      err_free(str1);
      return 0;
    }
    else
    {
      err_free(str1);
      return 1;
    }    
  }
}




_Bool isMatrixIndex(char* str)
{
  if (count(str,"[")==0)
  {
    return isVariable(str);
  }
  else
  {
    unsigned i=1;
    while (str[i-1]!='[')
    {
      i++;
    }
    
    char* strtmp=sub(str,0,i-1);
    char* str1=replace(strtmp," ","");
    char* str2=sub(str,i,strlen(str));
    err_free(strtmp);
    
    
    
    if (!isVariable(str1) || (count(str,"[")==count(str,"]") && str[strlen(str)-1]!=']') || count(str,"]")>count(str,"[") || (strstr(str2,",")==NULL && str[strlen(str)-1]==']'))
    {
      err_free(str2);
      err_free(str1);
      return 0;
    }
    else
    {
      err_free(str2);
      err_free(str1);
      return 1;
    }    
  }
}






_Bool isStr(char* str)
{
  if (strlen(str)<1)
  {
    return 0;
  }
  
  if (strcmp(str,"\"")==0 || strcmp(str,"\'")==0)
  {
    return 1;
  }
  else if (strlen(str)==1)
    return 0;
  if (strlen(str)<3)
  {
    if (str[0]=='\"')
      return 1;
  }
  char* sousch=sub(str, 1, strlen(str)-1);
  if (str[0]=='"' && count(sousch,"\"")==0)
  {
    err_free(sousch);
    return 1;
  }
  if (str[0]=='\'' && count(sousch,"'")==0)
  {
    err_free(sousch);
    return 1;
  }
  err_free(sousch);
  return 0;
  
}






_Bool isList(char* str)
{
  if (strlen(str)>0)
  {
    if (strcmp(str,"[")==0)
    {
      return 1;
    }
    else if (strlen(str)==1)
      return 0;


    
    char* sousch=sub(str,1,strlen(str));
    
    if (str[0]=='[' && count(str,"[")>=count(str,"]"))
    {
      err_free(sousch);
      return 1;
    }
    err_free(sousch);
  }
  return 0;
}




/*--------------------------------------------------------------------------------------------------------------*/





void cut(strlist* tokens, intlist* types, char* str, _Bool traiterStatements)
{
    /*fonction qui coupe la chaine de caractères en tokens
    traiterStatements est un booléen qui indique s'il faut regrouper les statements*/
    

    char* string;

    
    
    
    if (traiterStatements)
    {
        string = sandwich(str, '\n');
    }
    else
    {
        string = addStr(str, "\n");
    }
    

    *tokens = strlist_create(0);
    *types = intlist_create(0);
    
    _Bool nouvTok=false; //on modifie la valeur si on a un nouveau token à ajouter
    _Bool nouvTokComp=false; //on modifie la valeur quand on ajoute un nouveau token compose dans tokens
    intlist typeTok = intlist_create(0); // type du nouveau token, A LIBERER
    strlist tokenAdd = strlist_create(0); // token a ajouter A LIBERER
    
    
    // pour les sous-tokens
    _Bool isPotentiallyNumber=false;
    unsigned stepNumber = 0; // définit à quelle étape de définition de nombre on est lors de la détection
    _Bool isPotentiallyString=false;
    _Bool isPotentiallyString2=false; // chaine avec un guillemet court
    _Bool isPotentiallyWord=false;
    _Bool isPotentiallyOp=false;
    _Bool isPotentiallyComm=false;
    _Bool isPotentiallyLongComm=false;
    
    
    
    
    
    //pour les tokens composés
    _Bool isPotentiallyListIndex=false;
    _Bool isPotentiallyList=false;
    _Bool isPotentiallyFonc=false;
    _Bool isPotentiallyBlock=false;
    _Bool isPotentiallyInst=false;
    
    //nombre d’accollades ouvrantes detectees et d’accollades fermantes
    //on ajoute 1 en rencontrant une ouverte et on enleve un en rencontrant une fermee
    int nbAcc=0;
    int nbPar=0; // pareil pour les parentheses
    int nbCro=0; // pour les crochets
    
    unsigned foncParDeb=0; // va contenir le nombre de parenthèses déjà traversées au moment où on arrive à la fonction
    unsigned listeParDeb=0; // va contenir le nombre de parenthèses déjà traversées au moment où on arrive à la liste
    unsigned indexParDeb=0; // va contenir le nombre de parenthèses déjà traversées au moment où on arrive à la fonction
    
    
    
    
    //etapes de detection par exemple variable, puis parenthese "(" puis parenthese ")"
    unsigned listIndexStep=0;
    unsigned foncStep=0;
    unsigned instStep=0;
    
    
    unsigned debTok=0; // contient la place de debut de detection du token etudie
    unsigned debTok2=0; // contient la place de debut de detection du token compose etudie
    unsigned debTok3=0;
    
    unsigned debExpr=0; // va contenir la place de debut de l’expression dans le dernier algorithme
    _Bool isPotentiallyExpr=false; // pour la derniere boucle dans l’empaquetage d’expressions
    
    
    
    
    // pour la dernière boucle aussi, empaquetage des blocs conditionnels if/elif/else et for, while (statement)
    unsigned debStat=0;
    _Bool isPotentiallyStat=false;
    
    
    
    
    char char2=0; // va contenir le caractere precedent
    char char1=0; // va contenir le caractère actuel
    char* char1_2; // peut servir d'intermédiaire quand on veut transformer char1 en chaine de caractères
    
    unsigned nombrePoints=0; // nombre de points comptes apres le debut d’un nombre
    
    
    unsigned len_string = strlen(string);//pour utiliser la longueur de string sans appeler a chaque fois strlen
    
	
    for (unsigned i=0 ; i<len_string ; i++)
    {

        char1 = string[i];
        
        // comptage des variables relatives à la détection de nombres
        if (char1 == '.')
            nombrePoints+=1;
        if (isPotentiallyNumber && char1=='.' && nombrePoints==1)
            stepNumber=1;
        else if (isPotentiallyNumber && char1=='.')
            quitError("Erreur de syntaxe7");
        
        if (isPotentiallyNumber && isdigit(char1) && stepNumber == 1)
          stepNumber = 2;
        
        
        
        //fins tokens simples
        finTokensSimples(string, &isPotentiallyNumber, &isPotentiallyString, &isPotentiallyWord, &isPotentiallyOp, &isPotentiallyLongComm, &isPotentiallyString2, &isPotentiallyComm, &char2, &char1, &nouvTok, &tokenAdd, &typeTok, &debTok, i, &stepNumber);
        

        
        if (nouvTok) // ajout du nouveau token dans la liste tokens.
        {
            
            for (unsigned k=0 ; k < tokenAdd.len ; k++) // boucle pour chaque nouveau token ajoute. chaque tour de cette boucle est comme un tour de boucle normal sur tokens. cela permet d’eviter une boucle supplementaire                
            {
                // savoir si on a un operateur ou un mot cle ou un booleen
                if (typeTok.tab[k] == TYPE_OPERATOR[0]) // si il a déjà été détecté comme opérateur, on vérifie si c'est bien un opérateur valide
                    if (!(strlist_inList(&operateurs1, tokenAdd.tab[k])))
                        quitError("Erreur de syntaxe2");
                
                
                
                
                
                if (typeTok.tab[k] == TYPE_WORD[0])
                {
                    if (strlist_inList(&operateurs2, tokenAdd.tab[k]))
                    {
                        
                        typeTok.tab[k] = TYPE_OPERATOR[0];
                    }
                    else if (strlist_inList(&boolean, tokenAdd.tab[k]))
                    {
                        typeTok.tab[k] = TYPE_BOOL[0];
                    }
                    
                    else if (strlist_inList(&constant, tokenAdd.tab[k]))
                    {
                        if (strcmp(tokenAdd.tab[k], "None")==0)
                            typeTok.tab[k] = TYPE_NONE[0];
                        else
                            typeTok.tab[k] = TYPE_CONST[0];
                    }
                   
                    else if (strlist_inList(&keywords, tokenAdd.tab[k]))
                    {
                        
                        typeTok.tab[k] = TYPE_KEYWORD[0];
                    }
                    
                    else if (strlist_inList(&blockwords, tokenAdd.tab[k]))
                    {
                        
                        typeTok.tab[k] = TYPE_BLOCKWORD[0];
                    }
                    
                    else
                    {
                        
                        typeTok.tab[k] = TYPE_VARIABLE[0];
                    }
               
                }
               
                
                if (tokens->len==0 && typeTok.tab[k] != TYPE_COMMENT[0])
                {
                    strlist_append(tokens,tokenAdd.tab[k]);
                    intlist_append(types,typeTok.tab[k]);
                }
                    
                else if (tokens->len>0 && typeTok.tab[k] != TYPE_COMMENT[0])
                {
                    if (!((tokenAdd.tab[k][0]=='\n' || tokenAdd.tab[k][0]==';') && (tokens->tab[tokens->len-1][0] == '\n' || tokens->tab[tokens->len-1][0] == ';'))) // si le token precedent etait un endofline et que celui la aussi alors on ne l’ajoute pas
                    {
                        strlist_append(tokens,tokenAdd.tab[k]);
                        intlist_append(types,typeTok.tab[k]);
                    }
                    else
                    {
                        err_free(tokenAdd.tab[k]);
                    }
                }
                else
                {
                    err_free(tokenAdd.tab[k]);
                }
                
                
                
                
                //etapes de fonction et d’index de liste
                
                // a chaque nouveau token que l’on reçoit, on effectue les memes verifications que pour les sous tokens a savoir les verifications de grammaire pour les tokens composes
                
                //sorte de pile qui compte les accolades et les parentheses
                if (tokenAdd.tab[k][0]=='{')
                    nbAcc++;
                else if (tokenAdd.tab[k][0]=='}')
                    nbAcc--;
                
                
                if (tokenAdd.tab[k][0]=='(')
                    nbPar++;
                else if (tokenAdd.tab[k][0]==')')
                    nbPar--;
                
                
                if (tokenAdd.tab[k][0]=='[')
                    nbCro++;
                else if (tokenAdd.tab[k][0]==']')
                    nbCro--;
                
                
                // annuler ou poursuivre les fonctions, les instructions, les index de listes....
                
                annulerOuPoursuivre(&isPotentiallyFonc, &foncStep, &isPotentiallyInst, &instStep, &isPotentiallyListIndex, &listIndexStep, &tokenAdd, k, &foncParDeb, &indexParDeb, &listeParDeb);
                
                
                // fins de fonctions, d'instructions, d'index de listes...
                
                finsTypesComposes(&debTok2, &debTok3, k, &nouvTokComp, &isPotentiallyFonc, &foncStep, &tokenAdd, &nbPar, tokens, types, &isPotentiallyInst, &instStep, &listIndexStep, &isPotentiallyList, &isPotentiallyListIndex, &nbCro, &isPotentiallyBlock, &nbAcc, &foncParDeb, &indexParDeb, &listeParDeb);
                
                
                // fin des fins et debut des debuts (hi hi hi)
                
                debutTokensComposes(k, &typeTok, &tokenAdd, &debTok2, &debTok3, tokens, types, &nouvTokComp, &isPotentiallyList, &isPotentiallyFonc, &isPotentiallyListIndex, &isPotentiallyComm, &isPotentiallyBlock, &isPotentiallyInst, &isPotentiallyLongComm, &listIndexStep, &foncStep, &instStep, &foncParDeb, &indexParDeb, &listeParDeb, &nbPar, &nbCro);
                
                
            }
            
            isPotentiallyNumber=false;
            isPotentiallyString=false;
            isPotentiallyString2=false; // chaine avec un guillemet court
            isPotentiallyWord=false;
            isPotentiallyOp=false;
            isPotentiallyComm=false;
            isPotentiallyLongComm=false;
                    
            
            
            
            
            // reinitialise les variables qui ont servi au traitement de ce sous-token
            //strlist_destroy(&tokenAdd);
            
            err_free(tokenAdd.tab);
            
            tokenAdd = strlist_create(0);
            
            err_free(typeTok.tab);//on ne la détruit pas car elle contient des éléments de type TYPE_CROCHET, TYPE_VARIABLE donc des chaines de caractères littérales
            typeTok = intlist_create(0);
            
            
            nombrePoints=0;
            debTok=0;
            
            
        }
        
        

        
        
        //debut tokens simples
        debutTokensSimples(i, &debTok, &char1, &isPotentiallyString, &isPotentiallyNumber,&isPotentiallyWord, &isPotentiallyOp, &isPotentiallyString2, &isPotentiallyComm, &isPotentiallyLongComm);
        
        
        // vérifie si le caractère est reconnu
        if (!isPotentiallyWord && !isPotentiallyString && !isPotentiallyNumber && !isPotentiallyOp && !isPotentiallyString2 && !isPotentiallyComm && !isPotentiallyLongComm && !nouvTok && char1 != ' ')
            quitError("Erreur de syntaxe3");
        
        char1_2 = charToString(char1);
        if (nouvTok) //si on a un nouveau token, on vérifie que le caractère actuel est accepté par la grammaire globale
        {
            if (!isalnum(char1) && !(strlist_inList(&acceptedChars, char1_2)))
                quitError("Erreur de syntaxe3.5");
        }
        err_free(char1_2);
        
        nouvTok=false;
        
        
        char2=char1;
        
        
    }
    
    err_free(typeTok.tab);
    
    // si une variable de contidion de détection de token est encore à True, faute de syntaxe
    if (isPotentiallyWord || isPotentiallyString || isPotentiallyNumber || isPotentiallyOp  || isPotentiallyString2 || isPotentiallyComm || isPotentiallyLongComm || nbAcc!=0 || nbPar!=0 || nbCro!=0)
        quitError("Erreur de syntaxe4");
    
    
    
    // verification de la syntaxe
    
    verificationGrammaire(tokens,types, &isPotentiallyExpr, &debExpr);
    
    
    
        
    
    // nouvelle boucle pour réunir les statements for, while et if/elif/else
    
    if (tokens->len == 0)
        return ;
    
    
    if (traiterStatements)
        statements(types, tokens, &isPotentiallyStat, &debStat);
    
    
    
    strlist_destroy(&tokenAdd);
    err_free(string);
    
    
    return ;
}














char* nomBlockLine(char* blockline)
{
	if (count(blockline, "(") > 0)
		return sub(blockline,0,sindex(blockline, '('));
	else
		return copy(blockline);
}






void finTokensSimples(char* string, _Bool* isPotentiallyNumber, _Bool* isPotentiallyString, _Bool* isPotentiallyWord, _Bool* isPotentiallyOp, _Bool* isPotentiallyLongComm, _Bool* isPotentiallyString2, _Bool* isPotentiallyComm, char * char2, char* char1, _Bool* nouvTok, strlist* tokenAdd, intlist* typeTok, unsigned* debTok, unsigned i, unsigned* stepNumber)
{
    /*
    fonction interne à cut permettant de mettre fin à la détection des tokens simples
    */
    // chaque condition permet de mettre un terme à la détection d'un token en particulier
        
    // les listes typeTok et tokenAdd permettent de mettre fin à la détection de plusieurs tokens dans la même itération
    
    
    if ((*char2)=='"' && (*isPotentiallyString) && i-1!=(*debTok)) // fin de chaîne si le guillemet etudie n’est pas le meme que le premier de la chaine
    {
        char* sousch = sub(string, (*debTok), i);
        strlist_append(tokenAdd,sousch);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_STRING[0]);
        (*isPotentiallyString)=false;
    }
    
    if ((*char2)=='\'' && (*isPotentiallyString2) && i-1!=(*debTok)) // fin de chaîne si le guillemet etudie n’est pas le meme que le premier de la chaine
    {
        char* sousch = sub(string, (*debTok), i);
        strlist_append(tokenAdd,sousch);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_STRING[0]);
        (*isPotentiallyString2)=false;
    }
    
    if ((((*char1)=='.' && (*stepNumber)==2) || (!isdigit(*char1) && (*char1) != '.')) && (*isPotentiallyNumber) && !((*stepNumber)==1)) // fin de nombre
    {
        char* nb = sub(string, (*debTok), i);
        strlist_append(tokenAdd,nb);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_NUMBER[0]);
        (*isPotentiallyNumber)=false;
    }
    
    
    if ((*isPotentiallyWord) && !(isalnum((*char1)) || (*char1)=='_')) // fin word
    {
        strlist_append(tokenAdd,sub(string, (*debTok), i));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_WORD[0]);
        (*isPotentiallyWord)=false;
    }
    
    
    if ((*isPotentiallyComm) && (*char1)=='\n') // fin commentaire
    {
        strlist_append(tokenAdd,sub(string, (*debTok), i));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_COMMENT[0]);
        (*isPotentiallyComm)=false;
    }
        
    
    
    
        
    if ((*char2)=='$' && (*isPotentiallyLongComm) && (i-1)!=(*debTok)) // fin de chaîne si le guillemet etudie n’est pas le meme que le premier de la chaine
    {
        
        strlist_append(tokenAdd,sub(string, (*debTok), i));
        
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_COMMENT[0]);
        
        (*isPotentiallyLongComm)=false;
    }
    
    
    char* sousch = sub(string, (*debTok), i+1);
    if ((*isPotentiallyOp) && !strlist_inList(&operateurs1, sousch)) // fin operateur
    {
        strlist_append(tokenAdd,sub(string, (*debTok), i));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_OPERATOR[0]);
        (*isPotentiallyOp)=false;
    }
    
    err_free(sousch); // c'est cette fonction qui pose problème, ici
    
    // elements d’un caractere
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyLongComm) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && ((*char1)=='(' || (*char1)==')'))
    {
        (*nouvTok)=true;
        strlist_append(tokenAdd,charToString(*char1));
        if ((*char1)=='(')
            intlist_append(typeTok,TYPE_PARENTHESE1[0]);
        if ((*char1)==')')
            intlist_append(typeTok,TYPE_PARENTHESE2[0]);
        
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyLongComm) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && ((*char1)=='{' || (*char1)=='}'))
    {
        strlist_append(tokenAdd,charToString(*char1));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_ACCOLADE[0]);
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyLongComm) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && ((*char1)=='[' || (*char1)==']'))
    {
        strlist_append(tokenAdd,charToString(*char1));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_CROCHET[0]);
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyLongComm) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && (*char1)==',')
    {
        strlist_append(tokenAdd,charToString(*char1));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_VIRGULE[0]);
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyLongComm) && ((*char1)==';' || (*char1)=='\n'))
    {
        strlist_append(tokenAdd,charToString(*char1));
        
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_ENDOFLINE[0]);
        (*isPotentiallyNumber)=false;
        
        if ((*isPotentiallyString) || (*isPotentiallyString2))
        {
    			quitError("Erreur de syntaxe1");
        }
        
        
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
        (*isPotentiallyComm)=false;
    }
    
    return ;
}





void debutTokensSimples(unsigned i, unsigned* debTok, char* char1, _Bool* isPotentiallyString, _Bool* isPotentiallyNumber, _Bool* isPotentiallyWord, _Bool* isPotentiallyOp, _Bool* isPotentiallyString2, _Bool* isPotentiallyComm, _Bool* isPotentiallyLongComm)
{
    if ((*char1)=='"' && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // début de chaîne
    {
        (*debTok)=i;
        (*isPotentiallyString)=true;
    }
    
    
    if ((*char1)=='#' && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // début de commentaire
    {
        (*debTok)=i;
        (*isPotentiallyComm)=true;
    }
        
    if ((*char1)=='$' && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // début de commentaire long
    {
        (*debTok)=i;
        (*isPotentiallyLongComm)=true;
    }
    
    
    
    if ((*char1)=='\'' && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // début de chaîne
    {
        (*debTok)=i;
        (*isPotentiallyString2)=true;
    }
    
    
    
    
    if (isdigit((*char1)) && !(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // début de nombre
    {
        (*debTok)=i;
        (*isPotentiallyNumber)=true;
    }
    
    
    
    if ((isalpha((*char1)) || (*char1)=='_') && !(*isPotentiallyWord) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyOp)  && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // debut word
    {
        (*debTok)=i;
        (*isPotentiallyWord)=true;
    }
    
    char *char1_2 = charToString(*char1);
    if (strlist_inList(&sousop, char1_2) && !(*isPotentiallyWord) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyOp)  && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // debut operateur
    {
        (*debTok)=i;
        (*isPotentiallyOp)=true;
    }
    
    err_free(char1_2);
    
    return ;
}





void annulerOuPoursuivre(_Bool* isPotentiallyFonc, unsigned* foncStep, _Bool* isPotentiallyInst, unsigned* instStep, _Bool* isPotentiallyListIndex, unsigned* listIndexStep, strlist* tokenAdd, unsigned k, unsigned *foncParDeb, unsigned *listeParDeb, unsigned *indexParDeb)
{
    if ((*isPotentiallyFonc) && (*foncStep)==1) // annuler ou poursuivre la fonction
    {
        if (tokenAdd->tab[k][0]=='(' && strlen(tokenAdd->tab[k])==1)
            (*foncStep)=2;
        else
        {
            (*foncStep)=0;
            (*isPotentiallyFonc)=false;
            *foncParDeb = 0;
        }
    }
    
    
    
    if ((*isPotentiallyInst) && (*instStep)==1) // annuler ou poursuivre l’instruction de bloc
    {
        if (tokenAdd->tab[k][0]=='(' && strlen(tokenAdd->tab[k])==1)
            (*instStep)=2;
        else
        {
            (*instStep)=0;
            (*isPotentiallyInst)=false;
        }
    }
            
    
    
    if ((*isPotentiallyListIndex) && (*listIndexStep)==1) // annuler ou poursuivre l’index de liste
    {
        if (tokenAdd->tab[k][0]=='[' && strlen(tokenAdd->tab[k])==1)
            (*listIndexStep)=2;
        else
        {
            (*listIndexStep)=0;
            (*isPotentiallyListIndex)=false;
            *indexParDeb = 0;
        }
    }

    if ((*isPotentiallyListIndex) && (*listIndexStep)==2 && tokenAdd->tab[k][0] != ']' && tokenAdd->tab[k][0] != '[')
        (*listIndexStep)=3;
    
    return;
}






void debutTokensComposes(unsigned k, intlist* typeTok, strlist* tokenAdd, unsigned* debTok2, unsigned* debTok3, strlist* tokens, intlist *types, _Bool* nouvTokComp, _Bool* isPotentiallyList, _Bool* isPotentiallyFonc, _Bool* isPotentiallyListIndex, _Bool* isPotentiallyComm, _Bool* isPotentiallyBlock, _Bool* isPotentiallyInst, _Bool* isPotentiallyLongComm, unsigned* listIndexStep, unsigned *foncStep, unsigned *instStep, unsigned *foncParDeb, unsigned *listeParDeb, unsigned *indexParDeb, int* nbPar, int* nbCro)
{
    if (tokenAdd->tab[k][0] =='[' && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyComm) && !(*isPotentiallyBlock) && !(*isPotentiallyInst) && !(*isPotentiallyLongComm)) // debut de liste
    {
        (*debTok2)=tokens->len-1;
        (*isPotentiallyList)=true;
    }



    if (tokenAdd->tab[k][0] =='{' && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyBlock) && !(*isPotentiallyComm) && !(*isPotentiallyInst) && !(*isPotentiallyLongComm)) // debut de bloc d’instructions
    {
        (*debTok3)=tokens->len-1;
        (*isPotentiallyBlock)=true;
    }


                    

    if (typeTok->tab[k] == TYPE_VARIABLE[0] && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyComm) && !(*isPotentiallyBlock) && !(*isPotentiallyInst) && !(*isPotentiallyLongComm)) // debut de fonction et d’index de liste
    {
       (*debTok2)=tokens->len-1;
       (*isPotentiallyFonc)=true;
       (*isPotentiallyListIndex)=true;
       (*listIndexStep)=1;
       (*foncStep)=1;
       
       *foncParDeb = *nbPar;
       *indexParDeb = *nbCro;
    }
    


    if (typeTok->tab[k] == TYPE_BLOCKWORD[0] && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyComm) && !(*isPotentiallyInst)  && !(*isPotentiallyLongComm)) // debut d’instruction de bloc
    {
       (*debTok2)=tokens->len-1;
       (*isPotentiallyInst)=true;
       (*instStep)=1;
    }


    if ((*nouvTokComp)) // reinitialise les variables de detection de tokens car on repart a zero avec un nouveau token
    {
        (*isPotentiallyListIndex)=false;
        (*isPotentiallyList)=false;
        (*isPotentiallyFonc)=false;
        //(*isPotentiallyBlock)=false;
        (*isPotentiallyInst)=false;
        
        (*nouvTokComp)=false;
    }
    return;
}











void finsTypesComposes(unsigned *debTok2, unsigned *debTok3, unsigned k, _Bool *nouvTokComp, _Bool *isPotentiallyFonc, unsigned *foncStep, strlist* tokenAdd, unsigned *nbPar, strlist* tokens, intlist* types, _Bool *isPotentiallyInst, unsigned *instStep, unsigned *listIndexStep, _Bool *isPotentiallyList, _Bool *isPotentiallyListIndex, unsigned *nbCro, _Bool *isPotentiallyBlock, unsigned *nbAcc, unsigned *foncParDeb, unsigned *listeParDeb, unsigned *indexParDeb)
{
    if ((*isPotentiallyFonc) && (*foncStep)==2 && tokenAdd->tab[k][0] == ')' && (*nbPar - *foncParDeb)==0) // fin fonction
    {
        //printf("fin fonction : %s et k=%u\n", tokenAdd->tab[k], tokens->len-1);
        (*isPotentiallyFonc)=false;
        (*foncStep)=0;
        char* sommeTokens = copy("");
        unsigned len = tokens->len;
        for (unsigned i=0 ; i < len - (*debTok2) ; i++)
        {
            //printf("iteration de la boucle i=%u\n", i);
            if (types->tab[(*debTok2)] == TYPE_OPERATOR[0])
            {
                char* tmp1 = sandwich(tokens->tab[(*debTok2)], ' ');
                char* tmp2 = addStr(sommeTokens, tmp1);
                err_free(sommeTokens);err_free(tmp1);
                sommeTokens = tmp2;
            }
            else if (types->tab[(*debTok2)] != TYPE_ENDOFLINE[0])
            {
                char* tmp1 = addStr(sommeTokens, tokens->tab[(*debTok2)]);

                err_free(sommeTokens);
                sommeTokens = tmp1;
            }
            strlist_remove(tokens, *debTok2, true);
            intlist_remove(types, *debTok2);
        }

        strlist_append(tokens, sommeTokens);
        intlist_append(types, TYPE_FONCTION[0]);
        (*nouvTokComp)=true;
        *foncParDeb = 0;
    }

    else if ((*isPotentiallyFonc) && (*foncStep)==2 && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
      quitError("Erreur de syntaxe8");
    
    
    
    if ((*isPotentiallyInst) && (*instStep)==2 && tokenAdd->tab[k][0] == ')' && (*nbPar)==0) // fin instruction de bloc
    {
        (*isPotentiallyInst)=false;
        
        (*instStep)=0;
        char* sommeTokens = copy("");
        unsigned len = tokens->len;
        for (unsigned i=0 ; i < len - (*debTok2) ; i++)
        {
            if (types->tab[(*debTok2)] == TYPE_OPERATOR[0])
            {
                char* tmp1 = sandwich(tokens->tab[(*debTok2)], ' ');
                char* tmp2 = addStr(sommeTokens, tmp1);
                
                err_free(sommeTokens);err_free(tmp1);
                sommeTokens = tmp2;
            }
            else if (types->tab[(*debTok2)] != TYPE_ENDOFLINE[0])
            {
                char* tmp1 = addStr(sommeTokens, tokens->tab[(*debTok2)]);
                
                err_free(sommeTokens);
                
                sommeTokens = tmp1;
            }
            
            strlist_remove(tokens, *debTok2, true);
            
            intlist_remove(types, *debTok2);
            
        }
        
        strlist_append(tokens, sommeTokens);
        intlist_append(types, TYPE_BLOCKLINE[0]);
        (*nouvTokComp)=true;
    }
    
    else if ((*isPotentiallyInst) && (*instStep)==2 && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
      quitError("Erreur de syntaxe8");
        
    
    
    if ((*isPotentiallyListIndex) && (*listIndexStep)==3 && tokenAdd->tab[k][0] == ']' && (*nbCro - *indexParDeb)==0) // fin index liste
    {
        (*isPotentiallyListIndex)=false;
        (*listIndexStep)=0;
        char* sommeTokens = copy("");
        unsigned len = tokens->len;
        for (unsigned i=0 ; i < len - (*debTok2) ; i++)
        {
            if (types->tab[(*debTok2)] == TYPE_OPERATOR[0])
            {
                char* tmp1 = sandwich(tokens->tab[(*debTok2)], ' ');
                char* tmp2 = addStr(sommeTokens, tmp1);
                err_free(sommeTokens);err_free(tmp1);
                sommeTokens = tmp2;
            }
            else if (types->tab[(*debTok2)] != TYPE_ENDOFLINE[0])
            {
                char* tmp1 = addStr(sommeTokens, tokens->tab[(*debTok2)]);
                err_free(sommeTokens);
                sommeTokens = tmp1;
            }
            strlist_remove(tokens, *debTok2, true);
            intlist_remove(types, *debTok2);
        }
        
        strlist_append(tokens, sommeTokens);
        intlist_append(types, TYPE_LISTINDEX[0]);
        (*nouvTokComp)=true;
        *indexParDeb = 0;
    }
      
    else if ((*isPotentiallyListIndex) && ((*listIndexStep)==2 || (*listIndexStep)==3) && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
      quitError("Erreur de syntaxe8");
      
    
    
    
    
    if (tokenAdd->tab[k][0] ==']' && (*isPotentiallyList) && *nbCro == 0)// fin de liste
    {
        (*isPotentiallyList)=false;
        char* sommeTokens = copy("");
    
        
        unsigned len = tokens->len;
        for (unsigned i=0 ; i < len - (*debTok2) ; i++)
        {
            if (types->tab[(*debTok2)] == TYPE_OPERATOR[0])
            {
                char* tmp1 = sandwich(tokens->tab[(*debTok2)], ' ');
                char* tmp2 = addStr(sommeTokens, tmp1);
                err_free(sommeTokens);err_free(tmp1);
                sommeTokens = tmp2;
            }
            else if (types->tab[(*debTok2)] != TYPE_ENDOFLINE[0])
            {
                char* tmp1 = addStr(sommeTokens, tokens->tab[(*debTok2)]);
                err_free(sommeTokens);
                sommeTokens = tmp1;
            }
            strlist_remove(tokens, *debTok2, true);
            intlist_remove(types, *debTok2);
        }
        
        strlist_append(tokens, sommeTokens);
        intlist_append(types, TYPE_LIST[0]);
        (*nouvTokComp)=true;

    }

    else if ((*isPotentiallyList) && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
      quitError("Erreur de syntaxe8");

    
    
    if (tokenAdd->tab[k][0] =='}' && (*isPotentiallyBlock) && (*nbAcc)==0)// fin de bloc d’instructions
    {
        (*isPotentiallyBlock)=false;
        char* sommeTokens = copy("");
        unsigned len = tokens->len;
        for (unsigned i=0 ; i < len - (*debTok3) ; i++)
        {
            if (types->tab[(*debTok3)] == TYPE_OPERATOR[0])
            {
                char* tmp1 = sandwich(tokens->tab[(*debTok3)], ' ');
                char* tmp2 = addStr(sommeTokens, tmp1);
                err_free(sommeTokens);err_free(tmp1);
                sommeTokens = tmp2;
            }
            else
            {
                char* tmp1 = addStr(sommeTokens, tokens->tab[(*debTok3)]);
                err_free(sommeTokens);
                sommeTokens = tmp1;
            }
            strlist_remove(tokens, *debTok3, true);
            intlist_remove(types, *debTok3);
        }
        
        strlist_append(tokens, sommeTokens);
        intlist_append(types, TYPE_BLOCK[0]);
        (*nouvTokComp)=true;
    }
    
    
    return;
}






void empaquetageExprInst(unsigned* j, strlist* tokens, intlist* types, _Bool* isPotentiallyExpr, unsigned * debExpr, char* typeact)
{
    if ((*isPotentiallyExpr) && (*typeact)==TYPE_ENDOFLINE[0])
    {
        (*isPotentiallyExpr)=false;
        
        strlist_remove(tokens, *debExpr, true);
        intlist_remove(types, *debExpr);
        
        char* sommeTokens=copy("");
        for (unsigned i=(*debExpr);i<(*j)-1;i++)
        {
            if (types->tab[(*debExpr)] == TYPE_OPERATOR[0])
            {
                char* tmp1 = (tokens->tab[*debExpr][0] == '_') ? copy(" -") : sandwich(tokens->tab[(*debExpr)], ' ');
                char* tmp2 = addStr(sommeTokens, tmp1);
                err_free(sommeTokens);err_free(tmp1);
                sommeTokens = tmp2;
            }
            else
            {
                char* tmp1 = addStr(sommeTokens, tokens->tab[(*debExpr)]);
                err_free(sommeTokens);
                sommeTokens = tmp1;
            }

            strlist_remove(tokens, *debExpr, true);
            intlist_remove(types, *debExpr);
        }
        
        strlist_insert(tokens, sommeTokens, *debExpr);
        intlist_insert(types, TYPE_EXPRESSION[0], *debExpr);
        (*j)=(*debExpr)+1;
        (*debExpr)=0;
    }
    
    
    if ((*typeact)==TYPE_ENDOFLINE[0])
    {
        (*isPotentiallyExpr)=true;
        (*debExpr)=(*j);
    }
        
    if ((*isPotentiallyExpr) && ((*typeact)==TYPE_BLOCK[0] || (*typeact)==TYPE_BLOCKLINE[0] || (*typeact)==TYPE_KEYWORD[0]))
    {
        (*isPotentiallyExpr)=false;
        strlist_remove(tokens, *debExpr, true);
        intlist_remove(types, *debExpr);
        (*j)-=1;
        
        (*debExpr)=0;
    }
        
    if ((*j)==tokens->len-1 && (*typeact)==TYPE_ENDOFLINE[0])
    {
        strlist_remove(tokens, *j, true);
        intlist_remove(types, *j);
    }
}







void verificationGrammaire(strlist* tokens, intlist* types, _Bool* isPotentiallyExpr, unsigned* debExpr)
{
    int j=0;
    int grammanc; char typeact; char typeanc; int gramm; int i; _Bool isCorresponding; int grammact; _Bool corresponding; int k;
    _Bool isPotentiallyStat = false;
    
    
    while (j < tokens->len ) // on va verifier que le chaque suite de token est valide par la grammaire (verfication de grammaire)
    {
    
        // on va se servir de cette boucle pour gérer les '-' unaires et binaires
        
        
        grammanc=0;
        
        if (j > 0 && types->tab[j-1]==TYPE_OPERATOR[0])
        {
            
            grammanc=gramm1.tab[strlist_index(&operateurs3, tokens->tab[j-1])];
            
        }
        
        // ici on s'occuppe de modifier les signes '-'
        if (types->tab[j]==TYPE_OPERATOR[0] && tokens->tab[j][0] == '-') // si on a un opérateur et que cet opérateur est un moins
        {
            if (j==0 && j < tokens->len - 1)
            {
                err_free(tokens->tab[j]);
                tokens->tab[j] = copy("_"); // si j=0 (c'est-à-dire que c'est le premier token), on sait que c'est forcément un moins unaire
                
            }
            
            
            else if (!(types->tab[j-1] == TYPE_NUMBER[0] || types->tab[j-1] == TYPE_STRING[0] || types->tab[j-1] == TYPE_LISTINDEX[0] || types->tab[j-1] == TYPE_FONCTION[0] || types->tab[j-1] == TYPE_LIST[0] || types->tab[j-1] == TYPE_BOOL[0] || types->tab[j-1] == TYPE_CONST[0] || types->tab[j-1] == TYPE_NONE[0] || types->tab[j-1] == TYPE_PARENTHESE1[0] || types->tab[j-1] == TYPE_VARIABLE[0] || (types->tab[j-1]==TYPE_OPERATOR[0] && grammanc == VARLEFT)) )
            {
                // si on a un moins qui est directement précédé d'une autre unité lexicale de type nombre etc, ça veut dire que c'est l'opérateur moins donc
                // là on teste la condition inverse c'est à dire que l'on se trouve en présence d'un moins unaire
                // on va intégrer le moins à l'unité lexicale à qui il est adressé
                if (j < tokens->len - 1) // si le moins étudié n'est pas le dernier caractère des tokens
                {
                    err_free(tokens->tab[j]);
                    tokens->tab[j] = copy("_");
                }
            }
        }
        
        typeact=types->tab[j];
        
        if (j>0)
            typeanc=types->tab[j-1];
        
        
        
        if (j > 0 )
        {
            //determine l’index du token actuel dans la liste syntax
            i=0;
            
            
            if (typeanc == TYPE_BLOCKLINE[0])
                isPotentiallyStat = true;
            
            
            
            if (isPotentiallyStat && typeact != TYPE_ENDOFLINE[0] && typeact != TYPE_BLOCK[0] || (typeact==TYPE_BLOCKLINE[0] || isPotentiallyStat) && j==tokens->len-1)
                quitError("Erreur de syntaxe9");
            
            else if (isPotentiallyStat && typeact==TYPE_BLOCK[0])
                isPotentiallyStat = false;
            
            
            
            
            // petit algoithme qui determine si le type de token est accepte en token final
            i=0;
            isCorresponding=false;
            for (unsigned l=0 ; l < syntax.len ; l++)
            {
                if (syntax.tab[l].tab[0][0] == typeact)
                {
                    isCorresponding=true;
                    i=l;
                    
                    break;
                }
            }
            
            if (!isCorresponding) // le type actuel n'est pas accepté, il y a donc erreur de syntaxe
            {
                quitError("Erreur de  syntaxe5");
            }
            
            // algoithme qui verifie si le token precedent est accepte par la grammaire avant le token actuel
            corresponding=false;
            for (unsigned k = 1 ; k < (syntax.tab[i]).len ; k++) // parcourt la liste des types attendus
            {
                
                gramm=0;
                if (typeanc == syntax.tab[i].tab[k][0]) // si ca correspond on va vérifier si c'est un opérateur si ça c||respond avec ce qu'accepte l'opérateur
                {
                    //comme opérandes ou si le type précédent est un opérateur on vérifie que le type actuel est bien accepté par l'opérateur
                    
                    if (typeanc==TYPE_OPERATOR[0]) // si c'est le token d'avant qui est un opérateur
                    {
                        //recupere l’operateur pour verifier que ca c||respond
                        gramm=gramm1.tab[strlist_index(&operateurs3, tokens->tab[j-1])];
                        
                        
                        if (typeact==TYPE_OPERATOR[0])
                            grammact=gramm1.tab[strlist_index(&operateurs3, tokens->tab[j])];
                        
                        
                        
                        // on verifie que le token peut effectivement etre apres l’operateur
                        if ((gramm==VARLEFT && typeact != TYPE_ENDOFLINE[0] && typeact != TYPE_VIRGULE[0] && typeact != TYPE_PARENTHESE2[0] && !(typeact==TYPE_OPERATOR[0] && grammact==RIGHT_LEFT)) || (gramm==VARRIGHT && !(typeact==TYPE_LISTINDEX[0] || typeact==TYPE_VARIABLE[0])) || ((gramm==RIGHT_LEFT || gramm==VAR_RIGHT || gramm==RIGHT) && (typeact==TYPE_ENDOFLINE[0] || typeact==TYPE_VIRGULE[0] || typeact==TYPE_PARENTHESE2[0])) || (gramm==VARRIGHT && typeact != TYPE_VARIABLE[0]))
                        {
                            quitError("Erreur de  syntaxe5");
                        }
                    }
                    
                            
                    if (typeact == TYPE_OPERATOR[0]) // si le type actuel est un operateur, on verifie qu’il accepte bien l’élement d’avant
                    {
                        gramm=gramm1.tab[strlist_index(&operateurs3, tokens->tab[j])];
                        
                        if (typeanc==TYPE_OPERATOR[0])
                            grammanc=gramm1.tab[strlist_index(&operateurs3, tokens->tab[j-1])];
                        
                        if (((gramm==VARLEFT || gramm==VAR_RIGHT) && typeanc != TYPE_VARIABLE[0] && typeanc != TYPE_LISTINDEX[0]) || ((gramm==RIGHT || gramm==VARRIGHT)  && (typeanc==TYPE_NUMBER[0] || typeanc==TYPE_STRING[0] || typeanc==TYPE_LIST[0] ||  typeanc==TYPE_VARIABLE[0] ||  typeanc==TYPE_FONCTION[0] ||  typeanc==TYPE_BOOL[0] || typeanc==TYPE_CONST[0] || typeanc==TYPE_NONE[0] || typeanc==TYPE_LISTINDEX[0])) || gramm==RIGHT_LEFT && !(typeanc==TYPE_NUMBER[0] || typeanc==TYPE_STRING[0] || typeanc==TYPE_LIST[0] ||  typeanc==TYPE_VARIABLE[0] ||  typeanc==TYPE_FONCTION[0] ||  typeanc==TYPE_BOOL[0] ||  typeanc==TYPE_LISTINDEX[0] || typeanc==TYPE_CONST[0] || typeanc==TYPE_NONE[0] || typeanc==TYPE_PARENTHESE2[0] || (typeanc==TYPE_OPERATOR[0] && grammanc==VARLEFT)))
                        {
                            quitError("Erreur de syntaxe5");
                        }
                    }
                            
                    corresponding=true;
                    break;
                
                }
                
            }
                    
            if (corresponding==false) // si ca correspond a aucun type attendu
            {
                quitError("Erreur de  syntaxe5");
            }
        }
        
        
        //ici empaquetage des expressions et des blocs d’instructions
        
        empaquetageExprInst(&j, tokens, types, isPotentiallyExpr, debExpr, &typeact);
        
        j+=1;
    }

}







void compConditionBlock(unsigned cas, _Bool* isPotentiallyConBlock, unsigned *conBlockStep, strlist* tokens, intlist* types, unsigned *i, char* typeact, unsigned* debConBlock) // fonction qui réunit les blocs if/elif/else ensemble
{
    // on distingue deux cas différents ) le cas où on ne prend pas le token actuel (cas 1), et le cas où on le prend (cas2)
    
    if (cas == 1) // cas 1
    {
        *isPotentiallyConBlock = false;
        *conBlockStep = 0;
        char* sommeTokens = copy("");
        
        for (unsigned k=(*debConBlock) ; k < (*i)-1 ; k++)
        {
            
            char* tmp1 = addStr(sommeTokens, tokens->tab[*debConBlock]);
            
            err_free(sommeTokens);
            sommeTokens = tmp1;
            strlist_remove(tokens, *debConBlock, true);
            intlist_remove(types, *debConBlock);
        }
        
        char* tmp1 = addStr(sommeTokens, tokens->tab[*debConBlock]);
        err_free(sommeTokens);
        err_free(tokens->tab[*debConBlock]);
        tokens->tab[*debConBlock] = tmp1;
        types->tab[*debConBlock] = TYPE_CONDITIONBLOCK[0];
        *typeact = TYPE_CONDITIONBLOCK[0];
        *i=*debConBlock;
    }
        
    else if (cas == 2)
    {
        
        // cas 2
        *conBlockStep = 0;
        *isPotentiallyConBlock = false;
        char* sommeTokens = copy("");
        
        
        for (unsigned k= *debConBlock ; k < *i ; k++)
        {
            char* tmp1 = addStr(sommeTokens, tokens->tab[*debConBlock]);
            err_free(sommeTokens);
            sommeTokens = tmp1;
            strlist_remove(tokens, *debConBlock, true);
            intlist_remove(types, *debConBlock);
        }
        
        
        char* tmp1 = addStr(sommeTokens, tokens->tab[*debConBlock]);
        err_free(sommeTokens);
        err_free(tokens->tab[*debConBlock]);
        tokens->tab[*debConBlock] = tmp1;
        types->tab[*debConBlock] = TYPE_CONDITIONBLOCK[0];
        *typeact = TYPE_CONDITIONBLOCK[0];
        *i=*debConBlock;
    }
    
    
    return ;

}








void statements(intlist* types, strlist* tokens, _Bool *isPotentiallyStat, unsigned *debStat)
{
    
    _Bool isPotentiallyConBlock = false;
    unsigned conBlockStep = 0;
    unsigned debConBlock = 0;
    unsigned i=0;
    char typeact=types->tab[0];
    
    
    
    while (i < tokens->len)
    {
        
        // modifie le blockline pour savoir quel bloc c'est particulièrement
        if (types->tab[i]==TYPE_BLOCKLINE[0] || (types->tab[i] == TYPE_KEYWORD[0] && !strlist_inList(&lkeywords, tokens->tab[i])))
        {
            // ici il faut créer le TYPE_BLOCKTYPE
            char* nom = nomBlockLine(tokens->tab[i]);
            if (strcmp(nom, "if")==0)
                types->tab[i] = TYPE_BLOCKIF[0];
            
            if (strcmp(nom, "elif")==0)
                types->tab[i] = TYPE_BLOCKELIF[0];
            
            if (strcmp(nom, "else")==0)
                types->tab[i] = TYPE_BLOCKELSE[0];
            
            if (strcmp(nom, "while")==0)
                types->tab[i] = TYPE_BLOCKWHILE[0];
            
            if (strcmp(nom, "for")==0)
                types->tab[i] = TYPE_BLOCKFOR[0];
            err_free(nom);
        }
            
        
        typeact=types->tab[i];
        
        
        //--debut--//
        
        
        if ((*isPotentiallyStat) && typeact==TYPE_BLOCK[0]) // fin de statement, on l'empaquette
        {
            
            // ici, il faut créer le STATEMENTTYPE
            //on concatène les deux chaines de caractères
            char* add = addStr(tokens->tab[(*debStat)], tokens->tab[i]);
            
            err_free(tokens->tab[(*debStat)]);err_free(tokens->tab[i]);
            
            tokens->tab[*debStat] = add;
            
            
            
            switch (types->tab[*debStat])
            {
                case '5':
                    types->tab[*debStat] = '0';
                    break;
                
                case '6':
                    types->tab[*debStat] = '1';
                    break;
                
                case '7':
                    types->tab[*debStat] = '2';
                    break;
                
                case '8':
                    types->tab[*debStat] = '3';
                    break;
                
                case '9':
                    types->tab[*debStat] = '4';
                    break;
            }
            
             // on transforme un block en statement par une soustraction par 5;
            
            strlist_remove(tokens, i, false);// cet élément n'existe pas
            
            intlist_remove(types, i);
            
            
            (*isPotentiallyStat)=false;
            (*debStat)=0;
            
            
            // modifie le nouveau token que l'on étudie maintenant au cas où c'est un blockline ou un keyword
            
            
            
            if (i<tokens->len)
                if (types->tab[i] == TYPE_BLOCKLINE[0] || types->tab[i] == TYPE_KEYWORD[0])
                {
                    // il faut ici créer le TYPE_BLOCKTYPE
                    char* nom = nomBlockLine(tokens->tab[i]);
                    if (strcmp(nom, "if")==0)
                        types->tab[i] = TYPE_BLOCKIF[0];
                    
                    if (strcmp(nom, "elif")==0)
                        types->tab[i] = TYPE_BLOCKELIF[0];
            
                    if (strcmp(nom, "else")==0)
                        types->tab[i] = TYPE_BLOCKELSE[0];
                    
                    if (strcmp(nom, "while")==0)
                        types->tab[i] = TYPE_BLOCKWHILE[0];
            
                    if (strcmp(nom, "for")==0)
                        types->tab[i] = TYPE_BLOCKFOR[0];
                    err_free(nom);
                    
                }
            
            
            i-=1;
            typeact=types->tab[i];
        }
        
        
        else if (typeact == TYPE_BLOCK[0] && ! (*isPotentiallyStat))//un block non précédé d'un blockline
            quitError("Erreur de syntaxe10");
        
        
        // ------------------------ on réunit les if/else if/else en conditionblock ------------------------------------
        
        
        if (typeact == TYPE_EXPRESSION[0] || typeact == TYPE_STATEMENTFOR[0] || typeact == TYPE_STATEMENTWHILE[0] || typeact == TYPE_STATEMENTIF[0] || typeact == TYPE_STATEMENTELIF[0] || typeact == TYPE_STATEMENTELSE[0]) // on n'agit sur les conditionblock que si le type actuel est un type fini (càd pas un block ni un blockif, ni un blockelse etc)
        {

            // fin conditionblock
            
            
            if (typeact==TYPE_STATEMENTIF[0] && i==tokens->len-1 && ! isPotentiallyConBlock) // si le dernier token est un statementif (tout seul, on remplace juste le type par conditionblock
            {
                types->tab[i]=TYPE_CONDITIONBLOCK[0];
                typeact=types->tab[i];
            }
            
            
            if (isPotentiallyConBlock && (typeact != TYPE_STATEMENTIF[0] && typeact != TYPE_STATEMENTELIF[0] && typeact != TYPE_STATEMENTELSE[0])) // dans le cas où l'on tombe sur un token autre que if/else if/else qui met fin au conditionblock
                compConditionBlock(1, &isPotentiallyConBlock, &conBlockStep, tokens, types, &i, &typeact, &debConBlock);
                
            
            // annuler ou poursuivre conditionblock
            if (typeact == TYPE_STATEMENTELIF[0] && isPotentiallyConBlock) // dans le cas où on a un statementelse if
                if ((conBlockStep == 1 || conBlockStep == 2) && i==tokens->len-1)
                    compConditionBlock(2, &isPotentiallyConBlock, &conBlockStep, tokens, types, &i, &typeact, &debConBlock);
                else if (conBlockStep == 1)
                        conBlockStep = 2;
                else if (conBlockStep == 3)
                    quitError("Erreur de syntaxe11");
            
            else if (typeact == TYPE_STATEMENTELIF[0])
                quitError("Erreur de syntaxe11");
            
            
            
            if (typeact == TYPE_STATEMENTIF[0] && isPotentiallyConBlock && conBlockStep == 2) // dans le cas où on a un statementif
                
                compConditionBlock(1, &isPotentiallyConBlock, &conBlockStep, tokens, types, &i, &typeact, &debConBlock);
            
            
            if (typeact == TYPE_STATEMENTELSE[0] && isPotentiallyConBlock || typeact==TYPE_STATEMENTIF[0] && i==tokens->len-1) // dans le cas où on a un statementelse, c'est fini direct, exactement comme dans le cas ou on a un statementif (juste à la fin mais qui n'est pas tout seul
                compConditionBlock(2, &isPotentiallyConBlock, &conBlockStep, tokens, types, &i, &typeact, &debConBlock);
            
            else if (typeact==TYPE_STATEMENTELSE[0])
                quitError("Erreur de syntaxe11");
            
            
            
            
            // debut conditionblock
            if (typeact == TYPE_STATEMENTIF[0] && ! isPotentiallyConBlock)
            {
                isPotentiallyConBlock = true;
                conBlockStep = 1;
                debConBlock = i;
            }
        
        
        }
        
        
        
        
        
        
        
        if (typeact==TYPE_BLOCKFOR[0] || typeact==TYPE_BLOCKWHILE[0] || typeact==TYPE_BLOCKIF[0] || typeact==TYPE_BLOCKELSE[0] || typeact==TYPE_BLOCKELIF[0]) // debut statement
        {
            if (i==tokens->len-1)
                quitError("Erreur de syntaxe9");
            (*isPotentiallyStat)=true;
            (*debStat)=i;
        }
            
        
        i+=1;
        
    }
    
    return ;
}

