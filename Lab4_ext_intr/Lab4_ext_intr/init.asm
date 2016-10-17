; Robert Olsthoorn
; Initialization library so that the same code need not be rewritten for every file

.equ stack_init = 0x3fff	; Stack initialization

.set IOPORT = 0x288000		; Start of address
.set IOPORTEND = 0x289fff	; End of addressable size

.equ SRAM = 0b00010101		; 0|00101|01		UNUSED | 8K memory size | SRAM memory allocation

.macro SET_STACK

	;Initializing stack pointer, since always required
	ldi ZL, low(stack_init)		;Initializing low byte
	out CPU_SPL, ZL
	ldi ZH, high(stack_init)	;Initializing high byte
	out CPU_SPH, ZH

.endmacro

.macro SET_EBI
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
	
.endmacro	

;Set so not stored at beginning instructions
.org 0x100

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