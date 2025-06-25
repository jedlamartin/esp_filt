#pragma once
#include <esp_adc/adc_continuous.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <soc/soc_caps.h>

#define ADC_CHN         ADC_CHANNEL_3
#define FS              (SINE_FREQ * 20)
#define ADC_BUFFER_SIZE SOC_ADC_DIGI_DATA_BYTES_PER_CONV

extern adc_continuous_handle_t adc;

void conf_adc(adc_channel_t ADC_CHANNEL);

void start_adc();