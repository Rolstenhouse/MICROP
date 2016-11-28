/*
 * Lab7PartB.c
 * Name: Robert Olsthoorn
 * TA: Daniel
 * Section #: 8705
 * Description: This programs sets up the DAC, DMA, and timer control system in order to write a sine wave onto the DAC
				that varies between 0 and 2.5V at 250 Hz. Repeats indefinitely
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 2000000
#define frequency 250

//Function declarations
void init_DAC(float voltage);
void init_timer();
void init_DMA(uint16_t wave[]);

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

//Enables the timer on PORT C Channel 0
//Enables low level overflow interrupts
//Sets timer control to defined frequency
void init_timer(){
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
void init_DMA(uint16_t wave[]){
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

int main(void)
{
	uint16_t sineWave[] = {	0x800,0x8c8,0x98f,0xa52,0xb0f,0xbc5,0xc71,0xd12,
							0xda7,0xe2e,0xea6,0xf0d,0xf63,0xfa7,0xfd8,0xff5,
							0xfff,0xff5,0xfd8,0xfa7,0xf63,0xf0d,0xea6,0xe2e,
							0xda7,0xd12,0xc71,0xbc5,0xb0f,0xa52,0x98f,0x8c8,
							0x800,0x737,0x670,0x5ad,0x4f0,0x43a,0x38e,0x2ed,
							0x258,0x1d1,0x159,0xf2,0x9c,0x58,0x27,0xa,
							0x0,0xa,0x27,0x58,0x9c,0xf2,0x159,0x1d1,
						0x258,0x2ed,0x38e,0x43a,0x4f0,0x5ad,0x670,0x737};
						
	init_DAC(1.7);
	init_timer();
	init_DMA(sineWave);
	
    while (1) 
    {	
    }
}

//ISR to trigger on overflow in order to trigger the change on the value
ISR(TCC0_OVF_vect){
	TCC0_INTFLAGS = 0x01;	
}