-- This is a port of https://github.com/TheNumenorean/HellLightingProject/blob/master/Arduino/_1dCells/_1dCells.ino 

add_param("step_delay", 0, 1, 0.02, 0.5)

led_state = {}
for i=1, N_LEDS do
    led_state[i] = math.floor(math.random(0,1)+0.5)
end

function update()

    local tmp = {}

    for i=1, N_LEDS do

        local before_idx = i - 1
        local after_idx = i + 1

        if before_idx == 0 then before_idx = N_LEDS end
        if after_idx == N_LEDS then after_idx = 1 end

        local before = led_state[before_idx]
        local current = led_state[i]
        local after = led_state[after_idx]
        
        if current == 0 and (after == 0 or before == 0) and not (after == 0 and before == 0) then
            tmp[i] = 1
        else
            tmp[i] = 0
        end
    end

    led_state = tmp

    for i=1, N_LEDS do
        setRGB(i, 0, led_state[i], 0)
    end
end