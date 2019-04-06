					; To be stored inside F-BASIC string.
					; The server will encode LOADER_START to END_OF_PROGRAM in ASCII format
					;    b -> 0x30+((b&0xF0)>>4) , 0x30+(b&0x0F)

					; To be used as:
					; 10 OPEN "I",#1,"COM0:(F8N1)"
					; 20 LINE INPUT #1,A$
					; 30 EXEC VARPTR(A$)

					; To make it possible, the length sent from the server needs to be exactly 0x7E bytes.
					; 0x7E is JMP.

					; F-BASIC string (A$) is stored as:
					;   VARPTR(A$)    (Number of bytes)
					;   VARPTR(A$)+1  (String Pointer Higher Byte)
					;   VARPTR(A$)+2  (String Pointer Lower Byte)
					; To jump to the instruction stored in the string, the first byte needs to be 0x7E.

					ORG		$2000

					; IO_RS232C_CMD		$FD07	(7,U)
					; IO_RS232C_DATA	$FD06	(6,U)
					; IO_RS232C_ENABLE	$FD0C	($0C,U)	FM77AV20/40 and newer only
					; IO_RS232C_MODE	$FD0B	($0B,U)	FM77AV20/40 and newer only

					; 2019/01/20 In FM-7 (original) FD0B works as FD0F (RAM mode on/off)


						; Decoder part >>
						; No byte can be below #$20
						LEAX	LOADER_START+$20,PCR
						LEAX	-$20,X
						STX		,--S
						PULS	U

LOOP					LDA		,X+
						LSLA
						LSLA
						LSLA
						LSLA
						STA		,U
						LDA		,X+
						SUBA	#$30
						ORA		,U
						STA		,U+
						DECB
						BNE		LOOP
						; Decoder part <<




LOADER_START
					LDA		#$86		; $86$86 will be a marker for the server to encode the binary.

					LEAX	RESET_CMD,PCR
					LDU		#$FD00

					; The following 3 lines will enable RS232C in FM77AV20/40 and newer,
					; For some reason it prevents FM-7 from returning to F-BASIC.
					LDD		#$0510
					STA		$0C,U
					STB		$0B,U
					LDB		$0B,U			; New! FM-7's FD0F seems to be leaked to FD0B 
											; (motherboard not checking Address Bus 2?)
											; Tested 2019/01/20.  Thanks Apollo again!

RESET_LOOP			ORCC	#$50			; 3 clocks   Do it in the loop to wait for 8251A
					LDA		,X+				; 3 clocks
					STA		7,U				; 3 clocks
					BPL		RESET_LOOP		; Only last command is negative.	3 clocks

					MUL						; Wait 11 clocks.  Is it necessary?


					; Send "YAMAKAWA" to the server, then the server will return installer binary.
					LEAX	REQ_CMD,PCR
					LDB		#8
YAMAKAWA_LOOP		INCA
					BNE		YAMAKAWA_LOOP
					LDA		7,U
					LSRA
					BCC		YAMAKAWA_LOOP
					LDA		,X+
					STA		6,U
					DECB
					BNE		YAMAKAWA_LOOP


					LDX		#$6000			; 3 clocks
					CLRB					; 2 clocks

LOAD_LOOP			LDA		#2
					ANDA	7,U
					BEQ		LOAD_LOOP
					LDA		6,U				; 1 byte shorter than "LDA IO_RS232C_DATA"
					STA		,X+
					DECB
					BNE		LOAD_LOOP

					RTS

RESET_CMD			FCB		0,0,0,$40,$4E,$B7

REQ_CMD				FCB		"YAMAKAWA"

END_OF_PROGRAM
