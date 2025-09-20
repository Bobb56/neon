# programme Neon
if (len(__args__) == 0) then
    print ("Bienvenue dans la calculatrice.\nEntrez 'exit' pour sortir")
    
    calcul = input("Votre calcul : ")
    
    while (calcul != 'exit') do

        calcul = eval(calcul)
        
        print(calcul)
        
        calcul = input("Votre calcul : ")
    end
else
    str = ""
    for (i,0,len(__args__)) do
        str += __args__[i] + " "
    end
    
    print(eval(str))
end

print("Au revoir !")