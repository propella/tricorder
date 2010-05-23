// World Stethoscope Clone
// Make a sound by sensor input
// for Arduino 018

#include <avr/interrupt.h>
#include <avr/io.h>

// #define F_CPU 16000000UL
#define PRESCALE 32
#define COUNTER_MAX 4
// #define COUNTER_MAX 50 // Set larger than 50 when debug mode.
#define PRECISION 1024
#define FREQ_SIGNAL_MAX (5000L * 2)

#define DEBUG 0

#define PIN_OUTPUT 9
#define PIN_INPUT 1

long SAMPLING_RATE = F_CPU / PRESCALE / COUNTER_MAX;
long margin = SAMPLING_RATE;
volatile long frequency = FREQ_SIGNAL_MAX;
volatile long nextFrequency = FREQ_SIGNAL_MAX;
char signal = 0;

void setup() {
  pinMode(PIN_OUTPUT, OUTPUT);

  TCCR2A = 1<<WGM21 | 0<<WGM20; // Clear Timer on Compare Match
  TCCR2B = 0<<WGM22 | 0<<CS22 | 1<<CS21 | 1<<CS20; // clk/32 prescale
  OCR2A = COUNTER_MAX - 1; // Output Compare Register A
  TIMSK2 = 1<<OCIE2A; // Timer/Counter2 Output Compare Match A Interrupt Enable
  TCNT2 = 0; // Reset Timer/Counter2

  if (DEBUG) { Serial.begin(9600); }
}

ISR(TIMER2_COMPA_vect) { // Timer/Counter2 Overflow

  margin -= frequency;
  if (margin <= 0) {
    margin += SAMPLING_RATE;
    digitalWrite(PIN_OUTPUT, signal); // Output signal
    signal ^= 1;
    frequency = nextFrequency;
  }
}

void loop() {
  int inputValue = analogRead(PIN_INPUT) + 1; // 1 ... 1024
  long freq = inputValue * FREQ_SIGNAL_MAX / PRECISION;
  nextFrequency = freq;

  if (DEBUG) {
    delay(10);
    Serial.print("value: ");
    Serial.print(inputValue);
    Serial.print(" frequency: ");
    Serial.print(frequency);
    Serial.print(" margin: ");
    Serial.println(margin);
  }
}
