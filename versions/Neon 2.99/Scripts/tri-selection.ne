tab = [2,1] # eval(input("Entrez une liste de la forme [1,2,3] : "))
print("Debut du tri...")

for (i, 0, len(tab))
{
  # Trouver le min
   min = i
   for (j, i+1, len(tab))
   {
       if (tab[min] > tab[j])
       {
           min = j
       }
   }
   tmp = tab[i]
   tab[i] = tab[min]
   tab[min] = tmp
}

print(tab)