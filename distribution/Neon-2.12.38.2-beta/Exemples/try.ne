function inputNumber(text) do
    bo = True
    while (bo) do
        n = input(text)
        try
            n = nbr(n)
            bo = False
        except (UnknownError) do
            print("Erreur : Veuillez entrer un nombre")
        end
    end

    return (n)
end

h = inputNumber("Votre age : ")
print("Vous etes age de", h, "ans.")

input()