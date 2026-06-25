$
Implémentation des tableaux flexibles en Neon
---------------------------------------------
Les tableaux flexibles sont des tableaux permettant d'amortir le coût des opérations d'insertion et de suppression


$

function FlexArrayNode(first_index, last_index, successors) do
    return (
        FlexArrayNode(
            successors: successors
        )
    )
end


function FlexArrayLeaf(first_index, last_index, items) do
    return (
        FlexArrayLeaf(
            items: items
        )
    )
end


function FlexArray(list) do
    return (
        FlexArray(root: FlexArrayLeaf(list))
    )
end


# Returns the upper level of an index
function FlexArrayNode~getLeaf(self, starting_index, index) do
    if (self is FlexArrayLeaf) then
        return (
            LocatedLeaf(start: starting_index, items: self>>items)
        )
    end

    local(successor)
    for (successor_index, self>>successors.len()) do
        successor = self>>successors[successor_index]
        if (starting_index <= index and starting_index + successor>>sucessors.len() > index) then
            return (FlexArrayNode~getLeaf(successor, starting_index, index))
        else
            starting_index += sucessor>>successors.len()
    end
    raise(OutOfRange, '')
end


function FlexArrayNode~insert(self, index, value) do
    local(leaf)
    leaf = FlexArrayNode~getLeaf(self, index)

    if (index == leaf>>items.len() - 1) then
        leaf>>items.append(value)