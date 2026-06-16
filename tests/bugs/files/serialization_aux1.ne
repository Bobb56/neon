# ----------------------- PREMIER TEST ------------------
# On crée une liste contenant une liste, un container et une exception
l = [1,2,3]
z = Cmplx(re: 5, im:12)
e = createException("MonExceptionAMoi")
obj = [l, z, e]

# On rend cet objet cyclique
obj[1]>>im = obj

# On crée une fonction avec obj dans sa clôture
function f(x, y := obj) do
  return ([x + 42, obj])
end

# On rajoute f dans sa propre clôture
obj[0] = f

saveObj('files/data/test1', f)


# ---------------------- SECOND TEST ----------------------
# cette fonction renvoie f o g. On suppose que f et g prennent un seul argument
function compo(f, g) do
    function aux(x, f := f, g := g) do
        return (f(g(x)))
    end

    return (aux)
end

function plus1(x) do
    return (x+1)
end

function x2(x) do
    return (x * 2)
end

object = [compo, compo(x2, plus1)]

saveObj('files/data/test2', object)

# ---------------------- TROISIEME TEST ----------------------

p = Pointer(value: 5, exception: DivisionByZero)
saveObj('files/data/test3', p)
