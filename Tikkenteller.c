/*
 * Tikkenteller.c
 *
 * Created: 29-3-2020 13:19:31
 *  Author: CCT
 * ATTINY13a at 1.2MHz (default is 9.6MHz/8)
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

//bit number on PB where the output will appear
#define OUTPUT 4 

void delay_ms(uint16_t x);

uint16_t target=0;

int main(void)
{
	DDRB=(1<<3)|(1<<OUTPUT);
	PORTB|=0b11;//pullups
	
	//set up timer/counter0. CLOCK DIVIDE BY 1024. COUNT TO 94. fcpu=1.2MHz. gives 80ms CTC mode
	TCCR0A=(1<<WGM01);
	TCCR0B=(1<<CS00)|(1<<CS02);
	TIMSK0|=(1<<OCIE0A);
	OCR0A=93;
	sei();
	uint8_t buttoncount=0;
	
	while(1){
		if((PINB&0b1)==0){
			buttoncount++;
			if(buttoncount==3)target++;
			if(buttoncount==50)target+=9;
		}else buttoncount=0;
		delay_ms(5);
		
	}
	/*
	uint8_t oldpins=0;
	uint8_t curpins=0;
	uint8_t newpins=0;
	
    while(1){
		//read the input pins, debounce them and perform action
		newpins=PINB&0b11;
		if(newpins==curpins){
			if((~newpins)&oldpins&1)target++; //PB0 does 1 step
			if((~newpins)&oldpins&2)target+=10; //PB1 does 10 steps
			oldpins=newpins;
		}
		curpins=newpins;
		delay_ms(25);
    }
	*/
}

//General short delays
void delay_ms(uint16_t x)
{
	uint8_t y, z;
	for ( ; x > 0 ; x--){
		for ( y = 0 ; y < 130 ; y++){
			for ( z = 0 ; z < 6 ; z++){
				asm volatile ("nop");
			}
		}
	}
}

//interrupt routine
ISR(TIM0_COMPA_vect){
	static uint16_t current=0;
	if(PORTB&(1<<OUTPUT)){
		//output was high, now make it low
		PORTB&=~(1<<OUTPUT);
	}else{
		if(current!=target){
			//output was low, now make it high
			PORTB|=(1<<OUTPUT);
			current++;
		}
	}
}
