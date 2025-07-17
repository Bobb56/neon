#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/builtinfunctions.h"
#include "headers/dynarrays.h"
#include "headers/gc.h"
#include "headers/linenoise.h"
#include "headers/printerror.h"
#include "headers/runtime.h"
#include "headers/strings.h"
#include "headers/syntaxtrees.h"
#include "headers/neon.h"

#ifdef WASM
    #include <emscripten/emscripten.h>
    char* INPUTMESSAGE = NULL;
#endif



#ifdef TI83PCE
    #include "nio_ce.h"
    nio_console console;
#endif


// permet de déclencher un signal d'erreur à travers toute la chaîne de traitement de Neon.
// la règle est : si CODE_ERROR est non nul, il faut détruire les objets que l'on a créés dans cette fonction, et retourner
int CODE_ERROR = 0;


#if defined(LINUX)
    #include <signal.h>

    void handle_sigint(int sig) {
        CODE_ERROR = 104;
    }
#endif


#if defined(WINDOWS11) || defined(WINDOWS10)
    #include <windows.h>

    // Fonction de gestion du signal
    BOOL WINAPI ctrlHandler(DWORD signal) {
        if (signal == CTRL_C_EVENT) {
            CODE_ERROR = 104;
            return TRUE;  // Renvoyer TRUE pour indiquer que l'événement a été traité
        }
        return FALSE;
    }
#endif


/*
CHOSES SPÉCIFIQUES À L'ARCHITECTURE :
> lowlevel.asm
> set_stack_pointer
> eval_prolog contient de l'assembleur inline


Liste des choses qui marchent pas :
reverse des listes
_____________________

Potentielles futures erreurs :
------------------------------
Pour éviter d'empiler plusieurs fois la même chose dans les ast (du fait que la construction est basée sur typeTok),
j'ai créé une fonction qui vérifie si on essaie d'empiler deux fois la même chose à la suite.
Cette fonction est pour le moment désactivée car je pense avoir trouvé la solution en mettant block sur debTok2 au lieu de debTok3
On pourra la réactiver au besoin

S'il y a encore un problème de lecture dans la pile allouée par malloc, et un saut à l'adresse 0x0, c'est
probablement encore un problème d'inlinig généré par GCC (même si les fonctions concernées ont été normalement toutes
interdites d'inlining)

Peut être qu'il y a des endroits où on oublie de mettre à jour les promesses




Choses à modifier pour la refonte du système d'objets :
-------------------------------------------------------
> Modifier les getters sur les neobjects pour qu'ils prennent des const NeObj* -> plus rapide, ainsi que neobject_destroy
--> En fait tous les void fonction(...) sur des NeObj peuvent prendre en argument un pointeur constant sur le NeObj

Avancement et choses à faire :
-------------------------------
> Si ça se trouve, on peut appeler update_if_promise seulement lorsque l'on lit une variable ou un index de liste
> Mettre à jour la documentation sur les integer et les flottants
> Changer les prototypes des fonctions en utilisant le unspecified type
> Ajouter une sorte de JSON intégrée et sécurisée (stocké en binaire) pour stocker plein d'infos et les récupérer facilement
--> Faire une fonction saveVars(nom de fichier, plein de variables) et loadVars(plein de variables) qui stocke tout ça dans un fichier
> Donner des noms plus explicites aux fichiers
> Charger les fonctions de manière propre directement avec des structures



> Mettre des caractères spéciaux dans les mots clés modifiés (expt, ei, es, tr, atmc, ...)
> Faire des vrais arguments (genre -i, etc)
> Permettre à un processus d'arrêter ou de pauser un processus via une promesse

Procédure pour ajouter une fonction built-in :
------------------------------------------------
1) Programmer cette fonction selon le prototype NeObj _maFonction_ (NeList* args)
4) Ajouter 1 à la constante pré-processeur NBBUILTINFUNC définie au début de neon.h
3) Ajouter le prototype de cette fonction tout à la fin de neon.h
4) Ajouter la fonction dans le tableau de fonctions au début de ce fichier
5) Compléter les informations sur la fonction dans neonInit

NOUVEAUTéS depuis la dernière publication :
-------------------------------------------
- Nouveaux algorithmes de parsing : +0.1
    - Possibilité d'appeler des fonctions juste après l'indexation de liste, et de combiner ces schémas comme bon nous semble
    - Meilleure gestion des parenthèses dans les expressions
- Multiples blocs except +0.1
- Indication de la ligne du programme où l'erreur est survenue, et affichage de la ligne, peu importe l'erreur +0.1
- Variable Ans qui stocke le résultat des expressions dans le terminal +0.0.1
- Multiplications entre booléens et listes/strings/nombres +0.0.1
- Fonctionnalités étendues de la fonction help +0.0.0.1
- Compteur de références, nouveaux algorithmes de comparaison, de copie et fonction built-in de copie +0.1
Ajout d'un Garbage Collector et d'une fonction gc faisant appel à celui-ci
- Possibilité d'utiliser le caractère ~ dans les noms d'identificateurs pour définir des modules (pour faire des abstractions) +0.0.1
- Fonction loadNamespace pour pouvoir utiliser les fonctions sans les Prefix~ +0.0.1
- Fonctionnalités étendues de la fonction help : Affichage des types des variables dans help('variables'), ajout de help('modules'), et de l'affichage de l'aide d'un module +0.0.0.1
- Réécriture du runtime pour utiliser la pile système +0.1
- Ajout d'une fonction setColor +0.0.1
- Possibilité de surcharger les opérateurs add, sub, mul, div, minus, pow, mod, eucl, in ainsi que == et comparaison qui renvoie un entier +0.1
- Amélioration de la fonction exception et renommage en fonction raise +0.0.0.1
- Possibilité de faire des boucles for en rajoutant un pas, en enlevant la valeur de départ, et en itérant sur des listes et les chaines de caractères +0.1
- Optimisation des multiplications de chaînes de caractères +0.0.0.1
- Opérateurs or et and paresseux +0.0.0.1
- Refonte du système d'objets : les objets sont maintenant (partiellement) stockés sur la pile +0.1
- Les arguments optionnels sont évalués lors de la définition de la fonction et non lors de l'appel +0.0.0.1

--> Ne pas oublier de mettre à jour la documentation de Neon avec tout ça
*/





/************** DÉFINITION DES VARIABLES GLOBALES ESSENTIELLES AU FONCTIONNEMENT DE NEON ****************/

// Neon utilise les variables globales à foison car cela permet de ne pas trimbaler de structure d'état dans toutes les
// fonctions. Cela a pour avantage d'être (un tout petit peu) plus rapide
// en revanche l'inconvénient est qu'il est difficile d'utiliser en même temps plusieurs instances différentes de
// l'interpréteur Neon (il faut échanger la valeur de toutes ces variables globales)


// LES VARIABLES RELATIVES AU MULTITÂCHE

// cette variable indique le nombre d'instructions exécuté sur chaque processus avant de passer au suivant
// il s'agit d'une variable et non d'une constante car la fonction setAtomicTime doit pouvoir le modifier
int ATOMIC_TIME = 1500;
// cette variable est le compteur atomique périodique, qui fait changer de processus une fois arrivé à zéro
int atomic_counter = 0;

// cette variable globale sert à sauvegarder le pointeur d'origine de la pile système lorsque l'on modifie le stack pointer
// la valeur de rsp sauvegardée ici est toujours courante à eval_prolog
void* STACK_PTR = NULL;

// LES VARIABLES PERMETTANT DE GERER LES PROMESSES

// PROMISES associe aux identifiants des processus (l'indice dans le tableau) la valeur renvoyée par le processus
// Cette valeur est NULL quand le processus n'a pas terminé, et devient NeObj avec la valeur renvoyée une fois le processus terminé
NeList* PROMISES = NULL;

// PROMISES_CNT compte le nombre de promises d'un même processus créées (en faisant des copies de la promesse originale)
// qui n'ont pas encore été transformées en la valeur renvoyée par le processus
// Ces compteurs servent à savoir jusqu'à quand garder la valeur renvoyée par le processus dans PROMISES
// Quand un compteur atteint zéro, on n'a plus besoin de la valeur, donc on peut la supprimer
// c'est un tableau de pointeurs sur les compteurs de références
intptrlist PROMISES_CNT;

// une liste de booléens qui indique si un processus a fini pour savoir si on peut lui piquer sa place
// Quand un processus n'a pas fini, on ne peut pas lui prendre sa place, en revanche ce n'est pas parce qu'il a fini qu'on peut lui prendre sa place
// En effet, même si un processus a fini, parfois toutes les promesses n'ont pas encore récupéré leur valeur
intlist PROCESS_FINISH;

// cette variable globale sert à stocker le texte des exceptions lancées par l'utilisateur (user-defined exceptions)
// printError affiche cette chaine de caractères lorsqu'il détecte une exception lancée par l'utilisateur
char* EXCEPTION = NULL; // exception levée par l'utilisateur, à afficher par printError


// VARIABLES RELATIVES A LA ZONE DU PROGRAMME ACTUELLEMENT INTERPRÉTÉE
// NAME représente l'indice dans ADRESSES du NeObject représentant le nom actuel de fichier interprété
// Cette valeur est utilisée par la variable __name__
int NAME = 0; // adresse du NeObject représentant le nom actuel de fichier. A ne pas utiliser, il y a à la place la fonction updateFileName

// cette chaine de caractères représente le nom du fichier actuellement interprété
// Ce n'est pas la même chose que NAME car ceci n'est utilisé que pour se souvenir du nom du fichier dans lequel aller lire
// la ligne à laquelle s'est produite une erreur. Par conséquent l'utilisateur ne doit pas pouvoir la modifier, elle ne peut
// pas valoir "__main__" comme c'est le cas pour NAME
char* FILENAME = NULL;

// cette variable est mise à jour à chaque nouveau sous-arbre interprété, et contient le numéro de la ligne actuellement interprétée
// cette variable couplée à FILENAME permet d'afficher la ligne du programme actuellement interprétée lorsqu'une erreur est levée
int LINENUMBER = 0; // contient le numéro de la ligne actuellement interprétée

// ce tableau de chaines de caractères contient les messages d'erreurs associés à chaque code d'erreur Neon.
// Il est de taille NB_ERRORS, et peut être indexé directement par le code d'erreur
strlist ERRORS_MESSAGES;

// cette variable est une liste chainée de listes chainées de NeSave.
// chaque liste chainée de NeSave correspond à un étage de localité.
// Quand on crée un contexte, on ajoute une nouvelle liste chainée dans VAR_LOC, et quand on veut rendre une variable locale,
// on ajoute une NeSave correspondante en tête de la liste chainée en tête de VAR_LOC
//ptrlist* VAR_LOC; // c'est une pile de pile qui contiendra dans chaque contexte les variables à supprimer

// Cette variable est affectée lorsqu'une fonction utilisateur utilise return (). La valeur affectée est récupérée juste après
// par le bout de code qui avait lancé l'exécution de la fonction
NeObj RETURN_VALUE = NEO_VOID; // contient la valeur de retour de la fonction si le programme a été interrompu par un return ()

// VARIABLES RELATIVES AU CONTAINERS
// les types containers sont chacun associées à un indice unique, le même dans ces deux tableaux
// pour un type de container, son indice dans CONTAINERS donne son nom (exemple : Cmplx, Point, ...)
strlist* CONTAINERS; // à un indice du tableau, associe le nom du type de container
// dans les containers, en interne, les champs sont stockés dans une NeList, toujours dans le même ordre
// Pour chaque type de container, ATTRIBUTES contient une liste de chaines de caractères correspondant aux noms des champs dans l'ordre
NeList* ATTRIBUTES; // contient pour chaque type de container, une liste des noms de champs

// Ceci n'est pas un arbre, mais plutot une forêt. Tous les fils de cet arbre sont les arbres syntaxiques des fonctions
// définies par l'utilisateur. Les stocker permet de garder des versions uniques de ces arbres, même lors de la copie
// de fonctions utilisateurs, et de tout libérer à la fin
Tree* FONCTIONS; // cet arbre va contenir en fils tous les arbres de fonction des fonctions et des méthodes définies par l'utilisateur

// Ce couple de variables permet de gérer les variables de Neon, c'est-à-dire les objets auxquels on peut directement accéder
// par un nom depuis l'interpréteur.
// au cours de toute la vie d'une variable, son NeObj restera toujours le même, même quand on la détruit avec del
// Les noeuds des arbres faisant références à des variables stockent également le même NeObj que dans ADRESSES
// Ainsi, pour modifier une variable, on ne peut pas changer son NeObj (qui est fixe), on change donc directement son
// type, et son data. Cela engendre les magouilles qu'on fait pour appeler des fonctions et affecter ses arguments, et pour
// récupérer les valeurs de la variable objet d'une méthode
strlist* NOMS; // contient les noms des variables associés à leurs adresses mémoire dans ADRESSSES
NeList* ADRESSES; // contient les objets associés aux noms dans NOMS


// cette liste contiendra une neo_copy de tous les objets pouvant boucler sur eux-mêmes (listes, containers)
// on s'en sert pour le Mark & Sweep : on marque tous les objets accessibles depuis ADRESSES, et ensuite on parcourt les objets
// de OBJECTS_LIST, et s'il ne sont pas marqués, c'est qu'ils ne sont pas accessibles et doivent être supprimés
// ceci est un objet qui contiendra un container ou une nelist. Il s'agit de la tête d'une liste chaînée de containers
// et de nelist. Tous les containers et nelist existants sont chaînés dedans
// les containers et nelist contiennent un champ spécial qui est le chaînon suivant de la liste
NeObj OBJECTS_LIST = NEO_VOID;

// cet objet est le cycle des différents processus qui s'exécutent en même temps. L'interpréteur passe son temps à
// passer d'un processus à l'autre dans une dans infernale. Pour ajouter un processus, il suffit d'ajouter un chaînon
// dans ce cycle
ProcessCycle* process_cycle;



// VARIABLES GLOBALES SERVANT A DEFINIR LES ELEMENTS MODIFIABLES DE LA SYNTAXE
// UNIQUEMENT UTILISES PAR LIBTOK (et un tout petit peu LIBSTR)

//définition de acceptedChars
strlist acceptedChars ;

// definit les types de tokens composes et indique par quel type de token compose ils peuvent etre precedes
listlist syntax;
intlist types_debut;
intlist types_fin;

//définition de sousop
strlist sousop ;

// liste qui contient les opérandes prises par les opérateurs, correspond a operateurs3[]
intlist gramm1 ;

//définition de operateurs3
strlist operateurs3;

//définition de operateurs1
// opérateurs composés de caractères de sousop
strlist operateurs1;

// opérateurs mots
strlist operateurs2;

//mots de blocs d'instructions
strlist blockwords;
strlist blockwords1Line;
strlist keywordFunction;
strlist keywords;
strlist lkeywords;
strlist neon_boolean;
strlist exceptions;
strlist constant;
strlist OPERATEURS;
intlist PRIORITE;
intlist OPERANDES;
intlist exceptions_err;

strlist NOMSBUILTINSFONC;









void printRes(NeObj res)
{
    if (NEO_TYPE(res) != TYPE_NONE)
    {
        printString(" (");
        setColor(GREEN);
        printString(type(res));
        setColor(WHITE);
        printString(") = ");
        
        // on a besoin d'un runtime pour exécuter neobject_aff car il pourrait lancer une fonction utilisateur si surchargé
        initRuntime();
        neobject_aff(res);
        exitRuntime();
        
        newLine();
    }
    return;
}





void startMessage(void)
{
    setColor(BLUE);
    printString("                  Welcome to Neon");

    newLine();

    setColor(GREEN);    
    printString("Neon v");
    printString(VERSION);
    setColor(WHITE);

    printString(" for ");
    printString(PLATFORM);

    printString(", compiled on ");
    printString(__TIMESTAMP__);
    /*printString(" by GCC ");
    printString(__VERSION__);*/
    newLine();
    
    /*printString("Discuss on the official Discord server : ");
    printString(INVITATION);
    printString(" or contact me at raphael@langage-neon.org");
    newLine();*/
    
    printString("Visit ");
    setColor(GREEN); printString("https://langage-neon.raphaael.fr"); setColor(WHITE);
    printString(" for more information.");
    newLine();

    #ifdef EXPERIMENTAL
        newLine();
        setColor(RED);
        printString("/!\\ THIS IS AN EXPERIMENTAL VERSION. IT MAY NOT WORK. /!\\");
        newLine();newLine();
        setColor(WHITE);
    #endif

    return;
}






#ifdef WASM
    void EMSCRIPTEN_KEEPALIVE execCode(char* code)
    {
        
    	if (strcmp(code,"")==0) // si l'utilisateur n'a rien ecrit
    	    return;
        
        FILENAME = NULL;
    
    	Tree* tree = tree_create(NULL, 0, 0);
    	createSyntaxTree(tree, code);

        if (CODE_ERROR != 0)
        {
            printError(CODE_ERROR);
            CODE_ERROR = 0;
            tree_destroy(tree);
            return;
        }

        FILENAME = strdup(tree->label1);

    	// s'il n'y a qu'une expression, alors, on affiche le résultat de l'expression
    	if (tree->nbSons == 1 && tree->sons[0]->type != TYPE_TRYEXCEPT && tree->sons[0]->type != TYPE_CONDITIONBLOCK && tree->sons[0]->type != TYPE_STATEMENTFOR && tree->sons[0]->type != TYPE_STATEMENTFOREACH && tree->sons[0]->type != TYPE_STATEMENTWHILE && tree->sons[0]->type != TYPE_KEYWORD && tree->sons[0]->type != TYPE_FUNCTIONDEF && tree->sons[0]->type != TYPE_BLOCKWORD1LINE && tree->sons[0]->type != TYPE_ATOMICBLOCK)
        {
            
            res = eval(tree->sons[0]);


            if (CODE_ERROR != 1 && CODE_ERROR != 0)
            {
                printError(CODE_ERROR);
                tree_destroy(tree);
                continue;
            }
            else if (CODE_ERROR == 1) // quitte le terminal
            {
                tree_destroy(tree);
                return ;
            }
            
            printRes(res);
            neobject_destroy(res,true);
        }
        else if (tree->nbSons > 0)
        {
            exec(tree);
            if (CODE_ERROR != 1 && CODE_ERROR != 0)
            {
                printError(CODE_ERROR);
                tree_destroy(tree);
                continue;
            }
            else if (CODE_ERROR == 1) // quitte le terminal
            {
                tree_destroy(tree);
                return ;
            }
        }
        CODE_ERROR = 0;
    	tree_destroy(tree);
        return ;
    }




    char* EMSCRIPTEN_KEEPALIVE inputMessage(void)
    {
        return INPUTMESSAGE;
    }

#endif







// cette fonction permet de réutiliser le résultat d'une expression grâce à la variable Ans dans le terminal
void storeAns(NeObj res) {
    if (strlist_inList(NOMS, "Ans")) {
        int index = strlist_index(NOMS, "Ans");
        neobject_destroy(ADRESSES->tab[index]); // supprime totalement le précédent
        ADRESSES->tab[index] = res; // met le nouveau dedans
    }
    else {
        strlist_append(NOMS, strdup("Ans"));
        nelist_append(ADRESSES, res);
    }
}






void terminal(void)
{
    char* exp;
    NeObj res ; Tree* tree;
        
        while (true)
        {
            CODE_ERROR = 0; // réinitialise les erreurs

            if (FILENAME != NULL)
                free(FILENAME);
            FILENAME = NULL;

            exp = inputCode(SEQUENCE_ENTREE);

            if (exp == NULL && CODE_ERROR == 0) // pour afficher le keyboardInterrupt
                CODE_ERROR = 104;
            
            // quand l'utilisateur appuie sur CTRL-D, ça met CODE_ERROR à 1
            if (CODE_ERROR != 1 && CODE_ERROR != 0)
            {
                printError(CODE_ERROR);
                CODE_ERROR = 0;
                continue;
            }
            else if (CODE_ERROR == 1)
                return;



            if (strcmp(exp,"")==0) // si l'utilisateur n'a rien ecrit
            {
                free(exp);
                continue;
            }

            
            
            tree = tree_create(NULL, 0, 0);

            if (CODE_ERROR != 0)
            {
                printError(CODE_ERROR);
                free(exp);
                continue;
            }


            createSyntaxTree(tree, exp);

            free(exp);

            if (CODE_ERROR != 1 && CODE_ERROR != 0)
            {
                printError(CODE_ERROR);
                tree_destroy(tree);
                continue;
            }

            
            // s'il n'y a qu'une expression, alors, on affiche le résultat de l'expression
            if (tree->nbSons == 1 && tree->sons[0]->type != TYPE_TRYEXCEPT && tree->sons[0]->type != TYPE_CONDITIONBLOCK && tree->sons[0]->type != TYPE_STATEMENTFOR && tree->sons[0]->type != TYPE_STATEMENTFOREACH && tree->sons[0]->type != TYPE_STATEMENTWHILE && tree->sons[0]->type != TYPE_KEYWORD && tree->sons[0]->type != TYPE_FUNCTIONDEF && tree->sons[0]->type != TYPE_BLOCKWORD1LINE && tree->sons[0]->type != TYPE_ATOMICBLOCK)
            {
                res = eval(tree->sons[0]);

                if (CODE_ERROR != 1 && CODE_ERROR != 0)
                {
                    printError(CODE_ERROR);
                    tree_destroy(tree);
                    continue;
                }
                else if (CODE_ERROR == 1) // quitte le terminal
                {
                    tree_destroy(tree);
                    return ;
                }
                
                printRes(res);

                storeAns(res); // stocke le résultat dans une variable au lieu de le supprimer
            }
            else if (tree->nbSons > 0)
            {
                exec(tree);
                if (CODE_ERROR != 1 && CODE_ERROR != 0)
                {
                    printError(CODE_ERROR);
                    tree_destroy(tree);
                    continue;
                }
                else if (CODE_ERROR == 1) // quitte le terminal
                {
                    tree_destroy(tree);
                    return ;
                }
            }
            tree_destroy(tree);

        }

    return ;
}





void execFile(char* filename)
{
    
    char* program = openFile(filename); // fonction dépendant du système cible
    
    if (CODE_ERROR != 0)
        printError(CODE_ERROR);
    CODE_ERROR = 0;

    if (program == NULL)
        return ;
    
    //printf("Création de l'arbre syntaxique\n");

    FILENAME = strdup(filename);
    
    // exécution du fichier
    Tree* tree = tree_create(NULL, 0, 0);
    createSyntaxTree(tree, program);
    
    free(program);

    //printf("Arbre syntaxique terminé\n");

    if (CODE_ERROR != 0)
    {
        printError(CODE_ERROR);
        CODE_ERROR = 0;
        tree_destroy(tree);
        return;
    }

    exec(tree);
    
    if (CODE_ERROR != 1 && CODE_ERROR != 0)
    {
        printError(CODE_ERROR);
    }

    tree_destroy(tree);

    return ;
}








void importFile(char* filename)
{
    char* program = openFile(filename); // fonction dépendant du système cible
    
    if (CODE_ERROR != 0)
        return;

    if (program == NULL) {
        CODE_ERROR = 67;
        return ;
    }

    char* sov = FILENAME;
    FILENAME = strdup(filename);
    
    // exécution du fichier
    Tree* tree = tree_create(NULL, 0, 0);
    createSyntaxTree(tree, program);
    free(program);

    if (CODE_ERROR != 0)
    {
        free(sov);
        tree_destroy(tree);
        return;
    }

    exec_aux(tree);

    if (CODE_ERROR != 0) {
        free(sov);
        tree_destroy(tree);
        return;
    }

    free(FILENAME);
    FILENAME = sov;

    tree_destroy(tree);
    return ;
}




void defineVariables(void)
{
    // définition de la variable de version
    strlist_append(NOMS, strdup("__version__"));
    nelist_append(ADRESSES, neo_str_create(strdup(VERSION)));

    // définition de la variable de plateforme

    strlist_append(NOMS, strdup("__platform__"));
    nelist_append(ADRESSES, neo_str_create(strdup(PLATFORM)));

    strlist_append(NOMS, strdup("__name__"));
    nelist_append(ADRESSES, NEO_VOID);

    NAME = ADRESSES->len - 1; // l'adresse de __name__ à modifier


    // pi
    nelist_append(ADRESSES, neo_double_create(PI));
    strlist_append(NOMS, strdup("pi"));
}












void neonInit(void)
{
    #if defined(LINUX)
        linenoiseSetMultiLine(1); // spécial pour linenoise
        signal(SIGINT, handle_sigint);
    #endif
    
    #if defined(WINDOWS10) || defined(WINDOWS11)
        SetConsoleCtrlHandler(ctrlHandler, TRUE);
    #endif


    //définition de acceptedChars
    const char* acceptedChars_tab[32] = {"\"", "'", "+","*","-","/","<",">","=","%","&","@","!", ",", ";", "\n", "#", "$", "[", "]", "(", ")", "{", "}", "\\", ".", "_", " ", "\t", ".", ":", "~"};
    strlist_copy(&acceptedChars, acceptedChars_tab, 32);



    // definit les types de tokens composes et indique par quel type de token compose ils peuvent etre precedes

    // les tableaux à l'intérieur ne sont pas des const, car sinon ça pose un problème avec le type strlist. Ce n'est pas grave, j'enlèverais ce tableau bientôt pour mettre quelque chose de moins "lourd"
    const intlist syntax_tab[22] = {
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_KEYWORDFUNCTION} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1, TYPE_DOUBLE, TYPE_INTEGER, TYPE_EXPRESSION, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_FONCTION, TYPE_BOOL, TYPE_LISTINDEX, TYPE_ENDOFLINE,  TYPE_OPERATOR, TYPE_PARENTHESE2, TYPE_CONST, TYPE_VIRGULE, TYPE_EXCEPTION} , .len = 16},//suivant la grammaire de chaque operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_CROCHET} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_CROCHET} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_BLOCKLINE, TYPE_KEYWORD, TYPE_FONCTION} , .len = 4},
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE,TYPE_PARENTHESE2} , .len = 5}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_BLOCK} , .len = 2},
            (intlist) {.tab = (int[]) {TYPE_VIRGULE, TYPE_DOUBLE, TYPE_INTEGER, TYPE_EXPRESSION, TYPE_PARENTHESE1, TYPE_VARIABLE, TYPE_LISTINDEX, TYPE_BOOL, TYPE_OPERATOR, TYPE_LIST, TYPE_STRING, TYPE_BLOCKLINE, TYPE_BLOCK, TYPE_FONCTION, TYPE_PARENTHESE2, TYPE_KEYWORD, TYPE_CONST, TYPE_BLOCKWORD1LINE, TYPE_EXCEPTION} , .len = 19}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_VIRGULE, TYPE_PARENTHESE1} , .len = 4},
            (intlist) {.tab = (int[]) {TYPE_OPERATOR, TYPE_DOUBLE, TYPE_INTEGER, TYPE_EXPRESSION, TYPE_STRING, TYPE_LIST, TYPE_FONCTION, TYPE_LISTINDEX, TYPE_VARIABLE, TYPE_PARENTHESE2,TYPE_BOOL} , .len = 11},
            (intlist) {.tab = (int[]) {TYPE_DOUBLE, TYPE_INTEGER, TYPE_EXPRESSION, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_FONCTION, TYPE_BOOL, TYPE_LISTINDEX, TYPE_ENDOFLINE, TYPE_OPERATOR, TYPE_CONST, TYPE_EXCEPTION} , .len = 13},
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE, TYPE_BLOCK} , .len = 2},
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE} , .len = 4}, // suivant la grammaire de l’operateur
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE}, .len = 1},
            (intlist) {.tab = (int[]) {TYPE_ENDOFLINE}, .len = 1},
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE}, .len = 4},
            (intlist) {.tab = (int[]) {TYPE_PARENTHESE1,TYPE_OPERATOR,TYPE_ENDOFLINE,TYPE_VIRGULE, TYPE_CROCHET} , .len = 5}
    };


    listlist_copy(&syntax, syntax_tab, 22);

    // types acceptés en début d'entrée

    
    int types_debut_temp[22] = {TYPE_FONCTION, TYPE_PARENTHESE1, TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_LISTINDEX, TYPE_DOUBLE, TYPE_INTEGER, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_BOOL, TYPE_BLOCKLINE, TYPE_KEYWORD, TYPE_CONST, TYPE_WORD, TYPE_COMMENT, TYPE_CROCHET, TYPE_BLOCKWORD, TYPE_BLOCKWORD1LINE, TYPE_EXPRESSION, TYPE_EXCEPTION};
    intlist_copy(&types_debut, types_debut_temp, 22);

    int types_fin_temp[21] = {TYPE_FONCTION, TYPE_PARENTHESE2, TYPE_OPERATOR, TYPE_ENDOFLINE, TYPE_LISTINDEX, TYPE_DOUBLE, TYPE_INTEGER, TYPE_STRING, TYPE_LIST, TYPE_VARIABLE, TYPE_BOOL, TYPE_KEYWORD, TYPE_CONST, TYPE_WORD, TYPE_COMMENT, TYPE_CROCHET, TYPE_EXPRESSION, TYPE_EXCEPTION, TYPE_BLOCK, TYPE_BLOCKWORD1LINE};
    intlist_copy(&types_fin, types_fin_temp, 21);


    //définition de sousop
    const char* sousop_temp[] = {"+","*","-","/","<",">","=","%","&","@","!", ".",":"};
    strlist_copy(&sousop, sousop_temp, 13);


    // liste qui contient les opérandes prises par les opérateurs, correspond a operateurs3
    int gramm1_temp[NBOPERATEURS] = {RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,RIGHT, RIGHT, VARRIGHT, LEFT_VAR, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, VAR_VAR, VAR_RIGHT, SPECIAL, RIGHT};
    intlist_copy(&gramm1, gramm1_temp, NBOPERATEURS);



    //définition de operateurs3
    const char* operateurs3_temp[NBOPERATEURS] = {"and","or","xor","+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","not","%","//","&","<-","@", "_", "del", "->", ".", "EE", "=>", "in", ">>", ":", "<->", ":=", "...", "parallel"};
    strlist_copy(&operateurs3, operateurs3_temp, NBOPERATEURS);


    //définition de operateurs1
    // opérateurs composés de caractères de sousop
    const char* operateurs1_temp[] = {"+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","%","//","&","<-","@", "->", ".", "=>", ">>", ":", "<->", ":=", "..."};
    strlist_copy(&operateurs1, operateurs1_temp, 31);


    // opérateurs mots
    const char* operateurs2_temp[] = {"and","or","xor","not", "del", "EE", "in", "parallel"};
    strlist_copy(&operateurs2, operateurs2_temp, 8);



    const char* OPERATEURS_temp[NBOPERATEURS] = {"and","or","xor","+","*","-","/","**","==","!=","<=",">=","<",">","=","+=","-=","*=","/=","++","--","not","%","//","&","<-","@", "_", "del", "->", ".", "EE", "=>", "in", ">>", ":", "<->", ":=", "...", "parallel"};
    strlist_copy(&OPERATEURS, OPERATEURS_temp, NBOPERATEURS);


    const int PRIORITE_temp[NBOPERATEURS] = {7, 7, 7, 5, 4, 5, 4, 3, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, 3, 3, 7, 4, 4, 2, 8, 2, 1, 8, 8, 2, 3, 7, 6, 0, 8, 8, 8, 8, 7};
    intlist_copy(&PRIORITE, PRIORITE_temp, NBOPERATEURS);

    const int OPERANDES_temp[NBOPERATEURS] = {RIGHT_LEFT | LAZY,RIGHT_LEFT | LAZY,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,RIGHT_LEFT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VAR_RIGHT,VARLEFT,VARLEFT,RIGHT,RIGHT_LEFT,RIGHT_LEFT,VARRIGHT,RIGHT_LEFT,RIGHT, RIGHT, VARRIGHT, LEFT_VAR, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT | LAZY, RIGHT_LEFT, RIGHT_LEFT, RIGHT_LEFT, VAR_VAR, VAR_RIGHT, 0, RIGHT};
    intlist_copy(&OPERANDES, OPERANDES_temp, NBOPERATEURS);


    // ------------------

    // définit les tokens de délimitation de blocks

    //mots de blocs d'instructions
    const char* blockwords_temp[] = {"if","while","for", "foreach", "ei", "return", "import", "local", "tr", "expt", "await", "atmc"};
    strlist_copy(&blockwords, blockwords_temp, 12);

    const char* blockwords1Line_temp[] = {"return", "import", "local", "await"};
    strlist_copy(&blockwords1Line, blockwords1Line_temp, 4);

    const char* keywordFunction_temp[] = {"function", "method"};
    strlist_copy(&keywordFunction, keywordFunction_temp, 2);

    const char* keywords_temp[] = {"es", "continue", "break", "pass", "tr", "atmc"};
    strlist_copy(&keywords, keywords_temp, 6);

    const char* lkeywords_temp[] = {"continue", "break", "pass"};
    strlist_copy(&lkeywords, lkeywords_temp, 3);


    const char* neon_boolean_temp[] = {"True","False"};
    strlist_copy(&neon_boolean, neon_boolean_temp, 2);

    const char* exceptions_temp[] = {
        "SyntaxError",
        "FileNotFound",
        "UnmeasurableObject",
        "UndefinedVariable",
        "IncorrectFunctionCall",
        "MemoryError",
        "NonIndexableObject",
        "IncorrectIndex",
        "OutOfRange",
        "IncorrectType",
        "DivisionByZero",
        "UnknownError",
        "AssertionFailed",
        "DefinitionError",
        "KeyboardInterrupt"
    };
    // ici il faut s'assurer que capacity a la bonne valeur
    exceptions.tab = malloc(sizeof(char*)*16);
    exceptions.len = 15;
    exceptions.capacity = 4;
    for (int i = 0 ; i < 15 ; i++)
        exceptions.tab[i] = strdup(exceptions_temp[i]);
    

    const char* constant_temp[] = {"None", "Infinity", "NaN"};
    strlist_copy(&constant, constant_temp, 3);




    /*----- Préparation des fonctions ------*/
    const char* NOMSBUILTINSFONC_temp[NBBUILTINFUNC] = {"print","input","nbr","str","len","sub","exit","append","remove","insert","type", "reverse", "eval","clear","help", "randint", "failwith", "time", "assert", "output", "chr", "ord", "listComp", "createException", "raise", "int", "index", "replace", "count", "list", "sortAsc", "sortDesc", "sin", "cos", "tan", "deg", "rad", "sqrt", "ln", "exp", "log", "log2", "round", "abs", "ceil", "floor", "readFile", "writeFile", "setFunctionDoc", "setAtomicTime", "copy", "loadNamespace", "gc", "setColor"};
    strlist_copy(&NOMSBUILTINSFONC, NOMSBUILTINSFONC_temp, NBBUILTINFUNC);

    const int typesRetour[NBBUILTINFUNC] = {TYPE_NONE, TYPE_STRING, TYPE_ANYTYPE, TYPE_STRING, TYPE_INTEGER, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_STRING, TYPE_STRING, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_INTEGER, TYPE_NONE, TYPE_INTEGER, TYPE_NONE, TYPE_NONE, TYPE_STRING, TYPE_INTEGER, TYPE_LIST, TYPE_EXCEPTION, TYPE_NONE, TYPE_INTEGER, TYPE_INTEGER, TYPE_STRING, TYPE_INTEGER, TYPE_LIST, TYPE_NONE, TYPE_NONE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_STRING, TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_ANYTYPE, TYPE_NONE, TYPE_NONE, TYPE_NONE};

    const char* helpbuiltinsfonc[NBBUILTINFUNC] = {
        "Displays arguments in the terminal",
        "Displays the given argument and prompts the user to input in the terminal. Returns the entered value.",
        "Converts a string into a number.",
        "Converts any object into a string.",
        "Returns the length of an object.",
        "Performs substring extraction. sub(\"hello\", 1, 4) returns \"ell\".",
        "Exits the Neon interpreter.",
        "Adds an element to a list.",
        "Removes an element from a list.",
        "Inserts an element into a list.",
        "Returns the type of an object.",
        "Reverses the characters in a string of characters or reverses a list.",
        "Evaluates the given string of characters and returns the result.",
        "Clears the terminal.",
        "This function displays all kinds of information on objects and modules.\nType help() for more information.",
        "Returns a random integer between two integers.",
        "Displays the provided text in the terminal and exits the program.",
        "Returns the time elapsed since 1970 in seconds.",
        "Stops the program if at least 1 assertion has failed.",
        "Puts the given arguments on the terminal without spaces or \\n.",
        "Returns the ASCII character associated to an integer.",
        "Returns the ASCII value of the given character.",
        "Arguments : index, start, end, step, condition, value\nEvaluates value for all indices between start and end according to step, and adds it to the list if condition is true.\nindex must be the name of the variable used, and condition and value must be string expressions.",
        "Creates an Exception object with the given name.",
        "Exits the program with the given exception and displays the given message",
        "Converts an object into an integer",
        "Returns the index of an element in a list.",
        "replace(str, str1, str2) returns a string in which all occurrences of str1 have been replaced by str2 in str.",
        "Counts the number of occurrences in a list or a string.",
        "Separates every character of a string in a list.",
        "Sorts a list in ascending order.",
        "Sorts a list in descending order.",
        "sin: Sine function",
        "cos: Cosine function",
        "tan: Tangent function",
        "deg: Convert angle from radians to degrees",
        "rad: Convert angle from degrees to radians",
        "sqrt: Square root function",
        "ln: Natural logarithm (base e)",
        "exp: Exponential function (e^x)",
        "log: Logarithm (base 10)",
        "log2: Logarithm (base 2)",
        "round: Round to the nearest decimal number with the given precision",
        "abs: Absolute value",
        "ceil: Ceiling function (smallest integer greater than or equal to)",
        "floor: Floor function (largest integer less than or equal to)",
        "Returns the content of the file whose name was given",
        "Writes the given string in the file whose name was given. The syntax is writeFile(name, content)",
        "Sets a string documentation for a user-defined function or method",
        "Time to allow for each process before switching",
        "Performs a deep copy of an object, preserving the pointer dependencies",
        "Loads any variable from a given namespace without the namespace prefix",
        "Calls the Garbage Collector",
        "Changes the writing text color in console if available.\nColors: red, green, blue and white"
    };


    const int nbargs[NBBUILTINFUNC] = {-1,-1,1,1,1,3,0,2,2,3,1,1,1,0,-1,2,1,0,-1,-1,1,1,6,1,2,1,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,2,2,1,1,1,0,1}; // nombre d'arguments en tout

    // les fonctions ayant un nombre illimité d'arguments ne doivent avoir qu'un seul élément dans typeArgs
    const int* typesArgs[NBBUILTINFUNC] = {
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_STRING},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_STRING, TYPE_INTEGER, TYPE_INTEGER},
        NULL,
        (int[]){TYPE_LIST, TYPE_ANYTYPE},
        (int[]){TYPE_LIST, TYPE_INTEGER},
        (int[]){TYPE_LIST, TYPE_ANYTYPE, TYPE_INTEGER},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_STRING},
        (int[]){TYPE_STRING},
        NULL,
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_INTEGER, TYPE_INTEGER},
        (int[]){TYPE_STRING},
        NULL,
        (int[]){TYPE_BOOL},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_INTEGER},
        (int[]){TYPE_STRING},
        (int[]){TYPE_STRING, TYPE_INTEGER, TYPE_INTEGER, TYPE_INTEGER, TYPE_STRING, TYPE_STRING},
        (int[]){TYPE_STRING},
        (int[]){TYPE_EXCEPTION, TYPE_STRING},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_LIST, TYPE_ANYTYPE},
        (int[]){TYPE_STRING, TYPE_STRING, TYPE_STRING},
        (int[]){TYPE_ANYTYPE, TYPE_ANYTYPE},
        (int[]){TYPE_STRING},
        (int[]){TYPE_LIST},
        (int[]){TYPE_LIST},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_DOUBLE, TYPE_DOUBLE},
        (int[]){TYPE_DOUBLE},
        (int[]){TYPE_DOUBLE},
        (int[]){TYPE_DOUBLE},
        (int[]){TYPE_STRING},
        (int[]){TYPE_STRING, TYPE_STRING},
        (int[]){TYPE_ANYTYPE, TYPE_STRING},
        (int[]){TYPE_INTEGER},
        (int[]){TYPE_ANYTYPE},
        (int[]){TYPE_STRING},
        NULL,
        (int[]){TYPE_STRING}
    };

    /* ----- fin préparation des fonctions ------*/


    // tableau des messages d'erreur :

    const char* ERRORS_MESSAGES_temp[NB_ERRORS] = {
        "",// ne doit pas être utilisé
        "",// ne doit pas être utilisé
        "Multiple decimal points in the same number",
        "File doesn't exist",
        "This object has no length",
        "Use of an undefined variable",
        "Too many arguments in user-defined function call",
        "Insufficient arguments in user-defined function call",
        "Call to an undefined function",
        "Call to an invalid function",
        "Starting or ending value of the for loop is not a number",
        "Incorrect function definition",
        "Memory error",
        "Incorrect index for subtree deletion",
        "Invalid arguments provided in function call",
        "This object is not indexable",
        "List index is not a number",
        "Attempt to index an undefined list",
        "List index out of range",
        "Undetermined error, likely a syntax issue",
        "Unevaluable condition due to incorrect type",
        "Multiple arguments for return statement",
        "'For' loop variant is not a variable",
        "Incorrect built-in function name in function call",
        "Unknown operator",
        "Unknown character",
        "String, list, or other structure not terminated",
        "Unclosed string at the end of line",
        "Incompatible keyword used in an expression",
        "Block instruction line not followed by an instruction block (e.g., a lone if() statement)",
        "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
        "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
        "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
        "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
        "Lone block of instructions",
        "Incorrect statement definition",
        "Incorrect number of arguments when calling a built-in function",
        "Index out of range for inserting into a NeList",
        "Index out of range for deleting from a NeList",
        "Index out of range to access in a NeList",
        "Unsupported types for addition",
        "Unsupported types for subtraction",
        "Unsupported types for division",
        "Unsupported types for multiplication",
        "Unsupported types for modulo",
        "Euclidean division by zero",
        "Unsupported types for Euclidean division",
        "Unsupported types for 'and' operator",
        "Unsupported types for 'or' operator",
        "Unsupported types for 'xor' operator",
        "Unsupported types for exponentiation",
        "Unsupported types for equality tests",
        "Unsupported types for <=",
        "Unsupported types for >=",
        "Unsupported types for <",
        "Unsupported types for >",
        "Attempt to assign the value of an undefined variable to another variable",
        "Improper use of the '->' operator",
        "Unsupported types for 'not' operator",
        "Referencing an undefined variable",
        "Dereferencing something other than a string",
        "Unsupported types for unary minus operator",
        "Unsupported types for reverse function",
        "Attempt to use help on a non-function variable",
        "The randint function must have two positive integers a and b as arguments, with a < b",
        "Pointer is not allocated",
        "Undefined error",
        "Can not open file",
        "More closing parentheses, curved brackets or square brackets than opening ones",
        "local must have at least one argument",
        "local can only be used in a function or in a method",
        "Assertion failed",
        "The '.' operator is only for functions or methods and can only be applied on a variable",
        "Incorrect hexadecimal or binary number",
        "", // code 74 spécial pour les appels d'exceptions
        "Unsupported types for implication.",
        "except block not preceded by a try block.",
        "try block not followed by an except block",
        "except expects exceptions",
        "in can only be calculated with a list",
        "'>>' is only for containers",
        "A container field is set but the container is not used",
        "Unknown container field",
        "Same container name but not same fields",
        "Containers must contain at least one field",
        "Unknown container type",
        "Two different fields cannot have the same name in a container",
        "Incorrect container definition",
        "Object not in list",
        "Cannot assign a value or execute a method on a litteral object",
        "That list cannot be sorted",
        "Cannot call a function while dereferencing a container field",
        "':' outside container definition",
        "Unknown argument name in user-defined function call",
        "You have already given a value to this argument in user-defined function call",
        "':=' outside function call or function definition",
        "Bad arguments definition in function definition",
        "Use of '...' outside function definition",
        "Can only set a documentation for a user-defined function or method",
        "Use of return outside function",
        "Can only call a user-defined function in parallel",
        "await takes exactly one argument",
        "Atomic time must be a positive integer, greater or equal to 1",
        "Methods cannot be applied to volatile objects such as function or operators",
        "Program interrupted", // code 104 spécial pour intercepter un CTRL-C
        "Cannot assign a value to a string index",
        "Stack overflow encountered during runtime",
        "This operator has not yet been implemented for this type of container",
        "For loop can only be defined with 2, 3 or 4 arguments as follows : \n for (var, end)\n for (var, start, end)\n for (var, start, end, step)",
        "foreach needs exactly 2 arguments : a variable and a list or a string",
        "Methods must have at least one argument, and this argument cannot be optional",
        "for/foreach first argument must be a variable",
        "listComp range parameters must be integers",
        "This object cannot be converted to an integer",
        "This function can only receive integer of decimal arguments"
    };

    strlist_copy(&ERRORS_MESSAGES, ERRORS_MESSAGES_temp, NB_ERRORS);

    const int exceptions_err_temp[NB_ERRORS] = {
        0,// ne doit pas être utilisé
        0,// ne doit pas être utilisé
        0,
        1,
        2,
        3,
        4,
        4,
        3,
        4,
        0,
        0,
        5,
        11,
        4,
        6,
        7,
        3,
        8,
        0,
        9,
        0,
        9,
        4,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        4,
        8,
        8,
        8,
        9,
        9,
        9,
        9,
        9,
        10,
        9,
        9,
        9,
        9,
        9,
        9,
        9,
        9,
        9,
        9,
        3,
        9,
        9,
        3,
        9,
        9,
        9,
        9,
        9,
        5,
        11,
        1,
        0,
        0,
        9,
        12,
        9,
        0,
        0,
        9,
        0,
        0,
        9,
        9,
        9,
        5,
        3,
        13,
        13,
        13,
        13,
        0,
        8,
        4,
        9,
        0,
        0,
        4,
        4,
        0,
        0,
        0,
        9,
        0,
        0,
        0,
        8,
        4,
        14,
        9,
        5,
        9,
        0,
        0,
        0,
        0,
        9,
        9,
        9
    };

    intlist_copy(&exceptions_err, exceptions_err_temp, NB_ERRORS);



    // ----- fin messages d'erreur

    CONTAINERS = strlist_create(0);
    ATTRIBUTES = nelist_create(0);


    PROMISES = nelist_create(0);
    PROMISES_CNT = intptrlist_create(0);
    PROCESS_FINISH = intlist_create(0);

    process_cycle = processCycle_create();


    //VAR_LOC = ptrlist_create();

    FONCTIONS = tree_create(NULL, 0, 0);
    ADRESSES = nelist_create(0);
    NOMS = strlist_create(0);
    compFunc(helpbuiltinsfonc, nbargs, typesArgs, typesRetour);

    srand(time(NULL));

    // définition des variables de base

    defineVariables();

    return;
}


void neonExit(void)
{
    //destruction de tout ce qui a été alloué.

    free(process_cycle);

    neobject_destroy(RETURN_VALUE);

    // il faut destroy tout
    strlist_destroy(&acceptedChars, false);
    listlist_destroy(&syntax);
    strlist_destroy(&sousop, false) ;
    free(gramm1.tab) ;
    free(types_debut.tab);
    free(types_fin.tab);
    strlist_destroy(&operateurs3, false);
    strlist_destroy(&operateurs1, false);
    strlist_destroy(&operateurs2, false);
    strlist_destroy(&blockwords, false);
    strlist_destroy(&blockwords1Line, false);
    strlist_destroy(&keywords, false);
    strlist_destroy(&lkeywords, false);
    strlist_destroy(&keywordFunction, false);
    strlist_destroy(&neon_boolean, false);
    strlist_destroy(&exceptions, false);
    strlist_destroy(&constant, false);
    strlist_destroy(&OPERATEURS, false);
    free(PRIORITE.tab);
    free(OPERANDES.tab);
    free(exceptions_err.tab);
    strlist_destroy(&NOMSBUILTINSFONC, false);
    strlist_destroy(&ERRORS_MESSAGES, false);

    tree_destroy(FONCTIONS);

    strlist_destroy(CONTAINERS, true);
    nelist_destroy(ATTRIBUTES);

    nelist_destroy(ADRESSES);
    strlist_destroy(NOMS, true);

    // ramassage de miettes (garbage collection)
    gc_final_sweep();

    nelist_destroy(PROMISES);
    intptrlist_destroy(&PROMISES_CNT);
    free(PROCESS_FINISH.tab);

    if (FILENAME != NULL)
        free(FILENAME);

}




#ifdef TI83PCE
    int main(void)
#else
    int main (int argc, char* argv[])
#endif
{

    //stockage des variables
    neonInit();


    // définition de la liste des arguments
    NeObj l = neo_list_create(0);
    updateFileName(strdup("__main__")); // nom du fichier actuel

    #ifdef TI83PCE
        // récupération du nom de fichier si existant
        nio_init(&console, NIO_MAX_COLS, NIO_MAX_ROWS, 0, 0, NIO_COLOR_BLACK, NIO_COLOR_WHITE, true);
        nio_set_default(&console);

        char* filename = argsAns();

        nelist_append(ADRESSES,l);
        strlist_append(NOMS,strdup("__args__"));

        if (filename == NULL) // lance le terminal
        {
            startMessage();
            terminal();
        }
        else
        {
            execFile(filename);
            free(filename);
        }
        neonExit();
    #else
        #ifndef WASM

            // ajout des arguments dans le tableau contenant les arguments du programme
            for (int i = 2 ; i < argc ; i++)
                neo_list_append(l,neo_str_create(strdup(argv[i])));

            nelist_append(ADRESSES,l);
            strlist_append(NOMS,strdup("__args__"));

            updateFileName(strdup("__main__"));

            if (argc >= 2)
            {
                execFile(argv[1]);
            }
            else
            {
                startMessage();
                terminal();
            }

            neonExit();
        #endif
    #endif


    #ifdef TI83PCE
        nio_free(&console);
    #endif

    return 0;
}
