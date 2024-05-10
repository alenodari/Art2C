#include <Arduino.h>
#include <esp_err.h>

#include "screen_driver.h"

#define BOARD_LCD_MOSI 47
#define BOARD_LCD_MISO -1
#define BOARD_LCD_SCK 21
#define BOARD_LCD_CS 44
#define BOARD_LCD_DC 43
#define BOARD_LCD_RST -1
#define BOARD_LCD_BL 48
#define BOARD_LCD_PIXEL_CLOCK_HZ (20 * 1000 * 1000)
#define BOARD_LCD_BK_LIGHT_ON_LEVEL 0
#define BOARD_LCD_BK_LIGHT_OFF_LEVEL !BOARD_LCD_BK_LIGHT_ON_LEVEL
#define BOARD_LCD_H_RES 240
#define BOARD_LCD_V_RES 240
#define BOARD_LCD_CMD_BITS 8
#define BOARD_LCD_PARAM_BITS 8
#define LCD_HOST SPI2_HOST

static scr_driver_t lcd; 

esp_err_t screen_init() {
    
    printf("Screen Initializing Bus...");    
    spi_config_t bus_conf = {
            .miso_io_num = (gpio_num_t)BOARD_LCD_MISO,
            .mosi_io_num = (gpio_num_t)BOARD_LCD_MOSI,
            .sclk_io_num = (gpio_num_t)BOARD_LCD_SCK,
            .max_transfer_sz = 2 * 240 * 240 + 10,
        };
    spi_bus_handle_t spi_bus = spi_bus_create(SPI2_HOST, &bus_conf);

    printf("Interface...");    
    scr_interface_spi_config_t spi_lcd_cfg = {
            .spi_bus = spi_bus,
            .pin_num_cs = BOARD_LCD_CS,
            .pin_num_dc = BOARD_LCD_DC,
            .clk_freq = 40 * 1000000,
            .swap_data = 0,
    };
    scr_interface_driver_t *iface_drv;
    scr_interface_create(SCREEN_IFACE_SPI, &spi_lcd_cfg, &iface_drv);

    printf("Driver...");    
    if (ESP_OK != scr_find_driver(SCREEN_CONTROLLER_ST7789, &lcd)) { 
        printf("Failed\n");
    };

    printf("Init...");    
    scr_controller_config_t lcd_cfg = {
        .interface_drv = iface_drv,
        .pin_num_rst = BOARD_LCD_RST,
        .pin_num_bckl = BOARD_LCD_BL,
        .rst_active_level = 0,
        .bckl_active_level = 0,
        .width = 240,
        .height = 240,
        .offset_hor = 0,
        .offset_ver = 0,
        .rotate = (scr_dir_t)0
    };    
    if (ESP_FAIL == lcd.init(&lcd_cfg)) {
        printf("Failed\n");
    }

    printf("OK\n");
    
    scr_info_t lcd_info;
    lcd.get_info(&lcd_info);
    printf("Screen name:%s | width:%d | height:%d\n", lcd_info.name, lcd_info.width, lcd_info.height);

    return ESP_OK;
}

void screen_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    lcd.draw_bitmap(x, y, w, h, bitmap);
}
