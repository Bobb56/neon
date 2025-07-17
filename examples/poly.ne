

function compute_delta(a, b, c) do
    return (b*b - 4 * a * c)
end


function compute_x1(a, b, c, delta) do
    return ( (-b - sqrt(delta))/(2*a) )
end

function compute_x2(a, b, c, delta) do
    n = -b + sqrt(delta)
    d = 2*a
    return (n/d)
end

function compute_c(f) do
    return (f(0))
end

function compute_a(f, c) do
    "a+b" <- f(1) - c
    "4a+2b" <- f(2) - c
    "2a+2b" <- 2 * @"a+b"
    "2a" <- @"4a+2b" - @"2a+2b"
    return (@"2a"/2)
end


function compute_b(f, a, c) do
    return (f(1) - a - c)
end


function get_roots(f, a, b, c) do
    delta = compute_delta(a, b, c)
    
    if (delta > 0) then
        return ([compute_x1(a, b, c, delta), compute_x2(a, b, c, delta)])
    elif (delta == 0) then
        return ([compute_x1(a, b, c, delta)])
    else
        return ([])
    end
end


function display_roots(roots) do
    if (len(roots) == 2) then
        print("Here are the two roots :", roots[0], roots[1])
    elif (len(roots) == 1) then
        print("Here is the only root :", roots[0])
    else
        print("Your polynom has no root")
    end
end


function display_factorised(a, roots) do

    if (len(roots) == 0) then
        print("The polynom can't be factorised")
        return ()
    end
    
    if (a != 1) then
        output(a)
    end

    
    if (roots[0] == 0) then
        output('x')
    else
        output('(x')
        if (roots[0] < 0) then
            output('+' + str(-roots[0]))
        else
            output('-' + str(roots[0]))
        end
        output(')')
    end
    

    if (len(roots) == 1) then
        output('²')
    else
        if (roots[1] == 0) then
            output('x')
        else 
            output('(x')
            if (roots[1] < 0) then
                output('+' + str(-roots[1]))
            else
                output('-' + str(roots[1]))
            end
            output(')')
        end
    end
    output('\n')
end




function display_developped(a, b, c) do
    signe = ''
    
    if (a != 0 and a != 1) then
        output(a)
    end

    if (a != 0) then
        output('x²')
        signe = '+'
    end

    
    if (b < 0) then
        b = -b
        signe = '-'
    end

    if (b != 0 and b != 1) then
        output(signe + str(b) + 'x')
        signe = '+'
    elif (b == 1) then
        output(signe + 'x')
        signe = '+'
    end

    if (c < 0) then
        c = -c
        signe = '-'
    end

    if (c != 0) then
        output(signe + str(c))
    end
    
    output('\n')
end



function analysis(f) do
    c = compute_c(f)
    a = compute_a(f, c)
    b = compute_b(f, a, c)
    
    print("Your developped form is :")
    display_developped(a, b, c)
    
    roots = get_roots(f, a, b, c)
    
    display_roots(roots)
    
    display_factorised(a, roots)
end


function f(x) do
    return (eval(POLYNOM))
end


function main() do
    POLYNOM = input("Entrez l'expression d'un polynome de degre deux : ") # "(x+4)*(x-12)"
    analysis(f)
end


main()
