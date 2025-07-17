function join(process) do
    while (type(process) == "Promise") do pass end
end

function processus() do
    i = 0;
    while (i < 100) do i++ end
end

processes = Process(p1:None, p2:None, p3:None)


processes>>p1 = parallel processus()
processes>>p2 = parallel processus()
processes>>p3 = parallel processus()


processes>>p1 = parallel processus()
processes>>p2 = parallel processus()
processes>>p3 = parallel processus()


processes>>p3.join()
processes>>p2.join()
processes>>p1.join()


processes>>p2 = parallel processus()

processes>>p2.join()

print("Test join passé !")
