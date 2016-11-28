/*
 * Lab7PartB.c
 * Name: Robert Olsthoorn
 * TA: Daniel
 * Section #: 8705
 * Description: This program uses the DAC and the timer control system to output a sine wave
				at 250 Hz from 0-2.5V on PortA Pin 2.
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 2000000
#define frequency 250

//Function declarations
void init_DAC(float voltage);
void init_timer();

//TAKES: Voltage (must be between 0 and 2.5V)
//Sets the DAC to output a constant voltage on Port A Pin 2
void init_DAC(float voltage)
{
	PORTA_DIRSET = 0x04;
	PORTB_DIRCLR = 0x01;
	
	//Enable the DAC
	DACA_CTRLA = 0x05; //Enabling channel 0 and output overall
	DACA_CTRLB = 0x00;
	DACA_CTRLC = 0x18; //Reference voltage on AREFB (2.5V)
	DACA_CH0DATA = (4095/2.5)*voltage; //Write conversion function (4095/2.5)*1.7 = 2785
}

//Enables the timer on PORT E Channel 1
//Enables low level overflow interrupts
void init_timer(){
	//Duration timer
	TCE1_CTRLA = 0x05; //Prescaler set to 64x for longer duration
	TCE1_CTRLB = 0x11; //Enable CCA and normal waveform
	TCE1_CTRLC = 0x0F; //Compare output value
	TCE1_INTCTRLA = PMIC_LOLVLEN_bm; //Set low level overflow underflow interrupts
	PMIC_CTRL = PMIC_LOLVLEN_bm;	//Set low-level enable
	sei();		//Global interrupt enable
	TCE1_CCA = (F_CPU/1000/frequency/5)-1;	//To do (properly define function for time) Divide by prescaler
}

//Declare a global timer for the interrupt system
int timer = 0;

//MAIN FUNCTION
//Declares a sinewave and then iterates over the values waiting to move until the timer value has been met
int main(void)
{
	int sineWave[] = {	0x800,0x8c8,0x98f,0xa52,0xb0f,0xbc5,0xc71,0xd12,
						0xda7,0xe2e,0xea6,0xf0d,0xf63,0xfa7,0xfd8,0xff5,
						0xfff,0xff5,0xfd8,0xfa7,0xf63,0xf0d,0xea6,0xe2e,
						0xda7,0xd12,0xc71,0xbc5,0xb0f,0xa52,0x98f,0x8c8,
						0x800,0x737,0x670,0x5ad,0x4f0,0x43a,0x38e,0x2ed,
						0x258,0x1d1,0x159,0xf2,0x9c,0x58,0x27,0xa,
						0x0,0xa,0x27,0x58,0x9c,0xf2,0x159,0x1d1,
						0x258,0x2ed,0x38e,0x43a,0x4f0,0x5ad,0x670,0x737};
						
	init_timer();
	init_DAC(1.7);

    while (1) 
    {
			//Iterate over each value of the sinewave
			for(int i=0; i<(sizeof sineWave/ sizeof *sineWave); i++){				
				//Wait to overflow and then move on from counter
				TCE1_CTRLFSET = 0b00001000;	//Force restart of counter		
				while(timer == 0){
					DACA_CH0DATA = sineWave[i];
				}
				timer = 0;				
			}
			timer = 0;	
    }
}

//ISR for overflow interrupt buffer
ISR(TCE1_OVF_vect){
	timer = 1;	//Change timer value to break out of loop and move forward
}