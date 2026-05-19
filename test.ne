filename = "test.dat"

l = [1,2,3]
z = Cmplx(re: 5, im:12)
obj = [l, z]
obj[1]>>im = obj

print(obj)

serialize(filename, obj)

assert (deserialize(filename) == obj)
