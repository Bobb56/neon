function choice(liste)
{
    return (liste[randint(len(liste))])
}

g=["lucas", "raphael"]
p=["amaury","mathis", "hugo"]

a = randint(len(p))

arbitre = p[a]
remove(p, a)


cote=randint(2)

l1 = []

append(l1,choice(g))

append(l1,choice(p))

print(l1)

if (cote){
    print("gauche")}
else{
    print("droite")}

print("arbitre :", arbitre)