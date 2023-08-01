#include <Wire.h>
#include <Adafruit_PN532.h>
#include <RTClib.h>
#include <BH1750.h>
#include <Adafruit_BMP280.h>


#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield

Adafruit_PN532 nfc(IRQ, RESET);  
RTC_DS1307 rtc;
BH1750 lightMeter;
Adafruit_BMP280 bmp;
int LEDPIN=7;
bool presenze=false;


void setup() {
  //led per il passa
  pinMode(LEDPIN,OUTPUT);


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

  if (!bmp.begin()) {
    Serial.println("Errore inizializzazione BMP280");
    while (1);
  }

  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Errore inizializzazione BH1750");
    while (1);
  }

  //DateTime oraCorrente(2023, 06, 28, 14, 46, 0);
  //rtc.adjust(oraCorrente);
}

void loop() {
  // Leggi il tag NFC se presente
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;
  float lux = lightMeter.readLightLevel();

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if(lux<30 && presenze){
    digitalWrite(LEDPIN, HIGH)
  }
  else{
    digitalWrite(LEDPIN,LOW)
  }
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

    // lettura sensori 
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Pressione: ");
    Serial.print(pressure);
    Serial.println(" hPa");

    Serial.print("Luminosità: ");
    Serial.print(lux);
    Serial.println(" lux");

    Serial.println();

    delay(1000); // Aggiungi un ritardo per evitare la lettura continua del tag
  }
}
