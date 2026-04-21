#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "esp_log.h"

#define SAMPLE_RATE 16000

#define I2S_BCLK 26
#define I2S_WS 25
#define I2S_DOUT 27

static i2s_chan_handle_t tx_handle;

void audio_init()
{
    i2s_chan_config_t chan_cfg =
        I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);

    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),

        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(
            I2S_DATA_BIT_WIDTH_16BIT,
            I2S_SLOT_MODE_MONO),

        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_BCLK,
            .ws = I2S_WS,
            .dout = I2S_DOUT,
            .din = I2S_GPIO_UNUSED,
        },
    };

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
}

void audio_play(const unsigned char wav[], unsigned int wav_len)
{
    const uint8_t *audio = wav + 44;
    size_t audio_len = wav_len - 44;

    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));

    size_t written;

    ESP_ERROR_CHECK(
        i2s_channel_write(
            tx_handle,
            audio,
            audio_len,
            &written,
            portMAX_DELAY));

    ESP_ERROR_CHECK(i2s_channel_disable(tx_handle));
}
