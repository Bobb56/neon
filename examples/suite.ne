# implémente une suite de nombres


function sum(liste) do
    s = 0
    foreach (element, liste) do
        s += element
    end
    return (s)
end

function next(liste) do
    local(i)
    nliste = []
    i = 0
    while (i < len(liste)) do
        value = liste[i]
        count = 0
        while (i < len(liste) and liste[i] == value) do
            i++
            count++
        end
        nliste += [count, value]
    end
    return (nliste)
end

function suite(ntermes) do
    local(i)
    inter = [[1]]
    valeurs = []
    for (i, 0, ntermes) do
        inter.append(next(inter[i]))
        valeurs.append(sum(inter[i]))
    end
    return (valeurs)
end

print(suite(20))