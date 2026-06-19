# programme de gestion d'argent de monopoly

createException("ErrorInvalidCommand")


joueurs = []
argent = []


function load_files() do
    # ouverture du fichier de données
    try
        txt = readFile("config_banq.dat")
        dat = eval(txt)
        joueurs = dat[0]
        argent = dat[1]
        
        aff = str(joueurs)
        aff = sub(aff, 1, len(aff) - 1)
        print("\n[Loaded previous commands : " + aff + "]")
    except (FileSystemError) do
        joueurs = ["banq"]
        argent = [1 EE 6]
        print("[New game started]")
    end
    
    #ouverture du fichier de commandes
    try
        txt = readFile("banq.log")
        print("[Previous commands : ]")
        print(txt)
    except (FileSystemError) do
        writeFile("banq.log", "")
    end
end




function save_data(joueurs, argent) do
    data = str([joueurs, argent])
    writeFile("config_banq.dat", data)
end



function save_command(command) do
    contenu = readFile("banq.log")
    writeFile("banq.log", contenu + command + '\n')
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
            raise(ErrorInvalidCommand, "Invalid command")
        else
            commande[3] = nbr(commande[3])
            argent[joueurs.index(commande[2])] += commande[3]
            argent[joueurs.index(commande[1])] -= commande[3]
        end
    
    elif (commande[0] == 'see') then
        if (len(commande) < 2) then
            raise(ErrorInvalidCommand, "Invalid command")
        else
            for (i, 1, len(commande)) do
                if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Invalid command") end
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
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Invalid command") end
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
            raise(ErrorInvalidCommand, "Invalid command")
        end
    elif (commande[0] == 'deleteplayer') do
        for (i, 1, len(commande)) do
            if (not(commande[i] in joueurs)) then raise(ErrorInvalidCommand, "Invalid command") end
            ind = joueurs.index(commande[i])
            joueurs.remove(ind)
            argent.remove(ind)
        end
    elif (commande[0] == 'newgame') do
        joueurs = ["banq"]
        argent = [1 EE 6]
        save_data(joueurs, argent)
        writeFile("banq.log", "")
        clear()
        print("[Nouvelle partie commencée]")
    elif (commande[0] == 'remove') do
        if (not(commande[1] in joueurs)) then raise(ErrorInvalidCommand, "Invalid command") end
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
            raise(ErrorInvalidCommand, "Invalid command")
        end
    else
        raise(ErrorInvalidCommand, "Invalid command")
    end
end




function main() do
    print("Welcome to the Monopoly bank management program.")
    print("Type: move person1 person2 amount to transfer an amount from person1 to person2.")
    print("Type: see person1 person2 ... to view the money of person1, person2, ...")
    print("Type: give person amount to add money to person1.")
    print("Type: addplayer name initial_amount to add a player.")
    print("Type: deleteplayer name1 name2 ... to remove players.")
    print("Type: exit to quit.")
    
    load_files()
    
    while (True) do
        string = input("BANQ> ")
        
        if (string == "") then continue end
        
        commande = parse(string)
        try
            commande.traiter_commande()
            save_command("-> " + string)
        except (ErrorInvalidCommand) do
            print("This command has not been recognized as a valid command")
        end
        save_data(joueurs, argent)
    end
end



main()
