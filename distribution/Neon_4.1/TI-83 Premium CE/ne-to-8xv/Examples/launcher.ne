#NEON Neon launcher 1.0
initGraphics()

files=detectFiles("#NEON")+detectFiles("NEON"+chr(0))
files.remove(files.index("LAUNCHER"))
files.sortAsc()

while (True) do
  file = menu("Neon launcher 1.0", files, "No Neon scripts found")
  if (file == None) then
    break
  else
    safeExec(file, [])
  end
end
