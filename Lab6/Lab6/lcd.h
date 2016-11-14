#ifndef LCD_H
#define LCD_H

void init_LCD(void);
void LCD_Out_Char(char);
void LCD_Out_Str(char*);
void pollBF();
void LCD_Out_2Str(char* , char* );
void LCD_Clr();

#endif