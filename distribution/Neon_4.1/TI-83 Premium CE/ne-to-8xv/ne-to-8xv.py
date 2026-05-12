import sys
import os
import subprocess

if len(sys.argv) < 3:
    # demande a l'utilisateur les noms des fichiers
    entree = input("Nom du fichier à convertir : ")
    sortie = input("Nom du fichier de sortie (ne contenant que des chiffres ou des lettres, commencant par une lettre et contenant maximum 8 caractères) : ")
elif len(sys.argv) < 2:
    entree = sys.argv[1]
    sortie = input("Nom du fichier de sortie (ne contenant que des chiffres ou des lettres, commencant par une lettre et contenant maximum 8 caractères) : ")
else:
    entree = sys.argv[1]
    sortie = sys.argv[2]


def enlever(texte):
    ac=['Ã\xa0','Ã¢','Ã©','Ã¨','Ãª','Ã«','Ã¯','Ã®','Ã¶','Ã´','Ã»','Ã¼','Ã¹','Ã§','Ã‚','ÃŠ','ÃŽ','Ã”','Ã›','Ã„','Ã‹','Ã–','Ãœ','Ã€','Ã†','Ã¦','Ã‡','Ã‰','Ãˆ','Å’','Å“','Ã™',"â","ä","à","é","è","ë","ê","ï","î","ô","ö","ü","û","ù","Ã±","Ã£","Ãµ","â","ä","à","é","è","ë","ê","ï","î","ô","ö","ü","û","ù","Â°"]
    l=["a","a","e","e","e","e","i","i","o","o","u","u","u","c","A","E","I","O","U","A","E","O","U","A","AE","ae","C","E","E","OE","oe","U","a","a","a","e","e","e","e","i","i","o","o","u","u","u","n","a","o","a","a","a","e","e","e","e","i","i","o","o","u","u","u","°"]
    
    for i in range(len(ac)):
        texte = texte.replace(ac[i], l[i])

        
    chars=""
    for x in range (len(texte)):
        chars=chars+texte[x]
    
    return chars

# accents
fichier = open(entree, 'rt')
txt = fichier.read()
fichier.close()

txt = enlever(txt)

fichier = open("out.ne", 'w')
fichier.write(txt)
fichier.close()



# les 0D
fichier = open("out.ne", "rb")
data = list(fichier.read())
data = bytearray(data)
while data.count(ord('\r')) > 0:
    data.remove(ord('\r'))
fichier.close()



fichier = open("out.ne", "wb+")
fichier.write(data)
fichier.close()



if sys.platform == "linux":
    exec_file = "./convbin.elf"
else:
    exec_file = "\\.convbin.exe"

# Commande à exécuter
commande = [exec_file, "-i", "out.ne", "-o", sortie + ".8xv", "-j", "bin", "-p", "zx7", "-k", "8xv", "-n", sortie]

try:
    # Exécute la commande
    subprocess.run(commande, check=True)
    print("La commande a été exécutée avec succès.")
except subprocess.CalledProcessError as e:
    print(f"Erreur lors de l'exécution de la commande : {e}")
except FileNotFoundError:
    print("Le programme 'convbin' n'a pas été trouvé. Assurez-vous qu'il est installé et que son chemin d'accès est correct.")


os.remove("out.ne")
