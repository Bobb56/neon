
if (len(__args__) == 0)
{
    iter = nbr(input("Combien d'itérations ? "))
}
else
{
    iter = nbr(__args__[0])
}


l = []
for (i,1,iter)
{
    $
    l2 = []
    append(l2, l)
    l = l2
    $
    l = [l]
}

print(l)
