#include "dac.h"

dac_continuous_handle_t dac;

void conf_start_dac(uint32_t sine_freq, uint32_t fs) {
    if(sine_freq > fs / 2) {
        ESP_LOGE(TAG,
                 "Sine frequency must be less than half the sampling rate "
                 "(Nyquist limit)");
    }

    uint32_t length = fs / sine_freq <= 1024 ? fs / sine_freq : 1024;
    if(length * sine_freq != fs) {
        fs = length * sine_freq;
        ESP_LOGI(TAG,
                 "Sampling rate changed to %lu Hz to have whole number of "
                 "samples for %lu Hz freqency.",
                 fs,
                 sine_freq);
    }

    uint8_t* sine = malloc(length);

    for(int i = 0; i < length; i++) {
        float phase = 2 * M_PI / length * i;
        sine[i] = (uint8_t) (70.0f + 255.0f * SINE_AMP / 3.3f * sin(phase));
    }

    // Configure DAC
    const dac_continuous_config_t config = {
        .chan_mask = DAC_CHANNEL_MASK_CH0,
        .desc_num = 6,
        .buf_size = length,
        .freq_hz = fs,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT};

    // Start DAC
    ESP_ERROR_CHECK(dac_continuous_new_channels(&config, &dac));
    ESP_ERROR_CHECK(dac_continuous_enable(dac));
    size_t bytes_loaded;
    ESP_ERROR_CHECK(
        dac_continuous_write_cyclically(dac, sine, length, &bytes_loaded));
    if(bytes_loaded != (size_t) length) {
        ESP_LOGE(TAG,
                 "Couldn't load the whole sine wave table, %u bytes loaded "
                 "from %lu bytes.",
                 bytes_loaded,
                 length);
    }
    free(sine);
}