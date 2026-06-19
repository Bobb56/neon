# Implémentation de table de hachage en Neon grâce à la nouvelle fonction hash



function HashTbl(initial_size) do
    local(hashtbl)
    hashtbl = HashTbl(data: [], size: initial_size)

    for (i, initial_size) do
        hashtbl>>data.append([])
    end
    return (hashtbl)
end

function HashTbl~update(hashtbl, key, value) do
    local(cell)
    cell = hashtbl>>data[hash(key) % hashtbl>>size]
    foreach (item, cell) do
        if (item>>key == key) then
            item>>value = value
            return ()
        end
    end
    cell.append(HashTblItem(key: key, value: value))
end


function HashTbl~get(hashtbl, key) do
    local(cell)
    cell = hashtbl>>data[hash(key)%hashtbl>>size]
    foreach (item, cell) do
        if (item>>key == key) then
            return (item>>value)
        end
    end
    raise(OutOfRange, "The requested key <> was not found in the hash table.".format(key))
end


function random_string() do
    local(length, s)
    length = randint(5, 15)
    s = ""
    for (_, length) do
        s += chr(randint(97, 97 + 26))
    end
    return (s)
end

function HashTbl~test(n_items := 1000, h_size := 10) do
    h = HashTbl(h_size)

    # Ajout de chaînes de caractères aléatoires dans la table de hachage
    for (i, n_items) do
        h.HashTbl~update(random_string(), None)
    end

    # Moyenne théorique de remplissage des alvéoles
    expected_mean = n_items/h_size
    # Calcule de la moyenne de l'écart au carré entre le remplissage de chaque alvéole et le remplissage théorique
    mean = 0
    for (i, h_size) do
        mean += (expected_mean - h>>data[i].len())**2
        print(h>>data[i].len())
    end

    print("L'écart-type du remplissage des alvéoles est de <>.".format(sqrt(mean/h_size)))
end
