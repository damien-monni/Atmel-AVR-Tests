/**************************************
Damien MONNI - 07/01/2014
www.damien-monni.fr

External Interrupt test on pin 43 (PD0, INT0) - Arduino pin 21
**************************************/

#include <avr/io.h> 
#include <avr/interrupt.h>

volatile uint16_t previousTime = 0, time = 0; //Time from 70(1.1ms) to 125(2ms) on 8 bits timer
volatile uint8_t isHigh = 0;

int main(void){

	EICRA |= 1<<ISC00 | 1<<ISC01; //The rising edge of INT0 generates asynchronously an interrupt request
	EIMSK |= 1<<INT0; //Enable interrupt INT0
	
	TCCR1B |= 1<<CS11; //Prescaler of 8
	
	sei();
	
	DDRB |= 1<<DDB7;
	
	while(1){
		if(time < 20000){ //Between 17850/8 and 32000/8 = 2230 and  4000
			PORTB |= 1<<PORTB7;
		}
		else{
			PORTB &= ~(1<<PORTB7);
		}
	}
	
	return 0;
}

ISR(INT0_vect){
	if(isHigh == 0){
		previousTime = TCNT1;
		EIMSK &= ~(1<<INT0);
		EICRA &= ~(1<<ISC00); //Falling edge
		EIMSK |= 1<<INT0;
		isHigh = 1;
	}
	else{
		if(TCNT0 > previousTime){
			time = TCNT1 - previousTime;
		}
		else{
			time = (65536 - previousTime) + TCNT1;
		}
		EIMSK &= ~(1<<INT0);
		EICRA |= 1<<ISC00; //Falling edge
		EIMSK |= 1<<INT0;
		isHigh = 0;
	}
}