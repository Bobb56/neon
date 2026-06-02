# Module qui étent de langage avec des objets comme des enums en Rust ou des structs sans arguments

function Enum(name, nb_parameters := 0) do
    local(temp_create)

    if (nb_parameters == 0) then
        name <- Enum(name: name, parameters: None)
    else
        function temp_create(..., name := name) do
            return (Enum(name: name, parameters: __local_args__))
        end
        name <- temp_create
    end
    return (@name)
end


function Enum~repr(obj) do
    output(obj>>name)
    if (obj>>parameters != None) then
        output("(")
        for (i, 0, obj>>parameters.len()) do
            output(obj>>parameters[i])

            if (i < obj>>parameters.len() - 1) then
                output(", ")
            end
        end
        output(")")
    end
end




Enum('Test', 2)

print(Test(1,2))