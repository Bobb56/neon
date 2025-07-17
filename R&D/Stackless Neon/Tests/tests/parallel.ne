setAtomicTime(1)

l = []

function f1() do
    for (i, 0, 5) do l.append('abcdefghijklmnopqrstuvwxyz') end
end

function f2() do
    for (i, 0, 5) do l.append('0123456789') end
end


parallel f1()
parallel f2()

for (i, 0, 5) do
    l.append("Hello, World !")
end

assert(l == ["Hello, World !",
            "abcdefghijklmnopqrstuvwxyz",
            "0123456789",
            "Hello, World !",
            "abcdefghijklmnopqrstuvwxyz",
            "0123456789",
            "Hello, World !",
            "abcdefghijklmnopqrstuvwxyz",
            "0123456789",
            "Hello, World !",
            "abcdefghijklmnopqrstuvwxyz",
            "0123456789",
            "Hello, World !",
            "abcdefghijklmnopqrstuvwxyz",
            "0123456789"]
            )

print("Test parallel passé !")
