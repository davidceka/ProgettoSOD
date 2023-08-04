#include <Wire.h>
#include <Adafruit_PN532.h>
#include <RTClib.h>
#include <Arduino_FreeRTOS.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>



// Dichiarazione dei pin per la comunicazione I2C con il modulo NFC PN532
#define IRQ_PIN 2
#define RESET_PIN 3
#define LED_PIN 12
#define CMD_SYNC_RTC "SYNC_RTC"


// Dichiarazione degli oggetti PN532 e RTC
Adafruit_PN532 nfc(IRQ_PIN, RESET_PIN);
RTC_DS1307 rtc;
BH1750 lightSensor;
Adafruit_BMP280 bmp;


// Dichiarazione dei task
TaskHandle_t nfcTaskHandle;
TaskHandle_t sensorTaskHandle;


// Dichiarazione della struttura dati per il trasferimento seriale
struct DataPacket {
  float temperature;
  float pressure;
  float luminosity;
  bool ledState;
  uint8_t peopleCount;
  uint32_t timestamp;
} dataPacket;

// Prototipi delle funzioni dei task
void nfcTask(void *pvParameters);
void sensorTask(void *pvParameters);


void setup() {
  Serial.begin(115200);
  Wire.begin();

  //impostazione pin led
  pinMode(LED_PIN,OUTPUT);

  // Inizializzazione del modulo NFC PN532
  if(nfc.begin()){
    Serial.println(F("Modulo NFC Adafruit PN532 Inizializzato correttamente."));
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
      Serial.println("Errore di inizializzazione del lettore NFC");
      while (1);
    }
    nfc.SAMConfig();
  }

  // Inizializzazione del modulo RTC DS1307
  
  if (!rtc.begin()) {
    Serial.println(F("Impossibile inizializzare il modulo RTC. Verifica il collegamento!"));
    while (1);
  }
  else{
    Serial.println(F("Modulo RTC Inizializzato correttamente."));
  }
  
  if (!lightSensor.begin()) {
    Serial.println(F("Impossibile inizializzare il sensore di luminosità BH1750. Verifica il collegamento!"));
    while (1);
  }
  else{
    Serial.println(F("Light Sensor BH1750 Inizializzato correttamente."));
  }
  
  if (!bmp.begin()) {
    Serial.println(F("Impossibile trovare il sensore BMP280. Verifica il collegamento!"));
    while (1);
  }
  else{
    Serial.println(F("Temperature/Pressure Module BMP280 Inizializzato correttamente."));
  }

  // Creazione del task NFC
  xTaskCreate(nfcTask, "NFCTask", 256, NULL, 1, &nfcTaskHandle);
  xTaskCreate(sensorTask, "SensorTask", 128, NULL, 2, &sensorTaskHandle);

  vTaskStartScheduler(); // Avvia lo scheduler del sistema operativo

}

void loop() {
  // Il loop principale non fa nulla
}

void nfcTask(void *pvParameters) {
  (void) pvParameters;
  String data_=String();
  while (1) {
    // Leggi il tag NFC se presente
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    Serial.println(success);
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

    //sincronizzazione RTC
    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n'); // Leggi la stringa fino a trovare il carattere newline
      command.trim(); // Rimuovi eventuali spazi bianchi all'inizio e alla fine

      // Controlla se il comando ricevuto è "SYNC_RTC"
      if (command.equals(CMD_SYNC_RTC)) {
        // Esegui la sincronizzazione dell'RTC
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        Serial.println(F("RTC sincronizzato con il timestamp di compilazione."));
      }
    }

    // Aggiungi un ritardo per evitare la lettura continua del tag
    if(digitalRead(LED_PIN)==HIGH)
    {
      digitalWrite(LED_PIN,LOW);
    }
    else
    {
      digitalWrite(LED_PIN,HIGH);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void sensorTask(void *pvParameters) {
  (void) pvParameters;
  TickType_t lastWakeTime = xTaskGetTickCount();
  const TickType_t samplingInterval = pdMS_TO_TICKS(1000); // Intervallo di campionamento di 1 secondo

  String data_=String();

  
  for (;;) {
    // Lettura dei valori dai sensori
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure() / 100.0; // Conversione da Pascal a hPa
    float luminosity = lightSensor.readLightLevel();

    // Aggiornamento dei valori nella struttura dati
    dataPacket.temperature = temperature;
    dataPacket.pressure = pressure;
    dataPacket.luminosity = luminosity;
    //Serial.println("Dati Acquisiti.");
    // Aggiornamento del timestamp
    DateTime now = rtc.now();
    dataPacket.timestamp = now.unixtime();

    // Invio dei dati tramite comunicazione seriale
    //Serial.write((uint8_t*)&dataPacket, sizeof(dataPacket));
    /*data_= (String)dataPacket.temperature + ";" +
                      (String)dataPacket.pressure + ";" +
                      (String)dataPacket.luminosity + ";" +
                      (String)dataPacket.ledState + ";" +
                      (String)dataPacket.peopleCount + ";" +
                      (String)dataPacket.timestamp;*/

    //Serial.flush();
    Serial.println("22;200;35;0;1;1691165740");
    //Serial.println(sizeof(dataPacket));

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}
