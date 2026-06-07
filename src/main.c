#define NEON_SOURCE_ID 10

#include <string.h>

#include "headers/constants.h"
#include "headers/neon.h"
#include "headers/neonio.h"

#ifdef TI_EZ80
#include "headers/errors.h"
#endif

/*
Table des fichiers source :
---------------------------
1  | standardmodule.c
2  | contexts.c
3  | dynarrays.c
4  | errors.c
5  | ez80.S
6  | gc.c
7  | graphicmodule.c
8  | linux_amd64.S
9  | linux_riscv64.S
10 | main.c
11 | neon.c
12 | neonio.c
13 | objects.c
14 | operators.c
15 | parser.c
16 | processcycle.c
17 | runtime.c
18 | serialization.c
19 | sidememory.c
20 | strings.c
21 | syntaxtrees.c
22 | trees.c
23 | win_amd64.S
24 | linenoise.c
25 | nativefunctions.c
---------------------------

CHOSES SPÉCIFIQUES À L'ARCHITECTURE :
> L'implémentation des fonctions assembleur
> set_stack_pointer
> Taille du buffer des registres
> ATOMIC_TIME

Liste des choses qui ne marchent pas
--> Neon crashe sur calculatrice quand il y a une erreur
--> La ligne de l'erreur avec du code parallèle
_____________________

Potentielles futures erreurs :
------------------------------
S'il y a des problèmes liés aux piles et tout, bien vérifier les tailles des contextes des fonctions et regarder si on gère ça correctement

Peut être qu'il y a des endroits où on oublie de mettre à jour les promesses

General Protection Fault => Problème d'alignement de la pile

Si il y a un bug bizarre sur TI_EZ80, penser aux optimisations du compilateurs


Avancement et choses à faire :
--> Faire une variable numéro de ligne pour chaque processus différent, et lors d'une erreur, afficher les lignes de tous les processus
-------------------------------

PLUS TARD
--> Comparaison de TreeBuffers (en le parcourant de manière consciente de l'arbre dans lequel on est et en parcourant intelligemment les arbres)
--> Ajouter un type de donnée bitmap à Neon
--> Ajouter des tables de hachage à Neon (et une fonction hash par la même occasion)
--> Proposer de sauvegarder l'environnement au moment où il y a une erreur et où Neon s'apprête à quitter
--> Sauvegarder un environnement entier
> Faire des vrais arguments (genre -i, etc)
> Permettre à un processus d'arrêter ou de pauser un processus via une promesse


NOUVEAUTéS après la mise à jour 4.1 :
-------------------------------------------
- Fonction index sur les chaînes de caractères
- Affichage de la signature de la fonction dans help() d'une fonction built-in
- Ajout des fonctions bin et hex pour convertir des nombres en décimal et binaire
- Ajout des fonctions serialize et deserialize pour sauvegarder n'importe quel objet Neon dans un fichier
- Suppression de la fonction initGraphics et remplacement par une fonction init qui prend des noms de modules natifs en arguments et les initialise
- Restriction des règles des arguments optionnels dans les fonctions
- Ajouter dans la doc: si on a un argument qui s'appelle __local_args__, il overwrite la valeur de la variable __local_args__
- Ajout de la fonction format
- Ajout de la fonction hash pour hacher n'importe quel objet
- Ajout de la possibilité de formater le message dans raise, et de spécifier des arguments supplémentaires
*/





#ifdef TI_EZ80
int main(void) {
#else
int main (int argc, char* argv[]) {
#endif
    int error = neonInit();
    if (error < 0)
        return 0;

    // définition de la liste des arguments
    NeObj l = neo_list_create(0);

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
