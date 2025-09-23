initGraphics()

await(getKey() == 15)

for (i, 0, 240) do
    for (j, 0, 320) do
        setPixel(j, i, i+j+1)
    end
end

await(getKey() == 15)

print("Hello, World!")
input()