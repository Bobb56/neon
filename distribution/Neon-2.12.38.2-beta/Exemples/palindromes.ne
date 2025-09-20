# programme de detection des palindromes

mot = input("Votre mot : ")

while (mot != "exit") do
    motInverse = reverse(mot)
    
    if (motInverse == mot) then
        print("C'est un palindrome")
    else
        print("Ce n'est pas un palindrome")
    end
        
    mot = input("Votre mot : ")
end
