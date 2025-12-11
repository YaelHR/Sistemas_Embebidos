#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

volatile long encoderCount = 0;
volatile int8_t direction = 0;

long targetPosition = 0;
const long TOLERANCE = 5;
const uint8_t Kp = 3;
const uint8_t PWM_MAX = 255;

const uint8_t POT_PIN = 0;

void stopMotor(void);
void motorForward(void);
void motorReverse(void);
void setPWM(uint8_t duty);
long readEncoder(void);
void checkPotentiometer(void);

int main(void)
{
  cli();
  
  // Encoder setup
  DDRD &= ~((1 << DDD2) | (1 << DDD3));
  PORTD |= (1 << PORTD2) | (1 << PORTD3);
  
  // Motor L293D
  DDRD |= (1 << DDD7);
  DDRB |= (1 << DDB0) | (1 << DDB1);
  
  stopMotor();
  setPWM(0);
  
  // ADC potentiometer
  ADMUX = (1 << REFS0);
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  
  // INT0 encoder
  EICRA |= (1 << ISC01) | (1 << ISC00);
  EIMSK |= (1 << INT0);
  EIFR |= (1 << INTF0);
  
  // Timer1 PWM D9
  TCCR1A = 0; TCCR1B = 0;
  TCCR1A |= (1 << WGM10);
  TCCR1B |= (1 << WGM12);
  TCCR1A |= (1 << COM1A1);
  TCCR1B |= (1 << CS11);
  OCR1A = 0;
  
  sei();
  _delay_ms(50);
  
  while(1)
  {
    checkPotentiometer();
    
    long position = readEncoder();
    long error = targetPosition - position;
    long absError = (error >= 0) ? error : -error;
    
    if (absError <= TOLERANCE)
    {
      stopMotor();
      setPWM(0);
    }
    else
    {
      if (error > 0) motorForward();
      else motorReverse();
      
      uint32_t pwm = (uint32_t)absError * Kp;
      if (pwm > PWM_MAX) pwm = PWM_MAX;
      setPWM((uint8_t)pwm);
    }
    
    _delay_ms(20);
  }
}

void checkPotentiometer(void)
{
  static uint16_t lastReading = 512;
  
  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC));
  uint16_t potValue = ADC;
  
  int16_t change = (int16_t)potValue - (int16_t)lastReading;
  
  if (change > 10 || change < -10)
  {
    targetPosition = (long)(potValue * 800L / 1023L);
    if (targetPosition < 50) targetPosition = 0;
    else if (targetPosition > 750) targetPosition = 800;
  }
  
  lastReading = potValue;
}

ISR(INT0_vect)
{
  if (PIND & (1 << PIND3))
  {
    encoderCount++;
    direction = 1;
  }
  else
  {
    encoderCount--;
    direction = -1;
  }
}

void stopMotor(void)
{
  PORTD &= ~(1 << PORTD7);
  PORTB &= ~(1 << PORTB0);
}

void motorForward(void)
{
  PORTD |= (1 << PORTD7);
  PORTB &= ~(1 << PORTB0);
}

void motorReverse(void)
{
  PORTD &= ~(1 << PORTD7);
  PORTB |= (1 << PORTB0);
}

void setPWM(uint8_t duty)
{
  OCR1A = duty;
}

long readEncoder(void)
{
  long value;
  uint8_t sreg = SREG;
  cli();
  value = encoderCount;
  SREG = sreg;
  return value;
}