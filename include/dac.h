#pragma once
#include <driver/dac_oneshot.h>
#include <esp_adc/adc_continuous.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <math.h>

#define _USE_MATH_DEFINES
#define SINE_FREQ 1000
#define SINE_AMP  0.35
#define DAC_FS    10000

extern dac_oneshot_handle_t dac;
extern uint8_t* DAC_BUFFER;
extern uint32_t DAC_BUFFER_SIZE;
void conf_dac();