
; Server is responsible for appropriately replacing:
;  7EF17D                  JMP             $F17D		-> 7E (Hook Addr)
;  BDF17D                  JSR             $F17D		-> BD (Hook Addr)
;  AD9FFBFA                JSR             [$FBFA]		-> 12 BD (Hook Addr)
;  BDFE02                  JSR             $FE02		-> BD (Hook Addr)
;  BDFE05                  JSR             $FE05		-> BD (Hook Addr)
;  BDFE08                  JSR             $FE08		-> BD (Hook Addr)
;  7EFE02                  JMP             $FE02		-> 7E (Hook Addr)
;  7EFE05                  JMP             $FE05		-> 7E (Hook Addr)
;  7EFE08                  JMP             $FE08		-> 7E (Hook Addr)
;  BD00DE                  JSR             $DE			-> BD (Hook Addr)
;  7E00DE                  JMP             $DE			-> 7E (Hook Addr)

; Question: How can I write HOOK ADDRESS (00DF) after booting into F-BASIC?
; 
;  9DDE                    JSR             <$DE




IPL_LOAD_ADDRESS		EQU		$100
BASIC_BIOS_CALL_ADDR	EQU		$DF
IO_RS232C_COMMAND		EQU		$FD07
IO_URARAM				EQU		$FD0F
BIOS_VECTOR				EQU		$FBFA

						ORG		$6000
PROGRAM_ENTRY			BRA		INSTALL_ENTRY

HOOK_ADDRESS			FDB		$FC00
HOOK_ADDRESS_SECOND		FDB		$FC00

INSTALL_ENTRY			BSR		REAL_INSTALL_ENTRY
						JMP		$6E00  After Booting

FBASIC_INIT_HOOK		STX		FBASIC_INIT_HOOK-2,PCR
						LEAX	PROGRAM_ENTRY,PCR
						JMP		[$FBFE]

REAL_INSTALL_ENTRY		PSHS	A,B,X,Y,U,CC,DP



						LEAX	RS232C_RESET_CMD,PCR
						; According to http://vorkosigan.cocolog-nifty.com/blog/2009/12/a-b085.html
						; Need to wait 8 clocks between writes.
RS232C_RESET_LOOP		
						; I need minimum 8 CPU clocks delay between writes to IO_RS232C_COMMAND.
						; Why not doing something I need to do anyway. >>
						ORCC	#$50
						LDB		#BIOS_DISK_OVERRIDE_END-BIOS_DISK_OVERRIDE_BEGIN
						LDU		HOOK_ADDRESS,PCR
						STU		BASIC_BIOS_CALL_ADDR
						; Why not doing something I need to do anyway. <<

						LDA		,X+
						STA		IO_RS232C_COMMAND
						BPL		RS232C_RESET_LOOP	; Only last command is negative ; 3 clocks


						TST		IO_URARAM
						LDX		BIOS_VECTOR
						STX		BIOS_DISK_NOT_DISKCMD+1,PCR

						LDA		IO_URARAM	; May be altered to STA IO_URARAM from the server.
											; For supporting DOS mode.

						LEAX	BIOS_DISK_OVERRIDE,PCR

HOOK_INSTALL_LOOP		LDA		,X+
						STA		,U+
						DECB
						BNE		HOOK_INSTALL_LOOP


PREVENT_SECOND_RESET	FCB		$86,$FF		; LDA #$FF -> After first installation -> $35, $FF (PULS A,B,X,Y,U,CC,DP,PC)


						LDS		#$FC80
						LDA		HOOK_ADDRESS,PCR
						BPL		STACK_POINTER_SET
						LDS		#$8000
STACK_POINTER_SET

						; The following two lines makes NOP NOP to PULS A,B,X,Y,U,PC
						; After booting to the Disk BASIC, run EXEC &H6000 again to re-install the hook.
						LDA		#$35 ; Instruction  PULS
						STA		PREVENT_SECOND_RESET,PCR

						; In case the second installation after boot needs to be in the
						; different address.
						LDX		HOOK_ADDRESS_SECOND,PCR
						STX		HOOK_ADDRESS,PCR


						LEAX	IPL_LOAD_COMMAND,PCR
						BSR		BIOS_DISK_OVERRIDE		; Read Track 0 Side 0 Sector 1 from RS232C


						LEAX	PROGRAM_ENTRY,PCR
						LDU		#$2000
						LDY		#$4000
						CLRB
CLONE_INSTALLER_LOOP	LDA		,X+
						STA		,U+
						STA		,Y+
						DECB
						BNE		CLONE_INSTALLER_LOOP

						JMP		IPL_LOAD_ADDRESS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IPL_LOAD_COMMAND		FCB		$0A		; Read
						FCB		0		; Error return
						FDB		$100	; IPL load address
						FCB		0		; Track 0
						FCB		1		; Sector 1
						; Need two more zeros.  Shared with RS232C_RESET_CMD
						; FCB		0		; Side 0
						; FCB		0		; Drive 0

						; 8251A Data Sheet pp.12 'NOTE' paragraph
						; Regarding Internal Reset on Power-up.
RS232C_RESET_CMD		FCB		0,0,0,$40,$4E,$B7
