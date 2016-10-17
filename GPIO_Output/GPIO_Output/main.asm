/*
* Lab 2 Part C
* Name:			Robert Olsthoorn
* Section #:	8705
* TA Name:		Daniel
* Description:	This program registers input from a keypad and outputs that data in hex format to memory address r17
*/

.nolist ;Don't include in resulting lss file
.include "ATxmega128A1Udef.inc"	;Optional 
.list	;Begin including in .lss file

.ORG 0x0000					;Code starts running from address 0x0000.
	rjmp MAIN				;Relative jump to start of program.

.ORG 0x0100					;Write to program memory at address 0x200
							 
MAIN:
	ldi R16, 0xFF			;Prepare to set all pins
	sts PORTE_DIRSET, R16	;Set PortE as output pins

	rcall KEYPAD_INIT		;Initialize the keypad
	rjmp KEYPAD_PRESS		;Run the keyboard loop

;Function: Infinite loop to scan for information and print out Hex Value to LED
;Edits: Nothing
;Takes: r17
KEYPAD_PRESS:
	rcall KEYPAD_READ;		;Calling operation
	ldi r19, r17
	cpi r17, 0xFF
	brne KEYPAD_RELEASE		;Trigger that key is not bouncing 
	sts PORTE_OUT, r17		;Output LEDs to special pins
	rjmp KEYPAD_PRESS		;Infinite loop

KEYPAD_RELEASE:
	rcall KEYPAD_READ;
	cpi r17, 0xFF			; See if value is open
	brne KEYPAD_RELEASE;	; If it isn't, continue to look for the release of the variable
	;Place to make whatever subroutine call is desired
	sts STS_PORTE_OUT, r19	; If it is, set the LEDS on port E to the value
	rjmp KEYPAD_PRESS		; Return to waiting for some initial kepyress

;Function: Initialize the ports required for proper keypad reading
;Edits: PORTF_DIRSET, PORTF_DIRCLR, PORTF_PINCTRL(4-7)
;Takes: Nothing	
KEYPAD_INIT:
	push r16
	ldi r16, 0x0F
	sts PORTF_DIRSET, r16	;Setting pins 0 1 2 3 as output pins
	ldi r16, 0xF0
	sts PORTF_DIRCLR, r16	;Setting pins 4 5 6 7 as input pins
	ldi r16, 0b00011000
	sts PORTF_PIN4CTRL, r16	;Setting pins 4 5 6 7 as PINUP
	sts PORTF_PIN5CTRL, r16
	sts PORTF_PIN6CTRL, r16
	sts PORTF_PIN7CTRL, r16
	pop r16					;restoring stack
	ret

;Function: Initialize the ports required for proper keypad reading
;Edits: r17 to store information on the hex value
;Takes: Nothing	(All following subroutines are helpers to KEYPAD_READ)	
KEYPAD_READ:

;Takes PORTF_IN
COL1:				; for 1 4 7 *
	ldi r17, 0xFF				;set r17 and 0xFF as the error variable
	ldi r16, 0b00001110			
	sts PORTF_OUT, r16			;set col1 as low
	nop
	nop
	lds r18, PORTF_IN			;read in values from input
	rjmp READ1
	
;Takes PORTF_IN
COL2:				; for 2 5 8 0 
	ldi r16, 0b00001101			
	sts PORTF_OUT, r16			;set col2 as low
	nop
	nop
	lds r18, PORTF_IN			;read in values from input
	rjmp READ2

;Takes PORTF_IN
COL3:				; for 3 6 9 #
	ldi r16, 0b00001011			
	sts PORTF_OUT, r16			;set col3 as low
	nop
	nop
	lds r18, PORTF_IN			;read in values from input
	rjmp READ3
	
;Takes PORTF_IN
COL4:				; for A B C D
	ldi r16, 0b00000111			
	sts PORTF_OUT, r16			;set col4 as low
	nop
	nop
	lds r18, PORTF_IN			;read in values from the output
	rjmp READ4
	
;Takes r18 from COL1
;Edits r17
READ1:
	sbrs r18, 7				; check if inputbit 7 is set, skip next instruction otherwise
	ldi r17, 0xE	;E
	sbrs r18, 6				; check if inputbit 6 is set, skip next instruction otherwise
	ldi r17, 0x7	;7	
	sbrs r18, 5				; check if inputbit 5 is set, skip next instruction otherwise
	ldi r17, 0x4	;4
	sbrs r18, 4				; check if inputbit 4 is set, skip next instruction otherwise
	ldi r17, 0x1	;1
	cpi r17, 0xFF	; See if r17 was overwritten
	brne END_READ
	rjmp COL2
	
;Takes r18 from COL2
;Edits r17
READ2:
	sbrs r18, 7				; check if inputbit is set, skip otherwise
	ldi r17, 0x0	;0
	sbrs r18, 6				; check if inputbit is set, skip otherwise
	ldi r17, 0x8	;8
	sbrs r18, 5				; check if inputbit is set, skip otherwise
	ldi r17, 0x5	;5
	sbrs r18, 4				; check if inputbit is set, skip otherwise
	ldi r17, 0x2	;2
	cpi r17, 0xFF		;Compare to see if overwritten
	brne END_READ
	rjmp COL3
	
;Takes r18 from COL3
;Edits r17
READ3:
	sbrs r18, 7				; check if inputbit is set, skip otherwise
	ldi r17, 0xF	;#		
	sbrs r18, 6				; check if inputbit is set, skip otherwise
	ldi r17, 0x9	;9
	sbrs r18, 5				; check if inputbit is set, skip otherwise
	ldi r17, 0x6	;6
	sbrs r18, 4				; check if inputbit is set, skip otherwise
	ldi r17, 0x3	;3
	cpi r17, 0xFF       ;Compare to see if overwritten
	brne END_READ
	rjmp COL4		
	
;Takes r18 from COL4
;Edits r17
READ4:
	sbrs r18, 7				; check if inputbit is set, skip otherwise
	ldi r17, 0xD	;D
	sbrs r18, 6				; check if inputbit is set, skip otherwise
	ldi r17, 0xC	;C
	sbrs r18, 5				; check if inputbit is set, skip otherwise
	ldi r17, 0xB	;B
	sbrs r18, 4				; check if inputbit is set, skip otherwise
	ldi r17, 0xA	;A
	cpi r17, 0xFF		;Compare to see if overwritten
	brne END_READ
END_READ:
	ret		;return to line from where it was called