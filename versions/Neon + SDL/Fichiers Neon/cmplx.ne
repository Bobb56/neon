_i = Cmplx(re: 0, im: 1)



method cmplx_aff(z) do
    if (z>>re == 0) then
        if (z>>im == 0) then # zéro
            print(0)
        end
        
        output(z>>im) # imaginaire pur
        
        if (z>>im != 1) then
            print('i')
        else
            print()
        end
    
    elif (z>>im == 0) then # réel
        print(z>>re)
    else
        print(str(z>>re) + ' + ' + str(z>>im) + 'i')
    end
end


function cmplx_add(z1, z2) do
    return (Cmplx(re: z1>>re + z2>>re, im: z1>>im + z2>>im))
end


method cmplx_minus(z) do
    z>>re = -z>>re
    z>>im = -z>>im
end


method cmplx_substract(z1, z2) do
    return (Cmplx(re: z1>>re - z2>>re, im: z1>>im - z2>>im))
end


function cmplx_mult(z1, z2) do
    return (Cmplx(re: z1>>re*z2>>re - z1>>im*z2>>im, im: z1>>re*z2>>im + z1>>im*z2>>re))
end

function cmplx_mod_s(z) do
    return (z>>re**2 + z>>im**2)
end


function cmplx_mod(z) do
    return (cmplx_mod_s(z)**2)
end

function cmplx_div(z1, z2) do
    denom = cmplx_mod_s(z2)
    re = z1>>re*z2>>re + z1>>im*z2>>im
    im = z2>>re*z1>>im - z2>>im*z1>>re
    return (Cmplx(re: re/denom, im: im/denom))
end


function cmplx_inv(z) do
    denom = cmplx_mod_s(z)
    return (Cmplx(re: z>>re/denom, im: -z>>im/denom))
end

