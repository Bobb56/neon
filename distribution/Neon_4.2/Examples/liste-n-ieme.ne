method list_create(list, number) do
    if (number == 0) then
        return (None)
    else
        append(list, [])
        list_create(list[0], number-1)
    end
end


function list_iter(n) do
    l = []
    l.list_create(n)
    return (l)
end

assert (list_iter(5) == [[[[[[]]]]]])
