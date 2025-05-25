_main:
	;print to 100 skip 13
	WHILE R0 <= 100
		IF R0 == 13
			INC R0
			CONTINUE
		END
		INT 0x01
		INC R0
	END
