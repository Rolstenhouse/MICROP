#ifndef KEYPAD_H
#define KEYPAD_H

//FUNCTION SUMMARY
/*
NAME: avg
PURPOSE: Take the average of two numbers
INPUTS: Two uint_8's
OUTPUTS: Returns uint8 average
*/
char keypad_read(void);
char keypad_read_col1(void);
char keypad_read_col2(void);
char keypad_read_col3(void);
char keypad_read_col4(void);
void init_keypad(void);

#endif