//creation du tableau de pointeurs generiques a liberer en cas d’erreur


 ptrlist allocptr;



//déclaration des variables globales à cut
strlist acceptedChars;
listlist syntax;
strlist sousop;
intlist gramm1;
strlist operateurs3;
strlist operateurs1;
strlist operateurs2;
strlist blockwords;
strlist boolean;
strlist keywords;
strlist lkeywords;
strlist constant;


strlist OPERATEURS;
intlist PRIORITE;
intlist OPERANDES;


//stockage des variables
strlist NOMS;
intlist TYPES;
intlist ADRESSES;
strlist VARIABLES;
listlist LISTES;

strlist NOMSBUILTINSFONC;



char* (*OPFONC[NBOPERATEURS])(char*,unsigned,char*,unsigned,int*);
char* (*BUILTINSFONC[NBBUILTINFONC])(strlist*, intlist*, int*);



unsigned nbTempList;