# programme qui affiche une table de multiplication

nb = nbr(input("Quelle table de multiplication voulez-vous que nous affichions ? "))
max = nbr(input("Jusqu'ou voulez-vous que nous allions ? "))

print("Table de multiplication de", nb, ":")

for (n,0,max+1) {
  print(nb, "x", n, "=",nb*n)
}

input("Terminé ! Appuyez sur [Entrée].\n")