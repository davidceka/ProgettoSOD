#include <Wire.h>
#include <Adafruit_PN532.h>
#include <RTClib.h>
#include <Arduino_FreeRTOS.h>



// Dichiarazione dei pin per la comunicazione I2C con il modulo NFC PN532
#define IRQ_PIN 2
#define RESET_PIN 3

// Dichiarazione degli oggetti PN532 e RTC
Adafruit_PN532 nfc(IRQ_PIN, RESET_PIN);
RTC_DS1307 rtc;

// Dichiarazione del task NFC
TaskHandle_t nfcTaskHandle;

// Prototipi delle funzioni dei task
void nfcTask(void *pvParameters);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Inizializzazione del modulo NFC PN532
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Errore di inizializzazione del lettore NFC");
    while (1);
  }
  nfc.SAMConfig();

  // Inizializzazione del modulo RTC DS1307
  rtc.begin();
  if (!rtc.isrunning()) {
    Serial.println("Modulo RTC non trovato");
    while (1);
  }

  // Creazione del task NFC
  xTaskCreate(nfcTask, "NFCTask", 100, NULL, 1, &nfcTaskHandle);
}

void loop() {
  // Il loop principale non fa nulla
}

void nfcTask(void *pvParameters) {
  (void) pvParameters;

  while (1) {
    // Leggi il tag NFC se presente
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success) {
      DateTime now = rtc.now();

      // Stampa l'ora corrente
      Serial.print("Ora: ");
      Serial.print(now.hour(), DEC);
      Serial.print(":");
      Serial.print(now.minute(), DEC);
      Serial.print(":");
      Serial.println(now.second(), DEC);

      // Stampa la data corrente
      Serial.print("Data: ");
      Serial.print(now.day(), DEC);
      Serial.print("/");
      Serial.print(now.month(), DEC);
      Serial.print("/");
      Serial.println(now.year(), DEC);
    }

    // Aggiungi un ritardo per evitare la lettura continua del tag
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
