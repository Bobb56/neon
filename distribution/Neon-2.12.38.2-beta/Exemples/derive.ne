dx = 10**(-6)

function d(fonc, x) do
    return ( (fonc(x+dx) - fonc(x)) / dx )
end


function carre(x) do return (x**2) end
function cube(x) do return (x**3) end
function inv(x) do return (1/x) end
function sqrt(x) do return (x**0.5) end