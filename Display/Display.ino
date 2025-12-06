#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

const uint8_t SEGMENTOS[10] = {
    0b10000000, 0b11110010, 0b01001000,
    0b01100000, 0b00110010, 0b00100100,
    0b00000100, 0b11110000, 0b00000000,
    0b00100000,
};

void showNumber(uint8_t num) {
    PORTD = SEGMENTOS[num];
}

int main(void){
    DDRD = 0b11111110;  // D7-D1 salidas, D0 entrada
    PORTD = 0b00000001; // Pull-up en D0
    
    uint8_t contador = 0;
    uint8_t estadoAnterior = 1;
    
    showNumber(0);  // Mostrar 0 inicial
    
    while(1){
        uint8_t estadoActual = (PIND & 0b00000001);
        
        // Detectar flanco descendente
        if(estadoAnterior && !estadoActual){
            contador++;
            if(contador == 10){
                contador = 0;
            }
            showNumber(contador);
        }
        
        estadoAnterior = estadoActual;
        _delay_ms(50); // Antirrebote
    }
    
    return 0;
}