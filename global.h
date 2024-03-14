#ifndef GLOBAL_H
#define GLOBAL_H

#include "Arduino.h"
#include "esp_system.h"
#include "LoRaWan_APP.h"
#include "bsp.h"
#include "HT_st7736.h"
#include "HT_TinyGPS++.h"
#include "WiFi.h"
#include "Wire.h"
#include "esp_sleep.h"
#include <esp_timer.h>
#include <mbedtls/sha256.h>

// #include <Adafruit_MAX31865.h>

// #include "FS.h"
// #include "SD.h"
// #include "SPI.h"

// #include "bat.h"
// #include "lorawan.h"
#include "lora.h"
// #include "Dps310.h"
// #include "DpsClass.h"
// #include "HDC1080.h"
// #include "BMP280.h"
// #include "led.h"
// #include "_wifi.h"
// #include "sensor.h"
// #include "rs485.h"
// #include "max31865.h"
// #include "sd_card.h"
// #include "lora_to_lorawan_repeater.h"

extern bool resendflag;
extern bool deepsleepflag;
extern bool interrupt_flag;
extern bool reset_run_with_time_escape;
extern RTC_DATA_ATTR int bootCount; // 增加发送的数据包编号

extern HT_st7736 st7735;
extern TinyGPSPlus gps;

struct gps_data_t
{
  uint32_t time = 0;
  uint16_t year = 0;
  uint16_t month = 0;
  uint16_t day = 0;
  uint16_t hour = 0;
  uint16_t min = 0;
  uint16_t sec = 0;

  float lat = 0;
  float lng = 0;
  float alt = 0;
};
extern struct gps_data_t gps_data;

void printHex(byte *data, int length);
void run_with_time_escape(uint64_t escape_period_ms, void (*callback)(), void (*stop_callback)());
esp_sleep_wakeup_cause_t print_wakeup_reason();
uint64_t get_chip_id();
#endif