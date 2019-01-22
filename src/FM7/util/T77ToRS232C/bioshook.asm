						ORG		$6000


	; F-BASIC Reverse Engineering Manual Phase I pp. 3
	; Three methods of calling BIOS
	;		JSR		$DE		<- Overloadable
	;		JSR		[$FBFA]	<- Loader needs modification
	;		JSR		$F17D	<- Loader needs modification


IO_IRQ_MASK				EQU		$FD02

IO_RS232C_DATA			EQU		$FD06
IO_RS232C_COMMAND		EQU		$FD07

IO_RS232C_ENABLE		EQU		$FD0C
IO_RS232C_MODE			EQU		$FD0B

IO_URARAM				EQU		$FD0F


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


						BRA		INSTALL_BIOS_HOOK

DEF_INSTALL_ADDRESS		EQU		$FC00-(BIOS_HOOK_END-BIOS_HOOK)
DEF_BRIDGE_ADDRESS		EQU		$FC00

BIOS_VECTOR				EQU		$FBFA
DEF_BIOS_ENTRY			EQU		$F17D

INSTALL_ADDRESS_BEGIN	FDB		DEF_INSTALL_ADDRESS
BRIDGE_ADDRESS_BEGIN	FDB		DEF_BRIDGE_ADDRESS



INSTALL_BIOS_HOOK		LDA		IO_URARAM
						LDX		BIOS_VECTOR
						STX		BRIDGE_FALLBACK+1,PCR

						; Re-enabling RS232C in FM77AV20/40 and newer >>
						LDD		#$0510
						STA		IO_RS232C_ENABLE
						STB		IO_RS232C_MODE	; Be careful!  The very original FM-7 does not distinguish FD0B and FD0F.  This turns on RAM mode.
						; Re-enabling RS232C in FM77AV20/40 and newer <<

						; Clear F-BASIC COMn IRQ handler >>
						; Make RS232C invisible from F-BASIC.  This program will take over.
						; Ref: F-BASIC Reverse Engineering Manual I, Shuwa System Trading Inc., pp.291
						LDB		#10
						LDU		#$06F4
CLEAR_FBASIC_LOOP		CLR		,U+
						DECB
						BNE		CLEAR_FBASIC_LOOP
						; Clear F-BASIC COMn IRQ handler <<

						CLR		IO_IRQ_MASK
						STA		IO_URARAM

						LEAX	BIOS_HOOK,PCR
						LDU		INSTALL_ADDRESS_BEGIN,PCR
						STU		BRIDGE_JUMP+1,PCR
						LDB		#BIOS_HOOK_END-BIOS_HOOK
INSTALL_HOOK_LOOP		LDA		,X+
						STA		,U+
						DECB
						BNE		INSTALL_HOOK_LOOP

						LEAX	BRIDGE_CODE,PCR
						LDU		BRIDGE_ADDRESS_BEGIN,PCR
						CMPU	INSTALL_ADDRESS_BEGIN,PCR
						BEQ		DONT_USE_BRIDGE				; If Bridge==Install, then don't use separate bridge.

						LDB		#BRIDGE_CODE_END-BRIDGE_CODE
INSTALL_BRIDGE_LOOP		LDA		,X+
						STA		,U+
						DECB
						BNE		INSTALL_BRIDGE_LOOP
						BRA		INSTALL_EXIT


DONT_USE_BRIDGE			; Modify ORCC #1 RTS to JMP $F17D
						LDX		INSTALL_ADDRESS_BEGIN,PCR
						LDA		HOOK_CODE,PCR		; Instruction for JMP
						STA		BIOS_HOOK_FALLBACK-BIOS_HOOK,X
						LDD		BRIDGE_FALLBACK+1,PCR
						STD		BIOS_HOOK_FALLBACK-BIOS_HOOK+1,X


INSTALL_EXIT
						LDA		HOOK_CODE,PCR
						STA		$DE
						LDX		BRIDGE_ADDRESS_BEGIN,PCR
						STX		$DF

						LDA		IO_URARAM
						RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; BIOS_HOOK for F-BASIC programs.
; To be installed in $00DE to $00E0


HOOK_CODE				JMP		DEF_BRIDGE_ADDRESS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Bridge code for interfacing URA RAM and Main RAM
; To be installed in $FC00 to $FC0F by default.


BRIDGE_CODE				STA		$FD0F				; 3 bytes
BRIDGE_JUMP				JSR		$FB40				; 3 bytes	6
						TST		$FD0F				; 3 bytes	9
						BCC		BRIDGE_CODE_RTS		; 2 bytes	11
BRIDGE_FALLBACK			JMP		DEF_BIOS_ENTRY		; 3 bytes	14
BRIDGE_CODE_RTS			RTS							; 1 byte	15
BRIDGE_CODE_END


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; BIOS Audio Cassette functions override.
; To be installed at the back end of URA RAM by default.


BIOS_HOOK				ANDCC	#$FE
						PSHS	A,B,X,U,CC
						ORCC	#$50
						LDU		#$FD00
						LDA		,X
						DECA
						BEQ		BIOS_MOTOR
						DECA
						BEQ		BIOS_CTBWRT
						DECA
						BEQ		BIOS_CTBRED
						PULS	A,B,X,U,CC
BIOS_HOOK_FALLBACK		ORCC	#1		; These two lines will be changed to JSR $F17D by the installer
						RTS				; when the bridge is not used.


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

						; If [2,X] is $FF, turn on motor, turn off otherwise.
						; When motor is turned off, reset RS232C so that it won't fire IRQ any more.
BIOS_MOTOR				LDA		2,X
						INCA
						BNE		BIOS_MOTOR_OFF

BIOS_MOTOR_ON			LEAX	RS232C_RESET_CMD,PCR
MOTOR_RS232C_RESET_LOOP
						MUL
						LDA		,X+
						STA		7,U ; IO_RS232C_COMMAND
						BPL		MOTOR_RS232C_RESET_LOOP	; Only last command is negative
						PULS	A,B,X,U,CC,PC

RS232C_RESET_CMD		FCB		0,0,0,$40,$4E,$B7

BIOS_MOTOR_OFF			CLR		2,U ; Re-clear IRQ
						CLR		7,U ; IO_RS232C_COMMAND
						PULS	A,B,X,U,CC,PC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


BIOS_CTBWRT				LDA		#'W'
						BSR		RS232C_WRITE
						LDA		2,X
						BSR		RS232C_WRITE
						PULS	A,B,X,U,CC,PC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


BIOS_CTBRED				LDA		#'R'
						BSR		RS232C_WRITE
						BSR		RS232C_READ
						CMPA	#1
						BNE		BIOS_CTBRED_EXIT

						BSR		RS232C_READ
						DECA

						; XM7 emulator cannot receive binary number zero.
						; Need to use #1 as escape.  Taking 6 extra bytes.

BIOS_CTBRED_EXIT		STA		2,X
						PULS	A,B,X,U,CC,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


RS232C_READ				LDA		7,U ; IO_RS232C_COMMAND
						BITA	#2
						BEQ		RS232C_READ
						LDA		6,U	; IO_RS232C_DATA
						RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


RS232C_WRITE			LDB		7,U ; IO_RS232C_COMMAND
						LSRB
						BCC		RS232C_WRITE
						STA		6,U ; IO_RS232C_DATA
						RTS

BIOS_HOOK_END

END_OF_PROGRAM
