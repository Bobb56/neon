#define NEON_SOURCE_ID 10

#include <string.h>

#include "headers/constants.h"
#include "headers/neon.h"
#include "headers/neonio.h"

#ifdef TI_EZ80
#include "headers/errors.h"
#endif

/*
CHOSES SPÉCIFIQUES À L'ARCHITECTURE :
> L'implémentation des fonctions assembleur
> set_stack_pointer
> Taille du buffer des registres
> ATOMIC_TIME

Liste des choses qui ne marchent pas
_____________________

Potentielles futures erreurs :
------------------------------
S'il y a des problèmes liés aux piles et tout, bien vérifier les tailles des contextes des fonctions et regarder si on gère ça correctement

Peut être qu'il y a des endroits où on oublie de mettre à jour les promesses

General Protection Fault => Problème d'alignement de la pile

Dans runtime, quand on crée l'arbre (le functioncall associé à un lancement en parallèle,
on ne le crée pas dans global_env->FONCTIONS)


Avancement et choses à faire :
-------------------------------
PLUS TARD
--> Proposer de sauvegarder l'environnement au moment où il y a une erreur et où Neon s'apprête à quitter
--> Sauvegarder un environnement entier
--> Faire des messages d'erreur avec des paramètres, définis par neon_fail
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
