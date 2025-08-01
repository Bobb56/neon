#include <string.h>

#include "headers/neon.h"
#include "headers/runtime.h"


#ifdef TI_EZ80
    int main(void)
#else
    int main (int argc, char* argv[])
#endif
{
    neonInit();

    // définition de la liste des arguments
    NeObj l = neo_list_create(0);
    updateFileName(strdup("__main__")); // nom du fichier actuel

    #ifdef TI_EZ80
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
        // ajout des arguments dans le tableau contenant les arguments du programme
        for (int i = 2 ; i < argc ; i++)
            neo_list_append(l,neo_str_create(strdup(argv[i])));

        nelist_append(global_env->ADRESSES,l);
        strlist_append(global_env->NOMS,strdup("__args__"));

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


    #ifdef TI_EZ80
        nio_free(&console);
    #endif

    return 0;
}
