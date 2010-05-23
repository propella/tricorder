// World Stethoscope Clone for ATtiny45

#include <avr/interrupt.h>
#include <avr/io.h>

//#define PRESCALE 64
//#define COUNTER_MAX 4
#define PRESCALE 8
#define COUNTER_MAX 32
#define PRECISION 1024
#define FREQ_SIGNAL_MAX (5000L * 2)

#define PIN_OUTPUT PORTB3
#define PIN_INPUT PORTB4

const long SAMPLING_RATE = F_CPU / PRESCALE / COUNTER_MAX;
long margin = F_CPU / PRESCALE / COUNTER_MAX;
volatile long frequency = FREQ_SIGNAL_MAX;
volatile long nextFrequency = FREQ_SIGNAL_MAX;

ISR(TIM0_COMPA_vect) { /* Timer/Counter0 Compare */
PORTB |= 1<<PORTB0;
  margin -= frequency;
  if (margin <= 0) {
    margin += SAMPLING_RATE;
    PORTB ^= 1 << PIN_OUTPUT; /* Toggle the output */
    frequency = nextFrequency;
  }
}

int analogRead()
{
  uint8_t low, high;
  ADMUX |= 0b0010; /* Select ADC2(PB4) */
  ADCSRA |= 1<<ADSC; /* ADC Start Convention */
  while (bit_is_set(ADCSRA, ADSC));
  low = ADCL;
  high = ADCH;
  return (high << 8) | low;
}

int main(void)
{
  /* Initialize */
  DDRB |=   1<<PORTB0; /* Debug */

  DDRB |=   1<<PIN_OUTPUT; /* Make PIN_OUTPUT as output */
  DDRB &= ~(1<<PIN_INPUT); /* Make PIN_INPUT as input */
  ADCSRA |= 1<<ADEN; /* ADC Enable */

  TCCR0A = 1<<WGM01 | 0<<WGM00; // Clear Timer on Compare Match
  TCCR0B = 0<<WGM02 | 0<<CS02 | 1<<CS01 | 0<<CS00; // clk/8 prescale
  //  TCCR0B = 0<<WGM02 | 0<<CS02 | 1<<CS01 | 1<<CS00; // clk/64 prescale
  OCR0A = COUNTER_MAX - 1; // Output Compare Register A
  TIMSK = 1<<OCIE0A; // Timer/Counter0 Output Compare Match A Interrupt Enable
  TCNT0 = 0; // Reset Timer/Counter0
  sei();

  while (1) {
    int inputValue = analogRead(PIN_INPUT) + 1; // 1 ... 1024
    long freq = inputValue * FREQ_SIGNAL_MAX / PRECISION;
    nextFrequency = freq;
  }
  return 0;   /* never reached */
}
