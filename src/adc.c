#include "adc.h"

#include "dac.h"
#include "filter.h"

adc_continuous_handle_t adc;
extern TaskHandle_t filter_task;

static bool adc_callback(adc_continuous_handle_t handle,
                         const adc_continuous_evt_data_t* edata,
                         void* user_data) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(filter_task, &xHigherPriorityTaskWoken);

    return (xHigherPriorityTaskWoken == pdTRUE);
}

void conf_adc(adc_channel_t ADC_CHANNEL) {
    adc_continuous_handle_cfg_t handle_cfg = {
        .max_store_buf_size = ADC_BUFFER_SIZE * 2,
        .conv_frame_size = ADC_BUFFER_SIZE,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&handle_cfg, &adc));

    adc_digi_pattern_config_t pattern = {.atten = ADC_ATTEN_DB_12,
                                         .channel = ADC_CHANNEL,
                                         .unit = ADC_UNIT_1,
                                         .bit_width = ADC_BITWIDTH_12};

    adc_continuous_config_t cont_cfg = {
        .pattern_num = 1,
        .adc_pattern = &pattern,
        .sample_freq_hz = FS,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
    };

    ESP_ERROR_CHECK(adc_continuous_config(adc, &cont_cfg));

    // 3. Register event callback
    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = adc_callback,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(adc, &cbs, NULL));
}

void start_adc() { ESP_ERROR_CHECK(adc_continuous_start(adc)); }