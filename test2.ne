function f(x) do
  print("Exécution de f")
  print('x =', x)
  return (x + 42)
end

serialize('f.dat', f)

f2 = deserialize('f.dat')

f2(42)
