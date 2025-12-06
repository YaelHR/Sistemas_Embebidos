#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

// Pines del LCD
#define RS PB0  // Pin 8
#define EN PB1  // Pin 9

// Declaración de funciones
void lcd_cmd(unsigned char cmd);
void lcd_write(unsigned char data);
void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(unsigned char row, unsigned char col);
void lcd_string(const char* str);
void adc_init(void);
unsigned int adc_read(unsigned char channel);

// Inicializar el ADC
void adc_init(void){
    DDRC &= ~(1 << PC0);  // A0 como entrada
    ADMUX = (1 << REFS0);  // Vref = 5V
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // Encender ADC con prescaler 128
    _delay_us(20);
}

// Leer el valor del ADC
unsigned int adc_read(unsigned char channel){
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);  // Seleccionar canal
    _delay_us(10);
    ADCSRA |= (1 << ADSC);  // Iniciar conversión
    while(ADCSRA & (1 << ADSC));  // Esperar a que termine
    return ADC;  // Retornar valor de 0 a 1023
}

// Enviar medio byte al LCD (modo 4 bits)
void lcd_send_nibble(unsigned char nibble, unsigned char rs){
    PORTD &= 0x0F;  // Limpiar bits superiores
    PORTD |= (nibble & 0xF0);  // Poner datos en D4-D7
    
    if(rs)
        PORTB |= (1 << RS);  // RS = 1 para datos
    else
        PORTB &= ~(1 << RS);  // RS = 0 para comandos
    
    PORTB |= (1 << EN);  // Pulso EN
    _delay_us(1);
    PORTB &= ~(1 << EN);
    _delay_us(50);
}

// Enviar comando al LCD
void lcd_cmd(unsigned char cmd){
    lcd_send_nibble(cmd & 0xF0, 0);  // Parte alta
    lcd_send_nibble((cmd << 4) & 0xF0, 0);  // Parte baja
    _delay_ms(2);
}

// Escribir un carácter en el LCD
void lcd_write(unsigned char data){
    lcd_send_nibble(data & 0xF0, 1);  // Parte alta
    lcd_send_nibble((data << 4) & 0xF0, 1);  // Parte baja
    _delay_ms(2);
}

// Inicializar el LCD en modo 4 bits
void lcd_init(void){
    DDRD |= 0xF0;  // D4-D7 como salida
    DDRB |= (1 << RS) | (1 << EN);  // RS y EN como salida
    _delay_ms(50);
    
    // Secuencia de inicialización
    lcd_send_nibble(0x30, 0);
    _delay_ms(5);
    lcd_send_nibble(0x30, 0);
    _delay_us(150);
    lcd_send_nibble(0x30, 0);
    lcd_send_nibble(0x20, 0);  // Modo 4 bits
    
    // Configuración del LCD
    lcd_cmd(0x28);  // 4 bits, 2 líneas
    lcd_cmd(0x0C);  // Display ON, cursor OFF
    lcd_cmd(0x06);  // Incremento automático
    lcd_cmd(0x01);  // Limpiar pantalla
    _delay_ms(2);
}

// Limpiar la pantalla
void lcd_clear(void){
    lcd_cmd(0x01);
    _delay_ms(2);
}

// Posicionar el cursor
void lcd_set_cursor(unsigned char row, unsigned char col){
    if(row == 1)
        lcd_cmd(0x80 + (col - 1));  // Primera fila
    else
        lcd_cmd(0xC0 + (col - 1));  // Segunda fila
}

// Escribir una cadena de texto
void lcd_string(const char *str){
    while(*str){
        lcd_write(*str++);
    }
}

// Programa principal
int main(void){
    unsigned int raw;
    unsigned int mv;
    char buffer[16];
    
    lcd_init();
    adc_init();
    lcd_clear();
    
    while(1){
        raw = adc_read(0);  // Leer A0
        mv = ((unsigned long)raw * 5000UL) / 1023UL;  // Convertir a mV
        
        // Mostrar RAW
        lcd_set_cursor(1, 1);
        sprintf(buffer, "RAW: %4u     ", raw);
        lcd_string(buffer);
        
        // Mostrar mV
        lcd_set_cursor(2, 1);
        sprintf(buffer, "mV:  %4u     ", mv);
        lcd_string(buffer);
        
        _delay_ms(150);
    }
    
    return 0;
}