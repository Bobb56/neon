filename = "test.dat"

# On crée une liste contenant une liste et un container
l = [1,2,3]
z = Cmplx(re: 5, im:12)
obj = [l, z]

# On rend cet objet cyclique
obj[1]>>im = obj

# On crée une fonction avec obj dans sa clôture
function f(x, y := obj) do
  print("Exécution de f")
  print('x =', x)
  return (x + 42)
end

# On rajoute f dans sa propre clôture
obj[0] = f

serialize(filename, f)
f2 = deserialize(filename)

print(f, f2)

print(f(5), f2(5))

#assert (f2 == f)
