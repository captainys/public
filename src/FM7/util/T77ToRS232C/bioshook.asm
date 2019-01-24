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
						STX		NO_BRIDGE_FALLBACK+1,PCR

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


						LDU		INSTALL_ADDRESS_BEGIN,PCR
						CMPU	BRIDGE_ADDRESS_BEGIN,PCR
						BEQ		INSTALL_NO_BRIDGE

						STU		BRIDGE_JUMP+1,PCR
						LEAX	BIOS_HOOK,PCR
						LDB		#BIOS_HOOK_END-BIOS_HOOK
INSTALL_HOOK_LOOP		LDA		,X+
						STA		,U+
						DECB
						BNE		INSTALL_HOOK_LOOP

						LEAX	BRIDGE_CODE,PCR
						LDU		BRIDGE_ADDRESS_BEGIN,PCR
						LDB		#BRIDGE_CODE_END-BRIDGE_CODE
INSTALL_BRIDGE_LOOP		LDA		,X+
						STA		,U+
						DECB
						BNE		INSTALL_BRIDGE_LOOP
						BRA		INSTALL_EXIT



INSTALL_NO_BRIDGE		LEAX	NO_BRIDGE,PCR
						LDB		#BIOS_HOOK_END-NO_BRIDGE

INSTALL_NO_BRIDGE_LOOP	LDA		,X+
						STA		,U+
						DECB
						BNE		INSTALL_NO_BRIDGE_LOOP



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


BRIDGE_CODE				CLR		1,X ; Also clears carry ; 2 bytes
						PSHS	A,B,X,U,CC				; 2 bytes 4
						ORCC	#$50					; 2 bytes 6
						STA		$FD0F					; 3 bytes 9
BRIDGE_JUMP				JSR		DEF_INSTALL_ADDRESS		; 3 bytes 12
						TST		$FD0F					; 3 bytes 15
						BCC		BRIDGE_RTS				; 2 bytes 17

						PULS	A,B,X,U,CC				; 2 bytes 19
BRIDGE_FALLBACK			JMP		DEF_BIOS_ENTRY			; 3 bytes 22

BRIDGE_RTS				PULS	A,B,X,U,CC,PC			; 2 bytes 24
BRIDGE_CODE_END


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; BIOS Audio Cassette functions override.
; To be installed at the back end of URA RAM by default.

NO_BRIDGE				CLR		1,X ; Also clears carry
						PSHS	A,B,X,U,CC
						ORCC	#$50
						BSR		BIOS_HOOK
						BCC		NO_BRIDGE_RTS

						PULS	A,B,X,U,CC
NO_BRIDGE_FALLBACK		JMP		DEF_BIOS_ENTRY

NO_BRIDGE_RTS			PULS	A,B,X,U,CC,PC



BIOS_HOOK				LDU		#$FD00
						LDB		#$B7 ; This will be written to RS232C command
								     ; in BIOS_CTBWRT and BIOS_CTBRED
						LDA		,X
						DECA
						BEQ		BIOS_MOTOR
						DECA
						BEQ		BIOS_CTBWRT
						DECA
						BEQ		BIOS_CTBRED
						ORCC	#1
						RTS


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
						CLRA	; Also clears carry
						RTS

RS232C_RESET_CMD		FCB		0,0,0,$40,$4E,$B7

BIOS_MOTOR_OFF			CLR		2,U ; Re-clear IRQ
						CLR		7,U ; IO_RS232C_COMMAND
						RTS		; Previous CLR 7,U also clears carry


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


BIOS_CTBWRT				STB		7,U ; IO_RS232C_COMMAND
						LDA		#'W'
						BSR		RS232C_WRITE
						LDA		2,X
						BSR		RS232C_WRITE
						CLRA
						RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


BIOS_CTBRED				STB		7,U ; IO_RS232C_COMMAND
						LDA		#'R'
						BSR		RS232C_WRITE
						BSR		RS232C_READ
						CMPA	#1
						BNE		BIOS_CTBRED_EXIT

						BSR		RS232C_READ
						DECA

						; XM7 emulator cannot receive binary number zero.
						; Need to use #1 as escape.  Taking 6 extra bytes.

BIOS_CTBRED_EXIT		STA		2,X
						CLRA
						RTS

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
