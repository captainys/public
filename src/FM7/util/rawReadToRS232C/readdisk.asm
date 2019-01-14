					EXPORT	DRIVE
					EXPORT	TRACK
					EXPORT	SIDE
					EXPORT	READTRACK
					EXPORT	SEEKTRACK
					EXPORT	CLEARFORMATINFO
					EXPORT	ANALYZETRACK
					EXPORT	DUMPSECTOR
					EXPORT	RESTORE_DISK

					EXPORT	READ_ADDRMARK
					EXPORT	ADDRMARKADDR
					EXPORT	ADDRMARKUSED


					EXPORT	READSIZE
					EXPORT	RAWREADCHECKSUM
					EXPORT	RAWREADXOR
					EXPORT	DUMPSIZE
					EXPORT	DUMPCHECKSUM
					EXPORT	DUMPXOR
					EXPORT	FORMATINFOSIZE
					EXPORT	SECTORINFOADDR
					EXPORT	SECTORDUMPADDR
					EXPORT	BUFFADDR
					EXPORT	LASTIRQDRQ
					EXPORT	ERRORRETURN



KEYCODE_RETURN		EQU		$0D
KEYCODE_ESCAPE		EQU		$1B

IO_URARAM			EQU		$FD0F
IO_FDC_STAT_CMD		EQU		$FD18
IO_FDC_TRACK		EQU		$FD19
IO_FDC_SECTOR		EQU		$FD1A
IO_FDC_DATA			EQU		$FD1B
IO_FDC_SIDE			EQU		$FD1C
IO_FDC_DRIVE_MOTOR	EQU		$FD1D
IO_FDC_DRQ_IRQ		EQU		$FD1F

FDCCMD_STEPIN		EQU		$5A
FDCCMD_READTRACK	EQU		$E4
FDCCMD_WRITETRACK	EQU		$F4
FDCCMD_SEEK			EQU		$1A		; Verify Flag (Bit2) causes it to fail?
FDCCMD_READSECTOR	EQU		$84		; bit7=Sector Read,  bit4=Multiple Record(what is it?), bit2=30ms delay
									; FM-Techknow pp. 182.  SIDE (H or CHRN) will be checked if bit-1 is on.
									; If not, this command only checks C and R.
									; Therefore, bit-1 should be off for reading a sector with tricked side number.
									; Only track register and sector register must match C and R.
FDCCMD_FORCE_TERMINATE	EQU		#$D0

FDCCMD_READADDRMARK	EQU		$C0

BIOSCALL_RESTORE		EQU		$FE02
BIOSCALL_WRITESECTOR	EQU		$FE05
BIOSCALL_READSECTOR		EQU		$FE08

DISKERR_NOTREADY	EQU		10
DISKERR_WRITEPROTECTED	EQU	11
DISKERR_HARDERROR	EQU		12	; SEEK ERROR, LOST DATA, RECORD NOT FOUND
DISKERR_CRCERROR	EQU		13
DISKERR_DDMARK		EQU		14
DISKERR_TIMEOVER	EQU		15
DISKERR_OVERFLOW	EQU		0x81

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DRIVE				FCB		0				; INPUT
TRACK				FCB		0				; INPUT
SIDE				FCB		0				; INPUT
					FCB		0				; 

READSIZE			FDB		0				; OUTPUT
RAWREADCHECKSUM		FDB		0				; OUTPUT
RAWREADXOR			FCB		0				; OUTPUT

DUMPSIZE			FDB		0				; OUTPUT
DUMPCHECKSUM		FDB		0				; OUTPUT
DUMPXOR				FCB		0

FORMATINFOSIZE		FDB		0				; OUTPUT
SECTORINFOADDR		FDB		$3C00			; INPUT
BUFFADDR			FDB		$8000			; INPUT
RAWREADBARRIER		FDB		$FC00

SECTORDUMPADDR		FDB		$8000			; INPUT (RawRead transmitted then SectorRead. OK to be same)
SECTORDUMPBARRIER	FDB		$FC00

ADDRMARKADDR		FDB		$5800
ADDRMARKUSED		FDB		$0

LASTIRQDRQ			FCB		0				; $180C
LASTFDCSTATE		FCB		0
ERRORRETURN			FCB		0				; $180D

SEEKTRACK			PSHS	A,B,X,Y,U,CC
					CLR		ERRORRETURN,PCR
					ORCC	#$50
					LBSR	MOTOR_ON

					TST		IO_FDC_STAT_CMD
					BMI		SEEKTRACK_NOTREADY

					LBSR	FDC_WAIT_READY

					LDA		TRACK,PCR
					STA		IO_FDC_DATA		; Not IO_FDC_TRACK?
					CLR		IO_FDC_SIDE
					LDA		#FDCCMD_SEEK
					STA		IO_FDC_STAT_CMD

					; Looks like I should get IRQ.
					LBSR	WAIT_FOR_FDC_IRQ

					LDA		IO_FDC_STAT_CMD
					BITA	#$10			; Seek error?
					BNE		SEEKTRACK_HARDERR

					LBSR	FDC_WAIT_READY

					PULS	A,B,X,Y,U,CC,PC

SEEKTRACK_NOTREADY	LDA		#DISKERR_NOTREADY
					STA		ERRORRETURN,PCR
					PULS	A,B,X,Y,U,CC,PC

SEEKTRACK_HARDERR	LDA		#DISKERR_HARDERROR
					STA		ERRORRETURN,PCR
					PULS	A,B,X,Y,U,CC,PC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


RESTORE_DISK		PSHS	A,B,X,Y,U,CC,DP
					LEAX	RESTORE_DISK_CMD,PCR
					LDA		DRIVE,PCR
					STA		7,X
					LDA		#$FD
					TFR		A,DP
					JSR		BIOSCALL_RESTORE
					PULS	A,B,X,Y,U,CC,DP,PC

RESTORE_DISK_CMD	
					FCB		8
					FCB		0			; Error Return
					FCB		0
					FCB		0
					FCB		0
					FCB		0
					FCB		0
					FCB		0			; Input from BASIC.


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


READTRACK			PSHS	A,B,X,Y,U,CC
					CLR		ERRORRETURN,PCR
					ORCC	#$50

					STA		IO_URARAM

					LDA		#1			; Use Palette to test progress
					STA		$FD3F

					LDX		#0
					STX		READSIZE,PCR

					LBSR	MOTOR_ON
					TST		IO_FDC_STAT_CMD
					BMI		READTRACK_NOTREADY

					BSR		SEEKTRACK
					TST		ERRORRETURN,PCR
					BNE		READTRACK_END

					LDA		#2			; Use Palette to test progress
					STA		$FD3F

					LDA		TRACK,PCR
					STA		IO_FDC_TRACK
					LDA		SIDE,PCR
					STA		IO_FDC_SIDE

					LDA		#3			; Use Palette to test progress
					STA		$FD3F

					LDY		BUFFADDR,PCR
					LDA		#FDCCMD_READTRACK
					STA		IO_FDC_STAT_CMD

READTRACK_DATALOOP	LDA		IO_FDC_DRQ_IRQ
					BPL		READTRACK_NODRQ
					LDA		IO_FDC_DATA
					STA		,Y+
					CMPY	RAWREADBARRIER,PCR
					BNE		READTRACK_DATALOOP		

					LDA		#DISKERR_OVERFLOW
					STA		ERRORRETURN,PCR
					BRA		READTRACK_OVERFLOW

READTRACK_NODRQ		BITA	#$40		; IRQ?
					BEQ		READTRACK_DATALOOP

READTRACK_OVERFLOW
					LDA		#4			; Use Palette to test progress
					STA		$FD3F

					TFR		Y,D
					SUBD	BUFFADDR,PCR
					STD		READSIZE,PCR

					LDX		BUFFADDR,PCR
					LDY		READSIZE,PCR
					LBSR	RAWREAD_CALC_CHECKSUM
					STD		RAWREADCHECKSUM,PCR

					LDX		BUFFADDR,PCR
					LDY		READSIZE,PCR
					LBSR	RAWREAD_CALC_XOR
					STA		RAWREADXOR,PCR


READTRACK_END		LDA		#7			; Use Palette to test progress
					STA		$FD3F
					LDA		IO_URARAM
					PULS	A,B,X,Y,U,CC,PC

READTRACK_NOTREADY	LDA		#DISKERR_NOTREADY
					STA		ERRORRETURN,PCR

					LDA		#7			; Use Palette to test progress
					STA		$FD3F
					LDA		IO_URARAM

					PULS	A,B,X,Y,U,CC,PC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


					; Emulate BIOS Call
					; This does not re-seek to the track [4,X].
					; A register will return error code
READSECTOR			PSHS	B,X,Y,U,CC
					ORCC	#$50

					LDA		7,X
					STA		DRIVE,PCR
					LBSR	MOTOR_ON
					TST		IO_FDC_STAT_CMD
					BMI		READSECTOR_NOTREADY

					; Don't touch the track.  Track may be set to a wrong number for copy protection.
					; Just use the same track as the last raw-read.
					; LDA		4,X
					; STA		TRACK,PCR
					; BSR		SEEKTRACK
					; LDA		ERRORRETURN,PCR
					; BNE		READSECTOR_ERROR_END

					LDA		4,X
					STA		IO_FDC_TRACK
					LDA		5,X
					STA		IO_FDC_SECTOR
					LDA		6,X
					STA		IO_FDC_SIDE,PCR

					LDY		2,X

					LDA		#FDCCMD_READSECTOR
					STA		IO_FDC_STAT_CMD

READSECTOR_DATALOOP	LDA		IO_FDC_DRQ_IRQ
					BPL		READSECTOR_NODRQ
					LDA		IO_FDC_DATA
					STA		,Y+
					BRA		READSECTOR_DATALOOP
READSECTOR_NODRQ	BITA	#$40		; IRQ?
					BEQ		READSECTOR_DATALOOP
					LDA		IO_FDC_STAT_CMD
					STA		LASTFDCSTATE,PCR

					PSHS	A
					TFR		Y,D
					SUBD	2,X
					STD		READSIZE,PCR
					PULS	A

					BITA	#$80
					BNE		READSECTOR_NOTREADY;
					; BITA	#$40
					; BNE		READSECTOR_WRITEPROTECTED 	; Not supposed to happen for READSECTOR
					BITA	#$20
					BNE		READSECTOR_DDM
					BITA	#$10
					BNE		READSECTOR_HARDERROR
					BITA	#$08
					BNE		READSECTOR_CRCERROR

					; From BIOS Disassembly:
					;   $FD18=0      -> No error
					;   $FD18 & 0x80 -> Drive Not Ready
					;	$FD18 & 0x40 -> Disk Write Protected
					;   $FD18 & 0x14 -> Seek Error, Lost Data, or Record Not Found
					;	$FD18 & 0x08 -> CRC Error
					;	Otherwise, DDM

					CLRA
					PULS	B,X,Y,U,CC,PC

READSECTOR_NOTREADY
					LDA		#DISKERR_NOTREADY
					BRA		READSECTOR_ERROR_END
READSECTOR_DDM
					LDA		#DISKERR_DDMARK
					BRA		READSECTOR_ERROR_END
READSECTOR_HARDERROR
					LDA		#DISKERR_HARDERROR
					LBSR	FDC_RESET_AFTER_HARDERROR
					BRA		READSECTOR_ERROR_END
READSECTOR_CRCERROR
					LDA		#DISKERR_CRCERROR
					BRA		READSECTOR_ERROR_END
READSECTOR_ERROR_END
					STA		ERRORRETURN,PCR
					PULS	B,X,Y,U,CC,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


MOTOR_ON			PSHS	A
					LDA		DRIVE,PCR
					ANDA	#3
					ORA		#$80
					STA		$FD1D
					BSR		WAIT2000
					PULS	A,PC



WAIT2000			PSHS	A,B
					LDD		#$2000
WAIT2000_LOOP		SUBD	#1
					BNE		WAIT2000_LOOP
					PULS	A,B,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

WAIT_FOR_FDC_IRQ	LDB		#10
WAITIRQ_OUTLOOP		LDY		#$FFFF
WAITIRQ_INLOOP		LDA		IO_FDC_DRQ_IRQ
					STA		LASTIRQDRQ,PCR
					BITA	#$40
					BNE		WAITIRQ_GOTIRQ
					LEAY	-1,Y
					BNE		WAITIRQ_INLOOP
					DECB
					BNE		WAITIRQ_OUTLOOP
					LDA		#$FF
					RTS

WAITIRQ_GOTIRQ		CLRA
					RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


					; All sectors may not be found in one read-track.
					; Try reading multiple times to get the all sectors.
					; So, try:
					;    Clear format info
					;    For several times:
					;        Read Track
					;        Analyze Track
CLEARFORMATINFO
					PSHS	A,B,X
					LDX		SECTORINFOADDR,PCR
					LDA		#$FF
					STA		,X+
					STA		,X+
					STA		,X+
					STA		,X+
					STA		,X+
					LDD		#0
					STD		FORMATINFOSIZE,PCR
					PULS	A,B,X,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

					; Back Up No.1 pp. 142 tells:
					; First A1 may be bit shifted and may become C2
					; GAP                  4E
					; Synchro              00
					; Address Marks
					;   ????               C2 C2 C2 FC
					;   ID Mark            A1 A1 A1 FE (C) (H) (S) (N)  CRC
					;   Data Mark          A1 A1 A1 FB [DATA] CRC
					;   Deleted Data Mark  A1 A1 A1 F8 [DATA] CRC
					; GAP->Synchro->Address Mark->GAP Synchro->Address Mark ....

					; After this function:
					;   SECTORINFOADDR-
					;		C H R N 0, C H R N 0, C H R N 0, .... FF FF FF FF FF

ANALYZETRACK		PSHS	A,B,X,Y,U,CC

					ORCC	#$50
					STA		IO_URARAM

					LDD		BUFFADDR,PCR
					ADDD	READSIZE,PCR
					STD		ANATRA_ENDADDR,PCR

					CLR		ANATRA_C,PCR
					CLR		ANATRA_H,PCR
					CLR		ANATRA_R,PCR
					CLR		ANATRA_N,PCR

					LDY		SECTORINFOADDR,PCR
					LDD		FORMATINFOSIZE,PCR
					LEAY	D,Y



					; Read through RAW Read and add address marks.
					LDU		BUFFADDR,PCR
ANATRA_RAW_LOOP		LBSR		CHECK_ADDRMARK
					BNE		ANATRA_RAW_NOT_ADDRMARK
;IS_ADDRMARK
					LDA		3,U
					CMPA	#$FE
					BEQ		ANATRA_RAW_IDMARK
					CMPA	#$FB
					BEQ		ANATRA_RAW_DATAMARK
					CMPA	#$F8
					BEQ		ANATRA_RAW_DATAMARK ; Actually Deleted Data Mark, but do it like a data mark
					BRA		ANATRA_RAW_NOT_ADDRMARK

ANATRA_RAW_IDMARK	LDD		4,U
					STD		ANATRA_C,PCR
					LDD		6,U
					STD		ANATRA_C+2,PCR
					LEAU	10,U ; A1 A1 A1 FE C H S N CRC CRC
					LBSR		CHECK_SECTOR_ALREADY_EXIST
					BEQ		ANATRA_RAW_NEXT

					LDD		ANATRA_C,PCR
					STD		,Y++
					LDD		ANATRA_C+2,PCR
					STD		,Y++
					CLR		,Y+
					LDD		FORMATINFOSIZE,PCR
					ADDD	#5
					STD		FORMATINFOSIZE,PCR
					BRA		ANATRA_RAW_NEXT


ANATRA_RAW_DATAMARK		LDB		ANATRA_N,PCR
					LDA		#1
					ANDB	#3
					BEQ		ANATRA_RAW_DATAMARK_SIZECALC_DONE
ANATRA_RAW_DATAMARK_SIZECALC
					LSLA
					DECB
					BNE		ANATRA_RAW_DATAMARK_SIZECALC
ANATRA_RAW_DATAMARK_SIZECALC_DONE
					CLRB
					LSRA
					RORB

					LEAU	D,U	; A1 A1 A1 FB [DATA] CRC CRC
					LEAU	5,U

					CLR		ANATRA_C,PCR
					CLR		ANATRA_H,PCR
					CLR		ANATRA_R,PCR
					CLR		ANATRA_N,PCR

ANATRA_RAW_NOT_ADDRMARK	LEAU	1,U
ANATRA_RAW_NEXT		CMPU	ANATRA_ENDADDR,PCR
					BCS		ANATRA_RAW_LOOP



					; Read through address marks and add whatever missing.
					LDU		ADDRMARKADDR,PCR
					LDD		ADDRMARKUSED,PCR
					BEQ		ANATRA_ADRMARK_LOOP_BREAK

ANATRA_ADRMARK_LOOP	PSHS	A,B

					LDD		,U
					STD		ANATRA_C,PCR
					LDD		2,U
					STD		ANATRA_C+2,PCR

					BSR		CHECK_SECTOR_ALREADY_EXIST
					BEQ		ANATRA_ADRMARK_NEXT

					LDD		ANATRA_C,PCR
					STD		,Y++
					LDD		ANATRA_C+2,PCR
					STD		,Y++
					CLR		,Y+
					LDD		FORMATINFOSIZE,PCR
					ADDD	#5
					STD		FORMATINFOSIZE,PCR

ANATRA_ADRMARK_NEXT	PULS	A,B
					LEAU	6,U
					SUBD	#6
					BHI		ANATRA_ADRMARK_LOOP

ANATRA_ADRMARK_LOOP_BREAK



					LDA		#$FF
					STA		,Y+
					STA		,Y+
					STA		,Y+
					STA		,Y+
					STA		,Y+

					LDA		IO_URARAM

					PULS	A,B,X,Y,U,CC,PC

CHECK_ADDRMARK		; LDA		,U            ; Don't bother the first byte.  Randomly shifts.
					; BSR		ISC2orA1
					; BNE		CHECK_ADDRMARK_RTS
					LDA		1,U
					BSR		ISC2orA1
					BNE		CHECK_ADDRMARK_RTS
					LDA		2,U
					BSR		ISC2orA1
CHECK_ADDRMARK_RTS	RTS

ISC2orA1			CMPA	#$C2
					BEQ		ISC2orA1_RTS
					CMPA	#$A1
ISC2orA1_RTS		RTS


CHECK_SECTOR_ALREADY_EXIST
					PSHS	A,B,X,Y
					LDY		FORMATINFOSIZE,PCR
					BEQ		CHECK_SECTOR_ALREADY_EXIST_NOTFOUND
					LDX		SECTORINFOADDR,PCR

CHECK_SECTOR_ALREADY_EXIST_LOOP
					LDD		,X
					CMPD	ANATRA_C,PCR
					BNE		CHECK_SECTOR_ALREADY_EXIST_NEXT
					LDD		2,X
					CMPD	ANATRA_C+2,PCR
					BEQ		CHECK_SECTOR_ALREADY_EXIST_RTS
CHECK_SECTOR_ALREADY_EXIST_NEXT
					LEAX	5,X   ; 5 bytes per entry
					LEAY	-5,Y
					BNE		CHECK_SECTOR_ALREADY_EXIST_LOOP

CHECK_SECTOR_ALREADY_EXIST_NOTFOUND
					CLRA
					INCA	; Clear zero flag

CHECK_SECTOR_ALREADY_EXIST_RTS
					PULS	A,B,X,Y,PC			; Zero-flag=Found    Non-Zero-flag=Not Found

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ANATRA_ENDADDR		FDB		0
ANATRA_C			FCB		0
ANATRA_H			FCB		0
ANATRA_R			FCB		0
ANATRA_N			FCB		0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DUMPSECTOR			PSHS	A,B,X,Y,U,DP
					ORCC	#$50

					LDA		#$FD
					TFR		A,DP
					STA		IO_URARAM


					LDX		SECTORDUMPADDR,PCR
DUMPSECTOR_CLR_LOOP	CLR		,X+
					CMPX	SECTORDUMPBARRIER,PCR
					BNE		DUMPSECTOR_CLR_LOOP


					LDY		SECTORINFOADDR,PCR
					LDU		SECTORDUMPADDR,PCR

					LEAX	SECTOR_READ_CMD,PCR

DUMPSECTOR_LOOP
					LDA		3,Y
					CMPA	#$FF
					BEQ		DUMPSECTOR_LOOPEND

					; Version 2:
					; +0 +1 +2 +3  +4+5            +6           +7           +8 to +15
					; C  H  R  N   ActualReadSize  LastFDCState BIOSErrCode  Zero
					; +16
					; ActualReadSize bytes of data
					LDD		,Y
					STD		,U++		; U=SectorTop+2
					LDD		2,Y
					STD		,U++		; U=SectorTop+4
					LDA		#12
DUMPSECTOR_CLEAR_HEADER_LOOP
					CLR		,U+
					DECA
					BNE		DUMPSECTOR_CLEAR_HEADER_LOOP	; U=SectorTop+16

					STU		2,X
					LEAU	-16,U		; U=SectorTop
					LDA		,Y			; This Read Sector shouldn't SEEKTRACK, but the Track register must be set to C of CHRN.
					STA		4,X
					LDA		2,Y
					STA		5,X
					LDA		SIDE,PCR	; Side needs to be 0 or 1.  FDC will ignore matching H of CHRN, apparently.
					STA		6,X
					LDA		DRIVE,PCR
					STA		7,X

					PSHS	X,Y,U
					; JSR		BIOSCALL_READSECTOR
					LBSR	READSECTOR
					PULS	X,Y,U

					STA		4,Y
					STA		7,U

					LDA		LASTFDCSTATE,PCR
					STA		6,U

					LDD		READSIZE,PCR
					STD		4,U
					LEAU	16,U		; Move to the next sector location

					ADDD	#15			; 16-byte alignment
					ANDB	#$F0		; 16-byte alignment

					LEAU	D,U

					LEAY	5,Y			; Move to the next sector info
					BRA		DUMPSECTOR_LOOP

DUMPSECTOR_LOOPEND
					TFR		U,D
					SUBD	SECTORDUMPADDR,PCR
					STD		DUMPSIZE,PCR


					LDX		SECTORDUMPADDR,PCR
					LDY		DUMPSIZE,PCR
					LBSR	RAWREAD_CALC_CHECKSUM
					STD		DUMPCHECKSUM,PCR

					LDX		SECTORDUMPADDR,PCR
					LDY		DUMPSIZE,PCR
					LBSR	RAWREAD_CALC_XOR
					STA		DUMPXOR,PCR


DUMPSECTOR_EXIT		ANDCC	#$AF
					; Reset the track register.  Sector might be faking it.
					LDA		TRACK,PCR
					STA		IO_FDC_TRACK

					LDA		IO_URARAM
					PULS	A,B,X,Y,U,DP,PC


SECTOR_READ_CMD	FCB		10		; Read  (9 for write)
				FCB		0		; Error Return
				FDB		$5000	; Data buf top
				FCB		0		; Track
				FCB		1		; Sector
				FCB		0		; Side
				FCB		1		; Drive

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

				; X Addr
				; Y Size
RAWREAD_CALC_CHECKSUM
				LDU		#0
				CMPY	#0
				BEQ		RAWREAD_CALC_CHECKSUM_EXIT

				CLRA
RAWREAD_CALC_CHECKSUM_LOOP
				LDB		,X+
				LEAU	D,U
				LEAY	-1,Y
				BNE		RAWREAD_CALC_CHECKSUM_LOOP

RAWREAD_CALC_CHECKSUM_EXIT
				TFR		U,D
				RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RAWREAD_CALC_XOR
				CLRA
				CMPY	#0
				BEQ		RAWREAD_CALC_XOR_EXIT
RAWREAD_CALC_XOR_LOOP
				EORA	,X+
				LEAY	-1,Y
				BNE		RAWREAD_CALC_XOR_LOOP
RAWREAD_CALC_XOR_EXIT
				RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


READ_ADDRMARK	PSHS	A,B,X,U,CC

				CLR		ERRORRETURN,PCR
				ORCC	#$50

				STA		IO_URARAM

				LDX		#0
				STX		ADDRMARKUSED,PCR

				LBSR	MOTOR_ON
				TST		IO_FDC_STAT_CMD
				BMI		READ_ADDRMARK_NOTREADY

				LBSR	SEEKTRACK
				TST		ERRORRETURN,PCR
				BNE		READ_ADDRMARK_END

				LDA		TRACK,PCR
				STA		IO_FDC_TRACK
				LDA		SIDE,PCR
				STA		IO_FDC_SIDE

				; Index flag (Bit1 of $FD18) seems to update only when
				; not readyflag (Bit7 of $FD18) is zero, means FDC is ready.
				; therefore it cannot pick up index while reading an index mark.
				;
				; So, I can count time from index to index, but I need a real-time clock.
				;
				; Nah, never mind.  Just read 256 times and transmit them.
				; Repeated readings should be observed, but that's fine.

				; There seems to be no need for time out.  FDC will time out for me.

				; Clear the $600-bytes addr-mark buffer just in case.
				CLRB
				LDU		ADDRMARKADDR,PCR
				LDX		#0
READ_ADDRMARK_CLEAR_LOOP
				STX		,U++
				STX		,U++
				STX		,U++
				DECB			; Looks like DECB doesn't set carry flag.
				BNE		READ_ADDRMARK_CLEAR_LOOP


				LDB		#$FF
				LDU		ADDRMARKADDR,PCR

READ_ADDRMARK_WAIT_INDEX
				LDA		IO_FDC_STAT_CMD
				ANDA	#2
				BEQ		READ_ADDRMARK_WAIT_INDEX


READ_ADDRMARK_OUTER_LOOP
				LDA		#FDCCMD_READADDRMARK
				STA		IO_FDC_STAT_CMD
				TFR		U,X

READ_ADDRMARK_LOOP
				LDA		IO_FDC_DRQ_IRQ
				BPL		READ_ADDRMARK_NOTDRQ
				LDA		IO_FDC_DATA
				STA		,U+
				BRA		READ_ADDRMARK_LOOP
READ_ADDRMARK_NOTDRQ
				BITA	#$40
				BEQ		READ_ADDRMARK_LOOP

				LEAU	6,X				; Make sure 6-byte incmrent (What if FDC gives up after 4 bytes?)
				LDA		-3,U			; N of CHRN (size-shift)
				ANDA	#3
				INCA
				STA		READ_ADDRMARK_COUNTDOWN+1,PCR
READ_ADDRMARK_COUNTDOWN
				SUBB	#1
				BCC		READ_ADDRMARK_OUTER_LOOP

				TFR		U,D
				SUBD	ADDRMARKADDR,PCR
				STD		ADDRMARKUSED,PCR

READ_ADDRMARK_END
				LDA		IO_URARAM
				PULS	A,B,X,U,CC,PC

READ_ADDRMARK_NOTREADY
				LDA		#DISKERR_NOTREADY
				STA		ERRORRETURN,PCR
				LDA		IO_URARAM
				PULS	A,B,X,U,CC,PC

READ_ADDRMARK_SKIP		FCB		0



FDC_WAIT_READY	LDA		IO_FDC_STAT_CMD
				BITA	#1
				BNE		FDC_WAIT_READY
				RTS



FDC_RESET_AFTER_HARDERROR
				PSHS	A

				BSR		FDC_WAIT_READY
				LDA		#FDCCMD_FORCE_TERMINATE
				STA		IO_FDC_STAT_CMD
				BSR		FDC_WAIT_READY
				LBSR	WAIT2000

				BSR		FDC_WAIT_READY
				LDA		#FDCCMD_FORCE_TERMINATE
				STA		IO_FDC_STAT_CMD
				BSR		FDC_WAIT_READY
				LBSR	WAIT2000

				PULS	A,PC
