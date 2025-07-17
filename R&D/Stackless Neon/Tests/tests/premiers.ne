max = 21

premiers = []
est_premier = False

for (n, 2, max+1) do
    # on va vérifier que n est un nombre premier
    est_premier = True
    for (k, 2, n) do
        if (n % k == 0) then
            k = n-1 # pour sortir de la boucle, car n n'est pas premier
            est_premier = False
        end
    end
    
    if (est_premier) then
        append(premiers, n)
    end
end


assert(premiers == [2, 3, 5, 7, 11, 13, 17, 19])

