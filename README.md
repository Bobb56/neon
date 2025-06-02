# Neon

**Neon** est un langage de programmation interprété, impératif et de haut niveau, conçu pour être simple, lisible et nativement concurrent. Il est écrit en C, ce qui le rend léger, portable et facilement intégrable dans divers projets.

## ✨ Caractéristiques principales

- **Interprété** : Exécution ligne par ligne via un interpréteur dédié.
- **Syntaxe claire et lisible** : Inspirée de Python, facilitant l'apprentissage.
- **Concurrence native** : Gestion de processus parallèles avec les mots-clés `parallèle` et `await`.
- **Portable** : Fonctionne de manière identique sur tous les systèmes d'exploitation.
- **Open-source** : Code source accessible et modifiable par tous.
- **Terminal interactif** : Interface colorée et intuitive.
- **Interpréteur léger** : Ne nécessite pas d'installation complexe.

## 🧱 Types de base

Neon propose quatre types de base :

- `Number` : Nombres entiers ou réels.
- `String` : Chaînes de caractères.
- `Bool` : Booléens (`True` ou `False`).
- `NoneType` : Type représentant l'absence de valeur (`None`).

## 🧰 Variables

- Déclaration simple : `maVariable = 5`
- Modification : `maVariable = 10`
- Suppression : `del maVariable`

Les noms de variables doivent commencer par une lettre ou un underscore (`_`) et peuvent contenir des lettres, des chiffres, des underscores ou des apostrophes.

## 🔁 Contrôle de flux

### Blocs conditionnels

```neon
if (condition) then
    instructions
elif (autre_condition) then
    autres_instructions
else
    instructions_par_defaut
end
```

### Boucles

- **While** : Exécute un bloc tant qu'une condition est vraie.

```neon
while (condition) do
    instructions
end
```

- **For** : Itère sur une plage de valeurs.

```neon
for (i, debut, fin) do
    instructions
end
```

Note : La variable `i` est locale à la boucle et sa valeur initiale est restaurée après la boucle.

## 🧮 Fonctions intégrées

Neon offre plusieurs fonctions intégrées :

- `print(...)` : Affiche les arguments.
- `input(prompt)` : Affiche un message et lit l'entrée utilisateur.
- `len(objet)` : Renvoie la longueur d'une chaîne ou d'une liste.
- `type(objet)` : Renvoie le type de l'objet.
- `nbr(chaine)` : Convertit une chaîne en nombre.
- `str(nombre)` : Convertit un nombre en chaîne.
- `reverse(chaine)` : Inverse une chaîne de caractères.
- `randint(n)` : Renvoie un entier aléatoire entre 0 et n-1.
- `time()` : Renvoie le nombre de secondes écoulées depuis 1970.
- `failwith(message)` : Affiche un message d'erreur et termine le programme.
- `eval(chaine)` : Évalue une expression et renvoie le résultat.

## 🧵 Programmation concurrente

- **Lancement parallèle** : Utilisez `parallèle` pour exécuter des fonctions en parallèle.
- **Promises** : Le type `Promise` permet de gérer les retours de processus.
- **Await** : Le mot-clé `await` permet d'attendre passivement un résultat.
- **Blocs atomiques** : Garantissent l'exécution sans interruption de sections critiques.
- **Contrôle de l'entrelacement** : `setAtomicTime` permet de définir le temps d'entrelacement entre processus.

## 📥 Installation

Neon est disponible ici : [Téléchargements](https://langage-neon.raphaael.fr/t%C3%A9l%C3%A9chargements)

Aucune installation complexe n'est requise. Il suffit de télécharger l'interpréteur adapté à votre système.

## 💻 Tester en ligne

Vous pouvez tester Neon directement via Replit : [Tester en ligne](https://langage-neon.raphaael.fr/tester-en-ligne)

## 📚 Documentation

Une documentation complète est disponible ici : [Apprendre Neon](https://langage-neon.raphaael.fr/apprendre-neon)

## 🆕 Nouveautés

Parmi les nouveautés récentes de la version 3.0-beta :

- Ajout de l'exception `KeyboardInterrupt`
- Opérateur de swap `<->`
- Fonctions à arguments variadiques
- Arguments nommés dans le désordre
- Documentation via `setFunctionDoc`
- Variables locales dans les blocs conditionnels
- Blocs d'instructions atomiques

Plus de détails : [Nouveautés](https://langage-neon.raphaael.fr/nouveaut%C3%A9s)

https://langage-neon.raphaael.fr

Doc et plus : http://ti-pla.net/a3236802

Serveur Discord : https://discord.gg/wkBdK35w2a
