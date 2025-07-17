method algo(a, b) do
    if (b != 0) then
        a.append([])
        a[0].algo(b-1)
    end
end

l = []
l.algo(100000)
print(l)

# problèmes de stack overflow
