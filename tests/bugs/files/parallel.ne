setAtomicTime(1)

l = []

function f1() do
    for (i, 0, 5) do l.append('abcdefghijklmnopqrstuvwxyz') end
end

function f2() do
    for (i, 0, 5) do l.append('0123456789') end
end


p1 = parallel f1()
p2 = parallel f2()

for (i, 0, 5) do
    l.append("Hello, World !")
end

await(p1 == None and p2 == None)

assert(l == ["Hello, World !", "0123456789", "abcdefghijklmnopqrstuvwxyz", "Hello, World !", "0123456789", "abcdefghijklmnopqrstuvwxyz", "Hello, World !", "0123456789", "abcdefghijklmnopqrstuvwxyz", "Hello, World !", "0123456789", "abcdefghijklmnopqrstuvwxyz", "Hello, World !", "0123456789", "abcdefghijklmnopqrstuvwxyz"])
