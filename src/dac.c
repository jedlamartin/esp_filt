#include "dac.h"

dac_oneshot_handle_t dac = NULL;
uint8_t* DAC_BUFFER = NULL;
uint32_t DAC_BUFFER_SIZE = 0;

static char* TAG = "DAC";

void conf_dac() {
    if(SINE_FREQ > DAC_FS / 2) {
        ESP_LOGE(TAG,
                 "Sine frequency must be less than half the sampling rate "
                 "(Nyquist limit)");
    }

    DAC_BUFFER_SIZE = DAC_FS / SINE_FREQ;

    DAC_BUFFER = malloc(DAC_BUFFER_SIZE * sizeof(uint8_t));

    for(int i = 0; i < DAC_BUFFER_SIZE; i++) {
        float phase = 2 * M_PI / DAC_BUFFER_SIZE * i;
        DAC_BUFFER[i] =
            (uint8_t) (70.0f + 255.0f * SINE_AMP / 3.3f * sin(phase));
        printf(">DAC:%u\n", DAC_BUFFER[i]);
    }

    // Configure DAC
    const dac_oneshot_config_t config = {
        .chan_id = DAC_CHAN_0,
    };

    ESP_ERROR_CHECK(dac_oneshot_new_channel(&config, &dac));
}