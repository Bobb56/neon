#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "headers/constants.h"
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
#include "headers/processcycle.h"
#include "headers/operators.h"

// Variables d'environnement de Neon
NeonEnv* global_env = NULL;


#ifdef TI_EZ80
    #include "nio_ce.h"
    nio_console console;
#endif




#if defined(LINUX_AMD64)
    #include <signal.h>

    void handle_signal(int sig) {
        global_env->CODE_ERROR = 104;
    }
#endif


#ifdef WINDOWS_AMD64
    #include <windows.h>

    // Fonction de gestion du signal
    BOOL WINAPI ctrlHandler(DWORD signal) {
        if (signal == CTRL_C_EVENT) {
            global_env->CODE_ERROR = 104;
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

S'il y a des problèmes liés aux piles et tout, bien vérifier les tailles des contextes des fonctions et regarder si on gère ça correctement

Peut être qu'il y a des endroits où on oublie de mettre à jour les promesses

J'ai enlevé les vérifications de CODE_ERROR juste après avoir restauré les registres et la pile, ça peut causer des problèmes

General Protection Fault = Problème d'alignement de la pile


Avancement et choses à faire :
-------------------------------
> Ajouter une sorte de JSON intégrée et sécurisée (stocké en binaire) pour stocker plein d'infos et les récupérer facilement
--> Faire une fonction saveVars(nom de fichier, plein de variables) et loadVars(plein de variables) qui stocke tout ça dans un fichier
> Faire des vrais arguments (genre -i, etc)
> Permettre à un processus d'arrêter ou de pauser un processus via une promesse


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
- Changement des plateformes : LINUX_AMD64, WINDOWS_AMD64 et TI_EZ80. Toutes les versions windows n'ont pas de couleurs par défaut

--> Ne pas oublier de mettre à jour la documentation de Neon avec tout ça
*/



strlist ERRORS_MESSAGES;

// VARIABLES GLOBALES SERVANT A DEFINIR LES ELEMENTS MODIFIABLES DE LA SYNTAXE

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


// changes the global environment
void setNeonEnv(NeonEnv* env) {
    global_env = env;
}



void defineVariables(NeonEnv* env)
{
    // définition de la variable de version
    strlist_append(env->NOMS, strdup("__version__"));
    nelist_append(env->ADRESSES, neo_str_create(strdup(VERSION)));

    // définition de la variable de plateforme

    strlist_append(env->NOMS, strdup("__platform__"));
    nelist_append(env->ADRESSES, neo_str_create(strdup(PLATFORM)));

    strlist_append(env->NOMS, strdup("__name__"));
    nelist_append(env->ADRESSES, NEO_VOID);

    env->NAME = env->ADRESSES->len - 1; // l'adresse de __name__ à modifier


    // pi
    nelist_append(env->ADRESSES, neo_double_create(PI));
    strlist_append(env->NOMS, strdup("pi"));
}


void loadFunctions(NeonEnv* env, const Function builtinfunctions[], const char* names[])
{
    for (int i = 0 ; i < NBBUILTINFUNC ; i++)
    {
        Function f = builtinfunctions[i];
        NeObj func = neo_fun_create(f.ptr, f.help, f.nbArgs, f.typeArgs, f.typeRetour);
        strlist_append(env->NOMS, strdup(names[i]));
        nelist_append(env->ADRESSES, func);
    }
    return;
}

// initializes an empty environment
NeonEnv* NeonEnv_init(void) {
    NeonEnv* env = malloc(sizeof(NeonEnv));

    env->CODE_ERROR = 0;
    env->ATOMIC_TIME = 1500;
    env->atomic_counter = 0;
    env->NAME = 0;
    env->EXCEPTION = NULL;
    env->FILENAME = NULL;
    env->LINENUMBER = 0;
    env->RETURN_VALUE = NEO_VOID;
    env->OBJECTS_LIST = NEO_VOID;
    env->CONTAINERS = strlist_create(0);
    env->ATTRIBUTES = nelist_create(0);
    env->PROMISES = nelist_create(0);
    env->PROMISES_CNT = intptrlist_create(0);
    env->PROCESS_FINISH = intlist_create(0);
    env->process_cycle = ProcessCycle_create();
    env->FONCTIONS = tree_create(NULL, 0, 0);
    env->ADRESSES = nelist_create(0);
    env->NOMS = strlist_create(0);

    // the names of the built-in functions
    const char* names[] = {
        "print",
        "input",
        "nbr",
        "str",
        "len",
        "sub",
        "exit",
        "append",
        "remove",
        "insert",
        "type",
        "reverse",
        "eval",
        "clear",
        "help",
        "randint",
        "failwith",
        "time",
        "assert",
        "output",
        "chr",
        "ord",
        "listComp",
        "createException",
        "raise",
        "int",
        "index",
        "replace",
        "count",
        "list",
        "sortAsc",
        "sortDesc",
        "sin",
        "cos",
        "tan",
        "deg",
        "rad",
        "sqrt",
        "ln",
        "exp",
        "log",
        "log2",
        "round",
        "abs",
        "ceil",
        "floor",
        "readFile",
        "writeFile",
        "setFunctionDoc",
        "setAtomicTime",
        "copy",
        "loadNamespace",
        "gc",
        "setColor"
    };

    // built-in functions
    Function builtinfunctions[] = {
        (Function) {
            .ptr = _print_,
            .help = "Displays arguments in the terminal",
            .nbArgs = -1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _input_,
            .help = "Displays the given argument and prompts the user to input in the terminal. Returns the entered value.",
            .nbArgs = -1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_STRING
        },
        (Function) {
            .ptr = _nbr_,
            .help = "Converts a string into a number.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_STRING},
            .typeRetour = TYPE_ANYTYPE
        },
        (Function) {
            .ptr = _str_,
            .help = "Converts any object into a string.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_STRING
        },
        (Function) {
            .ptr = _len_,
            .help = "Returns the length of an object.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_INTEGER
        },
        (Function) {
            .ptr = _substring_,
            .help = "Performs substring extraction. sub(\"hello\", 1, 4) returns \"ell\".",
            .nbArgs = 3,
            .typeArgs = (int[]){TYPE_STRING, TYPE_INTEGER, TYPE_INTEGER},
            .typeRetour = TYPE_STRING
        },
        (Function) {
            .ptr = _exit_,
            .help = "Exits the Neon interpreter.",
            .nbArgs = 0,
            .typeArgs = NULL,
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _append_,
            .help = "Adds an element to a list.",
            .nbArgs = 2,
            .typeArgs = (int[]){TYPE_LIST, TYPE_ANYTYPE},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _remove_,
            .help = "Removes an element from a list.",
            .nbArgs = 2,
            .typeArgs = (int[]){TYPE_LIST, TYPE_INTEGER},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _insert_,
            .help = "Inserts an element into a list.",
            .nbArgs = 3,
            .typeArgs = (int[]){TYPE_LIST, TYPE_ANYTYPE, TYPE_INTEGER},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _type_,
            .help = "Returns the type of an object.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_STRING
        },
        (Function) {
            .ptr = _reverse_,
            .help = "Reverses the characters in a string of characters or reverses a list.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_STRING},
            .typeRetour = TYPE_STRING
        },
        (Function) {
            .ptr = _eval_,
            .help = "Evaluates the given string of characters and returns the result.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_STRING},
            .typeRetour = TYPE_STRING
        },
        (Function) {
            .ptr = _clear_,
            .help = "Clears the terminal.",
            .nbArgs = 0,
            .typeArgs = NULL,
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _help_,
            .help = "This function displays all kinds of information on objects and modules.\nType help() for more information.",
            .nbArgs = -1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _randint_,
            .help = "Returns a random integer between two integers.",
            .nbArgs = 2,
            .typeArgs = (int[]){TYPE_INTEGER, TYPE_INTEGER},
            .typeRetour = TYPE_INTEGER
        },
        (Function) {
            .ptr = _failwith_,
            .help = "Displays the provided text in the terminal and exits the program.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_STRING},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _time_,
            .help = "Returns the time elapsed since 1970 in seconds.",
            .nbArgs = 0,
            .typeArgs = NULL,
            .typeRetour = TYPE_INTEGER
        },
        (Function) {
            .ptr = _assert_,
            .help = "Stops the program if at least 1 assertion has failed.",
            .nbArgs = -1,
            .typeArgs = (int[]){TYPE_BOOL},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _output_,
            .help = "Puts the given arguments on the terminal without spaces or \\n.",
            .nbArgs = -1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _chr_,
            .help = "Returns the ASCII character associated to an integer.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_INTEGER},
            .typeRetour = TYPE_STRING
        },
        (Function) {
            .ptr = _ord_,
            .help = "Returns the ASCII value of the given character.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_STRING},
            .typeRetour = TYPE_INTEGER
        },
        (Function) {
            .ptr = _list_comp_,
            .help = "Arguments : index, start, end, step, condition, value\nEvaluates value for all indices between start and end according to step, and adds it to the list if condition is true.\nindex must be the name of the variable used, and condition and value must be string expressions.",
            .nbArgs = 6,
            .typeArgs = (int[]){TYPE_STRING, TYPE_INTEGER, TYPE_INTEGER, TYPE_INTEGER, TYPE_STRING, TYPE_STRING},
            .typeRetour = TYPE_LIST
        },
        (Function) {
            .ptr = _create_exception_,
            .help = "Creates an Exception object with the given name.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_STRING},
            .typeRetour = TYPE_EXCEPTION
        },
        (Function) {
            .ptr = _raise_,
            .help = "Exits the program with the given exception and displays the given message",
            .nbArgs = 2,
            .typeArgs = (int[]){TYPE_EXCEPTION, TYPE_STRING},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _int_,
            .help = "Converts an object into an integer",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_INTEGER
        },
        (Function) {
            .ptr = _index_,
            .help = "Returns the index of an element in a list.",
            .nbArgs = 2,
            .typeArgs = (int[]){TYPE_LIST, TYPE_ANYTYPE},
            .typeRetour = TYPE_INTEGER
        },
        (Function) {
            .ptr = _replace_,
            .help = "replace(str, str1, str2) returns a string in which all occurrences of str1 have been replaced by str2 in str.",
            .nbArgs = 3,
            .typeArgs = (int[]){TYPE_STRING, TYPE_STRING, TYPE_STRING},
            .typeRetour = TYPE_STRING
        },
        (Function) {
            .ptr = _count_,
            .help = "Counts the number of occurrences in a list or a string.",
            .nbArgs = 2,
            .typeArgs = (int[]){TYPE_ANYTYPE, TYPE_ANYTYPE},
            .typeRetour = TYPE_INTEGER
        },
        (Function) {
            .ptr = _list_,
            .help = "Separates every character of a string in a list.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_STRING},
            .typeRetour = TYPE_LIST
        },
        (Function) {
            .ptr = _sort_asc_,
            .help = "Sorts a list in ascending order.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_LIST},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _sort_desc_,
            .help = "Sorts a list in descending order.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_LIST},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _sin_,
            .help = "sin: Sine function",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _cos_,
            .help = "cos: Cosine function",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _tan_,
            .help = "tan: Tangent function",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _deg_,
            .help = "deg: Convert angle from radians to degrees",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _rad_,
            .help = "rad: Convert angle from degrees to radians",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _sqrt_,
            .help = "sqrt: Square root function",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _ln_,
            .help = "ln: Natural logarithm (base e)",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _exp_,
            .help = "exp: Exponential function (e^x)",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _log_,
            .help = "log: Logarithm (base 10)",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _log2_,
            .help = "log2: Logarithm (base 2)",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _round_,
            .help = "round: Round to the nearest decimal number with the given precision",
            .nbArgs = 2,
            .typeArgs = (int[]){TYPE_DOUBLE, TYPE_DOUBLE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _abs_,
            .help = "abs: Absolute value",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_DOUBLE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _ceil_,
            .help = "ceil: Ceiling function (smallest integer greater than or equal to)",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_DOUBLE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _floor_,
            .help = "floor: Floor function (largest integer less than or equal to)",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_DOUBLE},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _readFile_,
            .help = "Returns the content of the file whose name was given",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_STRING},
            .typeRetour = TYPE_STRING
        },
        (Function) {
            .ptr = _writeFile_,
            .help = "Writes the given string in the file whose name was given. The syntax is writeFile(name, content)",
            .nbArgs = 2,
            .typeArgs = (int[]){TYPE_STRING, TYPE_STRING},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _setFunctionDoc_,
            .help = "Sets a string documentation for a user-defined function or method",
            .nbArgs = 2,
            .typeArgs = (int[]){TYPE_ANYTYPE, TYPE_STRING},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _setAtomicTime_,
            .help = "Time to allow for each process before switching",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_INTEGER},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _copy_,
            .help = "Performs a deep copy of an object, preserving the pointer dependencies",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_ANYTYPE},
            .typeRetour = TYPE_ANYTYPE
        },
        (Function) {
            .ptr = _load_namespace_,
            .help = "Loads any variable from a given namespace without the namespace prefix",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_STRING},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _gc_,
            .help = "Calls the Garbage Collector",
            .nbArgs = 0,
            .typeArgs = NULL,
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _setColor_,
            .help = "Changes the writing text color in console if available.\nColors: red, green, blue and white",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_STRING},
            .typeRetour = TYPE_NONE
        }
    };

    loadFunctions(env, builtinfunctions, names);

    defineVariables(env);
    
    return env;
}



void NeonEnv_destroy(NeonEnv* env) {
    free(env->process_cycle);

    neobject_destroy(env->RETURN_VALUE);

    tree_destroy(env->FONCTIONS);

    strlist_destroy(env->CONTAINERS, true);
    nelist_destroy(env->ATTRIBUTES);

    nelist_destroy(env->ADRESSES);
    strlist_destroy(env->NOMS, true);

    // ramassage de miettes (garbage collection)
    gc_final_sweep();

    nelist_destroy(env->PROMISES);
    intptrlist_destroy(&env->PROMISES_CNT);
    free(env->PROCESS_FINISH.tab);

    if (env->FILENAME != NULL)
        free(env->FILENAME);

    free(env);
}









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










// cette fonction permet de réutiliser le résultat d'une expression grâce à la variable Ans dans le terminal
void storeAns(NeObj res) {
    if (strlist_inList(global_env->NOMS, "Ans")) {
        int index = strlist_index(global_env->NOMS, "Ans");
        neobject_destroy(global_env->ADRESSES->tab[index]); // supprime totalement le précédent
        global_env->ADRESSES->tab[index] = res; // met le nouveau dedans
    }
    else {
        strlist_append(global_env->NOMS, strdup("Ans"));
        nelist_append(global_env->ADRESSES, res);
    }
}






void terminal(void)
{
    char* exp;
    NeObj res ; Tree* tree;
        
        while (true)
        {
            global_env->CODE_ERROR = 0; // réinitialise les erreurs

            if (global_env->FILENAME != NULL)
                free(global_env->FILENAME);
            global_env->FILENAME = NULL;

            exp = inputCode(SEQUENCE_ENTREE);

            if (exp == NULL && global_env->CODE_ERROR == 0) // pour afficher le keyboardInterrupt
                global_env->CODE_ERROR = 104;
            
            // quand l'utilisateur appuie sur CTRL-D, ça met CODE_ERROR à 1
            if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0)
            {
                printError(global_env->CODE_ERROR);
                global_env->CODE_ERROR = 0;
                continue;
            }
            else if (global_env->CODE_ERROR == 1)
                return;



            if (strcmp(exp,"")==0) // si l'utilisateur n'a rien ecrit
            {
                free(exp);
                continue;
            }

            
            
            tree = tree_create(NULL, 0, 0);

            if (global_env->CODE_ERROR != 0)
            {
                printError(global_env->CODE_ERROR);
                free(exp);
                continue;
            }


            createSyntaxTree(tree, exp);

            free(exp);

            if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0)
            {
                printError(global_env->CODE_ERROR);
                tree_destroy(tree);
                continue;
            }

            
            // s'il n'y a qu'une expression, alors, on affiche le résultat de l'expression
            if (tree->nbSons == 1 && tree->sons[0]->type != TYPE_TRYEXCEPT && tree->sons[0]->type != TYPE_CONDITIONBLOCK && tree->sons[0]->type != TYPE_STATEMENTFOR && tree->sons[0]->type != TYPE_STATEMENTFOREACH && tree->sons[0]->type != TYPE_STATEMENTWHILE && tree->sons[0]->type != TYPE_KEYWORD && tree->sons[0]->type != TYPE_FUNCTIONDEF && tree->sons[0]->type != TYPE_BLOCKWORD1LINE && tree->sons[0]->type != TYPE_ATOMICBLOCK)
            {
                res = eval(tree->sons[0]);

                if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0)
                {
                    printError(global_env->CODE_ERROR);
                    tree_destroy(tree);
                    continue;
                }
                else if (global_env->CODE_ERROR == 1) // quitte le terminal
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
                if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0)
                {
                    printError(global_env->CODE_ERROR);
                    tree_destroy(tree);
                    continue;
                }
                else if (global_env->CODE_ERROR == 1) // quitte le terminal
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
    
    if (global_env->CODE_ERROR != 0)
        printError(global_env->CODE_ERROR);
    global_env->CODE_ERROR = 0;

    if (program == NULL)
        return ;
    
    //printf("Création de l'arbre syntaxique\n");

    global_env->FILENAME = strdup(filename);
    
    // exécution du fichier
    Tree* tree = tree_create(NULL, 0, 0);
    createSyntaxTree(tree, program);
    
    free(program);

    //printf("Arbre syntaxique terminé\n");

    if (global_env->CODE_ERROR != 0)
    {
        printError(global_env->CODE_ERROR);
        global_env->CODE_ERROR = 0;
        tree_destroy(tree);
        return;
    }

    exec(tree);
    
    if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0)
    {
        printError(global_env->CODE_ERROR);
    }

    tree_destroy(tree);

    return ;
}








void importFile(char* filename)
{
    char* program = openFile(filename); // fonction dépendant du système cible
    
    if (global_env->CODE_ERROR != 0)
        return;

    if (program == NULL) {
        global_env->CODE_ERROR = 67;
        return ;
    }

    char* sov = global_env->FILENAME;
    global_env->FILENAME = strdup(filename);
    
    // exécution du fichier
    Tree* tree = tree_create(NULL, 0, 0);
    createSyntaxTree(tree, program);
    free(program);

    if (global_env->CODE_ERROR != 0)
    {
        free(sov);
        tree_destroy(tree);
        return;
    }

    exec_aux(tree);

    if (global_env->CODE_ERROR != 0) {
        free(sov);
        tree_destroy(tree);
        return;
    }

    free(global_env->FILENAME);
    global_env->FILENAME = sov;

    tree_destroy(tree);
    return ;
}









void neonInit(void)
{
    #ifdef LINUX_AMD64
        linenoiseSetMultiLine(1); // spécial pour linenoise
        signal(SIGINT, handle_signal);
        signal(SIGTERM, handle_signal);
    #endif
    
    #ifdef WINDOWS_AMD64
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

    srand(time(NULL));

    return;
}


void neonExit(void)
{
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
    strlist_destroy(&ERRORS_MESSAGES, false);
}

