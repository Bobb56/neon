setAtomicTime(1)

l = []

function f1() do
    for (i, 0, 5) do print('abcdefghijklmnopqrstuvwxyz') end
end

function f2() do
    for (i, 0, 5) do print('0123456789') end
end


p1 = parallel f1()
p2 = parallel f2()

for (i, 0, 5) do
    print("Hello, World !")
end

await(p1 == None and p2 == None)