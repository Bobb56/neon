# Neon Program to find prime numbers in a range

function is_prime(n) do
    if (n <= 1) then
        return (False)
    end
 
    max_div = n ** 0.5
    
    for (i, 2, 1 + max_div) do
        if (n % i == 0) then
            return (False)
        end
    end
    return (True)
end

 
# Driver function
premiers = []

t0 = time()

for (n,1,10000) do
    if (is_prime(n)) do
        append(premiers, n)
    end
end

t1 = time()

print("Total prime numbers in range :", premiers)

print("Time required :", t1 - t0, 'seconds.')




input()