function exp(a, n) do
  if (n == 0) then
    return (1)
  elif (n%2 == 0) then
    return (exp(a, n/2)**2)
  else
    return (a * exp(a, n//2)**2)
  end
end
