mot = input("votre mot : ")
while (mot != 'exit') do
  if (reverse(mot) == mot) then
    print("c'est un palindrome")
  else
    print("ce n'est pas un palindrome")
  end
  mot = input("votre mot : ")
end
print("au revoir")
input()
