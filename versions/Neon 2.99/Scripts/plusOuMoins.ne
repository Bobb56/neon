clear()
nombre_max = 1000
print("Bonjour, et bienvenue dans le jeu du plus ou moins !\nNous avons choisi un nombre entre 0 et " + str(nombre_max) + ". Essayez de le deviner.")

entree = ""
nombre_essais = 0
nombre_choisi = randint(nombre_max + 1) # nombre que l'utilisateur va chercher

quitter = True

while (quitter) # boucle principale
{
    
    entree = input("A quel nombre pensez-vous ? ")


    nombre_essais++ # dès que l'on a entré un nombre, on a fait un essai
    
    if (nbr(entree) == nombre_choisi)
    {
        print("Bien joué, vous avez gagné en", nombre_essais, "essais.")
        rejouer = input("Voulez-vous rejouer ? (oui/non) ")
        if (rejouer == "non")
        {
            quitter = False
        }
        else
        {
            nombre_choisi = randint(1001)
            entree = str(nombre_choisi) # pour éviter les interférences avec les if qui suivent
            nombre_essais = 0
            clear()
        }
    }


    if (nbr(entree) > nombre_choisi and quitter)
    {
        print("Moins")
    }


    if (nbr(entree) < nombre_choisi and quitter)
    {
        print("Plus")
    }

    if (nbr(entree) > nombre_max and quitter)
    {
        print("Nous rappelons que le nombre ne peut pas être supérieur à 1000")
    }
    
}