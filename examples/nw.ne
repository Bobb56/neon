# module de simulation de réseau

$
chaque processus a :
une file des messages qu'il recoit. Recevoir un message consiste à défiler un élément
envoyer un message à un processus consiste à enfiler un élément dans la file du processus
$

nb_nodes = -1
nodes = []

function nothing() do pass end

function create_node() do
    nb_nodes++;
    nodes.append(Node(id: nb_nodes, neighbours: [], queue: [], func: nothing))
    return (nb_nodes)
end

function link(n1, n2) do
    nodes[n1]>>neighbours.append(n2)
end

function send_message(n, message) do
    nodes[n]>>queue.append(message)
end

function read_message(n) do
    if (len(nodes[n]>>queue) == 0) then return (None)
    else
        message = nodes[n]>>queue[0]
        nodes[n]>>queue.remove(0)
        return (message)
    end
end

function set_node_function(n, f) do
    nodes[n]>>func = f
end

function launch_network() do
    print("Début de la simulation...")
    promises = []
    for (i, 0, len(nodes)) do
        f = nodes[i]>>func
        promises.append(parallel f())
    end
    
    await(promises == [None] * len(nodes))
    print("Fin de la simulation.")
end


if (__name__ == '__main__') then
    n1 = create_node()
    n2 = create_node()
    n1.link(n2)
    n2.link(n1)
    
    function main_n1() do
        print("Lancement du processus du noeud 1")
        local(message)
        send_message(n2, "coucoun n2")
        await ((n1.read_message() -> message) != None)
        print("n1 a recu", message)
    end
    
    function main_n2() do
        print("Lancement du processus du noeud 2")
        local(message)
        send_message(n1, "coucoun n1")
        await ((n2.read_message() -> message) != None)
        print("n2 a recu", message)
    end
    
    n1.set_node_function(main_n1)
    n2.set_node_function(main_n2)
    
    launch_network()
end
