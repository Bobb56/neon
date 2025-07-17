max = 20000 # nbr(input ("Jusqu'ou voulez-vous que l'on detecte les nombres premiers ? "))

est_premier = False

premiers = []


for (n, 2, max+1) do
    # on va vérifier que n est un nombre premier
    est_premier = True
    for (k, 2, n) do
        if (n % k == 0) then
            est_premier = False
            break
        end
    end
    
    if (est_premier) then
        premiers.append(n)
    end
end

print(premiers)
