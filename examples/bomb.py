l0 = []

for _ in range(51):
    l1 = l0.copy()
    l2 = [0, 0]
    l2[0] = l0
    l2[1] = l1
    l0 = l2

print(l0)
