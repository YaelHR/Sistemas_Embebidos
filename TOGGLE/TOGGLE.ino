#define F_CPU 1200000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void){
	DDRD = 0b11110000; // Puerto D
  PORTD = 0b00001111; // Pull Up Activo en las Entradas
	_delay_ms(10);
	uint8_t PrimerE = 1;
	uint8_t estLED = 0;
	
	while (1){
		
		uint8_t ActualE = (PIND & 0b00000001); // Lee el estado actual del botón
		
		// (presión del botón)
		if (PrimerE && !ActualE) {
			estLED = !estLED; // Cambia el estado del LED
			
			if (estLED) {
				PORTD |= 0b00010000; // Enciende el bit 4
			} else {
				PORTD &= 0b11101111; // Apaga el bit 4
			}
		}
		
		PrimerE = ActualE; // Actualiza el estado anterior
		_delay_ms(50); // Retardo para antirrebote
	}
}