init(Graphics)

for (y, 0, 16) do
    for (x, 0, 16) do
        draw(Rect(x:x*15, y:y*15, w:15, h:15, c:y*16+x, f:True),
             Rect(x:x*15, y:y*15, w:15, h:15, c:0, f:False)
        )
    end
end

await(getKey() == 15)
