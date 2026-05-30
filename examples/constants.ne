# Module qui étent de langage avec des objets de type enums en Rust

function enum(name, nb_parameters) do
    local(temp_creation, temp_repr, temp_str)
    # Définition de la fonction de création d'objets
    function temp_creation(..., name := name, nb_parameters := nb_parameters) do
        expr = name + '('
        for (i, nb_parameters) do
            expr += '_' + str(i) + ': ' + str(__local_args__[i])
            if (i < nb_parameters - 1) then
                expr += ', '
            end
        end

        expr += ')'
        return (eval(expr))
    end

    # Définition de la fonction d'affichage
    function temp_repr(obj, name := name, nb_parameters := nb_parameters) do
        output(name, '( ')
        for (i, nb_parameters) do
            output(eval('obj>>_' + str(i)))

            if (i < nb_parameters - 1) then
                output(', ')
            end
        end
        output(')')
    end

    name <- temp_creation
    name + '~repr' <- temp_repr
end

enum('Test', 2)

print(Test(1,2))