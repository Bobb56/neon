Z = Peano(v: None)

function S(n) do
  return (Peano(v: [n>>v]))
end

function Peano~str(n) do
  if (n>>v == None) then
    return ('Z')
  else
    return ('S(' + str(Peano(v: n>>v[0])) + ')')
  end
end

function Peano~repr(n) do
  output(str(n))
end

function Peano~add(a, b) do
    if (a == Z) then
        return (b)
    else
        return (S(Peano~add(Peano(v: a>>v[0]), b)))
    end
end


function Peano~mul(a, b) do
    if (a == Z) then
        return (Z)
    elif (a == S(Z)) then
        return (b)
    else
        return (Peano~add(Peano~mul(Peano(v: a>>v[0]), b), b))
    end
end


function Peano~pow(a, b) do
    if (b == Z) then
        return (S(Z))
    else
        return (Peano~mul(Peano~pow(a, Peano(v: b>>v[0])), a))
    end
end

_5 = S(S(S(S(S(Z)))))
_2 = S(S(Z))

assert(_5 * _5 == _5 ** _2)
assert(str(_2) == 'S(S(Z))')
