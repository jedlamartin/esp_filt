#pragma once
#include <driver/gptimer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

extern gptimer_handle_t timer;
extern QueueHandle_t queue;

void config_timer();
void start_timer();