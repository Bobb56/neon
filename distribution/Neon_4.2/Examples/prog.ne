# mini langage de programmation interprété en Neon
eval_sov = eval


VARIABLES = []
varnames = []

# définition des fonctions built-in

function __add(l) do return(l[0] + l[1]) end
function __sub(l) do return(l[0] - l[1]) end
function __mul(l) do return(l[0] * l[1]) end
function __div(l) do return(l[0] / l[1]) end
function __inf(l) do return(l[0] < l[1]) end

function __disp(l) do
    foreach (element, l) do
        print(element)
    end
end

function __equal(l) do return (l[0] == l[1]) end
function __not(l) do return (not l[0]) end


function eval(tree) do
    local(func, objects)
    if (type(tree) == 'Function') then
        func = tree>>func
        objects = []
        foreach (expr, tree>>arguments) do
            objects.append(eval(expr))
        end
        return (func(objects))
    elif (type(tree) == 'Const') then
        return (tree>>content)
    elif (type(tree) == 'Var') then
        return (VARIABLES[tree>>var])
    end
end

function exec(tree) do
    foreach (son, tree>>sons) do
        if (type(son) == 'Affect') then
            while (son>>var >= len(VARIABLES)) then VARIABLES.append(None) end
            
            VARIABLES[son>>var] = eval(son>>expr)
        
        elif (type(son) == 'IfElse') then
            if (eval(son>>condition)) then
                exec(son>>statementif)
            elif (son>>statementelse>>sons != None) then
                exec(son>>statementelse)
            end
        elif (type(son) == 'WhileDo') then
            while (eval(son>>condition)) do
                exec(son>>statement)
            end
        else
            eval(son)
        end
    end
end




# définition des macros de syntaxe

function program(code) do
    return (SyntaxTree(sons: code))
end

function make_operator(func) do
    function operator(..., func := func) do
        return (Function(func: func, arguments: __local_args__))
    end
    return (operator)
end

add = make_operator(__add)
sub = make_operator(__sub)
mul = make_operator(__mul)
div = make_operator(__div)
equal = make_operator(__equal)
neg = make_operator(__not)
inf = make_operator(__inf)
disp = make_operator(__disp)



function affect(name, exp) do
    if (not name in varnames) then
        VARIABLES.append(None)
        varnames.append(name)
    end
    return (Affect(var: varnames.index(name), expr: exp))
end

function const(val) do
    return (Const(content: val))
end

function var(name) do
    return (Var(var: varnames.index(name)))
end

function ifelse(cond, code1, code2 := None) do
    return (IfElse(condition: cond, statementif: program(code1), statementelse: program(code2)))
end

function whiledo(cond, code) do
    return (WhileDo(condition: cond, statement: program(code)))
end


# ------------ Test de programme ------------

myProg = program([
    affect("a", const(5)),
    affect("b", add(var("a"), const(3))),
    ifelse(equal(var("b"), const(8)), [
        affect("a", add(sub(var("a"), var("b")), const(18)))
    ], [
        affect("a", mul(div(var("a"), var("b")), const(15)))
    ]),
    affect("c", const(0)),
    ifelse (neg(equal(var('a'), var('b'))), [
        affect("c", div(const(6), const(7)))
    ]),
    affect("i", const(0)),
    whiledo(inf(var('c'), const(0x7ffff)), [
        affect("c", add(var('c'), mul(var('a'), var('a')))),
        affect("i", add(var("i"), const(1)))
    ])
])


exec(myProg)

VARIABLES = listComp('i', 0, len(VARIABLES), 1, "True", "int(VARIABLES[i])")

assert(VARIABLES == [15, 8, 524475, 2331])
