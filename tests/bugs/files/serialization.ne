# Fichier de test des fonctionnalités de sérialisation de Neon
# Fonctionne avec les fichiers serialization_aux2.ne et serialization_aux2.ne

# Sérialisation de tout un tas d'objets dans un environnement tout neuf
safeExec('files/serialization_aux1.ne', [])

# Désérialisation de ces objets dans un nouvel environnement, et vérification de leur intégrité
safeExec('files/serialization_aux2.ne', [])

# Tests sur les processus, là ça ne fonctionne que si on est dans le même environnement
function f(x) do
    local(l)
    l = []
    for (i, x) do
        l.append(i)
    end
    return (len(l))
end

promise = parallel f(1000)

saveObj('files/data/promise', promise)
promise2 = loadObj('files/data/promise')

assert(promise2 is Promise)
