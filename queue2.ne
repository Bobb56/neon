# bibliothèque de file implémentée par listes doublement chaînées, fonctionne grâce au compteur de références


function Queue~make_empty() do
    return ( Queue(head: None, tail: None) )
end

function Queue~is_empty(queue) do
    return (queue>>head == None and queue>>tail == None)
end

createException("FileVide")

method Queue~push(queue, data) do
    Cell = Cell(label: data, prev: None, next: None)
    if (queue>>head == None and queue>>tail == None) then
        Cell -> queue>>head -> queue>>tail
    elif (queue>>head == queue>>tail) then
        Cell>>next = queue>>head
        queue>>head>>prev = Cell
        queue>>head = Cell
    else
        Cell>>next = queue>>head
        queue>>head>>prev = Cell
        queue>>head = Cell
    end
end

method Queue~pop(queue) do
    if (queue>>head == None and queue>>tail == None) then
        exception(FileVide)
        return ()
    elif (queue>>head == queue>>tail) then
        val = queue>>tail>>label
        None -> queue>>head -> queue>>tail
    else
        val = queue>>tail>>label
        queue>>tail>>prev>>next = None
        queue>>tail = queue>>tail>>prev
    end
    return (val)
end


function Queue~list_reverse(liste) do
    return ( listComp('k', 1, len(liste) + 1, 1, "True", "liste[len(liste)-k]") )
end


method Queue~iter(queue, fonction) do
    node = queue>>tail
    while (node != None) do
        fonction(node>>label)
        node = node>>prev
    end
end




function Queue~main() do
    Q = Queue~make_empty()
    for (i, 0, 6) do
        Q.Queue~push(i)
    end
    
    l = []
    
    Q' = Q.copy()
    Q' = 5
    
    while (not Q.Queue~is_empty()) do
        l.append(Q.Queue~pop())
    end
    
    assert(l.Queue~list_reverse() == listComp("k", 0, 6, 1, "True", "5-k"))
end



Queue~main()
