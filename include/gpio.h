#pragma once
#include <driver/gpio.h>

void conf_gpio(gpio_num_t PWR_PIN);
void toggle_gpio(gpio_num_t PWR_PIN);