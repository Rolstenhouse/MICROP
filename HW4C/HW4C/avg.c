/*
 * HW4C.c
 *
 * Created: 10/24/16 5:28:47 PM
 * Author : rolstenhouse
 */ 

#include <avr/io.h>

#define F_CPU 2000000       // ATxmega runs at 2MHz on Reset.

//FUNCTION SUMMARY

/*
NAME: avg
PURPOSE: Take the average of two numbers
INPUTS: Two uint_8's
OUTPUTS: Returns uint8 average
*/
uint8_t avg(uint8_t first, uint8_t second);

//FUNCTIONS
uint8_t avg(uint8_t first, uint8_t second){
	return (first+second) >> 1;
}

//MAIN FUNCTION
int main(void)
{
    uint8_t temp = avg(10,10);
	while(1){
		//Endless loop to keep running
	}
}



