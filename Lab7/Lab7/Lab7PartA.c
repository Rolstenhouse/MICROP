/*
 * Lab7PartA.c
 * Name: Robert Olsthoorn
 * TA: Daniel
 * Section #: 8705
 * Description: This program initializes the DAC and runs it continuously at 1.7V
 */ 

#include <avr/io.h>

#define F_CPU 2000000;

void init_DAC(float voltage);

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

//MAIN FUNCTION 
//DESCRIPTION: Calls initialization for the DAC at 1.7 volts
int main(void)
{
    init_DAC(1.7);
	while (1) 
    {
    }
}

