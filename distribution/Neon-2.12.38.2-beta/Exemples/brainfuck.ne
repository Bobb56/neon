# interpréteur Brainfuck
function run_brainfuck(code) do
    tape = [] ; for (i, 0, 256) do append(tape, 0) end
    pointer = 0
    loop_stack = []
    
    for (instruction, 0, len(code)) do
    
        if (sub(code, instruction, instruction+1) == '>') then pointer ++
        
        elif (sub(code, instruction, instruction+1) == '<') then pointer --
        
        elif (sub(code, instruction, instruction+1) == '+') then tape[pointer] = (tape[pointer] + 1) % 256
        
        elif (sub(code, instruction, instruction+1) == '-') then tape[pointer] = (tape[pointer] - 1) % 256
        
        elif (sub(code, instruction, instruction+1) == '.') then output(chr(tape[pointer]))
        
        elif (sub(code, instruction, instruction+1) == ',') then tape[pointer] = nbr(input("Entrez un nombre entre 0 et 255 : "))
        
        elif (sub(code, instruction, instruction+1) == '[') then
            if (tape[pointer] == 0) then
                loop_count = 1
                while (loop_count > 0) do
                    instruction ++
                    if (sub(code, instruction, instruction+1) == '[') do loop_count ++
                    elif (sub(code, instruction, instruction+1) == ']') do loop_count -- end
                end
            else
                append(loop_stack, instruction)
            end
        
        elif (sub(code, instruction, instruction+1) == ']') do
            if (tape[pointer] != 0) do instruction = loop_stack[len(loop_stack)-1] - 1
            else remove(loop_stack, len(loop_stack)-1) end
        end
    end
    output('\n')
end


code = input("Entrez du code brainfuck : ")
run_brainfuck(code)
input()