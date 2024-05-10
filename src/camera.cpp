#include <Arduino.h>
#include <esp_camera.h>

static camera_config_t ov2640_camera_config = {
    .pin_pwdn  = -1,
    .pin_reset = -1,
    .pin_xclk = 15,
    .pin_sccb_sda = 4,
    .pin_sccb_scl = 5,

    .pin_d7 = 16,
    .pin_d6 = 17,
    .pin_d5 = 18,
    .pin_d4 = 12,
    .pin_d3 = 10,
    .pin_d2 = 8,
    .pin_d1 = 9,
    .pin_d0 = 11,
    .pin_vsync = 6,
    .pin_href = 7,
    .pin_pclk = 13,

    .xclk_freq_hz = 20000000,//EXPERIMENTAL: Set to 16MHz on ESP32-S2 or ESP32-S3 to enable EDMA mode
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_RGB565,//YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_VGA,//QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

    .jpeg_quality = 31, //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 1, //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY, //CAMERA_GRAB_WHEN_EMPTY or CAMERA_GRAB_LATEST. Sets when buffers should be filled
};

static camera_config_t ov7725_camera_config = {
    .pin_pwdn  = -1,
    .pin_reset = -1,
    .pin_xclk = 0,
    .pin_sccb_sda = 26,
    .pin_sccb_scl = 27,

    .pin_d7 = 35,
    .pin_d6 = 34,
    .pin_d5 = 39,
    .pin_d4 = 36,
    .pin_d3 = 21,
    .pin_d2 = 19,
    .pin_d1 = 18,
    .pin_d0 = 5,
    .pin_vsync = 25,
    .pin_href = 23,
    .pin_pclk = 22,

    .xclk_freq_hz = 20000000,//EXPERIMENTAL: Set to 16MHz on ESP32-S2 or ESP32-S3 to enable EDMA mode
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_RGB565,//YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_VGA,//QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

    .jpeg_quality = 31, //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 1, //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY, //CAMERA_GRAB_WHEN_EMPTY or CAMERA_GRAB_LATEST. Sets when buffers should be filled
};

esp_err_t camera_init() {
    printf("Camera initializing...");
    esp_err_t err = esp_camera_init(&ov2640_camera_config);
    
    if (err != ESP_OK) {
        printf("Failed\n");
        return err;
    }
    printf("OK\n");
    return ESP_OK;
}

esp_err_t camera_capture(uint8_t* output) {
    printf("Camera Capturing...");
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        printf("Failed\n");
        return ESP_FAIL;
    }
    printf("Width: %d, Height: %d, Pixel Format: %d, FB Lenght: %d, ...", fb->width, fb->height, fb->format, fb->len);

    memcpy(output, fb->buf, fb->len);

    esp_camera_fb_return(fb);
    printf("OK\n");
    return ESP_OK;
}