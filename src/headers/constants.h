#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdbool.h>

#define VERSION "3.8.5.6-beta"
/*
Numérotation de versions :
Ajout d'opérateur ou de fonction ou modification d'un élément de syntaxe : +0.0.1
Ajout d'une nouvelle syntaxe ou modification de la structure d'exécution : +0.1
Modification mineure : +0.0.0.1
Modification du look de l'interpréteur en mode console : +0.1
Ajout d'un type de données NeObject : +0.0.1
*/

#define INVITATION "https://discord.gg/wkBdK35w2a"


// à définir en fonction des destinations de compilation
#define LINUX_AMD64
//#define WINDOWS_AMD64
//#define TI_EZ80

#ifdef LINUX_AMD64
    #define PLATFORM "LINUX_AMD64"
#endif
#ifdef WINDOWS_AMD64
    #define PLATFORM "WINDOWS_AMD64"
#endif
#ifdef TI_EZ80
    #define PLATFORM "TI_EZ80"
#endif



#ifdef TI_EZ80
    #include <ti/real.h>
    #include <sys/rtc.h>
#endif


#if defined(LINUX_AMD64) || defined(TI_EZ80)
    // a définir uniquement si la console standard du système d'exploitation visé supporte les couleurs
    #define COLOR
#endif



// si la version actuelle n'est pas stable mais est en distribution
//#define EXPERIMENTAL

#define SEQUENCE_ENTREE    ">> "
#define SEQUENCE_SUITE     ".. "

#define NB_ERRORS 115

#define STACK_SIZE (8*1024*1024) // taille de la pile par défaut à 8 Mio, ça peut changer en fonction de la plateforme

#define REG_BUFFER_SIZE 64 // taille de la zone de sauvegarde de registres dans les processus

/*constantes de couleur*/
#define BLUE 0
#define RED 1
#define WHITE 2
#define GREEN 3

#define PI 3.141592653589793

#define UNIT ((void*)-1) // valeur de pointeur spéciale comme NULL

// pour builtinfonc et opfonc
#define NBOPERATEURS 40


// definition des types d’arguments pris par les operateurs
// pour construire un type d'opérateur, on fait GRAMMAR ou bien GRAMMAR | LAZY s'il est paresseux

#define VARLEFT     1   // une variable a gauche
#define RIGHT       2   // une operande a droite
#define RIGHT_LEFT  4   // une operande a droite et a gauche 
#define VAR_RIGHT   8   // une variable a gauche et une operande a droite
#define LEFT_VAR    16  // une variable à doite et n'importe quoi à gauche
#define VARRIGHT    32  // une variable a droite
#define VAR_VAR     64  // une variable à droite et une variable à gauche
#define SPECIAL     128 // les opérateurs que l'on peut retrouver partout
#define LAZY        256 // pour les opérateurs attendant un Tree* au lieu d'un NeObj

// valeurs de retour
#define CONTINUE 1
#define BREAK 2
#define PASS 3
#define EXIT_RETURN 4

// valeurs des blockword1line
#define RETURN 1
#define IMPORT 2
#define LOCAL 3
#define AWAIT 4

// ces types servent à la fois de type d'objet dans l'arbre de syntaxe,
// de type d'élément de syntaxe utilisé par cut et de type de NeObject
#define FLATTEN(type)           (type & 127) // on enlève le HEAP_ALLOCATED pour remettre le type sur l'intervalle [0, 127]
#define HEAP_ALLOCATED          128 // pour les objets n'ayant pas de dépendance sur le tas


#define TYPE_ANYTYPE            127
#define TYPE_FONCTION           (0 | HEAP_ALLOCATED)
#define TYPE_LISTINDEX          1
#define TYPE_OPERATOR           2
#define TYPE_STRING             (3 | HEAP_ALLOCATED)
#define TYPE_INTEGER            4
#define TYPE_DOUBLE             5
#define TYPE_LIST               (6 | HEAP_ALLOCATED)
#define TYPE_BLOCK              7
#define TYPE_BOOL               8
#define TYPE_CONST              (9 | HEAP_ALLOCATED)
#define TYPE_VARIABLE           10
#define TYPE_BLOCKLINE          11
#define TYPE_ENDOFLINE          12
#define TYPE_PARENTHESE1        13
#define TYPE_PARENTHESE2        14
#define TYPE_VIRGULE            15
#define TYPE_KEYWORD            16
#define TYPE_NONE               17
#define TYPE_BLOCKWORD1LINE     18
#define TYPE_KEYWORDFUNCTION    19
#define TYPE_EXCEPTION          20
#define TYPE_EXPRESSION         21

#define TYPE_COMMENT            22
#define TYPE_WORD               23
#define TYPE_CROCHET            24
#define TYPE_BLOCKWORD          25
#define TYPE_ACCOLADE           26
#define TYPE_CONDITIONBLOCK     27

#define TYPE_STATEMENTFOR       28
#define TYPE_STATEMENTWHILE     29
#define TYPE_STATEMENTIF        30
#define TYPE_STATEMENTELIF      31
#define TYPE_STATEMENTELSE      32
#define TYPE_STATEMENTTRY       33
#define TYPE_STATEMENTEXCEPT    34

#define TYPE_FUNCTIONDEF        35
#define TYPE_METHODDEF          36

#define TYPE_BLOCKFOR           37
#define TYPE_BLOCKWHILE         38
#define TYPE_BLOCKIF            39
#define TYPE_BLOCKELIF          40
#define TYPE_BLOCKELSE          41
#define TYPE_BLOCKTRY           42
#define TYPE_BLOCKEXCEPT        43
#define TYPE_TRYEXCEPT          44
#define TYPE_ATOMICLINE         45
#define TYPE_ATOMICBLOCK        46

#define TYPE_SYNTAXTREE         47

#define TYPE_BUILTINFUNC        48
#define TYPE_USERMETHOD         (49 | HEAP_ALLOCATED)
#define TYPE_USERFUNC           (50 | HEAP_ALLOCATED)
#define TYPE_CONTAINER          (51 | HEAP_ALLOCATED)
#define TYPE_ATTRIBUTE          52
#define TYPE_PROMISE            (53 | HEAP_ALLOCATED)
#define TYPE_AWAIT              54

#define TYPE_EMPTY              55

#define TYPE_BLOCKFOREACH       56
#define TYPE_STATEMENTFOREACH   57

#endif