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

float voltage = 0;

inc_DAC()
{
	voltage = voltage + .25;
	if(voltage<=2.5){
		DACA_CH0DATA = (4095/2.5)*(voltage);
	}
	else{
		voltage = 2.5;
	}
}

dec_DAC(){
	voltage = voltage - .25;
	if(voltage>=0){
		DACA_CH0DATA = (4095/2.5)*(voltage);
	}
	else{
		voltage = 0;
	}
}

void init_USART(){
	PORTD_DIRSET = 0x08;
	PORTD_OUTSET = 0x08;
	PORTD_DIRCLR = 0x04;

	//Initializing portQ bits as outputs (with appropriate values)
	
	PORTQ_DIRSET = 0x0A;
	PORTQ_OUTCLR = 0x0A;
	
	USARTD0_CTRLB = 0x18;	//Emable receiver enable and transmitter enable
	
	//BSEL = 
	//BSCALE = -1;
	
	//Lower 8 of BSEL
	USARTD0_BAUDCTRLA = 0x9D;
	USARTD0_BAUDCTRLB = 0xE1;
	//USARTD0_BAUDCTRLA = 0x21; //19D
	//BSCALE first 4, BSEL, lower 4
	//USARTD0_BAUDCTRLB = 0x91; // 

	//;Writing the equivalent calcluations for baud rate
	//;	BSEL = 0x121	; 289 in hex
	//;	BSCALE = 0b1001 ;-7 since 2's complement
	
	
	USARTD0_CTRLC = 0b00110011; // 00 (asynchronous) | 11 (odd parity) | 0 (stop bit) | 011 (8 bit data size)
	//;Setting the USART for asynchonous mode
	//ldi r16, 0b00100011	; 00 (asynchronous) | 10 (even parity) | 0 (stop bit) | 011 (8 bit data size)
	//sts USARTD0_CTRLC, r16
}

//MAIN FUNCTION 
//DESCRIPTION: Calls initialization for the DAC at 1.7 volts
int main(void)
{
    init_DAC(0);
	init_USART();
	while (1) 
    {
		if((USARTD0_STATUS & 0x80) == 0x80){
			char read = USARTD0_DATA;
			if(read == 'H' || read == 'h'){
				inc_DAC();
			}
			else if(read == 'L' || read == 'l'){
				dec_DAC();
			}
		//Check for transmission 
		}
    }
}

