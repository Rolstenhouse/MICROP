/*
* Lab 2 Part C
* Name:			Robert Olsthoorn
* Section #:	8705
* TA Name:		Daniel
* Description:	This program works with the CPLD to add I/O PORT Expansion. It specifically takes in input from switches and 
*				displays them on an LED bank. If the original number is even, it shifts left 8 times and then looks for 
*				input again while if it is odd, it shifts right 8 times and then looks for input again.
*/

.nolist ;Don't include in resulting lss file
.include "ATxmega128A1Udef.inc"	;Optional 
.list	;Begin including in .lss file

.equ stack_init = 0x3fff	; Stack initialization

.set IOPORT = 0x288000		; Start of address
.set IOPORTEND = 0x289fff	; End of addressable size

.equ SRAM = 0b00010101		; 0|00101|01		UNUSED | 8K memory size | SRAM memory allocation

.org 0x0000
rjmp MAIN

.org 0x0200
MAIN:

	;Initializing stack pointer, since always required
	ldi ZL, low(stack_init)		;Initializing low byte
	out CPU_SPL, ZL
	ldi ZH, high(stack_init)	;Initializing high byte
	out CPU_SPH, ZH

	ldi R16, 0b00010111		;setting CS0 (bit 4) ALE1 (bit 2) RE(L) (bit 1), WE(L) (bit 0); 
	sts PORTH_DIRSET, R16 	
	
	ldi R16, 0b0011			;Setting RE(L) and CS0(L) as active low signal			
	sts PORTH_OUTSET, R16

	ldi R16, 0xFF			;Set all PORTJ pins (D7-D0) to be outputs. As required 
	sts PORTJ_DIRSET, R16	;  in the data sheet.  (Ignored!) See 8331, sec 27.9.

	ldi R16, 0xFF			;Set all PORTK pins (A15-A0) to be outputs. As required	
	sts PORTK_DIRSET, R16	;  in the data sheet.  (Ignored!) See 8331, sec 27.9.

	ldi R16, 0x01			;Setting the 3 port EBI ctrl (See docs)
	sts EBI_CTRL, R16		

	ldi ZH, high(EBI_CS0_BASEADDR)	;Setting the CS0 base address space to pointer Z
	ldi ZL, low(EBI_CS0_BASEADDR)

	ldi R16, low(IOPORT>>8)	;Store BASEADDRL into EBI_CS0_BASEADDR A[15:12], (since by default all other values are 0)
	st Z+, R16				

	ldi R16, (IOPORT>>16)	;Store BASEADDRH into EBI_CS)_BASEADDR A[23:16]
	st Z, R16				

	ldi R16, SRAM				;Set to value previously set in header
	sts EBI_CS0_CTRLA, R16			
	
	ldi R16, byte3(IOPORT)		;Loading with the highest value 0x28
	sts CPU_RAMPX, r16			;Adding onto CPU_RAMPX for indirect addressing
	ldi XH, HIGH(IOPORT)		;Declaring X to point to IOPORT location
	ldi XL, LOW(IOPORT)				

	ldi r18, 50					; Used as a multiplier for the DELAY_X0ms_LOOP

LOOP:
	ld r16, X			;Read in value from EBI input register
	ldi r19, 8			;Declare number of shifts before rechecking value
	sbrc r16, 0			;Skip to Rotate_left if r16 is even
	rjmp ROTATE_RIGHT	;rjmp to Rotate_right if r16 is odd

ROTATE_LEFT:
	cpi r19, 0x00		;Making sure r19 is the one being compared
	breq LOOP			;If shifted 8 times, reloop
	dec r19				;dec counter
	lsl r16				;Shift all the bits left
	;rol			; doesn't work to rotate the value left
	brcc NO_CARRY		; If carry bit set, add 1 (works like a rotate)
	inc r16				; Adding 1
NO_CARRY:				; No carry bit
	st X, r16			; Put r16 on the data bus to output
	rcall DELAY_X0ms	; Calls with r18 at 50 for 1/2 sec delay
	rjmp ROTATE_LEFT;	; rjmp to top

ROTATE_RIGHT:
	cpi r19, 0x00		; make sure r19 is the one which is compared
	breq LOOP			; if shifted 8 times reloop
	dec r19				; dec loop counter
	;ror r16		; doesn't work to rotate the value right
	sbrc r16, 0			; Don't add bit 7 (necessary to rotate properly) if bit 0 is set.
	rjmp ADD_BIT7		; Add to bit 7 if bit 0 is set
	lsr r16				; logically shift all bits right
RETURN_ADD_BIT7:
	st X, r16			; put r16 on the output data bus
	rcall DELAY_X0ms	; delay for  1/2 second
	rjmp ROTATE_RIGHT	; return to rotate right
ADD_BIT7:				; 
	lsr r16				;shift everything right
	push r17			;store r17 on the stack (so can be used without modification)
	ldi r17, 0x80		;ldi r17 with 1000 0000
	add r16, r17		;adding 10000000 to what was shifted right
	pop r17				;restoring value in r17
	rjmp RETURN_ADD_BIT7	;rjmp back to storing x


; 
; DELAY LIBRARY FUNCTIONS
;

; Stores ZL, ZH
DELAY_10ms:
	push ZL		; temporarily store on the stack
	push ZH		; temporarily store on the stack

	ldi ZL, 0x80	; store low to get approximately 10ms
	ldi ZH, 0x13	; store high to get approximately 10ms

DELAY_10ms_LOOP:
	sbiw ZH:ZL, 1	; Subtract immediate with carry, will properly subtract 1
	brne DELAY_10ms_LOOP	; Loop

	pop ZH			;return ZH to original
	pop ZL			; retunr ZL to original
	ret				; return to rcall location

; Takes parameter r18, for number of iterations of 10ms
; Does not modify r18
; INTENT: Loop for set number of X0ms 

DELAY_X0ms:
	push r18	; temporarily store on the stack

DELAY_X0ms_LOOP:
	rcall DELAY_10ms	;Call delay function
	subi r18, 1			;Subtract 1 from r18, 
	brne DELAY_X0ms_LOOP	; If not equal to 0, break out of value
	
	pop r18				; return r18 to original value
	ret					; return to rcall location