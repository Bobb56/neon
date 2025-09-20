function f(a) do
  for (i, 0, 100) do
    output(a)
  end
end

parallel f(1)
parallel f(2)
parallel f(3)
