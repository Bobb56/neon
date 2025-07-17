# programme de detection des palindromes

mot = input("Votre mot : ")

while (mot != "exit")
{
    motInverse = reverse(mot)
    
    $
    boucle qui servait auparavant à retourner le mot
    for (i, 0, len(mot))
    {
        motInverse += sub(mot, len(mot)-i-1, len(mot)-i)
    }
    $
    
    if (motInverse == mot)
    {
        print("C'est un palindrome")
    }
    else
    {
        print("Ce n'est pas un palindrome")
    }
    mot = input("Votre mot : ")
}