					ORG		$2000

					; IO_RS232C_CMD		$FD07	(7,U)
					; IO_RS232C_DATA	$FD06	(6,U)
					; IO_RS232C_ENABLE	$FD0C	($0C,U)	FM77AV20/40 and newer only
					; IO_RS232C_MODE	$FD0B	($0B,U)	FM77AV20/40 and newer only

					; 2019/01/20 In FM-7 (original) FD0B works as FD0F (RAM mode on/off)

					LEAX	RESET_CMD,PCR
					LDU		#$FD00			; 3 clocks
					; The following 3 lines will enable RS232C in FM77AV20/40 and newer,
					; For some reason it prevents FM-7 from returning to F-BASIC.
					LDD		#$0510
					STA		$0C,U
					STB		$0B,U

RESET_LOOP			ORCC	#$50			; 3 clocks   Do it in the loop to wait for 8251A
					LDA		,X+				; 3 clocks
					STA		7,U				; 3 clocks
					BPL		RESET_LOOP		; Only last command is negative.	3 clocks

					MUL						; Wait 11 clocks.  Is it necessary?
					LDX		#$6000			; 3 clocks
					LDB		#$02			; 2 clocks

LOAD_LOOP			BITB	7,U
					BEQ		LOAD_LOOP
					LDA		6,U				; 1 byte shorter than "LDA IO_RS232C_DATA"
					STA		,X+
					CMPX	#$6100
					BNE		LOAD_LOOP

					LDA		$0F,U			; New! FM-7's FD0F seems to be leaked to FD0B 
											; (motherboard not checking Address Bus 2?)
											; Tested 2019/01/20.  Thanks Apollo again!

					RTS

RESET_CMD			FCB		0,0,0,$40,$4E,$B7

END_OF_PROGRAM
