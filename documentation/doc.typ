#set page(numbering: "1")

#align(center + top)[ // Centre le contenu horizontalement et l'aligne en haut de la page
  #text(
    weight : "bold",
    2em, // Taille du texte : 2.5 fois la taille de base du document
    "Langage de programmation Neon" // Le texte de votre titre
  )
  #v(1em) // Un petit espace après le titre pour un sous-titre si besoin
  #text(1.5em, "Documentation") // Un sous-titre optionnel
]

= Introduction

Neon est un langage de script généraliste nativement concurrent destiné entre autres à rendre possible la programmation concurrente possible sur des machines ne disposant pas de système d'exploitation multitâches. Le langage Neon permet la description de programmes de calcul séquentiels et concurrents, l'interpréteur Neon permet de les exécuter.

Neon est designé pour être facile à comprendre et facile à utiliser. Cette documentation n'a pas de vocation pédagogique mais sert à recenser de manière exhaustive l'entièreté des fonctionnalités de Neon. Ce document n'est pas non plus destiné à être lu dans l'ordre, mais à ranger les informations dans des catégories.

Par Neon, on désigne à la fois le langage dans ses spécificités et à la fois son seul et unique interpréteur.


#outline(title : "Sommaire")



= Partie 1 : Les objets et les variables

Tous les objets utilisables dans les programmes Neon sont regroupés au sein d'une seule et unique structure C appelée NeObject.

Voici la liste de tous les types de NeObjects existants :

`Integer`\
`Decimal`\
`String`\
`Bool`\
`List`\
`NoneType`\
`Exception`\
`Built-in function`\
`Function`\
`Method`\
`Container`\
`Promise`

== 1.1 - Les variables, listes et containers

=== 1.1.1 - Conserver des objets unitaires avec des variables

Une variable une case mémoire gérée par Neon qui peut contenir un objet. La plupart des objets créés dans Neon n'ont pas d'existence persistante. Ils sont créés lors de l'évaluation d'une expression, utilisés comme argument d'une fonction ou d'un opérateur, puis supprimés.

Pour conserver un objet dans la durée, il est possible de le stocker à l'intérieur d'une variable. Une variable est désignée par un nom. Son nom doit obligatoirement commencer par une lettre ou un tiret du bas `_`. Les caractères suivants peuvent être des lettres, chiffres, tirets du bas et apostrophes `'`. De nombreux opérateurs permettent de gérer les variables, modifier leur contenu, les supprimer, etc.

Si une variable n'existe pas et qu'une expression essaie d'accéder à sa valeur, une erreur sera déclenchée. Pour créer une variable, il suffit de lui assigner une valeur comme si elle existait déjà. Exemple : `maNouvelleVariable = 12`. Ceci suffit à créer une variable qui aura pour valeur 12.

En plus des variables il existe d'autres moyens de conserver des valeurs dans la durée. Il existe des objets qui permettent de ranger un certain nombre d'autres objets, d'y accéder et de les modifier : les listes et les containers. Leurs cas d'utilisation sont différents.

=== 1.1.2 - Un objet de stockage à grande échelle : les listes

Les listes sont des objets qui ont la capacité de stocker un nombre (presque) illimité d'autres objets, de manière ordonnée. On les utilise pour stocker une grande quantité d'informations, souvent de même type. Une liste est comme une suite de variables, où chaque élément est identifié par un indice. Chaque élément d'une liste (donc identifié par un certain indice) est une case mémoire au même titre qu'une variable. Ainsi, les opérateurs qui permettent d'interagir avec les variables permettent également et au même titre d'interagir avec les cases des listes.

=== 1.1.3 - Regrouper des informations avec les containers

Les containers sont également des objets contenant d'autres objets. Chaque champ d'un container est également une case mémoire au même titre qu'une variable, et peut être modifié grâce aux mêmes opérateurs que ceux qui modifient le contenu des cases des listes et des variables. Les containers sont utilisés pour regrouper un certain nombre d'informations différentes concernant le même objet.

Bien que ces trois entités (variables, listes et containers) aient des points communs, il est important de bien comprendre la différence fondamentale entre variables et listes/containers.

Une variable est certes une case mémoire qui peut stocker un objet, mais contrairement aux listes et aux containers, une variable *n'est pas* un objet. Elle contient un objet. La seule manière d'accéder à une variable est par son nom. Ainsi une variable peut contenir une liste ou un container, ou tout autre objet, mais ne peut pas contenir une autre variable.

En revanche, une liste ou un container est un simple objet. Il n'a pas de nom à proprement parler, et peut être créé comme résultat d'une expression comme il peut simplement être supprimé dès que l'on n'en n'a plus besoin. Pour le conserver il faut le stocker aussi (dans une liste, un container ou une variable).


== 1.2 - Les objets


=== 1.2.1 - L'organisation en mémoire, compteur de références et garbage collector
Tous les objets sont des structures passées sur la pile de la taille d'un pointeur + un octet. Certains objets comme les nombres, les booléens, les constantes None, les exceptions et les promesses sons stockés uniquement dans cette structure. Pour les autres objets, la structure contient un pointeur vers une zone dans le tas qui contient réellement l'objet. Quand on copie ces objets, seulement la structure passée sur la pile est copiée, le bloc dans le tas est le même pour toutes les copies. Chacun de ces blocs contient un compteur qui compte le nombre de références du bloc, et le bloc est libéré quand ce compteur atteint zéro.

Certains objets (les containers et le listes) contiennent d'autres objets. Cette situation peut créer des objets cycliques rendant le compteur de références insuffisant.

Pour cela Neon dispose en plus d'un Garbage Collector. Le Garbage Collector maintient une liste de tous les containers et de toutes les listes créés. À certains endroits précis de l'interpréteur, un ramassage est déclenché, et Neon marque tous les objets accessibles depuis les variables encore vivantes. Les objets non marqués lors de cette phase sont supprimés définitivement.

=== 1.2.2 - Le type `Integer`

Les objets de type `Integer` sont des nombres entiers. Leur taille dépend du système cible, ce sont des entiers 64 bits sur des OS 64 bits, et des entiers 24 bits sur ez80.

Les `Integer` peuvent être créés par des constantes dans le code source du programme, combinés entre avec des opérateurs pour en générer de nouveaux ou envoyés dans des fonctions built-in pour en créer de nouveaux. Voir la section sur les opérateurs et les fonction built-in pour plus d'informations.

Il est possible de définir des constantes `Integer` en écriture décimale, mais également en hexadécimal et en binaire. Pour définir un `Integer` à partir de son écriture hexadécimale, il faut précéder l'écriture hexadécimale du préfixe `0x`, et du préfixe `0b` pour le binaire.

Pour l'hexadécimal, les lettres peuvent être des lettres minuscules ou majuscules.

=== 1.2.3 - Le type `Decimal`

Les objets de type `Decimal` sont des nombres flottants. Comme pour les nombre entiers, leur taille dépend du système cible. Ce sont des flottants à double précision lorsque l'OS le permet, sinon ce sont des float sur 32 bits (même pour ez80).

Les `Decimal` peuvent être créés par des constantes dans le code source du programme, combinés entre avec des opérateurs pour en générer de nouveaux ou envoyés dans des fonctions built-in pour en créer de nouveaux. Voir la section sur les opérateurs et les fonction built-in pour plus d'informations.


*Note :* Les opérateurs de calcul sur les nombres ne préservent pas tous le type. En effet, alors que la multiplication, la soustraction et la somme de deux entiers reste un entier, la division de deux entiers renverra toujours un nombre décimal. Ce comportement des opérateurs est explicité plus en détails dans la section sur les opérateurs.




=== 1.2.3 - Le type `Bool`

Les objets de type `Bool` ne peuvent valoir que `True` ou `False`. Ce sont les deux seules valeurs possibles. Ces valeurs peuvent être obtenues en les écrivant littéralement dans un programme, ou grâce aux opérateurs booléens, qui seront décrits dans la section Opérateurs.

Toute expression booléenne (résultat des opérateurs de comparaison, des opérateurs logiques) s'évalue en `True` ou `False`.

Les booléens permettent de représenter des valeurs de vérité, et donc d'instrumenter des branchements conditionnels, des boucles, etc.

=== 1.2.4 - Le type `List`

Une liste est un objet permettant de stocker une suite finie d'objets quelconques. Le terme informatique exact pour décrire cet objet serait tableau. Le langage de programmation Neon ne fait pas de différenciation entre ces deux termes. Lorsque l'on parle de liste chaînée, on parle explicitement de liste chaînée. On peut indexer des listes grâce à la syntaxe suivante : `list[i]`. L'indexation des éléments d'une liste commence à zéro, au lieu de commencer naturellement à 1. Ainsi, `list[i]` ne correspond pas à l'élément i mais à l'élément i+1. Cela peut paraîtré déroutant pour quelqu'un qui n'y est pas habitué, mais c'est en réalité cohérent avec le reste du langage. Ce choix a été fait pour rester dans les conventions de la grande majorité des langages de programmation. Historiquement, cette convention date des années 70 avec le langage C notamment, où l'indexation des tableaux elle exprime en réalité un *décalage par rapport au premier élément* et non l'accès à l'élément n°i.

Une liste peut contenir n'importe quel type d'objet, y compris elle-même.

Lorsque l'on copie une liste, ses éléments ne sont pas copiés. Ainsi toute modification apportée à une copie de liste apparaîtra sur toutes les autres copies et l'originale. Pour vraiment copier une liste, et la rendre indépendante de la liste originale, il faut utiliser la fonction `copy` pour faire une copie profonde.

On peut obtenir des listes de plusieurs manières, en récupérant le retour de certaines fonctions ou certains opérateurs, mais également en écrivant des listes littérales. Ce sont des expressions qui sont évaluées en une liste.

La syntaxe pour créer une liste littérale est `[element1, element2, ..., elementn]`. Les éléments peuvent être des expressions quelconques. La liste contiendra les objets résultants de l'évaluation des expressions (voir la section sur les expressions pour de plus amples explications).


=== 1.2.5 - Le type `String`

Les String sont des chaînes de caractères ASCII null-terminated. Elles peuvent être crées comme constantes dans le code source du programme ou combinées entre elles/obtenues grâce à d'autres fonctions. Pour créer une chaîne de caractère comme constante, il faut entourer le texte de guillemets `"` ou bien d'apostrophes `'`. Exemple :

`mot = 'voiture'`\
ou bien\
`mot = "voiture"`

L'intérêt de ces deux différentes syntaxes est que pour définir une chaîne contenant le caractère `"`, on peut utiliser les délimiteurs `'` et inversement.

On peut définir certains caractères spéciaux dans les chaînes de caractères à l'aide du caractère \\. Voici la liste complète :\
`\a` #sym.arrow.r.long caractère d'appel\
`\b` #sym.arrow.r.long retour arrière\
`\f` #sym.arrow.r.long nouvelle page\
`\r` #sym.arrow.r.long retour chariot\
`\v` #sym.arrow.r.long tabulation verticale\
`\t` #sym.arrow.r.long tabulation horizontale\
`\n` #sym.arrow.r.long nouvelle ligne

On peut notamment obtenir leur taille grâce à la fonction `len`, et accéder au caractère n°i grâce à la syntaxe `string[i]`. Cette syntaxe est la même que pour indexer des listes.

=== 1.2.6 - Le type `NoneType`

Le type `NoneType` désigne un seul et unique objet : la constante `None`. C'est le seul objet de type `NoneType`. Cette constante peut être obtenue en écrivant None dans un programme, ou récupérée comme retour de certaines fonctions et de certains opérateurs. Cette valeur est une valeur par défaut qui sert souvent à indiquer que quelque chose ne contient rien ou que quelque chose ne sert à rien, ou est vide.

=== 1.2.7 - Le type `Exception`

Les exceptions sont des objets qui désignent des types d'erreur. Quand une erreur est déclenchée dans Neon, cette erreur correspond à une certaine exception. On peut se servir de ces exceptions pour attrapper certains types d'erreurs via `try .. except`. Plus exactement, quand une exception est levée dans un bloc `try`, on peut lancer l'exécution d'un certain code en fonction de l'exception levée. Il y a des exceptions built-in, et des exceptions qui peuvent être créées par la fonction `createException`.

Voici la liste des exceptions built-in :\
*`SyntaxError`* : Est déclenchée par une erreur de syntaxe\
*`FileNotFound`* : Est déclenchée quand l'ouverture d'un fichier quelconque échoue dans l'interpréteur\
*`UnmeasurableObject`* : Déclenchée par la fonction `len` sur un objet non mesurable\
*`UndefinedVariable`* : Lorsqu'on essaie d'utiliser un objet non défini\
*`IncorrectFunctionCall`* : Lorsqu'un appel à une fonction échoue\
*`MemoryError`* : Erreur lors de l'allocation de ressources. Ce type d'erreur est généralement grave\
*`NonIndexableObject`* : Tentative d'indexer un objet non indexable. Seules les listes et les chaînes de caractères sont indexables\
*`IncorrectIndex`* : Déclenchée lorsque l'index d'une chaine de caractères ou d'une liste n'est pas un entier positif\
*`OutOfRange`* : Lorsqu'une indexation dépasse la taille d'un objet\
*`IncorrectType`* : Une type n'est pas compatible avec une certaine opération\
*`DivisionByZero`* : Division euclidienne par zéro\
*`UnknownError`* : Déclenchée lorsqu'il n'y a pas assez d'informations sur l'erreur\
*`AssertionFailed`* : Déclenchée lorsque la fonction `assert` échoue\
*`DefinitionError`* : Déclenchée principalement lorsque la définition d'un container est incorrecte au regard des informations dont dispose l'interpréteur sur ce type de container\
*`KeyboardInterrupt`* : Déclenchée par un Ctrl-C dans le terminal\

=== 1.2.8 - Le type `built-in function`

Ce type regroupe toutes les fonctions présentes au chargement de l'interpréteur en mémoire.

Les fonctions sont des objets comme les autres, elle peuvent passer de variable en variable, être stockées dans des listes, etc. On peut appeler tout objet contenant une fonction.
Toutes les informations sur une fonction built-in peuvent être obtenues en tapant `help(fonction)` dans le terminal.

De manière générale, quand une fonction ne renvoie rien, elle renvoie en réalité None.

Voici la liste de toutes les fonctions built-in :

*`print` :*\
Cette fonction attend un nombre indéfini de paramètres, et affiche la représentation en objet de chaque paramètre, séparé par un espace. Cette fonction affiche également un retour à la ligne. Elle renvoie None.

*`input` :*\
Cette fonction prend en argument une chaîne de caractères, l'affiche et attend du texte de l'utilisateur. Elle renvoie une chaîne de caractères correspondant au texte entré.

*`nbr` :*\
Cette fonction prend en argument une chaîne de caractères représentant un nombre et la convertit en entier ou en décimal.

*`str` :*\
Cette fonction prend un objet quelconque et renvoie sa représentation textuelle évaluable par Neon.

*`len` :*\
Cette fonction prend une liste ou une chaîne de caractères et renvoie sa longueur.

*`sub` :*\
Cette fonction attend trois arguments : une chaîne de caractères et deux entiers. sub(string, i1, i2) renvoie la sous-chaîne de string commençant au caractère numéro i2 jusqu'au caractère numéro i2 exclu.

*`exit` :*\
Cette fonction n'attend aucun argument, et quitte l'interpréteur. Elle renvoie None

*`append` :*\
Cette fonction prend en argument une liste et un objet quelconque, et ajoute l'objet à la fin de la liste.

*`remove` :*\
Cette fonction prend en argument une liste et un indice de cette liste, et supprime l'élément présent à cet indice.

*`insert` :*\
Cette fonction prend en argument une liste, un objet et un indice dans cette liste, et insère l'objet à l'indice indiqué.

*`type` :*\
Cette fonction prend en argument un objet et renvoie son type. Les types d'objet sont représentés par des chaînes de caractères. Voici les types renvoyés par la fonction `type` :\
`"Bool"` #sym.arrow.r.long booléen\
`"String"` #sym.arrow.r.long chaîne de caractères\
`"Integer"` #sym.arrow.r.long entier\
`"Decimal"` #sym.arrow.r.long nombre décimal\
`"Built-in function"` #sym.arrow.r.long fonction built-in\
`"List"` #sym.arrow.r.long liste\
`"Function"` #sym.arrow.r.long fonction utilisateur\
`"Method"` #sym.arrow.r.long méthode utilisateur\
`"Exception"` #sym.arrow.r.long exception\
`"Promise"` #sym.arrow.r.long promesse\
`"unspecified type"` #sym.arrow.r.long correspond au type -1. Aucun objet n'a ce type, c'est une valeur spéciale servant à décrire la signature des fonctions. En général, un argument de type `unspecified type` peut être de plusieurs types, et ceux-ci sont spécifiés dans la chaîne d'aide de la fonction\
`"Undefined"` #sym.arrow.r.long renvoyé sur un objet non défini (de `TYPE_EMPTY`)\

La fonction type ne renvoie jamais de type `"Container"` car elle renvoie directement le nom du container.


*`reverse` :*\
Cette fonction prend en argument une chaîne de caractères et renvoie la chaîne inversée.

*`eval` :*\
Cette fonction prend en argument une chaîne de caractères correspondant à une expression Neon, et renvoie le résultat de l'évaluation de l'expression.

*`clear` :*\
Cette fonction efface le terminal.

*`help` :*\
Cette fonction affiche de l'aide liée à certains objets ou certains types d'objets. Voici les arguments possibles à la fonction `help` :
`help("modules")` #sym.arrow.r.long affiche tous les noms de modules présents dans la mémoire
`help("variables")` #sym.arrow.r.long affiche toutes les variables définies présentes dans la mémoire ainsi que leur type
`help("MonModule")` #sym.arrow.r.long affiche tous les objets liés au module MonModule présents dans la mémoire
`help(mon_objet)` #sym.arrow.r.long affiche le type de l'objet mon_objet

Dans le cas d'une fonction built-in, la fonction `help` affiche aussi le type des arguments attendus, le type de retour et une chaîne de caractères expliquant comment utiliser la fonction.
Dans le cas d'une fonction utilisateur (ou d'une méthode utilisateur), affiche le nom de la fonction, les arguments attendus, et si une chaîne de caractères d'aide a été assignée par la fonction `setFunctionDoc`, affiche cette chaîne.

*`randint` :*\
Cette fonction prend en argument deux entiers : une borne inférieure et une borne supérieure, et renvoie un entier aléatoire compris entre ces deux bornes, borne supérieure exclue.

*`failwith` :*\
Cette fonction prend en argument une chaîne de caractères, et quitte l'interpréteur en affichant cette chaîne de caractères.

*`time` :*\
Cette fonction renvoie le nombre de secondes écoulées depuis l'Epoch.

*`assert` :*\
Cette fonction prend en argument un booléen, et lève l'exception AssertionFailed si le booléen vaut False.

*`output` :*\
Cette fonction prend en argument un nombre illimité d'objets de tous types, et affiche tous ces objets à la suite sans retour à la ligne.

*`chr` :*\
Cette fonction prend en argument un code ASCII (nombre entier) et renvoie le caractère correspondant.

*`ord` :*\
Cette fonction prend en argument un caractère et renvoie son code ASCII.

*`listComp` :*\
Cette fonction sert à fabriquer des listes de manière efficace. Elle prend en argument, dans l'ordre :\
- Le nom d'une variable (dans une chaîne de caractères)\
- Un indice de début\
- Un indice de fin\
- Un pas\
- Une chaîne de caractères correspondant à une expression booléenne
- Une chaîne de caractères correspondant à une expression quelconque de Neon.

L'appel de `listComp("variable", debut, fin, pas, "condition", "expression")` renvoie une liste créée de cette manière :
```
l = []
for (variable, debut, fin, pas) do
    if (condition) then
        l.append(expression)
    end
end
```

*`createException` :*\
Cette fonction sert à créer des exceptions. Elle prend en argument une chaîne de caractères, et crée une exception ayant le nom donné en argument. Un nouveau mot-clé est créé avec le nom de cette exception, et elle est accessible directement avec ce mot-clé. La fonction `createException` renvoie également l'exception créée.

*`raise` :*\
Cette fonction prend en argument une exception et une chaîne de caractères et quitte l'interpréteur en levant cette exception et en affichant la chaîne de caractères comme message d'erreur.

*`int` :*\
Cette fonction prend en argument un objet et le convertit en entier.

*`index` :*\
Cette fonction prend en argument une liste et un objet de cette liste, et renvoie l'indice de la première apparition de l'objet dans la liste.

*`replace` :*\
Cette fonction prend en argument trois chaînes de caractères, et remplace toutes les occurences de la deuxième chaîne dans la première chaîne par la troisième chaîne.

*`count` :*\
Cette fonction compte le nombre d'apparitions d'une sous-chaîne dans une chaîne de caractères ou compte le nombre d'objets présents dans une liste. Il faut indiquer d'abord la liste ou la chaîne de caractères, puis la sous-chaîne ou le sous objet.

*`list` :*\
Cette fonction transforme une chaîne de caractères en liste dont les éléments sont les caractères de la chaîne.

*`sortAsc` :*\
Cette fonction trie une liste dans l'ordre croissant suivant l'ordre lexicographique ou l'ordre sur les nombres.

*`sortDesc` :*\
Pareil mais dans l'autre sens.

*`sin` :*\
Calcule le sinus d'un angle en radians.

*`cos` :*\
Calcule le cosinus d'un angle en radians.

*`tan` :*\
Calcule la tangente d'un angle en radians.

*`deg` :*\
Convertit en degrés un angle en radians.

*`rad` :*\
Convertit en radians un angle en degrés.

*`sqrt` :*\
Calcule la racine carrée d'un nombre.

*`ln` :*\
Calcule le logarithme népérien d'un nombre.

*`exp` :*\
Calcule l'exponentielle d'un nombre

*`log` :*\*
Calcule le logarithme base 10 d'un nombre.

*`log2` :*\
Calcule le logarithme base 2 d'un nombre

*`round` :*\
Calcule l'arrondi d'un nombre à la précision demandée en deuxième argument.

*`abs` :*\
Renvoie la valeur absolue d'un nombre.

*`ceil` :*\
Renvoie l'arrondi par valeur supérieure d'un nombre.

*`floor` :*\
Renvoie l'arrondi par valeur inférieure d'un nombre.

*`readFile` :*\
Prend en argument le nom d'un fichier texte et renvoie son contenu.

*`writeFile` :*\
Cette fonction prend en argument un nom de fichier et une chaîne de caractères, et écrit cette chaîne de caractères dans le fichier dont le nom a été indiqué en argument. Si le fichier existe, son contenu est remplacé.

*`setFunctionDoc` :*\
Cette fonction prend en argument une fonction utilisateur, et une chaîne de caractères, et définit cette chaîne de caractères comme message d'aide pour cette fonction. Ce message d'aide est affiché lorsque la fonction help est appelée avec cette fonction.

*`setAtomicTime` :*\
Cette fonction change la période de changement de processus avec l'entier donné en argument.

*`copy` :*\
Cette fonction renvoie la copie profonde d'un objet, en conservant les dépendances de pointeurs au sein de l'objet.

*`loadNamespace` :*\
Cette fonction charge dans la mémoire une copie des objets du module dont le nom est donné en argument sans préfixe.

*`gc` :*\
Cette fonction appelle le Garbage Collector.

*`setColor` :*\
Cette fonction change la couleur du texte affiché dans le terminal après son appel. Les couleurs disponibles sont : `"blue"`, `"red"`, `"green"`, `"white"`. Sur les terminaux où c'est disponible, le rouge et le bleu sont affichés en gras.


=== 1.2.9 - Le type `Function`

En Neon il est possible de créer des fonctions qui prennent des arguments en entrée et qui exécutent du code en fonction de ces arguments. Lorsqu'une fonction est définie, une variable avec le nom de la fonction est créée, avec pour valeur l'objet fonction correspondant. Tout objet fonction est appelable, comme c'est le cas pour les fonctions built-in. On peut appeler la fonction help dessus, et lui ajouter de l'aide grâce à la fonction setFunctionDoc.

La manière de créer des fonctions sera détaillée dans la section consacrée.

=== 1.2.10 - Le type `Method`

Une méthode est également un objet défini par utilisateur, quasiment comme une fonction. La seule différence est que contrairement à une fonction où tous les arguments sont des variables locales à la fonction et où leur modification n'a aucun impact sur les variables à l'extérieur, le premier argument d'une méthode peut être modifié directement par la méthode. Concrètement, une fois qu'une méthode a fini d'être exécutée, l'interpréteur Neon récupère la valeur de la première variable locale correspondant au premier argument, et l'assigne à l'objet qui a été envoyé en premier argument de la méthode. Ainsi toute modification effectuée au premier argument à l'intérieur d'une méthode sera appliquée à l'objet qui aura été envoyé en premier argument.

La manière de créer des méthodes sera détaillée dans la section consacrée.

=== 1.2.11 - Le type `Container`

Les containers sont des objets qui permettent de ranger de manière propre d'autres objets, en leur donnant chacun un nom à l'intérieur du container. Chaque container possède également un nom qui le caractérise parmi les autres containers. Tous les containers définis avec le même nom doivent posséder exactement les mêmes champs. Par souci de clarté, les noms de containers doivent commencer par une majuscule. À part les noms de module, ce sont les seuls objets à devoir commencer par une majuscule.

Exemple : Définition d'un container Personne
```
monContainer = Personne(prenom: "Paul", nom: "Durand", age: 34, enfants:["Pierre", "Jacques"])
```

Ce container est de type `Personne`, et contient les champs `prenom`, `nom`, `age` et `enfants`. La première apparition d'un container d'un certain type dans le code fixe les noms des champs pour ce type de container. C'est-à-dire qu'à partir du moment où le premier objet de type `Personne` aura été défini comme ci-dessus, tous les containers de type `Personne` devront contenir les champs `prenom`, `nom`, `age` et `enfants`. Il n'est pas nécessaire de définir les champs dans l'ordre. Tant que tous les champs sont présents, la définition est correcte.

Comme pour les listes, la copie d'un container n'entraîne pas la copie des objets qu'il contient. Ainsi un container peut se contenir lui-même, et une modification de champ dans un container entraînera la modification de ce champ dans toutes les copies qui ont été faites de cet objet et l'original. Pour vraiment copier un container il faut utiliser la fonction `copy`.

Pour accéder à un champ d'un container, il faut utiliser l'opérateur `>>`. Si on reprend l'exemple du container défini plus haut, `monContainer>>prenom` correspond à la variable qui contient `"Paul"`.

On peut utiliser cette syntaxe à la fois pour accéder aux noms des champs : print(monContainer>>prenom) et à la fois pour modifier les champs : `monContainer>>prenom = "Martine"`

=== 1.2.12 - Le type `Promise`

Les objets de type promise ne peuvent être obtenus que par retour du lancement en parallèle d'un processus. Neon permet le lancement de fonction en parallèle grâce au mot-clé `parallel`.

Quand on lance une fonction en parallèle avec `parallel fonction(arg1, arg2, args...)`, une promesse est renvoyée. Cette promesse est un objet qui ne sert à rien pendant que le processus tourne, si ce n'est identifier le processus qui l'a renvoyée, car elle est unique pour chaque processus. Tant que le processus n'a pas terminé, son type est `"Promise"`. Une fois que la fonction lancée en parallèle a terminé et renvoyé sa valeur, la promesse que le mot-clé `parallel` avait renvoyée (et toutes ses copies) vont automatiquement se transformer en la valeur de retour de la fonction. Si la fonction ne renvoie rien, c'est qu'elle renvoie en réalité `None`, et la promesse prendra la valeur de `None`. Seules les fonctions utilisateur peuvent être exécutées en parallèle (pas les méthodes, ni les fonctions built-in). Les fonctions built-in sont exécutées de manière atomique, et la fonction `setAtomicTime` permet de décider de la fréquence de passage d'un processus à l'autre.

La manière de gérer les processus sera détaillée dans la section dédiée.


= Partie 2 : Les expressions

Les expressions sont à la base de tout code Neon. Une expression est une construction syntaxique qui peut être évaluée en un objet. Toute expression est évaluée en l'un des objets détaillés dans la *Partie I*. Une expression sert à décrire un calcul. Les constantes littérales sont les expressions les plus basiques. Elles sont évaluées directement en leur objet associé. Une variable (désignée par son nom) est également une expression, et est évaluée en la valeur de la variable.

Des expressions plus compliquées peuvent être créées en combinant d'autres expressions à l'aide d'opérateurs et de fonctions.

Ces opérateurs et fonctions attendent des arguments (ou opérandes), qui, au moment de l'évaluation (ou de l'exécution) de la fonction ou de l'opérateur doivent être des objets. Pour envoyer un objet en tant qu'argument à une fonction ou un opérateur, il suffit d'écrire à la place de l'argument une expression qui s'évalue en l'objet voulu.

== 2.1 - Les opérateurs

Voici la liste de tous les opérateurs, et leur effet sur les objets de différents types :

=== 2.1.1 - Opérateurs binaires

*`+`* :\
Cet opérateur additionne deux nombres, concatène deux chaînes de caractères et concatène deux listes.

*`*`* :\
Cet opérateur effectue une multiplication entre deux nombres. Le produit entre un entier `n` et une liste va renvoyer la concaténation de cette liste avec elle-même `n` fois. Même chose pour le produit entre une chaîne de caractères et un entier.\
Le produit entre une liste (ou une chaîne de caractères) et un booléen a le même comportement que si le booléen était interprété comme 1 ou 0.

*`-`* :\
Cet opérateur effectue une soustraction entre deux nombres.\
La soustraction entre une chaîne de caractères et un entier `n` retire le `n` derniers caractères de la chaîne.\
La soustraction entre deux chaîne de caractères renvoie la première chaîne moins toutes les occurrences de la deuxième.

*`/`* :\
Bien que cela puisse paraître étonnant, cet opérateur effectue une simple division entre deux nombres, rien d'autre. Il est vrai qu'on aurait pu imaginer des utilisations de l'opérateur `/` pour toutes sortes de types, mais il faut parfois être raisonnable.

*`**`* :\
Opérateur exposant. `a ** b` renvoie `a` à la puissance `b`.

*`==`* :\
Opérateur de comparaison entre objets. Il renvoie True si et seulement si les deux objets sont égaux.

*`!=`* :\
Renvoie True si les deux objets sont différents.

*`>=`* :\
Comparaison entre deux nombres. Renvoie True si et seulement si l'opérande de gauche est supérieure ou égale à l'opérande de droite.

*`<=`* :\
Comparaison entre deux nombres. Renvoie True si et seulement si l'opérande de gauche est inférieure ou égale à l'opérande de droite.

*`<`* :\
Comparaison entre deux nombres. Renvoie True si et seulement si l'opérande de gauche est inférieure strictement à l'opérande de droite.

*`>`* :\
Comparaison entre deux nombres. Renvoie True si et seulement si l'opérande de gauche est supérieure strictement à l'opérande de droite.

*`and`* :\
Effectue un ET logique entre deux booléens. Cet opérateur est paresseux : si l'opérande de gauche s'évalue à `False`, l'opérande de droite n'est pas évalué et l'opérateur renvoie `False`.

*`or`* :\
Effectue un OU logique entre deux booléens. Cet opérateur est paresseux : si l'opérande de gauche s'évalue à `True`, l'opérande de droite n'est pas évalué et l'opérateur renvoie `True`.

*`xor`* :\
Effectue un OU EXCLUSIF logique entre deux booléens.

*`=>`* :\
Renvoie le résultat de l'implication logique entre deux booléens.

*`=`* :\
Opérateur d'affectation. L'opérande de gauche doit être soit une variable, soit un index de liste soit un attribut de container, l'opérande de droite peut être n'importe quel objet. Cet opérateur a pour effet de changer la valeur contenue dans l'opérande de gauche. Une affectation renvoie None.

Si l'opérande de gauche est une variable qui existe, son contenu sera simplement modifié. Sinon, si la variable n'existe pas encore, elle sera préalablement créée.

*`->`* :\
Cet opérateur est également un opérateur d'affectation. L'objet se place à gauche, et la variable/index de liste/attribut de container se place à droite. Contrairement à l'opérateur `=`, cet opérateur renvoie également une copie de la valeur assignée. Cela permet d'effectuer des affectations en chaîne : `6 -> a -> b -> c`

*`+=`* :\
Opérateur binaire ; `a += b` correspond exactement à `a = a + b`.

*`-=`* :\
Opérateur binaire ; `a += b` correspond exactement à `a = a - b`.

*`/=`* :\
Opérateur binaire ; `a += b` correspond exactement à `a = a / b`.

*`*=`* :\
Opérateur binaire ; `a += b` correspond exactement à `a = a * b`.

*`%`* :\
Cet opérateur est un opérateur binaire qui attend deux entiers. Il renvoie le reste de la division euclidienne entre l'opérande de gauche et l'opérande de droite.

*`//`* :\
Cet opérateur est un opérateur binaire qui attend deux entiers. Il renvoie le reste de la division euclidienne entre l'opérande de gauche et l'opérande de droite.

Bien que l'opérateur de division soit un simple opérateur entre nombres, j'ai quand même craqué pour cet opérateur. Lorsque l'une des opérandes est un entier `a` et que l'autre opérande est une chaîne de caractères de taille `n`, l'opérateur renvoie une nouvelle chaîne composée des `n//a` premiers caractères de la chaîne originale.

*`<-`* :\
Cet opérateur attend une chaîne de caractères à gauche et un objet quelconque à droite. L'opérateur crée une variable avec le nom indiqué à gauche, même si le nom n'est pas un nom d'identificateur valide, et lui affecte l'objet indiqué à droite. Si la variable existe déjà, l'opérateur change simplement sa valeur avec le nouvel objet. L'opérateur renvoie également une copie de l'objet.

*`EE`* :\
Cet opérateur correspond à la mise à la puissance de dix. `a EE b` est égal à `a * 10 ** b`.

*`in`* :\
Cet opérateur attend un objet quelconque à gauche et une liste à droite, et renvoie True si et seulement si la liste contient l'opérande de gauche.

*`<->`* :\
Cet opérateur attend à gauche et à droite des variables/index de liste/attributs de containers, et échange leur valeur.

=== 2.1.2 - Opérateurs unaires

*`-`* : Cet opérateur peut également être utilisé comme opérateur unaire. L'opérande se place à droite. Renvoie l'opposé d'un nombre.

*`del`* :\
Cet opérateur prend une variable à droite, et supprime cette variable.

*`@`* :\
Cet opérateur est un opérateur unaire, et attend une chaîne de caractères à droite. Il renvoie la valeur de la variable ayant pour nom la chaîne de caractères indiquée comme opérande.

*`&`* :\
Cet opérateur est un opérateur unaire qui attend une variable à droite. L'opérateur renvoie le nom de la variable.

*`++`* :\
Opérateur unaire, attend une variable, un index de liste ou un attribut de container à gauche. `a++` correspond exactement à `a += 1`.

*`--`* :\
Opérateur unaire, attend une variable, un index de liste ou un attribut de container à gauche. `a--` correspond exactement à `a -= 1`.

*`not`* :\
Opérateur unaire, opérande à droite. Effectue une négation logique.


=== 2.1.3 Opérateurs spéciaux


*`parallel`* :\
Cet opérateur est un opérateur spécial qui ne prend pas en entrée un objet mais une expression. l'opérateur `parallel` doit recevoir à droite un appel de fonction utilisateur, et lance cet appel de fonction en parallèle, dans un nouveau fil d'exécution.

Exemple : `parallel f(arg1, arg2, arg3)` lance la fonction `f` en parallèle sur les arguments `arg1`, `arg2` et `arg3`. De plus amples explications seront données dans la section dédiée au multitâches.

*`.`* : Cet opérateur est également un opérateur spécial qui attend à gauche un objet et à droite un appel de fonction ou de méthode. Écrire `objet.fonction(arg1, arg2)` correspond exactement à `fonction(objet, arg1, arg2)`

*`>>`* : Cet opérateur est également un opérateur spécial qui attend à gauche un container et à droite un nom de champ de container.

=== 2.1.4 Les priorités opératoires

Les priorités de tous les opérateurs sont réparties parmi 9 niveaux de priorité de 0 à 8. Le niveau le plus élevé correspond à l'opérateur le moins prioritaire, celui qui sera évalué en dernier.

*Niveau 0 :* `>>`\
*Niveau 1 :* `-` (unaire)\
*Niveau 2 :* `&`, `@`, `.`, \
*Niveau 3 :* `**`, `++`, `--`, `EE`\
*Niveau 4 :* `*`, `/`, `%`, `//`\
*Niveau 5 :* `+`, `-`\
*Niveau 6 :* `==`, `!=`, `<=`, `>=`, `<`, `>`, `in`\
*Niveau 7 :* `and`, `or`, `xor`, `not`, `=>`, `parallel`\
*Niveau 8 :* `=`, `+=`, `-=`, `*=`, `/=`, `<-`, `->`, `del`, `<->`\


== 2.2 - Les fonctions

Les fonctions sont des objets qui produisent un comportement, souvent une sortie, et généralement à partir d'arguments. Si un objet `myFunction` est une fonction (que ce soit une fonction utilisateur ou une fonction built-in), la fonction peut être exécutée sur les arguments `a1`, ..., `an` via la syntaxe suivante : `myFunction(a1, a2, ..., an)`. Lorsque cette expression est évaluée, l'objet renvoyé est la sortie produite par la fonction. Certaines fonctions ne produisent pas de sortie (ou du moins ce n'est pas leur but), ces fonctions renvoient `None`.

Les fonctions peuvent évidemment être composées entre elles, et avec des opérateurs.


= Partie 3 : Structure d'un programme

Comme dit dans l'introduction, Neon est un langage qui permet de décrire des programmes de calcul séquentiels. Cela signifie que le langage Neon permet de décrire l'exécution d'une succession de tâches simples. On appelle également cela un programme.

Un programme est un assemblage (concaténation et composition) de blocs de code. Un bloc de code représente une tâche à exécuter. Il y a beaucoup de types de blocs de code différents, chacun avec ses particularités.

Le bloc de code le plus basique est l'expression. Considérée en tant que bloc de code, une expression n'est pas importante pour le résultat qu'elle renvoie, mais les actions qu'elle produit pendant son évaluation. Quand on écrit une expression en tant que bloc de code, l'expression est évaluée, mais son résultat final est ignoré. Si on écrit le programme suivant, constitué d'un unique bloc de code qui est une expression :

`2 + 3`

la valeur 5 sera bel et bien créée, mais ignorée et supprimée. Même si cette expression est un bloc de code valide, elle ne sert à rien en tant que bloc de code car elle ne produit aucun comportement. L'état du programme (les variables, la mémoire) est le même après son évaluation et avant son évaluation.

En revanche, si on écrit le programme suivant, encore une fois constitué d'un unique bloc de code qui est une expression :

`maVariable = 2 + 3`

la valeur créée par l'expression 2 + 3 aura été utilisée puisque la variable `maVariable` la contient désormais. En revanche, la valeur renvoyée par l'expression entière `maVariable = 2 + 3` (`None`) est encore une fois ignorée.

Un programme Neon est une succession et une composition de blocs de code. Pour exécuter deux blocs de code à la suite, il faut soit les séparer d'un retour à la ligne, soit les séparer d'un point virgule `;`. Exemple d'un programme composé d'une succession de tâches simples :

```
maVariable = 2 + 3
maVariable *= 7
maVariable --
print(maVariable)
```


À partir de ces blocs de code basiques, il est possible d'en construire de plus complexes.

== 3.1 - Les blocs conditionnels

Les blocs conditionnels servent à exécuter du code à certaines conditions. Alors que les blocs de code constitués d'expressions à la suite s'exécutent quoi qu'il arrive, le code à l'intérieur de blocs conditionnels s'exécute uniquement si une certaine condition spécifiée est vraie.

Un bloc conditionnel complet s'écrit de la manière suivante :

```
if (expression booléenne) then
    code à exécuter
elif (autre expression booléenne) then
    autre code à exécuter
⁝ nombre arbitraire de blocs elif
elif (autre expression booléenne) then
    autre code à exécuter
else
    autre code à exécuter
end
```

Voici comment il est interprété : les expressions booléennes vont être testées une à une dans l'ordre. D'abord celle du `if`, puis celle du premier `elif`, etc. Pour la première de ces expressions qui s'évalue à `True`, le code juste en dessous est exécuté, et on sort du bloc conditionnel.

Si aucune des conditions ne s'évalue à `True` (donc toutes les conditions s'évaluent à `False`), le code à l'intérieur du `else` est exécuté.

Comme dit plus haut, ceci est un bloc conditionnel complet.

Un bloc conditionnel valide doit forcément commencer par un bloc `if`, puis peut contenir un nombre quelconque de bloc `elif` (y compris zéro), puis peut contenir un bloc `else`.

Les blocs conditionnels suivants sont valides :

```
if (1+1 == 2) then
    print("vrai")
end
```

\

```
if (1+1 == 0) then
    print("vrai")
else
    print("faux")
end
```

== 3.2 - Les boucles `while`

Les boucles sont des structures qui répètent l'exécution d'un certain bloc de code en fonction de différents paramètres.\
Une boucle `while` répète l'exécution d'un bloc de code tant qu'une certaine condition est vraie. Voici la syntaxe d'une boucle `while` :

```
while (expression booléenne) do
    code à exécuter
end
```

Il est important de noter que la condition est toujours testée *avant* l'exécution du code dans le bloc.

On en déduit plusieurs faits :\
- Le code à l'intérieur du bloc peut toujours supposer vraie la condition\
- Si la condition est fausse dès l'entrée dans la boucle, le code ne sera jamais exécuté


== 3.3 - Les boucles `for`

Les boucles `for` permettent d'exécuter un bloc de code en faisant varier la valeur d'un entier, appelé variant, sur un certain intervalle.

La syntaxe complète d'une boucle `for` est :

```
for (variant, début, fin, pas) do
    code à exécuter
end
```

Les champs `début`, `fin` et `pas` doivent être des expressions d'évaluant en nombres entiers, et le champ `variant` doit être une variable.

À l'entrée dans la boucle `for`, la variable utilisée pour le variant va passer en variable locale à la boucle. C'est-à-dire que la valeur précédente de la variable ne sera plus accessible, et la variable aura la nouvelle valeur utilisée dans la boucle `for`. Quand la boucle `for` sera terminée, la valeur précédente du variant sera restaurée. Si la variable utilisée pour le variant n'était pas définie avant la boucle `for`, elle redeviendra indéfinie. La section 4.3 détaille plus les variables locales et globales.

Le comportement de la boucle `for` est simple : le code à exécuter à l'intérieur du bloc sera exécuté pour chaque valeur différente du variant, à commencer par `début`, et jusqu'à `fin` *exclu*, en incrémentant la variable de `pas` à chaque tour de boucle.

La variable utilisée pour le variant est réactualisée à chaque tour de boucle, ce qui signifie que même si elle est modifiée à l'intérieur du code, elle sera restaurée comme si de rien n'était à la fin du tour de boucle, et la boucle ne sera pas impactée.

Il n'est pas toujours obligatoire de spécifier le pas et la valeur de début de la boucle. Il existe deux autres manières de définir une boucle `for` :

```
for (variant, début, fin) do
    code à exécuter
end
```

Dans ce cas, le pas par défaut est de 1.

```
for (variant, fin) do
    code à exécuter
end
```

Dans ce cas, le pas par défaut est de 1 et la valeur de début de la boucle est 0.

== 3.4 - Les boucles `foreach`

Les boucles `foreach` sont basées sur le même principe que les boucles `for` : elles permettent d'exécuter un bloc de code pour chaque valeur d'une liste ou chaque caractère d'une chaîne de caractères, dans l'ordre croissant des indices.

Voici la syntaxe :

```
foreach (element, iterable) do
    code à exécuter
end
```

Comme pour les boucles `for`, la variable `element` est locale à la boucle et est restaurée à chaque nouveau tour de boucle.


== 3.5 - Instructions de contrôle

Les instructions de contrôle sont à elles seules des blocs de code, au même titre que les expressions. Elles doivent donc être séparées d'autres blocs de code par un retour à la ligne ou un point virgule.

=== 3.5.1 - Instruction `break`

L'instruction `break` doit être utilisée exclusivement dans les boucles. Lorsque'une instruction `break` est rencontrée, l'exécution quitte immédiatement la boucle la plus intérieure dans laquelle se situe le `break`, et continue juste après ladite boucle.


=== 3.5.2 - Instruction `continue`

L'instruction `continue` doit être utilisée exclusivement dans les boucles. Lorsque'une instruction `continue` est rencontrée, l'exécution saute au début de la boucle la plus intérieure dans laquelle se situe le `continue`. Dans cette opération, la condition de la boucle est revérifiée, donc si la condition était fausse au moment du `continue`, celui-ci aura le même effet qu'un `break`.

=== 3.5.3 - Instruction `pass`

L'instruction `pass` ne sert strictement à rien.

== 3.6 - La gestion d'erreurs

Neon dispose d'un système de gestion d'erreurs. Lorsqu'une erreur survient, une exception est déclenchée. Il est possible de détecter le déclenchement d'exceptions grâce aux blocs `try ... except`, et d'exécuter du code en fonction de l'exception déclenchée.

Les exceptions existant par défaut sont détaillées dans la section 1.2.7.

Il est possible de manipuler les exceptions en tant qu'objet, et d'en créer à l'aide de la fonction `createException`.

Il est également possible de déclencher volontairement des exceptions grâce à la fonction `raise` (voir comment l'utiliser dans la section 1.2.8).

Voici comment écrire un bloc `try ... except` :

```
try
    code à exécuter
except (Exception1, Exception2, ...) do
    code à exécuter
⁝ nombre arbitraire de blocs except
except (Exception3, ...) do
    code à exécuter
end
```

Lorsqu'on exécute un bloc `try ... except`, le code à l'intérieur du `try` est d'abord exécuté. Si tout se passe bien (c'est-à-dire aucune exception n'est déclenchée), le bloc `try ... except` a terminé quand le code à l'intérieur du `try` a fini.

En revanche, si une exception est levée pendant l'exécution du code à l'intérieur du `try`, l'exécution du code sera stoppée au moment de l'exception, et le premier bloc `except` dont l'une des exceptions indiquées en tête correspond à l'exception lancée sera exécuté, entièrement et normalement. Quand ce bloc `except` a terminé, l'exécution du bloc `try ... except` est également terminée. 

À noter que Exception, Exception2, ... doivent être directement des objets de type Exception, pas des expressions s'évaluant en exceptions.

Le nombre d'exceptions que l'on peut spécifier pour un bloc `except` n'a aucune limite, et peut même être nul. Dans le cas où l'on ne spécifie aucune exception entre les parenthèses, le bloc `except` est exécuté quelle que soit l'exception levée.


= Partie 4 - Définition de fonctions et méthodes


== 4.1 Définition de procédures


== 4.2 Fonctions basiques


== 4.3 Variables locales et globales


== 4.4 Méthodes avancées de passage d'arguments

=== 4.4.1 - Passage d'arguments dans le désordre

=== 4.4.2 - Arguments optionnels

=== 4.4.3 - Nombre illimité d'arguments

=== 4.4.4 - Arguments vraiment optionnels

== 4.5 - Programmation d'ordre supérieur, clôtures

== 4.6 - Programmation modulaire


= Partie 5 - Programmation concurrente

== 5.1 - Vision par processus

Dans les sections précédentes, quand on décrivait le comportement du programme, on en parlait comme d'une sorte de tête de lecture parcourant le programme et exécutant les instructions qu'elle rencontre. Cette vision correspond relativement bien au réel déroulement de l'interprétation d'un programme. Cette tête de lecture dont on parle implicitement quand on décrit le comportement d'un programme est en quelque sorte la personnification de l'exécution d'un programme.

À partir de maintenant et grâce à la programmation concurrente, nous allons créer des programmes possédant plusieurs têtes de lecture différentes, c'est-à-dire exécutant simultanément différentes portions d'un programme. On dira alors qu'un programme possède différents fils d'exécution.

Avant d'aller plus loin, il est essentiel de comprendre comment une telle chose est possible, et surtout comment elle est réalisée dans Neon.

De nombreux ordinateurs ou appareils électroniques ne disposent que d'un unique processeur avec un unique coeur. Cependant, la quasi totalité des systèmes d'exploitation nécessitent la capacité d'exécuter plusieurs programmes en même temps. Ces systèmes d'exploitation gèrent alors ce qu'on appelle de l'entrelacement entre processus. Le processeur exécute quelques instructions d'un processus, puis quelques instructions d'un autre processus, puis revient au premier processus, etc. Il alterne l'exécution entre tous les processus. Cette gymnastique donne l'illusion que ces processus sont exécutés en même temps.

Neon ayant pour but de pouvoir être exécuté sur n'importe quelle plateforme, il n'effectue aucune supposition sur le système d'exploitation. Ainsi, il ne peut pas utiliser de telles fonctionnalités d'entrelacement.

C'est pourquoi l'interpréteur Neon gère lui-même l'entrelacement entre ses propres processus. Cette caractéristique possède des avantages, mais également des inconvénients.

En effet, l'avantage n°1 est la possibilité d'exécuter plusieurs programmes à la fois sur des appareils pour lesquels c'est impossible autrement. Le fait que l'interpréteur ait directement le contrôle sur l'entrelacement permet également de mieux contrôler certains paramètres comme le temps passé à exécuter un processus avant de passer au suivant.

En revanche, sur des appareils disposant de plusieurs coeurs d'exécution simultanée, l'interpréteur Neon sera incapable de distribuer les tâches sur ces différents coeurs.

Ainsi, un programme Neon écrit de manière concurrente ne sera jamais plus rapide que sa version écrite entièrement séquentiellement. Les fonctionnalités concurrentes de Neon ne doivent être utilisées que lorsque cela facilite l'écriture du programme.

== 5.2 - L'opérateur `parallel`

== 5.3 - Le retour de processus via les promesses

== 5.4 - Variables locales aux processus

== 5.5 - Blocs atomiques

== 5.6 - Attente passive

== 5.7 - Fonctions du système d'entrelacement

Le passage d'un processus à l'autre par l'interpréteur Neon n'est pas effectué à n'importe quel moment. Il est effectué par une fonction nommée `neon_interp_yield`. Cette fonction est appelée uniquement à deux endroits : lors d'un appel à la fonction d'évaluation de Neon et lors d'un appel à la fonction d'exécution d'un bloc de code Neon.

À chaque fois que `neon_interp_yield` est appelée, un compteur est décrémenté. Tant que ce compteur n'atteint pas zéro, la fonction ne fait rien d'autre, mais lorsqu'il devient nul, `neon_interp_yield` va mettre en pause le processus actuel et relancer un autre processus.


=== 5.7.1 - La fonction `setAtomicTime`

Le nombre de fois que `neon_interp_yield` décrémente le compteur avant de passer au processus suivant est stocké dans la variable `ATOMIC_TIME`. C'est la valeur par défaut du compteur atomique d'un processus.
Par défaut, `ATOMIC_TIME` vaut 1500, c'est-à-dire que pour chaque processus, au bout de 1500 appels à la fonction `neon_interp_yield`, celle-ci passera au processus suivant.

La fonction `setAtomicTime` permet de modifier cette valeur (1 est la plus petite valeur possible).


= Partie 6 - Fonctionnalités supplémentaires

== 6.1 - Mot-clé `import`

== 6.2 - Arguments de programme

== 6.3 - Variables prédéfinies

=== 6.3.1 - La variable `__name__`

=== 6.3.2 - La variable `__platform__`

=== 6.3.3 - La variable `__version__`

== 6.4 - La variable spéciale `Ans`