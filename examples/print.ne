function print(..., END := '\n') do
  for (i, 0, len(__local_args__)) do
    output(__local_args__[i])
    if (i < len(__local_args__) - 1) then
      output(' ')
    end
  end
  output(END)
end
