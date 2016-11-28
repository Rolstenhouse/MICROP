#include "ebi.h"
#include "ebi_driver.h"
#include <avr/io.h>
#define F_CPU 2000000       // ATxmega runs at 2MHz on Reset.
#define CS0_Start 0x288000
#define CS0_End 0x289FFF
#define CS1_Start 0x394000
#define CS1_END 0x397FFF
#define LCD_START 0x395000
#define LCD_END 0x396FFF
#define LCD_CMD 0x395000
#define LCD_DATA 0x395001

void init_LCD(void)
{
	EBI_init();
	pollBF();
	__far_mem_write(LCD_CMD, 0x38);
	pollBF();
	__far_mem_write(LCD_CMD, 0x0F);
	pollBF();
	__far_mem_write(LCD_CMD, 0x01);
}

void LCD_Out_Char(char c)
{
	pollBF();
	__far_mem_write(LCD_DATA, c);
}

void LCD_Out_Str(char* STR)
{
	while( *STR )
	{
		LCD_Out_Char(*STR++);
	}
}

void LCD_Out_2Str(char* c1, char* c2){
	LCD_Out_Str(c1);
	pollBF();
	__far_mem_write(LCD_CMD, 0xC0); //Write instruction to move to second line
	pollBF();
	LCD_Out_Str(c2);
}
	
void LCD_Clr(){
	pollBF();
	__far_mem_write(LCD_CMD, 0x01);
}

void pollBF(){
	asm volatile("nop");
	asm volatile("nop");
	volatile uint8_t bf = 0;
	do{
		bf = __far_mem_read(LCD_CMD);
	}while((bf & 0x80)==0x80);
}