idf_component_register(
    SRCS "main.c" "st7789.c" "sht30.c"
    INCLUDE_DIRS "."
    REQUIRES
        ui
        esp_timer
        esp_driver_gpio
        esp_driver_i2c
        esp_driver_spi
)
