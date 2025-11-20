tokens = ["chat", "chats", "souris", "souris", "soleil", "pluie", "court", "marche", "mange", "mangée", "mangee"]
codes = ["case", "case", "pointeur", "pointeur", "print", "input", "-", "-", "+", "+", "+"]


function char(chaine, indice) do
    return (sub(chaine, indice, indice+1))
end



# une première fonction qui sépare les phrases grâce aux points
function sep_phrases(text) do
    local(i)
    phrases = []
    deb = 0
    for (i, 0, len(text)) do
        if (text.char(i) == '.') then
            phrases.append(text.sub(deb, i))
            deb = i+1
        end
    end
    return ( phrases )
end


# une fonction qui prend en argument une phrase (suite de mots séparés par des espaces) et qui renvoie la liste des mots
function sep_mots(phrase) do
    local(i)
    mots = []
    deb = 0
    
    i = 0
    while (i < len(phrase)) do
        if (phrase.char(i) == ' ') then
            mots.append(phrase.sub(deb, i))
            deb = i+1
        end
        i++;
    end
    
    return ( mots + [phrase.sub(deb, i)] )
end





function est_chiffre(car) do
    return ( ord(car) >= 48 and ord(car) <= 57)
end



function est_nombre(str) do
    local(i)
    foreach (i, str) do
        if (not est_chiffre(i)) then
            return (False)
        end
    end
    return (len(str) > 0)
end


function nettoyer_phrase(phrase) do # enlève les mots inutiles d une phrase envoyée sous forme d une liste
    local(i,nouvliste)
    nouvliste = []
    foreach (word, phrase) do
        if (word in tokens or est_nombre(word)) then
            nouvliste.append(word)
        end
    end
    return (nouvliste)
end


function decoder_mot(mot) do
    if (mot in tokens) then # c est pas un nombre
        return (codes[tokens.index(mot)])
    else
        return (nbr(mot))
    end
end


function decoder_phrase(phrase) do
    local(i)
    l = []
    for (i, 0, len(phrase)) do
        l.append(decoder_mot(phrase[i]))
    end
    return (l)
end


function decoder_prog(text) do # prend en argument une liste de phrases et renvoie la liste des phrases décodées
    local(i, l, nouvliste)
    liste = sep_phrases(text)
    nouvliste = []
    for (i, 0, len(liste)) do
        l = sep_mots(liste[i])
        l = nettoyer_phrase(l)
        nouvliste.append(decoder_phrase(l))
    end
    return (nouvliste)
end


# type commande : Commande(verbe: '+', arg: 56)


function calc_commande(phrase) do # prend en argument une phrase décodée et renvoie de quelle commande il s agit
    if ("print" in phrase) then
        return (Commande(verbe: '.', arg: None))
    elif ("input" in phrase) then
        return (Commande(verbe: ',', arg: None))
    elif (("case" in phrase) and ("pointeur" in phrase)) then # deux cas possibles : début ou fin de boucle
        if (phrase[0] == "case") then
            return (Commande(verbe: '[', arg: None))
        else
            return (Commande(verbe: ']', arg: None))
        end
    elif (phrase[0] == 'case') then
        return (Commande(verbe: phrase[1], arg: phrase[2]))
    else
        if (phrase[1] == '+') then
            return (Commande(verbe: '>', arg: phrase[2]))
        else
            return (Commande(verbe: '<', arg: phrase[2]))
        end
    end
end



function compile(prog) do
    l = decoder_prog(prog)
    local(nouvliste,i)
    nouvliste = []
    for (i, 0, len(l)) do
        nouvliste.append(calc_commande(l[i]))
    end
    
    return (nouvliste)
end






function run(code, input) do
    out = ""
    
    tape = [] ; for (i, 0, 1000) do append(tape, 0) end
    pointer = 0
    loop_stack = []
    
    for (instruction, 0, len(code), 1) do
        commande = code[instruction]
    
        if (commande>>verbe == '>') then pointer += commande>>arg
        
        elif (commande>>verbe == '<') then pointer -= commande>>arg
        
        elif (commande>>verbe == '+') then tape[pointer] = tape[pointer] + commande>>arg
        
        elif (commande>>verbe == '-') then tape[pointer] = tape[pointer] - commande>>arg
        
        elif (commande>>verbe == '.') then out += chr(tape[pointer]%256)
        
        elif (commande>>verbe == ',') then tape[pointer] = input[0] ; input.remove(0)
        
        elif (commande>>verbe == '[') then
            if (tape[pointer] == 0) then
                loop_count = 1
                while (loop_count > 0) do
                    instruction ++
                    if (sub(code, instruction, instruction+1) == '[') do loop_count ++
                    elif (sub(code, instruction, instruction+1) == ']') do loop_count -- end
                end
            else
                loop_stack.append(instruction)
            end
        
        elif (commande>>verbe == ']') do
            if (tape[pointer] != 0) do instruction = loop_stack[len(loop_stack)-1] - 1
            else remove(loop_stack, len(loop_stack)-1) end
        end
    end
    return (out)
end




prog = readFile("files/data/hello_world.txt")
code = compile(prog)
out = run(code, [])
assert(out == "Hello World !")

