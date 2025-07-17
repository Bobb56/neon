l1 = [-16, -97, -114, -123]
l2 = [-16, -97, -116, -68]


function char(l) do
    s = ""
    for (i, 0, len(l)) do
        s += chr(l[i])
    end
    return (s)
end


print(char(l1), char(l2))
