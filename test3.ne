function f(x) do
    local(l)
    l = []
    for (i, x) do
        l.append(i)
    end
    return (len(l))
end

promise = parallel f(1000)

serialize('promise.neobj', promise)

promise2 = deserialize('promise.neobj')

