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



INSTALL_BIOS_HOOK		PSHS	A,B,X,Y,U,CC
						ORCC	#$50

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


						LEAX	NO_BRIDGE,PCR
						LDB		#BIOS_HOOK_END-NO_BRIDGE

						LDU		INSTALL_ADDRESS_BEGIN,PCR
						STU		BRIDGE_JUMP+1,PCR
						CMPU	BRIDGE_ADDRESS_BEGIN,PCR
						BEQ		INSTALL_NO_BRIDGE

						LEAX	BIOS_HOOK,PCR
						LDB		#BIOS_HOOK_END-BIOS_HOOK
						BSR		TRANSFER_LOOP

						LEAX	BRIDGE_CODE,PCR
						LDU		BRIDGE_ADDRESS_BEGIN,PCR
						LDB		#BRIDGE_CODE_END-BRIDGE_CODE

INSTALL_NO_BRIDGE		BSR		TRANSFER_LOOP


						LDA		#$7E		; JMP instruction
						STA		$DE
						LDX		BRIDGE_ADDRESS_BEGIN,PCR
						STX		$DF

						LDA		IO_URARAM
						PULS	A,B,X,Y,U,CC,PC

TRANSFER_LOOP			LDA		,X+
						STA		,U+
						DECB
						BNE		TRANSFER_LOOP
						RTS


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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BIOS_HOOK				; Typical way of handling I/O can bd
						; LDA	#$FD
						; TFR	A,DP
						; and then use direct-page access.
						; But, it takes 4 bytes.
						; If I use U register, I can set #$FD00 to U in 3 bytes.

						LDU		#$FD00
						LDB		,X
						DECB
						BEQ		BIOS_MOTOR
						DECB
						BEQ		BIOS_CTBWRT
						DECB
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
						; According to http://vorkosigan.cocolog-nifty.com/blog/2009/12/a-b085.html
						; Need to wait 8 clocks between writes.
MOTOR_RS232C_RESET_LOOP
						CLRA								; 2 clocks
						LDA		,X+							; 5 clocks
						STA		7,U ; IO_RS232C_COMMAND
						BPL		MOTOR_RS232C_RESET_LOOP	; Only last command is negative ; 3 clocks

						; CLRA clears carry flag.
						; LDA, STA, and BPL does not change.
						; Can take 10 clocks after each STA 7,U

						RTS

						; 8251A Data Sheet pp.12 'NOTE' paragraph
						; Regarding Internal Reset on Power-up.
RS232C_RESET_CMD		FCB		0,0,0,$40,$4E,$97

						; Need to make sure RS232C does nothing after MOTOR OFF.
						; "Emergency" (COMPAC) cannot take key inputs unless
						; RS232C is set to do nothing.
BIOS_MOTOR_OFF			
						; Commonly known I/O map tells bit1 of $FD00 means
						;    1: Motor off
						;    0: Motor on
						; which is flipped from actual.  Actual FM-7 write 1 to bit1 of $FD00 to MOTOR-ON.

						; Also, F-BASIC "SAVE" command write $42 outside of BIOS.  Overriding the BIOS
						; does not stop F-BASIC from MOTOR-ON.  Therefore, after loading, it must be set
						; to OFF.  
						; To motor off, $40 needs to be written to $FD00.  Bit6 carries a printer strobe
						; I need that 1 bit to prevent printer confusion.
						; I don't want to do it, but it wastes another 4 bytes.
						LDA		#$40
						STA		,U

						CLR		2,U ; Re-clear IRQ
						CLR		7,U ; IO_RS232C_COMMAND
						RTS		; Previous CLR 7,U also clears carry


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


						; 8251A Data Sheet pp.16 "NOTES" #4
						; Recovery time between writes for asynchronous mode is 8 tCY
						; Probably it is recovery time between sends.
						; May not need wait after setting the status bits.
BIOS_CTBWRT				LDA		#$97
						STA		7,U ; IO_RS232C_COMMAND

						LDA		2,X

RS232C_WRITE_LOOP		LDB		7,U ; IO_RS232C_COMMAND
						LSRB
						BCC		RS232C_WRITE_LOOP
						STA		6,U ; IO_RS232C_DATA

						CLRA
						RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


BIOS_CTBRED				LDA		#$B7	; RTS=ON
						STA		7,U ; IO_RS232C_COMMAND

						LDA		#$82	; 
RS232C_READ_LOOP		BITA	7,U ; IO_RS232C_COMMAND
						BMI		RS232C_READ_LOOP	; Wait for DSR=0
						BEQ		RS232C_READ_LOOP	; Wait for RxRdy
						LDA		6,U	; IO_RS232C_DATA
						STA		2,X

						LDA		#$97
						STA		7,U	; RTS=OFF

WAIT_DSR_ON				TST		7,U
						BPL		WAIT_DSR_ON

						CLRA
						RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BIOS_HOOK_END

END_OF_PROGRAM
