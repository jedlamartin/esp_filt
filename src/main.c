#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <string.h>

#include "adc.h"
#include "dac.h"
#include "filter.h"
#include "gpio.h"
#include "i2c.h"
#include "ssd1306.h"
#include "timer.h"

uint8_t disp_buffer[128 * 4];
float filtered = 0.0f;
SemaphoreHandle_t mutex = NULL;
SemaphoreHandle_t convolution_semaphore = NULL;
TaskHandle_t filter_task = NULL;

extern adc_continuous_handle_t adc;

void vTaskSendComPort(void* pvParameters);
void vTaskFilter(void* pvParameters);

static char* TAG = "Main";

void app_main() {
    mutex = xSemaphoreCreateMutex();
    convolution_semaphore = xSemaphoreCreateCounting(2, 0);

    if(mutex == NULL) {
        ESP_LOGE("Main", "Failed to create mutex!");
    }

    conf_adc(ADC_CHN);

    conf_i2c();

    conf_dac();
    config_timer();
    start_timer();

    // Generate filter
    init_filter();

    // Create tasks
    xTaskCreate(vTaskFilter,
                "Filter",
                4096 * 4,
                NULL,
                tskIDLE_PRIORITY + 1,
                &filter_task);

    start_adc();

    xTaskCreate(vTaskSendComPort,
                "SendComPort",
                4096,
                NULL,
                tskIDLE_PRIORITY + 1,
                NULL);
}

void vTaskSendComPort(void* pvParameters) {
    float filtered_temp = 0.0f;
    for(;;) {
        // Get the current filtered value
        xSemaphoreTake(mutex, portMAX_DELAY);
        filtered_temp = filtered;
        xSemaphoreGive(mutex);

        // Convert to the display's format
        render_number_scaled(filtered_temp * 1000, disp_buffer);

        // Send to display
        i2c_sendData(disp_buffer, 128 * 4);

        // Log
        printf(">");
        printf("sqrt:%f\r\n", filtered_temp);

        // Run it every 100 ms
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vTaskFilter(void* pvParameters) {
    uint8_t raw_samples[ADC_BUFFER_SIZE];
    for(;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        uint32_t bytes_copied = 0;
        ESP_ERROR_CHECK(adc_continuous_read(
            adc, raw_samples, ADC_BUFFER_SIZE, &bytes_copied, 0));

        if(bytes_copied != ADC_BUFFER_SIZE) {
            ESP_LOGE(TAG,
                     "Recieved %lu bytes instead of %d bytes from the ADC!",
                     bytes_copied,
                     ADC_BUFFER_SIZE);
        }
        uint16_t sample = ((adc_digi_output_data_t*) raw_samples)->type1.data;
        float filtered_tmp = filter((float) sample);

        if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
            filtered = filtered_tmp;
            xSemaphoreGive(mutex);
        }
        taskYIELD();
    }
}