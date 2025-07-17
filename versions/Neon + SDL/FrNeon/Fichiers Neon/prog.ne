# implémentation du tri selection en Neon



tab = eval(input("Entrez une liste : ")) # demande d'une liste

for (i, 0, len(tab)) # parcourt la liste
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
   #échange le min trouvé avec l'élément en place i
   tmp = tab[i]
   tab[i] = tab[min]
   tab[min] = tmp
}

print(tab)
input()
