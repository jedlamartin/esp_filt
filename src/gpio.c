#include "gpio.h"

void conf_gpio(gpio_num_t PWR_PIN) {
    // Configure GPIO
    const gpio_config_t config = {.pin_bit_mask = (1ULL << PWR_PIN),
                                  .mode = GPIO_MODE_OUTPUT,
                                  .intr_type = GPIO_INTR_DISABLE};
    ESP_ERROR_CHECK(gpio_config(&config));
    ESP_ERROR_CHECK(gpio_set_level(PWR_PIN, 0));
}

void toggle_gpio(gpio_num_t PWR_PIN) {
    ESP_ERROR_CHECK(gpio_set_level(PWR_PIN, !gpio_get_level(PWR_PIN)));
}