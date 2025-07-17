#include "timer.h"

#include "adc.h"
#include "dac.h"
#include "filter.h"

gptimer_handle_t timer = NULL;
uint32_t DAC_BUFFER_POINTER = 0;

static bool timer_callback(gptimer_handle_t timer,
                           const gptimer_alarm_event_data_t* edata,
                           void* user_ctx) {
    ESP_ERROR_CHECK(
        dac_oneshot_output_voltage(dac, DAC_BUFFER[DAC_BUFFER_POINTER]));
    DAC_BUFFER_POINTER = (DAC_BUFFER_POINTER < (DAC_BUFFER_SIZE - 1)) ?
                             DAC_BUFFER_POINTER + 1 :
                             0;

    return false;
}

void config_timer() {
    const gptimer_config_t init = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_DOWN,
        .resolution_hz = 1e6,    // 1MHz;
    };
    ESP_LOGI("Timer", "RES:%lu", init.resolution_hz);
    ESP_ERROR_CHECK(gptimer_new_timer(&init, &timer));

    const gptimer_alarm_config_t config = {
        .alarm_count = 0,
        .reload_count = init.resolution_hz / DAC_FS - 1,
        .flags.auto_reload_on_alarm = true};
    gptimer_event_callbacks_t cbs = {.on_alarm = timer_callback};

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer, &cbs, NULL));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer, &config));
    ESP_ERROR_CHECK(gptimer_enable(timer));
}

void start_timer() { ESP_ERROR_CHECK(gptimer_start(timer)); }
