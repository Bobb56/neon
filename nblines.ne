files = [
    "errors.c",
    "builtinfunctions.c",
    "dynarrays.c",
    "strings.c",
    "parser.c",
    "syntaxtrees.c",
    "neon.c",
    "gc.c",
    "neonio.c",
    "operators.c",
    "runtime.c",
    "objects.c",
    "processcycle.c",
    "main.c",
    "trees.c",
    "win_amd64.S",
    "linux_amd64.S",
    "linux_riscv64.S",
    "ez80.asm",
    
    "headers/errors.h",
    "headers/builtinfunctions.h",
    "headers/dynarrays.h",
    "headers/strings.h",
    "headers/parser.h",
    "headers/syntaxtrees.h",
    "headers/neon.h",
    "headers/gc.h",
    "headers/neonio.h",
    "headers/operators.h",
    "headers/runtime.h",
    "headers/objects.h",
    "headers/processcycle.h",
    "headers/lowlevel.h",
    "headers/trees.h"
]

other_files = [
    "extern/linenoise.c",
    "extern/linenoise.h"
]

function print_number(n) do
    n = str(n)
    n' = ""
    for (i, len(n)) do
        if (i%3 == 0 and i > 0) then
            n' = ' ' + n'
        end
        n' = n[len(n)-i-1] + n'
    end
    return (n')
end

print_number.setFunctionDoc("Cette fonction prend en argument un nomber entier et renvoie une chaine de caractères représentant ce nombre avec des espaces tous les 3 chiffres")


function nbLinesFile(file) do
    f = readFile("src/" + file)
    return (f.count("\n"))
end

function nbLines(files) do
    n = 0
    foreach (file, files) do
        n += nbLinesFile(file)
    end
    return (n)
end



print("Le projet Neon compte", print_number(nbLines(files)), "lignes de code sans linenoise.c, mais", print_number(nbLines(files + other_files)), "lignes en tout.")
