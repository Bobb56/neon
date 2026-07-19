init(Graphics)

draw(Rect(x:0, y:0, w:320, h:240, c:rgb(255, 0, 0), f:True))

await(getKey() == 15)

draw(Rect(x:0, y:0, w:320, h:240, c:rgb(0, 255, 0), f:True))

await(getKey() == 15)

draw(Rect(x:0, y:0, w:320, h:240, c:rgb(0, 0, 255), f:True))

await(getKey() == 15)
