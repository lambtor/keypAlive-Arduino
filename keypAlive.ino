#include "Keyboard.h"
#include "Adafruit_NeoPixel.h"

// written by lambtor
// 2026.01.13
// usb stack: arduino
// programmer: J-link over openOCD

Adafruit_NeoPixel oStrip = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
int16_t nBright = 20;
const long nPingInterval = 1000;
unsigned long nFullInterval = 240000;
unsigned long nMillisCheck = 0;
unsigned long nLastFull = 0;
unsigned long nLastPing = 0;
const int16_t nColorRed = 112;
const int16_t nColorGreen = 128;
const int16_t nColorBlue = 0;
//  112,128,0

void setup() {
  //start strip as blank
  oStrip.begin();
  oStrip.setBrightness(nBright);
  oStrip.show();
  //initialize both of the state checks so they're relative to "now"
  nMillisCheck = millis();
  nLastFull = nMillisCheck;
  nLastPing = nMillisCheck;
  Keyboard.begin();
}

void loop() {  
  /* every 4 minutes, keyboard press F13 as keepalive
  use neopixels to convey a timer. fade each in from 0-20 brightness per minute  */
  nMillisCheck = millis();
  unsigned long lRelative = (nMillisCheck - nLastFull);
  unsigned long lStepRelative = (nMillisCheck - nLastPing);
  // every declared interval, step the next LED up in brightness
  // if we haven't crossed ping interval, exit
  if (lStepRelative < nPingInterval) {
    //Serial.print("exit relative");
    return;
  }

  float nPct = 0;
  
  // first check if overall interval has passed  
  if (lRelative > nFullInterval) {
    // here the overall timer has passed. hit F13, flash all LEDs white, then reset them all off
    oStrip.fill(0xFFFFFF);
    oStrip.show();
    Keyboard.press(KEY_F13);
    delay(100);
    Keyboard.releaseAll();
    delay(100);
    oStrip.clear();
    oStrip.show();
    nLastFull = nMillisCheck;
    //Serial.print("exit full");
  } else if (lRelative >= long(nFullInterval * 0.75))  {
    // other intervals are based from the overall.
    // step bulb 3
    // use step interval to calculate percentage distance from edge and use that to set brightness of fill color RGB
    nPct = getPct((lRelative - long(nFullInterval * 0.75)), (long(nFullInterval) - long(nFullInterval * 0.75)));
    oStrip.setPixelColor(3, int16_t(nColorRed * nPct), int16_t(nColorGreen * nPct), int16_t(nColorBlue * nPct));
    //Serial.print("exit 75+");
  } else if (lRelative >= long(nFullInterval * 0.5))  {
    // step bulb 2
    nPct = getPct((lRelative - long(nFullInterval * 0.5)), (long(nFullInterval) - long(nFullInterval * 0.5)));
    oStrip.setPixelColor(2, int16_t(nColorRed * nPct), int16_t(nColorGreen * nPct), int16_t(nColorBlue * nPct));
    //Serial.print("exit 50+");
  } else if (lRelative >= long(nFullInterval * 0.25))  {
    // step bulb 1
    nPct = getPct((lRelative - long(nFullInterval * 0.25)), (long(nFullInterval) - long(nFullInterval * 0.25)));
    oStrip.setPixelColor(1, int16_t(nColorRed * nPct), int16_t(nColorGreen * nPct), int16_t(nColorBlue * nPct));
    //Serial.print("exit 25+");
  } else {
    // step bulb 0
    nPct = getPct(lRelative, long(nFullInterval));
    oStrip.setPixelColor(0, int16_t(nColorRed * nPct), int16_t(nColorGreen * nPct), int16_t(nColorBlue * nPct));
    //Serial.print("exit 0+");
  }
  oStrip.show();
  nLastPing = nMillisCheck;
}

float getPct(long nCurrent, long nMax) {
  if (nMax == 0 || nCurrent == 0) return 0.00;
  float result = (float)nCurrent / (float)nMax;
  
  // Constrain result to 0.00 - 1.00 range
  if (result > 1.00) result = 1.00;
  if (result < 0.00) result = 0.00;

  // Rounding to 2 decimal places
  return round(result * 100.00) / 100.00;
}
