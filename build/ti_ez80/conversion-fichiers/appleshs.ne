initGraphics()



function lose() do
    text = Text(text: "You lose!", x:0, y:100, fgcolor: 192, bgcolor: 255, size:7)
    text>>x = 160 - text.getTextWidth()/2
    draw(text)
    await(getKey() == 15)
    exit()
end

function update_apples(apples) do
    apple_index = 0
    
    while (apple_index < len(apples)) do
        apple = apples[apple_index]
        
        apple>>figure>>y += apple>>speed
        
        # checks if the apple is catched by the pad
        if (apple>>figure>>y > 222) then
            if (apple>>figure>>x >= screen>>pad>>x and apple>>figure>>x <= screen>>pad>>x + screen>>pad>>width) then # catched
                apples.remove(apple_index)
                score ++
                if (score > highscore) then
                    highscore = score
                end
            else
                save_highscore()
                lose()
            end
        end
        apple_index ++
    end
end

function eventually_add_apple(apples) do
    if (randint(0, 30) == 0) then
        if (apples != []) then
            m = 0
            foreach (apple, apples) do
                if (apple>>speed > m) then
                    m = apple>>speed
                end
            end
        else
            m = 1
        end
        
        apple = Apple(
            figure: Circle(x:randint(5, 235), y:0, radius:5, color: 192, filled:True),
            speed: m % apple_max_speed + 1
        )
        screen>>apples.append(apple)
    end
end

function update_pad(key) do
    if (key == 2) then # left
        screen>>pad>>x -= 7
    elif (key == 3) then # right
        screen>>pad>>x += 7
    end
end

function update_score() do
    screen>>score>>text = "Score: " + str(score)
    screen>>hscore>>text = "High score: " + str(highscore)
end

function load_highscore() do
    try
        return(int(readFile(APPLSDAT)))
    except () do
        return(0)
    end
end

function save_highscore() do
    writeFile(APPLSDAT, str(highscore))
end



function main() do
    APPLSDAT = "ApplsDAT"
    max_apples = 2
    apple_max_speed = 3
    pad_speed = 7
    score = 0
    highscore = load_highscore()
    
    screen = Screen(
        bg: Rect(x:0, y:0, width:320, height:240, color:15, filled:True),
        pad: Rect(x:145, y:220, width:55, height:5, color:0, filled:True),
        apples: [
            Apple(
                figure: Circle(x:randint(5, 235), y:0, radius:5, color: 192, filled:True),
                speed: 1
            ),
            Apple(
                figure: Circle(x:randint(5, 235), y:0, radius:5, color: 192, filled:True),
                speed: 2
            )
        ],
        score: Text(text: "Score: " + str(score), x:5, y:5, fgcolor: 0, bgcolor:255, size:1),
        hscore: Text(text: "High score: " + str(highscore), x:5, y:15, fgcolor: 0, bgcolor:255, size:1)
    )
    
    counter = 0
    while ((getKey() -> key) != 15) do
        update_pad(key)
        update_apples(screen>>apples)
        update_score()
        
        if (len(screen>>apples) < max_apples) then # adds an apple
            eventually_add_apple(screen>>apples)
        end
        
        draw(screen)
        counter ++
        if (counter%200 == 0) then
            pad_speed++
            apple_max_speed++
        end
    end
end


main()
