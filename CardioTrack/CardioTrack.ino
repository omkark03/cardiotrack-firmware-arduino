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
// ECG debug mode: raw ECG at 250 Hz to Serial Plotter (Tools > Serial Plotter, 115200),
// or with ECG_STREAM_TEXT one summary line per second to the Serial Monitor.
#include "src/hal/factory.h"

static uint32_t nextTick;

#ifdef ECG_STREAM_TEXT
#include "src/core/quality.h"

static int16_t  win[Cfg::ECG_SAMPLE_HZ];   // the current second
static uint32_t winN = 0;
static uint32_t sampleNo = 0;
static uint32_t lastBeat = 0;
static int      thr = 4096;                // beat threshold, set from the previous second
static bool     above = false;
static uint32_t beats = 0;
static int      bpm = 0;

static void ecgText(int16_t v) {
  // beat = signal rising through the threshold, at least 250 ms after the previous beat
  bool hi = v > thr;
  if (hi && !above && sampleNo - lastBeat > Cfg::ECG_SAMPLE_HZ / 4) {
    if (lastBeat) bpm = (int)(60 * Cfg::ECG_SAMPLE_HZ / (sampleNo - lastBeat));
    lastBeat = sampleNo;
    beats++;
  }
  above = hi;
  sampleNo++;

  win[winN++] = v;
  if (winN < Cfg::ECG_SAMPLE_HZ) return;

  int lo = win[0], top = win[0];
  long sum = win[0];
  for (uint32_t i = 1; i < winN; i++) {
    if (win[i] < lo) lo = win[i];
    if (win[i] > top) top = win[i];
    sum += win[i];
  }
  int mean = (int)(sum / (long)winN);
  int meanMv = mean * 3300 / 4095;           // rough: the ESP32 ADC is not linear near the ends
  EcgQuality q = ecgQuality(win, winN);
  if (sampleNo - lastBeat > 2 * Cfg::ECG_SAMPLE_HZ) bpm = 0;   // no beat for 2 s

  const char* verdict;
  if (q.satPct > Cfg::ECG_SAT_MAX_PCT)        verdict = "BAD CONTACT (railed, check electrodes)";
  else if (q.stdCounts < Cfg::ECG_FLAT_STD)   verdict = "FLAT (no signal, check OUTPUT wire)";
  else if (bpm >= 40 && bpm <= 180)           verdict = "OK";
  else                                        verdict = "NO CLEAR BEATS (noise or still settling)";

  Serial.printf("mean=%4d (~%4d mV) min=%4d max=%4d swing=%4d railed=%5.1f%% beats=%lu bpm=%3d -> %s\n",
                mean, meanMv, lo, top, top - lo, (double)q.satPct, (unsigned long)beats, bpm, verdict);

  thr = q.stdCounts < Cfg::ECG_FLAT_STD ? 4096 : lo + (top - lo) * 6 / 10;
  winN = 0;
  beats = 0;
}
#endif

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
#ifdef ECG_STREAM_TEXT
  ecgText(ecg().readSample());
#else
  Serial.println(ecg().readSample());
#endif
  while ((int32_t)(micros() - nextTick) < 0) {}
}

#else
// Normal firmware: wake -> measure -> upload -> sleep (see src/app/app.cpp)
#include "src/app/app.h"

void setup() { App::setup(); }
void loop()  { App::loop(); }

#endif
