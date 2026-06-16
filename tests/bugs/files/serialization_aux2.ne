# ----------------------- PREMIER TEST ------------------
# On crée une liste contenant une liste et un container
l = [1,2,3]
z = Cmplx(re: 5, im:12)
obj = [l, z]

# On rend cet objet cyclique
obj[1]>>im = obj

# On crée une fonction avec obj dans sa clôture
function f(x, y := obj) do
  return ([x + 42, obj])
end

# On rajoute f dans sa propre clôture
obj[0] = f

f2 = loadObj('files/data/test1')

for (_, 100) do
  value = randint(-50000, 50000)/1000
  assert(f(value)[0] == f2(value)[0])
end

# ---------------------- SECOND TEST ----------------------

# Définition préalable d'une variable utilisée par compo
g = 5

object = loadObj('files/data/test2')

function mult(alpha) do
    function aux(x, alpha := alpha) do
        return (x * alpha)
    end

    return (aux)
end

function add(lambda) do
    function aux(x, lambda := lambda) do
        return (x + lambda)
    end

    return (aux)
end

double = mult(2)
plus2 = add(2)
compo = object[0]
f2 = compo(plus2, double)

f = object[1]

for (_, 100) do
  value = randint(-50000, 50000)/1000
  assert(f(value) == f2(value))
end

# ---------------------- TROISIEME TEST ----------------------
# On définit un type de container incompatible avec ce que l'on va désérialiser
p = Pointer(val: 3, exception: 12)

try
  p = loadObj('files/data/test3')
  assert(False)
except (DeserializationError) do
  pass
except() do
  assert(False)
end
