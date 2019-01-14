					EXPORT		FORMAT_DISK

					EXPORT		RESTORE
					EXPORT		STEPIN
					EXPORT		MAKETRACKIMAGE
					EXPORT		WRITETRACK
					EXPORT		TRACK
					EXPORT		SIDE
					EXPORT		TRACKINFOPTR
					EXPORT		TRACKIMAGEPTR
					EXPORT		TRACKIMAGEENDPTR
					EXPORT		ERRORRETURN

KEYCODE_RETURN		EQU		$0D
KEYCODE_ESCAPE		EQU		$1B

IO_FDC_STAT_CMD		EQU		$FD18
IO_FDC_DATA			EQU		$FD1B
IO_FDC_SIDE			EQU		$FD1C
IO_FDC_DRIVE_MOTOR	EQU		$FD1D
IO_FDC_DRQ_IRQ		EQU		$FD1F

FDCCMD_STEPIN		EQU		$5A
FDCCMD_WRITETRACK	EQU		$F4

BIOSCALL_RESTORE	EQU		$FE02





PROGRAM_ENTRY		BRA		FORMAT_DISK		; &H1800
PREFORMAT_RESTORE_ENTRY		BRA		PREFORMAT_RESTORE			; &H1802

FORMAT_DRIVE		FCB		0				; &H1804
FORMAT_TRACKINFO	FDB		$2000


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


PREFORMAT_RESTORE				PSHS	A,B,X,Y,U,DP

					LDA		FORMAT_DRIVE,PCR
					STA		RESTORE_DRIVE,PCR

					LEAX	RESTORECMD,PCR
					CLR		ERRORRETURN,PCR

					; LDA		#$FD
					; TFR		A,DP
					; JSR		BIOSCALL_RESTORE
					; BEQ		RESTORE_NOERROR

					JSR		[$FBFA]
					BCC		PREFORMAT_RESTORE_NOERROR

					INC		ERRORRETURN,PCR
					LEAX	RESTORECMD,PCR		; Is X preserved?
					LDA		1,X
					STA		BIOSERRORCODE,PCR

PREFORMAT_RESTORE_NOERROR		PULS	A,B,X,Y,U,DP,PC



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



FORMAT_DISK			PSHS	A,B,X,Y,U

					LBSR	BIOSINIT

					LDA		FORMAT_DRIVE,PCR
					ADDA	#'0'
					STA		ENTERKEYMSG_STRONG_DRIVE,PCR

					LEAX	ENTERKEYMSG_STRONG,PCR
					LDD		#ENTERKEYMSG_STRONG_END-ENTERKEYMSG_STRONG
					LBSR	PRINT_BY_BIOS

ENTERKEY_WAIT		LEAX	BIOSKEYIN_OUTBUF,PCR
					STX		BIOSKEYIN_PTR,PCR
					LEAX	BIOSKEYIN,PCR
					JSR		[$FBFA]
					LDA		BIOSKEYIN_OUTBUF+1,PCR
					BEQ		ENTERKEY_WAIT
					LDA		BIOSKEYIN_OUTBUF,PCR
					CMPA	#KEYCODE_ESCAPE
					LBEQ	FORMAT_DISK_ABORT
					CMPA	#KEYCODE_RETURN
					BNE		ENTERKEY_WAIT

					BSR		FORMAT_DISK_RUN

					PULS	A,B,X,Y,U,PC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


FORMAT_DISK_RUN		PSHS	A,B,X,Y,U

					LDX		FORMAT_TRACKINFO,PCR
					CLR		NFORMATTED,PCR

					LBSR	PREFORMAT_RESTORE
					LDA		ERRORRETURN,PCR
					BEQ		RESTORE_SUCCESS

					LEAX	RESTORE_ERRORMSG,PCR
					LDD		#RESTORE_ERRORMSG_END-RESTORE_ERRORMSG
					LBSR	PRINT_BY_BIOS

					LDA		BIOSERRORCODE,PCR
					LEAX	BIOSERRMSG_CODE,PCR
					LBSR	XtoA
					LEAX	BIOSERRMSG,PCR
					LDD		#BIOSERRMSG_END-BIOSERRMSG
					BSR		PRINT_BY_BIOS

					BRA		FORMAT_DISK_ABORT


RESTORE_SUCCESS

TRACKLOOP			STX		TRACKINFOPTR,PCR
					LDA		NFORMATTED,PCR
					CLRB
					LSRA
					ROLB
					STA		TRACK,PCR
					STB		SIDE,PCR


					; PRINT Track and Side >>
					PSHS	A,B,X,Y

					LDA		TRACK,PCR
					LEAX	TRACK_DIGIT,PCR
					LBSR	ItoA

					LDA		SIDE,PCR
					ADDA	#'0'
					STA		SIDE_DIGIT,PCR

					LEAX	TRACKMSG,PCR
					LDD		#TRACKMSG_END-TRACKMSG
					BSR		PRINT_BY_BIOS

					PULS	A,B,X,Y
					; PRINT Track and Side <<

					PSHS	X
					LBSR	MAKETRACKIMAGE
					LBSR	WRITETRACK
					PULS	X

					TST		ERRORRETURN,PCR
					BEQ		FORMAT_DISK_NOERROR_FOR_TRACK

					LEAX	ERRORMSG,PCR
					LDD		#ERRORMSG_END-ERRORMSG
					BSR		PRINT_BY_BIOS
					BRA		FORMAT_DISK_ABORT

FORMAT_DISK_NOERROR_FOR_TRACK
					TFR		X,Y
					LDB		,X+
					ABX
					ABX
					ABX
					TST		,X
					BNE		FORMAT_DISK_NEXTTRACK
					TFR		Y,X

FORMAT_DISK_NEXTTRACK
					INC		NFORMATTED,PCR
					LDA		NFORMATTED,PCR
					CMPA	#80
					BEQ		FORMAT_DISK_END

					BITA	#1
					LBNE	TRACKLOOP

					LBSR	STEPIN
					LBRA	TRACKLOOP

FORMAT_DISK_END
FORMAT_DISK_ABORT
					PULS	A,B,X,Y,U,PC



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


					; X String pointer
					; D String length
PRINT_BY_BIOS		STX		BIOSSTRINGOUT_PTR,PCR
					STD		BIOSSTRINGOUT_LEN,PCR
					LEAX	BIOSSTRINGOUT,PCR
					JSR		[$FBFA]
					RTS



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



BIOSINIT			LEAX	BINITCMD,PCR
					JSR		[$FBFA]
					RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


STEPIN				PSHS	A,B,X,Y,U
					ORCC	#$50
					CLR		ERRORRETURN,PCR

					TST		IO_FDC_STAT_CMD		; Checking Drive not ready
					BMI		STEPIN_ERROREND

					LDA		#FDCCMD_STEPIN
STEPIN_READYWAIT	LDB		IO_FDC_STAT_CMD
					LSRB
					BCS		STEPIN_READYWAIT	; Bit0=Busy
					STA		IO_FDC_STAT_CMD

					LBSR	WAIT7B			; I don't understand why it is needed.  But, DFMCD does it.

STEPIN_IRQWAIT		LDB		IO_FDC_DRQ_IRQ
					BITB	#$40
					BEQ		STEPIN_IRQWAIT

					LBSR	WAITA00

					LDB		IO_FDC_STAT_CMD
					BITB	#$98			; Not ready, seek error, CRC error.  Wait, CRC error???
					BNE		STEPIN_ERROREND
					ANDCC	#$AF
					PULS	A,B,X,Y,U,PC

STEPIN_ERROREND		INC		ERRORRETURN,PCR
					ANDCC	#$AF
					PULS	A,B,X,Y,U,PC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


MAKETRACKIMAGE		LDX		TRACKINFOPTR,PCR
					LDY		TRACKIMAGEPTR,PCR
					LDA		#$4E
					LDU		#32
					BSR		FILL

					LDB		,X+			; Number of sectors
					STB		SECTORCOUNT,PCR
					BPL		SECTORLOOP
					LBRA	ENDMAKETRACKIMAGE

SECTORLOOP			LDA		#0
					LDU		#12
					BSR		FILL

					BSR		MAKEIDMARK

					LDA		TRACK,PCR
					STA		,Y+
					LDA		SIDE,PCR
					STA		,Y+
					LDA		,X+			; Sector number
					STA		,Y+
					LDA		,X+			; Size Shift
					STA		,Y+
					STA		SHIFTCOUNT,PCR

					LEAX	1,X			; Skip (Dummy zero)

					LDA		#$F7
					STA		,Y+

					LDA		#$4E
					LDU		#$16		;  Not 16 bytes.  It's #$16 bytes.
					BSR		FILL

					CLRA
					LDU		#12
					BSR		FILL

					BSR		MAKEDATAMARK

					CLRA
					LDB		#128
CALC_ACTUAL_SIZE_IN_BYTE
					LSLB
					ROLA
					DEC		SHIFTCOUNT,PCR
					BNE		CALC_ACTUAL_SIZE_IN_BYTE

					TFR		D,U
					LDA		#$E5		; Why E5?  I don't know.
					BSR		FILL

					LDA		#$F7
					STA		,Y+

					LDA		#$4E
					LDU		#$36		; Not 36 bytes.  It's #$36 bytes.
					BSR		FILL

					DEC		SECTORCOUNT,PCR
					BNE		SECTORLOOP

					LDA		#$4E
					LDU		#$300
					BSR		FILL

ENDMAKETRACKIMAGE	STY		TRACKIMAGEENDPTR,PCR
					RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


					; Fill	A to Y for U bytes
FILL				STA		,Y+
					LEAU	-1,U
					CMPU	#0
					BNE		FILL
					RTS

MAKEIDMARK			LDA		#$F5
					STA		,Y+
					STA		,Y+
					STA		,Y+
					LDA		#$FE
					STA		,Y+
					RTS

MAKEDATAMARK		LDA		#$F5
					STA		,Y+
					STA		,Y+
					STA		,Y+
					LDA		#$FB
					STA		,Y+
					RTS

MAKEDELETEDDATAMARK	
					LDA		#$F5
					STA		,Y+
					STA		,Y+
					STA		,Y+
					LDA		#$F8
					STA		,Y+
					RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; 
; NSECTOR SEC SIZESHIFT SEC SIZESHIFT SEC SIZESHIFT SEC SIZESHIFT....
; If32 sectors
; -> 1 Track=64 bytes
; -> 80 Tracks=5120 bytes  5KB!?

; $2000-$3FFF  Sector info
; $4000-       Track image



WRITETRACK			PSHS	A,B,X,Y,U
					ORCC	#$50
					CLR		ERRORRETURN,PCR

					; DFMCD uses RESTORE command to do this.
					; LDA		FORMAT_DRIVE,PCR
					; ORA		#$80
					; STA		IO_FDC_DRIVE_MOTOR			; Drive & Motor On
					; LBSR	WAIT2000

					LDA		SIDE,PCR
					STA		IO_FDC_SIDE
					LBSR	WAIT2000

					LDA		#FDCCMD_WRITETRACK
					STA		IO_FDC_STAT_CMD

					LDY		TRACKIMAGEPTR,PCR

WRITETRACKLOOP		LDA		,Y+
					CMPY	TRACKIMAGEENDPTR,PCR
					BLO		NOT_UNDERFLOW
					LDA		#$4E			; Try to fill with #$4E  It may help.
NOT_UNDERFLOW

DRQWAITLOOP			LDB		IO_FDC_DRQ_IRQ
					BPL		NOTDRQ
					STA		IO_FDC_DATA
					BRA		WRITETRACKLOOP
NOTDRQ				LSLB
					BPL		DRQWAITLOOP		; Not IRQ (Not DRQ, Not Error, Not End of Track)

					LDB		IO_FDC_STAT_CMD			; Status
					BITB	#$C4			; Drive not ready, Write protected, or Lost data (Maybe didn't write fast enough?)
					BEQ		WRITETRACK_NOERROREND
					INC		ERRORRETURN,PCR

WRITETRACK_NOERROREND
					BSR		WAIT7B			; I don't understand why it is needed.  But, DFMCD does it.
					ANDCC	#$AF
					PULS	A,B,X,Y,U,PC



WAIT7B				LDY		#$007B
WAIT7BLOOP			LEAY	-1,Y
					BNE		WAIT7BLOOP
					RTS

WAITA00				LDY		#$0A00
WAITA00LOOP			LEAY	-1,Y
					BNE		WAITA00LOOP
					RTS

WAIT2000			LDY		#$2000
WAIT2000LOOP		LEAY	-1,Y
					BNE		WAITA00LOOP
					RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


NFORMATTED			FCB		0
SECTORCOUNT			FCB		0
SHIFTCOUNT			FCB		0


TRACK				FCB		0
SIDE				FCB		0
TRACKINFOPTR		FDB		$2000
TRACKIMAGEPTR		FDB		$4000
TRACKIMAGEENDPTR	FDB		$0




