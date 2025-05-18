#pragma once
#include <driver/dac_continuous.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <math.h>

#define TAG "DAC"
#define _USE_MATH_DEFINES
#define SINE_FREQ 1000
#define SINE_AMP  0.35

extern dac_continuous_handle_t dac;

void conf_start_dac(uint32_t sine_freq, uint32_t fs);