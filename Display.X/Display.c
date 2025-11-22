/*
 * File:   Display.c
 * Author: ejoji
 */

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#include <xc.h>
#include "pic16F877AConfig.h"

#define _XTAL_FREQ 20000000
#define DEBOUNCE_MS 20


const unsigned char DISPLAY[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

// ------------------------------------------------------
//   ANTIRREBOTE RB3
// ------------------------------------------------------
unsigned char debounceRB3(void){
    if(PORTBbits.RB3 == 0){         // botón presionado
        __delay_ms(DEBOUNCE_MS);
        if(PORTBbits.RB3 == 0){
            return 1;
        }
    }
    return 0;
}

// ------------------------------------------------------
//   FUNCIÓN PARA MOSTRAR EL NÚMERO EN RA + RE
// ------------------------------------------------------
void showNumber(unsigned char num){

    PORTA = DISPLAY[num] & 0x6D;          
    PORTE = (DISPLAY[num] >> 6) & 0x06;   
}

void main(void){

    ADCON1 = 0x07;
    CMCON = 0x07;

    TRISB = 0b00001000; // RB3 entrada
    TRISA = 0x00;       // RA salidas
    TRISE = 0x00;       // RE salidas

    OPTION_REGbits.nRBPU = 1; 

    PORTA = 0x00;
    PORTE = 0x00;

    unsigned char counter = 0;
    unsigned char prev = 0;

    showNumber(0);

    while(1){

        unsigned char pressed = debounceRB3();

        if(pressed == 1 && prev == 0){
            // incrementar número
            counter++;
            if(counter == 10){
                counter = 0;
            }

            showNumber(counter);
        }

        prev = pressed;
    }

    return;
}

