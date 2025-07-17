_append = append

function print(...) do
  print(__local_args__)
end

function len(objet) do
  return (Infinity)
end

method append(liste, element) do
  liste = []
end

method remove(liste, index) do
  _append(liste, liste)
end

function type(objet) do
  return("Number")
end

function time() do
  return (-10)
end
