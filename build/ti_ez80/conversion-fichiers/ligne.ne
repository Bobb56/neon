initGraphics()

ori=0

x=1
y=1
speed=1

r=speed

function anim() do
  r0=Rect(x:0,y:0,width:320,height:240,color:0,filled:True)
  draw(r0)
  w = 1
  while (w < 318) do
    h = w/4*3
    r1=Rect(x:160-w/2, y:120-h/2, width:w, height:h, color:255, filled:True)
    draw(r1)
    w+=(320-w)/50 + 5
  end
  r0>>filled = False
  draw(r0)
end



function fill(x,y) do
  c=randint(1,256)
  i=0
  while (getPixel(x,y)==0 and i<10) do
    x++
    y++
    i++
  end
  if (x<320 and x>=0 and y<240 and y>=0) then 
    draw(FloodFill(x:x,y:y,color:c))
  end
end

function addEvent() do
  if (randint(0,2)==0) then
    k=randint(1,5)
  else
    k=0
  end
  ttl=randint(0,20)
  return([k,ttl/speed])
end

method orienter(angle,target) do
  d1=((angle+360)-target)%360
  d2=((target+360)-angle)%360
  
  if (d1>d2) then
    if (angle<=0) then
      angle=360-3*speed
    else
      angle-=3*speed
    end
  else
    if (angle>=360) then
      angle=3*speed
    else
      angle+=3*speed
    end
  end
end

anim()
event=addEvent()
while ((getKey()->k) != 9) do
  if (k==0) then
    k=event[0]
  end
  event[1]--
  if (event[1]<=0) then
    event=addEvent()
  end
  
  if (k==1) then
    ori.orienter(270)
  elif (k==2) then
    ori.orienter(0)
  elif (k==3) then
    ori.orienter(180)
  elif (k==4) then
    ori.orienter(90)
  end
  
  x0=x
  y0=y
  x+=r*cos(rad(ori))
  y+=r*sin(rad(ori))
  if (x>319) then x=0 end
  if (x<0) then x=319 end
  if (y>239) then y=0 end
  if (y<0) then y=239 end
  
  if ((int(x)!=int(x0) or int(y)!=int(y0)) and getPixel(int(x),int(y))==0) then
    fill(int(x),int(y))
  end
  setPixel(int(x),int(y),0)
end


await(getKey()==15)