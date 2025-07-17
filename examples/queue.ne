# module de file en Neon

empty_queue = []


method push(file, elt) do
    file.append(elt)
end


method pop(file) do
    if (file == empty_queue) then
        failwith("Push failed : Empty queue")
    end
    res = file[0]
    file.remove(0)
    return (res)
end


function queue_aff(file) do
    print(file)
end
