l = []
Vide = [0, 0]

for (_, 0, 27) do # cette boucle ne s'exécutera probablement pas jusqu'à la fin. Pourquoi ?
    l' = l.copy()
    l'' = Vide.copy()
    l''[0] <-> l
    l''[1] <-> l'
    l <-> l''
end
