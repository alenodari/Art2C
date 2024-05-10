#include <esp_err.h>

esp_err_t screen_init();
void screen_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);