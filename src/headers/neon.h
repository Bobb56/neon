#ifndef NEON_H
#define NEON_H

#include <stdlib.h>

#include "objects.h"
#include "dynarrays.h"
#include "processcycle.h"

#ifdef TI_EZ80
#include "extern/nio_ce/headers/nspireio.h"
#endif

typedef struct NeonEnv {

    // permet de déclencher un signal d'erreur à travers toute la chaîne de traitement de Neon.
    // la règle est : si CODE_ERROR est non nul, il faut détruire les objets que l'on a créés dans cette fonction, et retourner
    int CODE_ERROR;

    // cette variable indique le nombre d'instructions exécuté sur chaque processus avant de passer au suivant
    // il s'agit d'une variable et non d'une constante car la fonction setAtomicTime doit pouvoir le modifier
    int ATOMIC_TIME;
    // cette variable est le compteur atomique périodique, qui fait changer de processus une fois arrivé à zéro
    int atomic_counter;


    // LES VARIABLES PERMETTANT DE GERER LES PROMESSES

    // PROMISES associe aux identifiants des processus (l'indice dans le tableau) la valeur renvoyée par le processus
    // Cette valeur est NULL quand le processus n'a pas terminé, et devient NeObj avec la valeur renvoyée une fois le processus terminé
    NeList* PROMISES;

    // PROMISES_CNT compte le nombre de promises d'un même processus créées (en faisant des copies de la promesse originale)
    // qui n'ont pas encore été transformées en la valeur renvoyée par le processus
    // Ces compteurs servent à savoir jusqu'à quand garder la valeur renvoyée par le processus dans PROMISES
    // Quand un compteur atteint zéro, on n'a plus besoin de la valeur, donc on peut la supprimer
    // c'est un tableau de pointeurs sur les compteurs de références
    intptrlist PROMISES_CNT;

    // une liste de booléens qui indique si un processus a fini pour savoir si on peut lui piquer sa place
    // Quand un processus n'a pas fini, on ne peut pas lui prendre sa place, en revanche ce n'est pas parce qu'il a fini qu'on peut lui prendre sa place
    // En effet, même si un processus a fini, parfois toutes les promesses n'ont pas encore récupéré leur valeur
    intlist PROCESS_FINISH;

    // cette variable globale sert à stocker le texte des exceptions lancées par l'utilisateur (user-defined exceptions)
    // printError affiche cette chaine de caractères lorsqu'il détecte une exception lancée par l'utilisateur
    char* EXCEPTION; // exception levée par l'utilisateur, à afficher par printError

    // cette variable contient les exceptions par défaut et toutes les exceptions créées par l'utilisateur
    strlist* EXCEPTIONS;


    // VARIABLES RELATIVES A LA ZONE DU PROGRAMME ACTUELLEMENT INTERPRÉTÉE
    // NAME représente l'indice dans ADRESSES du NeObject représentant le nom actuel de fichier interprété
    // Cette valeur est utilisée par la variable __name__
    Var NAME; // adresse du NeObject représentant le nom actuel de fichier. A ne pas utiliser, il y a à la place la fonction updateFileName

    // cette chaine de caractères représente le nom du fichier actuellement interprété
    // Ce n'est pas la même chose que NAME car ceci n'est utilisé que pour se souvenir du nom du fichier dans lequel aller lire
    // la ligne à laquelle s'est produite une erreur. Par conséquent l'utilisateur ne doit pas pouvoir la modifier, elle ne peut
    // pas valoir "__main__" comme c'est le cas pour NAME
    char* FILENAME;

    // cette variable est mise à jour à chaque nouveau sous-arbre interprété, et contient le numéro de la ligne actuellement interprétée
    // cette variable couplée à FILENAME permet d'afficher la ligne du programme actuellement interprétée lorsqu'une erreur est levée
    int LINENUMBER; // contient le numéro de la ligne actuellement interprétée

    // Cette variable est affectée lorsqu'une fonction utilisateur utilise return (). La valeur affectée est récupérée juste après
    // par le bout de code qui avait lancé l'exécution de la fonction
    NeObj RETURN_VALUE; // contient la valeur de retour de la fonction si le programme a été interrompu par un return ()

    // VARIABLES RELATIVES AU CONTAINERS
    // les types containers sont chacun associées à un indice unique, le même dans ces deux tableaux
    // pour un type de container, son indice dans CONTAINERS donne son nom (exemple : Cmplx, Point, ...)
    strlist* CONTAINERS; // à un indice du tableau, associe le nom du type de container
    // dans les containers, en interne, les champs sont stockés dans une NeList, toujours dans le même ordre
    // Pour chaque type de container, ATTRIBUTES contient une liste de chaines de caractères correspondant aux noms des champs dans l'ordre
    NeList* ATTRIBUTES; // contient pour chaque type de container, une liste des noms de champs

    // Ceci n'est pas un arbre, mais plutot une forêt. Tous les fils de cet arbre sont les arbres syntaxiques des fonctions
    // définies par l'utilisateur. Les stocker permet de garder des versions uniques de ces arbres, même lors de la copie
    // de fonctions utilisateurs, et de tout libérer à la fin
    struct TreeList FONCTIONS; // cet arbre va contenir en fils tous les arbres de fonction des fonctions et des méthodes définies par l'utilisateur

    // Ce couple de variables permet de gérer les variables de Neon, c'est-à-dire les objets auxquels on peut directement accéder
    // par un nom depuis l'interpréteur.
    // au cours de toute la vie d'une variable, son NeObj restera toujours le même, même quand on la détruit avec del
    // Les noeuds des arbres faisant références à des variables stockent également le même NeObj que dans ADRESSES
    // Ainsi, pour modifier une variable, on ne peut pas changer son NeObj (qui est fixe), on change donc directement son
    // type, et son data. Cela engendre les magouilles qu'on fait pour appeler des fonctions et affecter ses arguments, et pour
    // récupérer les valeurs de la variable objet d'une méthode
    strlist* NOMS; // contient les noms des variables associés à leurs adresses mémoire dans ADRESSSES
    NeList* ADRESSES; // contient les objets associés aux noms dans NOMS


    // cette liste contiendra une neo_copy de tous les objets pouvant boucler sur eux-mêmes (listes, containers)
    // on s'en sert pour le Mark & Sweep : on marque tous les objets accessibles depuis ADRESSES, et ensuite on parcourt les objets
    // de OBJECTS_LIST, et s'il ne sont pas marqués, c'est qu'ils ne sont pas accessibles et doivent être supprimés
    // ceci est un objet qui contiendra un container ou une nelist. Il s'agit de la tête d'une liste chaînée de containers
    // et de nelist. Tous les containers et nelist existants sont chaînés dedans
    // les containers et nelist contiennent un champ spécial qui est le chaînon suivant de la liste
    NeObj OBJECTS_LIST;

    // cet objet est le cycle des différents processus qui s'exécutent en même temps. L'interpréteur passe son temps à
    // passer d'un processus à l'autre dans une dans infernale. Pour ajouter un processus, il suffit d'ajouter un chaînon
    // dans ce cycle
    ProcessCycle* process_cycle;

    // la console de nio_ce
    #ifdef TI_EZ80
    nio_console console;
    #endif

} NeonEnv;

extern NeonEnv* global_env;

/*NEON.C*/
void NeonEnv_destroy(NeonEnv* env);
NeonEnv* NeonEnv_init(void);
NeonEnv* NeonEnv_set(NeonEnv* new_env);
void setNeonEnv(NeonEnv* env);
void printRes(NeObj res);
void execFile(char* filename);
void importFile(char* filename);
void startMessage(void);
void terminal (void);
void neonInit(void);
void neonExit(void);


#endif