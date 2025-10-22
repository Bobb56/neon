# programme de gestion d'argent de monopoly

createException("ErrorInvalidCommand")


joueurs = []
argent = []


function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banque.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Données précédentes chargées : " + aff + "]")
    except (FileNotFound) do
        joueurs = ["banque"]
        argent = [1 EE 6]
        print("[Nouvelle partie commencée]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banque.log")
        print("[Commandes précédentes lancées : ]")
        print(txt)
    except (FileNotFound) do
        writeFile("banque.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banque.dat", data)
end



function save_command(command) do
    contenu = readFile("banque.log")
    writeFile("banque.log", contenu + command + '\n')
end



function char(string, index) do
    return (sub(string, index, index+1))
end


function index(list, el) do
    local (i)
    i = 0
    while (i < len(list)) do
        if (list[i] == el) then return(i) end
        i++
    end
    raise(OutOfBounds, "Cet élément n'existe pas")
end

function parse(string) do # prend en argument une commande entrée et renvoie la liste des arguments
    string += ' '
    deb = 0
    retour = []
    for (i, 0, len(string)) do
        if (string.char(i) == ' ') then
            retour.append(sub(string, deb, i))
            deb = i+1
        end
    end
    return (retour)
end


method traiter_commande(commande) do # prend une liste d'arguments et transforme ça en commande lisible

    if (commande[0] == 'move') then
        if (len(commande) != 4 or not(commande[1] in joueurs) or not(commande[1] in joueurs)) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
                output(commande[i] + ' : ')
                print(argent[joueurs.index(commande[i])])
            end
        end
    elif (commande[0] == 'hack') then
        string = input("NEON>> ")
        while (string != 'exit') do
            try
                print(eval(string))
            except () do
                print("Erreur de Neon")
            end
            string = input("NEON>> ")
        end
    
    elif (commande[0] == 'give') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] += nbr(commande[2])
    elif (commande[0] == 'exit') then
        exit()
    elif (commande[0] == 'addplayer') do
        if (not(commande[1] in joueurs)) then
            joueurs.append(commande[1])
            if (len(commande) > 2) then # la somme d'argent de ce joueur
                argent.append(nbr(commande[2]))
            else
                argent.append(0)
            end
        else
            print("Joueur déjà existant")
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banque"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banque.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Cette commande est invalide") end
        argent[joueurs.index(commande[1])] -= nbr(commande[2])
    elif (commande[0] == 'calc') do
        local(string)
        string = ""
        for (i, 1, len(commande)) do
            string += commande[i] + ' '
        end
        try
            print(eval(string))
        except () do
            raise(ErrorInvalidCommand, "Cette commande est invalide")
        end
    else
        raise(ErrorInvalidCommand, "Cette commande est invalide")
    end
end




function main() do
    print("Bienvenue sur la programme de gestion de banque de monopoly. Voici la configuration initiale :")
    print("Tapez move personne1 personne2 argent pour effectuer un transfert de personne 1 vers personne2 d'une somme de argent.")
    print("Tapez see personne1 personne2 ... pour voir l'argent de personne1, personne2, ...")
    print("Tapez give personne argent pour ajouter de l'argent à personne1")
    print("Tapez addplayer nom argent_initial pour ajouter un joueur")
    print("Tapez deleteplayer nom1 nom2 ... pour supprimer des joueurs")
    print("Tapez exit pour quitter.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("Veuillez retaper votre commande car celle-ci est invalide")
        end
        save_data(joueurs, argent)
    end
end



