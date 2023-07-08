#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <BH1750.h>

Adafruit_BMP280 bmp;
BH1750 lightMeter;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!bmp.begin()) {
    Serial.println("Errore inizializzazione BMP280");
    while (1);
  }

  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Errore inizializzazione BH1750");
    while (1);
  }
}

void loop() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;
  float lux = lightMeter.readLightLevel();

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

  delay(1000);
}