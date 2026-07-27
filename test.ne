# ---------------------- SECOND TEST ----------------------
# cette fonction renvoie f o g. On suppose que f et g prennent un seul argument
function compo(f, g) do
    function aux(x, f := f, g := g) do
        return (f(g(x)))
    end

    return (aux)
end

function plus1(x) do
    return (x+1)
end

function x2(x) do
    return (x * 2)
end

object = [compo, compo(x2, plus1)]

saveObj('data/test2', object)