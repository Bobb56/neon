files = ["ackermann",
        "arbres",
        "atomic",
        "cmplx",
        "dico",
        "error_parallel",
        "join",
        "liste-n-ieme",
        "local_parallel",
        "mat",
        "parallel",
        "premiers",
        "queue",
        "stack"]

for (i, 0, len(files)) do
    import("tests/" + files[i])
end
