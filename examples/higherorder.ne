# ce programme Neon démontre les capacités d'ordre supérieur du langage

# cette fonction prend en entrée deux fonctions f1 et f2 : number -> number et renvoie une fonction f : number -> number telle que f(x) = f1(x) + f2(x)
function plus(f1, f2) do
    function aux(x, f1 := f1, f2 := f2) do
        return (f1(x) + f2(x))
    end
    
    return (aux)
end

# cette fonction renvoie f o g. On suppose que f et g prennent un seul argument
function compo(f, g) do
    function aux(x, f := f, g := g) do
        return (f(g(x)))
    end
    
    return (aux)
end


function cube(x) do return (x*x*x) end
function double(x) do return (2*x) end

f1 = plus(cube, double)
f2 = compo(cube, double)
