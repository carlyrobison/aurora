-- author("Jake Ketchum")
-- This is a port of the original "binary" pattern. 
-- This is a lot less efficient though.
add_param("step_delay",0,1,0.01,0.05)

count = 0
bit_state = {}
for i=1, N_LEDS do
    bit_state[i] = 0
end

function update()
    -- Toggle the 0 bit. 
    if (count > step_delay * 50)
    then
        count = 0
        for i=1, N_LEDS do
            if (bit_state[i] == 0)
            then 
                bit_state[i] = 1
                break
            end 
            bit_state[i] = 0
        end
    end
    for i=1, N_LEDS do
        setRGB(i,bit_state[i],0,0)
    end

    -- Update Count. 
    count = count + 1
end