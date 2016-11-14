/*
* HW4 Problem 1 ASM
* Name:			Robert Olsthoorn
* Section #:	8705
* TA Name:		Daniel
* Description:	This program takes the average of two numbers declared in the .equ
*/

.nolist ;Don't include in resulting lss file
.include "ATxmega128A1Udef.inc"	;Optional 
.list	;Begin including in .lss file

.equ stack_init = 0x3fff	; Stack initialization
.equ num1 = 10
.equ num2 = 20

.org 0x000
rjmp MAIN

.org 0x200
MAIN:
	ldi r16, num1
	ldi r17, num2

	rcall AVERAGE
	rjmp DONE
DONE:
	rjmp DONE
/*
TAKES: r16, r17 as argument 
EDITS: r16
RETURNS: Average in r16
*/
AVERAGE:
	push r17
	add r16, r17
	asr r16
	pop r17
	ret
	