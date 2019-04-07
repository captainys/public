					; To be stored inside F-BASIC string.
					; The server will escape with $20
					;    If b==$20, take COM of the next byte.

					; The server will identify where to encode by looking at the last two bytes of this binary.

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

INSTALLER_ADDR		EQU		$6000


PROGRAM_BEGIN
					ORG		$2000

					; IO_RS232C_CMD		$FD07	(7,U)
					; IO_RS232C_DATA	$FD06	(6,U)
					; IO_RS232C_ENABLE	$FD0C	($0C,U)	FM77AV20/40 and newer only
					; IO_RS232C_MODE	$FD0B	($0B,U)	FM77AV20/40 and newer only

					; 2019/01/20 In FM-7 (original) FD0B works as FD0F (RAM mode on/off)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

					; Decoder part >>
					; No byte can be below or equal to #$20


					LEAX	END_OF_DECODER+$2F2F,PCR

					LDA		#-3
					NEGA
					STA		-(END_OF_DECODER+$2F2F-DECODE_BRANCH-1),X
					DEC		-(END_OF_DECODER+$2F2F-DECODE_BRANCH+1),X

					LEAX	-$2F2F,X
					STX		,--S
					PULS	U
					LDB		#END_OF_PROGRAM-END_OF_DECODER

DECODE_LOOP			LDA		,X+
					CMPA	#$21		; Tentative.  Decremented to #$20
DECODE_BRANCH		BNE		DECODE_LOOP	; Tentative.  Need to make it #3
					LDA		,X+
					COMA

DECODE_NO_ESCAPE	STA		,U+
					DECB
					BNE		DECODE_LOOP

END_OF_DECODER

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




LOADER_START

					ORCC	#$50
					LDU		#$FD00



					LEAX	RS232C_RESET_CMD,PCR
RS232C_RESET_LOOP	MUL
					LDA		,X+
					COMA
					STA		7,U
					BPL		RS232C_RESET_LOOP



					LEAX	YAMAKAWA,PCR

SEND_REQ_CMD_LOOP	INCA
					BNE		SEND_REQ_CMD_LOOP

					LDA		,X+
					BMI		SEND_REQ_CMD_END

WAIT_TXRDY			LDB		7,U
					LSRB
					BCC		WAIT_TXRDY
					STA		6,U
					BRA		SEND_REQ_CMD_LOOP
SEND_REQ_CMD_END


					LDX		#INSTALLER_ADDR

					CLRB
LOAD_LOOP			LDA		#2
					ANDA	7,U
					BEQ		LOAD_LOOP
					LDA		6,U				; 1 byte shorter than "LDA IO_RS232C_DATA"
					STA		,X+
					DECB
					BNE		LOAD_LOOP

					JMP		INSTALLER_ADDR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Keep "YAMAKAWA" immediately before RS232C_RESET_CMD.
YAMAKAWA			FCB 	"YAMAKAWA"				 ; Subsequent $FF will be a stopper.
RS232C_RESET_CMD	FCB		$FF,$FF,$FF,$BF,$B1,$48  ; COM and write to FD07

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ENCODE_START		FCB		END_OF_DECODER-PROGRAM_BEGIN

END_OF_PROGRAM
