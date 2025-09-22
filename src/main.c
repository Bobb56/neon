#include <string.h>

#include "headers/neon.h"
#include "headers/runtime.h"


#ifdef TI_EZ80
#include "headers/neonio.h"
#include "extern/nio_ce.h"
nio_console console;
#endif


/*
CHOSES SPÉCIFIQUES À L'ARCHITECTURE :
> lowlevel.asm
> set_stack_pointer

Liste des choses qui marchent pas :
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
- Fonction initGraphics, fonction draw, nouveaux containers
- Amélioration de la fonction help avec les containers

*/





#ifdef TI_EZ80
    int main(void)
#else
    int main (int argc, char* argv[])
#endif
{
    neonInit();

    // définition de la liste des arguments
    NeObj l = neo_list_create(0);
    update__name__(strdup("__main__")); // nom du fichier actuel

    #ifdef TI_EZ80
        // récupération du nom de fichier si existant
        nio_init(&console, NIO_MAX_COLS, NIO_MAX_ROWS, 0, 0, NIO_COLOR_WHITE, NIO_COLOR_BLACK, true);
        nio_set_default(&console);

        char* filename = argsAns();

        variable_append(global_env, "__args__", l);

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

        variable_append(global_env, "__args__", l);

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
