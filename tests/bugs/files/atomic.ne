a = 2

function f() do
    for (i, 0, 100) do
        a = 5
    end
end

function g() do
    for (i, 0, 50) do
        atomic
            a = 10
            a++
            a++
            assert(a == 12)
        end
    end
end
        


p1 = parallel f()
p2 = parallel g()

await(p1 == None and p2 == None)
