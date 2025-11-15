-- This is a port of the original "Purple" pattern. 

add_param("speed",-1,1,0.1,-0.3)
add_param("brightness",0,1,0.01,1)

g = 0
speed_mod = .15 -- magic number to match original speeds.

function update()
    for i=1, N_LEDS do
        if((i + math.floor(g+0.5)) % 3 == 0)
        then
            setRGB(i, 0.5*brightness, 0, 0.5*brightness)
        else
            setRGB(i,0,0,0)
        end
    end
    g = (g+1*speed*speed_mod) % 3
end
