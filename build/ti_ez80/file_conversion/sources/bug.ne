keys=[0,0,0,0,0,4,15,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,0,0,0,15,0,0,0]

function getKey() do
  if (keys==[]) then
    key=0
  else
    key=keys[0]
    keys.remove(0)
  end
  return (key)
end


function load_highscore() do
    try
        return(loadObj(APPLSDAT))
    except () do
        return(0)
    end
end



function main() do
    APPLSDAT = "ApplsDAT"
    max_apples = 2
    apple_max_speed = 3
    pad_speed = 7
    score = 0
    highscore = load_highscore()

    screen = Screen(
        bg: Rect(x:0, y:0, w:320, h:240, c:100, f:True),
        pad: Rect(x:145, y:220, w:55, h:5, c:0, f:True),
        apples: [
            Apple(
                figure: Circle(x:randint(5, 235), y:0, r:5, c: 255, f:True),
                speed: 1
            ),
            Apple(
                figure: Circle(x:randint(5, 235), y:0, r:5, c: 255, f:True),
                speed: 2
            )
        ],
        score: Text(t: "Score: " + str(score), x:5, y:5, fg: 0, bg:255, s:1),
        hscore: Text(t: "High score: " + str(highscore), x:5, y:15, fg: 0, bg:255, s:1)
    )

    counter = 0
    while ((getKey() -> key) != 15) do end
end


main()

await(getKey()==15)
