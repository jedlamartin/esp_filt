#include "filter.h"

// #define DEBUG

float buffer[BUF_LENGTH] = {0.0f};
int buffer_pointer = 0;
float sine_table[BUF_LENGTH] = {0.0f};
float cosine_table[BUF_LENGTH] = {0.0f};

float buffer_sqrt[RMS_LENGTH] = {0.0f};
int rms_pointer = 0;

float out_rms = 0.0f;

extern SemaphoreHandle_t convolution_semaphore;
TaskHandle_t conv_task_handle1 = NULL;
TaskHandle_t conv_task_handle2 = NULL;
ConvChunk chunk1 = {0, BUF_LENGTH / 2, 0.0f, 0.0f};
ConvChunk chunk2 = {BUF_LENGTH / 2, BUF_LENGTH, 0.0f, 0.0f};

void generateTable() {
    const float scale = 2.0f / BUF_LENGTH;

    for(int n = 0; n < BUF_LENGTH; n++) {
        const float phase = 2.0f * M_PI * SINE_FREQ * n / FS;
        sine_table[BUF_LENGTH - n - 1] = sinf(phase) * scale;
        cosine_table[BUF_LENGTH - n - 1] = cosf(phase) * scale;
    }
    /*for(int n = 0; n < BUF_LENGTH; n++) {
        printf(">");
        printf("sine:%f\n", sine_table[n]);
    }*/
}

void vTaskConvolve(void* param) {
    ConvChunk* chunk = (ConvChunk*) param;
    while(1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    // Wait for notification

        float out_sine = 0.0f;
        float out_cosine = 0.0f;

        for(int i = chunk->start; i < chunk->end; i++) {
            int idx = (buffer_pointer - i + BUF_LENGTH) % BUF_LENGTH;
            // out_sine += sine_table[i] * buffer[idx];
            out_cosine += cosine_table[i] * buffer[idx];
        }

        chunk->result_sine = out_sine;
        chunk->result_cosine = out_cosine;
        xSemaphoreGive(convolution_semaphore);
    }
}

float filter(int newValue) {
    buffer[buffer_pointer] = ((float) newValue / 4095.0f) * 3.3f;
    // buffer[buffer_pointer] = newValue;

#ifdef DEBUG
    printf("Ptr: %d, Chunk1: %d->%d, Chunk2: %d->%d\n",
           buffer_pointer,
           (buffer_pointer - chunk1.start + BUF_LENGTH) % BUF_LENGTH,
           (buffer_pointer - chunk1.end + BUF_LENGTH) % BUF_LENGTH,
           (buffer_pointer - chunk2.start + BUF_LENGTH) % BUF_LENGTH,
           (buffer_pointer - chunk2.end + BUF_LENGTH) % BUF_LENGTH);
#endif

    xTaskNotifyGive(conv_task_handle1);
    xTaskNotifyGive(conv_task_handle2);

    xSemaphoreTake(convolution_semaphore, portMAX_DELAY);
    xSemaphoreTake(convolution_semaphore, portMAX_DELAY);

    float sine_component = chunk1.result_sine + chunk2.result_sine;
    float cosine_component = chunk1.result_cosine + chunk2.result_cosine;

    float out =
        sine_component * sine_component + cosine_component * cosine_component;
    out_rms -= buffer_sqrt[rms_pointer];
    out_rms += out;
    buffer_sqrt[rms_pointer] = out;

    buffer_pointer = (buffer_pointer + 1) % BUF_LENGTH;
    rms_pointer = (rms_pointer + 1) % RMS_LENGTH;

    float rms_val = out_rms / RMS_LENGTH;
    return sqrtf(rms_val);
    // return out;
    // return newValue;
}