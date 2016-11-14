/*
* Lab6_PartA.c
* Name:			Robert Olsthoorn
* Section #:	8705
* TA Name:		Daniel
* Description:  This program uses the keypad to enable/disable the note c6 playing on a piezo buzzer.
* 
 */ 

#include <avr/io.h>
#include "keypad.h"

int C6 = 0x3C0;

void playNote(int frequency);

//Takes the frequency and sets the count to be that value
void playNote(int frequency){
	TCE0_CCA = frequency;
}

int main(void)
{
	//Port initialization
	PORTE_DIRSET = 0x01;
	
	TCE0_CTRLA = 0x01; //Sets prescaler to be on
	TCE0_CTRLB = 0x11; //Enable CCD, CCC, CCB, and CCA Compare and capture enable
	TCE0_CTRLC = 0x0F; //Set Compare output value
	TCE0_CTRLD = 0b10100000; //Timer event action (frequency capture)
	TCE0_CTRLE = 0x00; //Set timer in normal count mode
	
	init_keypad();
    while (1) 
    {
		//Look for keypad on part 2	
		char read = keypad_read();
		if(read == '2'){
			//Enabling output from TCE0
			TCE0_CTRLB = 0x11;
			playNote(C6);
		}
		else{
			//Disabling output from TCE0
			TCE0_CTRLB = 0x10;
		}
    }
}

