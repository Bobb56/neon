function f(x) do
    for (i, 0, x) do
        x += x
    end
    return (x)
end

p = parallel f(10)

print(p)

l = [p, p, p, p, p]

print(l)

while (type(p) == "Promise") do
    pass
end

print(p)

print(l)
