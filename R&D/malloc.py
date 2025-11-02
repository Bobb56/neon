T1 = [[0 for i in range(5)] for i in range(10)]
T2 = [i for i in range(10)]
T3 = [i for i in range(10)]

PTR = 0

def malloc(): # malloc renvoie un entier correspondant à l'indice du tableau de T1 auquel on a le droit
    global PTR
    PTR += 1
    return T3[PTR-1]




def free(ARG):
    global PTR
    
    PTR -= 1

    temp = T3[T2[ARG]]
    T3[T2[ARG]] = T3[PTR]
    T3[PTR] = temp
    
    T2[T3[T2[ARG]]] = T2[ARG]
    T2[ARG] = PTR


