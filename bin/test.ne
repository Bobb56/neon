function add(x, y) do
  if (y == 0) then
    return (x)
  else
    return (add(x, y-1) + 1)
  end
end

print(add(10000, 10000))
