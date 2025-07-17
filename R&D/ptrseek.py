f = open("data.txt", "r")
string = f.read()
f.close()

string = '["' + string.replace(" ", "\", \"") + '"]'
string = string.replace("\n", "")

array = eval(string)

nonfreed = []

for element in array:
    c = array.count(element)
    element2 = 'f' + element[1:]
    c2 = array.count(element2)
    if c != c2 and not element[1:] in nonfreed:
        nonfreed.append(element[1:])
        print(element[1:])
