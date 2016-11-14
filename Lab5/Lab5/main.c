/*
 * Lab5.c
 *
 * Created: 10/30/16 12:44:17 PM
 * Author : rolstenhouse
 */ 

#include <avr/io.h>
#include "ebi.h"
#include "ebi_driver.h"
#define F_CPU 2000000       // ATxmega runs at 2MHz on Reset.
#define CS0_Start 0x288000
#define CS0_End 0x289FFF
#define CS1_Start 0x394000
#define CS1_END 0x397FFF
#define LCD_START 0x395000
#define LCD_END 0x396FFF
#define LCD_CMD 0x395000
#define LCD_DATA 0x395001

void Init_LCD(void)
{
	pollBF();
	__far_mem_write(LCD_CMD, 0x38);
	pollBF();
	__far_mem_write(LCD_CMD, 0x0F);
	pollBF();
	__far_mem_write(LCD_CMD, 0x01);
}

void LCD_OUT_CHAR(char c)
{
    pollBF();
    __far_mem_write(LCD_DATA, c);
}

void LCD_OUT_STR(char* STR)
{
    while( *STR )
    {
        LCD_OUT_CHAR(*STR++);
    }
}

int main(void)
{
    /* Replace with your application code */
		EBI_init();	// Ready Ebi since LCD is bound to external addresse
		
		pollBF();
		__far_mem_write(LCD_CMD, 0x38);
		pollBF();
		__far_mem_write(LCD_CMD, 0x0F);
		pollBF();
		__far_mem_write(LCD_CMD, 0x01);
		pollBF();
		LCD_OUT_STR("Robert Olsthoorn");
		
    while (1) 
    {
		pollBF();
    }
}

void pollBF(){
	asm volatile("nop");
	asm volatile("nop");
	volatile uint8_t bf = 0;
	do{
		bf = __far_mem_read(LCD_CMD);
	}while((bf & 0x80)==0x80);
}