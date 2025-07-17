tab = eval(input("Entrez votre liste : "))

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