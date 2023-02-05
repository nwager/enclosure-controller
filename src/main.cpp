#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS    14
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG
#define DATA_PIN    4
#define VOLTS       12
#define MAX_MA      (NUM_LEDS * 60)

CRGBArray<NUM_LEDS> leds;

void setup() {
  delay( 3000 ); //safety startup delay
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_MA);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);

  fill_solid(leds, NUM_LEDS, CRGB::Black);
}

void loop() {
  static uint8_t h = 0;
  fill_rainbow(leds, NUM_LEDS, h++, 5);
  FastLED.show();

  delay(10);
}