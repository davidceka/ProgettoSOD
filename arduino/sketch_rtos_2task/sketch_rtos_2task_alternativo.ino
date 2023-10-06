#include <Wire.h>
#include <Adafruit_PN532.h>
#include <RTClib.h>
#include <Arduino_FreeRTOS.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <semphr.h>

// Dichiarazione dei pin per la comunicazione I2C con il modulo NFC PN532
#define IRQ_PIN 2
#define RESET_PIN 3
#define LED_PIN 11
#define CMD_SYNC_RTC "SYNC_RTC"
#define LIGHT_THRESHOLD 100
#define MAX_BRIGHTNESS 255 // Luminosità massima (valore PWM)
#define MIN_BRIGHTNESS 0 // Luminosità minima (valore PWM)

// Dichiarazione degli oggetti PN532 e RTC
Adafruit_PN532 nfc(IRQ_PIN, RESET_PIN);
RTC_DS1307 rtc;
BH1750 lightSensor;
Adafruit_BMP280 bmp;

// Dichiarazione dei task
TaskHandle_t nfcTaskHandle;
TaskHandle_t sensorTaskHandle;

// Semaforo per la comunicazione seriale
SemaphoreHandle_t serialMutex;

char tag0[8]="EB5BD4EC";
char tag1[8]="E2F6925D";
int presenze[2]={0,0};

// Conteggio delle persone attualmente nella stanza
int peopleCount = 0;
int ledState=0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  serialMutex = xSemaphoreCreateMutex();

  // Impostazione pin led
  pinMode(LED_PIN, OUTPUT);

  // Inizializzazione del modulo NFC PN532
  if (nfc.begin()) {
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
  else {
    Serial.println(F("Modulo RTC Inizializzato correttamente."));
  }

  if (!lightSensor.begin()) {
    Serial.println(F("Impossibile inizializzare il sensore di luminosità BH1750. Verifica il collegamento!"));
    while (1);
  }
  else {
    Serial.println(F("Light Sensor BH1750 Inizializzato correttamente."));
  }

  if (!bmp.begin()) {
    Serial.println(F("Impossibile trovare il sensore BMP280. Verifica il collegamento!"));
    while (1);
  }
  else {
    Serial.println(F("Temperature/Pressure Module BMP280 Inizializzato correttamente."));
  }

  // Creazione del task NFC
  xTaskCreate(nfcTask, "NFCTask", 312, NULL, 1, &nfcTaskHandle);
  xTaskCreate(sensorTask, "SensorTask", 200, NULL, 2, &sensorTaskHandle);

  vTaskStartScheduler(); // Avvia lo scheduler del sistema operativo
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
    char readTag[8]="";
    int action=0;
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success) {
      for(int i=0;i<uidLength;i++){
        sprintf(readTag + 2 * i, "%02X", uid[i]); // 2 caratteri esadecimali per ciascun byte
      }
      //if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE){
        if(!strcmp(readTag,tag0)){
          
            if(presenze[0]==0){
              presenze[0]++;
              peopleCount++;
              action=1;
            }
            else{
              presenze[0]--;
              peopleCount--;
              action=0;
            }
        }
        else{
          if(presenze[1]==0){
              presenze[1]++;
              peopleCount++;
              action=1;
            }
            else{
              presenze[1]--;
              peopleCount--;
              action=0;
            }
        }
        //xSemaphoreGive(serialMutex); // Rilascia il semaforo dopo l'invio

      //}
      DateTime now = rtc.now();

      Serial.print(readTag);
      Serial.print(";");
      Serial.print(now.unixtime());
      Serial.print(";");
      Serial.println(action);

      // Aggiungi un ritardo per evitare la lettura continua del tag
    
      vTaskDelay(pdMS_TO_TICKS(2000));
    }

    // Sincronizzazione RTC
    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');
      command.trim();

      if (command.startsWith(CMD_SYNC_RTC)) {
        // Estrai il timestamp dal comando
        String timestampString = command.substring(strlen(CMD_SYNC_RTC));
        unsigned long timestamp = timestampString.toInt();

        // Converti il timestamp in un oggetto DateTime e imposta l'orologio RTC
        DateTime syncTime = DateTime(timestamp);
        rtc.adjust(syncTime);

        Serial.println(F("RTC sincronizzato con il timestamp fornito da Python."));
      }
}

    
  }
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
    if(luminosity>LIGHT_THRESHOLD||peopleCount==0){
      digitalWrite(LED_PIN,LOW);
      ledState=0;
    }
    else{
      int brightness = map(luminosity, 0, LIGHT_THRESHOLD, MAX_BRIGHTNESS, MIN_BRIGHTNESS);
      analogWrite(LED_PIN, brightness);
      ledState=1;
    }
    // Aggiornamento del timestamp
    DateTime now = rtc.now();

    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
      // Proteggi l'accesso alla porta seriale con il semaforo
      Serial.print(temperature);
      Serial.print(";");
      Serial.print(pressure);
      Serial.print(";");
      Serial.print(luminosity);
      Serial.print(";");
      Serial.print(ledState);
      Serial.print(";");
      Serial.print(peopleCount);
      Serial.print(";");
      Serial.print(now.unixtime());
      Serial.print("\n");
      

      xSemaphoreGive(serialMutex); // Rilascia il semaforo dopo l'invio
    }


    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}
