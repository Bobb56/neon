init(Graphics)

text = Text(text: "This is text", x:0, y:100, fgcolor: 0, bgcolor: 255, size:7)
text>>x = 160 - text.getTextWidth()/2
draw(text)
await(getKey() == 15)
