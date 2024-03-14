#include "global.h"

RTC_DATA_ATTR int bootCount = 0;
struct gps_data_t gps_data;
HT_st7736 st7735;
TinyGPSPlus gps;
#define VGNSS_CTRL 37
// test_status_t test_status;
uint16_t wifi_connect_try_num = 15;
bool resendflag = false;
bool deepsleepflag = false;
bool interrupt_flag = false;

void VextON(void)
{
	pinMode(Vext, OUTPUT);
	digitalWrite(Vext, LOW);
}

void VextOFF(void) // Vext default OFF
{
	pinMode(Vext, OUTPUT);
	digitalWrite(Vext, HIGH);
}

void enter_deepsleep(void)
{
	Radio.Sleep();
	SPI.end();
	pinMode(RADIO_DIO_1, ANALOG);
	pinMode(RADIO_NSS, ANALOG);
	pinMode(RADIO_RESET, ANALOG);
	pinMode(RADIO_BUSY, ANALOG);
	pinMode(LORA_CLK, ANALOG);
	pinMode(LORA_MISO, ANALOG);
	pinMode(LORA_MOSI, ANALOG);
	esp_sleep_enable_timer_wakeup(10 * 1000 * (uint64_t)1000);
	esp_deep_sleep_start();
}

void gps_test(void)
{
	String buffer0, buffer1, buffer2;
	pinMode(VGNSS_CTRL, OUTPUT);
	digitalWrite(VGNSS_CTRL, LOW);
	Serial1.begin(115200, SERIAL_8N1, 33, 34);
	Serial.println("gps_test");
	while (1)
	{
		if (Serial1.available() > 0)
		{
			if (Serial1.peek() != '\n')
			{
				gps.encode(Serial1.read());
			}
			else
			{
				Serial1.read();
				if (gps.time.second() == 0)
				{
					continue;
				}
				gps_data.year = gps.date.year();
				gps_data.month = gps.date.month();
				gps_data.day = gps.date.day();
				gps_data.hour = gps.time.hour();
				gps_data.min = gps.time.minute();
				gps_data.sec = gps.time.second();

				buffer0 = String(gps.time.hour()) + String(":") + String(gps.time.minute()) + String(":") + String(gps.time.second());
				Serial.println(buffer0);
				buffer1 = String(gps.location.lat(), 6) + String(",") + String(gps.location.lng(), 6);
				Serial.println(buffer1);
				Serial.flush();
				st7735.st7735_write_str(110, 0, "GPS", Font_7x10, ST7735_MAGENTA, ST7735_BLACK);
				st7735.st7735_write_str(0, 30, buffer0, Font_7x10, ST7735_WHITE, ST7735_BLACK);
				st7735.st7735_write_str(0, 50, buffer1, Font_7x10, ST7735_WHITE, ST7735_BLACK);
				delay(1000);
				pinMode(VGNSS_CTRL, OUTPUT);
				digitalWrite(VGNSS_CTRL, HIGH);
				delay(1000);

				break;
				delay(5000);
				while (Serial1.read() > 0)
					;
			}
		}
	}
}

void setup()
{
	Serial.begin(115200);
	Mcu.begin();

	print_wakeup_reason();
	++bootCount;
	Serial.println("Boot number: " + String(bootCount));
	Serial.flush();
	delay(1000);
	get_chip_id();

	st7735.st7735_init();
	st7735.st7735_fill_screen(ST7735_BLACK);

	// // attachInterrupt(0, interrupt_GPIO0, FALLING);
	// resendflag = false;
	// deepsleepflag = false;
	// interrupt_flag = false;
	pinMode(LED, OUTPUT);
	digitalWrite(LED, LOW);
	gps_test();
	// test_status = GPS_TEST;
}

void loop()
{
	lora_interval_send(10);
}
