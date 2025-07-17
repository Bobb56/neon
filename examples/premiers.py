max = int(input ("Jusqu'ou voulez-vous que l'on detecte les nombres premiers ? "))

est_premier = False

for n in range (2, max+1):
    # on va vérifier que n est un nombre premier
    est_premier = True
    for k in range(2, n):
        if n % k == 0:
            k = n-1 # pour sortir de la boucle, car n n'est pas premier
            est_premier = False
    
    if est_premier:
        print(n)


