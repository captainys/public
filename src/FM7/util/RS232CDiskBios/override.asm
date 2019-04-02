BIOS_DISK_OVERRIDE_BEGIN



; BIOSCALL_RESTORE			EQU		$FE02
; BIOSCALL_WRITESECTOR		EQU		$FE05
; BIOSCALL_READSECTOR		EQU		$FE08


; Server receive:
;   8-byte BIOS command
; then server replies:
;   Number of sector-bytes (1:128  2:256  4:512  8:1024)
;     Send specified bytes or receive specified bytes
;   BIOS error code


; If called as:
;	JSR		$F17D
;	JSR		[$FBFA]
;	JSR		<$DE
;	JSR		$00DE
; registers need to be saved.

; If called as:
;   JSR		$FE02
;	JSR		$FE05
;	JSR		$FE08
; registers don't need to be saved.  Error code will be in A register.

; In both cases carry must be set in case of error.




; For the first time this program is called, this program will:
;   (1) Install BIOS Hook to $FC00 to $FC7F (address can be customized)
;   (2) Change NOP NOP to PULS A,B,X,Y,U,CC,DP,PC
;   (3) Copy self to $6D00 to $6DFF
;   (4) Read Track 0 Side 0 Sector 1 to $0100
;	(5) Jump to $0100
;
; F-BASIC IPL does read Disk BASIC and then it does:
;		LDX		#$6E00
;		COMA
;		JMP		[$FBFE]
; then F-BASIC ROM will erase 0 to $6DFF and then jump to $6E00.
;
; The server checks presence of LDX #$6E00 and JMP [$FBFE] in the IPL, and alters $6E00 to $6D00 before transmitting.
; With this change, Disk BASIC will call the installer in $6D00 and then jump to $6E00.
; Disk BASIC will read/write sectors from/to RS232C instead of Disk.
;
; The server also changes BIOS call to HOOK call before transmitting.
;
; This mechanism should support any title that does not encrypt the binary and
; reads/writes disks using BIOS and it does not overwrites BIOS HOOK.
;
; Unfortunate situation may be JSR [$FBFA] lies across sectors.


BIOS_ENTRY				EQU		$F17D


BIOS_DISK_OVERRIDE		PSHS	A,B,X,Y,U,CC,DP
						CLR		1,X
						LDA		#$FD
						TFR		A,DP
						LDA		,X
						SUBA	#$08
						; If [,X]==8 (Restore), need to return with carry=0.
						; If A-#8==0, zero=1, carry=0.  Safe.
						BEQ		BIOS_DISK_OVERRIDE_EXIT
						CMPA	#2
						BLS		BIOS_DISK_READ_WRITE
						PULS	A,B,X,Y,U,CC,DP
BIOS_DISK_NOT_DISKCMD	JMP		BIOS_ENTRY
						; This address is updated to [$FBFA] from the installer.
						; Just in case.

BIOS_DISK_READ_WRITE	BSR		FE05_DISK_WRITE_OR_READ
BIOS_DISK_OVERRIDE_EXIT	PULS	A,B,X,Y,U,CC,DP,PC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



FE05_DISK_WRITE_OR_READ
						; Set up >>
						ORCC	#$50


						; Transmitting BIOS command to the server.
						TFR		X,U
						LDY		#16
						BSR		RS232C_WRITE_MULTI	; On Exit B=0


						; Server sends:
						;   128-byte sector  -> 1
						;   256-byte sector  -> 2
						;   512-byte sector  -> 4
						;   1024-byte sector -> 8
						BSR		RS232C_READ			; RS232C_READ doesn't change B.  Therefore B=0
						TFR		D,Y
						; Y is sector size in bytes times 2


						; U needs to be the pointer to the data buffer.
						LDU		2,X
						; Set up <<


						; FM-7 BIOS official definition is $FE05=DWRITE and $FE08=DREAD.
						; Actually $FE05 and $FE08 both jumps to the same address and 
						; re-checks [,X].
						LDA		,X
						CMPA	#9
						BEQ		FE05_DISK_WRITE_LOOP


FE08_DISK_READ_LOOP		BSR		RS232C_READ
						STA		,U+
						LEAY	-2,Y
						BNE		FE08_DISK_READ_LOOP
						FCB		$CE			; Instruction for LDU #xxxx
											; This skips BSR RS232C_WRITE_MULTI
											; Saves one byte compared to BRA FEXX_DISK_END_READ_WRITE

FE05_DISK_WRITE_LOOP	BSR		RS232C_WRITE_MULTI


FEXX_DISK_END_READ_WRITE
						BSR		RS232C_READ	; Receive error code

						COMB	; Force carry=1

						STA		1,X
						BEQ		CLEAR_CARRY_AND_RTS
						RTS		; Return Carry=1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
						; U Data Pointer
						; Y Number of bytes times 2
RS232C_WRITE_MULTI		LDA		,U+
						BSR		RS232C_WRITE
						LEAY	-2,Y
						BNE		RS232C_WRITE_MULTI	; On Exit B=0
						RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RS232C_WRITE
RS232C_WRITE_WAIT		LDB		<$07 ; IO_RS232C_COMMAND
						LSRB
						BCC		RS232C_WRITE
						STA		<$06 ; IO_RS232C_DATA

						; Actual FM77AV40 needed post-write wait.
						; It failed to send third byte and the rest of BIOS command sequence
						; without the post-write wait.

FE02_DISK_RESTORE		; Share code.  Save bytes.
CLEAR_CARRY_AND_RTS		; Wastes 256-wait, but saves one byte.
						CLRB					; Carry=0
POSTWRITEWAIT			DECB					; DECB doesn't change Carry.
						BNE		POSTWRITEWAIT	; On Exit B=0
						; Calling function uses B=0 on exit.  Needs to be BNE.
						; FE02_DISK_RESTORE uses Carry=0 on exit.
						; CLEAR_CARRY_AND_RTS uses Carry=0 on exit.
						RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RS232C_READ				LDA		#2
						ANDA	<$07 ; IO_RS232C_COMMAND
						BEQ		RS232C_READ
						LDA		<$06 ; IO_RS232C_DATA
						RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


BIOS_DISK_OVERRIDE_END
