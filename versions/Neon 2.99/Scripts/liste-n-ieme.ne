print("Quel algorithme voulez-vous utiliser ?")
print("1 - Algo naif")
print("2 - Algo rapide")
algo = nbr(input("-> "))

nb=nbr(input("Votre nombre : "))

method list_create(list, number) {
    if (number == 0) {
        return (None)
    }
    else
    {
        append(list, [])
        list_create(list[0], number-1)
    }
}

# principal

l = []

if (algo == 1)
{
    for (i, 0, nb)
    {
        l = [l]
    }
}
else
{
    list_create(l, nb)
}

print(l)
input()