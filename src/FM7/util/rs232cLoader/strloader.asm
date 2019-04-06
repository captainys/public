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

INSTALLER_ADDR		EQU		$6000
BINARY_REQ_CMD		EQU		'Y'

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
					LDU		,S
					LDB		#END_OF_PROGRAM-LOADER_START

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
					DECB
					BNE		DECODE_LOOP
					RTS

YAMAKAWA			FCB 	"YAMAKAWA"
					; Decoder part <<



LOADER_START		LDU		#$FD00		; LDU #$FD00 will be used as a marker for the server to encode the binary.
										; must be at the beginning of the loader.

					ORCC	#$50
					LDB		#$B7
					STB		7,U
					MUL

					; Send "Y" to the server, then the server will return installer binary.
WAIT_TXRDY			LDA		7,U
					LSRA
					BCC		WAIT_TXRDY
					LDA		#BINARY_REQ_CMD
					STA		6,U

					LDX		#INSTALLER_ADDR
					CLRB
LOAD_LOOP			LDA		#2
					ANDA	7,U
					BEQ		LOAD_LOOP
					LDA		6,U				; 1 byte shorter than "LDA IO_RS232C_DATA"
					STA		,X+
					DECB
					BNE		LOAD_LOOP

					JMP 	INSTALLER_ADDR

END_OF_PROGRAM
