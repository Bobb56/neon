# programme de test de Neon qui teste la fonctionnalité de retour à l'exécution après la fin du processus principal

setAtomicTime(1)

l = []
p1 = None
p2 = None

function atexit() do
    await(p1 == 0 and p2 == 0)
    assert(l == [[0, "blue"], [1, "blue"], [0, "red"], [2, "blue"], [0, "green"], [1, "red"], [3, "blue"], [1, "green"], [2, "red"], [4, "blue"], [2, "green"], [3, "red"], [5, "blue"], [4, "red"], [6, "blue"], [5, "red"], [7, "blue"], [6, "red"], "This is the end", [8, "blue"], [7, "red"], [9, "blue"], [8, "red"], [9, "red"]])
end

parallel atexit()



function process(color, bound) do
    for (i, 0, bound) do
        atomic
            l.append([i, color])
        end
        _ = 0 # pour lancer une évaluation qui va changer de processus
    end
    
    return (0)
end

# code principal
p1 = parallel process('blue', 10)
p2 = parallel process('red', 10)

process('green', 3)

# fin
l.append("This is the end")
