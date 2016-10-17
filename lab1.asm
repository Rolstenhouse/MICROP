/*
* @Author: Rolstenhouse 
* LAB1
*

This program filters a set of data written into program memory into a new 
table, where all hex data values between 0x16 and 0x80 are XOR'd with 0x37
and then copied into the new table

*/

.include "ATxmega128A1Udef.inc"

.ORG 0x0000
	rjmp main

.CSEG
.ORG 0x3744
table:		.db 0xA4, 0x70, 0x81, 0x58, 0x58, 0x53, 0x96, 0x17,
				0x5D, 0xEE, 0x58, 0xF1, 0x83, 0xDB, 0x55, 0x99,
				0x16, 0xC2, 0xD7, 0xF5, 0
MAIN:

loop:
		cp r1, 0
		breq done
		rjmp loop

done:
		nop
		