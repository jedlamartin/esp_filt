#include "adc.h"

#include "dac.h"
#include "filter.h"

adc_oneshot_unit_handle_t adc;

char* tag = "ADC";

void conf_adc(adc_channel_t ADC_CHANNEL) {
    // Configure ADC
    const adc_oneshot_unit_init_cfg_t adc_config = {
        .clk_src = ADC_DIGI_CLK_SRC_DEFAULT,
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE};
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_config, &adc));

    const adc_oneshot_chan_cfg_t cfg = {.atten = ADC_ATTEN_DB_0,
                                        .bitwidth = ADC_BITWIDTH_12};

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, ADC_CHANNEL, &cfg));
}
