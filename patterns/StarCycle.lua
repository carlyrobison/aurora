-- This is a direct port of the original fire pattern used in Hell Lighting. 
-- There are a number of more sophisticated fire options available these days,
-- but this one is still a nice option for a low-key background pattern. 

add_param("star_density",0,0.2,0,0.1)
-- add_param("cycle_delay",0,10,1,5)

star_state = {}
star_duration = {}

-- for i=1, N_LEDS do
--     if math.random() < star_density then 
--         star_state[i] = "Protostar"
--         star_state[i-1] = "Gas"
--         star_state[i+1] = "Gas"
--         star_state[i+2] = "Gas"
--         star_state[i-2] = "Gas"
--     else
--         star_state[i] = "Nothing"
--     end
--     star_duration[i] = 1
-- end
for i=1, N_LEDS do
    star_state[i] = "Nothing"
    star_duration[i] = 1
end
star_state[5] = "Protostar"
star_state[4] = "Gas"
star_state[6] = "Gas"


-- This is a vastly oversimplified model of stellar life cycles.
-- Stars start as protostars surrounded by gas.
-- The gas disappears as the protostars turn
-- into main sequence stars.
-- At the end of the star's life, it turns into
-- a red giant. Then it might turn into:
-- white dwarf (dim + fades)
-- supernova + neutron star
-- supernova + black hole
-- stars that supernova expel gas
function advance_star(i)
    current_star_state = star_state[i]
    current_star_duration = star_duration[i]
    if current_star_state == "Gas" and current_star_duration > 100 then
        star_state[i] = "Nothing"
        star_duration[i] = 0
    elseif current_star_state == "Protostar" and current_star_duration > 100 then
        -- advance to a real star
        star_state[i] = "Main Sequence Star"
        star_duration[i] = 0
    elseif current_star_state == "Main Sequence Star" and current_star_duration > 1000 then
        star_state[i] = "Red Giant"
        star_duration[i] = 0
    elseif current_star_state == "Red Giant" and current_star_duration > 100 then
        -- chance of something spectacular
        probability = math.random()
        if probability < 0.1 then
            star_state[i] = "Supernova"
            for j=1, 10 do
                star_state[i+j] = "Bright Gas"
                star_state[i-j] = "Bright Gas"
            end
        else
            star_state[i] = "Gas"
        end
        star_duration[i] = 0
    elseif current_star_state == "Bright Gas" then
        star_state[i] = "Nothing"
        star_duration[i] = 0
    elseif current_star_state == "Supernova" then
        if math.random() < 0.5 then
            star_state[i] = "Black Hole"
        else
            star_state[i] = "Neutron Star"
        end
        star_duration[i] = 0
    end
    -- advance time
    star_duration[i] = star_duration[i] + 1
end

function set_rgb(i)
    if star_state[i] == "Nothing" then
        setRGB(0, 0, 0)
    elseif star_state[i] == "Protostar" then
        setRGB(50, 50, 50)
    elseif star_state[i] == "Gas" then
        setRGB(10, 10, 10)
    elseif star_state[i] == "Bright Gas" then
        setRGB(100, 100, 100)
    elseif star_state[i] == "Main Sequence Star" then
        setRGB(100, 100, 50)
    elseif star_state[i] == "Red Giant" then
        setRGB(200, 100, 0)
    elseif star_state[i] == "Supernova" then
        setRGB(255, 255, 255)
    elseif star_state[i] == "Black Hole" then
        setRGB(0, 0, 0)
    elseif star_state[i] == "Neutron Star" then 
        setRGB(20, 20, 20)
    else
        setRGB(0, 200, 0)
    end
end

time = 0
function update()
    time = time + 1
    if time % 100 == 0 then
        for i=1, N_LEDS do
            advance_star(i)
        end
    end
    for i=1, N_LEDS do
        set_rgb(i)
    end
end
