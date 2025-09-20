#module d'abres généraux en Neon

a = Noeud(label: 1, fils: [None, Noeud(label: 1, fils: [None]), Noeud(label: 17, fils: [None, None])])


function creer_arbre(label) do
  return (Noeud(label:label, fils:[]))
end


arbre_vide = None

method ajouter_fils(pere, fils) do
  if (pere == arbre_vide) then
    failwith("Erreur : arbre vide")
  else
    pere>>fils.append(fils)
  end
end


method changer_label(arbre, nouv_label) do
    arbre>>label = nouv_label
end


method acceder_fils(pere, numero) do
  return (pere>>fils[numero])
end


method suppr_fils(arbre, numero) do
  arbre>>fils.remove(numero)
end


method arbre_aff_aux(arbre, espaces) do
    local(i)
    output(espaces  * ' ')
    if (arbre == arbre_vide) then
        print("None")
    else
        print(arbre>>label)
        for (i, 0, len(arbre>>fils)) do
            arbre_aff_aux(arbre>>fils[i], espaces + 1)
        end
    end
end


method arbre_aff(arbre) do
    arbre_aff_aux(arbre, 0)
end

a.ajouter_fils(Noeud(label: 13, fils: [None]))

try
    a.suppr_fils(17)
except () do
    a.suppr_fils(2)
end

assert(a == Noeud(label: 1, fils: [None, Noeud(label: 1, fils: [None]), Noeud(label: 13, fils: [None])]))
