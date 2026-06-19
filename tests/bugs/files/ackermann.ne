function Ackermann(m, n) do
    if (m == 0) then
        return (n+1)
    elif (m > 0 and n == 0) then
        return (Ackermann(m-1, 1))
    else
        return (Ackermann(m-1, Ackermann(m, n-1)))
    end
end

function Factorial(n) do
    if (n <= 1) then
        return (1)
    else
        return (n * Factorial(n-1))
    end
end

assert(Ackermann(3,5) == 253)
assert(Factorial(5) == 120)
