$
Implémentation des tableaux flexibles en Neon
---------------------------------------------
Les tableaux flexibles sont des tableaux permettant d'amortir le coût des opérations d'insertion et de suppression


$

function Node(value) do
    return (Node(prev: None, next: None, value: value))
end

# ajoute un noeud
function Node~append(node, value) do
    local(node2)
    if (node>>next == None and node>>prev == None) then
        node2 = Node(value)
        node2>>next = node
        node>>next = node2
        node2>>prev = node
        node>>prev = node2
    else
        node2 = Node(value)
        node2>>next = node>>next
        node>>next = node2
        node2>>next>>prev = node2
        node2>>prev = node
    end
end



$
Un FlexArray est la donnée d'une pile de modifications, d'un tableau d'accès aux indices et d'une file doublement chaînée circulaire
On initialise un FlexArray par une liste
$
function FlexArray(init_list) do
    local(array, node)
    node = Node(init_list[0])
    array = [Index(elt: node, version: 0)]
    for (i, 1, len(init_list)) do
        node.Node~append(init_list[i])
        array.append(Index(elt: node>>next, version: 0))
    end
    return (FlexArray(updates: [], cycle: node, array: array, length:len(init_list)))
end


function FlexArray~index(fa, index) do
    FlexArray~updateIndex(fa, index)
    return (fa>>array[index]>>elt>>value)
end

function FlexArray~len(fa) do
    return (fa>>length)
end


function FlexArray~str(fa) do
    s = '['
    for (i, FlexArray~len(fa)) do
        s += str(FlexArray~index(fa, i))
        if (i < FlexArray~len(fa)-1) then s += ', ' end
    end
    s += ']'
    return (s)
end

function FlexArray~repr(fa) do
    output(str(fa))
end


function FlexArray~updateIndex(fa, index) do
    for (i, fa>>array[index]>>version, len(fa>>updates)) do
        if (fa>>updates[i]>>index <= index) then
            if (fa>>updates[i]>>sgn == 1) then
                fa>>array[index]>>elt = fa>>array[index]>>elt>>next
            else
                fa>>array[index]>>elt = fa>>array[index]>>elt>>prev
            end
        end
    end
    fa>>array[index]>>version = len(fa>>updates)
end


function FlexArray~remove(fa, index) do
    if (index < 0 or index >= FlexArray~len(fa)) then raise(IncorrectIndex, "Cannot remove element from FlexArray") end
    FlexArray~updateIndex(fa, index)
    fa>>updates.append(Update(sgn:-1, index:index))
    fa>>length--
end

function FlexArray~insert(fa, index, value) do
    FlexArray~updateIndex(fa, index)
    prevnode = fa>>array[index]>>elt>>prev
    Node~append(prevnode, value)
    print(prevnode>>value, prevnode>>next>>value, prevnode>>next>>next>>value)
    #fa>>array[index]>>elt = prevnode>>next
    print(fa>>array[index]>>elt>>value, fa>>array[index+1]>>elt>>value, fa>>array[index+2]>>elt>>value)

    if (len(fa>>array) == fa>>length) then
        fa>>array.append(Index(elt: fa>>array[0]>>elt, version: None))
    else
        fa>>array[fa>>length]>>elt = fa>>array[0]>>elt
    end
    fa>>array[fa>>length]>>version = len(fa>>updates)
    fa>>length++
    
    fa>>updates.append(Update(sgn:-1, index:index))
    
end

fa = FlexArray([1,2,3,4,5])
print(fa)