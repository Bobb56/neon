init(Graphics)
r = Rect(x:0, y:0, width:320, height: 240, color:rgb(0, 0, 255), filled: True)
draw(r)
await(getKey() == 15)
