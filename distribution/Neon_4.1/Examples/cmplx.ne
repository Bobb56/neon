i = Cmplx(re: 0, im: 1)

function Cmplx(re, im := 0) do
    if (type(re) in ['Decimal', 'Integer'] and type(im) in ['Decimal', 'Integer']) then
        return (Cmplx(re:re, im:im))
    else
        return (re)
    end
end


function Cmplx~repr(z) do
    if (z>>re == 0) then
        if (z>>im == 0) then # zéro
            output(0)
        end
        
        if (z>>im != 1 and z>>im != -1) then
            output(z>>im, '*')
        elif (z>>im == -1) then
            output('-')
        end
        
        output('i')
    
    elif (z>>im == 0) then # réel
        output(z>>re)
    else
        if (z>>im == 1) then
            output(str(z>>re) + ' + i')
        elif (z>>im > 0) then
            output(str(z>>re) + ' + ' + str(z>>im) + '*i')
        elif (z>>im == -1) then
            output(str(z>>re) + ' ' + str(z>>im) + 'i')
        else
            output(str(z>>re) + ' ' + str(z>>im) + '*i')
        end
    end
end


function Cmplx~add(z1, z2) do
    z1 = Cmplx(z1) ; z2 = Cmplx(z2)
    return (Cmplx(re: z1>>re + z2>>re, im: z1>>im + z2>>im))
end


function Cmplx~minus(z) do
    return (Cmplx(re:-z>>re, im:-z>>im))
end


function Cmplx~sub(z1, z2) do
    z1 = Cmplx(z1) ; z2 = Cmplx(z2)
    return (Cmplx(re: z1>>re - z2>>re, im: z1>>im - z2>>im))
end


function Cmplx~mul(z1, z2) do
    z1 = Cmplx(z1) ; z2 = Cmplx(z2)
    return (Cmplx(re: z1>>re*z2>>re - z1>>im*z2>>im, im: z1>>re*z2>>im + z1>>im*z2>>re))
end

function cmplx_mod_s(z) do
    return (z>>re**2 + z>>im**2)
end


function Cmplx~abs(z) do
    return (sqrt(cmplx_mod_s(z)))
end

function Cmplx~div(z1, z2) do
    z1 = Cmplx(z1) ; z2 = Cmplx(z2)
    denom = cmplx_mod_s(z2)
    re = z1>>re*z2>>re + z1>>im*z2>>im
    im = z2>>re*z1>>im - z2>>im*z1>>re
    return (Cmplx(re: re/denom, im: im/denom))
end


function Cmplx~inv(z) do
    denom = cmplx_mod_s(z)
    return (Cmplx(re: z>>re/denom, im: -z>>im/denom))
end

function Cmplx~equal(z1, z2) do
    z1 = Cmplx(z1) ; z2 = Cmplx(z2)
    return (z1>>re == z2>>re and z1>>im == z2>>im)
end


z2 = i*i
z3 = 1/z2
z3 = -z3

"1 + 2i" <- 1 + 2*i
"2 + 3i" <- 2 + 3*i
"1 + i" <- @"2 + 3i" - @"1 + 2i"

assert(@"1 + i" / (1+i) == z3)
assert(Cmplx(4) == 4) # surcharge de l'opérateur d'égalite
