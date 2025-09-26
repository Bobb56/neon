# bibliothèque de mutex en Neon

function Mutex~create() do
    return ( Mutex(free: True) )
end

# cette fonction permet de verrouiller un mutex
function Mutex~lock(mutex) do
    while (True) do
        await(mutex>>free) # attente passive
        atomic
            if (mutex>>free) then
                mutex>>free = False
                break
            end
        end
    end
end

function Mutex~unlock(mutex) do
    mutex>>free = True
end