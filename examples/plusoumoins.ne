# Jeu de plus ou moins en Neon

function main() do
    secret_number = randint(0, 101)
    
    while (True) do
        guess = nbr(input("What is you guess ? "))

        if (guess < secret_number) then
            print("Greater")
        elif (guess > secret_number) then
            print("Less")
        else
            print("You won")
            return ()
        end

    end


end


main()
