#include <driver/i2c_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <string.h>

#define DISP_ADDR 0x3c

extern i2c_master_bus_handle_t i2c_master;
extern i2c_master_dev_handle_t i2c_dev;

void conf_i2c();
void i2c_sendData(uint8_t* buffer, int length);