fonctions = [] # tableau qui va contenir les informations sur les fonctions

function function_doc(functionName, returnType, argumentsTypes, help) { $ fonction qui sert à ajouter une documentation à une fonction $
    append(fonctions, [functionName, returnType, argumentsTypes, help])
}

function function_help(functionName) { $ affiche l'aide liée à la fonction dont le nom est functionName $
    print("Fonction", functionName)
    i = 0 ; while (fonctions[i][0] != functionName) { i++ }
    print("Retourne un argument de type", fonctions[i][1])
    args = str(fonctions[i][2])
    args = sub(args, 1, len(args) - 1)
    print("Prend", len(fonctions[i][2]), "arguments de types", args)
    print(help)
}



function f(x) { return( x**2 ) } # fonction au pif

function_doc(&f, 'Number', ['Number'], "Met un nombre au carre") # documente la fonction


function_help(&f) # affiche l'aide que l'on vient d'ajouter à la fonction


