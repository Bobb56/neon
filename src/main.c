#include <string.h>

#include "headers/constants.h"
#include "headers/neon.h"
#include "headers/runtime.h"
#include "headers/errors.h"
#include "headers/neonio.h"
#include "headers/serialization.h"


/*
CHOSES SPÉCIFIQUES À L'ARCHITECTURE :
> lowlevel.asm
> set_stack_pointer

Liste des choses qui ne marchent pas
_____________________

Potentielles futures erreurs :
-> Désérialisation de f ne calcule pas la même chose
-> la version désérialisée de f ne veut pas être supprimée
------------------------------
S'il y a des problèmes liés aux piles et tout, bien vérifier les tailles des contextes des fonctions et regarder si on gère ça correctement

Peut être qu'il y a des endroits où on oublie de mettre à jour les promesses

General Protection Fault => Problème d'alignement de la pile

Dans runtime, quand on crée l'arbre (le functioncall associé à un lancement en parallèle,
on ne le crée pas dans global_env->FONCTIONS)


Avancement et choses à faire :
-------------------------------
PLUS TARD
--> Comparer les TreeBuffer pour comparer des fonctions ?
--> Ajouter compteur de références dans le treebuffer
--> Ajouter un header dans le fichier de sérialisation d'objets qui permet de rendre compatibles entre eux des runtimes (exemple : donner l'association nom de variable/indice dans ADRESSES, les champs de containers, etc)
--> Sauvegarder un environnement entier
--> Faire une fonction saveVar(nom_fichier, variable) et loadVar(nom_fichier) qui stocke tout ça dans un fichier
> Faire des vrais arguments (genre -i, etc)
> Permettre à un processus d'arrêter ou de pauser un processus via une promesse


NOUVEAUTéS après la mise à jour 4.1 :
-------------------------------------------
- Fonction index sur les chaînes de caractères
- Affichage de la signature de la fonction dans help() d'une fonction built-in
- Ajout des fonctions bin et hex pour convertir des nombres en décimal et binaire
*/





#ifdef TI_EZ80
int main(void) {
#else
int main (int argc, char* argv[]) {
#endif

    /*FILE* f = fopen("test2.dat", "w+");
    write_number_value2(f, -1);
    fclose(f);
    f = fopen("test2.dat", "r");
    printf("Read value : %ld\n", read_number_value2(f));
    fclose(f);*/



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
