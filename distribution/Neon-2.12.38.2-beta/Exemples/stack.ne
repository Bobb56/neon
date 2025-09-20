# module de piles en Neon


empty_stack = Stack(tete: None, queue: None)


method stack_aff(stack) do
    output('[')
    stack.stack_aff_aux()
end


method stack_aff_aux(pile) do
    if (pile == empty_stack) then
        print('[]')
    elif (pile>>queue == None) then # si la pile ne contient qu'un seul élément
        output(pile>>tete)
        print(']')
    else
        output(pile>>tete, ', ')
        pile>>queue.stack_aff_aux()
    end
end



method push(pile, element) do
    if (pile>>queue == None and pile>>tete == None) then
        pile>>tete = element
    else
        pile = Stack(tete : element, queue : pile)
    end
end


method pop(pile) do
    if (pile == empty_stack) then
        failwith("Erreur : Pile vide")
    elif (pile>>queue == None) then # si la pile ne contient qu'un seul élément
        ret = pile>>tete
        pile>>tete = None
    else
        ret = pile>>tete
        pile = pile >> queue
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
