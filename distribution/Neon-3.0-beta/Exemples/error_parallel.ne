function f() do
    i = 0
    while (i < 2000) do
        i++
    end
    p' = parallel g()
    return (p')
end

function g() do
    a = 0
    for (i, 0, 1001) do
        a++
    end
    a = 1//0
end

function join(process) do
    await(type(process) != "Promise")
end


try
    p = parallel f()
    p.join() # avec ça, on s'assure de rester dans le try tant que les deux processus tournent
except (DivisionByZero) do
    p = 5
end

assert(p == 5)

print("Test error_parallel passé !")
