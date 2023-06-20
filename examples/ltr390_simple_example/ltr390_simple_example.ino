//
// LTR390 simple example
// Written by Larry Bank
// email: bitbank@pobox.com
// Copyright (c) 2023 BitBank Software, Inc.
//
#include <bb_ltr390.h>

#ifdef ARDUINO_ADAFRUIT_QTPY_ESP32S2
// -1 signifies to use the default I2C pin definitions for this target board
#define SDA_PIN -1
#define SCL_PIN -1
// Set this to 1 to use Bit Bang I2C, 0 to use hardware I2C
#define BITBANG 0

#else // Generic boards (e.g. ESP32 Dev Module). No predefined pins for I2C

#define BITBANG 0
// ESP32-S3 Xiao
//#define SDA_PIN 5
//#define SCL_PIN 6

// ESP32-C3 Xiao
#define SDA_PIN 6
#define SCL_PIN 7
#endif

LTR390 ltr;

#ifdef ARDUINO_FEATHERS2
#define POWER_PIN 21
#elif defined (ARDUINO_FEATHERS3)
#define POWER_PIN 39
#else
#define POWER_PIN -1
#endif

void setup() {
char szTemp[64];
uint32_t u32UV;
float fUVI;
int rc;

  Serial.begin(115200);
  delay(3000); // Give time for CDC serial service to start
  Serial.println("Starting...");

  if (POWER_PIN != -1) {
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, HIGH); // Enable second LDO if present
  }

  if (ltr.init(SDA_PIN, SCL_PIN, BITBANG) == LTR390_SUCCESS) { // found a supported device
    Serial.println("LTR390 init success!");
    if (ltr.start(true) == LTR390_SUCCESS) { // start sampling
      while (1) {
        ltr.getSample();
        u32UV = ltr.uv();
        fUVI = ltr.getUVI();
        sprintf(szTemp, "Raw: %d, UVI: %0.1f\n", u32UV, fUVI); // show the raw UV reading and the UV index value
        Serial.print(szTemp);
        delay(1000); // Update the info once every second
      }
    }
  }
} /* setup() */

void loop() {
  // nothing to do here
} /* loop() */
