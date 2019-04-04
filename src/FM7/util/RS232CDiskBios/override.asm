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


BIOS_DISK_OVERRIDE		ANDCC	#$FE
						PSHS	A,B,Y,U,DP,CC
						CLR		1,X
						LDA		#$FD
						TFR		A,DP
						LDA		,X
						SUBA	#$08
						BEQ		BIOS_DISK_OVERRIDE_EXIT
						CMPA	#2
						BLS		BIOS_DISK_READ_WRITE

						; Need to PULS before JMP. BIOS will set Carry flag.
BIOS_DISK_NOT_DISKCMD	JSR		BIOS_ENTRY
						FCB		$CE	; Instruction for LDU #x.  Will skip next BSR.

BIOS_DISK_READ_WRITE	BSR		FE05_DISK_WRITE_OR_READ

						BCC		BIOS_DISK_OVERRIDE_EXIT
						PULS	A	; Previous CC
						ORA		#1
						PSHS	A

BIOS_DISK_OVERRIDE_EXIT	PULS	A,B,Y,U,DP,CC,PC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



FE05_DISK_WRITE_OR_READ
						; Set up >>
						ORCC	#$50


						; Transmitting BIOS command to the server.
						TFR		X,U
						LDY		#16							; Y needs to be 2*bytes.
						BSR		RS232C_WRITE_MULTI_GET_RES	
						; B=0 on exit.
						; A=Server reply. Higher byte of the sector size.


						; Server sends:
						;   128-byte sector  -> 1
						;   256-byte sector  -> 2
						;   512-byte sector  -> 4
						;   1024-byte sector -> 8
						TFR		D,Y
						; Y is sector size in bytes times 2


						; U needs to be the pointer to the data buffer.
						LDU		2,X
						; Set up <<


						; FM-7 BIOS official definition is $FE05=DWRITE and $FE08=DREAD.
						; Actually $FE05 and $FE08 both jumps to the same address and 
						; re-checks [,X].
						LDA		,X
						ADDA	#246
						; IF A=9 (DWRITE), 9+246=255. No Carry.
						; IF A=10(DREAD), 10+246=256. Overflow. Carry set.
						BCC		FE05_DISK_WRITE_LOOP

						; Carry=1

FE08_DISK_READ_LOOP		BSR		RS232C_READ
						STA		,U+
						LEAY	-2,Y
						BNE		FE08_DISK_READ_LOOP
						BSR		RS232C_READ	; Receive error code

						FCB		$CE			; Instruction for LDU #xxxx
											; This skips BSR RS232C_WRITE_MULTI_GET_RES
											; Saves one byte compared to BRA FEXX_DISK_END_READ_WRITE

FE05_DISK_WRITE_LOOP	BSR		RS232C_WRITE_MULTI_GET_RES	
						; B=0 and Carry=1 on exit
						; A=Server Response (Error code)


FEXX_DISK_END_READ_WRITE
						; Carry=1 always.

						STA		1,X
						BNE		FEXX_JUST_RTS

FE02_DISK_RESTORE		; Share code.  Save bytes.
						CLRB	; Carry=0
FEXX_JUST_RTS			RTS	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
						; U Data Pointer
						; Y Number of bytes times 2
RS232C_WRITE_MULTI_GET_RES
						INCB
						BNE		RS232C_WRITE_MULTI_GET_RES
						; First byte may wait less than 256 times.
						; Second and the rest bytes will wait for 256 times.

						; Actual FM77AV40 needed post-write wait.
						; It failed to send third byte and the rest of BIOS command sequence
						; without the post-write wait.

RS232C_WRITE_IOWAIT		LDA		<$07 ; IO_RS232C_COMMAND
						LSRA
						BCC		RS232C_WRITE_IOWAIT
						; Carry=1
						LDA		,U+
						STA		<$06 ; IO_RS232C_DATA

						LEAY	-2,Y
						BNE		RS232C_WRITE_MULTI_GET_RES

						; B=0 and Carry=1 on exit.
						; Always receive one-byte response from the server after sending multi bytes.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RS232C_READ				LDA		#2
						ANDA	<$07 ; IO_RS232C_COMMAND
						BEQ		RS232C_READ
						LDA		<$06 ; IO_RS232C_DATA
						RTS			; No change in Carry

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


BIOS_DISK_OVERRIDE_END
