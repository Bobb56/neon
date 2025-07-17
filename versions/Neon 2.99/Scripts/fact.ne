function fact(n)
{
    a = 7
    if (n < 2)
    {
        return ( 1 )
    }
    else
    {
        return ( n * fact(n-1) )
    }
}

print("Factorielle 5 :", fact(5))

truc = 17

method list_multiply(l, x)
{
    h = 5
    for (i, 0, len(l))
    {
        l[i] *= x
    }
}

l = [1,2,3,4]
list_multiply(l, 2)
print("Nouvelle liste :", l)