#include <Wire.h>
#include <Adafruit_PN532.h>
#include <RTClib.h>
#include <Arduino_FreeRTOS.h>
#include <SerialTransfer.h>

// Pin per la comunicazione seriale con Raspberry Pi
#define SERIAL_RX_PIN 0
#define SERIAL_TX_PIN 1

// Pin per la comunicazione I2C con i sensori
#define SDA_PIN A4
#define SCL_PIN A5

// Definizioni per il sensore di luminosità BH1750
#include <BH1750.h>
BH1750 lightSensor;

// Definizioni per il sensore BMP280
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;

// Definizioni per il lettore NFC
#define IRQ_PIN 2
#define RESET_PIN 3
Adafruit_PN532 nfc(IRQ_PIN, RESET_PIN);

// Definizioni per il modulo RTC
RTC_DS1307 rtc;

// Dichiarazione dei task
TaskHandle_t sensorTaskHandle;
TaskHandle_t nfcTaskHandle;
TaskHandle_t serialTaskHandle;

// Dichiarazione della struttura dati per il trasferimento seriale
struct DataPacket {
  float temperature;
  float pressure;
  float luminosity;
  bool ledState;
  uint8_t peopleCount;
  uint32_t timestamp;
} dataPacket;

SerialTransfer serialTransfer;

// Prototipi delle funzioni dei task
void sensorTask(void *pvParameters);
void nfcTask(void *pvParameters);
void serialTask(void *pvParameters);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  nfc.begin();
  lightSensor.begin();
  if (!bmp.begin()) {
    Serial.println(F("Impossibile trovare il sensore BMP280. Verifica il collegamento!"));
    while (1);
  }

  serialTransfer.begin(Serial);
  serialTransfer.rxObj(dataPacket);

  // Creazione dei task
  xTaskCreate(sensorTask, "SensorTask", 128, NULL, 2, &sensorTaskHandle);
  xTaskCreate(nfcTask, "NFCTask", 128, NULL, 1, &nfcTaskHandle);
  xTaskCreate(serialTask, "SerialTask", 128, NULL, 1, &serialTaskHandle);
}

void loop() {
  // Non viene utilizzato il loop principale
}

void sensorTask(void *pvParameters) {
  (void) pvParameters;
  TickType_t lastWakeTime = xTaskGetTickCount();
  const TickType_t samplingInterval = pdMS_TO_TICKS(1000); // Intervallo di campionamento di 1 secondo

  for (;;) {
    // Lettura dei valori dai sensori
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure() / 100.0; // Conversione da Pascal a hPa
    float luminosity = lightSensor.readLightLevel();

    // Aggiornamento dei valori nella struttura dati
    dataPacket.temperature = temperature;
    dataPacket.pressure = pressure;
    dataPacket.luminosity = luminosity;

    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Pressione: ");
    Serial.print(pressure);
    Serial.println(" hPa");

    Serial.print("Luminosità: ");
    Serial.print(luminosity);
    Serial.println(" lux");

    Serial.println();


    vTaskDelayUntil(&lastWakeTime, samplingInterval);
  }
}

void nfcTask(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success) {
      // Incremento il conteggio delle persone nella stanza
      dataPacket.peopleCount++;
      Serial.print("trovato id");
    }

    vTaskDelay(pdMS_TO_TICKS(200)); // Intervallo di scansione NFC di 200 ms
  }
}

void serialTask(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    // Aggiornamento del timestamp
    DateTime now = rtc.now();
    dataPacket.timestamp = now.unixtime();

    // Invio dei dati tramite comunicazione seriale
    serialTransfer.sendData(sizeof(dataPacket));
    vTaskDelay(pdMS_TO_TICKS(1000)); // Intervallo di invio dati di 1 secondo
  }
}
