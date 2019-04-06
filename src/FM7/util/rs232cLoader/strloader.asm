					; To be stored inside F-BASIC string.
					; The server will encode NEED_ENCODE_START to NEED_ENCODE_END in ASCII format
					;    b -> 0x30+((b&0xF0)>>4) , 0x30+(b&0x0F)

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


					; Decoder part >>
					; No byte can be below #$20
					LEAX	LOADER_START+$20,PCR
					LEAX	-$20,X
					STX		,--S		; Jump by RTS
					PULS	U
					LDB		#-(NEED_ENCODE_END-NEED_ENCODE_START)

DECODE_LOOP			LDA		,X+
					LSLA
					LSLA
					LSLA
					LSLA
					STA		,U
					LDA		,X+
					SUBA	#$30
					ORA		,U
					STA		,U+
					INCB
					BNE		DECODE_LOOP

					LDB		#-(END_OF_PROGRAM-NEED_ENCODE_END)

TFR_LOOP			LDA		,X+
					STA		,U+
					INCB
					BNE		TFR_LOOP

					; Decoder part <<

					; The code that doesn't need to be encoded >>
					LDB		#$B7
					; The code that doesn't need to be encoded <<



					; Need encoding from here. >>
NEED_ENCODE_START

LOADER_START		LDU		#$FD00

					ORCC	#$50
					STB		7,U

					; X is pointing "YAMAKAWA"

SEND_REQ_CMD		INCA
					BNE		SEND_REQ_CMD

WAIT_TXRDY			LDA		7,U
					LSRA
					BCC		WAIT_TXRDY
					LDA		,X+
					BMI		CMD_SENT
					STA		6,U
					BRA		SEND_REQ_CMD
CMD_SENT

					LDX		#INSTALLER_ADDR
					PSHS	X			; Jump by RTS
NEED_ENCODE_END
					; Need encoding above here. <<



					;  Instructions > $20  >>
					CLRB
LOAD_LOOP			CLRA					; Avoid instruction < #$20
					INCA
					INCA
					ANDA	7,U
					BEQ		LOAD_LOOP
					LDA		6,U				; 1 byte shorter than "LDA IO_RS232C_DATA"
					STA		,X+
					DECB
					BNE		LOAD_LOOP

					RTS						; Jump by RTS to INSTALLER_ADDR
					;  Instructions > $20  <<
END_OF_PROGRAM


YAMAKAWA			FCB 	"YAMAKAWA",$FF

					FCB		#NEED_ENCODE_START-PROGRAM_BEGIN
					FCB		#NEED_ENCODE_END-PROGRAM_BEGIN
