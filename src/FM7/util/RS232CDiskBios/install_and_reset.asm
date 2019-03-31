
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

						ORG		$6000
						BRA		INSTALL_ENTRY

HOOK_ADDRESS			FDB		$FC00

INSTALL_ENTRY			PSHS	A,B,X,Y,U,CC,DP

						ORCC	#$50

						BSR		RS232C_RESET

						LDB		#BIOS_DISK_OVERRIDE_END-BIOS_DISK_OVERRIDE_BEGIN
						LEAX	BIOS_DISK_OVERRIDE,PCR
						LDU		HOOK_ADDRESS,PCR
HOOK_INSTALL_LOOP		LDA		,X+
						STA		,U+
						DECB
						BNE		HOOK_INSTALL_LOOP


PREVENT_SECOND_RESET	NOP
						NOP


						; The following two lines makes NOP NOP to PULS A,B,X,Y,U,PC
						; After booting to the Disk BASIC, run EXEC &H6000 again to re-install the hook.
						LDX		FOR_SECOND_EXEC,PCR
						STX		PREVENT_SECOND_RESET,PCR

						LEAX	IPL_LOAD_COMMAND,PCR
						LDU		HOOK_ADDRESS,PCR
						STU		BASIC_BIOS_CALL_ADDR
						JSR		,U

						ORCC	#$50
						LDD		HOOK_ADDRESS,PCR
						CMPD	#$8000
						BLE		INSTALLED_LOWER_ADDRESS

						LDS		#$6FFF
						BRA		STACK_POINTER_SET
INSTALLED_LOWER_ADDRESS
						LDS		#$FCFF
STACK_POINTER_SET

						LDD		#0
						LDU		#0
						LDX		#0
						LDY		#0

						JMP		IPL_LOAD_ADDRESS


FOR_SECOND_EXEC			PSHS	A,B,X,Y,U,CC,DP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



RS232C_RESET			LEAX	RS232C_RESET_CMD,PCR
						; According to http://vorkosigan.cocolog-nifty.com/blog/2009/12/a-b085.html
						; Need to wait 8 clocks between writes.
RS232C_RESET_LOOP
						CLRA								; 2 clocks
						LDA		,X+							; 5 clocks
						STA		IO_RS232C_COMMAND
						BPL		RS232C_RESET_LOOP	; Only last command is negative ; 3 clocks

						; CLRA clears carry flag.
						; LDA, STA, and BPL does not change.
						; Can take 10 clocks after each STA 7,U

						RTS

						; 8251A Data Sheet pp.12 'NOTE' paragraph
						; Regarding Internal Reset on Power-up.
RS232C_RESET_CMD		FCB		0,0,0,$40,$4E,$B7



IPL_LOAD_COMMAND		FCB		$0A		; Read
						FCB		0		; Error return
						FDB		$100	; IPL load address
						FCB		0		; Track 0
						FCB		1		; Sector 1
						FCB		0		; Side 0
						FCB		0		; Drive 0
