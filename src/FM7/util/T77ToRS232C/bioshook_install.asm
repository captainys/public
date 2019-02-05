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

DEF_BRIDGE_ADDRESS		EQU		$FC00+DEF_BRIDGE_OFFSET
DEF_INSTALL_ADDRESS		EQU		DEF_BRIDGE_ADDRESS-(BIOS_HOOK_END-BIOS_HOOK)

BIOS_VECTOR				EQU		$FBFA
DEF_BIOS_ENTRY			EQU		$F17D

INSTALL_ADDRESS_BEGIN	FDB		DEF_INSTALL_ADDRESS
BRIDGE_ADDRESS_BEGIN	FDB		DEF_BRIDGE_ADDRESS



INSTALL_BIOS_HOOK		PSHS	A,B,X,Y,U,CC
						ORCC	#$50

						LDY		#$FD00

						LDX		BIOS_VECTOR
						STX		BRIDGE_FALLBACK+1,PCR
						STX		NO_BRIDGE_FALLBACK+1,PCR

						; Re-enabling RS232C in FM77AV20/40 and newer >>
						LDD		#$0510
						STA		$0C,Y	; IO_RS232C_ENABLE
						STB		$0B,Y	; IO_RS232C_MODE	; Be careful!  The very original FM-7 does not distinguish FD0B and FD0F.  This turns on RAM mode.
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

						CLR		$02,Y	; IO_IRQ_MASK
						STA		$0F,Y	; IO_URARAM


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

						LDA		$0F,Y	; IO_URARAM
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
						PSHS	A,B,X,Y,U,CC			; 2 bytes 4
						ORCC	#$50					; 2 bytes 6
						STA		$FD0F					; 3 bytes 9
BRIDGE_JUMP				JSR		DEF_INSTALL_ADDRESS		; 3 bytes 12
						; U=#$FD00 on return.
						TST		$0F,U					; 3 bytes 15
						BCC		BRIDGE_RTS				; 2 bytes 17

						PULS	A,B,X,Y,U,CC				; 2 bytes 19
BRIDGE_FALLBACK			JMP		DEF_BIOS_ENTRY			; 3 bytes 22

BRIDGE_RTS				PULS	A,B,X,Y,U,CC,PC			; 2 bytes 24
BRIDGE_CODE_END


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; BIOS Audio Cassette functions override.
; To be installed at the back end of URA RAM by default.

NO_BRIDGE				CLR		1,X ; Also clears carry
						PSHS	A,B,X,Y,U,CC
						ORCC	#$50
						BSR		BIOS_HOOK
						BCC		NO_BRIDGE_RTS

						PULS	A,B,X,Y,U,CC
NO_BRIDGE_FALLBACK		JMP		DEF_BIOS_ENTRY

NO_BRIDGE_RTS			PULS	A,B,X,Y,U,CC,PC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; bioshook.asm or bioshook_burst.asm must follow immediately
