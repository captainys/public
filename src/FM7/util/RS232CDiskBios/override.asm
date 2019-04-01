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



BIOS_DISK_OVERRIDE		PSHS	A,DP
						LDA		#$FD
						TFR		A,DP
						LDA		,X
						SUBA	#$08
						BEQ		BIOS_DISK_NO_ERROR  ; RESTORE
						CMPA	#2
						BLS		BIOS_DISK_READ_WRITE
						PULS	A,DP
						JMP		[$FBFA]

BIOS_DISK_NO_ERROR		CLR		1,X
						PULS	A,DP,PC


BIOS_DISK_READ_WRITE	BSR		FE05_DISK_WRITE_OR_READ
BIOS_OVERRIDE_EXIT		PULS	A,DP,PC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FE02_DISK_RESTORE
CLRA_AND_THEN_RTS		CLRA
						RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FE05_DISK_WRITE_OR_READ
						; FM-7 BIOS official definition is $FE05=DWRITE and $FE08=DREAD.
						; Actually $FE05 and $FE08 both jumps to the same address and 
						; re-checks [,X].
						LDA		,X
						CMPA	#10
						BEQ		FE08_DISK_READ

						PSHS	B,Y,U,CC
						BSR		FEXX_SETUP

FE05_DISK_WRITE_LOOP
						LDA		,U+
						BSR		RS232C_WRITE
						LEAY	-1,Y
						BNE		FE05_DISK_WRITE_LOOP

						; Fall down to BIOS_DISK_END_READ_WRITE

FEXX_DISK_END_READ_WRITE
						BSR		RS232C_READ	; Receive error code

						PULS	B,Y,U,CC

						STA		1,X
						BEQ		CLRA_AND_THEN_RTS

FEXX_DISK_ERROR			ORCC	#1
						RTS



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



FEXX_SETUP				ORCC	#$50
						BSR		SEND_COMMAND_RECEIVE_SIZE
						LDU		2,X
						RTS



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



FE08_DISK_READ			PSHS	B,Y,U,CC

						BSR		FEXX_SETUP

FE08_DISK_READ_LOOP
						BSR		RS232C_READ
						STA		,U+
						LEAY	-1,Y
						BNE		FE08_DISK_READ_LOOP

						BRA		FEXX_DISK_END_READ_WRITE



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



SEND_COMMAND_RECEIVE_SIZE
						CLRB
SEND_COMMAND_LOOP
						LDA		B,X
						BSR		RS232C_WRITE
						INCB
						ANDB	#7
						BNE		SEND_COMMAND_LOOP
						; B=0 on exitting the loop.

						BSR		RS232C_READ		; READ will preserve B
						; Server must send:
						;   128-byte sector  -> 1
						;   256-byte sector  -> 2
						;   512-byte sector  -> 4
						;   1024-byte sector -> 8
						LSRA
						RORB
						TFR		D,Y
						; Y will be sector size in bytes

						RTS



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



RS232C_WRITE			PSHS	B
RS232C_WRITE_WAIT		LDB		<$07 ; IO_RS232C_COMMAND
						LSRB
						BCC		RS232C_WRITE
						STA		<$06 ; IO_RS232C_DATA

						; Actual FM77AV40 needed post-write wait.
						; It failed to send third byte and the rest of BIOS command sequence
						; without the post-write wait.
						CLRB
WRITEWAIT				DECB
						BNE		WRITEWAIT

						PULS	B,PC



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



RS232C_READ				LDA		#2
						ANDA	<$07 ; IO_RS232C_COMMAND
						BEQ		RS232C_READ
						LDA		<$06 ; IO_RS232C_DATA
						RTS



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



BIOS_DISK_OVERRIDE_END
