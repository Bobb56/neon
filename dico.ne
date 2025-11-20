# l'implémentation ci-dessous des dictionnaires est naïve
# un dictionnaire est un tableau de couples (clé, objet)

dico_vide = []

method dico_add(dico, cle, element) do
  local(item)
  item = Item(key: cle, elt: element)
  dico.append(item)
end


function dico_index(dico, cle) do
  local(i)
  i = 0
  while (i < len(dico) and dico[i]>>key != cle) do i++ end
  return (i)
end

method dico_remove(dico, cle) do
  local(i)
  i = dico_index(dico, cle)
  dico.remove(i)
end


function dico_elt(dico, cle) do
  local(i)
  i = dico_index(dico, cle)
  return (dico[i]>>elt)
end


function dico_cle(dico, element) do
  local(i)
  i = 0
  while (i < len(dico) and dico[i]>>elt != element) do i++ end
  return (dico[i]>>key)
end



function dico_aff(dico) do # même comportement que = dico_aff = print
  print(dico)
end

D = dico_vide
D.dico_add("1", 1)
D.dico_add("2", 2)
D.dico_add("3", 3)

assert(D.dico_elt("1") == 1)
assert(D.dico_cle(3) == "3")

D.dico_remove("2")

try
    a = D.dico_elt("2")
except () do
    a = None
end

assert(a == None)
