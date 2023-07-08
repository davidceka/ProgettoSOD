#include <Wire.h>
#include <Adafruit_PN532.h>
#include "RTClib.h"

#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield

Adafruit_PN532 nfc(IRQ, RESET);  
RTC_DS1307 rtc;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Errore di inizializzazione del lettore NFC");
    while (1);
  }

  nfc.SAMConfig();

  //DateTime oraCorrente(2023, 06, 28, 14, 46, 0);
  //rtc.adjust(oraCorrente);
}

void loop() {
  // Leggi il tag NFC se presente
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    DateTime now = rtc.now();

    // Stampa l'ora
    Serial.print("Ora: ");
    Serial.print(now.hour(), DEC);
    Serial.print(":");
    Serial.print(now.minute(), DEC);
    Serial.print(":");
    Serial.println(now.second(), DEC);

    // Stampa la data
    Serial.print("Data: ");
    Serial.print(now.day(), DEC);
    Serial.print("/");
    Serial.print(now.month(), DEC);
    Serial.print("/");
    Serial.println(now.year(), DEC);

    delay(1000); // Aggiungi un ritardo per evitare la lettura continua del tag
  }
}
