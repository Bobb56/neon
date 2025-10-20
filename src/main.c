#include <string.h>

#include "headers/neon.h"
#include "headers/runtime.h"
#include "headers/errors.h"
#include "headers/neonio.h"


/*
CHOSES SPÉCIFIQUES À L'ARCHITECTURE :
> lowlevel.asm
> set_stack_pointer

Liste des choses qui marchent pas :
_____________________

Potentielles futures erreurs :
------------------------------
S'il y a des problèmes liés aux piles et tout, bien vérifier les tailles des contextes des fonctions et regarder si on gère ça correctement

Peut être qu'il y a des endroits où on oublie de mettre à jour les promesses

General Protection Fault => Problème d'alignement de la pile


Avancement et choses à faire :
-------------------------------
> Alléger le code pour être en dessous des 64k en zx7
> Mettre à jour la doc, NEON\x00
> Arrêt d'un programme en appuyant sur ON sur calculatrice
> Ajouter une sorte de JSON intégrée et sécurisée (stocké en binaire) pour stocker plein d'infos et les récupérer facilement
--> Faire une fonction saveVars(nom de fichier, plein de variables) et loadVars(plein de variables) qui stocke tout ça dans un fichier
> Faire des vrais arguments (genre -i, etc)
> Permettre à un processus d'arrêter ou de pauser un processus via une promesse


NOUVEAUTéS depuis la dernière publication :
-------------------------------------------
- Fonction initGraphics, fonction draw, nouveaux containers, setPixel, getPixel, rgb, getTextWidth
- Amélioration de la fonction help avec les containers
- Fonction detectFiles



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
            neon_free(filename);
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

    return 0;
}
