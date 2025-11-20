# programme de test de Neon qui teste la fonctionnalité de retour à l'exécution après la fin du processus principal

setAtomicTime(1)

l = []
p1 = None
p2 = None

function atexit() do
    await(p1 == 0 and p2 == 0)
    assert(l == ["main", "p2", "p1", "main", "p2", "p1", "main", "p2", "p1", "main", "This is the end", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1", "p2", "p1"])
end

parallel atexit()


function process(name, bound) do
    for (i, 0, bound) do
        atomic
            l.append(name)
        end
    end
    
    return (0)
end

# code principal
p1 = parallel process('p1', 20)
p2 = parallel process('p2', 20)

process('main', 4)

# fin
l.append("This is the end")
