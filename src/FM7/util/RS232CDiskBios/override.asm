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



; ‚Â‚¢‚Å‚ÉŒ¾‚¤‚ÆF-BASIC‰Šú‰»’†‚ÉFC00`FCFFƒNƒŠƒA‚µ‚Ä‚éB


BIOS_DISK_OVERRIDE		PSHS	A,DP
						LDA		#$FD
						TFR		A,DP
						LDA		,X
						SUBA	#$08
						BEQ		BIOS_DISK_NO_ERROR  ; RESTORE
						DECA
						BEQ		BIOS_DISK_WRITE
						DECA
						BEQ		BIOS_DISK_READ
						PULS	A,DP
						JMP		[$FBFA]

BIOS_DISK_NO_ERROR		PULS	A,DP
						ANDCC	#$FE
						RTS


BIOS_DISK_WRITE			BSR		FE05_DISK_WRITE
						BRA		BIOS_OVERRIDE_EXIT

BIOS_DISK_READ			BSR		FE08_DISK_READ

BIOS_OVERRIDE_EXIT		PULS	A,DP
						RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FE02_DISK_RESTORE		CLRA
						RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FE05_DISK_WRITE			PSHS	B,Y,U,CC

						BSR		SEND_COMMAND_RECEIVE_SIZE

						LDU		2,X
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
						BNE		FEXX_DISK_ERROR

FEXX_DISK_NOERROR		ANDCC	#$FE
						RTS

FEXX_DISK_ERROR			ORCC	#1
						RTS



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



FE08_DISK_READ			PSHS	B,Y,U,CC

						BSR		SEND_COMMAND_RECEIVE_SIZE

						LDU		2,X
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
						PSHS	B
						BSR		RS232C_WRITE	; WRITE will destroy B
						PULS	B
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



RS232C_WRITE			LDB		<$07 ; IO_RS232C_COMMAND
						LSRB
						BCC		RS232C_WRITE
						STA		<$06 ; IO_RS232C_DATA
						RTS



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



RS232C_READ				LDA		#2
						ANDA	<$07 ; IO_RS232C_COMMAND
						BEQ		RS232C_READ
						LDA		<$06 ; IO_RS232C_DATA
						RTS



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



BIOS_DISK_OVERRIDE_END
