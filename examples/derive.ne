dx = 10**(-6)

function derive(fonc) do
    # on utilise les arguments optionnels pour enregistrer cette version de la fonction au moment de la définition
    function temp(x, original := fonc) do
        return ((original(x + dx) - original(x))/dx)
    end
    
    return (temp)
end



function carre(x) do return (x**2) end
function cube(x) do return (x**3) end
function inv(x) do return (1/x) end
function sqrt(x) do return (x**0.5) end


