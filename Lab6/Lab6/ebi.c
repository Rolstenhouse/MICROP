/*//////////////////////////////////////////////////////////////////////////////////
 * ebi.c
 *
 *  Author:  Robert Olsthoorn
 *  Purpose: To provide an example for the EBI in C. Program configures both CS0
 *           and CS1 to control two external address ranges.  The example also 
 *           shows how pointers	work for reading and writing to I/O.  Finally, the
 *           example shows how an inline Assembly command (include file) may be
 *           used to access memory locations using 3 bytes.
 *///////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////INCLUDES///////////////////////////////////////
#include <avr/io.h>
#include "ebi_driver.h"

//////////////////////////////////INITIALIZATIONS////////////////////////////////////
#define F_CPU 2000000       // ATxmega runs at 2MHz on Reset.
#define CS0_Start 0x288000
#define CS0_End 0x289FFF
#define CS1_Start 0x394000
#define CS1_END 0x397FFF
#define LCD_START 0x395000
#define LCD_END 0x396FFF

/************************************************************************************
* Name:     EBI_init
* Purpose:  Function to initialize the desired EBI Ports.  Configures to run IO Port, 
*           SRAM, and LCD.  All CSs and other control signals generate appropriate
*           enables inside CPLD.
* Inputs:
* Output:
************************************************************************************/
void EBI_init()
{
	EBI_3port_init();
	CS0_init();
	CS1_init();
}

void EBI_3port_init()
{
	    PORTH.DIR = 0x37;       // Enable RE, WE, CS0, CS1, ALE1
	    PORTH.OUT = 0x33;
	    PORTJ.OUTSET = 0xFF;
		PORTK.DIR = 0xFF;   //Enable Address 7:0 (outputs)
	    
	    EBI.CTRL = EBI_SRMODE_ALE1_gc | EBI_IFMODE_3PORT_gc;            // ALE1 multiplexing, 3 port configuration	
}

void CS0_init()
{
	    EBI.CS0.BASEADDRH = (uint8_t) (CS0_Start >> 16) & 0xFF;
		EBI.CS0.BASEADDRL = (uint8_t) (CS0_Start >> 8) & 0xFF;  //Set CS0 range from range specified
	    EBI.CS0.CTRLA = EBI_CS_MODE_SRAM_gc | EBI_CS_ASPACE_8KB_gc;	    // SRAM mode, 8k address space
}

void CS1_init()
{
	    EBI.CS1.BASEADDR = (uint16_t) (CS1_Start>>8) & 0xFFFF;          // Set CS1 range to 0x394000 - 0x397FFF
	    EBI.CS1.CTRLA = EBI_CS_MODE_SRAM_gc | EBI_CS_ASPACE_16KB_gc;
}