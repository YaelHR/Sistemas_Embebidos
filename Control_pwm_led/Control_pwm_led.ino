#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

void ADC_Init(void) {
    // Seleccionar ADC0 (MUX = 0000)
    ADMUX = (1 << REFS0);      // Referencia AVcc, ADC0

    // Habilitar ADC, prescaler de 128 → 125 kHz
    ADCSRA = (1 << ADEN)  |    // Habilitar ADC
             (1 << ADPS2) |
             (1 << ADPS1) |
             (1 << ADPS0);
}

uint16_t ADC_Read(void) {
    ADCSRA |= (1 << ADSC);         // Iniciar conversión
    while (ADCSRA & (1 << ADSC));  // Esperar a que termine
    return ADC;                    // Regresar valor (0–1023)
}

void PWM_Init(void) {
    // Configurar PD6 (OC0A) como salida
    DDRD |= (1 << PD6);

    // Fast PWM, modo 3: WGM01=1, WGM00=1
    TCCR0A = (1 << WGM01) | (1 << WGM00);

    // Modo no-inverting: COM0A1=1
    TCCR0A |= (1 << COM0A1);

    // Prescaler = 64 → frecuencia PWM ~ 976 Hz
    TCCR0B = (1 << CS01) | (1 << CS00);

    // Duty cycle inicial
    OCR0A = 0;
}

int main(void) {
    ADC_Init();
    PWM_Init();

    uint16_t adcValue;
    uint8_t pwmValue;

    while (1) {
        adcValue = ADC_Read();       // 0–1023

        
        if (adcValue < 5) {
            // Si ADC está por debajo de 10, apagar completamente
            pwmValue = 0;
            TCCR0A &= ~(1 << COM0A1);  // Desconectar PWM de OC0A
            PORTD &= ~(1 << PD6);      // Forzar pin a LOW
        } else {
            // Reconectar PWM si estaba desconectado
            TCCR0A |= (5 << COM0A1);
            // Mapear 10-1023 → 0-255
            pwmValue = (adcValue - 1) / 4;
            OCR0A = pwmValue;
        }

        _delay_ms(10);
    }
}