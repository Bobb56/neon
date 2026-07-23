#define NEON_SOURCE_ID 10

#include <string.h>

#include "headers/constants.h"
#include "headers/neon.h"
#include "headers/neonio.h"

#ifdef TI_EZ80
#include "headers/errors.h"
#include "extern/ide/headers/neonide.h"
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
24 | deadline.c
25 | nativefunctions.c
26 | interpcontext.c
27 | eval.c
28 | exec.c

100 | ide/console.c

---------------------------

CHOSES SPÉCIFIQUES À L'ARCHITECTURE :
> L'implémentation des fonctions assembleur
> set_stack_pointer
> Taille du buffer des registres
> ATOMIC_TIME

Liste des choses qui ne marchent pas
- Deadline word left/right
_____________________

Potentielles futures erreurs :
------------------------------
S'il y a des problèmes liés aux piles et tout, bien vérifier les tailles des contextes des fonctions et regarder si on gère ça correctement

Peut être qu'il y a des endroits où on oublie de mettre à jour les promesses

General Protection Fault => Problème d'alignement de la pile

Si il y a un bug bizarre sur TI_EZ80, penser aux optimisations du compilateurs


Avancement et choses à faire :
------------------------------
--> Ajouter un type de donnée bitmap/nombres scalaires à Neon
--> Ajouter un objet graphique Sprite qui est un tableau de nombres à deux dimensions. Chaque nombre n'est pas forcément un pixel mais est un rectangle, et les dimensions de chaque unité sont également précisées dans l'objet
--> Ajouter des tables de hachage à Neon
--> Permetter la gestion propre des namespaces (ajouter un mot-clé module), et mieux gérer les modules built-in
--> Ajouter une page Features history sur le site, ou bien une section News, ou Changelog sur la page de téléchargement, pour présenter les nouveautés apportées par chaque version. Un peu comme une grande page avec plein de cadres et une timeline, sur laquelle on peut faire des liens vers un cadre en particulier
--> Faire une variable numéro de ligne pour chaque processus différent, et lors d'une erreur, afficher les lignes de tous les processus
--> Sauvegarder un environnement entier
--> Implémentation de sauvegarde de pile en retour précipité d'eval et compagnie, et de restauration d'état à partir de la valeur de la pile
--> Afficher une backtrace en retour d'erreur
--> Lors de la restauration d'une pile, réexécuter les contextes dont le code a changé
--> Proposer de sauvegarder l'environnement au moment où il y a une erreur et où Neon s'apprête à quitter
> Faire des vrais arguments (genre -i, etc)
> Permettre à un processus d'arrêter ou de pauser un processus via une promesse


NOUVEAUTéS après la mise à jour 4.1 :
-------------------------------------
+0.0.1      Fonction index sur les chaînes de caractères
+0.0.1      Affichage de la signature de la fonction dans help() d'une fonction built-in
+0.0.2      Ajout des fonctions bin et hex pour convertir des nombres en décimal et binaire
+0.0.2      Ajout des fonctions saveObj et loadObj pour sauvegarder n'importe quel objet Neon dans un fichier
+0.0.0.1    Suppression de la fonction initGraphics et remplacement par une fonction init qui prend des noms de modules natifs en arguments et les initialise
+0.0.0.1    Restriction des règles des arguments optionnels dans les fonctions
+0.0.1      Ajout de la fonction format
+0.0.1      Ajout de la fonction hash pour hacher n'importe quel objet
+0.0.1      Ajout de la possibilité de formater le message dans raise, et de spécifier des arguments supplémentaires
+0.1        Modification de l'affichage des erreurs et amélioration des messages d'erreur avec des informations spécifiques au problème ayant déclenché l'erreur
+0.0.1      Ajout de la surcharge de l'appel de fonctions
+0.0.1      Ajout de l'opérateur is
+0.0.1      Modification du type de retour de la fonction type
+0.1        Ajout de l'instruction define(var1, var2, ...) pour définir des constantes symboliques
+0.0.1      Remplacement de la fonction init par une instruction init, et fonctionnement avec des identifiers plutôt que des chaînes de caractères
+0.0.1      Modification des paramètres de la fonction setColor (constantes au lieu de chaînes de caractères)
+0.0.1      Obfuscation des données écrites par saveObj
+0.0.0.1    Ajout des couleurs GREY et ORANGE
+0.1        Coloration syntaxique dans le terminal

----------- TOTAL
+0.3.15.3

Choses en plus à ajouter dans la doc :
--------------------------------------
- Si on a un argument qui s'appelle __local_args__, il overwrite la valeur de la variable __local_args__
- Ajouts/modifications d'exceptions
- Ajout de l'opérateur is, fonctionnement un peu bizarre, fonctionne par mot et non par objet, modification de la fonction type qui renvoie une constante, création de Container qui crée une constante si la variable n'est pas déjà définie, le type de None c'est lui-même
- Les nouveaux mots-clé réservés (couleurs, types)
- Incompatibilité des fichiers sérialisés
- Ajout de nouvelles couleurs
- rgb prend ses intensités entre 0 et 255
- renommage des champs des containers graphiques
- Fin de support pour les appvars python
*/


#ifdef TI_EZ80
int main(void) {
    kb_EnableOnLatch();
    gfx_Begin();
    set_neon_palette();

	launch_neonide();

    gfx_End();
    kb_DisableOnLatch();
    return 0;
}
#else
int main(int argc, char* argv[]) {
    int error = neonInit();
    if (error < 0)
        return 0;

    // définition de la liste des arguments
    NeObj l = neo_list_create(0);
    // ajout des arguments dans le tableau contenant les arguments du programme
    for (int i = 2 ; i < argc ; i++)
        neo_list_append(l, neo_str_create(neon_strdup(argv[i])));

    variable_append(global_env, "__args__", l);

    if (argc >= 2) {
        execFile(argv[1]);
    }
    else if (launcher(LAUNCHER_NAME)) {
        execFile(LAUNCHER_NAME);
    }
    else {
        run_interactive();
    }
    neonExit();
    return 0;
}

#endif
