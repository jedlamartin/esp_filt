#pragma once

#include <soc/soc_caps.h>

#include "adc.h"
#include "dac.h"

#define FILTER_PERIODS 10
#define RMS_PERIODS    500
#define BUF_LENGTH     (int) (((FS) / (SINE_FREQ)) * (FILTER_PERIODS))
#define RMS_LENGTH     (int) (((FS) / (SINE_FREQ)) * (RMS_PERIODS))

extern float buffer[BUF_LENGTH];
extern int buffer_pointer;
extern float sine_table[BUF_LENGTH];

typedef struct {
    const int start;
    const int end;
    float result_sine;
    float result_cosine;
} ConvChunk;

extern TaskHandle_t conv_task_handle2;
extern TaskHandle_t conv_task_handle1;
extern ConvChunk chunk1;
extern ConvChunk chunk2;

void generateTable();
float filter(int newValue);
void vTaskConvolve(void* param);