#include <string.h>

#include "headers/neon.h"
#include "headers/runtime.h"




/*
CHOSES SPÉCIFIQUES À L'ARCHITECTURE :
> lowlevel.asm
> set_stack_pointer

Liste des choses qui marchent pas :
Définition de fonctions dans le terminal
Affichage de l'infini et des nan

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
        nio_init(&console, NIO_MAX_COLS, NIO_MAX_ROWS, 0, 0, NIO_COLOR_BLACK, NIO_COLOR_WHITE, true);
        nio_set_default(&console);

        char* filename = argsAns();

        variable_append("__args__", l);

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

        variable_append("__args__", l);

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
