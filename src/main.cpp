#include <Arduino.h>
#include <SPIFFS.h>
#include <SD_MMC.h>
#include <esp_timer.h>

#include "screen.h"
#include "camera.h"
#include "sdcard.h"
#include "test.h"

void print_available_memory() {
  printf("\nMemory Heap: (%d/%d) and PSRAM: (%d/%d) and SPIFFS: (%d/%d)\n\n", ESP.getFreeHeap(), ESP.getHeapSize(), ESP.getFreePsram(), ESP.getPsramSize(), SPIFFS.usedBytes(), SPIFFS.totalBytes());
}

void setup() {
  delay(3000);
  Serial.begin(115200);
  SPIFFS.begin();
  print_available_memory();

  delay(3000);
  esp_err_t screen_init_ok = screen_init();
  if (screen_init_ok != ESP_OK)
    printf("Screen Failed\n");
  esp_err_t camera_init_ok = camera_init();
  if (camera_init_ok != ESP_OK)
    printf("Camera Failed\n");
  if (!sd_card_init())
    printf("SD Card Failed\n");
  print_available_memory();

  // test_init();
}

void resize_image_to_display(uint16_t *inputImage, int input_width, int input_height, uint16_t *outputImage, int output_width, int output_height) {
  if (!inputImage || !outputImage) return;

  int scale_factor_x = input_width / output_width;
  int scale_factor_y = input_height / output_height;

  for (int output_row = 0; output_row < output_height; output_row++) {
    for (int output_column = 0; output_column < output_width; output_column++) {
      int original_column = output_column * scale_factor_x;
      int original_row = output_row * scale_factor_y;

      int avg_r = 0, avg_g = 0, avg_b = 0;

      for (int row_index = 0; row_index < scale_factor_y; row_index++) {
        for (int column_index = 0; column_index < scale_factor_x; column_index++) {
          int column = original_column + column_index;
          int row = original_row + row_index;

          if (column >= input_width || row >= input_height) continue;

          uint16_t pixel = inputImage[row * input_width + column];
          int r = (pixel >> 11) & 0x1F;
          int g = (pixel >> 5) & 0x3F;
          int b = pixel & 0x1F;

          avg_r += r;
          avg_g += g;
          avg_b += b;
        }
      }

      avg_r /= scale_factor_x * scale_factor_y;
      avg_g /= scale_factor_x * scale_factor_y;
      avg_b /= scale_factor_x * scale_factor_y;

      outputImage[output_row * output_width + output_column] = (uint16_t)((avg_r << 11) & 0xF800) | ((avg_g << 5) & 0x07E0) | (avg_b & 0x001F);
    }
  }
}

void crop_image_to_display(uint8_t *inputImage, int input_width, int input_height, uint8_t *outputImage, int output_width, int output_height) {
  int initial_column = (input_width - output_width) / 2;
  int initial_row = (input_height - output_height) / 2;

  int output_column = 0, output_row = 0;
  for (int input_row = initial_row; input_row < (initial_row + output_height); input_row++) {
    for (int input_column = initial_column; input_column < (initial_column + output_width); input_column++) {
      int input_position = (input_row * input_width + input_column) * 2;
      int output_position = (output_row * output_width + output_column) * 2;
      outputImage[output_position] = inputImage[input_position];
      outputImage[output_position + 1] = inputImage[input_position + 1];
      output_column++;
    }
    output_row++;
  }
}

void loop() {
  uint8_t *capture_image = (uint8_t *)malloc(640 * 480 * 2);
  uint8_t *display_image = (uint8_t *)malloc(240 * 240 * 2);
  char image_file_name[30];

  for (int image_index = 0; image_index < 10; image_index++)
  {
    snprintf(image_file_name, 30, "/image%02d.565", image_index);
    esp_err_t camera_capture_ok = camera_capture(capture_image);
    // delay(3000);
    if (camera_capture_ok != ESP_OK)
    {
      printf("Camera Capture Failed\n");
    }
    else
    {
      // writeFile(SD_MMC, image_file_name, capture_image, (640 * 480 * 2));
      printf("Screen Drawing %02d\n", image_index);
      crop_image_to_display(capture_image, 640, 480, display_image, 240, 240);
      screen_draw_bitmap(0, 0, 240, 240, (uint16_t*)display_image);
      delay(3000);
    }
  }

  free(capture_image);
  free(display_image);

  // test();

  delay(60000);
}