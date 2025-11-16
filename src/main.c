#include <string.h>

#include "headers/constants.h"
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
> Changer le mode d'allocation de la pile (StackBuffer)
> Modifier le mode de fonctionnement des TreeBuffer et StackBuffer en utilisant des fichiers en RAM sur TI_EZ80
> Ajouter une fonction menu pour faciliter le développement de shells propres et optimisés
(fonction menu exactement comme Neon Launcher actuel)

PLUS TARD
> Faire une nouvelle console pour TI_EZ80
> Ajouter une fonction menu qui prend en entrée une liste de listes d'objets MenuItems(id, inactive, hover), un objet pour le background et qui renvoie l'id de l'élément sélectionné ou None si l'utilisateur a quitté
Les touches supportées sont [<,>,v,^,Enter,Clear]
> Ajouter une sorte de JSON intégrée et sécurisée (stocké en binaire) pour stocker plein d'infos et les récupérer facilement
--> Faire une fonction saveVar(nom_fichier, variable) et loadVar(nom_fichier) qui stocke tout ça dans un fichier
> Faire des vrais arguments (genre -i, etc)
> Permettre à un processus d'arrêter ou de pauser un processus via une promesse


NOUVEAUTéS depuis la dernière publication :
-------------------------------------------
- Fonction initGraphics, fonction draw, nouveaux containers, setPixel, getPixel, rgb, getTextWidth
- Amélioration de la fonction help avec les containers
- Fonction detectFiles
- Arrêt d'un programme en appuyant sur ON sur calculatrice
- Lancement automatique de __launcher__.ne ou LAUNCHER si fichier présent au lancement de l'interpréteur
- Fonction safeExec qui exécute un programme Neon dans un nouvel environnement
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
        variable_append(global_env, "__args__", l);

        char* filename = argsAns();
        if (filename != NULL) {
            execFile(filename);
            neon_free(filename);
        }
        else if (launcher(LAUNCHER_NAME)) {
            execFile(LAUNCHER_NAME);
        }
        else {
            startMessage();
            terminal();
        }
    #else
        // ajout des arguments dans le tableau contenant les arguments du programme
        for (int i = 2 ; i < argc ; i++)
            neo_list_append(l, neo_str_create(strdup(argv[i])));

        variable_append(global_env, "__args__", l);

        if (argc >= 2) {
            execFile(argv[1]);
        }
        else if (launcher(LAUNCHER_NAME)) {
            execFile(LAUNCHER_NAME);
        }
        else {
            startMessage();
            terminal();
        }
    #endif

    neonExit();
    return 0;
}
