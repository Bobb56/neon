# Programme de décomposition d'entiers en facteurs premiers


function trouver_diviseur(n) do
  for (i, 2, n//2+1) do
    if (n % i == 0) then
      return (i)
    end
  end
  
  return (n)
end


function decomp(n) do
  facteurs = []
  while ((trouver_diviseur(n) -> nfact) != n) do
    facteurs.append(nfact)
    n = n//nfact
  end
  facteurs.append(nfact)
  return (facteurs)
end
