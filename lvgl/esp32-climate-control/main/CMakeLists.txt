idf_component_register(
    SRCS "main.c" "st7789.c"
    INCLUDE_DIRS "."
    REQUIRES
        ui
        esp_timer
        esp_driver_gpio
        esp_driver_spi
)
