files = [
    "errors.c",
    "standardmodule.c",
    "graphicmodule.c",
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
    "ez80.S",
    "sidememory.c",
    "contexts.c",
    "serialization.c",
    "nativefunctions.c",
    "exec.c",
    "eval.c",
    "extern/deadline.c",
    "extern/ide/clipboard.c",
    "extern/ide/console.c",
    "extern/ide/dialogs.c",
    "extern/ide/editor.c",
    "extern/ide/find.c",
    "extern/ide/font.c",
    "extern/ide/home.c",
    "extern/ide/libmalloc.c",
    "extern/ide/neonide.c",
    "extern/ide/keys.c",
    
    "headers/errors.h",
    "headers/standardmodule.h",
    "headers/graphicmodule.h",
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
    "headers/trees.h",
    "headers/sidememory.h",
    "headers/contexts.h",
    "headers/serialization.h",
    "headers/nativefunctions.h",
    "headers/exec.h",
    "headers/eval.h",
    "headers/allocator.h",
    "headers/constants.h",
    "extern/deadline.h",
    "extern/ide/headers/clipboard.h",
    "extern/ide/headers/console.h",
    "extern/ide/headers/dialogs.h",
    "extern/ide/headers/editor.h",
    "extern/ide/headers/find.h",
    "extern/ide/headers/font.h",
    "extern/ide/headers/home.h",
    "extern/ide/headers/libmalloc.h",
    "extern/ide/headers/neonide.h",
    "extern/ide/headers/state.h",
    "extern/ide/headers/keys.h"
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



print("The Neon project contains a total of <> lines of code.".format(print_number(nbLines(files))))
