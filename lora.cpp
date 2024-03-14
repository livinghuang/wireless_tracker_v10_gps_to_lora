#include "global.h"
#define BUFFER_SIZE 30 // Define the payload size here
static void prepareTxFrame(uint8_t port);
typedef enum
{
  LOWPOWER,
  STATE_RX,
  STATE_TX
} States_t;

int16_t txNumber = 0;
int16_t rxNumber = 0;
States_t state;
bool sleepMode = false;
int16_t Rssi, rxSize;

String rssi = "RSSI --";
String packet;
String send_num;

unsigned int counter = 0;
bool receiveflag = false; // software flag for LoRa receiver, received data makes it true.
long lastSendTime = 0;    // last send time
int interval = 1000;      // interval between sends
int16_t RssiDetection = 0;
char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];
void OnTxDone(void)
{
  Serial.print("TX done......");
  state = STATE_RX;
}

void OnTxTimeout(void)
{
  Radio.Sleep();
  Serial.print("TX Timeout......");
  state = STATE_TX;
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
  rxNumber++;
  Rssi = rssi;
  rxSize = size;
  memcpy(rxpacket, payload, size);
  rxpacket[size] = '\0';
  Radio.Sleep();
  Serial.printf("\r\nreceived packet \"%s\" with Rssi %d , length %d\r\n", rxpacket, Rssi, rxSize);
  Serial.println("wait to send next packet");
  receiveflag = true;
  state = STATE_TX;
}
void lora_init(void)
{
  txNumber = 0;
  Rssi = 0;
  rxNumber = 0;
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxDone = OnRxDone;

  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);
  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, 3000);

  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
  state = STATE_TX;
  // Serial.println("waiting lora data!");
}
void lora_status_handle(void)
{
  if (resendflag)
  {
    state = STATE_TX;
    resendflag = false;
  }

  if (receiveflag && (state == LOWPOWER))
  {
    receiveflag = false;
    packet = "Rdata:";
    int i = 0;
    while (i < rxSize)
    {
      packet += rxpacket[i];
      i++;
    }
    String packSize = "R_Size:";
    packSize += String(rxSize, DEC);
    packSize += " R_rssi:";
    packSize += String(Rssi, DEC);
    send_num = "send num:";
    send_num += String(txNumber, DEC);
    delay(100);
    Serial.println(packet);
    Serial.println(packSize);
    Serial.println(send_num);
    if ((rxNumber % 2) == 0)
    {
      // digitalWrite(LED, HIGH);
    }
  }
  switch (state)
  {
  case STATE_TX:
    delay(1000);
    txNumber++;
    sprintf(txpacket, "hello %d,Rssi:%d", txNumber, Rssi);
    Serial.printf("\r\nsending packet \"%s\" , length %d\r\n", txpacket, strlen(txpacket));
    Radio.Send((uint8_t *)txpacket, strlen(txpacket));
    state = LOWPOWER;
    break;
  case STATE_RX:
    Serial.println("into RX mode");
    Radio.Rx(0);
    state = LOWPOWER;
    break;
  case LOWPOWER:
    Radio.IrqProcess();
    break;
  default:
    break;
  }
}

static void prepareTxFrame(uint8_t port)
{
  appDataSize = sizeof(gps_data) + 1;
  memcpy(appData, &gps_data, appDataSize);
  printHex(appData, appDataSize);
}

void set_radio_off(void)
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
}

void lora_interval_send(uint8_t interval_second)
{
  lora_init();
  prepareTxFrame(0);
  uint8_t resend = 3;
  int32_t delay_time = 100;
  while (resend--)
  {
    Radio.Send((uint8_t *)appData, appDataSize); // 发送数据包
    delay(delay_time + randr(0, delay_time));
  }

  set_radio_off();
  esp_sleep_enable_timer_wakeup(interval_second * 1000 * 1000);
  esp_deep_sleep_start();
  while (1)
  {
  }
}

void lora_continuous_receive(void)
{
  if (receiveflag)
  {
    receiveflag = false;
    packet = "Rdata:";
    int i = 0;
    while (i < rxSize)
    {
      packet += rxpacket[i];
      i++;
    }
    String packSize = "R_Size:";
    packSize += String(rxSize, DEC);
    packSize += " R_rssi:";
    packSize += String(Rssi, DEC);
    send_num = "send num:";
    send_num += String(txNumber, DEC);
    delay(100);
    Serial.println(packet);
    Serial.println(packSize);
    Serial.println(send_num);
  }
  switch (state)
  {
  case STATE_TX:
    state = STATE_RX;
    break;
  case STATE_RX:
    Serial.println("into RX mode");
    Radio.Rx(0);
    state = LOWPOWER;
    break;
  case LOWPOWER:
    Radio.IrqProcess();
    break;
  default:
    break;
  }
}
