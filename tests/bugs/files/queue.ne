# module de file en Neon


function empty_queue() do
    return ([])
end

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

F = empty_queue()

for (i, 1, 6) do
    F.push(i)
end

l = []

while (F != empty_queue()) do
    l.append(F.pop())
end

assert(l == listComp("i", 1, 6, 1, "True", "i"))
