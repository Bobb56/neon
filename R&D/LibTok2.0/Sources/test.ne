try
  while (True) do end
except(OutOfRange, KeyboardInterrupt) do
  print("Hors d'atteinte")
except (DivisionByZero, IncorrectType) do
  print("Division par zéro")
end
