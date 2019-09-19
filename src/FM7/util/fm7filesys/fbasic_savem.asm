;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

						; Input
						;	Y		FAT Buffer (152 bytes)
						;	U		Sector Read/Write Buffer (256 bytes)
						;	X		12-byte file-info structure.
						;	[,X]	Pointer to the file name
						;	[2,X]	Pointer to the data
						;	[4,X]	Reserved.  Keep zero.
						;	[5,X]	Drive 0 or 1
						;	[6,X]	Size in bytes <= Will be destroyed
						;	[8,X]	Address where the file should be LOADMed
						;	[10,X]	Exec address
						; Output
						; 	A		F-BASIC Error Code

FSYS_FILE_SAVEM_STACK_SIZE		EQU		20
FSYS_FILE_SAVEM_FILE_INFO		EQU		FSYS_FILE_SAVEM_STACK_SIZE+1
FSYS_FILE_SAVEM_FAT_BUFFER		EQU		FSYS_FILE_SAVEM_STACK_SIZE+3
FSYS_FILE_SAVEM_SECTOR_BUFFER	EQU		FSYS_FILE_SAVEM_STACK_SIZE+5

FSYS_FILE_SAVEM_EXEC_ADDR		EQU		18
FSYS_FILE_SAVEM_LAST_5_BUF		EQU		15
FSYS_FILE_SAVEM_UNUSED_14		EQU		14
FSYS_FILE_SAVEM_DATA_POINTER	EQU		12
FSYS_FILE_SAVEM_BYTE_LEFT		EQU		10
FSYS_FILE_SAVEM_SECTOR_LOADED	EQU		9
FSYS_FILE_SAVEM_BYTE_INTO_SEC	EQU		8
FSYS_FILE_SAVEM_DRIVE			EQU		7

; Structure for FSYS_FAT_NEXT_CLUSTER
FSYS_FILE_SAVEM_SIDE			EQU		6
FSYS_FILE_SAVEM_SECTOR			EQU		5
FSYS_FILE_SAVEM_TRACK			EQU		4
FSYS_FILE_SAVEM_CLUSTER			EQU		3

; Structure for FSYS_DIR_FIND_FILE_ENTRY
FSYS_FILE_SAVEM_DIR_OFFSET		EQU		2
FSYS_FILE_SAVEM_DIR_SIDE		EQU		1
FSYS_FILE_SAVEM_DIR_SECTOR		EQU		0

FSYS_FILE_SAVEM			PSHS	U,Y,X,B
						;	[15,S]	5 byte	Header and Footer
						;	[14,S]	1 byte	Unused.  To match with LOADM
						;	[12,S]	2 bytes	Current Data Pointer
						;	[10,S]	2 bytes	Bytes left	
						;	[9,S]	1 byte	Unused.  To match with LOADM
						;	[8,S]	1 byte	Bytes into sector
						;	[7,S]	1 byte	Drive
						;	[6,S]	1 byte	Current Side
						;	[5,S]	1 byte	Current Sector
						;	[4,S]	1 byte	Current Track
						;	[3,S]	1 byte	Current Cluster
						;	[2,S]	1 byte	Directory Offset from Sector Top
						;	[1,S]	1 byte	Directory Side
						;	[,S]	1 byte	Directory Sector


						LEAS	-FSYS_FILE_SAVEM_STACK_SIZE,S

						LDB		5,X
						STB		FSYS_FILE_SAVEM_DRIVE,S		; Remember drive.
						LBSR	FSYS_FAT_LOAD
						LBCS	FSYS_FILE_SAVEM_DEVICE_IO_ERROR


						LDX		FSYS_FILE_SAVEM_FILE_INFO,S
						LDD		6,X
						ADDD	#10	; Including 5-byte header and 5-byte footer.

						LDY		FSYS_FILE_SAVEM_FAT_BUFFER,S
						LBSR	FSYS_FAT_RESERVE_CLUSTER_CHAIN_BY_BYTE_COUNT
						LBCS	FSYS_FILE_SAVEM_DISK_FULL

						STA		FSYS_FILE_SAVEM_CLUSTER,S		; First cluster


						LDX		FSYS_FILE_SAVEM_FILE_INFO,S
						LDA		5,X		; Drive
						LDX		,X		; File name
						LEAY	FSYS_FILE_SAVEM_DIR_SECTOR,S
						LDU		FSYS_FILE_SAVEM_SECTOR_BUFFER,S
						LBSR	FSYS_DIR_FIND_FILE_ENTRY
						;	If carry-set && a==0, file-not-found, means it can go ahead.
						LBCC	FSYS_FILE_SAVEM_FILE_ALREADY_EXISTS
						TSTA
						LBNE	FSYS_FILE_SAVEM_DEVICE_IO_ERROR


						LDX		FSYS_FILE_SAVEM_FILE_INFO,S
						LDA		5,X		; Drive
						LEAY	FSYS_FILE_SAVEM_DIR_SECTOR,S
						LDU		FSYS_FILE_SAVEM_SECTOR_BUFFER,S
						LBSR	FSYS_DIR_FIND_EMPTY_ENTRY
						LBCS	FSYS_FILE_SAVEM_DIRECTORY_FULL


						LDX		FSYS_FILE_SAVEM_SECTOR_BUFFER,S
						LDB		2,Y
						ABX

						LDU		[FSYS_FILE_SAVEM_FILE_INFO,S]
						LDB		#8
FSYS_FILE_SAVEM_FILENAME_COPY_LOOP
						LDA		,U+
						STA		,X+
						DECB
						BNE		FSYS_FILE_SAVEM_FILENAME_COPY_LOOP

						LDB		#8
FSYS_FILE_SAVEM_FILENAME_CLEAR_LOOP
						CLR		,X+
						DECB
						BNE		FSYS_FILE_SAVEM_FILENAME_CLEAR_LOOP

						LDD		#$0200						; Machine-Go, Binary
						STD		$0B-$10,X					; X increased by $10 by the above two loops.
						LDA		FSYS_FILE_SAVEM_CLUSTER,S	; First cluster
						STA		$0E-$10,X					; Therefore subtract $10.



						; Write directory first.  FSYS_FAT_SAVE will destroy sector buffer.
						LDU		FSYS_FILE_SAVEM_SECTOR_BUFFER,S
						LDA		FSYS_FILE_SAVEM_DIR_SIDE,S
						LDB		FSYS_FILE_SAVEM_DRIVE,S
						TFR		D,X
						LDA		#FSYS_DIR_TRACK
						LDB		FSYS_FILE_SAVEM_DIR_SECTOR,S
						LBSR	FSYS_BIOS_WRITESECTOR
						BCS		FSYS_FILE_SAVEM_DEVICE_IO_ERROR

						LDY		FSYS_FILE_SAVEM_FAT_BUFFER,S
						LDB		FSYS_FILE_SAVEM_DRIVE,S
						LBSR	FSYS_FAT_SAVE
						BCS		FSYS_FILE_SAVEM_DEVICE_IO_ERROR



						LEAX	FSYS_FILE_SAVEM_CLUSTER,S
						BSR		FSYS_FILE_WRITE_TO_CLUSTER_CHAIN_BEGIN

						LDX		FSYS_FILE_SAVEM_FILE_INFO,S
						LDU		FSYS_FILE_SAVEM_SECTOR_BUFFER,S
						CLR		,U
						LDD		6,X			; Binary Size
						STD		1,U
						LDD		8,X			; Where it should be loaded.
						STD		3,U
						LDB		#5			; For 5-byte header.
						STB		FSYS_FILE_SAVEM_BYTE_INTO_SEC,S

						LDX		FSYS_FILE_SAVEM_FILE_INFO,S
						LDD		2,X			; Location of the data
						STD		FSYS_FILE_SAVEM_DATA_POINTER,S	; Data pointer for WRITE_TO_CLUSTER_CHAIN
						LDD		6,X
						STD		FSYS_FILE_SAVEM_BYTE_LEFT,S		; Size left for WRITE_TO_CLUSTER_CHAIN
						LEAX	FSYS_FILE_SAVEM_CLUSTER,S
						LDY		FSYS_FILE_SAVEM_FAT_BUFFER,S
						BSR		FSYS_FILE_WRITE_TO_CLUSTER_CHAIN
						BCS		FSYS_FILE_SAVEM_DEVICE_IO_ERROR


						LDA		#$FF
						STA		FSYS_FILE_SAVEM_LAST_5_BUF,S
						CLR		1+FSYS_FILE_SAVEM_LAST_5_BUF,S
						CLR		2+FSYS_FILE_SAVEM_LAST_5_BUF,S
						LDX		FSYS_FILE_SAVEM_FILE_INFO,S
						LDD		10,X		; Exec
						STD		FSYS_FILE_SAVEM_EXEC_ADDR,S

						LEAX	FSYS_FILE_SAVEM_LAST_5_BUF,S
						STX		FSYS_FILE_SAVEM_DATA_POINTER,S	; Data pointer for WRITE_TO_CLUSTER_CHAIN
						LDD		#5
						STD		FSYS_FILE_SAVEM_BYTE_LEFT,S		; Size left for WRITE_TO_CLUSTER_CHAIN

						LEAX	FSYS_FILE_SAVEM_CLUSTER,S
						BSR		FSYS_FILE_WRITE_TO_CLUSTER_CHAIN
						BCS		FSYS_FILE_SAVEM_DEVICE_IO_ERROR


						BSR		FSYS_FILE_FLUSH_TO_CLUSTER_CHAIN
						BCS		FSYS_FILE_SAVEM_DEVICE_IO_ERROR

						; Carry Clear -> A must be zero.
						FCB		$8E		; Make it LDX	#$????
FSYS_FILE_SAVEM_ERROR_EXIT
						ORCC	#1

FSYS_FILE_SAVEM_EXIT
						LEAS	FSYS_FILE_SAVEM_STACK_SIZE,S
						PULS	U,Y,X,B,PC


FSYS_FILE_SAVEM_FILE_ALREADY_EXISTS
						LDA		#FBASIC_ERROR_FILE_ALREADY_EXISTS
						BRA		FSYS_FILE_SAVEM_ERROR_EXIT
FSYS_FILE_SAVEM_DEVICE_IO_ERROR
						LDA		#FBASIC_ERROR_DEVICE_IO_ERROR
						BRA		FSYS_FILE_SAVEM_ERROR_EXIT
FSYS_FILE_SAVEM_DISK_FULL
						LDA		#FBASIC_ERROR_DISK_FULL
						BRA		FSYS_FILE_SAVEM_ERROR_EXIT
FSYS_FILE_SAVEM_DIRECTORY_FULL
						LDA		#FBASIC_ERROR_DIRECTORY_FULL
						BRA		FSYS_FILE_SAVEM_ERROR_EXIT



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input
;	X		Pointer to the location data
;	[,X]	1 byte	(I) Current Cluster
;	[1,X]	1 byte	(O) Current Track
;	[2,X]	1 byte	(O) Current Sector
;	[3,X]	1 byte	(O) Current Side
;	[4,X]	1 byte	(I) Drive
;	[5,X]	1 byte	(O) Bytes into the sector
;	[6,X]	1 byte	Don't care
;	[7,X]	2 bytes	Don't care
;	[9,X]	2 bytes	Don't care
;
FSYS_FILE_WRITE_TO_CLUSTER_CHAIN_BEGIN
						PSHS	U,X
						TFR		X,U
						LDA		,X
						LBSR	FSYS_FAT_CLUSTER_TO_TRACK_SIDE_SECTOR

						STD		1,U
						TFR		X,D
						STA		3,U	; Don't overwrite drive.

						CLR		5,U

						PULS	X,U,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Input
;	U		Sector Read/Write Buffer (256 bytes)
;	Y		FAT Buffer (152 bytes)
;	X		Pointer to the location data
;	[,X]	1 byte	(IO) Current Cluster
;	[1,X]	1 byte	(IO) Current Track
;	[2,X]	1 byte	(IO) Current Sector
;	[3,X]	1 byte	(IO) Current Side
;	[4,X]	1 byte	(I) Drive
;	[5,X]	1 byte	(IO) Bytes into the sector
;	[6,X]	1 byte	Don't care
;	[7,X]	2 bytes	(IO) Bytes left	
;	[9,X]	2 bytes	(IO) Current Data Pointer
;
FSYS_FILE_WRITE_TO_CLUSTER_CHAIN
						PSHS	X,Y,U


FSYS_FILE_WRITE_TO_CLUSTER_CHAIN_BYTE_LOOP
						LDY		,S

						LDX		4,S
						LDB		5,Y		; Bytes into the sector
						ABX

						LDU		9,Y		; Current Data Pointer

FSYS_FILE_WRITE_TO_CLUSTER_CHAIN_BYTE_INSIDE_LOOP
						LDD		7,Y
						BEQ		FSYS_FILE_WRITE_TO_CLUSTER_CHAIN_EXIT

						SUBD	#1
						STD		7,Y

						LDA		,U+
						STA		,X+

						INC		5,Y		; Bytes into the sector
						BNE		FSYS_FILE_WRITE_TO_CLUSTER_CHAIN_BYTE_INSIDE_LOOP

						STU		9,Y

						; Sector filled.

						LDD		1,Y		; Track Sector
						LDX		3,Y		; Side Drive
						LDU		4,S
						LBSR	FSYS_BIOS_WRITESECTOR

						LDA		2,Y
						INC		2,Y
						ANDA	#7
						BNE		FSYS_FILE_WRITE_TO_CLUSTER_CHAIN_BYTE_LOOP

						; Cluster filled
						LDX		2,S
						LBSR	FSYS_FAT_NEXT_CLUSTER
						BCC		FSYS_FILE_WRITE_TO_CLUSTER_CHAIN_BYTE_LOOP
						; Carry-set means cluster-chain overflow.  Something went bad.


FSYS_FILE_WRITE_TO_CLUSTER_CHAIN_EXIT
						PULS	X,Y,U,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Input
;	Y		FAT Buffer (152 bytes)
;	U		Sector Read/Write Buffer (256 bytes)
;	X		Pointer to the location data
;	[,X]	1 byte	(IO) Current Cluster
;	[1,X]	1 byte	(IO) Current Track
;	[2,X]	1 byte	(IO) Current Sector
;	[3,X]	1 byte	(IO) Current Side
;	[4,X]	1 byte	(I) Drive
;	[5,X]	1 byte	(IO) Bytes into the sector
;	[6,X]	1 byte	Don't care
;	[7,X]	2 bytes	(IO) Bytes left	
;	[9,X]	2 bytes	(IO) Current Data Pointer
FSYS_FILE_FLUSH_TO_CLUSTER_CHAIN
						PSHS	U,Y,X

						LDB		5,X
						BEQ		FSYS_FILE_FLUSH_TO_CLUSTER_CHAIN_EXIT
						CLRA
						LEAU	D,U

FSYS_FILE_FLUSH_TO_CLUSTER_CHAIN_ZERO_LOOP
						CLR		,U+
						INCB
						BNE		FSYS_FILE_FLUSH_TO_CLUSTER_CHAIN_ZERO_LOOP

						LDU		4,S
						LDD		1,X
						LDX		3,X
						LBSR	FSYS_BIOS_WRITESECTOR

FSYS_FILE_FLUSH_TO_CLUSTER_CHAIN_EXIT
						PULS	X,Y,U,PC
