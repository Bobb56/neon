# fichier de test destiné à tester le garbage collector (en tous cas à vérifier qu'il crashe pas, c'est les fuites mémoire qui vont vérifier s'il fonctionne effectivement)

l = [0]
l.append(l) # objet auto-référençant

l' = l.copy() # copie profonde conservant les dépendances
l'[0] = 1

l = 4 # plus rien ne pointe sur l

gc()

assert(l'[0] == 1)

l' = None

gc()

