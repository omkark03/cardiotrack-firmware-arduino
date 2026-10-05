// CardioTrack chest hub firmware (Arduino IDE version).
//
// Board: "ESP32 Dev Module"    Core: esp32 by Espressif, version 2.0.17 (NOT 3.x)
// Tools > Partition Scheme: "No OTA (2MB APP/2MB SPIFFS)"
// Serial Monitor: 115200 baud
// Pick the mode in build_flags.h (CT_PROFILE). Copy secrets.example.h to secrets.h first.

#include <Arduino.h>
#include "build_flags.h"
#include "board_pins.h"
#include "config.h"

#ifdef APP_ECG_STREAM
// ECG debug mode: raw ECG at 250 Hz to Serial Plotter (Tools > Serial Plotter, 115200).
#include "src/hal/factory.h"

static uint32_t nextTick;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.printf("# ECG stream | ECG pin=%d | ", Pins::ECG_SIG);
#ifdef USE_MOCK_SENSORS
  Serial.println("MOCK");
#else
  Serial.println("REAL");
#endif
  delay(500);
  ecg().begin();
  nextTick = micros();
}

void loop() {
  nextTick += 1000000UL / Cfg::ECG_SAMPLE_HZ;
  Serial.println(ecg().readSample());
  while ((int32_t)(micros() - nextTick) < 0) {}
}

#else
// Normal firmware: wake -> measure -> upload -> sleep (see src/app/app.cpp)
#include "src/app/app.h"

void setup() { App::setup(); }
void loop()  { App::loop(); }

#endif
