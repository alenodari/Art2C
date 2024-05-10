// #include "esp_vfs_fat.h"
// #include "sdmmc_cmd.h"
// #include "driver/sdmmc_host.h"
#include <Arduino.h>
#include <SPI.h>
#include <SD_MMC.h>

#include "sdcard.h"
// #include <SD.h>

SPIClass mySPI(0);

int clk = 39;
int cmd = 38;
int d0  = 40;

bool sd_card_init() {
    printf("SD Card Init... ");
    if(!SD_MMC.setPins(clk, cmd, d0)){
        printf("Pins failed\n");
        return false;
    }
    if(!SD_MMC.begin("/sdcard", true)){
        printf("Mount Failed\n");
        return false;
    }

    uint8_t cardType = SD_MMC.cardType();

    if(cardType == CARD_NONE){
        printf("No SD Card attached\n");
        return false;
    }

    printf("Type: ");
    if(cardType == CARD_MMC){
        printf("MMC... ");
    } else if(cardType == CARD_SD){
        printf("SDSC... ");
    } else if(cardType == CARD_SDHC){
        printf("SDHC... ");
    } else {
        printf("UNKNOWN... ");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    printf("Size: %lluMB\nOK\n", cardSize);
    return true;
}

void writeFile(fs::FS &fs, char* path, uint8_t* content, int content_size) {
    printf("SD Card Writing file: %s... ", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        printf("Failed to open file\n");
        return;
    }
    int written_size = file.write(content, content_size);
    file.flush();
    if(written_size == content_size){
        printf("OK\n");
    } else {
        printf("Failed\n");
    }
}

