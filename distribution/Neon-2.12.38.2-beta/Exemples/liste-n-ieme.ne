print("Quel algorithme voulez-vous utiliser ?")
print("1 - Algo naif")
print("2 - Algo rapide")
algo = nbr(input("-> "))

nb=nbr(input("Votre nombre : "))

method list_create(list, number) do
    if (number == 0) then
        return (None)
    else
        list.append([])
        list[0].list_create(number-1)
    end
end

# principal

l = []

if (algo == 1) then
    for (i, 0, nb) do
        l = [l]
    end
else
    l.list_create(nb)
end

print(l)
input()