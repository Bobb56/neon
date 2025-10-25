#NEON
initGraphics()
# global definitions
setTextTransparentColor(1)
white=rgb(255,255,255)
green = rgb(0,100,0)
black = rgb(0,0,0)
vertical_limit = 15
header = [Rect(x:0,y:0,width:320,height:20,color:green,filled:True),Text(text:"Neon launcher",x:10,y:5,fgcolor:white,bgcolor:1,size:1)]
back = Rect(x:0,y:20,width:320,height:220,color:white,filled:True)

function texty(i) do return (28+i*14) end

function make_item(i, name) do
  return(Text(text:name,x:10,y:texty(i),fgcolor:black,bgcolor:white,size:1))
end

function make_cursor(i, name) do
  return([Rect(x:7,y:texty(i)-3,width:306,height:14,color:green,filled:True),Text(text:name,x:10,y:texty(i),fgcolor:white,bgcolor:green,size:1)])
end


function getmenu(files, i) do
  if (len(files) <= vertical_limit//2) then
    i1 = 0
    i2 = len(files)-1
  elif (i < vertical_limit//2) then
    i1 = 0
    i2 = vertical_limit-1
  elif (i >= len(files) - vertical_limit//2) then
    i1 = len(files)-vertical_limit
    i2 = len(files)-1
  else
    i1 = i+vertical_limit//2-vertical_limit+1
    i2 = i+vertical_limit//2
  end
  objects = []
  for (j,i1,i2+1) do
    if (i==j) then
      objects.append(make_cursor(j-i1, files[j]))
    else
      objects.append(make_item(j-i1, files[j]))
    end
  end
  return (objects)
end

function Launcher~launch(file) do
  draw(Rect(x:100,y:100,width:100,height:20,color:green,filled:True),Text(text:"Please wait...",x:105,y:105,fgcolor:white,bgcolor:green,size:1))
  local(i,files,header,white,green,black,vertical_limit,key,refresh,texty,make_item,make_cursor,getmenu)
  try
    import(file)
  except(ExitSignal) do
    pass
  end
end



function Launcher~main() do
  files=detectFiles("#NEON")+detectFiles("NEON"+chr(0))
  files.remove(files.index("LAUNCHER"))
  files.sortAsc()
  
  # display header
  draw(header)
  
  if (len(files)==0) then
    draw(Text(text:"No Neon script found",x:70,y:140,fgcolor:black,bgcolor:255,size:1))
    await(getKey() == 15)
  else
    i = 0
    refresh=True
    while ((getKey()->key) != 15) do
      if (key==1) then
        i = (i+1)%len(files)
        refresh=True
      elif (key==4) then
        i = (len(files)+i-1)%len(files)
        refresh=True
      elif (key==9) then
        Launcher~launch(files[i])
        refresh=True
      end
      if (refresh) then print("aaaaaaaaaaaaa\n") ; draw(header,back,getmenu(files,i)) ; print("aaaaaaaaaaaaa\n"); refresh=False end
    end
  end
end

Launcher~main()
