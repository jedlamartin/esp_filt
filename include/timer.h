#pragma once
#include <driver/gpio.h>
#include <driver/gptimer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "dac.h"

extern gptimer_handle_t timer;
extern uint32_t DAC_BUFFER_POINTER;

void config_timer();
void start_timer();