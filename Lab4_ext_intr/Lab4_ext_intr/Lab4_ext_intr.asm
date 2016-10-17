/*
* Lab 4 Part A
* Name:			Robert Olsthoorn
* Section #:	8705
* TA Name:		Daniel
* Description:	Program registers PIN0 on portE as a trigger to watch for interrupts and when falling edge is detected
*				it increments a counter by 1 and sets it as on the LED port declared by the EBI

* Lab4_ext_intr.asm
*/

.nolist ;Don't include in resulting lss file
.include "ATxmega128A1Udef.inc"	;Optional 
.list	;Begin including in .lss file

.include "init.asm"

.org PORTE_INT0_vect
	rjmp ISR_LED_COUNT

.def counter = r17

.org 0x0000
rjmp MAIN

.org 0x0200
MAIN:
	SET_STACK
	SET_EBI		;Using X as the pointer to the LEDs ;Also sets LEDs as outputs

	ldi r16, 0x00	;Clear existing bits
	st X, r16

	ldi counter, 0
	rcall INIT_EXT_INTR	

DONE:
	rjmp DONE;

INIT_EXT_INTR:
	push r16

	ldi r16, 0x01		;Setting value as a low-level interrupt
	sts PORTE_INTCTRL, r16

	sts PORTE_INT0MASK, r16 ;Setting pin0 for interrupt

	sts PORTE_DIRCLR, r16	;Setting pin0 as default to go for input pin
	ldi r16, 0b00000010		; 00000 | 010 ;Setting 010 for sensing falling edge
	sts PORTE_PIN0CTRL, r16
	ldi r16, 0x01
	sts PMIC_CTRL, r16	;Seting PMIC as a low-level interrupt level

	pop r16
	sei		;Setting the global interrupt flag

	ret;

ISR_LED_COUNT:
	call DELAY_10ms
	call DELAY_10ms
	call DELAY_10ms
	inc counter
	st X, counter

	reti