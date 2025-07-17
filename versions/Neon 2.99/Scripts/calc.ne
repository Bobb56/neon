# programme Neon
if (len(__args__) == 0)
{
    print ("Bienvenue dans la calculatrice.\nEntrez 'exit' pour sortir")
    
    calcul = input("Votre calcul : ")
    
    while (calcul != 'exit')
    {
        calcul = eval(calcul)
        
        print(calcul)
        
        calcul = input("Votre calcul : ")
    }
}
else
{
    str = ""
    for (i,0,len(__args__))
    {
        str += __args__[i] + " "
    }
    print(eval(str))
}
print("Au revoir !")