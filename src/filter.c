#include "filter.h"

Biquad* biquad;
float rms_buf[RMS_LENGTH] = {0.0f};
float rms_value = 0.0f;
int rms_pointer = 0;

float filter_biquad(Biquad* biquad, const float sample) {
    float output = (biquad->b[0] * sample) +
                   (biquad->b[1] * biquad->aBuffer[0]) +
                   (biquad->b[2] * biquad->aBuffer[1]) -
                   (biquad->a[1] * biquad->bBuffer[0]) -
                   (biquad->a[2] * biquad->bBuffer[1]);

    // Shift input delay line (x[n-1] -> x[n-2], new sample -> x[n-1])
    biquad->aBuffer[1] = biquad->aBuffer[0];
    biquad->aBuffer[0] = sample;

    // Shift output delay line (y[n-1] -> y[n-2], new output -> y[n-1])
    biquad->bBuffer[1] = biquad->bBuffer[0];
    biquad->bBuffer[0] = output;

    return output;
}

void init_biquad(Biquad* biquad, const float* b, const float* a) {
    biquad->a[0] = a[0];
    biquad->a[1] = a[1];
    biquad->a[2] = a[2];

    biquad->b[0] = b[0];
    biquad->b[1] = b[1];
    biquad->b[2] = b[2];

    biquad->aBuffer[0] = biquad->aBuffer[1] = 0.0f;
    biquad->bBuffer[0] = biquad->bBuffer[1] = 0.0f;
}
void init_filter() {
    biquad = (Biquad*) malloc(sizeof(Biquad) * BIQ_NUM);

    float b1[] = {0.284276248f, 0.568552496f, 0.284276248f};
    float b2[] = {0.284276248f, -0.568552496f, 0.284276248f};
    float b3[] = {0.254395016f, 0.5087900319f, 0.254395016f};
    float b4[] = {0.254395016f, -0.5087900319f, 0.254395016f};
    float b5[] = {0.2452372753f, 0.f, -0.2452372753f};

    float a1[] = {1.f, -1.276958739f, 0.7340433918f};
    float a2[] = {1.f, -1.927452335f, 0.9434989037f};
    float a3[] = {1.f, -1.801156902f, 0.8214940675f};
    float a4[] = {1.f, -1.143865177f, 0.4326832977f};
    float a5[] = {1.f, -1.435644015f, 0.5095254495f};

    init_biquad(&biquad[0], b1, a1);
    init_biquad(&biquad[1], b2, a2);
    init_biquad(&biquad[2], b3, a3);
    init_biquad(&biquad[3], b4, a4);
    init_biquad(&biquad[4], b5, a5);
}

float filter(int newValue) {
    float out = (float) newValue / 4095.0f;

    for(int i = 0; i < BIQ_NUM; i++) {
        out = filter_biquad(&biquad[i], out);
    }

    float rms = out * out;
    rms_value = rms_value + (rms - rms_buf[rms_pointer]) / RMS_LENGTH;
    rms_buf[rms_pointer] = rms;
    rms_pointer = (rms_pointer + 1) % RMS_LENGTH;
    return sqrtf(rms_value);
}