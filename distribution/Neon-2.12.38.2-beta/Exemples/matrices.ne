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


method mat_aff(mat) do # affichage de matrices
    for (i, 0, len(mat)) do
        output("| ")
        for (j, 0, len(mat[i])) do
            output(mat[i][j], ' ')
        end
        output("|\n")
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

# test des fonctions ainsi définies

if (__name__ == '__main__') then
    A = mat_comp('i', 'j', 0, 0, 10, 10, 'j')

    B = mat_comp('i', 'j', 0, 0, 10, 10, '9-j')

    A.mat_aff()

    print()

    B.mat_aff()

    C = mat_add(A,B)

    print()

    mat_aff(C)

    D = mat_comp('_1', '_2', 0, 0, 10, 10, '9')

    assert(D == C)

end

