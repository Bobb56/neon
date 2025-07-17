function is_prime(n) do
    if (n <= 1) do
        return (False)
    end
 
    max_div = n ** 0.5
    
    for (i, 2, 1 + max_div) do
        if (n % i == 0) do
            return (False)
        end
    end
    return (True)
end


max = nbr(input("Maximum ? "))

print(list_comp("n", 2, max + 1, 1, "is_prime(n)", "n"))