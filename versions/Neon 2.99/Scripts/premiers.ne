time1 = time()

max = 100 # nbr(input ("Jusqu'ou voulez-vous que l'on detecte les nombres premiers ? "))
premiers = []
est_premier = False

for (n, 2, max+1)
{
    # on va vérifier que n est un nombre premier
    est_premier = True
    for (k, 2, n)
    {
        if (n % k == 0)
        {
            k = n-1 # pour sortir de la boucle, car n n'est pas premier
            est_premier = False
        }
    }
    
    if (est_premier)
    {
        append(premiers, n)
    }
}

print("Temps d'execution :", time() - time1, "secondes.")
print(premiers)