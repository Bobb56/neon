#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "headers.c"



/*


Documentation des erreurs de syntaxe :

Erreur de syntaxe 1 : Chaine de caractère non fermée en fin de ligne
Erreur de syntaxe 2 : Opérateur inconnu
Erreur de syntaxe 3 : Caractère inconnu
Erreur de syntaxe 4 : Chaine de caractères, liste, ou autre non terminée
Erreur de syntaxe 5 : Mauvaise utilisation d'un opérateur, deux types à la suite non compatibles, ou utilisation d'un type inconnu/incompatible
Erreur de syntaxe 7 : Nombre contenant plusieurs virgules décimales
Erreur de syntaxe 8 : Mot clé incompatible utilisé dans une expression
Erreur de syntaxe 9 : Ligne de bloc d'instructions non suivi par un bloc d'instructions (ex: un if() tout seul)
Erreur de syntaxe 10 : Bloc d'instructions tout seul
Erreur de syntaxe 11 : Mauvais agencement de blocs if/elif/else

Éventuelles fuites mémoires ou problèmes d'accès aux données à cause de tokenAdd : on ajoute à tokens des elements de tokenAdd sans les copier, et on ne supprime pas proprement tokenAdd

*/


extern int CODE_ERROR;







//déclaration des variables globales à cut
extern strlist acceptedChars;
extern listlist syntax;
extern intlist types_debut;
extern intlist types_fin;
extern strlist sousop;
extern intlist gramm1;
extern strlist operateurs3;
extern strlist operateurs1;
extern strlist operateurs2;
extern strlist blockwords;
extern strlist blockwords1Line;
extern strlist keywordFunction;
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





_Bool isFull(char* string)
{
    strlist* tokens = strlist_create(0);
    intlist types;
    cut(tokens, &types, string, true);

    strlist_destroy(tokens, true);
    err_free(types.tab);
    
    if (CODE_ERROR == 29 || CODE_ERROR == 26 || CODE_ERROR == 11 || CODE_ERROR == 77)
    {
        CODE_ERROR = 0;
        return false;
    }
    else if (CODE_ERROR != 0)
        return false;
    else
        return true;
}


int unitCharToInt(char car, const char base)
{
    if (car != '0' && car != '1' && base == 'b') // le binaire n'autorise que 0 ou 1
        CODE_ERROR = 73;
    
    if (isdigit(car))
        return car - 48;
    else if (car >= 'a' && car <= 'f')
        return car - 87;

    CODE_ERROR = 73;
    return 0;
}


long int binToDec(char* chaine, int debut, int longueur)
{
    int res = 0;
    for (int i = debut ; i < debut + longueur ; i++)
    {
        res *= 2;
        res += unitCharToInt(chaine[i], 'b');

        if (CODE_ERROR != 0)
            return 0;
        
    }
    return res;
}




long int hexToDec(char* chaine, int debut, int longueur)
{
    int res = 0;
    for (int i = debut ; i < debut + longueur ; i++)
    {
        res *= 16;
        res += unitCharToInt(chaine[i], 'h');
        
        if (CODE_ERROR != 0)
            return 0;
    }
    return res;
}



_Bool isString(char* string, char* test, int size)
{
    _Bool bo = true;
    for (int i = 0 ; i < size ; i++)
    {
        if (string[i] != test[i])
        {
            bo = false;
            break;
        }
    }
    return bo && !isalnum(string[size]);
}


void write_libtok(char* buf, char* string, int size_string, int size)
{
    for (int i=0 ; i < size_string ; i++)
        buf[i] = string[i];
    
    for (int i = size_string ; i < size ; i++)
        buf[i] = ' ';
}




void cut(strlist* tokens, intlist* types, char* str, _Bool traiterStatements)
{
    /*fonction qui coupe la chaine de caractères en tokens
    traiterStatements est un booléen qui indique s'il faut regrouper les statements*/

    char* string;

    
    
    if (traiterStatements)
        string = sandwich(str, '\n');
    else
        string = addStr(str, "\n");

    int len_string = strlen(string);//pour utiliser la longueur de string sans appeler a chaque fois strlen

    //-------------------- ALGORITHME QUI REMPLACE LES DO ET END PAR DES ACCOLADES ------------------------
    #define ouvrant "faire"
    #define longueur_ouvrant 5
    
    #define ouvrant2 "alors"
    #define longueur_ouvrant2 5

    #define fermant "Fin"
    #define longueur_fermant 3

    #define sep1 "SinonSi"
    #define longueur_sep1 7

    #define sep2 "Sinon"
    #define longueur_sep2 5

    #define sep3 "Essayer"
    #define longueur_sep3 7

    #define sep4 "SiErreur"
    #define longueur_sep4 8


    _Bool str1 = false;
    _Bool str2 = false;
    _Bool isPotentiallyWord = false;

    // après, il y a les commentaires, mais on s'en fout
    for (int k = 0 ; k < len_string ; k++)
    {

        if (isalnum(string[k]) || string[k] == '_' || string[k] == '\'' && !str1 && !str2)
            isPotentiallyWord = true;
        
        if (isPotentiallyWord && string[k] != '_' && string[k] != '\'' && !isalnum(string[k]))
            isPotentiallyWord = false;


        if (string[k] == '\'' && !isPotentiallyWord) str1 = !str1;
        if (string[k] == '"') str2 = !str2;

        if (!str1 && !str2 && !isalnum(string[k])) // pas dans les chaines de caractères, ni dans les noms de variable
        {
            if (string[k] == '|') // spécifique à Freon
                string[k] = ' ';
            
            if (k < len_string - longueur_ouvrant - 1) // pour le 'do'
            {
                if (isString(string + k + 1, ouvrant, longueur_ouvrant)) // remplacement du 'do'
                    write_libtok(string + k + 1, "{", 1, longueur_ouvrant);
            }
            
            if (k < len_string - longueur_ouvrant - 1) // pour le 'then'
            {
                if (isString(string + k + 1, ouvrant2, longueur_ouvrant2)) // remplacement du 'then'
                    write_libtok(string + k + 1, "{", 1, longueur_ouvrant2);
            }
    
            if (k < len_string - longueur_fermant - 1) // pour le 'end'
            {
                if (isString(string + k + 1, fermant, longueur_fermant)) // remplacement du 'end'
                    write_libtok(string + k + 1, "}", 1, longueur_fermant);
            }

            if (k < len_string - longueur_sep1 - 1) // pour le 'elif'
            {
                if (isString(string + k + 1, sep1, longueur_sep1)) // remplacement du 'elif'
                    write_libtok(string + k + 1, "} ei", 4, longueur_sep1);
            }

            if (k < len_string - longueur_sep2 - 1) // pour le 'else'
            {
                if (isString(string + k + 1, sep2, longueur_sep2)) // remplacement du 'else'
                    write_libtok(string + k + 1, "}es{", 4, longueur_sep2);
            }

            if (k < len_string - longueur_sep3 - 1) // pour le 'try'
            {
                if (isString(string + k + 1, sep3, longueur_sep3)) // remplacement du 'try'
                    write_libtok(string + k + 1, "tr{", 3, longueur_sep3);
            }

            if (k < len_string - longueur_sep4 - 1) // pour le 'except'
            {
                if (isString(string + k + 1, sep4, longueur_sep4)) // remplacement du 'except'
                    write_libtok(string + k + 1, "} expt", 6, longueur_sep4);
            }
        }
    }

    //--------------------------- FIN DE L'ALGORITHME DE REMPLACEMENT ------------------------------------
        
    *types = intlist_create(0);
    _Bool nouvTok=false; //on modifie la valeur si on a un nouveau token à ajouter
    _Bool nouvTokComp=false; //on modifie la valeur quand on ajoute un nouveau token compose dans tokens
    intlist typeTok = intlist_create(0); // type du nouveau token, A LIBERER
    strlist* tokenAdd = strlist_create(0); // token a ajouter A LIBERER
    
    
    // pour les sous-tokens
    _Bool isPotentiallyNumber=false;
    int stepNumber = 0; // définit à quelle étape de définition de nombre on est lors de la détection
    _Bool isPotentiallyHexBin=false;
    int stepHexBin = 0;
    _Bool isPotentiallyString=false;
    _Bool isPotentiallyString2=false; // chaine avec un guillemet court
          isPotentiallyWord=false;
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
    
    int foncParDeb=0; // va contenir le nombre de parenthèses déjà traversées au moment où on arrive à la fonction
    int listeParDeb=0; // va contenir le nombre de parenthèses déjà traversées au moment où on arrive à la liste
    int indexParDeb=0; // va contenir le nombre de parenthèses déjà traversées au moment où on arrive à la fonction
    
    
    
    
    //etapes de detection par exemple variable, puis parenthese "(" puis parenthese ")"
    int listIndexStep=0;
    int foncStep=0;
    int instStep=0;
    
    
    int debTok=0; // contient la place de debut de detection du token etudie
    int debTok2=0; // contient la place de debut de detection du token compose etudie
    int debTok3=0;
    
    int debExpr=0; // va contenir la place de debut de l’expression dans le dernier algorithme
    _Bool isPotentiallyExpr=false; // pour la derniere boucle dans l’empaquetage d’expressions
    
    
    
    
    // pour la dernière boucle aussi, empaquetage des blocs conditionnels if/elif/else et for, while (statement)
    int debStat=0;
    _Bool isPotentiallyStat=false;
    
    
    
    
    char char2=0; // va contenir le caractere precedent
    char char1=0; // va contenir le caractère actuel
    char* char1_2; // peut servir d'intermédiaire quand on veut transformer char1 en chaine de caractères
    
    int nombrePoints=0; // nombre de points comptes apres le debut d’un nombre

    
    for (int i=0 ; i<len_string ; i++)
    {

        
        
        char1 = string[i];
        
        // comptage des variables relatives à la détection de nombres
        if (char1 == '.')
            nombrePoints+=1;
        if (isPotentiallyNumber && char1=='.' && nombrePoints==1 && i+1<len_string && isdigit(string [i+1]))
            stepNumber=1;
        else if (isPotentiallyNumber && char1=='.' && (i+1>=len_string || isdigit(string[i+1])))
        {
            CODE_ERROR = 2;// plusieurs virgules décimales
            err_free(string);
            strlist_destroy(tokenAdd, true);
            err_free(typeTok.tab);
            return;
        }
        
        if (isPotentiallyNumber && isdigit(char1) && stepNumber == 1)
          stepNumber = 2;

        //hex-bin
        if (stepHexBin == 0 && isPotentiallyHexBin)
        {
            if ((char1 == 'x' || char1 == 'b') && char2 == '0') // bonne syntaxe
            {
                stepHexBin  = 1;
                isPotentiallyNumber = false;
            }
            else
                isPotentiallyHexBin = false;
        }
        else if (stepHexBin == 1)
        {
            if (isdigit(char1) || (char1 >= 97 && char1 <= 102))
                stepHexBin = 2;
            else
            {
                CODE_ERROR = 73;
                err_free(string);
                strlist_destroy(tokenAdd, true);
                err_free(typeTok.tab);
                return ;
            }
        }

        
        //fins tokens simples
        finTokensSimples(string, &isPotentiallyNumber, &isPotentiallyString, &isPotentiallyWord, &isPotentiallyOp, &isPotentiallyLongComm, &isPotentiallyString2, &isPotentiallyComm, &isPotentiallyHexBin, &char2, &char1, &nouvTok, tokenAdd, &typeTok, &debTok, i, &stepNumber, &stepHexBin);

        if (CODE_ERROR != 0)
        {
            err_free(string);
            strlist_destroy(tokenAdd, true);
            err_free(typeTok.tab);
            return;
        }
        
        if (nouvTok) // ajout du nouveau token dans la liste tokens.
        {
            
            for (int k=0 ; k < tokenAdd->len ; k++) // boucle pour chaque nouveau token ajoute. chaque tour de cette boucle est comme un tour de boucle normal sur tokens. cela permet d’eviter une boucle supplementaire                
            {
                // savoir si on a un operateur ou un mot cle ou un booleen
                if (typeTok.tab[k] == TYPE_OPERATOR) // si il a déjà été détecté comme opérateur, on vérifie si c'est bien un opérateur valide
                {
                    if (!(strlist_inList(&operateurs1, tokenAdd->tab[k])))
                    {
                        CODE_ERROR = 24; // opérateur inconnu
                        err_free(string);
                        strlist_destroy(tokenAdd, true);
                        err_free(typeTok.tab);
                        return;
                    }
                }
                
                
                
                if (typeTok.tab[k] == TYPE_WORD)
                {
                    if (strlist_inList(&operateurs2, tokenAdd->tab[k]))
                    {
                        typeTok.tab[k] = TYPE_OPERATOR;
                    }
                    else if (strlist_inList(&boolean, tokenAdd->tab[k]))
                    {
                        typeTok.tab[k] = TYPE_BOOL;
                    }

                    else if (strlist_inList(&exceptions, tokenAdd->tab[k]))
                    {
                        typeTok.tab[k] = TYPE_EXCEPTION;
                    }
                    
                    else if (strlist_inList(&constant, tokenAdd->tab[k]))
                    {
                        if (strcmp(tokenAdd->tab[k], "None")==0)
                            typeTok.tab[k] = TYPE_NONE;
                        else
                            typeTok.tab[k] = TYPE_CONST;
                    }

                    else if (strlist_inList(&keywordFunction, tokenAdd->tab[k]))
                    {
                        typeTok.tab[k] = TYPE_KEYWORDFUNCTION;
                    }

                        
                    else if (strlist_inList(&keywords, tokenAdd->tab[k]))
                    {
                        typeTok.tab[k] = TYPE_KEYWORD;
                    }
                    
                    else if (strlist_inList(&blockwords, tokenAdd->tab[k]))
                    {
                        
                        typeTok.tab[k] = TYPE_BLOCKWORD;
                    }
                    
                    else
                    {
                        
                        typeTok.tab[k] = TYPE_VARIABLE;
                    }
               
                }
               
                
                if (tokens->len==0 && typeTok.tab[k] != TYPE_COMMENT)
                {
                    strlist_append(tokens,strdup(tokenAdd->tab[k]));
                    intlist_append(types,typeTok.tab[k]);
                }
                
                else if (tokens->len>0 && typeTok.tab[k] != TYPE_COMMENT)
                {
                    if (!((tokenAdd->tab[k][0]=='\n' || tokenAdd->tab[k][0]==';') && (tokens->tab[tokens->len-1][0] == '\n' || tokens->tab[tokens->len-1][0] == ';'))) // si le token precedent etait un endofline et que celui la aussi alors on ne l’ajoute pas
                    {
                        strlist_append(tokens,strdup(tokenAdd->tab[k]));
                        intlist_append(types,typeTok.tab[k]);
                    }
                }
                
                
                //etapes de fonction et d’index de liste
                
                // a chaque nouveau token que l’on reçoit, on effectue les memes verifications que pour les sous tokens a savoir les verifications de grammaire pour les tokens composes
                
                //sorte de pile qui compte les accolades et les parentheses
                if (tokenAdd->tab[k][0]=='{')
                    nbAcc++;
                else if (tokenAdd->tab[k][0]=='}')
                    nbAcc--;
                
                
                if (tokenAdd->tab[k][0]=='(')
                    nbPar++;
                else if (tokenAdd->tab[k][0]==')')
                    nbPar--;
                
                
                if (tokenAdd->tab[k][0]=='[')
                    nbCro++;
                else if (tokenAdd->tab[k][0]==']')
                    nbCro--;
                
                

                // vérification du nombre de parentheses
                if (nbAcc < 0 || nbCro < 0 || nbPar < 0) {
                    CODE_ERROR = 68;
                    err_free(string);
                    strlist_destroy(tokenAdd, true);
                    err_free(typeTok.tab);
                    return;
                    return ;
                }


                
                // annuler ou poursuivre les fonctions, les instructions, les index de listes....
                
                annulerOuPoursuivre(&isPotentiallyFonc, &foncStep, &isPotentiallyInst, &instStep, &isPotentiallyListIndex, &listIndexStep, tokenAdd, &typeTok, k, &foncParDeb, &indexParDeb, &listeParDeb, &nbCro);


                
                // fins de fonctions, d'instructions, d'index de listes...
                
                finsTypesComposes(&debTok2, &debTok3, k, &nouvTokComp, &isPotentiallyFonc, &foncStep, tokenAdd, &nbPar, tokens, types, &isPotentiallyInst, &instStep, &listIndexStep, &isPotentiallyList, &isPotentiallyListIndex, &nbCro, &isPotentiallyBlock, &nbAcc, &foncParDeb, &indexParDeb, &listeParDeb,&i,len_string);
                
                // fin des fins et debut des debuts (hi hi hi)

                if (CODE_ERROR != 0)
                {
                    err_free(string);
                    strlist_destroy(tokenAdd, true);
                    err_free(typeTok.tab);
                    return;
                }
                
                
                debutTokensComposes(k, &typeTok, tokenAdd, &debTok2, &debTok3, tokens, types, &nouvTokComp, &isPotentiallyList, &isPotentiallyFonc, &isPotentiallyListIndex, &isPotentiallyComm, &isPotentiallyBlock, &isPotentiallyInst, &isPotentiallyLongComm, &listIndexStep, &foncStep, &instStep, &foncParDeb, &indexParDeb, &listeParDeb, &nbPar, &nbCro);
                
            }
            
            isPotentiallyNumber=false;
            isPotentiallyHexBin = false;
            isPotentiallyString=false;
            isPotentiallyString2=false; // chaine avec un guillemet court
            isPotentiallyWord=false;
            isPotentiallyOp=false;
            isPotentiallyComm=false;
            isPotentiallyLongComm=false;
            
            
            
            
            // reinitialise les variables qui ont servi au traitement de ce sous-token
            strlist_destroy(tokenAdd, true);
            
            //err_free(tokenAdd->tab);
            //err_free(tokenAdd);
            tokenAdd = strlist_create(0);
            
            err_free(typeTok.tab);//on ne la détruit pas car elle contient des éléments de type TYPE_CROCHET, TYPE_VARIABLE donc des chaines de caractères littérales
            typeTok = intlist_create(0);
            
            
            nombrePoints=0;
            debTok=0;
            
        }
        
        //debut tokens simples
        debutTokensSimples(i, &debTok, &char1, &isPotentiallyString, &isPotentiallyNumber,&isPotentiallyWord, &isPotentiallyOp, &isPotentiallyString2, &isPotentiallyComm, &isPotentiallyLongComm, &isPotentiallyHexBin);
        
        
        
        // vérifie si le caractère est reconnu
        if (!isPotentiallyHexBin && !isPotentiallyWord && !isPotentiallyString && !isPotentiallyNumber && !isPotentiallyOp && !isPotentiallyString2 && !isPotentiallyComm && !isPotentiallyLongComm && !nouvTok && char1 != ' ' && char1 != '\t')
        {
            CODE_ERROR = 25; // caractère inconnu
            err_free(string);
            strlist_destroy(tokenAdd, true);
            err_free(typeTok.tab);
            return;
        }
        
        char1_2 = charToString(char1);
        if (nouvTok) //si on a un nouveau token, on vérifie que le caractère actuel est accepté par la grammaire globale
        {
            if (!isalnum(char1) && !(strlist_inList(&acceptedChars, char1_2)))
            {
                CODE_ERROR = 25;
                err_free(string);
                err_free(char1_2);
                strlist_destroy(tokenAdd, true);
                err_free(typeTok.tab);
                return;
            }
        }
        err_free(char1_2);
        
        nouvTok=false;
        
        
        char2=char1;
    }
    
    err_free(typeTok.tab);
    
    // si une variable de condition de détection de token est encore à True, faute de syntaxe
    if (isPotentiallyHexBin || isPotentiallyWord || isPotentiallyString || isPotentiallyNumber || isPotentiallyOp  || isPotentiallyString2 || isPotentiallyComm || isPotentiallyLongComm || nbAcc!=0 || nbPar!=0 || nbCro!=0)
    {
        err_free(string);
        strlist_destroy(tokenAdd, true);

        if (nbAcc < 0 || nbCro < 0 || nbPar < 0)
            CODE_ERROR = 68;
        else
            CODE_ERROR = 26; // chaine de caractère, liste ou autre non terminée
        
        return;
    }
    
    
    
    // verification de la syntaxe
    verificationGrammaire(tokens,types, &isPotentiallyExpr, &debExpr);

    if (CODE_ERROR != 0)
    {
        err_free(string);
        strlist_destroy(tokenAdd, true);
        return;
    }

    
    // nouvelle boucle pour réunir les statements for, while et if/elif/else
    
    if (tokens->len == 0)
    {
        err_free(string);
        strlist_destroy(tokenAdd, true);
        return ;
    }
    
    
    
    if (traiterStatements)
    {
        statements(types, tokens, &isPotentiallyStat, &debStat);
        if (CODE_ERROR != 0)
        {
            err_free(string);
            strlist_destroy(tokenAdd, true);
            return;
        }
    }
    
    strlist_destroy(tokenAdd, true);
    err_free(string);


    return ;
}














char* nomBlockLine(char* blockline)
{
	if (count(blockline, "(") > 0)
		return sub(blockline,0,sindex(blockline, '('));
	else
		return strdup(blockline);
}






void finTokensSimples(char* string, _Bool* isPotentiallyNumber, _Bool* isPotentiallyString, _Bool* isPotentiallyWord, _Bool* isPotentiallyOp, _Bool* isPotentiallyLongComm, _Bool* isPotentiallyString2, _Bool* isPotentiallyComm, _Bool* isPotentiallyHexBin, char * char2, char* char1, _Bool* nouvTok, strlist* tokenAdd, intlist* typeTok, int* debTok, int i, int* stepNumber, int* stepHexBin)
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
        intlist_append(typeTok,TYPE_STRING);
        (*isPotentiallyString)=false;
    }
    
    if ((*char2)=='\'' && (*isPotentiallyString2) && i-1!=(*debTok)) // fin de chaîne si le guillemet etudie n’est pas le meme que le premier de la chaine
    {
        char* sousch = sub(string, (*debTok), i);
        strlist_append(tokenAdd,sousch);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_STRING);
        (*isPotentiallyString2)=false;
    }
    
    if ((((*char1)=='.' && (*stepNumber)==2) || (!isdigit(*char1) && (*char1) != '.')) && (*isPotentiallyNumber) && !(*stepNumber==1) || (*isPotentiallyNumber && *char1=='.' && !isdigit(string[i+1]))) // fin de nombre
    {
        char* nb = sub(string, (*debTok), i);
        strlist_append(tokenAdd,nb);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_NUMBER);
        (*isPotentiallyNumber)=false;
        *stepNumber = 0;
    }

    if (*isPotentiallyHexBin && *stepHexBin == 2 && !isdigit(*char1) && !(*char1 >= 97 && *char1 <= 102)) // fin hex-bin
    {
        // conversion en nombre décimal

        long int trad = (string[*debTok+1] == 'x') ? hexToDec(string, *debTok + 2, i - *debTok - 2) : binToDec(string, 2 + *debTok, i - *debTok - 2);

        if (CODE_ERROR != 0)
            return ;
        
        Number n = number_fromDouble((double)trad);
        char* nb = number_toStr(n);
        number_destroy(n);

        // ajout du nombre converti
        strlist_append(tokenAdd,nb);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_NUMBER);
        (*isPotentiallyHexBin)=false;
        *stepHexBin = 0;
    }
    
    
    
    if ((*isPotentiallyWord) && !(isalnum(*char1) || *char1 == '_' || *char1 == '\'')) // fin word
    {
        strlist_append(tokenAdd,sub(string, (*debTok), i));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_WORD);
        (*isPotentiallyWord)=false;
    }
    
    
    if ((*isPotentiallyComm) && (*char1)=='\n') // fin commentaire
    {
        strlist_append(tokenAdd,sub(string, (*debTok), i));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_COMMENT);
        (*isPotentiallyComm)=false;
    }
        
    
    
    
        
    if ((*char2)=='$' && (*isPotentiallyLongComm) && (i-1)!=(*debTok))
    {
        
        strlist_append(tokenAdd,sub(string, (*debTok), i));
        
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_COMMENT);
        
        (*isPotentiallyLongComm)=false;
    }
    
    
    char* sousch = sub(string, (*debTok), i+1);
    if ((*isPotentiallyOp) && !strlist_inList(&operateurs1, sousch)) // fin operateur
    {
        strlist_append(tokenAdd,sub(string, (*debTok), i));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_OPERATOR);
        (*isPotentiallyOp)=false;
    }
    
    err_free(sousch);
    
    // elements d’un caractere
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyLongComm) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && ((*char1)=='(' || (*char1)==')'))
    {
        (*nouvTok)=true;
        strlist_append(tokenAdd,charToString(*char1));
        if ((*char1)=='(')
            intlist_append(typeTok,TYPE_PARENTHESE1);
        if ((*char1)==')')
            intlist_append(typeTok,TYPE_PARENTHESE2);
        
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyLongComm) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && ((*char1)=='{' || (*char1)=='}'))
    {
        strlist_append(tokenAdd,charToString(*char1));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_ACCOLADE);
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyLongComm) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && ((*char1)=='[' || (*char1)==']'))
    {
        strlist_append(tokenAdd,charToString(*char1));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_CROCHET);
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyLongComm) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && (*char1)==',')
    {
        strlist_append(tokenAdd,charToString(*char1));
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_VIRGULE);
        (*isPotentiallyNumber)=false;
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
    }
    
    
    if (!(*isPotentiallyNumber) && !(*isPotentiallyLongComm) && ((*char1)==';' || (*char1)=='\n'))
    {
        char* str = charToString(*char1);
        strlist_append(tokenAdd,str);
        (*nouvTok)=true;
        intlist_append(typeTok,TYPE_ENDOFLINE);
        (*isPotentiallyNumber)=false;
        
        if ((*isPotentiallyString) || (*isPotentiallyString2))
        {
    		CODE_ERROR = 27; // chaine de carcatère non fermée en fin de ligne
            return;
        }
        
        
        (*isPotentiallyWord)=false;
        (*isPotentiallyOp)=false;
        (*isPotentiallyComm)=false;
    }
    return ;
}





void debutTokensSimples(int i, int* debTok, char* char1, _Bool* isPotentiallyString, _Bool* isPotentiallyNumber, _Bool* isPotentiallyWord, _Bool* isPotentiallyOp, _Bool* isPotentiallyString2, _Bool* isPotentiallyComm, _Bool* isPotentiallyLongComm, _Bool* isPotentiallyHexBin)
{

    if ((isalpha(*char1) || *char1 == '_') && !(*isPotentiallyHexBin) && !(*isPotentiallyWord) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyOp)  && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // debut word
    {
        (*debTok)=i;
        (*isPotentiallyWord)=true;
    }

    if ((*char1)=='"' && !(*isPotentiallyHexBin) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // début de chaîne
    {
        (*debTok)=i;
        (*isPotentiallyString)=true;
    }
    
    
    if ((*char1)=='#' && !(*isPotentiallyHexBin) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // début de commentaire
    {
        (*debTok)=i;
        (*isPotentiallyComm)=true;
    }
        
    if ((*char1)=='$' && !(*isPotentiallyHexBin) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // début de commentaire long
    {
        (*debTok)=i;
        (*isPotentiallyLongComm)=true;
    }
    
    
    
    if ((*char1)=='\'' && !*isPotentiallyWord && !(*isPotentiallyHexBin) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // début de chaîne
    {
        (*debTok)=i;
        (*isPotentiallyString2)=true;
    }
    
    
    
    
    if (isdigit((*char1)) && !(*isPotentiallyHexBin) && !(*isPotentiallyNumber) && !(*isPotentiallyString) && !(*isPotentiallyWord) && !(*isPotentiallyOp) && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // début de nombre
    {
        (*debTok)=i;
        (*isPotentiallyNumber)=true;
        *isPotentiallyHexBin = true;
    }
    
    
    
    char *char1_2 = charToString(*char1);
    if (strlist_inList(&sousop, char1_2) && !(*isPotentiallyHexBin) && !(*isPotentiallyWord) && !(*isPotentiallyString) && !(*isPotentiallyNumber) && !(*isPotentiallyOp)  && !(*isPotentiallyString2) && !(*isPotentiallyComm) && !(*isPotentiallyLongComm)) // debut operateur
    {
        (*debTok)=i;
        (*isPotentiallyOp)=true;
    }
    
    err_free(char1_2);
    
    return ;
}





void annulerOuPoursuivre(_Bool* isPotentiallyFonc, int* foncStep, _Bool* isPotentiallyInst, int* instStep, _Bool* isPotentiallyListIndex, int* listIndexStep, strlist* tokenAdd, intlist* typeTok, int k, int *foncParDeb, int *listeParDeb, int *indexParDeb, int *nbCro)
{
    if ((*isPotentiallyFonc) && (*foncStep)==1) // annuler ou poursuivre la fonction
    {
        
        if (tokenAdd->tab[k][0]=='(' && strlen(tokenAdd->tab[k])==1)
            (*foncStep)=2;
        else if (typeTok->tab[k] != TYPE_VARIABLE)
        {
            *foncStep = 0;
            *isPotentiallyFonc = 0;
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

    if (*listIndexStep == 5)
    {
        *listIndexStep = 0;
        (*isPotentiallyListIndex)=false;
        *indexParDeb = 0;
    }
    
    
    if ((*isPotentiallyListIndex) && (*listIndexStep==1 || *listIndexStep==4)) // annuler ou poursuivre l’index de liste
    {
        if (strcmp(tokenAdd->tab[k],"[")==0)
            (*listIndexStep)=2;
        else if (*listIndexStep == 4)
        {
            *listIndexStep=5;
        }
        else
        {
            *listIndexStep=0;
            *isPotentiallyListIndex = false;
            *indexParDeb = 0;
        }
    }
    
    if ((*isPotentiallyListIndex) && (*listIndexStep)==2 && strcmp(tokenAdd->tab[k],"]") != 0 && strcmp(tokenAdd->tab[k],"[") != 0)
        (*listIndexStep)=3;

    if ((*isPotentiallyListIndex) && (*listIndexStep)==3 && strcmp(tokenAdd->tab[k],"]") == 0 && (*nbCro - *indexParDeb)==0)
        *listIndexStep = 4;
    
    return;
}






void debutTokensComposes(int k, intlist* typeTok, strlist* tokenAdd, int* debTok2, int* debTok3, strlist* tokens, intlist *types, _Bool* nouvTokComp, _Bool* isPotentiallyList, _Bool* isPotentiallyFonc, _Bool* isPotentiallyListIndex, _Bool* isPotentiallyComm, _Bool* isPotentiallyBlock, _Bool* isPotentiallyInst, _Bool* isPotentiallyLongComm, int* listIndexStep, int *foncStep, int *instStep, int *foncParDeb, int *listeParDeb, int *indexParDeb, int* nbPar, int* nbCro)
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



    

    if (typeTok->tab[k] == TYPE_VARIABLE && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyComm) && !(*isPotentiallyBlock) && !(*isPotentiallyInst) && !(*isPotentiallyLongComm)) // debut de fonction et d’index de liste
    {
       (*debTok2)=tokens->len-1;
       (*isPotentiallyFonc)=true;
       (*isPotentiallyListIndex)=true;
       (*listIndexStep)=1;
       (*foncStep)=1;
       
       *foncParDeb = *nbPar;
       *indexParDeb = *nbCro;
    }
    


    if (typeTok->tab[k] == TYPE_BLOCKWORD && !(*isPotentiallyList) && !(*isPotentiallyFonc) && !(*isPotentiallyListIndex) && !(*isPotentiallyComm) && !(*isPotentiallyInst)  && !(*isPotentiallyLongComm)) // debut d’instruction de bloc
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











void finsTypesComposes(int *debTok2, int *debTok3, int k, _Bool *nouvTokComp, _Bool *isPotentiallyFonc, int *foncStep, strlist* tokenAdd, int *nbPar, strlist* tokens, intlist* types, _Bool *isPotentiallyInst, int *instStep, int *listIndexStep, _Bool *isPotentiallyList, _Bool *isPotentiallyListIndex, int *nbCro, _Bool *isPotentiallyBlock, int *nbAcc, int *foncParDeb, int *listeParDeb, int *indexParDeb, int *i, int len_string)
{

    if (*isPotentiallyFonc && *foncStep==2 && tokenAdd->tab[k][0] == ')' && (*nbPar - *foncParDeb)==0) // fin fonction
    {
        (*isPotentiallyFonc)=false;
        (*foncStep)=0;
        char* sommeTokens = strdup("");
        int len = tokens->len;
        for (int i=0 ; i < len - (*debTok2) ; i++)
        {
            if (types->tab[(*debTok2)] == TYPE_OPERATOR)
            {
                char* tmp1 = sandwich(tokens->tab[(*debTok2)], ' ');
                char* tmp2 = addStr(sommeTokens, tmp1);
                err_free(sommeTokens);err_free(tmp1);
                sommeTokens = tmp2;
            }
            else if (types->tab[(*debTok2)] != TYPE_ENDOFLINE)
            {
                char* tmp1 = addStr(sommeTokens, tokens->tab[(*debTok2)]);

                err_free(sommeTokens);
                sommeTokens = tmp1;
            }
            strlist_remove(tokens, *debTok2, true);
            intlist_remove(types, *debTok2);
        }

        strlist_append(tokens, sommeTokens);
        intlist_append(types, TYPE_FONCTION);
        (*nouvTokComp)=true;
        *foncParDeb = 0;
    }

    else if ((*isPotentiallyFonc) && (*foncStep)==2 && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
    {
        CODE_ERROR = 28; // mot clé incompatible utilisé dans une expression
        return;
    }
    
    
    
    else if ((*isPotentiallyInst) && (*instStep)==2 && tokenAdd->tab[k][0] == ')' && (*nbPar)==0) // fin instruction de bloc
    {
        (*isPotentiallyInst)=false;
        
        (*instStep)=0;
        char* sommeTokens = strdup("");
        int len = tokens->len;
        for (int i=0 ; i < len - (*debTok2) ; i++)
        {
            if (types->tab[(*debTok2)] == TYPE_OPERATOR)
            {
                char* tmp1 = sandwich(tokens->tab[(*debTok2)], ' ');
                char* tmp2 = addStr(sommeTokens, tmp1);
                
                err_free(sommeTokens);err_free(tmp1);
                sommeTokens = tmp2;
            }
            else if (types->tab[(*debTok2)] != TYPE_ENDOFLINE)
            {
                char* tmp1 = addStr(sommeTokens, tokens->tab[(*debTok2)]);
                
                err_free(sommeTokens);
                
                sommeTokens = tmp1;
            }
            
            strlist_remove(tokens, *debTok2, true);
            
            intlist_remove(types, *debTok2);
            
        }
        
        strlist_append(tokens, sommeTokens);

        char* nomblockline = nomBlockLine(sommeTokens);
        
        if (strlist_inList(&blockwords1Line, nomblockline))
            intlist_append(types, TYPE_BLOCKWORD1LINE);
        else
            intlist_append(types, TYPE_BLOCKLINE);
        
        err_free(nomblockline);
    
        (*nouvTokComp)=true;
    }
    
    else if ((*isPotentiallyInst) && (*instStep)==2 && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
    {
        CODE_ERROR = 28; // mot clé incompatible utilisé dans une expression
        return;
    }


    // condition de fin lorsque l'on a un nombre égal de crochets fermants et de crochets ouvrants, et que le caractère actuel n'est pas un crochet ouvrant ou un espace
    else if (*isPotentiallyListIndex && *listIndexStep==5 && ((strcmp(tokenAdd->tab[k],"]") == 0 && *nbCro - *indexParDeb == -1) || (strcmp(tokenAdd->tab[k],"[") != 0 && strcmp(tokenAdd->tab[k],"]") != 0) || len_string-2 == *i) && strcmp(tokenAdd->tab[k],"(") != 0) // fin index liste
    {
        
        (*isPotentiallyListIndex)=false;
        (*listIndexStep)=0;
        char* sommeTokens = strdup("");
        int len = tokens->len;

        /* explication du fonctionnement :
        Lorsque l'on arrive ici, on a le choix : ou alors on a un token de trop, ou alors non car on est arrivé à la fin de la chaine de caractères.
        Dans le cas où on a un token de trop, on introduit un décalage pour parcourir tokens avec un tour de moins
        Ensuite, dans le cas où l'on avait introduit un décalage, on insère l'index de liste 'compilé' juste avant le token en trop. Sinon, on l'ajoute juste à la fin
        Note : cette méthode n'introduit aucun changement de complexité, elle est juste un peu moins élémentaire.
        */
        
        int decalage = 0;
        if (tokens->tab[tokens->len-1][0] != ']') // quand on a quitté parce que ce n'est pas la fin, on a un caractère de plus
            decalage = 1;
        
        
        for (int i=0 ; i < len - *debTok2 - decalage ; i++)
        {
            if (types->tab[(*debTok2)] == TYPE_OPERATOR)
            {
                char* tmp1 = sandwich(tokens->tab[(*debTok2)], ' ');
                char* tmp2 = addStr(sommeTokens, tmp1);
                err_free(sommeTokens);err_free(tmp1);
                sommeTokens = tmp2;
            }
            else if (types->tab[(*debTok2)] != TYPE_ENDOFLINE)
            {
                char* tmp1 = addStr(sommeTokens, tokens->tab[(*debTok2)]);
                err_free(sommeTokens);
                sommeTokens = tmp1;
            }
            strlist_remove(tokens, *debTok2, true);
            intlist_remove(types, *debTok2);
        }

        if (decalage == 1)
        {
            strlist_insert(tokens, sommeTokens, tokens->len - 1);
            intlist_insert(types, TYPE_LISTINDEX, types->len - 1);
        }
        else
        {
            strlist_append(tokens, sommeTokens);
            intlist_append(types, TYPE_LISTINDEX);
        }
        (*nouvTokComp)=true;
        *indexParDeb = 0;

    }
      
    else if ((*isPotentiallyListIndex) && ((*listIndexStep)==2 || (*listIndexStep)==3) && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
    {
        CODE_ERROR = 28; // mot clé incompatible utilisé dans une expression
        return;
    }
      
    
    
    
    
    else if (tokenAdd->tab[k][0] == ']' && (*isPotentiallyList) && *nbCro == 0)// fin de liste
    {
        (*isPotentiallyList)=false;
        char* sommeTokens = strdup("");
    
        
        int len = tokens->len;
        for (int i=0 ; i < len - (*debTok2) ; i++)
        {
            if (types->tab[(*debTok2)] == TYPE_OPERATOR)
            {
                char* tmp1 = sandwich(tokens->tab[(*debTok2)], ' ');
                char* tmp2 = addStr(sommeTokens, tmp1);
                err_free(sommeTokens);err_free(tmp1);
                sommeTokens = tmp2;
            }
            else if (types->tab[(*debTok2)] != TYPE_ENDOFLINE)
            {
                char* tmp1 = addStr(sommeTokens, tokens->tab[(*debTok2)]);
                err_free(sommeTokens);
                sommeTokens = tmp1;
            }
            strlist_remove(tokens, *debTok2, true);
            intlist_remove(types, *debTok2);
        }
        
        strlist_append(tokens, sommeTokens);
        intlist_append(types, TYPE_LIST);
        (*nouvTokComp)=true;
    }

    else if ((*isPotentiallyList) && (strlist_inList(&keywords, tokenAdd->tab[k]) || strlist_inList(&blockwords, tokenAdd->tab[k])))
    {
        CODE_ERROR = 28; // mot clé incompatible utilisé dans une expression
        return;
    }

    
    
    else if (tokenAdd->tab[k][0] =='}' && (*isPotentiallyBlock) && (*nbAcc)==0)// fin de bloc d’instructions
    {
        (*isPotentiallyBlock)=false;
        char* sommeTokens = strdup("");
        int len = tokens->len;
        for (int i=0 ; i < len - (*debTok3) ; i++)
        {
            if (types->tab[(*debTok3)] == TYPE_OPERATOR || types->tab[(*debTok3)] == TYPE_KEYWORDFUNCTION)
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
        intlist_append(types, TYPE_BLOCK);
        (*nouvTokComp)=true;
    }
    
    
    return;
}






void empaquetageExprInst(int* j, strlist* tokens, intlist* types, _Bool* isPotentiallyExpr, int * debExpr, char* typeact)
{
    if ((*isPotentiallyExpr) && (*typeact)==TYPE_ENDOFLINE)
    {
        (*isPotentiallyExpr)=false;
        strlist_remove(tokens, *debExpr, true);
        intlist_remove(types, *debExpr);
        
        char* sommeTokens=strdup("");
        for (int i=(*debExpr);i<(*j)-1;i++)
        {
            if (types->tab[(*debExpr)] == TYPE_OPERATOR)
            {
                char* tmp1 = (tokens->tab[*debExpr][0] == '_') ? strdup(" -") : sandwich(tokens->tab[(*debExpr)], ' ');
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
        intlist_insert(types, TYPE_EXPRESSION, *debExpr);
        (*j)=(*debExpr)+1;
        (*debExpr)=0;

        
    }
    
    
    if ((*typeact)==TYPE_ENDOFLINE)
    {
        (*isPotentiallyExpr)=true;
        (*debExpr)=(*j);
    }
        
    if ((*isPotentiallyExpr) && ((*typeact)==TYPE_BLOCK || (*typeact)==TYPE_BLOCKLINE || (*typeact)==TYPE_KEYWORD || (*typeact)==TYPE_KEYWORDFUNCTION || *typeact == TYPE_BLOCKWORD1LINE))
    {
        (*isPotentiallyExpr)=false;
        strlist_remove(tokens, *debExpr, true);
        intlist_remove(types, *debExpr);
        (*j)-=1;
        
        (*debExpr)=0;
    }
        
    if ((*j)==tokens->len-1 && (*typeact)==TYPE_ENDOFLINE)
    {
        strlist_remove(tokens, *j, true);
        intlist_remove(types, *j);
    }
}







void verificationGrammaire(strlist* tokens, intlist* types, _Bool* isPotentiallyExpr, int* debExpr)
{
    
    int j=0;
    int grammanc; char typeact; char typeanc; int gramm; int i; _Bool isCorresponding; int grammact = 0; _Bool corresponding;
    _Bool isPotentiallyStat = false;

    if (types->len > 0 && !intlist_inList(&types_debut, types->tab[0]))
    {
        CODE_ERROR = 32;
        return ;
    }
    
    
    while (j < tokens->len ) // on va verifier que le chaque suite de token est valide par la grammaire (verfication de grammaire)
    {
    
        // on va se servir de cette boucle pour gérer les '-' unaires et binaires
        
        
        grammanc=0;
        
        if (j > 0 && types->tab[j-1]==TYPE_OPERATOR)
        {
            
            grammanc=gramm1.tab[strlist_index(&operateurs3, tokens->tab[j-1])];
            
        }
        
        // ici on s'occuppe de modifier les signes '-'
        if (types->tab[j]==TYPE_OPERATOR && tokens->tab[j][0] == '-') // si on a un opérateur et que cet opérateur est un moins
        {
            if (j==0 && j < tokens->len - 1)
            {
                err_free(tokens->tab[j]);
                tokens->tab[j] = strdup("_"); // si j=0 (c'est-à-dire que c'est le premier token), on sait que c'est forcément un moins unaire
                
            }
            
            
            else if (!(types->tab[j-1] == TYPE_NUMBER || types->tab[j-1] == TYPE_STRING || types->tab[j-1] == TYPE_LISTINDEX || types->tab[j-1] == TYPE_FONCTION || types->tab[j-1] == TYPE_LIST || types->tab[j-1] == TYPE_BOOL || types->tab[j-1] == TYPE_CONST || types->tab[j-1] == TYPE_NONE || types->tab[j-1] == TYPE_EXCEPTION || types->tab[j-1] == TYPE_PARENTHESE2 || types->tab[j-1] == TYPE_VARIABLE || (types->tab[j-1]==TYPE_OPERATOR && grammanc == VARLEFT)) )
            {
                // si on a un moins qui est directement précédé d'une autre unité lexicale de type nombre etc, ça veut dire que c'est l'opérateur moins donc
                // là on teste la condition inverse c'est à dire que l'on se trouve en présence d'un moins unaire
                // on va intégrer le moins à l'unité lexicale à qui il est adressé
                if (j < tokens->len - 1) // si le moins étudié n'est pas le dernier caractère des tokens
                {
                    err_free(tokens->tab[j]);
                    tokens->tab[j] = strdup("_");
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
            
            
            if (typeanc == TYPE_BLOCKLINE)
                isPotentiallyStat = true;
            
            
            
            if ((isPotentiallyStat && typeact != TYPE_ENDOFLINE && typeact != TYPE_BLOCK) || ( (typeact==TYPE_BLOCKLINE || isPotentiallyStat) && j==tokens->len-1) )
            {
                CODE_ERROR = 29; // Ligne de bloc d'instructions non suivi par un bloc d'instructions (ex: un if() tout seul) (1)
                return;
            }
            else if (isPotentiallyStat && typeact==TYPE_BLOCK)
                isPotentiallyStat = false;
            
            
            
            
            // petit algoithme qui determine si le type de token est accepte en token final
            i=0;
            isCorresponding=false;
            for (int l=0 ; l < syntax.len ; l++)
            {
                if (syntax.tab[l].tab[0] == typeact)
                {
                    isCorresponding=true;
                    i=l;
                    
                    break;
                }
            }
            
            if (!isCorresponding) // le type actuel n'est pas accepté, il y a donc erreur de syntaxe
            {
                CODE_ERROR = 30; // Mauvaise utilisation d'un opérateur, deux types à la suite non compatibles, ou utilisation d'un type inconnu/incompatible (2)
                return;
            }
            
            // algoithme qui verifie si le token precedent est accepte par la grammaire avant le token actuel
            corresponding=false;
            for (int k = 1 ; k < (syntax.tab[i]).len ; k++) // parcourt la liste des types attendus
            {
                
                gramm=0;
                if (typeanc == syntax.tab[i].tab[k]) // si ca correspond on va vérifier si c'est un opérateur si ça correspond avec ce qu'accepte l'opérateur
                {
                    //comme opérandes ou si le type précédent est un opérateur on vérifie que le type actuel est bien accepté par l'opérateur
                    
                    if (typeanc==TYPE_OPERATOR) // si c'est le token d'avant qui est un opérateur
                    {
                        //recupere l’operateur pour verifier que ca correspond
                        gramm=gramm1.tab[strlist_index(&operateurs3, tokens->tab[j-1])];
                        
                        
                        if (typeact==TYPE_OPERATOR)
                            grammact=gramm1.tab[strlist_index(&operateurs3, tokens->tab[j])];
                        
                        
                        
                        // on verifie que le token peut effectivement etre apres l’operateur
                        if ((gramm==VARLEFT && typeact != TYPE_ENDOFLINE && typeact != TYPE_VIRGULE && typeact != TYPE_PARENTHESE2 && !(typeact==TYPE_OPERATOR && grammact==RIGHT_LEFT)) || ((gramm==VARRIGHT || gramm == LEFT_VAR) && !(typeact==TYPE_LISTINDEX || typeact==TYPE_VARIABLE)) || ((gramm==RIGHT_LEFT || gramm==VAR_RIGHT || gramm==RIGHT) && (typeact==TYPE_ENDOFLINE || typeact==TYPE_VIRGULE || typeact==TYPE_PARENTHESE2)) || (gramm==VARRIGHT && typeact != TYPE_VARIABLE))
                        {
                            // Mauvaise utilisation d'un opérateur, deux types à la suite non compatibles, ou utilisation d'un type inconnu/incompatible (2)
                            CODE_ERROR = 31;
                            return;
                        }
                    }
                    
                            
                    if (typeact == TYPE_OPERATOR) // si le type actuel est un operateur, on verifie qu’il accepte bien l’élement d’avant
                    {
                        gramm=gramm1.tab[strlist_index(&operateurs3, tokens->tab[j])];
                        
                        if (typeanc==TYPE_OPERATOR)
                            grammanc=gramm1.tab[strlist_index(&operateurs3, tokens->tab[j-1])];


                        if (((gramm==VARLEFT || gramm==VAR_RIGHT) && typeanc != TYPE_VARIABLE && typeanc != TYPE_LISTINDEX) || ((gramm==RIGHT || gramm==VARRIGHT)  && (typeanc==TYPE_NUMBER || typeanc==TYPE_STRING || typeanc==TYPE_LIST ||  typeanc==TYPE_VARIABLE ||  typeanc==TYPE_FONCTION ||  typeanc==TYPE_BOOL || typeanc==TYPE_CONST || typeanc==TYPE_NONE || typeanc==TYPE_EXCEPTION || typeanc==TYPE_LISTINDEX)) || ((gramm==LEFT_VAR || gramm==RIGHT_LEFT) && !(typeanc==TYPE_NUMBER || typeanc==TYPE_STRING || typeanc==TYPE_LIST ||  typeanc==TYPE_VARIABLE ||  typeanc==TYPE_FONCTION ||  typeanc==TYPE_BOOL ||  typeanc==TYPE_LISTINDEX || typeanc==TYPE_CONST || typeanc==TYPE_NONE || typeanc==TYPE_EXCEPTION || typeanc==TYPE_PARENTHESE2 || (typeanc==TYPE_OPERATOR && grammanc==VARLEFT))))
                        {
                            // Mauvaise utilisation d'un opérateur, deux types à la suite non compatibles, ou utilisation d'un type inconnu/incompatible (2)
                            CODE_ERROR = 32;
                            return;
                        }
                    }
                            
                    corresponding=true;
                    break;
                
                }
                
            }

            
            if (corresponding==false) // si ca correspond a aucun type attendu
            {
                // Erreur de syntaxe : Mauvaise utilisation d'un opérateur, deux types à la suite non compatibles, ou utilisation d'un type inconnu/incompatible (5)
                CODE_ERROR = 33;
                return;
            }
        }
        
        
        //ici empaquetage des expressions et des blocs d’instructions
        
        empaquetageExprInst(&j, tokens, types, isPotentiallyExpr, debExpr, &typeact);

        
        j+=1;
    }

    if (types->len > 0 && !intlist_inList(&types_fin, types->tab[types->len-1]))
    {
        CODE_ERROR = 32;
        return ;
    }

}







void compConditionBlock(int cas, _Bool* isPotentiallyConBlock, int *conBlockStep, strlist* tokens, intlist* types, int *i, char* typeact, int* debConBlock) // fonction qui réunit les blocs if/elif/else ensemble
{
    // on distingue deux cas différents ) le cas où on ne prend pas le token actuel (cas 1), et le cas où on le prend (cas2)
    
    if (cas == 1) // cas 1
    {
        *isPotentiallyConBlock = false;
        *conBlockStep = 0;
        char* sommeTokens = strdup("");
        
        for (int k=(*debConBlock) ; k < (*i)-1 ; k++)
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
        types->tab[*debConBlock] = TYPE_CONDITIONBLOCK;
        *typeact = TYPE_CONDITIONBLOCK;
        *i=*debConBlock;
    }
    else if (cas == 2)
    {
        
        // cas 2
        *conBlockStep = 0;
        *isPotentiallyConBlock = false;
        char* sommeTokens = strdup("");
        
        
        for (int k= *debConBlock ; k < *i ; k++)
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
        types->tab[*debConBlock] = TYPE_CONDITIONBLOCK;
        *typeact = TYPE_CONDITIONBLOCK;
        *i=*debConBlock;
    }
    
    
    return ;

}








void statements(intlist* types, strlist* tokens, _Bool *isPotentiallyStat, int *debStat)
{
    
    _Bool isPotentiallyConBlock = false;
    int funcMethStep = 0;
    int conBlockStep = 0;
    int debConBlock = 0;
    int i=0;
    char typeact=types->tab[0];
    
    
    while (i < tokens->len)
    {


        // empaquetage bloc de fonction


        
        if (funcMethStep == 2)
        {

            if (types->tab[i] == TYPE_BLOCK)
            {
                funcMethStep = 0;
                
                char* temp1 = addStr(tokens->tab[i-1], tokens->tab[i]);

                _Bool isMethod = strcmp("methode", tokens->tab[i-2]) == 0;

                i-=2;
                
                strlist_remove(tokens, i, true);
                intlist_remove(types, i);

                strlist_remove(tokens, i, true);
                intlist_remove(types, i);

                types->tab[i] = (isMethod) ? TYPE_METHODDEF : TYPE_FUNCTIONDEF;
                err_free(tokens->tab[i]);
                tokens->tab[i] = temp1;
                
            }
            else
            {
                CODE_ERROR = 11;
                return;
            }
        }


        
        if (funcMethStep == 1)
        {
            if (types->tab[i] == TYPE_FONCTION)
            {
                funcMethStep = 2;
            }
            else
            {
                CODE_ERROR = 11;
                return;
            }
        }


        
        if (types->tab[i] == TYPE_KEYWORDFUNCTION)
        {
            if (i+2 < tokens->len)
                funcMethStep = 1;
            else
            {
                CODE_ERROR = 11;
                return;
            }
        }


        
        
        // modifie le blockline pour savoir quel bloc c'est particulièrement
        if (types->tab[i]==TYPE_BLOCKLINE || (types->tab[i] == TYPE_KEYWORD && !strlist_inList(&lkeywords, tokens->tab[i])))
        {
            // ici il faut créer le TYPE_BLOCKTYPE
            char* nom = nomBlockLine(tokens->tab[i]);
            if (strcmp(nom, "Si")==0)
                types->tab[i] = TYPE_BLOCKIF;
            
            if (strcmp(nom, "ei")==0)
                types->tab[i] = TYPE_BLOCKELIF;
            
            if (strcmp(nom, "es")==0)
                types->tab[i] = TYPE_BLOCKELSE;
            
            if (strcmp(nom, "TantQue")==0)
                types->tab[i] = TYPE_BLOCKWHILE;
            
            if (strcmp(nom, "Pour")==0)
                types->tab[i] = TYPE_BLOCKFOR;

            if (strcmp(nom, "tr")==0)
                types->tab[i] = TYPE_BLOCKTRY;
            
            if (strcmp(nom, "expt")==0)
                types->tab[i] = TYPE_BLOCKEXCEPT;
            
            err_free(nom);
        }
            
        
        typeact=types->tab[i];
        
        
        //--debut--//
        
        
        if ((*isPotentiallyStat) && typeact==TYPE_BLOCK) // fin de statement, on l'empaquette
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

                case 'X':
                    types->tab[*debStat] = 'x';
                    break;

                case 'Y':
                    types->tab[*debStat] = 'y';
                    break;
            }
            
             // on transforme un block en statement par une soustraction par 5;
            
            strlist_remove(tokens, i, false);// cet élément n'existe pas
            
            intlist_remove(types, i);
            
            
            (*isPotentiallyStat)=false;
            (*debStat)=0;
            
            
            // modifie le nouveau token que l'on étudie maintenant au cas où c'est un blockline ou un keyword
            
            
            
            if (i<tokens->len)
                if (types->tab[i] == TYPE_BLOCKLINE || types->tab[i] == TYPE_KEYWORD)
                {
                    // il faut ici créer le TYPE_BLOCKTYPE
                    char* nom = nomBlockLine(tokens->tab[i]);
                    if (strcmp(nom, "Si")==0)
                        types->tab[i] = TYPE_BLOCKIF;
                    
                    if (strcmp(nom, "ei")==0)
                        types->tab[i] = TYPE_BLOCKELIF;
            
                    if (strcmp(nom, "es")==0)
                        types->tab[i] = TYPE_BLOCKELSE;
                    
                    if (strcmp(nom, "TantQue")==0)
                        types->tab[i] = TYPE_BLOCKWHILE;
            
                    if (strcmp(nom, "Pour")==0)
                        types->tab[i] = TYPE_BLOCKFOR;

                    if (strcmp(nom, "expt")==0)
                        types->tab[i] = TYPE_BLOCKEXCEPT;

                    if (strcmp(nom, "tr")==0)
                        types->tab[i] = TYPE_BLOCKTRY;
                    
                    err_free(nom);
                    
                }
            
            
            i-=1;
            typeact=types->tab[i];
        }
        
        
        else if (typeact == TYPE_BLOCK && ! (*isPotentiallyStat))//un block non précédé d'un blockline
        {
            // Erreur de syntaxe : Bloc d'instructions tout seul
            CODE_ERROR = 34;
            return;
        }
        
        
        // ------------------------ on réunit les if/else if/else en conditionblock ------------------------------------
        
        
        if (typeact == TYPE_EXPRESSION || typeact == TYPE_STATEMENTFOR || typeact == TYPE_STATEMENTWHILE || typeact == TYPE_STATEMENTIF || typeact == TYPE_STATEMENTELIF || typeact == TYPE_STATEMENTELSE || typeact == TYPE_BLOCKWORD1LINE || typeact == TYPE_KEYWORDFUNCTION || typeact == TYPE_STATEMENTTRY || typeact == TYPE_STATEMENTEXCEPT) // on n'agit sur les conditionblock que si le type actuel est un type fini (càd pas un block ni un blockif, ni un blockelse etc)
        {

            // fin conditionblock
            
            
            if (typeact==TYPE_STATEMENTIF && i==tokens->len-1 && ! isPotentiallyConBlock) // si le dernier token est un statementif (tout seul, on remplace juste le type par conditionblock
            {
                types->tab[i]=TYPE_CONDITIONBLOCK;
                typeact=types->tab[i];
            }
            
            
            if (isPotentiallyConBlock && (typeact != TYPE_STATEMENTIF && typeact != TYPE_STATEMENTELIF && typeact != TYPE_STATEMENTELSE)) // dans le cas où l'on tombe sur un token autre que if/else if/else qui met fin au conditionblock
            {
                compConditionBlock(1, &isPotentiallyConBlock, &conBlockStep, tokens, types, &i, &typeact, &debConBlock);
                i++;
            }
            
            // annuler ou poursuivre conditionblock
            if (typeact == TYPE_STATEMENTELIF && isPotentiallyConBlock) // dans le cas où on a un statementelse if
            {
                if ((conBlockStep == 1 || conBlockStep == 2) && i==tokens->len-1)
                    compConditionBlock(2, &isPotentiallyConBlock, &conBlockStep, tokens, types, &i, &typeact, &debConBlock);
                else if (conBlockStep == 1)
                        conBlockStep = 2;
                else if (conBlockStep == 3)
                {
                    CODE_ERROR = 35; // Erreur de syntaxe : Mauvais agencement de blocs if/elif/else
                    return;
                }
            }
            else if (typeact == TYPE_STATEMENTELIF)
            {
                CODE_ERROR = 35; // Erreur de syntaxe : Mauvais agencement de blocs if/elif/else
                return;
            }
            
            
            
            if (typeact == TYPE_STATEMENTIF && isPotentiallyConBlock && conBlockStep == 2) // dans le cas où on a un statementif
                
                compConditionBlock(1, &isPotentiallyConBlock, &conBlockStep, tokens, types, &i, &typeact, &debConBlock);
            
            
            if ((typeact == TYPE_STATEMENTELSE && isPotentiallyConBlock) || (typeact==TYPE_STATEMENTIF && i==tokens->len-1)) // dans le cas où on a un statementelse, c'est fini direct, exactement comme dans le cas ou on a un statementif (juste à la fin mais qui n'est pas tout seul
                compConditionBlock(2, &isPotentiallyConBlock, &conBlockStep, tokens, types, &i, &typeact, &debConBlock);
            
            else if (typeact==TYPE_STATEMENTELSE)
            {
                CODE_ERROR = 35;  // Erreur de syntaxe : Mauvais agencement de blocs if/elif/else
                return;
            }
            
            
            
            
            // debut conditionblock
            if (typeact == TYPE_STATEMENTIF && ! isPotentiallyConBlock)
            {
                isPotentiallyConBlock = true;
                conBlockStep = 1;
                debConBlock = i;
            }
        
        
        }
        
        
        //réunion des blocs try/except
        if (typeact == TYPE_STATEMENTEXCEPT) // potentiellement fin de bloc try/except
        {
            if (i==0 || types->tab[i-1] != TYPE_STATEMENTTRY)
            {
                CODE_ERROR = 76;
                return ;
            }
            else
            {
                char* tmp = addStr(tokens->tab[i-1], tokens->tab[i]);
                err_free(tokens->tab[i-1]);
                tokens->tab[i-1] = tmp;
                
                types->tab[i-1] = TYPE_TRYEXCEPT;
                strlist_remove(tokens, i, true);
                intlist_remove(types, i);
                i -= 1;
            }
        }

        if (typeact == TYPE_STATEMENTTRY && i==tokens->len-1)
        {
            CODE_ERROR = 77;
            return;
        }
        
        
        if (typeact==TYPE_BLOCKTRY || typeact == TYPE_BLOCKEXCEPT || typeact==TYPE_BLOCKFOR || typeact==TYPE_BLOCKWHILE || typeact==TYPE_BLOCKIF || typeact==TYPE_BLOCKELSE || typeact==TYPE_BLOCKELIF) // debut statement
        {
            if (i==tokens->len-1)
            {
                CODE_ERROR = 29; // Ligne de bloc d'instructions non suivi par un bloc d'instructions (ex: un if() tout seul)
                return;
            }
            (*isPotentiallyStat)=true;
            (*debStat)=i;
        }
            
        
        i++;
        
    }
    return ;
}

