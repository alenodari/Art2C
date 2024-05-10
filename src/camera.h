#include <esp_err.h>

esp_err_t camera_init();
esp_err_t camera_capture(uint8_t* output);