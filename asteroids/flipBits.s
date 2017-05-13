	PRESERVE8 ; Indicate the code here preserve
			  ; 8 byte stack alignment
	THUMB 	  ; Indicate THUMB code is used
	AREA |.text|, CODE, READONLY ; Start of CODE area
		
	EXPORT flipBits
	ENTRY
flipBits	FUNCTION
	PUSH {R1, R2, LR}	; save return address of r1 and r1 with link register
	MOV R2, R0			; moves bits in r0 to r2
	RBIT R1, R2			; reverses the order of bit
	LSR R0, R1, #24		; logical shift right by 24 bits - store in r1
	POP {R2,R1, LR}		; r1 and r2 are restored
	BX LR				; return bits in registers
	ENDFUNC
	END					; end of file