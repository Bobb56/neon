init(Graphics)

function lose() do
    text = Text(t: "You lose!", x:0, y:100, fg: 192, bg: 255, s:7)
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
            if (apple>>figure>>x >= screen>>pad>>x and apple>>figure>>x <= screen>>pad>>x + screen>>pad>>w) then # catched
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
            figure: Circle(x:randint(5, 235), y:0, r:5, c: 192, f:True),
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
    screen>>score>>t = "Score: " + str(score)
    screen>>hscore>>t = "High score: " + str(highscore)
end

function load_highscore() do
    try
        return(loadObj(APPLSDAT))
    except () do
        return(0)
    end
end

function save_highscore() do
    saveObj(APPLSDAT, highscore)
end



function main() do
    APPLSDAT = "ApplsDAT"
    max_apples = 2
    apple_max_speed = 3
    pad_speed = 7
    score = 0
    highscore = load_highscore()

    screen = Screen(
        bg: Rect(x:0, y:0, w:320, h:240, c:rgb(0, 150, 100), f:True),
        pad: Rect(x:145, y:220, w:55, h:5, c:0, f:True),
        apples: [
            Apple(
                figure: Circle(x:randint(5, 235), y:0, r:5, c: rgb(255, 0, 0), f:True),
                speed: 1
            ),
            Apple(
                figure: Circle(x:randint(5, 235), y:0, r:5, c: rgb(255, 0, 0), f:True),
                speed: 2
            )
        ],
        score: Text(t: "Score: " + str(score), x:5, y:5, fg: 0, bg:255, s:1),
        hscore: Text(t: "High score: " + str(highscore), x:5, y:15, fg: 0, bg:255, s:1)
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
