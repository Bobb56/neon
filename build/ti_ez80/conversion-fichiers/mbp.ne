# This script draws a Mandelbrot fractal set
initGraphics()

function cmplx_mult(z1, z2) do
  return (Cmplx(re: z1>>re*z2>>re - z1>>im*z2>>im, im: z1>>re*z2>>im + z1>>im*z2>>re))
end

function cmplx_add(z1, z2) do
  return (Cmplx(re: z1>>re + z2>>re, im: z1>>im + z2>>im))
end

function cmplx_abs(z) do
  return (sqrt(z>>re*z>>re + z>>im*z>>im))
end


function mandelbrot(N_iteration := 15, res := 2, x1, y1, x2, y2) do
  local(pixel, z, c, i)
  pixel = Rect(x:0, y:0, width:res, height:res, color:0, filled:True)
  for (x, x1, x2, res) do
    for (y, y1, y2, res) do
      z = Cmplx(re:0, im:0)
      c = Cmplx(re:3.5*x/319-2.5, im:-2.5*y/239+1.25)
      i = 0
      while (i < N_iteration and cmplx_abs(z) < 2) do
        i++
        z = cmplx_add(cmplx_mult(z, z), c)
      end

      c = int(255*i/N_iteration)
      pixel>>color = rgb(c, c*0.75, c*0.25)
      pixel>>x = x
      pixel>>y = y
      draw(pixel)
      
    end
  end
end

points = [
            [0, 0],
            [0, 80],
            [0, 160],
            [0, 240],
            [80, 0],
            [80, 80],
            [80, 160],
            [80, 240],
            [160, 0],
            [160, 80],
            [160, 160],
            [160, 240]
]

foreach (co, points) do
  x = co[1]
  y = co[0]
  parallel mandelbrot(15, 4, x, y, x + 80, y + 80)
end

await(getKey() == 15)
exit()
