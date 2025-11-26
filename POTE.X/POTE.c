/*
 * File:   POTE.c
 * Author: ejoji
 *
 * Created on 24 de noviembre de 2025, 03:19 PM
 */

#include <xc.h>
#include <stdio.h>     
#define _XTAL_FREQ 20000000
#include "pic16F877A.h"

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

//lcd
#define RS RB6
#define EN RB7
#define LCD_PORT PORTD


void lcd_cmd(unsigned char);
void lcd_write(unsigned char);
void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(unsigned char, unsigned char);
void lcd_string(const char*);
void adc_init(void);
unsigned int adc_read_an0(void);


void adc_init(void){
    TRISA = 0b00000001;    // AN0 como entrada
    ADCON1 = 0b11001110;   // AN0 anal?gico, Vref = VDD
    ADCON0 = 0b10000001;   // Canal AN0, ADC encendido
    __delay_us(20);
}


unsigned int adc_read_an0(void){
    __delay_us(20);
    GO_nDONE = 1;
    while(GO_nDONE);
    return ((ADRESH << 8) | ADRESL);  // 10 bits
}


// FUNCIONES LCD

void lcd_cmd(unsigned char cmd){
    RS = 0;
    LCD_PORT = cmd;
    EN = 1;
    __delay_ms(2);
    EN = 0;
}

void lcd_write(unsigned char data){
    RS = 1;
    LCD_PORT = data;
    EN = 1;
    __delay_ms(2);
    EN = 0;
}

void lcd_init(void){
    TRISD = 0x00;  // Puerto D salida
    TRISB6 = 0;    // RS salida
    TRISB7 = 0;    // EN salida

    __delay_ms(15);
    lcd_cmd(0x38); // LCD 8 bits, 2 l?neas
    lcd_cmd(0x0C); // Display ON, cursor OFF
    lcd_cmd(0x06); // Auto incremento
    lcd_cmd(0x01); // Clear
    __delay_ms(2);
}

void lcd_clear(void){
    lcd_cmd(0x01);
}

void lcd_set_cursor(unsigned char row, unsigned char col){
    if(row == 1)
        lcd_cmd(0x80 + (col - 1));
    else
        lcd_cmd(0xC0 + (col - 1));
}

void lcd_string(const char *str){
    while(*str){
        lcd_write(*str++);
    }
}


void main(void){
    unsigned int raw, mv;
    char buffer[16];

    lcd_init();
    adc_init();
    lcd_clear();

    while(1){
        raw = adc_read_an0();
        mv = ((unsigned long)raw * 5000) / 1023;  // Conversi?n a mV

       //raw
        lcd_set_cursor(1,1);
        sprintf(buffer, "RAW: %4u", raw);
        lcd_string(buffer);

       //mv
        lcd_set_cursor(2,1);
        sprintf(buffer, "mV:  %4u", mv);
        lcd_string(buffer);

        __delay_ms(150);
    }
}
