#include "timer.h"

#include "adc.h"
#include "dac.h"
#include "filter.h"

gptimer_handle_t timer = NULL;
extern TaskHandle_t task_DAC;
uint32_t DAC_BUFFER_POINTER = 0;

bool timer_callback(gptimer_handle_t timer,
                    const gptimer_alarm_event_data_t* edata,
                    void* user_ctx) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(task_DAC, &xHigherPriorityTaskWoken);
    if(xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
    return true;
}

void config_timer() {
    const gptimer_config_t init = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_DOWN,
        .resolution_hz = 40e6,    // FS * RESOLUTION};
    };
    ESP_LOGI("Timer", "RES:%lu", init.resolution_hz);
    ESP_ERROR_CHECK(gptimer_new_timer(&init, &timer));

    const gptimer_alarm_config_t config = {
        .alarm_count = 0,
        .reload_count = init.resolution_hz / DAC_FS - 1,
        .flags.auto_reload_on_alarm = true};
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer, &config));

    gptimer_event_callbacks_t cbs = {.on_alarm = timer_callback};

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer, &cbs, NULL));

    ESP_ERROR_CHECK(gptimer_enable(timer));
}

void start_timer() { ESP_ERROR_CHECK(gptimer_start(timer)); }
