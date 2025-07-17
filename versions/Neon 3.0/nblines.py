files = [
    "affprog.c",
    "builtinfonc.c",
    "errfonc.c",
    "headers.c",
    "liblist.c",
    "libstr.c",
    "libtok.c",
    "libtree.c",
    "main.c",
    "neonio.c",
    "numbers.c",
    "opfonc.c",
    "varlib.c"
]

def nbLines(fichier):
    f = open(fichier, "r+")
    n = len(f.readlines())
    f.close()
    return n


n = 0
for fichier in files:
    n += nbLines(fichier)

n_linenoise = n + nbLines("linenoise.c")

print(f"Le projet Neon compte {n} lignes de code sans linenoise.c, mais {n_linenoise} lignes en tout.")
