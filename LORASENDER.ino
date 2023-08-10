#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <LoRa.h>
#include <Time.h>

int analogInput = 0;
float vout = 0.0;
float vin = 0.0;
float R1 = 3720.0; // сопротивление R1 (3K)
float R2 = 3120.0; // сопротивление R2 (3.9K)

int value = 0;

int telemetrypacknum = 0;

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);

void setup() {
  pinMode(analogInput, INPUT);
  LoRa.begin(436.250E6);
  ss.begin(GPSBaud);
  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(9);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.enableCrc();
  LoRa.setGain(6);
}

void loop() {
  value = analogRead(analogInput);
  vout = (value * 5.0) / 1024.0;
  vin = vout / (R2 / (R1 + R2));
  if (vin < 0.09) {
    vin = 0.0;
  }
  pinMode(8, OUTPUT);
  LoRa.beginPacket();
  LoRa.print("USTUGSAT_TO_UB1QBM");
  LoRa.print("/");
  LoRa.print(telemetrypacknum); // НОМЕР ПАКЕТА
  LoRa.print("/");
  LoRa.print(gps.time.hour());
  LoRa.print(":");
  LoRa.print(gps.time.minute());
  LoRa.print(":");
  LoRa.print(gps.time.second());
  LoRa.print("/");
  LoRa.print(gps.satellites.value()); // КОЛ-ВО СПУТНИКОВ
  LoRa.print("/");
  LoRa.print(gps.location.lat(), 6); // ШИРОТА
  LoRa.print("/");
  LoRa.print(gps.location.lng(), 6); // ДОЛГОТА
  LoRa.print("/");
  LoRa.print(gps.altitude.meters()); // ВЫСОТА
  LoRa.print("/");
  LoRa.print(gps.speed.kmph()); // СКОРОСТЬ
  LoRa.print("/");
  LoRa.print(gps.course.deg()); // КУРС
  LoRa.print("/");
  LoRa.print(vin * 1.0144357);
  LoRa.print("V");
  LoRa.print("/");
  LoRa.print("END");
  LoRa.endPacket();

  telemetrypacknum++;
  digitalWrite(8, HIGH);
  smartDelay(2000);
  digitalWrite(8, LOW);
}

static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available()) {
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}
