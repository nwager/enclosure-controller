#include <Arduino.h>
#include <FastLED.h>
#include <Bounce.h>

// FastLED macros
#define NUM_LEDS    14
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG
#define DATA_PIN    4
#define VOLTS       12
#define MAX_MA      (NUM_LEDS * 60)

CRGBArray<NUM_LEDS> leds;

// pwm fan stuff
#define FAN_PWM_PIN 6
#define FAN_PWM_FREQ_HZ 25000

// user input deets
#define HIGH_PIN    2
#define BUTTON_PIN  3
#define FAN_POT_PIN A2
#define LED_POT_PIN A0

#define ADC_MAX      1023
#define POT_PAD_LOW  20
#define POT_PAD_HIGH 20

Bounce button = Bounce(BUTTON_PIN, 100);

enum State {
  S_WHITE,
  S_RED,
  S_GREEN,
  S_BLUE,
  NUM_STATES // must be last in list
};

State state = S_WHITE;

uint16_t fan_control = ADC_MAX;
uint16_t led_control = ADC_MAX;

const float EMA_GAIN = 0.2;

// function prototypes
void update_controls();
uint16_t pad_pot_reading(uint16_t v);
CRGB get_state_color(State s);

void setup() {
  delay(1000); // safety startup delay
  Serial.begin(115200);

  // setup leds
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_MA);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);
  FastLED.clear(true);

  // setup fan
  analogWriteFrequency(FAN_PWM_PIN, FAN_PWM_FREQ_HZ);

  // setup UI
  pinMode(HIGH_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(HIGH_PIN, 1);
}

void loop() {
  update_controls();

  // update leds
  fill_solid(leds, NUM_LEDS, get_state_color(state));
  FastLED.setBrightness(map(led_control, ADC_MAX, 0, 0, 255));
  FastLED.show();

  // update fan
  analogWrite(FAN_PWM_PIN, map(fan_control, ADC_MAX, 0, 0, 255));

  delay(10);
}

void update_controls() {
  fan_control = pad_pot_reading((uint16_t)(
    EMA_GAIN * analogRead(FAN_POT_PIN) + (1.0-EMA_GAIN) * fan_control
  ));
  
  led_control = pad_pot_reading((uint16_t)(
    EMA_GAIN * analogRead(LED_POT_PIN) + (1.0-EMA_GAIN) * led_control
  ));

  if (button.update() && button.fallingEdge()) {
    state = (State)((state + 1) % NUM_STATES);
  }
}

uint16_t pad_pot_reading(uint16_t v) {
  if (v < POT_PAD_LOW) {
    return 0;
  } else if (v > ADC_MAX - POT_PAD_HIGH) {
    return ADC_MAX;
  } else {
    return v;
  }
}

CRGB get_state_color(State s) {
  switch (s) {
  case S_WHITE:
    return CRGB::White;
  case S_RED:
    return CRGB::Red;
  case S_GREEN:
    return CRGB::Green;
  case S_BLUE:
    return CRGB::Blue;
  default:
    return CRGB::Black;
  }
}