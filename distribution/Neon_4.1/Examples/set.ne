# implémentation d'une bibliothèque d'ensembles en Neon

function Set(...) do
    return ( Set(content: __local_args__) )
end

function Set~repr(set) do
    output('Set(')
    for (i, 0, len(set>>content)) do
        output(set>>content[i])
        if (i < len(set>>content) - 1) then
            output(', ')
        end
    end
    output(')')
end

function Set~in(element, set) do
    return (element in set>>content)
end

function Set~get(set) do
    return (set>>content[randint(0, len(set>>content))])
end

method Set~append(set, element) do
    if (not element in set) then
        set>>content.append(element)
    end
end

function Set~union(set1, set2) do
    newset = set2.copy()
    foreach (element, set1>>content) do
        newset.Set~append(element)
    end
    return (newset)
end

function Set~isSuperset(set1, set2) do
    foreach (element, set2>>content) do
        if (not element in set1) do
            return (False)
        end
    end
    return (True)
end

function Set~isSubset(set1, set2) do
    return (Set~isSuperset(set2, set1))
end

function Set~equal(set1, set2) do
    if (type(set1) == "Set" and type(set2) == "Set") then
        return (set1.Set~isSuperset(set2) and set2.Set~isSuperset(set1))
    end
    return (False)
end


function Set~inter(set1, set2) do
    newset = Set()
    foreach (element, set1>>content) do
        if (element in set2) then
            newset>>content.append(element)
        end
    end
    return (newset)
end


function Set~exclude(set1, set2) do
    newset = Set()
    foreach(element, set1>>content) do
        if (element in set1 and not element in set2) then
            newset>>content.append(element)
        end
    end
    return (newset)
end


Set~add = Set~union
Set~div = Set~exclude

function main() do
    set1 = Set(1, 2, 3, 4)
    assert(set1 == Set(4, 3, 1, 2))

    assert(4 in set1)
    set2 = Set(1, 3, 5, 7)

    assert(set1 + set2 == Set(1, 3, 5, 7, 2, 4))
    assert(Set~inter(set1, set2) == Set(1, 3))
    
    assert(set1 / Set(1, 2) == Set(4, 3))
end

main()
