#pragma once

#include <soc/soc_caps.h>

#include "adc.h"
#include "dac.h"

#define FILTER_PERIODS 100
#define RMS_PERIODS    500
#define BUF_LENGTH     (((FS) / (SINE_FREQ)) * (FILTER_PERIODS))
#define RMS_LENGTH     (((FS) / (SINE_FREQ)) * (RMS_PERIODS))

extern volatile int buffer[BUF_LENGTH];
extern volatile int buffer_pointer;
extern volatile float sine_table[BUF_LENGTH];

void generateTable();
float filter(int newValue);