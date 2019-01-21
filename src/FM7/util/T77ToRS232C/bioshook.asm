					ORG		$6000


	; F-BASIC Reverse Engineering Manual Phase I pp. 3
	; Three methods of calling BIOS
	;		JSR		$DE		<- Overloadable
	;		JSR		[$FBFA]	<- Loader needs modification
	;		JSR		$F17D	<- Loader needs modification


					BRA		INSTALL_BIOS_HOOK

DEF_INSTALL_ADDRESS		EQU		$FC00-(BIOS_HOOK_END-BIOS_HOOK)
DEF_BRIDGE_ADDRESS		EQU		$FC00

BIOS_VECTOR				EQU		$FBFA
DEF_BIOS_ENTRY			EQU		$F17D

INSTALL_ADDRESS_BEGIN	FDB		DEF_INSTALL_ADDRESS
BRIDGE_ADDRESS_BEGIN	FDB		DEF_BRIDGE_ADDRESS



INSTALL_BIOS_HOOK	LDA		IO_URARAM
					LDX		BIOS_VECTOR
					STX		BRIDGE_DEFAULT+1,PCR

					; Re-enabling RS232C in FM77AV20/40 and newer >>
					LDD		#$0510
					STA		IO_RS232C_ENABLE
					STB		IO_RS232C_MODE	; Be careful!  The very original FM-7 does not distinguish FD0B and FD0F.  This turns on RAM mode.
					; Re-enabling RS232C in FM77AV20/40 and newer <<

					CLR		IO_IRQ_MASK


					STA		IO_URARAM


					LEAX	BIOS_HOOK,PCR
					LDU		INSTALL_ADDRESS_BEGIN,PCR
					STU		BRIDGE_JUMP+1,PCR
					LDB		#BIOS_HOOK_END-BIOS_HOOK
INSTALL_HOOK_LOOP	LDA		,X+
					STA		,U+
					DECB
					BNE		INSTALL_HOOK_LOOP


					LEAX	BRIDGE_CODE,PCR
					LDU		BRIDGE_ADDRESS_BEGIN,PCR
					CMPU	INSTALL_ADDRESS_BEGIN,PCR
					BEQ		DONT_USE_BRIDGE				; If Bridge==Install, then don't use separate bridge.

					LDB		#BRIDGE_CODE_END-BRIDGE_CODE
INSTALL_BRIDGE_LOOP	LDA		,X+
					STA		,U+
					DECB
					BNE		INSTALL_BRIDGE_LOOP
					BRA		INSTALL_EXIT


DONT_USE_BRIDGE		; Modify ORCC #1 RTS to JMP $F17D
					LDX		INSTALL_ADDRESS_BEGIN,PCR
					LDA		HOOK_CODE,PCR		; Instruction for JMP
					STA		BIOS_HOOK_FALLBACK-BIOS_HOOK,X
					LDD		BRIDGE_DEFAULT+1,PCR
					STD		BIOS_HOOK_FALLBACK-BIOS_HOOK+1,X


INSTALL_EXIT
					LDA		HOOK_CODE,PCR
					STA		$DE
					LDX		BRIDGE_ADDRESS_BEGIN,PCR
					STX		$DF

					LDA		IO_URARAM
					RTS



HOOK_CODE			JMP		DEF_BRIDGE_ADDRESS



BRIDGE_CODE			STA		$FD0F				; 3 bytes
BRIDGE_JUMP			JSR		$FB40				; 3 bytes	6
					TST		$FD0F				; 3 bytes	9
					BCC		BRIDGE_CODE_RTS		; 2 bytes	11
BRIDGE_DEFAULT		JMP		DEF_BIOS_ENTRY		; 3 bytes	14
BRIDGE_CODE_RTS		RTS							; 1 byte	15
BRIDGE_CODE_END


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IO_IRQ_MASK			EQU		$FD02

IO_RS232C_DATA		EQU		$FD06
IO_RS232C_COMMAND	EQU		$FD07

IO_RS232C_ENABLE	EQU		$FD0C
IO_RS232C_MODE		EQU		$FD0B

IO_URARAM			EQU		$FD0F



BIOS_HOOK			LDA		,X
					DECA
					BEQ		BIOS_MOTOR
					DECA
					BEQ		BIOS_CTBWRT
					DECA
					BEQ		BIOS_CTBRED
BIOS_HOOK_FALLBACK	ORCC	#1
					RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


BIOS_MOTOR			LDA		2,X
					CLR		1,X
					CMPA	#$FF
					BEQ		MOTOR_ON


MOTOR_OFF			LDA		#$40
					STA		IO_RS232C_COMMAND
					ANDCC	#$FE
					RTS


MOTOR_ON			PSHS	B,X

					LEAX	RS232C_RESET_CMD,PCR
MOTOR_ON_RS232C_RESET_LOOP
					MUL
					LDA		,X+
					STA		IO_RS232C_COMMAND
					BPL		MOTOR_ON_RS232C_RESET_LOOP	; Only last command is negative

					ANDCC	#$FE
					PULS	B,X,PC

RS232C_RESET_CMD	FCB		0,0,0,$40,$4E,$B7


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


BIOS_CTBWRT			ANDCC	#$FE
					PSHS	B,CC
					ORCC	#$50
					LDA		#'W'
					BSR		RS232C_WRITE
					LDA		2,X
					BSR		RS232C_WRITE
					PULS	B,CC,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


BIOS_CTBRED			ANDCC	#$FE
					PSHS	B,CC

					ORCC	#$50
					CLR		1,X

					LDA		#'R'
					BSR		RS232C_WRITE
					BSR		RS232C_READ
					CMPA	#1
					BNE		BIOS_CTBRED_EXIT

					BSR		RS232C_READ
					DECA

					; XM7 emulator cannot receive binary number zero.
					; Need to use #1 as escape.  Taking 6 extra bytes.

BIOS_CTBRED_EXIT
					STA		2,X
					PULS	B,CC,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


RS232C_READ			LDA		IO_RS232C_COMMAND
					BITA	#2
					BEQ		RS232C_READ
					LDA		IO_RS232C_DATA
					RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


RS232C_WRITE		LDB		IO_RS232C_COMMAND
					BITB	#1
					BEQ		RS232C_WRITE
					STA		IO_RS232C_DATA
					RTS

BIOS_HOOK_END

END_OF_PROGRAM
