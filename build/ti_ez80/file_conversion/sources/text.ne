init(Graphics)

text = Text(t: "This is text", x:0, y:100, fg: 0, bg: 255, size:7)
text>>x = 160 - text.getTextWidth()/2
draw(text)
await(getKey() == 15)
