# programme Neon
print ("Bienvenue dans la calculatrice.\nEntrez 'exit' pour sortir")

calcul = input("Votre calcul : ")

while (calcul != 'exit')
{
    calcul = eval(calcul)
    
    print(calcul)
    
    calcul = input("Votre calcul : ")
}

print("Au revoir !")