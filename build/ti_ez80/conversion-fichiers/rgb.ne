#NEON
initGraphics()

draw(Rect(x:0, y:0, width:320, height:240, color:rgb(255, 0, 0), filled:True))

await(getKey() == 15)

draw(Rect(x:0, y:0, width:320, height:240, color:rgb(0, 255, 0), filled:True))

await(getKey() == 15)

draw(Rect(x:0, y:0, width:320, height:240, color:rgb(0, 0, 255), filled:True))

await(getKey() == 15)
