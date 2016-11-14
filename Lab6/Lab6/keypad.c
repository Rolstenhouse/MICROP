/*
 * HW4C.c
 *
 * Description: Keypad reading and other information
 */ 

#include <avr/io.h>

#define F_CPU 2000000       // ATxmega runs at 2MHz on Reset.
#define CHECK_BIT(var,pos) !((var) & (1<<(pos)))

char keypad_read(void);
char keypad_read_col1(void);
char keypad_read_col2(void);
char keypad_read_col3(void);
char keypad_read_col4(void);
void init_keypad(void);

//FUNCTIONS
//Return char
char keypad_read(){
	char col = keypad_read_col1();
	if(col!='!'){
		return col;
	}
	col = keypad_read_col2();
	if(col!='!'){
		return col;
	}
	col = keypad_read_col3();
	if(col!='!'){
		return col;
	}
	return keypad_read_col4();
}

char keypad_read_col1(){
	PORTF_OUT = 0b00001110;
	int input = PORTF_IN;
	if(CHECK_BIT(input, 7)){
		return '*';
	}
	if(CHECK_BIT(input, 6)){
		return '7';
	}
	if(CHECK_BIT(input, 5)){
		return '4';
	}
	if(CHECK_BIT(input, 4)){
		return '1';
	}
	return '!';
}

char keypad_read_col2(){
		PORTF_OUT = 0b00001101;
		int input = PORTF_IN;
		if(CHECK_BIT(input, 7)){
			return '0';
		}
		if(CHECK_BIT(input, 6)){
			return '8';
		}
		if(CHECK_BIT(input, 5)){
			return '5';
		}
		if(CHECK_BIT(input, 4)){
			return '2';
		}
		return '!';
}
char keypad_read_col3(){
	PORTF_OUT = 0b00001011;
	int input = PORTF_IN;
	if(CHECK_BIT(input, 7)){
		return '#';
	}
	if(CHECK_BIT(input, 6)){
		return '9';
	}
	if(CHECK_BIT(input, 5)){
		return '6';
	}
	if(CHECK_BIT(input, 4)){
		return '3';
	}
	return '!';
}
char keypad_read_col4(){
	PORTF_OUT = 0b00000111;
	int input = PORTF_IN;
	if(CHECK_BIT(input, 7)){
		return 'D';
	}
	if(CHECK_BIT(input, 6)){
		return 'C';
	}
	if(CHECK_BIT(input, 5)){
		return 'B';
	}
	if(CHECK_BIT(input, 4)){
		return 'A';
	}
	return '!';
}

void init_keypad(){
	//KEYPAD PORT INITIALIZATION
	PORTF_DIRSET = 0x0F;
	PORTF_DIRCLR = 0xF0;
	int pin_ctrl = 0x18;
	PORTF_PIN4CTRL = pin_ctrl;
	PORTF_PIN5CTRL = pin_ctrl;
	PORTF_PIN6CTRL = pin_ctrl;
	PORTF_PIN7CTRL = pin_ctrl;
}
