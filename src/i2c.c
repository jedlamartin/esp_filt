#include "i2c.h"

#define TIMEOUT portTICK_PERIOD_MS * 10

i2c_master_bus_handle_t i2c_master;
i2c_master_dev_handle_t i2c_dev;

void conf_i2c() {
    const i2c_master_bus_config_t master_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 0,    // Changed from -1 to 0 (I2C_NUM_0)
        .scl_io_num = GPIO_NUM_22,
        .sda_io_num = GPIO_NUM_21,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true    // Important for I2C
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&master_config, &i2c_master));

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = DISP_ADDR,
        .scl_speed_hz = 400000    // Increased to 400kHz (SSD1306 supports this)
    };
    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(i2c_master, &dev_config, &i2c_dev));

    // Power up delay
    vTaskDelay(pdMS_TO_TICKS(100));

    // SSD1306 initialization sequence
    uint8_t init_cmds[] = {
        0xAE,          // Display OFF
        0xD5, 0x80,    // Set display clock divide
        0xA8, 0x1F,    // Set multiplex ratio (31 for 128x32)
        0xD3, 0x00,    // Set display offset
        0x40,          // Set start line
        0x8D, 0x14,    // Charge pump enable
        0x20, 0x00,    // Memory mode: horizontal
        0xA1,          // Segment remap
        0xC8,          // COM output scan direction
        0xDA, 0x02,    // COM pins config
        0x81, 0xCF,    // Contrast
        0xD9, 0xF1,    // Pre-charge period
        0xDB, 0x40,    // VCOMH deselect level
        0xA4,          // Display resume
        0xA6,          // Normal display
        0xAF           // Display ON
    };

    // Send all commands
    for(int i = 0; i < sizeof(init_cmds); i++) {
        uint8_t cmd[2] = {0x00, init_cmds[i]};    // Control byte + command
        ESP_ERROR_CHECK(
            i2c_master_transmit(i2c_dev, cmd, sizeof(cmd), TIMEOUT));
    }
    printf("SSD1306 initialized\n");

    // Switch to data mode (D/C# = 1)
    /*     uint8_t data_cmd = 0x40;
        ESP_ERROR_CHECK(i2c_master_transmit(i2c_dev, &data_cmd, 1, TIMEOUT));

        uint8_t buffer[128 * 4];
        for(int i = 0; i < 128 * 4; i++) {
            buffer[i] = 0xFF;    // All pixels ON in each column
        }
        ESP_ERROR_CHECK(
            i2c_master_transmit(i2c_dev, buffer, sizeof(buffer), TIMEOUT)); */
}

void i2c_sendData(uint8_t* buffer, int length) {
    uint8_t temp[length + 1];
    temp[0] = 0x40;    // Control byte for data
    memcpy(&temp[1], buffer, length);
    ESP_ERROR_CHECK(i2c_master_transmit(i2c_dev, temp, length + 1, TIMEOUT));
}