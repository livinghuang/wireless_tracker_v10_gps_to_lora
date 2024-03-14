#include "global.h"

// sensor_data_t global_sensor_result;

void printHex(byte *data, int length)
{
  for (int i = 0; i < length; i++)
  {
    // Print each byte in hexadecimal format with leading zeros
    if (data[i] < 0x10)
    {
      Serial.print("0");
    }
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println(); // Print a newline character at the end
}

// Default stop callback function
void default_stop_callback()
{
  printf("Default Stop Callback\n");
}
// Function that runs a loop with a time-based escape period and calls the callback function
// It also has a timeout condition and an optional stop_callback
bool reset_run_with_time_escape = false;
void run_with_time_escape(uint64_t escape_period_ms, void (*callback)(), void (*stop_callback)())
{
  if (stop_callback == NULL)
  {
    stop_callback = default_stop_callback; // Use the default stop callback if not provided
  }
  uint64_t start_time = esp_timer_get_time(); // Get system timer value in microseconds
  while (1)
  {
    uint64_t escaped_time = (esp_timer_get_time() - start_time) / 1000; // Get system timer value in microseconds
    if (escaped_time >= escape_period_ms)
    {
      stop_callback();
      printf("run_with_time_escape done\n");
      break; // Exit the loop when the timeout is reached
    }
    else
    {
      callback(); // Call the callback function
    }
    // Other processing can go here
    if (reset_run_with_time_escape)
    {
      reset_run_with_time_escape = false;
      printf("Resetting run_with_time_escape\n");
      start_time = esp_timer_get_time();
    }
    // Sleep for a short interval to avoid busy-waiting
    delay(10); // Sleep for 1 millisecond
  }
}

esp_sleep_wakeup_cause_t print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }

  return wakeup_reason;
}

uint64_t get_chip_id()
{
  uint64_t chipid = ESP.getEfuseMac();
  Serial.printf("ESP32ChipID=%04X%08X\n", (uint16_t)(chipid >> 32), (uint32_t)chipid);
  return chipid;
}
