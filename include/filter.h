#pragma once

#include <soc/soc_caps.h>

#include "adc.h"
#include "dac.h"

#define BIQ_NUM     5
#define RMS_PERIODS 500
#define RMS_LENGTH  (int) (((FS) / (SINE_FREQ)) * (RMS_PERIODS))

typedef struct {
    float a[3];
    float b[3];
    float aBuffer[2];
    float bBuffer[2];
} Biquad;

void init_filter();
float filter(int newValue);
