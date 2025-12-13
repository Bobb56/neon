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
#include "extern/linenoise.h"
#include "headers/errors.h"
#include "headers/runtime.h"
#include "headers/strings.h"
#include "headers/syntaxtrees.h"
#include "headers/neon.h"
#include "headers/processcycle.h"
#include "headers/trees.h"

#ifdef TI_EZ80
#include "headers/graphics.h"
#include <graphx.h>
#endif


// Variables d'environnement de Neon
NeonEnv* global_env = NULL;


#ifdef LINUX
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



// changes the global environment
void setNeonEnv(NeonEnv* env) {
    global_env = env;
}



void defineVariables(NeonEnv* env)
{
    // définition de la variable de version
    variable_append(env, "__version__", neo_str_create(strdup(VERSION)));

    // définition de la variable de plateforme
    variable_append(env, "__platform__", neo_str_create(strdup(PLATFORM)));

    // le nom du fichier
    variable_append(env, "__name__", NEO_VOID);

    env->NAME = env->ADRESSES->len - 1; // l'adresse de __name__ à modifier
}


void loadFunctions(NeonEnv* env)
{
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
        "setColor",
        "initGraphics",
        "detectFiles",
        "safeExec"
    };

    // built-in functions
    Function builtinfunctions[] = {
        (Function) {
            .ptr = _print_,
            .help = "Displays arguments in the terminal",
            .nbArgs = -1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _input_,
            .help = "Displays the given argument and prompts the user to input in the terminal. Returns the entered value.",
            .nbArgs = -1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_STRING
        },
        (Function) {
            .ptr = _nbr_,
            .help = "Converts a string into a number.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_STRING},
            .typeRetour = TYPE_UNSPECIFIED
        },
        (Function) {
            .ptr = _str_,
            .help = "Converts any object into a string.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_STRING
        },
        (Function) {
            .ptr = _len_,
            .help = "Returns the length of an object.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
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
            .typeArgs = (int[]){TYPE_LIST, TYPE_UNSPECIFIED},
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
            .typeArgs = (int[]){TYPE_LIST, TYPE_UNSPECIFIED, TYPE_INTEGER},
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _type_,
            .help = "Returns the type of an object.",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_STRING
        },
        (Function) {
            .ptr = _reverse_,
            .help = "Reverses the characters in a string or reverses a list. Returns the reversed object without modifying the original one",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_UNSPECIFIED
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
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
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
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
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
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_INTEGER
        },
        (Function) {
            .ptr = _index_,
            .help = "Returns the index of an element in a list.",
            .nbArgs = 2,
            .typeArgs = (int[]){TYPE_LIST, TYPE_UNSPECIFIED},
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
            .typeArgs = (int[]){TYPE_UNSPECIFIED, TYPE_UNSPECIFIED},
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
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _cos_,
            .help = "cos: Cosine function",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _tan_,
            .help = "tan: Tangent function",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _deg_,
            .help = "deg: Convert angle from radians to degrees",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _rad_,
            .help = "rad: Convert angle from degrees to radians",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _sqrt_,
            .help = "sqrt: Square root function",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _ln_,
            .help = "ln: Natural logarithm (base e)",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _exp_,
            .help = "exp: Exponential function (e^x)",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _log_,
            .help = "log: Logarithm (base 10)",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _log2_,
            .help = "log2: Logarithm (base 2)",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_DOUBLE
        },
        (Function) {
            .ptr = _round_,
            .help = "round: Round to the nearest real number with the given precision",
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
            .typeArgs = (int[]){TYPE_UNSPECIFIED, TYPE_STRING},
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
            .typeArgs = (int[]){TYPE_UNSPECIFIED},
            .typeRetour = TYPE_UNSPECIFIED
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
        },
        (Function) {
            .ptr = _initGraphics_,
            .help = "Initializes graphic and keyboard library",
            .nbArgs = 0,
            .typeArgs = NULL,
            .typeRetour = TYPE_NONE
        },
        (Function) {
            .ptr = _detectFiles_,
            .help = "Returns a list of all AppVars starting by a specific string",
            .nbArgs = 1,
            .typeArgs = (int[]){TYPE_STRING},
            .typeRetour = TYPE_LIST
        },
        (Function) {
            .ptr = _safeExec_,
            .help = "Executes a Neon file with the given arguments in a separate environment",
            .nbArgs = 2,
            .typeArgs = (int[]){TYPE_STRING, TYPE_LIST},
            .typeRetour = TYPE_NONE
        }
    };


    for (int i = 0 ; i < NBBUILTINFUNC ; i++)
    {
        Function f = builtinfunctions[i];
        NeObj func = neo_fun_create(f.ptr, f.help, f.nbArgs, f.typeArgs, f.typeRetour);
        strlist_append(env->NOMS, strdup(names[i]));
        nelist_append(env->ADRESSES, func);
    }
    return;
}



void loadExceptions(NeonEnv* env) {
    const char* exceptions[] = {
        "SyntaxError",
        "FileSystemError",
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
        "KeyboardInterrupt",
        "NotImplemented",
        "ExitSignal"
    };

    for (int i = 0 ; i < 17 ; i++) {
        strlist_append(env->EXCEPTIONS, strdup(exceptions[i]));
    }
}



// initializes an empty environment
NeonEnv* NeonEnv_init(void) {
    NeonEnv* env = neon_malloc(sizeof(NeonEnv));

    int error = TreeBuffer_init(&env->FONCTIONS);

    if (error < 0) {
        free(env);
        return NULL;
    }

    env->NAME = 0;
    env->CODE_ERROR = 0;
    env->LINENUMBER = 0;
    env->FILENAME = NULL;
    env->EXCEPTION = NULL;
    env->atomic_counter = 0;
    env->RETURN_VALUE = NEO_VOID;
    env->OBJECTS_LIST = NEO_VOID;
    env->NOMS = strlist_create(0);
    env->PROMISES = nelist_create(0);
    env->ADRESSES = nelist_create(0);
    env->ATTRIBUTES = nelist_create(0);
    env->EXCEPTIONS = strlist_create(0);
    env->CONTAINERS = strlist_create(0);
    env->process_cycle = ProcessCycle_create();
    env->PROCESS_FINISH = intlist_create(0);
    env->PROMISES_CNT = intptrlist_create(0);
    env->ATOMIC_TIME = PLATFORM_SPECIFIC_ATOMIC_TIME;


    loadFunctions(env);
    loadExceptions(env);

    defineVariables(env);

    #ifdef TI_EZ80
    nio_init(&env->console, NIO_MAX_COLS, NIO_MAX_ROWS, 0, 0, NEON_PALETTE_WHITE, NEON_PALETTE_BLACK, true);
    env->text_transparent_color = 255;
    #endif
    
    return env;
}

NeonEnv* NeonEnv_set(NeonEnv* new_env) {
    NeonEnv* global_env_sov = global_env;
    global_env = new_env;
    #ifdef TI_EZ80
    nio_set_default(&global_env->console);
    gfx_SetTextTransparentColor(global_env->text_transparent_color);
    #endif
    return global_env_sov;
}



void NeonEnv_destroy(NeonEnv* env) {
    neon_free(env->process_cycle);

    strlist_destroy(env->EXCEPTIONS, true);

    neobject_destroy(env->RETURN_VALUE);

    TreeBuffer_destroy(&env->FONCTIONS, TREE_VOID);

    strlist_destroy(env->CONTAINERS, true);
    gc_extern_nelist_destroy(env->ATTRIBUTES);

    nelist_destroy(env->ADRESSES);
    strlist_destroy(env->NOMS, true);

    // ramassage de miettes (garbage collection)
    gc_final_sweep();

    nelist_destroy(env->PROMISES);
    intptrlist_destroy(&env->PROMISES_CNT);
    neon_free(env->PROCESS_FINISH.tab);

    if (env->FILENAME != NULL)
        neon_free(env->FILENAME);
    
    #ifdef TI_EZ80
    nio_free(&global_env->console);
    #endif

    neon_free(env);
}




int neonInit(void)
{
    srand(time(NULL));
    

    #ifdef LINUX
        linenoiseSetMultiLine(1); // spécial pour linenoise
        signal(SIGINT, handle_signal);
        signal(SIGTERM, handle_signal);
    #endif
    
    #ifdef WINDOWS
        SetConsoleCtrlHandler(ctrlHandler, TRUE);
    #endif

    #ifdef TI_EZ80
        kb_EnableOnLatch();
        gfx_Begin();
        set_neon_palette();
    #endif

    NeonEnv* env = NeonEnv_init();

    if (env == NULL)
        return -1;

    NeonEnv_set(env);
    return 0;
}


void neonExit(void)
{
    NeonEnv_destroy(global_env);
    #ifdef TI_EZ80
    gfx_End();
    kb_DisableOnLatch();
    #endif
    return;
}








void printRes(NeObj res)
{
    if (NEO_TYPE(res) != TYPE_NONE)
    {
        printString(" (");
        setColor(GREEN);
        printString(type(res));
        setColor(DEFAULT);
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
    setColor(DEFAULT);

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
    setColor(GREEN); printString("https://langage-neon.raphaael.fr"); setColor(DEFAULT);
    printString(" for more information.");
    newLine();

    #ifdef EXPERIMENTAL
        newLine();
        setColor(RED);
        printString("/!\\ THIS IS AN EXPERIMENTAL VERSION. IT MAY NOT WORK. /!\\");
        newLine();newLine();
        setColor(DEFAULT);
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
    NeObj res ;
    TreeBuffer tb;
    TreeBufferIndex tree;
        
    while (true) {
        global_env->CODE_ERROR = 0; // réinitialise les erreurs

        if (global_env->FILENAME != NULL)
            neon_free(global_env->FILENAME);
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


        if (strcmp(exp,"") == 0) // si l'utilisateur n'a rien ecrit
        {
            neon_free(exp);
            continue;
        }
        
        
        if (global_env->CODE_ERROR != 0)
        {
            printError(global_env->CODE_ERROR);
            neon_free(exp);
            continue;
        }

        TreeBuffer_init(&tb);
        tree = createSyntaxTree(&tb, exp, true);

        if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0)
        {
            printError(global_env->CODE_ERROR);
            TreeBuffer_destroy(&tb, tree);
            continue;
        }

        
        // s'il n'y a qu'une expression, alors, on affiche le résultat de l'expression
        if (treeSntxTree(&tb, tree)->treelist.length == 1 && NeTree_isexpr(&tb, treelistGet(&tb, treeSntxTree(&tb, tree)->treelist)[0]))
        {
            res = eval(&tb, treelistGet(&tb, treeSntxTree(&tb, tree)->treelist)[0]);

            if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0)
            {
                printError(global_env->CODE_ERROR);
                TreeBuffer_destroy(&tb, tree);
                continue;
            }
            else if (global_env->CODE_ERROR == 1) // quitte le terminal
            {
                TreeBuffer_destroy(&tb, tree);
                return ;
            }
            
            printRes(res);

            storeAns(res); // stocke le résultat dans une variable au lieu de le supprimer
        }
        else if (treeSntxTree(&tb, tree)->treelist.length > 0)
        {
            exec(&tb, tree);
            if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0)
            {
                printError(global_env->CODE_ERROR);
                TreeBuffer_destroy(&tb, tree);
                continue;
            }
            else if (global_env->CODE_ERROR == 1) // quitte le terminal
            {
                TreeBuffer_destroy(&tb, tree);
                return ;
            }
        }
        TreeBuffer_destroy(&tb, tree);
    }
    return ;
}





void execFile(char* filename)
{
    
    char* program = openFile(filename); // fonction dépendant du système cible
    
    if_error {
        goto handle_error;
    }

    global_env->FILENAME = strdup(filename);
    
    TreeBuffer tb;
    TreeBuffer_init(&tb);

    TreeBufferIndex tree = createSyntaxTree(&tb, program, true);
    
    if_error {
        TreeBuffer_destroy(&tb, tree);
        goto handle_error;
    }

    exec(&tb, tree);
    
    if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0) {
        TreeBuffer_destroy(&tb, tree);
        goto handle_error;
    }

    TreeBuffer_destroy(&tb, tree);
    return ;

handle_error:
    printError(global_env->CODE_ERROR);
    neon_pause("Press ENTER to leave Neon...");
    return;
}








void importFile(char* filename)
{
    char* program = openFile(filename); // fonction dépendant du système cible
    
    return_on_error();

    if (program == NULL) {
        global_env->CODE_ERROR = 67;
        return ;
    }

    char* sov = global_env->FILENAME;
    global_env->FILENAME = strdup(filename);
    
    // exécution du fichier
    TreeBuffer tb;
    TreeBuffer_init(&tb);

    TreeBufferIndex tree = createSyntaxTree(&tb, program, true);

    if (global_env->CODE_ERROR != 0)
    {
        neon_free(sov);
        TreeBuffer_destroy(&tb, tree);
        return;
    }

    exec_aux(&tb, tree);

    if (global_env->CODE_ERROR != 0) {
        neon_free(sov);
        TreeBuffer_destroy(&tb, tree);
        return;
    }

    neon_free(global_env->FILENAME);
    global_env->FILENAME = sov;

    TreeBuffer_destroy(&tb, tree);
    return ;
}
