

function Screen~create(files) do
    return (1)
end




function Screen~runTests(screen, clearScr := True) do
    print(screen)
end


function main(args) do
    screen = None
    screen.Screen~runTests(clearScr := False)
end


main(__args__)
