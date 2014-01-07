/**************************************
Damien MONNI - 07/01/2014
www.damien-monni.fr

External Interrupt test on pin 43 (PD0, INT0) - Arduino pin 21
**************************************/

#include <avr/io.h> 
#include <avr/interrupt.h>

int main(void){

	EICRA |= 1<<ISC00 //Any logical change on INT0 generates an interrupt request
	EIMSK |= 1<<INT0 //Enable interrupt INT0
	
	sei();
	
	DDRB |= 1<<DDB7;
	
	while(1);
	
	return 0;
}

ISR(INT0_vect){
	PORTB ^= 1<<PORTB7;
}