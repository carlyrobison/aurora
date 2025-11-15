-- This is a direct port of the original fire pattern used in Hell Lighting. 
-- There are a number of more sophisticated fire options available these days,
-- but this one is still a nice option for a low-key background pattern. 

add_param("brightness",0,1,0.02,0.2)
add_param("cycle_delay",0,10,1,5)

r = {}
g = {}
b = {}

for i=1, N_LEDS do
    r[i] = 0
    g[i] = 0
    b[i] = 0
end

count = 0

function update()
    if (count < cycle_delay)
    then
        count = count + 1
    else
        for i=2, N_LEDS-1 do
            if (r[i] == 0) 
            then
                r[i] = math.random(100,255)
                g[i] = math.random(0,20)
            else
                x = ((r[i-1] - r[i]) - (r[i+1] - r[i])) / 4.0
                r[i] = (x + math.random(100, 255))
                g[i] = math.random(0, 20)
                b[i] = 0
            end
        end
        count = 0
    end
    for i=1, N_LEDS do
        setRGB(r[i]*brightness/255.0,g[i]*brightness/255.0,b[i]*brightness/255.0)
    end
end
