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
extern QueueHandle_t queue;
volatile float filtered = 0;
SemaphoreHandle_t mutex = NULL;

extern adc_oneshot_unit_handle_t adc;

void vTaskSendComPort(void* pvParameters);
void vTaskFilter(void* pvParameters);

void app_main() {
    mutex = xSemaphoreCreateMutex();

    if(mutex == NULL) {
        ESP_LOGE("Main", "Failed to create mutex!");
    }

    conf_start_dac(SINE_FREQ, 1e6);

    conf_adc(ADC_CHN);

    config_timer();

    conf_i2c();

    // conf_gpio(GPIO_NUM_13);

    // Generate filter
    generateTable();

    // Start timer
    start_timer();

    // Create tasks
    xTaskCreate(vTaskSendComPort,
                "SendComPort",
                4096,
                NULL,
                tskIDLE_PRIORITY + 1,
                NULL);
    xTaskCreate(
        vTaskFilter, "Filter", 4096 * 4, NULL, tskIDLE_PRIORITY + 1, NULL);
}

void vTaskSendComPort(void* pvParameters) {
    float filtered_temp = 0.0f;
    for(;;) {
        // Get the current filtered value
        // printf("Taking mutex!\n");
        xSemaphoreTake(mutex, portMAX_DELAY);
        filtered_temp = filtered;
        xSemaphoreGive(mutex);
        // printf("Released mutex!\n");

        // Convert to the display's format
        render_number_scaled(filtered_temp, disp_buffer);

        // Send to display
        i2c_sendData(disp_buffer, 128 * 4);

        // Log
        printf(">");
        printf("sqrt:%f\r\n", filtered_temp);

        // Run it every 100 ms
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void vTaskFilter(void* pvParameters) {
    int raw_value = 0;
    for(;;) {
        // printf("Taking semaphore for filtering!\n");
        if(xQueueReceive(queue, &raw_value, portMAX_DELAY) != pdPASS) {
            ESP_LOGE("Main", "Could not recieve the item from the queue!");
        }
        // raw_value_tmp = raw_value;

        // printf("Started filtering!\n");
        float filtered_tmp = filter(raw_value);
        // float filtered_tmp = 0.0f;
        // printf("Finished filtering!\n");

        // printf("Taking mutex to overwrite!\n");
        if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
            filtered = filtered_tmp;
            xSemaphoreGive(mutex);
        }
        // printf("Released mutex to overwrite!\n");
        taskYIELD();
    }
}