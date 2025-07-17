# module de piles en Neon


function empty_stack() do
    return (Stack(hd: None, tl: None))
end

function is_empty(stack) do
    return (stack>>hd == None and stack>>tl == None)
end

method stack_aff(stack) do
    output('[')
    stack.stack_aff_aux()
end


method stack_aff_aux(pile) do
    if (is_empty(pile)) then
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
    if (is_empty(pile)) then
        pile>>hd = element
    else
        pile = Stack(hd : element, tl : pile)
    end
end


method pop(pile) do
    if (is_empty(pile)) then
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
    p = empty_stack()
    p.push(1)
    p>>tl = p
    p.pop()
end
