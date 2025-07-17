t1 = time()

i = nbr(input("Entrez un nombre, s'il vous plaît : "))

while (i < Infinity)
{
	i *= 1.001
	print(i)
	clear()
}

print("Votre nombre a mis", time() - t1, "secondes à être égal à l'infini.")
