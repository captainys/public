
					; Input
					;   X	File name
					;	U	Pointer to the ASCII program
					;	Y	Data size in bytes
					;	A	Drive 0 or 1
					; ASCII BASIC Format
					;   $0d,$0a
					;   "ASCII String",$0d,$0a
					;   "ASCII String",$0d,$0a
					;		:
					;   "ASCII String",$0d,$0a
					;	$1a
					; Output
					;	A	F-BASIC error code
FSYS_FILE_SAVE_ASCII_BASIC
					PSHS	B
					LDB		#0
					STB		FSYS_FILE_SAVE_FILE_TYPE,PCR
					LDB		#$FF
					STB		FSYS_FILE_SAVE_ASC_OR_BIN,PCR
					BSR		FSYS_FILE_SAVE
					PULS	B,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

					; Input
					;   X	File name
					;	U	Pointer to the data
					;	Y	Data size in bytes
					;	A	Drive 0 or 1
					;	[FSYS_FILE_SAVE_FILE_TYPE]	File Type (0:BAS 1:DATA 2:Machine-Go)
					;	[FSYS_FILE_SAVE_ASC_OR_BIN]	Ascii($ff) or Binary(0)
					; Output
					;   A	Error code (F-BASIC)
					;			53 Device I/O Error
					;			72 Drive Not Ready
					;			73 Disk Write Protected
					;			67 Disk Full
					;			65 Directory Full
FSYS_FILE_SAVE
					PSHS	B,X,U

					PSHS	A	; BYTE PTR [2,S] is drive
					PSHS	Y	; WORD PTR [,S] is file size.

					LBSR	FSYS_FAT_LOAD
					LDD		,S
					LBSR	FSYS_FAT_FIND_AVAILABLE_CLUSTER
					TSTA
					BEQ		FSYS_FILE_SAVE_DISK_FULL

					LDA		2,S
					LBSR	FSYS_FILE_RECORD_TO_KILL
					; X is still pointing file name

					LEAY	FSYS_FILE_AVAIL_DIR_TRACK,PCR

					LDA		2,S
					LBSR	FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY
					TSTA
					BNE		FSYS_FILE_SAVE_BIOSERROR
					LDA		,Y
					ORA		1,Y
					ORA		2,Y
					ORA		3,Y
					ORA		4,Y
					BEQ		FSYS_FILE_SAVE_DIRECTORY_FULL

					; X is still pointing file name
					LBSR	FSYS_FAT_RESERVE_CLUSTER_CHAIN

					CLRA
					LDB		2,S
					TFR		D,Y	; Y is drive
					LDD		,S	; D is size

					PSHS	X
					TFR		U,X
					LBSR	FSYS_FAT_WRITE_CLUSTER_CHAIN
					PULS	X
					TSTA
					BNE		FSYS_FILE_SAVE_BIOSERROR

					; Still directory sector must be loaded
					; Populate new entry
					CLRA
					LDB		FSYS_FILE_AVAIL_DIR_OFFSET,PCR
					LDY		#FSYS_DIR_BUFFER
					LEAY	D,Y

					LBSR	FSYS_DIR_CLEAR_ENTRY
					LBSR	FSYS_DIR_COPY_FILE_NAME

					LDA		FSYS_FILE_SAVE_FILE_TYPE,PCR
					STA		$0B,Y		; For F-BASIC
					LDA		FSYS_FILE_SAVE_ASC_OR_BIN,PCR
					STA		$0C,Y		; ASCII
					LDA		FSYS_FAT_CHAINBUFFER,PCR
					STA		$0E,Y		; First cluster

					; Write directory
					LBSR	FSYS_DIR_WRITE_SECTOR

					LDA		2,S
					BSR		FSYS_FILE_PRIVATE_DELETE_FILE

					; Save FAT
					LDA		2,S
					LBSR	FSYS_FAT_SAVE

					CLRA

FSYS_FILE_SAVE_RTS
					PULS	A
					PULS	Y
					PULS	B,X,U,PC
FSYS_FILE_SAVE_BIOSERROR
					BSR		FSYS_FILE_BIOSERROR_TO_FBASICERROR
					BRA		FSYS_FILE_SAVE_RTS
FSYS_FILE_SAVE_DIRECTORY_FULL
					LDA		#FBASIC_ERROR_DIRECTORY_FULL
					BRA		FSYS_FILE_SAVE_RTS
FSYS_FILE_SAVE_DISK_FULL
					LDA		#FBASIC_ERROR_DISK_FULL
					BRA		FSYS_FILE_SAVE_RTS

FSYS_FILE_SAVE_FILE_TYPE	FCB		0	; 0:BASIC	1:DATA	2:Machine-Go
FSYS_FILE_SAVE_ASC_OR_BIN	FCB		0	; $FF:ASCII	0:BINARY

FSYS_FILE_AVAIL_DIR_TRACK	FCB		0
FSYS_FILE_AVAIL_DIR_SIDE	FCB		0
FSYS_FILE_AVAIL_DIR_SECTOR	FCB		0
FSYS_FILE_AVAIL_DIR_OFFSET	FCB		0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

					; Input
					;	A	Drive
					; This function delets a file entry stored in [FSYS_FILE_TOKILL_TRACK,...]
					; and mark FAT clusters unused.
FSYS_FILE_PRIVATE_DELETE_FILE
					PSHS	B,X,Y,U

					LEAX	FSYS_FILE_TOKILL_TRACK,PCR
					LDB		,X+
					ORB		,X+
					ORB		,X+
					ORB		,X+
					BEQ		FSYS_FILE_PRIVATE_DELETE_FILE_RTS

					PSHS	A	; A is still drive.
					LEAY	-4,X
					LBSR	FSYS_DIR_FETCH_DIR
					LDB		3,Y
					LDX		#FSYS_DIR_BUFFER
					ABX
					CLR		,X
					PULS	A
					LBSR	FSYS_DIR_WRITE_SECTOR

					LDA		$0E,X
					LBSR	FSYS_FAT_MAKE_CLUSTER_CHAIN
					TSTA
					BEQ		FSYS_FILE_PRIVATE_DELETE_FILE_RTS

					LEAU	FSYS_FAT_CHAINBUFFER,PCR
FSYS_FILE_PRIVATE_DELETE_FILE_LOOP
					LDB		,U+
					LEAX	FSYS_FAT_BUFFER+5,PCR
					ABX
					LDB		#$FF
					STB		,X
					DECA
					BNE		FSYS_FILE_PRIVATE_DELETE_FILE_LOOP

FSYS_FILE_PRIVATE_DELETE_FILE_RTS
					PULS	B,X,Y,U,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

					; Input
					;	X	File name
					;	A	Drive
FSYS_FILE_RECORD_TO_KILL
					PSHS	A,B,X,Y,U
					LEAY	FSYS_FILE_TOKILL_TRACK,PCR
					; A is already the drive from input
					; X is already the file name from input
					LBSR	FSYS_DIR_FIND_FILE_ENTRY
					PULS	A,B,X,Y,U,PC

FSYS_FILE_TOKILL_TRACK			FCB		0
FSYS_FILE_TOKILL_SIDE			FCB		0
FSYS_FILE_TOKILL_SECTOR			FCB		0
FSYS_FILE_TOKILL_OFFSET			FCB		0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

					; Input
					;	[,U]	Pointer to the file name
					;	[2,U]	Pointer to the data
					;	[4,U]	Reserved.  Keep zero.
					;	[5,U]	Drive 0 or 1
					;	[6,U]	Size in bytes <= Will be destroyed
					;	[8,U]	Address where the file should be LOADMed
					;	[10,U]	Exec address
					; Output
					; 	A		F-BASIC Error Code
FSYS_FILE_SAVEM		PSHS	B,X,Y,U,CC,DP

					LDX		6,U		; Size
					LBEQ	FSYS_FILE_SAVEM_ZEROBYTE

					LBSR	FSYS_FILE_WRITE_CLUSTER_BUFFERING_CLEAR

					LDX		,U		; File name
					LDA		5,U	; Drive
					LBSR	FSYS_FAT_LOAD
					LDD		6,U		; Size
					LBSR	FSYS_FAT_FIND_AVAILABLE_CLUSTER
					TSTA
					LBEQ	FSYS_FILE_SAVEM_DISK_FULL

					LDX		,U
					LDA		5,U	; Drive
					LBSR	FSYS_FILE_RECORD_TO_KILL

					; X is still pointing file name

					LEAY	FSYS_FILE_AVAIL_DIR_TRACK,PCR

					LDA		5,U	; Drive
					LBSR	FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY
					TSTA
					LBNE	FSYS_FILE_SAVEM_BIOSERROR
					LDA		,Y
					ORA		1,Y
					ORA		2,Y
					ORA		3,Y
					ORA		4,Y
					LBEQ	FSYS_FILE_SAVEM_DIRECTORY_FULL

					; X is still pointing file name
					LBSR	FSYS_FAT_RESERVE_CLUSTER_CHAIN


					CLR		FSYS_FAT_CLUSTER_COUNTER,PCR
					CLR		FSYS_FAT_CLUSTER_SECTOR_COUNTER,PCR

					LEAX	FSYS_DISK_SECTOR_BUFFER,PCR
					CLR		,X+
					LDD		6,U	; Size
					STD		,X++
					LDD		8,U	; Store addr
					STD		,X++

					LDB		#5	; 5 bytes are already filled.
					LDX		2,U	; Data buffer
FSYS_FILE_SAVEM_LOOP
					LDY		4,U	; Drive
					LDA		,X+
					BSR		FSYS_FILE_WRITE_CLUSTER_BUFFERING_PUMPBUF
					BNE		FSYS_FILE_SAVEM_RTS
					LDY		6,U
					LEAY	-1,Y
					STY		6,U
					BNE		FSYS_FILE_SAVEM_LOOP

					LDA		#$ff
					BSR		FSYS_FILE_WRITE_CLUSTER_BUFFERING_PUMPBUF
					BNE		FSYS_FILE_SAVEM_RTS
					CLRA
					BSR		FSYS_FILE_WRITE_CLUSTER_BUFFERING_PUMPBUF
					BNE		FSYS_FILE_SAVEM_RTS
					BSR		FSYS_FILE_WRITE_CLUSTER_BUFFERING_PUMPBUF
					BNE		FSYS_FILE_SAVEM_RTS

					LDA		10,U	; EXEC ADDR HIGH
					BSR		FSYS_FILE_WRITE_CLUSTER_BUFFERING_PUMPBUF
					BNE		FSYS_FILE_SAVEM_RTS
					LDA		11,U	; EXEC ADDR LOW
					BSR		FSYS_FILE_WRITE_CLUSTER_BUFFERING_PUMPBUF
					BNE		FSYS_FILE_SAVEM_RTS

					BSR		FSYS_FILE_WRITE_CLUSTER_BUFFERING_FLUSH


					CLRA
					LDB		FSYS_FILE_AVAIL_DIR_OFFSET,PCR
					LDY		#FSYS_DIR_BUFFER
					LEAY	D,Y

					LBSR	FSYS_DIR_CLEAR_ENTRY
					LDX		0,U			; File name
					LBSR	FSYS_DIR_COPY_FILE_NAME

					LDA		#2			; #2 for F-BASIC
					STA		$0B,Y
					CLRA				; #0 for binary
					STA		$0C,Y
					LDA		FSYS_FAT_CHAINBUFFER,PCR
					STA		$0E,Y		; First cluster

					; Write directory
					LDA		5,U			; Drive
					LBSR	FSYS_DIR_WRITE_SECTOR

					LDA		5,U			; Drive
					LBSR	FSYS_FILE_PRIVATE_DELETE_FILE

					; Save FAT
					LDA		5,U			; Drive
					LBSR	FSYS_FAT_SAVE



FSYS_FILE_SAVEM_ZEROBYTE
					CLRA
FSYS_FILE_SAVEM_RTS
					PULS	B,X,Y,U,CC,DP,PC
FSYS_FILE_SAVEM_DISK_FULL
					LDA		#FBASIC_ERROR_DISK_FULL
					BRA		FSYS_FILE_SAVEM_RTS
FSYS_FILE_SAVEM_BIOSERROR
					LBSR	FSYS_FILE_BIOSERROR_TO_FBASICERROR
					BRA		FSYS_FILE_SAVEM_RTS
FSYS_FILE_SAVEM_DIRECTORY_FULL
					LDA		#FBASIC_ERROR_DIRECTORY_FULL
					BRA		FSYS_FILE_SAVEM_RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

					;	Data in [FSYS_DISK_SECTOR_BUFFER]
					;	Y	Drive
					;	B	Buffer used
					;	A	Next byte
					;	Must set:
					;	[FSYS_FAT_CLUSTER_COUNTER]
					;	[FSYS_FAT_CLUSTER_SECTOR_COUNTER]
FSYS_FILE_WRITE_CLUSTER_BUFFERING_PUMPBUF
					PSHS	X
					LEAX	FSYS_DISK_SECTOR_BUFFER,PCR
					ABX
					STA		,X

					CLRA
					INCB
					BNE		FSYS_FILE_WRITE_CLUSTER_BUFFERING_PUMPBUF_RTS

					PSHS	B
					TFR		Y,D
					TFR		B,A
					LEAX	FSYS_DISK_SECTOR_BUFFER,PCR
					LBSR	FSYS_FAT_WRITE_CLUSTER_CHAIN_ONE_SECTOR
					BSR		FSYS_FILE_WRITE_CLUSTER_BUFFERING_CLEAR
					PULS	B

FSYS_FILE_WRITE_CLUSTER_BUFFERING_PUMPBUF_RTS
					TSTA
					PULS	X,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

					; Input
					;	B	Bytes used in the buffer
					;	Y	Drive
FSYS_FILE_WRITE_CLUSTER_BUFFERING_FLUSH
					TSTB
					BEQ		FSYS_FILE_WRITE_CLUSTER_BUFFERING_FLUSH_RTS

					PSHS	A,B,X,Y
					TFR		Y,D
					TFR		B,A
					LEAX	FSYS_DISK_SECTOR_BUFFER,PCR
					LBSR	FSYS_FAT_WRITE_CLUSTER_CHAIN_ONE_SECTOR
					PULS	A,B,X,Y

FSYS_FILE_WRITE_CLUSTER_BUFFERING_FLUSH_RTS
					RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FSYS_FILE_WRITE_CLUSTER_BUFFERING_CLEAR
					PSHS	A,X

					LEAX	FSYS_DISK_SECTOR_BUFFER,PCR
					CLRA
FSYS_FILE_WRITE_CLUSTER_BUFFERING_CLEAR_LOOP
					CLR		,X+
					DECA
					BNE		FSYS_FILE_WRITE_CLUSTER_BUFFERING_CLEAR_LOOP

					PULS	A,X,PC

