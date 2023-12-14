set(COMPONENT_SRCS "nvs.c" "oled.c" "oled.c nvs.c" "main.c" "soil_moisture.c" "dht22.c" "main.c wifi.c http_client.c mqtt.c oled.c dht22.c soil_moisture.c gpio_setup.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()