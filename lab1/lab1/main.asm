/*
* Lab 3 Part B
* Name:			Robert Olsthoorn
* Section #:	8705
* TA Name:		Daniel
* Description:	This program filters a set of data written into program memory into a new 
				table, where all hex data values between 0x16 and 0x80 are XOR'd with 0x37
				and then copied into the new table.
*/

.nolist ;Don't include in resulting lss file
.include "ATxmega128A1Udef.inc"	;Optional 
.list	;Begin including in .lss file

.equ InputTable = 0x1BA2	;Declare input table start location (in program memory so will shift right 1)
.equ OutputTable = 0x2016	;Declare output table start location
.equ LowLimit = 0x16		;Declare lower filtering limit
.equ HighLimit = 0x80		;Declare high filtering limit
.equ XOR = 0x37				;Declare what value to exclusive or the value with
.equ EndValue = 0			;Declare the value to put in at the end of the table

.org 0x0000		;Begin program execution at 0x0000
	rjmp main	;Start at main

.org InputTable ; 0x3744 / 2 0x1BA2 Store potential in memory
Table: .db 0xA4, 0x70, 0x81, 0x58, 0x58, 0x53, 0x96, 0x17, 0x5D, 0xEE, 0x58, 0xF1, 0x83, 0xDB, 0x55, 0x99, 0x16, 0xC2, 0xD7, 0xF5, 0	;Store table in program memory

.dseg	;Begin data segment so that output is stored in data memorth (SRAM)

.org OutputTable	;Specify output table in memory
Outs: .byte 0x0		;Initialize variable for pointer allocation

.cseg				;Everything else in the code segment

.org 0x200		;Begin program execution at 0x200
MAIN:
		ldi ZL, low(Table << 1)	;Initialize low Z pointer (in memory)
		ldi ZH, high(Table << 1);Initialize high Z pointer (in memory)

		ldi YL, low(Outs)	;initialize low Y pointer to out location
		ldi YH, high(Outs)	;initialize high Y pointer to out location

LOOP:
		lpm r16, Z+		;load the value from the table into r16, and increment pointer Z
		cpi r16, 0		;compare to 0 to branch if data read is worthy of deleting
		breq END_TABLE	;branch if equal to zero

		ldi r17, LowLimit	;Load register with lowerlimit value for comparison
		cp r16, r17			;Lower limit range check
		brlo SKIP			;Branch if not over lower limit value

		ldi r17, HighLimit	;Load register with highlimit value for comparison
		subi r17, 1			;Subtract 1 so that compare operation not inclusive
		cp r17, r16			;Upper limit range check
		brlo SKIP			;Branch if not under upper limit value
		
		ldi r17, XOR		;Load register with value defined by XOR
		eor r16, r17		;exclusive or of registers
		st Y+, r16	;store data at r16	;Y pointer to store value from exclusive or

SKIP:	rjmp LOOP			;Repeat loop

DONE:
		rjmp DONE ;Infinite loop to completion becuase program terminated

END_TABLE:
		ldi r16, EndValue	;Load register with determined end value
		st Y, r16			;Store endValue at Pointer Y
		rjmp DONE			;Jump to Done