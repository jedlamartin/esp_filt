#include "ssd1306.h"

const uint8_t font5x8[][5] = {
    // 0 - 9
    {0x3E, 0x51, 0x49, 0x45, 0x3E},    // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00},    // 1
    {0x62, 0x51, 0x49, 0x49, 0x46},    // 2
    {0x22, 0x49, 0x49, 0x49, 0x36},    // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10},    // 4
    {0x2F, 0x49, 0x49, 0x49, 0x31},    // 5
    {0x3E, 0x49, 0x49, 0x49, 0x30},    // 6
    {0x01, 0x71, 0x09, 0x05, 0x03},    // 7
    {0x36, 0x49, 0x49, 0x49, 0x36},    // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E},    // 9
    // '-'
    {0x08, 0x08, 0x08, 0x08, 0x08}    // -
};

#define FONT_WIDTH    5
#define FONT_HEIGHT   8
#define CHAR_SPACING  1
#define BUFFER_WIDTH  128
#define BUFFER_HEIGHT 32
#define BUFFER_SIZE   (BUFFER_WIDTH * BUFFER_HEIGHT / 8)

void render_number(int value, uint8_t* buffer) {
    char text[12];    // Enough for -2147483648
    sprintf(text, "%d", value);

    memset(buffer, 0x00, BUFFER_SIZE);    // Clear buffer

    int x = 0;
    int y = 0;    // Top page (8px blocks vertically)

    for(size_t i = 0; text[i] != '\0' && x + FONT_WIDTH < BUFFER_WIDTH; ++i) {
        uint8_t c = text[i];
        const uint8_t* glyph;

        if(c >= '0' && c <= '9') {
            glyph = font5x8[c - '0'];
        } else if(c == '-') {
            glyph = font5x8[10];    // index of '-'
        } else {
            continue;    // skip unsupported characters
        }

        for(int col = 0; col < FONT_WIDTH; ++col) {
            int buffer_index = x + (y * BUFFER_WIDTH);
            if(buffer_index < BUFFER_SIZE) {
                buffer[buffer_index] = glyph[col];
            }
            x++;
        }

        // spacing
        if(x < BUFFER_WIDTH) {
            buffer[x + (y * BUFFER_WIDTH)] = 0x00;
            x++;
        }
    }
}

void draw_scaled_glyph(uint8_t* buffer, int x, int y, const uint8_t glyph[5]) {
    // Each original glyph column becomes 2 columns of 2x scaled pixels
    for(int col = 0; col < 5; col++) {
        uint8_t orig = glyph[col];

        for(int sx = 0; sx < 2; sx++) {    // 2x width
            for(int row = 0; row < 8; row++) {
                if(orig & (1 << row)) {
                    for(int sy = 0; sy < 2; sy++) {    // 2x height
                        int px = x + col * 2 + sx;
                        int py = y + row * 2 + sy;

                        if(px >= 0 && px < BUFFER_WIDTH && py >= 0 &&
                           py < BUFFER_HEIGHT) {
                            int byte_index = px + (py / 8) * BUFFER_WIDTH;
                            buffer[byte_index] |= (1 << (py % 8));
                        }
                    }
                }
            }
        }
    }
}

void render_number_scaled(int value, uint8_t* buffer) {
    char text[12];
    sprintf(text, "%d", value);

    memset(buffer, 0x00, BUFFER_SIZE);    // clear screen

    int char_width = 5 * 2 + 1;    // 10 pixels + 1 spacing
    int text_length = strlen(text);
    int total_width = text_length * char_width - 1;

    int start_x = (BUFFER_WIDTH - total_width) / 2;
    int start_y =
        (BUFFER_HEIGHT - 16) / 2;    // vertical center (font height is 16)

    int x = start_x;

    for(int i = 0; i < text_length; i++) {
        char c = text[i];
        const uint8_t* glyph;

        if(c >= '0' && c <= '9') {
            glyph = font5x8[c - '0'];
        } else if(c == '-') {
            glyph = font5x8[10];
        } else {
            continue;    // skip unsupported
        }

        draw_scaled_glyph(buffer, x, start_y, glyph);
        x += char_width;
    }
}