# fichier de test pour tester les fonctions avec nombre variable d'arguments


function sum(...) do
    result = 0
    foreach(object, __local_args__) do
        result += object
    end
    return (result)
end


function f(x, y:=5, z, ..., t:=12) do
    return (x + 2*y + 10*z + len(__local_args__) - 42*t)
end

assert(f(1, 2, 3) == -469)
assert(f(x := 1, z := 4) == -453)
assert(f(0, 0, 0, 0, 0, 0, 0, 0, 0) == -498)
assert(f(0, 0, 0, 0, t := 0, 0, 0, 0, 0, 0) == 6)
