setAtomicTime(1)
for (i, 0, 200) do
    pass
end

function f1() do
    for (i, 0, 5) do print('abcdefghijklmnopqrstuvwxyz') end
end

function f2() do
    for (i, 0, 5) do print('0123456789') end
end


parallel f1()
parallel f2()

for (i, 0, 5) do
    print("Hello, World !")
end