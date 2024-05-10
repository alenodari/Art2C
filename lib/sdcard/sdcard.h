#pragma once

#ifndef _SDCARD_H_
#define _SDCARD_H_

bool sd_card_init();
void writeFile(fs::FS &fs, char*, uint8_t*, int);

#endif