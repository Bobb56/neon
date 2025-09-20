# module de fractions en Neon

function pgcd(a, b) do
    while (b != 0) do
        t = b
        b = a % b
        a = t
    end
    return (a)
end

# overloaded
function Frac~repr(frac) do
    if (frac>>denom == 1) then
        output(frac>>num)
    else
        output(frac>>num, '/', frac>>denom)
        output()
    end
end

function Frac(num, denom := 1) do
    return(Frac~reduced(Frac(num: num, denom: denom)))
end

method Frac~reduce(frac) do
    while ((pgcd(frac>>num, frac>>denom) -> dc) > 1) do
        frac>>num = frac>>num // dc
        frac>>denom = frac>>denom // dc
    end
end

function Frac~reduced(frac) do
    frac2 = frac.copy()
    frac2.Frac~reduce()
    return (frac2)
end

method fractionize(a) do
    if (type(a) == 'Integer') then
        a = Frac(a)
    end
end


function Frac~mul(a, b) do
    a.fractionize()
    b.fractionize()
    return (Frac~reduced(Frac(num: a>>num * b>>num, denom: a>>denom * b>>denom)))
end

function Frac~inv(a) do
    a.fractionize()
    return (Frac(num: a>>denom, denom: a>>num))
end

function Frac~div(a, b) do
    return (Frac~reduced(Frac~mul(a, Frac~inv(b))))
end

function Frac~add(a, b) do
    a.fractionize()
    b.fractionize()
    frac = Frac(num: a>>num * b>>denom + b>>num * a>>denom, denom: b>>denom * a>>denom)
    return (Frac~reduced(frac))
end

function Frac~sub(a, b) do
    a.fractionize()
    b.fractionize()
    frac = Frac(num: a>>num * b>>denom - b>>num * a>>denom, denom: b>>denom * a>>denom)
    return (Frac~reduced(frac))
end

function Frac~to_double(frac) do
    return (frac>>num / frac>>denom)
end

a = Frac(3, 2)
b = Frac(7, 5)
c = b / 4

assert(a + b == Frac(29, 10))
assert(a / a == Frac(1) and b / b == Frac(1))
assert(a * a * b + c == a + Frac(2))
assert(Frac(3 * 5 * 7 * 23, 7 * 2 * 3) == Frac(5 * 23, 2))
