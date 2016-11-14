/*
* Lab6_PartB.c
* Name:			Robert Olsthoorn
* Section #:	8705
* TA Name:		Daniel
* Description:  This program uses the keypad and the buzzer to play notes for a specified duration.
				It uses the interrupt service routine as well as two separate timer controls for this functionality.
* 
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "keypad.h"
#include "lcd.h"
#include "define.h"

int timer = 0;
void playNoteDuration(float frequency, float duration);
void playNoteDurationSeparated(float frequency, float duration);

//Plays a specified frequency, for a specified amount of time
void playNoteDuration(float frequency, float duration){
	TCE0_CTRLB = 0x11;
	TCE0_CCA = (1/frequency)*F_CPU/2+8;			//Properly set the frequency
	TCE1_CCA = (F_CPU*(duration/1000)/64)-1;	//To do (properly define function for time) Divide by prescaler
	TCE1_CTRLFSET = 0b00001000;					//Force restart of counter
	
	//Loop until the Interrupt routine is called
	while(timer==0);
	timer = 0;
}

//Plays a note for a specified duration and then adds a rest space afterwards
void playNoteDurationSeparated(float frequency, float duration){
	playNoteDuration(frequency, duration);		//Call previous function to play the note for the specified length
	TCE1_CCA = 100;								//Properly define an arbitrary rest unit
	TCE1_CTRLFSET = 0b00001000;					//Force restart of counter
	
	//Loop until the Interrupt route is called
	while(timer==0);
	timer = 0;
}

int main(void)
{	
	PORTE_DIRSET = 0x01;
	//Note timer
	TCE0_CTRLA = 0x01; //Sets prescaler to be on
	TCE0_CTRLB = 0x11; //Enable CCA Compare and capture enable
	TCE0_CTRLC = 0x0F; //Set Compare output value
	TCE0_CTRLD = 0b10100000; //Timer event action (frequency capture)
	TCE0_CTRLE = 0x00; //Set timer in normal count mode
	
	//Duration timer
	TCE1_CTRLA = 0x05; //Prescaler set to 64x for longer duration
	TCE1_CTRLB = 0x11; //Enable CCA and normal waveform
	TCE1_CTRLC = 0x0F; //Compare output value
	TCE1_CTRLD = 0x00; //No special timer events
	TCE1_CTRLE = 0x00;  //Set timer in normal count mode
	TCE1_INTCTRLA = PMIC_LOLVLEN_bm; //Set low level overflow underflow interrupts
	PMIC_CTRL = PMIC_LOLVLEN_bm;	//Set low-level enable
	sei();		//Global interrupt enable
	
	init_keypad();
	init_LCD();
	
	//Loop to continuously poll keypad
    while (1) 
    {	
		char read = keypad_read();
		
		//Code to switch through keypad options
		switch(read){
			case '1':				
				playNoteDuration(c6,d2);
				LCD_Clr();
				LCD_Out_2Str("C6", "1046.50 Hz");
				break;
			case '2':
				playNoteDuration(sc6,d2);
				LCD_Clr();
				LCD_Out_2Str("C#6/Db6", "1108.73 Hz");
				break;
			case '3':
				playNoteDuration(d6, d2);
				LCD_Clr();
				LCD_Out_2Str("D6", "1174.66 Hz");
				break;
			case '4':
				playNoteDuration(sd6, d2);
				LCD_Clr();
				LCD_Out_2Str("D#6/Eb6", "1244.51 Hz");
				break;
			case '5':
				playNoteDuration(e6, d2);
				LCD_Clr();
				LCD_Out_2Str("E6", "1318.51 Hz");
				break;
			case '6':
				playNoteDuration(f6, d2);
				LCD_Clr();
				LCD_Out_2Str("F6", "1396.91 Hz");
				break;
			case '7':
				playNoteDuration(sf6, d2);
				LCD_Clr();
				LCD_Out_2Str("F#6/Gb6", "1479.98 Hz");
				break;
			case '8':
				playNoteDuration(g6, d2);
				LCD_Clr();
				LCD_Out_2Str("G6", "1567.98 Hz");
				break;
			case '9':
				playNoteDuration(sg6, d2);
				LCD_Clr();
				LCD_Out_2Str("G#6/Ab6", "1661.22 Hz");
				break;
			case '0':
				playNoteDuration(a6, d2);
				LCD_Clr();
				LCD_Out_2Str("A6", "1760.00 Hz");
				break;
			case 'A':
				playNoteDuration(sa6, d2);
				LCD_Clr();
				LCD_Out_2Str("A#6/Bb6", "1864.66 Hz");
				break;
			case 'B':
				playNoteDuration(b6, d2);
				LCD_Clr();
				LCD_Out_2Str("B6", "1975.53 Hz");
				break;
			case 'C':
				playNoteDuration(c7, d2);
				LCD_Clr();
				LCD_Out_2Str("C7", "2093.00 Hz");
				break;
			case 'D':
				playNoteDuration(sc7, d2);
				LCD_Clr();
				LCD_Out_2Str("C#7/Db7", "2217.46 Hz");
				break;
			case '*':
				 //Define music
				 asm volatile ("nop"); //Problem with switch statements and declaring array
				 float melody[] = {b4, b4, b4, b4,  b4, b4, b4, b4, b4, b4, b4, b4, e5, e5, e5, e5, e5, e5, e5, e5, d5, d5, d5, d5, d5, d5, d5, a4, a4};
				 float beat[] =   {d1, d1, d1, d1, d2, d1, d1, d1, d1, d1, d1, d2, d1, d1, d1, d1, d1, d1, d1, d2, d1, d1, d1, d1, d1, d1, d2, d1, d1};
				 
				 LCD_Clr();
				 LCD_Out_2Str("SandStorm", "By Darude");
				 
				 //Iterate over all notes and call them
				 for(int j=0; j<3; j++){
					 for(int i=0; i<(sizeof melody / sizeof *melody); i++){
						 //Decrease period that music is played at
						 playNoteDurationSeparated(melody[i], beat[i]*.005);
					 }
				 }
				break;
			case '#':
				//Define music 
				asm volatile ("nop"); //Problem with switch statements and declaring array
				float melody2[] = {g5, g5, a5, g5, c6, b5, g5, g5, a5, g5, d6, c6, g5, g5, g6, e6, c6, b5, a5, f6, f6, e6, c6, d6, c6};
				float beat2[]   = {d1, d1, d2, d2, d2, d3, d1, d1, d2, d2, d2, d3, d1, d1, d2, d2, d2, d2, d2, d1, d1, d2, d2, d2, d3};
				
				LCD_Clr();
				LCD_Out_2Str("Happy Birthday", "Unknown");
				
				for(int i=0; i<(sizeof melody2 / sizeof *melody2); i++){
					playNoteDurationSeparated(melody2[i], beat2[i]);
				}
				break;
			default:
				//If nothing played, just disable the counter
				TCE0_CTRLB = 0x10;		
		}
    }
}

//ISR for overflow interrupt buffer
ISR(TCE1_OVF_vect){
	//Automatically stop playing music
	TCE0_CTRLB = 0x10;
	timer = 1;
}