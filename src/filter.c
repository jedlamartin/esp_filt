#include "filter.h"

#include "esp_random.h"

volatile int buffer[BUF_LENGTH] = {0};
volatile int buffer_pointer = 0;
volatile float sine_table[BUF_LENGTH] = {0.0f};

volatile float buffer_sqrt[RMS_LENGTH] = {0.0f};
volatile int rms_pointer = 0;

volatile float out_sqrt = 0.0f;

void generateTable() {
    for(int n = 0; n < BUF_LENGTH; n++) {
        float phase = 2.0f * M_PI * SINE_FREQ * n / FS;
        sine_table[BUF_LENGTH - n - 1] = sinf(phase) / FILTER_PERIODS;
    }
    /*for(int n = 0; n < BUF_LENGTH; n++) {
        printf(">");
        printf("sine:%f\n", sine_table[n]);
    }*/
}

float filter(int newValue) {
    // float rndAmp = 500.0f;
    //  Generate random float between -1.0 and 1.0
    // float random_unit =
    //     ((float) esp_random() / (float) UINT32_MAX) * 2.0f - 1.0f;

    // Scale by rndAmp and convert to integer, then add to newValue
    // newValue = newValue + (int) (rndAmp * random_unit);

    buffer[buffer_pointer] = newValue;
    float out = 0.0f;
    int idx = buffer_pointer;

    for(int i = 0; i < BUF_LENGTH; i++) {
        out += sine_table[i] * buffer[idx];
        idx = (idx - 1 + BUF_LENGTH) % BUF_LENGTH;
    }

    // Comment this
    // out = newValue;

    out_sqrt -= buffer_sqrt[rms_pointer] * buffer_sqrt[rms_pointer];
    out_sqrt += out * out;

    buffer_sqrt[rms_pointer] = out;

    buffer_pointer = (buffer_pointer + 1) % BUF_LENGTH;
    rms_pointer = (rms_pointer + 1) % RMS_LENGTH;
    return sqrtf(out_sqrt / RMS_LENGTH);
    // return out;
    // return newValue;
}