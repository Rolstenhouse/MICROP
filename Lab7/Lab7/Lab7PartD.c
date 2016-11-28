/*
 * Lab7PartD.c
 * Name: Robert Olsthoorn
 * TA: Daniel
 * Section #: 8705
 * Description: This program utilizes the XMEGA's digital to analog converter in order
				to generate a sine wave set at a specified frequency from the keypad. The program
				uses the DMA to avoid making calls to the CPU in order to ensure precision of the waveform.
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ebi.h"
#include "keypad.h"
#include "lcd.h"

#define F_CPU 2000000

//Function declarations
void init_DAC();
void init_timer(float frequency);
void init_DMA(int16_t wave[]);

////FUNCTIONS////

//Sets up the XMEGA onboard DAC to run on portA pin 2
//Sets reference voltage to 2.5V
void init_DAC()
{		
		PORTA_DIRSET = 0x04;
		PORTB_DIRCLR = 0x01;
		
		//Enable the DAC
		DACA_CTRLA = 0x05; //Enabling channel 0 and output overall
		DACA_CTRLB = 0x00;
		DACA_CTRLC = 0x18; //Reference voltage on AREFB (2.5V)
}

//TAKES: Inital frequency for initialization
//Enables the timer on PORT C Channel 0
//Enables low level overflow interrupts
void init_timer(float frequency)
{
		TCC0_CTRLA = 0x01;
		TCC0_CTRLB = 0x11; //Enable CCA and normal waveform
		TCC0_CTRLC = 0x0F; //Compare output value
		TCC0_CCA = F_CPU/frequency/64;		//To do (properly define function for time) Divide by prescaler
		TCC0_INTCTRLA = PMIC_LOLVLEN_bm; //Set low level overflow underflow interrupts
		PMIC_CTRL = PMIC_LOLVLEN_bm;	//Set low-level enable
		sei();		//Global interrupt enable
}

//TAKES: source address
//Initiales the DMA on Channel 0 to source data from an array and output to the DAC channel
//Trigger source set to Overflow from timer control
//Sets transfer count equal to the total number of bytes needed to say a transfer has been completed
//Sets repeat mode to repeat forever
void init_DMA(int16_t wave[] )
{
	DMA_CTRL |= DMA_ENABLE_bm;
	
	DMA_CH0_REPCNT = 0;
	DMA_CH0_CTRLA = DMA_CH_BURSTLEN_2BYTE_gc | DMA_CH_SINGLE_bm | DMA_CH_REPEAT_bm; //Single shot, 2 bytes, repeat on
	
	DMA_CH0_ADDRCTRL = DMA_CH_SRCRELOAD0_bm | DMA_CH_SRCDIR_INC_gc | DMA_CH_DESTRELOAD_BURST_gc | DMA_CH_DESTDIR_INC_gc;
	DMA_CH0_TRIGSRC = DMA_CH_TRIGSRC_TCC0_OVF_gc;
	
	DMA_CH0_TRFCNT = 128; //Number of bytes until transfer completed (64*2)
	
	DMA_CH0_SRCADDR0 = ((uint16_t)(&wave[0]) >> 0) & 0xFF;
	DMA_CH0_SRCADDR1 = ((uint16_t)(&wave[0]) >> 8) & 0xFF;
	
	DMA_CH0_DESTADDR0 = ((uint16_t) (&DACA_CH0DATA) >> 0) & 0xFF;
	DMA_CH0_DESTADDR1 = ((uint16_t) (&DACA_CH0DATA) >> 8) & 0xFF;

	DMA_CH0_CTRLA |= DMA_CH_ENABLE_bm;
}


////MAIN PROGRAM////
//DESCRIPTION:	Continuously scans the keypad and updates the timer control and the DMA appropriately.
int main(void)
{
	float frequency = 250;
	float baseFrequency = 50;
	
	//Sine wave 64 point declaration	
	uint16_t sineWave[] = {	0x800,0x8c8,0x98f,0xa52,0xb0f,0xbc5,0xc71,0xd12,
						0xda7,0xe2e,0xea6,0xf0d,0xf63,0xfa7,0xfd8,0xff5,
						0xfff,0xff5,0xfd8,0xfa7,0xf63,0xf0d,0xea6,0xe2e,
						0xda7,0xd12,0xc71,0xbc5,0xb0f,0xa52,0x98f,0x8c8,
						0x800,0x737,0x670,0x5ad,0x4f0,0x43a,0x38e,0x2ed,
						0x258,0x1d1,0x159,0xf2,0x9c,0x58,0x27,0xa,
						0x0,0xa,0x27,0x58,0x9c,0xf2,0x159,0x1d1,
						0x258,0x2ed,0x38e,0x43a,0x4f0,0x5ad,0x670,0x737};
	//Triangle wave 64 point declaration					
	uint16_t triangleWave[] = {	0x80,0x100,0x180,0x200,0x280,0x300,0x380,0x400,
								0x480,0x500,0x580,0x600,0x680,0x700,0x780,0x800,
								0x87f,0x8ff,0x97f,0x9ff,0xa7f,0xaff,0xb7f,0xbff,
								0xc7f,0xcff,0xd7f,0xdff,0xe7f,0xeff,0xf7f,0xfff,
								0xf7f,0xeff,0xe7f,0xdff,0xd7f,0xcff,0xc7f,0xbff,
								0xb7f,0xaff,0xa7f,0x9ff,0x97f,0x8ff,0x87f,0x800,
								0x780,0x700,0x680,0x600,0x580,0x500,0x480,0x400,
								0x380,0x300,0x280,0x200,0x180,0x100,0x80,0x0};
	//Initialization of all relevant peripherals
	init_DAC();
	init_timer(frequency);
	init_DMA(sineWave);
    init_keypad();
	init_LCD();
	
    while (1) 
    {	
		char read = keypad_read();
		int intRead = (int) (read - '0');	//Setting the int so that '0' ==> 0, etc.
		
		if(read == '*'){
			DMA_CH0_SRCADDR0 = ((uint16_t)(&triangleWave[0]) >> 0) & 0xFF;	//Properly adjust src address
			DMA_CH0_SRCADDR1 = ((uint16_t)(&triangleWave[0]) >> 8) & 0xFF;
		}
		else if (read == '#'){
			DMA_CH0_SRCADDR0 = ((uint16_t)(&sineWave[0]) >> 0) & 0xFF;	//Properly adjust src address
			DMA_CH0_SRCADDR1 = ((uint16_t)(&sineWave[0]) >> 8) & 0xFF;
		}
		else if (read == '0'){
			DACA_CTRLA = 0x00; //Disabling output (i.e. setting src to be 0)
		}
		else if ((read > '0') && (read<='9')){
			DACA_CTRLA = 0x05; //Enabling channel 0 and output overall
			TCC0_CCA = F_CPU/(baseFrequency*intRead)/64;	//Setting frequency based on char input
		}
		else if ((read >='A') && (read <='D')){
			DACA_CTRLA = 0x05; //Enabling channel 0 and output overall
			TCC0_CCA = F_CPU/(baseFrequency*(intRead - 7))/64;	//Setting frequency offset to get 'A' == 10, 'B' == 11 etc.
		}
    }
}

//Setting ISR to clear the Overflow flags after the DMA has already been triggered
ISR(TCC0_OVF_vect){
	TCC0_INTFLAGS = 0x01;	
}