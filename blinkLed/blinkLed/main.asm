/* 6.23 */

.nolist ;Don't include in resulting lss file
.include "ATxmega128A1Udef.inc"	;Optional 
.list	;Begin including in .lss file

.def minimum = r16
.def counter = r18
.equ startData = 0x2016
.equ length = 32

.ORG 0x0000					;Code starts running from address 0x0000.
	rjmp MAIN				;Relative jump to start of program.
					;Write to program memory at address 0x200
.dseg
.ORG startData	
BUF: .byte 1

.cseg

.org 0x100					 
MAIN:
	ldi r16, 0x37
	push r16
	ldi r16, 0xAB
	push r16
	ldi r16, 0xEF
	push r16
	ldi r16, 0x12
	push r16
	rcall PUSHSTACK
	rjmp DONE

PUSHSTACK:
	ldi r16, 0x1C
	push r16
	ret

DONE:
	rjmp DONE