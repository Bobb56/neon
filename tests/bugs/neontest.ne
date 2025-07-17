# ceci est un module de tests écrit en Neon

function maxLength(list) do
    max = -1
    for (i, 0, len(list)) do
        if (len(list[i]) > max) then
            max = len(list[i])
        end
    end
    return (max)
end

function min(a, b) do
    if (a < b) then
        return (a)
    else
        return (b)
    end
end

# affiche un seul programme
function printProg(percent, name, maxlength, successful) do
    spaces = 4-len(percent)
    spaces_right = spaces//2
    spaces_left = spaces - spaces_right
    
    output('[' + ' '*spaces_left) ; setColor("green") ; output(percent) ; setColor("white") ; output(' '*spaces_right + '] ')
    output('Test ', name, (maxlength - len(name) + 1)*' ')
    
    if (successful == None) then
        setColor('green')
        output('Running...')
    elif (successful) then
        setColor('blue')
        output('Success')
    else
        setColor('red')
        output('Failed.')
    end
    setColor('white')
end

# affiche le programme n°n en prenant en compte le programme actuel comme programme n°len(screen>>passed)
# ne fait rien si n dépasse le nombre de programmes
function printNthProg(screen, n) do
    if (n < len(screen>>passed)) then
        printProg(str(int((n+1)/screen>>nbtests*100))+'%', screen>>passed[n]>>name, screen>>maxlength, screen>>passed[n]>>successful)
    elif (screen>>current != None and n == len(screen>>passed)) then
        printProg('....', screen>>current, screen>>maxlength, None)
    end
end


# cette fonction affiche toutes les lignes de l'affichage (donc plusieurs programmes sur la même ligne potentiellement)
function printLines(screen, nblines := 20, nbcolumns := 2) do
    nbrealcolumns = (len(screen>>passed)) // nblines + 1
    
    for (i, 0, min(nblines, len(screen>>passed) + 1)) do
        for (j, 0, min(nbcolumns, nbrealcolumns)) do # le nombre effectif de colonnes
            printNthProg(screen, i + j*nblines)
            if (j < nbrealcolumns - 1) then
                output('    │    ')
            end
        end
        print()
    end
end


function launch(file) do
    local(i) # le programme peut utiliser i, ça ne modifiera pas le i de la boucle
    setAtomicTime(1)
    import("files/" + file)
    gc() # garbage collection
end


function Screen~create(files) do
    return (Screen(passed: [], remaining: files.copy(), current: None, maxlength: maxLength(files), errors: 0, nbtests: len(files)))
end



function Screen~display(screen, clearScr := True) do
    if (clearScr) then
        clear()
    end
    
    print(' '*30, "Neon tests suite running :")
    print()
    
    printLines(screen)
    
    if (screen>>remaining == [] and screen>>current == None) then
        if (screen>>errors == 0) then
            print("All tests ran successfully !")
        else
            print(len(screen>>passed) - screen>>errors, 'tests successful,', screen>>errors, 'tests failed.')
        end
    end
end


function Screen~runTests(screen, clearScr := True) do
    while (len(screen>>remaining) > 0) do
        screen>>current = screen>>remaining[0]
        screen>>remaining.remove(0)
        
        screen.Screen~display(clearScr := clearScr)
        
        try
            launch(screen>>current)
            screen>>passed.append(Passed(name: screen>>current, successful: True))
        except (KeyboardInterrupt) do
            break
        except () do
            screen>>passed.append(Passed(name: screen>>current, successful: False))
            screen>>errors ++
        end
        
        screen>>current = None
    end
    screen.Screen~display(clearScr := clearScr)
end


function main(args) do
    screen = Screen~create (
        ["ackermann",
            "arbres",
            "atomic",
            "chatverse",
            "cmplx",
            "dico",
            "error_parallel",
            "join",
            "liste-n-ieme",
            "local_parallel",
            "mat",
            "nw",
            "parallel",
            "premiers",
            "queue",
            "queue2",
            "stack",
            "poly",
            "set",
            "variables_arguments",
            "gc",
            "lambda",
            "prog"
        ]
    )
    print(screen)
    screen.Screen~runTests(clearScr := args==[] or args[0] != 'log')
end


main(__args__)
