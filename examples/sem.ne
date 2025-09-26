# bibliothèque de sémaphores à compteurs en Neon


function Sem~create(compteur) do
    return (Sem(cmpt: compteur))
end



function Sem~P(s) do
    while (True) do
        await(s>>cmpt > 0)
        atomic
            if (s>>cmpt > 0) then
                s>>cmpt--
                break
            end
        end
    end
end


function Sem~V(s) do
    s>>cmpt++
end