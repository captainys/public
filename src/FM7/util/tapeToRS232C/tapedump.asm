IO_URARAM					EQU		$FD0F
IO_MMR_SWITCH				EQU		$FD93
IO_MMR_SEGMENT				EQU		$FD90
IO_MMR_POINTER				EQU		$FD80
IO_MMR_6000					EQU		$FD86
MMR_MAP_ADDRESS				EQU		$6000
BIOS_VECTOR					EQU		$FBFA



							ORG		$1200
							BRA		FM77AV			; EXEC &H1200
							BRA		FM7				; EXEC &H1202
							LBRA	TRANSMIT_RS232C	; EXEC &H1204

USE_MMR						FCB		1
USE_7000					FCB		0				; POKE &H1208,

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FM7							CLR		USE_MMR,PCR

FM77AV						PSHS	A,B,X,Y,U

							LDA		#$20		; BRA
							STA		USE_OR_NOT_USE_MMR,PCR
							STA		USE_OR_NOT_USE_7000,PCR

							LDA		USE_7000,PCR
							BEQ		SET_NOT_USE_7000
							LDA		#$86		; LDA #??
							STA		USE_OR_NOT_USE_7000,PCR
SET_NOT_USE_7000

							LDA		USE_MMR,PCR
							BEQ		SKIP_MMR_INIT
							LBSR	INIT_MMR
							LDA		#$86		; LDA	#??
							STA		USE_OR_NOT_USE_MMR,PCR

SKIP_MMR_INIT
							LDA		IO_URARAM

							; Y should be BIOS entry address in the rest.
							LDY		BIOS_VECTOR

							LBSR	MOTOR_ON

							; X should be BIOS_TAPE_READ_COMMAND until MOTOR_OFF
							LEAX	BIOS_TAPE_READ_COMMAND,PCR

							LDD		#0
							STD		LAST_ADDRESS,PCR
							STD		SIZE_MMR_USED,PCR

							CLR		IO_MMR_6000
							CLR		LAST_MMR,PCR

							; END_OF_PROGRAM to 6FFF
							LBSR	GET_ADDRESS_ZERO
							LDA		#$70
							BSR		FILL_BYTE


USE_OR_NOT_USE_7000			BRA		SKIP_7000_TO_7FFF	; Will be BRA or LDA #??


							LDU		#$7000
							LDA		#$80
							BSR		FILL_BYTE

SKIP_7000_TO_7FFF
	; Why not 8000 to FC00?
	; Because somewhere in FB00 to FC00 may be used by my BIOS Redirector.
							LDU		#$8000
							LDA		#$FB
							BSR		FILL_BYTE


USE_OR_NOT_USE_MMR			BRA		EXIT_PROGRAM		; Will be BRA or LDA #??


MMR_LOOP					BSR		FILL_BYTE_MMR

							INC		LAST_MMR,PCR
							INC		IO_MMR_6000
							LDA		IO_MMR_6000
							CMPA	#$10
							BCS		MMR_LOOP



							LDA		#$20
							STA		LAST_MMR,PCR
							STA		IO_MMR_6000

MMR_EXT_LOOP				BSR		FILL_BYTE_MMR

							INC		LAST_MMR,PCR
							INC		IO_MMR_6000
							LDA		IO_MMR_6000
							CMPA	#$30
							BCS		MMR_LOOP



EXIT_PROGRAM
							BSR		MOTOR_OFF
							PULS	A,B,X,Y,U,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

							; INPUT		U where to store.
							;           A Higher byte of the (last address+1)
FILL_BYTE					STA		FILL_LOOP_CMPU+2,PCR
FILL_LOOP
							BSR		READ_ONE_BYTE_FROM_TAPE
							STA		IO_URARAM
							; If you are unlucky enough to get FIRQ at this timing, you'll be dead.
							; The probability is extremely small though.
							STA		,U+
							TST		IO_URARAM
							STU		LAST_ADDRESS,PCR
FILL_LOOP_CMPU				CMPU	#$7000
							BNE		FILL_LOOP

							RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FILL_BYTE_MMR				LDU		#MMR_MAP_ADDRESS
FILL_BYTE_MMR_LOOP
							BSR		READ_ONE_BYTE_FROM_TAPE

							LDB		#$80
							STB		IO_MMR_SWITCH
							STA		,U+
							CLR		IO_MMR_SWITCH

							LDD		SIZE_MMR_USED,PCR
							ADDD	#1
							STD		SIZE_MMR_USED,PCR

							CMPU	#MMR_MAP_ADDRESS+$1000
							BNE		FILL_BYTE_MMR_LOOP

							RTS



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

							; Y must be BIOS entry address
							; This destroys X.
MOTOR_ON					LEAX	BIOS_MOTOR_ON_COMMAND,PCR
							JMP		,Y

							; Y must be BIOS entry address
							; This destroys X.
MOTOR_OFF					LEAX	BIOS_MOTOR_OFF_COMMAND,PCR
							JMP		,Y

							; X must point to BIOS_TAPE_READ_COMMAND
							; Y must be BIOS entry address
READ_ONE_BYTE_FROM_TAPE		JSR		,Y
							LDA		2,X
							RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INIT_MMR					CLR		IO_MMR_SWITCH
							CLRA

INIT_MMR_OUTER_LOOP			STA		IO_MMR_SEGMENT
							LDB		#$30
							LDX		#IO_MMR_POINTER

INIT_MMR_INNER_LOOP			STB		,X+
							INCB
							CMPB	#$40
							BNE		INIT_MMR_INNER_LOOP

							INCA
							CMPA	#8
							BNE		INIT_MMR_OUTER_LOOP

							RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

TRANSMIT_RS232C
							PSHS	A,B,X,Y,U,CC

							ORCC	#$50
							LBSR	RS232C_OPEN

							; END_OF_PROGRAM to 7000
							BSR		GET_ADDRESS_ZERO
							LDX		#$7000
							LDY		LAST_ADDRESS,PCR
							BSR		STD_MIN
							LBSR	TRANSMIT_RS232C_AREA
							CMPX	LAST_ADDRESS,PCR
							BCC		END_CONVENTIONAL_RAM



							LDU		#$8000
							LDA		USE_7000,PCR
							BEQ		SKIP_TRANSMIT_7000
							LDU		#$7000
SKIP_TRANSMIT_7000
							LDX		#$FB00
							LDY		LAST_ADDRESS,PCR
							BSR		STD_MIN
							BSR		TRANSMIT_RS232C_AREA



END_CONVENTIONAL_RAM		LDA		USE_MMR,PCR
							BEQ		EXIT_TRANSMIT_RS232C

							LDA		#$80
							STA		IO_MMR_SWITCH
							CLR		IO_MMR_6000

TRANSMIT_MMR_LOOP			LDA		IO_MMR_6000
							CMPA	LAST_MMR,PCR
							BCC		TRANSMIT_MMR_LOOP_DONE

							LDU		#MMR_MAP_ADDRESS
							LDX		#$1000
							BSR		TRANSMIT_RS232C_AREA

							INC		IO_MMR_6000
							LDA		IO_MMR_6000
							CMPA	#$10
							BNE		TRANSMIT_MMR_LOOP

							LDA		#$20			; Need to skip SubCPU RAM Area
							STA		IO_MMR_6000
							BRA		TRANSMIT_MMR_LOOP



TRANSMIT_MMR_LOOP_DONE		; There may be left-over.
							LDD		SIZE_MMR_USED,PCR
							ANDA	#$0F
							CMPD	#0
							BEQ		EXIT_TRANSMIT_RS232C

							LDU		#MMR_MAP_ADDRESS
							TFR		D,X
							BSR		TRANSMIT_RS232C_AREA



EXIT_TRANSMIT_RS232C		LBSR	INIT_MMR
							LDA		IO_URARAM

							LBSR	RS232C_CLOSE

							PULS	A,B,X,Y,U,CC,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

GET_ADDRESS_ZERO			LEAU	END_OF_PROGRAM,PCR
							TFR		U,D
							ADDD	#$FF
							CLRB
							TFR		D,U
							RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

							; Set greater of X,Y in X. Unsigned.
STD_MIN						PSHS	Y
							CMPX	,S
							BCS		STD_MIN_EXIT
							TFR		Y,X
STD_MIN_EXIT				PULS	Y,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

							; U start address
							; X (end address+1)
TRANSMIT_RS232C_AREA		PSHS	A,B,X,Y,U

							PSHS	U
							TFR		X,D
							SUBD	,S++

							STU		RS232C_SEND_BUFFER_BEGIN,PCR
							STD		RS232C_SEND_BUFFER_SIZE,PCR
							LBSR	RS232C_SEND_ASCII

							PULS	A,B,X,Y,U,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BIOS_MOTOR_ON_COMMAND		FCB		1,0,$FF

BIOS_MOTOR_OFF_COMMAND		FCB		1,0,0

BIOS_TAPE_READ_COMMAND		FCB		3,0,0

LAST_ADDRESS				FDB		0

SIZE_MMR_USED				FDB		0
LAST_MMR					FCB		0


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
