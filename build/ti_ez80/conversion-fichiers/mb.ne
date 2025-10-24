#NEON
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


function mandelbrot(N_iteration := 15, res := 2) do
  pixel = Rect(x:0, y:0, width:res, height:res, color:0, filled:True)
  for (x, 0, 320, res) do
    for (y, 0, 240, res) do
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
      
      if (getKey() == 15) then exit() end
    end
  end
end

mandelbrot(20, 2)

await(getKey() == 15)
