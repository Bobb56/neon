# Neon Program to find prime numbers in a range

function is_prime(n) {
    if (n <= 1) {
        return (False)
    }
 
    max_div = n ** 0.5
    
    for (i, 2, 1 + max_div) {
        if (n % i == 0) {
            return (False)
        }
    }
    return (True)
}

 
# Driver function
t0 = time()
c = 0 #for counting
premiers = []
 
for (n,1,100) {
    if (is_prime(n)) {
        append(premiers, n)
    }
}

t1 = time()
print("Time required :", t1 - t0, 's.')

print("Total prime numbers in range :", premiers)