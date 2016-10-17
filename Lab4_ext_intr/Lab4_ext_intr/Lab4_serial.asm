/*
* Lab 4 Part C
* Name:			Robert Olsthoorn
* Section #:	8705
* TA Name:		Daniel
* Description:	This program sets up athe USART for receiving and transmission through the USB interface to interact with the console.

* Lab4_serial.asm
*/

.nolist ;Don't include in resulting lss file
.include "ATxmega128A1Udef.inc"	;Optional 
.list	;Begin including in .lss file

.include "init.asm"
 
.org 0x0000
rjmp MAIN

.org 0x0200
MAIN:
	SET_STACK
	rcall USART_INIT

DONE:
	rjmp DONE;

/*
DESCRIPTION: Initialiazes the user transmission protocol information and correctly sets all port directions
IN: NOTHING
OUT: NOTHING
OVERWRITES: PORTD_DIRSET, PORTQ_DIRSET, USARTD0_CTRLB, USARTD0_CTRLC, USARTD0_BAUDCTRLA, USARTD0_BAUDCTRLB
*/
USART_INIT:
	push r16
	ldi r16, 0x08		;Setting bit 3 for TX as output
	sts PORTD_DIRSET, r16
	sts PORTD_OUTSET, r16	;Defaulting Tx output balue to be one
	ldi r16, 0x04		;Setting bit 2 as RX for input
	sts PORTD_DIRCLR, r16

	;Initializing portQ bits as outputs (with appropriate values)
	ldi r16, 0x0A	;Setting bits 3 and 1 as output
	sts PORTQ_DIRSET, r16	;Declaring direction as output ;USB_SW_EN
	sts PORTQ_OUTCLR, r16	;Setting default values on outputs to be lows	;USB_SW_SEL 

	ldi r16, 0x18
	sts USARTD0_CTRLB, r16 ;Enabling Receiver Enable and transmitter enable

	;Writing the equivalent calcluations for baud rate
	;	BSEL = 0x121	; 289 in hex
	;	BSCALE = 0b1001 ;-7 since 2's complement
	
	;Lower 8 bits of BSEL
	ldi r16, 0x21
	sts USARTD0_BAUDCTRLA, r16

	;BSCALE first 4 bits, BSEL lower 4 bits
	ldi r16, 0x91
	sts USARTD0_BAUDCTRLB, r16
		
	;Setting the USART for asynchonous mode
	ldi r16, 0b00100011	; 00 (asynchronous) | 10 (even parity) | 0 (stop bit) | 011 (8 bit data size)
	sts USARTD0_CTRLC, r16

	pop r16
	ret

/*
DESCRIPTION: Polls the USART's status to see if the transmission has been completed, and then transmits its own message
IN: r17
OUT: Loads USARTD0_DATA with r17
OVERWRITES: NOTHING
*/
OUT_CHAR:
	push r16
OUT_CHAR_LOOP:

	lds r16, USARTD0_STATUS
	sbrs r16, 5	;5 is set if ready to receive transmission information on the buffer
	rjmp OUT_CHAR_LOOP
		
	sts USARTD0_DATA, r17

	pop r16
	ret


/*
DESCRIPTION: Transmits a string stored in memory terminated by a null character using the OUT_CHAR subroutine
IN: Z
OUT: NOTHING
OVERWRITES: Z pointer as it increments
*/
OUT_STRING:
	push r17
OUT_STRING_LOOP:
	lpm r17, Z+
	cpi r17, 0x00	;Compare with the null terminated value
	breq END_STRING
	rcall OUT_CHAR	;Passing in r17 to the out character fucntion
	rjmp OUT_STRING_LOOP
END_STRING:
	pop r17
	ret


/*
DESCRIPTION: Polls the USART's status to see if the receive has been completed
IN: NONE
OUT: r17 with DATA
OVERWRITES: r17 (data passed)
*/
IN_CHAR:
	push r16
IN_CHAR_LOOP:
	lds r16, USARTD0_STATUS
	sbrs r16, 7;7 is set when the receiver has received some information
	rjmp IN_CHAR_LOOP
	lds r17, USARTD0_DATA	;Store the value that was received into the buffer
	pop r16
	ret