# Interpréteur Neon

## Introduction

Vous venez de télécharger l'interpréteur de Neon, un langage de programmation interprété multiplateforme et multitâches. Vous trouverez dans ce dossier un documentation complète, et des fichiers d'installation sur Linux, Windows et TI-83 Premium CE [Edition Python].

Pour des renseignements : https://langage-neon.raphaael.fr

## Instructions d'installation de Neon

Avec ces fichiers, vous disposez des fichiers exécutables pour : Linux, Windows et la TI-83 Premium CE / TI-84 Plus CE.
De plus, vous avez également des fichiers d'installation pour Linux et Windows.

Les fichiers exécutables de Neon sont faits pour une utilisation portable, sinon, je vous conseille de l'installer.

### Windows

Pour installer Neon sur Windows, il suffit de lancer les fichiers d'installation et de suivre les instructions. Neon peut ensuite être lancé depuis la console en tapant neon pour peu d'avoir ajouté le chemin de l'exécutable dans le PATH, ou bien lancé comme une application quelconque.

### Linux

Pour l'installer sur Linux, vous devez avoir au préalable installé le gestionnaire de paquets Snap. Ensuite, lancez simplement la commande : sudo snap install neon-interpreter
Neon va être automatiquement téléchargé et installé.
Alternativement, pour l'installer depuis le fichier `neon-interpreter_4.0_amd64.snap`, tapez la commande suivante dans le même dossier que le fichier d'installation : `sudo snap install --dangerous neon-interpreter_4.0_amd64.snap`
L'option `--dangerous` sert à indiquer à Snap qu'on est conscient des risques d'installer un paquet depuis un fichier (les risques sont qu'il est plus facile d'installer des applications malveillantes en local que depuis les dépôts Snap officiels).

Sous Linux, la commande pour lancer neon sera neon-interpreter. Pour pouvoir lancer Neon en tapant neon dans la console, il faut paramétrer Snap avec la commande suivante : `sudo snap alias neon-interpreter neon`

Pour le désinstaller, lancez la commande suivante : `sudo snap remove neon-interpreter`

### TI-83 Premium CE [Edition Python]

Pour utiliser Neon sur la calculatrice :
Transférez le fichier NEON.8xp sur votre calculatrice.
Pour créer et éditer des programmes Neon, je vous conseille d'utiliser l'application Python. Neon est compatible avec les fichiers créés avec l'application python.

Vous avez deux modes d'accès à Neon : par la console, ou en exécutant un fichier.
Pour aller dans la console, il faut mettre "" dans Ans (ou Rép). Pour ce faire, allez dans l'écran l'accueil (l'écran de calcul de la TI), et tapez "", puis [ENTREE].

Si vous voulez exécuter un fichier (un AppVar), il faut mettre le nom de l'appvar dans Ans (ou Rép), de la même manière : "EXEMPLE" puis [ENTREE]. Le nom du fichier doit respecter les critères de noms des programmes (que des majuscules ou des chiffres, et commençant par une majuscule.

Si vous voulez convertir un fichier .ne en AppVar exécutable par Neon, vous allez avoir besoin des trois fichiers conv.py, CONVERTISSEUR.bat, et convbin.exe
Mettez dans le même dossier le fichier .ne que vous voulez convertir, puis exécutez CONVERTISSEUR.bat. A la fin, vous aurez un fichier .8xv à transférer à la calculatrice.
