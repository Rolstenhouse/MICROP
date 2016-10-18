/*
* Lab 4 QUIZ
* Name:			Robert Olsthoorn
* Section #:	8705
* TA Name:		Daniel
* Description:	

* Lab4_serial_quiz.asm
*/

.nolist ;Don't include in resulting lss file
.include "ATxmega128A1Udef.inc"	;Optional 
.list	;Begin including in .lss file

.org PORTE_INT0_vect
	rjmp ISR_PAUSE
	rjmp MENU_PRINT

.include "init.asm"
 
 .equ CR = 0x0D
 .equ LF = 0x0A
 .equ ESC = 0x1B
 .equ END = 0x00

 ;Storing table in memory
 .org 0x300
	MENU: .db	"Robert's Favorite:", CR, LF
			.db	"1.     OS/Computer (Mac or PC)", CR, LF
			.db "2.     EE/CE Course ", CR, LF
			.db	"3.     Hobby", CR, LF
			.db	"4.     Quote", CR, LF
			.db	"5.     Movie", CR, LF
			.db	"6.     Re-display menu", CR, LF
			.db	"ESC:   exit", CR, LF, END

	Computer: .db	"Robert has been told he's fallen in love with his Mac", CR, LF, END
	Course:	.db		"Robert enjoyed digital logic EEL3701 a lot ", CR, LF, END
	Hobby: .db		"Robert has been known to dabble in salsa dancing ", CR, LF, END
	Quote: .db		"Robert believes 'Strength does not come from winning. Your struggles develop your strengths' ", CR, LF, END
	Movie: .db		"Robert found the idea of life on Mars fascinating with his favorite movie Total Recall ", CR, LF, END
	Exit: .db		"Done!", CR, LF, END
	NewLine: .db	" ", CR, LF, END
	Error: .db		"ERROR", CR, LF, END

.org 0x0000
rjmp MAIN

.org 0x0500
MAIN:
	SET_STACK
	SET_EBI ;Unneccessary
	rcall USART_INIT
	rcall INIT_EXT_INTR
	rjmp MENU_PRINT

INIT_EXT_INTR:
	push r16

	ldi r16, 0x01		;Setting value as a low-level interrupt
	sts PORTE_INTCTRL, r16

	sts PORTE_INT0MASK, r16 ;Setting pin0 for interrupt

	sts PORTE_DIRCLR, r16	;Setting pin0 as default to go for input pin
	ldi r16, 0b00000001		; 00000 | 010 ;Setting 001 for sensing rising edge
	sts PORTE_PIN0CTRL, r16
	ldi r16, 0x01
	sts PMIC_CTRL, r16	;Seting PMIC as a low-level interrupt level

	pop r16
	sei		;Setting the global interrupt flag

	ret;

ISR_PAUSE:
	;Check status on portE
	push r16
	push ZL
	push ZH
	ldi ZL, low(Error << 1)
	ldi ZH, high(Error <<1)
	rcall OUt_STRING 
ISR_PAUSE_LOOP:
	lds r16, PORTE_IN
	sbrc r16, 0
	rjmp ISR_PAUSE_LOOP

	ldi ZL, low(Menu << 1)
	ldi ZH, high(Menu <<1)
	rcall OUT_STRING


	;Clear flag
	ldi r16, 0x01
	sts PORTE_INTFLAGS, r16; Clear flag
	pop ZH
	pop ZL
	pop r16
	reti

/*
DESCRIPTION: State to filter based on read input
IN: NOTHING
OUT: NOTHING
OVERWRITES: r17, used for internal comparison
*/
AWAIT_INPUT:
	rcall IN_CHAR
	cpi r17, '1'
	breq MENU1
	cpi r17, '2'
	breq MENU2
	cpi r17, '3'
	breq MENU3
	cpi r17, '4'
	breq MENU4
	cpi r17, '5'
	breq MENU5
	cpi r17, '6'
	breq MENU_PRINT
	cpi r17, ESC
	breq DONE
	rjmp AWAIT_INPUT

MENU1:
	ldi ZL, low(Computer << 1)
	ldi ZH, high(Computer <<1)
	rjmp PRINT_TO_CONSOLE

MENU2:
	ldi ZL, low(Course << 1)
	ldi ZH, high(Course <<1)
	rjmp PRINT_TO_CONSOLE

MENU3:
	ldi ZL, low(Hobby << 1)
	ldi ZH, high(Hobby <<1)
	rjmp PRINT_TO_CONSOLE

MENU4:
	ldi ZL, low(Quote << 1)
	ldi ZH, high(Quote <<1)
	rjmp PRINT_TO_CONSOLE

MENU5:
	ldi ZL, low(Movie << 1)
	ldi ZH, high(Movie <<1)
	rjmp PRINT_TO_CONSOLE

MENU_PRINT:
	ldi ZL, low(MENU << 1)
	ldi ZH, high(MENU << 1)
	rcall OUT_STRING
	ldi ZL, low(Newline << 1)
	ldi ZH, high(Newline << 1)
	rcall OUT_STRING
	rjmp AWAIT_INPUT

DONE:
	ldi ZL, low(Exit << 1)
	ldi ZH, high(Exit << 1)
	rcall OUT_STRING
DONE_LOOP:
	rjmp DONE_LOOP;

/*
DESCRIPTION: Handles the printing of the line as well as the newline character 
IN: Z
OUT: Prints to OUT_STRING console
OVERWRITES: Z pointer with newline information
*/
PRINT_TO_CONSOLE:
	rcall OUT_STRING
	ldi ZL, low(Newline << 1)
	ldi ZH, high(Newline << 1)
	rcall OUT_STRING
	rjmp MENU_PRINT
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
	;	BSEL = 0x1AC	; 428 in hex
	;	BSCALE = 0b1001 ;-7 since 2's complement
	
	;Lower 8 bits of BSEL
	ldi r16, 0xAC
	sts USARTD0_BAUDCTRLA, r16

	;BSCALE first 4 bits, BSEL lower 4 bits
	ldi r16, 0x91
	sts USARTD0_BAUDCTRLB, r16
		
	;Setting the USART for asynchonous mode
	ldi r16, 0b00100011	; 00 (asynchronous) | 11 (odd parity) | 1 (2 stop bits) | 011 (8 bit data size)
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
	cpi r17, END	;Compare with the null terminated value
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