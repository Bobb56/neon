#define NEON_SOURCE_ID 11

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "headers/constants.h"
#include "headers/neonio.h"
#include "headers/objects.h"
#include "headers/nativefunctions.h"
#include "headers/dynarrays.h"
#include "headers/gc.h"
#include "headers/errors.h"
#include "headers/runtime.h"
#include "headers/strings.h"
#include "headers/syntaxtrees.h"
#include "headers/neon.h"
#include "headers/processcycle.h"
#include "headers/trees.h"
#include "headers/sidememory.h"
#include "headers/errors.h"

#ifdef TI_EZ80
#include "headers/graphicmodule.h"
#include <graphx.h>
#endif


// Variables d'environnement de Neon
NeonEnv* global_env = NULL;


#ifdef LINUX
    #include <signal.h>

    void handle_signal(int sig) {
        if (sig == SIGINT || sig == SIGTERM) {
            neon_fail(104, NO_ARGS);
        }
    }
#endif


#ifdef WINDOWS_AMD64
    #include <windows.h>

    // Fonction de gestion du signal
    BOOL WINAPI ctrlHandler(DWORD signal) {
        if (signal == CTRL_C_EVENT) {
            neon_fail(104, NO_ARGS);
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
    variable_append(env, "__name__", neo_str_create(strdup("__main__")));

    env->NAME = env->ADRESSES->len - 1; // l'adresse de __name__ à modifier
}



void loadFunctions(NeonEnv* env) {
    init_module(StandardModule, env);
}


void loadExceptions(NeonEnv* env) {
    const char* exceptions[] = {
        "SyntaxError",
        "FileSystemError",
        "UnmeasurableObject",
        "UndefinedObject",
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
        "ExitSignal",
        "InternalError",
        "DeserializationError",
        "Error"
    };

    for (int i = 0 ; i < 20 ; i++) {
        strlist_append(env->EXCEPTIONS, strdup(exceptions[i]));
    }
}



size_t NeonEnv_get_size(NeonEnv* env) {
    size_t size = 0;
    size += sizeof(NeonEnv);
    size += strlist_getsize(env->NOMS);
    size += nelist_getsize(env->PROMISES);
    size += nelist_getsize(env->ADRESSES);
    size += nelist_getsize(env->ATTRIBUTES);
    size += strlist_getsize(env->EXCEPTIONS);
    size += strlist_getsize(env->CONTAINERS);
    size += intlist_getsize(env->PROCESS_FINISH);
    return size;
}




// initializes an empty environment
NeonEnv* NeonEnv_init(void) {
    NeonEnv* env = neon_malloc(sizeof(NeonEnv));
    if (env == NULL) {
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
    env->TREEBUFFERS = ptrlist_create();
    env->ERROR_MESSAGE_ARGUMENTS = NULL;
    env->PROMISES = nelist_create(0);
    env->ADRESSES = nelist_create(0);
    env->ATTRIBUTES = nelist_create(0);
    env->EXCEPTIONS = strlist_create(0);
    env->CONTAINERS = strlist_create(0);
    env->process_cycle = ProcessCycle_create();
    env->PROCESS_FINISH = intlist_create(0);
    env->PROMISES_CNT = intptrlist_create(0);
    env->ATOMIC_TIME = PLATFORM_SPECIFIC_ATOMIC_TIME;

    #ifdef LINUX
    env->history = strlist_create(0);
    #endif

    // if (!env->NOMS || !env->TREEBUFFERS || !env->PROMISES || !env->ADRESSES || !env->ATTRIBUTES || !env->EXCEPTIONS || !env->CONTAINERS || !env->process_cycle || !env->PROCESS_FINISH.tab || !env->PROMISES_CNT.tab) {
    //     return NULL;
    // }


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
    
    neon_reset_error();

    neon_free(env->process_cycle);

    strlist_destroy(env->EXCEPTIONS, true);

    neobject_destroy(env->RETURN_VALUE);

    TreeBuffer_delete_all(env->TREEBUFFERS);
    ptrlist_destroy(env->TREEBUFFERS, true, true);

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

    #ifdef LINUX
    strlist_destroy(env->history, true);
    #endif
    
    #ifdef TI_EZ80
    nio_free(&global_env->console);
    #endif

    neon_free(env);
}




int neonInit(void)
{
    srand(time(NULL));
    

    #ifdef LINUX
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

    // Initialisation de l'allocateur secondaire
    side_memory_init();

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

    // Nettoyage de l'allocateur secondaire
    side_memory_exit();
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
    setColor(GREEN); printString("https://neon.raphaael.fr"); setColor(DEFAULT);
    printString(" for more information.");
    newLine();

    #ifdef EXPERIMENTAL
        setColor(RED);
        printString("/!\\ THIS IS AN EXPERIMENTAL VERSION. IT MAY NOT WORK. /!\\");
        setColor(DEFAULT);
        newLine();newLine();
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






void terminal(void) {
    char* exp;
    NeObj res ;
    TreeBuffer tb;
        
    while (true) {
        neon_reset_error(); // réinitialise les erreurs

        if (global_env->FILENAME != NULL)
            neon_free(global_env->FILENAME);
        global_env->FILENAME = NULL;

        exp = inputCode();

        if (exp == NULL && global_env->CODE_ERROR == 0) // pour afficher le keyboardInterrupt
            neon_fail(104, NO_ARGS);
        
        // quand l'utilisateur appuie sur CTRL-D, ça met CODE_ERROR à 1
        if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0) {
            printError();
            continue;
        }
        else if (global_env->CODE_ERROR == 1)
            return;


        if (strcmp(exp,"") == 0) { // si l'utilisateur n'a rien ecrit
            neon_free(exp);
            continue;
        }
        
        
        if_error {
            printError();
            neon_free(exp);
            continue;
        }

        tb = createSyntaxTree(exp, true);

        if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0) {
            printError();
            continue;
        }

        
        // s'il n'y a qu'une expression, alors, on affiche le résultat de l'expression
        if (treelistLength(&tb, treeSntxTree(&tb, tb.entry_point)->treelist) == 1 && NeTree_isexpr(&tb, treelistGet(&tb, treeSntxTree(&tb, tb.entry_point)->treelist)[0]))
        {
            TreeBufferIndex exprtree = treelistGet(&tb, treeSntxTree(&tb, tb.entry_point)->treelist)[0];

            res = eval(&tb, exprtree);

            if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0)
            {
                printError();
                TreeBuffer_destroy(&tb);
                continue;
            }
            else if (global_env->CODE_ERROR == 1) // quitte le terminal
            {
                TreeBuffer_destroy(&tb);
                return ;
            }
            
            printRes(res);

            storeAns(res); // stocke le résultat dans une variable au lieu de le supprimer
        }
        else if (treelistLength(&tb, treeSntxTree(&tb, tb.entry_point)->treelist) > 0)
        {
            exec(&tb, tb.entry_point);
            if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0)
            {
                printError();
                TreeBuffer_destroy(&tb);
                continue;
            }
            else if (global_env->CODE_ERROR == 1) // quitte le terminal
            {
                TreeBuffer_destroy(&tb);
                return ;
            }
        }
        TreeBuffer_destroy(&tb);
    }
    return ;
}




void execFile(char* filename) {
    char* program = openFile(filename); // fonction dépendant du système cible
    
    if_error {
        goto handle_error;
    }

    global_env->FILENAME = strdup(filename);

    TreeBuffer tb = createSyntaxTree(program, true);
    
    if_error {
        goto handle_error;
    }

    tb_exec(&tb);
    
    if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0) {
        TreeBuffer_destroy(&tb);
        goto handle_error;
    }

    TreeBuffer_destroy(&tb);
    return ;

handle_error:
    printError();
    neon_pause("Press ENTER to leave Neon...");
    return;
}





void importFile(char* filename)
{
    char* program = openFile(filename); // fonction dépendant du système cible
    
    return_on_error();

    if (program == NULL) {
        neon_fail(67, neo_new_str_create(filename));
        return ;
    }

    char* sov = global_env->FILENAME;
    global_env->FILENAME = strdup(filename);
    
    // exécution du fichier
    TreeBuffer tb = createSyntaxTree(program, true);

    if (global_env->CODE_ERROR != 0)
    {
        neon_free(sov);
        return;
    }

    tb_exec_aux(&tb);

    if (global_env->CODE_ERROR != 0) {
        neon_free(sov);
        TreeBuffer_destroy(&tb);
        return;
    }

    neon_free(global_env->FILENAME);
    global_env->FILENAME = sov;

    TreeBuffer_destroy(&tb);
    return;
}
