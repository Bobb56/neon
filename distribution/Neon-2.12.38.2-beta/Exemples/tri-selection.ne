liste = eval(input("Entrez une liste de la forme [1,2,3] :"))
print("Debut du tri...")

method triSelection(tab) do
  for (i, 0, len(tab)) do
    # Trouver le min
    min = i
    for (j, i+1, len(tab)) do
        if (tab[min] > tab[j]) then
            min = j
        end
    end
    tmp = tab[i]
    tab[i] = tab[min]
    tab[min] = tmp
  end
end

triSelection(liste)
print(liste)