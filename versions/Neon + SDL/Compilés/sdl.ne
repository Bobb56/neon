enableGraphics()

function horizLine(y) do
  for (x, 0, 1920) do
    setPixel(x, y, x+y/10, x-y/10, x*(y/10))
  end
  refreshScreen()
end

for (y, 0, 1080) do
  horizLine(y)
end

sleep(10000)

disableGraphics()
