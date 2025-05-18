#include "timer.h"

#include "adc.h"
#include "dac.h"
#include "filter.h"


volatile float raw_value;
gptimer_handle_t timer = NULL;
SemaphoreHandle_t queue = NULL;

static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

#define RESOLUTION 1000

bool timer_callback(gptimer_handle_t timer,
                    const gptimer_alarm_event_data_t* edata,
                    void* user_ctx) {
    int raw_value_tmp;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    portENTER_CRITICAL_ISR(&spinlock);
    esp_err_t err = adc_oneshot_read(adc, ADC_CHN, &raw_value_tmp);
    portEXIT_CRITICAL_ISR(&spinlock);
    if(err != ESP_OK) {
        return true;
    }

    if(xQueueSendFromISR(queue, &raw_value_tmp, &xHigherPriorityTaskWoken) !=
       pdPASS) {
        return true;
    }
    if(xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }

    return true;
}

void config_timer() {
    queue = xQueueCreate(8, sizeof(int));
    if(queue == NULL) {
        ESP_LOGE("Timer", "Failed to create the queue!");
        return;
    }
    const gptimer_config_t init = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_DOWN,
        .resolution_hz = 10e6};    // FS * RESOLUTION};
    ESP_LOGI("Timer", "RES:%lu", init.resolution_hz);
    ESP_ERROR_CHECK(gptimer_new_timer(&init, &timer));

    const gptimer_alarm_config_t config = {
        .alarm_count = 0,
        .reload_count = init.resolution_hz / FS - 1,
        .flags.auto_reload_on_alarm = true};
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer, &config));

    gptimer_event_callbacks_t cbs = {.on_alarm = timer_callback};

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer, &cbs, NULL));

    ESP_ERROR_CHECK(gptimer_enable(timer));
}

void start_timer() { ESP_ERROR_CHECK(gptimer_start(timer)); }
