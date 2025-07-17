NBVARS = 3

function make_var_name(index) do
    return ('x' + str(index))
end

for (i, 0, NBVARS) do
    make_var_name(i) <- i
end

env = [None] * NBVARS

createException("TooMuchVariables")



function lambda(var, expr) do
    if (var >= NBVARS) then raise(TooMuchVariables, "") end
    
    expr = expr.replace(make_var_name(var), 'env[' + str(var) + ']')
    
    function f(x, expr := expr, env := env, var := var) do
        env[var] = x
        return (eval(expr))
    end
    return (f)
end

function lambda_str(var, expr) do
    return ("lambda(" + make_var_name(var) + ', "' + expr + '")')
end

plus = lambda(x0, lambda_str(x1, "x0 + x1"))

true = lambda(x0, lambda_str(x1, "x0"))
false = lambda(x0, lambda_str(x1, "x1"))

assert(plus(40)(2) == 42)

assert(true("coucou")("hello") == "coucou")
assert(false("coucou")("hello") == "hello")
