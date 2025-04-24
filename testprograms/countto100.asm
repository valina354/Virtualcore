; program that counts fro3m 0 to 100 then resets cpu

main:
	CALL enable_interrupts
	MOV R0, 0 	; set defaults
	MOV R1, 100	; value to compare to
	looping:
		INC R0 ;increment the register
		CMP R0, R1	; compare the register with the final valje
		INT 0x01	;print R0
		PUSH R0	;push R0 onto the stack
		MOV R0, 100	;time for sleep interrupt
		INT 0x30	;sleep
		POP R0	;pop value back
		JMPNE looping	;jump not equal
		INT 0x40	; reset cpu

enable_interrupts:
	ELI	;enable interrupts
	RET
