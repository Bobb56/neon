setAtomicTime(1)

function process(x) do
    for (i, 0, 5) do
        print(x)
    end
end

p = parallel process(1)
p2 = parallel process(2)

await (p == None and p2 == None)

input()