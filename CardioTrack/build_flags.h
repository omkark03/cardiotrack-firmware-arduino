#pragma once
// ---------------------------------------------------------------------------
// Arduino IDE has no "build environments", so the PlatformIO env choice lives here.
// Change ONLY the number below, save, and press Upload.
//
//   1 = MOCK_BENCH   fake sensors, no deep sleep (delay), 10 s burst / 30 s cycle   (fast pipeline test)
//   2 = MOCK_FULL    fake sensors, REAL deep sleep, 60 s burst / 300 s cycle
//   3 = STREAM_MOCK  fake ECG streamed to Serial Plotter
//   4 = STREAM_REAL  real AD8232 ECG streamed to Serial Plotter
//   5 = REAL_BENCH   real sensors, no deep sleep, 10 s / 30 s
//   6 = PCB          real sensors, real deep sleep, 60 s / 300 s, battery divider
// ---------------------------------------------------------------------------
#ifndef CT_PROFILE
#define CT_PROFILE 1
#endif

// Mix and match while wiring sensors: with profile 5 or 6, uncomment the ones NOT wired yet.
// #define USE_MOCK_ECG
// #define USE_MOCK_PPG
// #define USE_MOCK_IMU
// #define USE_MOCK_TEMP

#if CT_PROFILE == 1
  #define BOARD_BREADBOARD_V1
  #define USE_MOCK_SENSORS
  #define BENCH_MODE
  #define CFG_BURST_SEC 10
  #define CFG_CYCLE_SEC 30
#elif CT_PROFILE == 2
  #define BOARD_BREADBOARD_V1
  #define USE_MOCK_SENSORS
#elif CT_PROFILE == 3
  #define BOARD_BREADBOARD_V1
  #define USE_MOCK_SENSORS
  #define APP_ECG_STREAM
#elif CT_PROFILE == 4
  #define BOARD_BREADBOARD_V1
  #define APP_ECG_STREAM
#elif CT_PROFILE == 5
  #define BOARD_BREADBOARD_V1
  #define BENCH_MODE
  #define CFG_BURST_SEC 10
  #define CFG_CYCLE_SEC 30
#elif CT_PROFILE == 6
  #define BOARD_PCB_V1
  #define BATTERY_PRESENT
#else
  #error "CT_PROFILE must be 1..6 (see build_flags.h)"
#endif
