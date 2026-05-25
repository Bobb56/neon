import ('higherorder')

serialize('compo.neobj', compo)
compo2 = deserialize('compo.neobj')

function plus1(x) do return (x+1) end
plus2 = compo2(plus1, plus1)

print(plus2(45))
