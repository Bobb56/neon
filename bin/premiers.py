max = 20000 # int(input ("Jusqu'ou voulez-vous que l'on detecte les nombres premiers ? "))

est_premier = False
premiers = []

for n in range (2, max+1):
    # on va vérifier que n est un nombre premier
    est_premier = True
    for k in range(2, n):
        if n % k == 0:
            est_premier = False
            break
    
    if est_premier:
        premiers.append(n)

print(premiers)


