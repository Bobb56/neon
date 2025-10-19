initGraphics()

for (y, 0, 16) do
    for (x, 0, 16) do
        draw(Rect(x:x*15, y:y*15, width:15, height:15, color:y*16+x, filled:True),
             Rect(x:x*15, y:y*15, width:15, height:15, color:0, filled:False)
        )
    end
end

await(getKey() == 15)
