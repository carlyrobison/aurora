-- author("Alex Meiburg")
-- This is a port of the original game of life pattern. 

add_param("step_delay", 0, 1, 0.02, 0.5)

led_state = {}
for i=1, N_LEDS do
    led_state[i] = math.floor(math.random(0,1)+0.5)
end

function update()
    for i=1, N_LEDS do
        setRGB(i, led_state[i], led_state[i], led_state[i])
    end
end