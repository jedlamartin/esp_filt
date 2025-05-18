#pragma once
#include <esp_adc/adc_oneshot.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>

#define ADC_CHN ADC_CHANNEL_3
#define FS      (SINE_FREQ * 3)

extern adc_oneshot_unit_handle_t adc;

void conf_adc(adc_channel_t ADC_CHANNEL);