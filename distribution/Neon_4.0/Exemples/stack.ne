# module de piles en Neon


function empty_stack() do
    return ( Stack(hd: None, tl: None) )
end


method stack_aff(stack) do
    output('[')
    stack.stack_aff_aux()
end


method stack_aff_aux(pile) do
    if (pile == empty_stack()) then
        print('[]')
    elif (pile>>tl == None) then # si la pile ne contient qu'un seul élément
        output(pile>>hd)
        print(']')
    else
        output(pile>>hd, ', ')
        pile>>tl.stack_aff_aux()
    end
end



method push(pile, element) do
    if (pile>>tl == None and pile>>hd == None) then
        pile>>hd = element
    else
        pile = Stack(hd : element, tl : pile)
    end
end


method pop(pile) do
    if (pile == empty_stack()) then
        failwith("Erreur : Pile vide")
    elif (pile>>tl == None) then # si la pile ne contient qu'un seul élément
        ret = pile>>hd
        pile>>hd = None
        return (ret)
    else
        ret = pile>>hd
        pile = pile >> tl
        return (ret)
    end
end


P = empty_stack()

for (i, 1, 6) do
    P.push(i)
end

l = []

while (P != empty_stack()) do
    l.append(P.pop())
end

assert(l == listComp("i", 0, 5, 1, "True", "5-i"))
