max = 10000
est_premier = False

premiers = []


for (n, 2, max+1) {
    # on va vérifier que n est un nombre premier
    est_premier = True
    for (k, 2, n) {
        if (n % k == 0) {
            est_premier = False
            k = n-1
        }
    }
    
    if (est_premier) {
        append(premiers, n)
    }
}

print(premiers)
