/**************************************
Damien MONNI - 06/01/2014 (last update : 21/01/2014)
www.damien-monni.fr

Make brushless motors on pin 46, 47, 48, 49 to run at the initial speed (0 tr/min - 700us) on an ATmega2560.
Initial speeds in microsecond should be enter in the servo[] table.
This works for a 16MHz clock source
**************************************/

#include <avr/io.h> 
#include <avr/interrupt.h>

//Convert microsecond to tick for a 16MHz clock source with a prescaler of 8
uint32_t usToTicks(uint32_t us);

volatile unsigned int servo[4] = {700, 700, 700, 700}; //Initial speed - 700 to 2000 for ESC Turnigy Plush
volatile int8_t channel = 0; //Controlled motor number : 0, 1, 2 or 3

int main(void){

	TCCR1B |= 1<<CS11; //Prescaler of 8
	TIMSK1 |= (1<<OCIE1A); //Interrupt on OCR1A
	OCR1A = usToTicks(servo[0]); //Set the first interrupt to occur when the first pulse was ended
	
	DDRL |= 1<<DDL0 | 1<<DDL1 | 1<<DDL2 | 1<<DDL3; //Ports 49, 48, 47, 46 on Arduino Mega 2560 set as OUTPUT
	PORTL = 1<<channel; //Set first servo pin high
	
	sei(); //Enable global interrupts
	
	while(1);

	return 0;
}

//PMW Building ISR
ISR(TIMER1_COMPA_vect)
{
	if(channel < 0){ //Every motors was pulsed, waiting for the next period
		if(TCNT1 >= usToTicks(20000)){ //50Hz with a prescaler of 8 at 16MHz
			TCNT1 = 0;
			channel = 0;
			PORTL |= 1<<channel;
			OCR1A = usToTicks(servo[0]);
		}
		else{
			OCR1A = usToTicks(20000);
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
			channel = -1; //Wait for the next 20ms (50Hz) period
		}
	}
}

//Convert microsecond to tick for a 16MHz clock with a prescaler of 8
uint32_t usToTicks(uint32_t us){
	return (16.0 * us) / (float)8; // 16 = tick per microsecond (16MHz/1000000) ; 8 = prescaler
}