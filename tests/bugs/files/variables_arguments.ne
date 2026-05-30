# fichier de test pour tester les fonctions avec nombre variable d'arguments


function sum(...) do
    result = 0
    foreach(object, __local_args__) do
        result += object
    end
    return (result)
end


function f(x, y, z:=5, ..., t:=12) do
    return (x + 2*z + 10*y + len(__local_args__) - 42*t)
end

assert(f(1, 2, 3) == -477)
assert(f(99, y := 4, z := 18) == -329)
assert(f(0, 0, 0, 0, 0, 0, 0, 0, 0) == -498)
assert(f(0, 0, 0, 0, t := 0) == 1)
assert(sum(1,2,3,4,5,6,7,8,9,10) == 55)
