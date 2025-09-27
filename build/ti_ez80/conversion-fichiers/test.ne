initGraphics()

rect = Rect(x:0, y:95, width:50, height:50, color:0, filled:True)
blank = Rect(x:0, y:0, width:320, height:240, color:255, filled:True)

while (getKey() != 15) do
    while (rect>>x <= 263) do
        draw(blank, rect)
        rect>>x += 7
        rect>>color = (rect>>color + 1) % 256
    end
    
    while (rect>>x >= 7) do
        draw(blank, rect)
        rect>>x -= 7
        rect>>color = (rect>>color + 1) % 256
    end
end