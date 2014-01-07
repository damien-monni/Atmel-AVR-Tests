/**************************************
Damien MONNI - 06/01/2014 (last update : 07/01/2014)
www.damien-monni.fr

Make brushless motors on pin 43, 44, 45, 46 to run at the initial speed (0 tr/min) on an ATmega2560.
Initial speeds in microsecond should be enter in the servo[] table.
**************************************/

#include <avr/io.h> 
#include <avr/interrupt.h>

//Convert microsecond to tick for a 16MHz clock with a prescaler of 8
uint16_t usToTicks(uint16_t us);

volatile unsigned int servo[4] = {700, 700, 700, 700}; //Initial speed - 700 to 2000 for ESC Turnigy Plush
volatile int8_t channel = 0; //Controlled motor number : 0, 1, 2 or 3

volatile uint16_t previousTime = 0, time = 0; //Time from 70(1.1ms) to 125(2ms) on 8 bits timer
volatile uint8_t isHigh = 0;

volatile uint16_t timeS = 0;

int main(void){

	EICRA |= 1<<ISC00 | 1<<ISC01; //The rising edge of INT0 generates asynchronously an interrupt request
	EIMSK |= 1<<INT0; //Enable interrupt INT0

	TCCR1B |= 1<<CS11; //Prescaler of 8
	TIMSK1 |= (1<<OCIE1A); //Interrupt on OCR1A
	OCR1A = usToTicks(servo[0]); //Set the first interrupt to occur when the first pulse was ended
	
	DDRL |= 1<<DDL0 | 1<<DDL1 | 1<<DDL2 | 1<<DDL3; //Ports 49, 48, 47, 46 on Arduino Mega 2560 set as OUTPUT
	PORTL = 1<<channel; //Set first servo pin high

        DDRB |= 1<<DDB7;
	
	sei(); //Enable global interrupts
	
	while(1){
		if(timeS > 50*8){
			servo[0] = (time/25)/8; //Not working. We have to find what is time value
		}
	}

	return 0;
}

//PMW Building ISR
ISR(TIMER1_COMPA_vect)
{
	if(channel < 0){ //Every motors was pulsed, waiting for the next period
		//TODO : try to use TCNT1 >= usToTicks(20000) instead of TCNT1 >= 40000
		if(TCNT1 >= 40000){ //50Hz with a prescaler of 8 at 16MHz
			TCNT1 = 0;
			channel = 0;
			PORTL |= 1<<channel;
			OCR1A = usToTicks(servo[0]);
			timeS++;
		}
		else{
			//TODO : try to use OCR1A = usToTicks(20000) instead of OCR1A = 40000
			OCR1A = 40000;
		}
	}
	else{
		if(channel < 3){ //Last servo pin just goes high
			OCR1A = TCNT1 + usToTicks(servo[channel + 1]);
			PORTL &= ~(1<<channel); //Clear actual motor pin
			PORTL |= 1<<(channel + 1); //Set the next one
			channel++;
		}
		else{
			PORTL &= ~(1<<channel); //Clear the last motor pin
			OCR1A = TCNT1 + 4; //Call again the interrupt just after that
			channel = -1; //Wait for the next period
		}
	}
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
		if(TCNT1 > previousTime){
			time = TCNT1 - previousTime;
		}
		else{
			time = (40000 - previousTime) + TCNT1;
		}
		EIMSK &= ~(1<<INT0);
		EICRA |= 1<<ISC00; //Raising edge
		EIMSK |= 1<<INT0;
		isHigh = 0;
	}
}

//Convert microsecond to tick for a 16MHz clock with a prescaler of 8
uint16_t usToTicks(uint16_t us){
	return (16 * us) / (float)8; // 16 = tick per microsecond (16MHz/1000000) ; 8 = prescaler
}