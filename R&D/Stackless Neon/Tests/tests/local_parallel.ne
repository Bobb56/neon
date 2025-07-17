a = 18

function f(x) do
    local(a)
    local(b)
    a = 100
    b = x
    while (a > 0) do
        a--
    end
end



p0 = parallel f(0)
p1 = parallel f(1)
p2 = parallel f(2)
p3 = parallel f(3)


await(p0 == None and p1 == None and p2 == None and p3 == None)

assert(a == 18)
print("Test local_parallel passé !")
