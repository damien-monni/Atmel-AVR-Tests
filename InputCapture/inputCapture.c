/**************************************
Damien MONNI - 07/01/2014 (last update : 21/01/2014)
www.damien-monni.fr

External Interrupt test on PIN 43 (PD0, INT0) - Arduino PIN 21
Tested with a 16MHz clock and the HobbyKing RC
You can play with "ledOnTime" to see how your RC is set
**************************************/

#include <avr/io.h> 
#include <avr/interrupt.h>

uint16_t ledOnTime = 20000;

volatile uint16_t previousTime = 0, time = 0; //Use to calculate the time of pulses
volatile uint8_t isHigh = 0; //Use to know if we were on a pulse or not

int main(void){

	EICRA |= 1<<ISC00 | 1<<ISC01; //The rising edge of INT0 generates asynchronously an interrupt request
	EIMSK |= 1<<INT0; //Enable interrupt INT0
	
	TCCR1B |= 1<<CS11; //Prescaler of 8 on 16bits Timer1
	
	sei(); //enable global interrupts
	
	DDRB |= 1<<DDB7; //Set the arduino led as Input
	
	while(1){
		if(time < ledOnTime){ //Between 17850 and 32000 or 2230 and 4000 with the prescaler of 8.
			PORTB |= 1<<PORTB7; //Turn on the LED
		}
		else{
			PORTB &= ~(1<<PORTB7); //Turn off the LED
		}
	}
	
	return 0;
}

//New edge detected on PIN 43, Arduino PIN 21
ISR(INT0_vect){
	//Raising edge, just remember the timer value
	if(isHigh == 0){
		previousTime = TCNT1;
		EIMSK &= ~(1<<INT0);
		EICRA &= ~(1<<ISC00); //Setting the next interrupt to occur on a falling edge
		EIMSK |= 1<<INT0;
		isHigh = 1;
	}
	//Falling edge, calculate the timespan checking if timer value has overlap the 16 bits
	else{
		if(TCNT0 > previousTime){
			time = TCNT1 - previousTime;
		}
		else{
			time = (65536 - previousTime) + TCNT1;
		}
		EIMSK &= ~(1<<INT0);
		EICRA |= 1<<ISC00; //Setting the next interrupt to occur on a raising edge
		EIMSK |= 1<<INT0;
		isHigh = 0;
	}
}