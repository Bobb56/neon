# fonction factorielle récursive

function fact(n) do
    a = 7
    if (n < 2) then
        return ( 1 )
    else
        return ( n * fact(n-1) )
    end
end

print("Factorielle 5 :", fact(5))


#test d'une méthode qui mutiplie des listes


var_glob = 18

method list_multiply(l, x) do
    local(var_glob)
    var_glob = 5
    
    for (i, 0, len(l)) do
        l[i] *= x
    end
end

if (var_glob != 18) then failwith("Erreur de variable locale") end

l = [1,2,3,4]
list_multiply(l, 2)
print("Nouvelle liste :", l)