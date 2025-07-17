mot = input("votre mot : ")
while (mot != 'exit')
{
  if (reverse(mot) == mot)
  {
    print("c'est un palindrome")
  }
  else
  {
    print("ce n'est pas un palindrome")
  }
  mot = input("votre mot : ")
}
print("au revoir")
