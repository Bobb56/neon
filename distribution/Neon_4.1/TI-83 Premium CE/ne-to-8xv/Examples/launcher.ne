#NEON Neon launcher 1.0
initGraphics()

# Préfixes qui permettent de détecter les programmes à afficher
prefixes = [
    "#NEON",
    "NEON"+chr(0),
    "PYCD"
]

# Détection des fichiers 
files=[]
foreach (prefix, prefixes) do
    files += detectFiles(prefix)
end

if ("LAUNCHER" in files) then
    files.remove(files.index("LAUNCHER"))
end
files.sortAsc()

# Boucle principale
while (True) do
  file = menu("Neon launcher 1.0", files, "No Neon scripts found")
  if (file == None) then
    break
  else
    safeExec(file, [])
  end
end