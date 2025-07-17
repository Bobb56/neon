# module de piles en Neon


empty_stack = Stack(hd: None, tl: None)


method stack_aff(stack) do
    output('[')
    stack.stack_aff_aux()
end


method stack_aff_aux(pile) do
    if (pile == empty_stack) then
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
    if (pile == empty_stack) then
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


if (__name__ == '__main__') then
    p = empty_stack
    for(i, 0, 10) do
        p.push(i)
    end
    
    p.stack_aff()
end
