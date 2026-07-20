#init(Graphics)
import('graphics')

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



ori=0

x=1
y=1
speed=1

r=speed

function anim() do
  r0=Rect(x:0,y:0,w:320,h:240,c:0,f:True)
  draw(r0)
  w = 1
  while (w < 318) do
    h = w/4*3
    r1=Rect(x:160-w/2, y:120-h/2, w:w, h:h, c:255, f:True)
    draw(r1)
    w+=(320-w)/50 + 5
  end
  r0>>f = False
  draw(r0)
end



function fill(x,y) do
  c=randint(1,256)
  i=0
  while (getPixel(x,y)==0 and i<10) do
    x++
    y++
    i++
  end
  if (x<320 and x>=0 and y<240 and y>=0) then
    draw(FloodFill(x:x,y:y,c:c))
  end
end

function addEvent() do
  if (randint(0,2)==0) then
    k=randint(1,5)
  else
    k=0
  end
  ttl=randint(0,20)
  return([k,ttl/speed])
end

method orienter(angle,target) do
  d1=((angle+360)-target)%360
  d2=((target+360)-angle)%360

  if (d1>d2) then
    if (angle<=0) then
      angle=360-3*speed
    else
      angle-=3*speed
    end
  else
    if (angle>=360) then
      angle=3*speed
    else
      angle+=3*speed
    end
  end
end

anim()
event=addEvent()
while ((getKey()->k) != 9) do
  if (k==0) then
    k=event[0]
  end
  event[1]--
  if (event[1]<=0) then
    event=addEvent()
  end

  if (k==1) then
    ori.orienter(270)
  elif (k==2) then
    ori.orienter(0)
  elif (k==3) then
    ori.orienter(180)
  elif (k==4) then
    ori.orienter(90)
  end

  x0=x
  y0=y
  x+=r*cos(rad(ori))
  y+=r*sin(rad(ori))
  if (x>319) then x=0 end
  if (x<0) then x=319 end
  if (y>239) then y=0 end
  if (y<0) then y=239 end

  if ((int(x)!=int(x0) or int(y)!=int(y0)) and getPixel(int(x),int(y))==0) then
    fill(int(x),int(y))
  end
  setPixel(int(x),int(y),0)
end


await(getKey()==15)
