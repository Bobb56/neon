function do_nothing(...) do pass end
function make_constant(constant) do
    function func(..., constant := constant) do
        return (constant)
    end
    return (func)
end

function make_random(a, b) do
    function func(..., a := a, b := b) do
        return (randint(a, b))
    end
    return (func)
end


draw = do_nothing
setPixel = do_nothing
getPixel = make_random(0, 256)
getTextWidth = make_random(8, 50)
rgb = make_random(0, 256)
getKey = make_random(0, 100)
