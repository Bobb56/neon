function mat_comp(var1, var2, deb1, deb2, fin1, fin2, val) do # compréhension de matrices
    mat = []
    
    var1 <- deb1
    while (@var1 < fin1) do
        l = []
        
        var2 <- deb2
        while (@var2 < fin2) do
            l.append(eval(val))
            var2 <- @var2 + 1
        end
        
        mat.append(l)
        var1 <- @var1 + 1
        
    end
    return (mat)
end


function bord(n, deb, taille) do
    if (n == 0) then
        if (deb) then
            return ("╭")
        else
            return ("╮")
        end
    elif (n == taille - 1) then
        if (deb) then
            return ("╰")
        else
            return ("╯")
        end
    else
        return("│")
    end
end


method mat_aff(mat) do # affichage de matrices
    for (i, 0, len(mat)) do
        output(bord(i, True, len(mat)) + ' ')
        for (j, 0, len(mat[i])) do
            output(mat[i][j], ' ')
        end
        output(bord(i, False, len(mat)) + "\n")
    end
end



function mat_add(A, B) do # addition de matrices
    assert(len(A) > 0 and len(B) > 0)
    assert(len(A) == len(B) and len(A[0]) == len(B[0]))
    
    mat = []
    for (i, 0, len(A)) do
        l = []
        for (j, 0, len(A[i])) do
            l.append(A[i][j] + B[i][j])
        end
        mat.append(l)
    end
    return (mat)
end




function mat_mul(A, B) do
    assert(len(A) > 0 and len(B) > 0)
    assert(len(A) == len(B) and len(A[0]) == len(B[0]))
    C = []
    for (i, 0, len(A)) do
        C.append([])
        for (j, 0, len(B[0])) do
            somme = 0
            for (k, 0, len(B)) do
                somme += A[i][k] * B[k][j]
            end
            C[i].append(somme)
        end
    end
    return (C)
end




function mat_Identity(n) do
    I = []
    for (i, 0, n) do
        I.append([])
        for (j, 0, n) do
            if (i==j) then
                I[i].append(1)
            else
                I[i].append(0)
            end
        end
    end
    return (I)
end


n = 5


A = mat_comp('i', 'j', 0, 0, n, n, 'j')

B = mat_comp('i', 'j', 0, 0, n, n, '9-j')


C = mat_add(A,B)


D = mat_comp('_1', '_2', 0, 0, n, n, '9')

I = mat_Identity(n)



assert(mat_mul(I, D) == mat_mul(C, I))

print("Test mat passé !")

