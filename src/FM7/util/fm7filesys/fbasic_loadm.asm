						; Input
						;	Y		FAT Buffer (152 bytes)
						;	U		Sector Read Buffer (256 bytes)
						;	X		12-byte file-info structure.
						;	[,X]	2 bytes	Pointer to the file name
						;	[2,X]	2 bytes	Address Offset
						;	[4,X]	1 byte	Reserved.  Keep zero.
						;	[5,X]	1 byte	Drive 0 or 1
						;	[6,X]	
						;	[8,X]	
						;	[10,X]	
						; Output
						;	[,X]	
						;	[2,X]	
						;	[4,X]	
						;	[5,X]	
						;	[6,X]	2 bytes	Size in bytes
						;	[8,X]	2 bytes	Top address of the loaded binary
						;	[10,X]	2 bytes	Exec address
						; 	A		F-BASIC Error Code
FSYS_FILE_LOADM_STACK_SIZE		EQU		20
FSYS_FILE_LOADM_FILE_INFO		EQU		FSYS_FILE_LOADM_STACK_SIZE+1
FSYS_FILE_LOADM_FAT_BUFFER		EQU		FSYS_FILE_LOADM_STACK_SIZE+3
FSYS_FILE_LOADM_SECTOR_BUFFER	EQU		FSYS_FILE_LOADM_STACK_SIZE+5

FSYS_FILE_LOADM_EXEC_ADDR		EQU		18
FSYS_FILE_LOADM_LAST_5_BUF		EQU		15
FSYS_FILE_LOADM_STAGE			EQU		14
FSYS_FILE_LOADM_DATA_POINTER	EQU		12
FSYS_FILE_LOADM_BYTE_LEFT		EQU		10
FSYS_FILE_LOADM_SECTOR_LOADED	EQU		9
FSYS_FILE_LOADM_BYTE_INTO_SEC	EQU		8
FSYS_FILE_LOADM_DRIVE			EQU		7

; Structure for FSYS_FAT_NEXT_CLUSTER
FSYS_FILE_LOADM_SIDE			EQU		6
FSYS_FILE_LOADM_SECTOR			EQU		5
FSYS_FILE_LOADM_TRACK			EQU		4
FSYS_FILE_LOADM_CLUSTER			EQU		3

; Structure for FSYS_DIR_FIND_FILE_ENTRY
FSYS_FILE_LOADM_DIR_OFFSET		EQU		2
FSYS_FILE_LOADM_DIR_SIDE		EQU		1
FSYS_FILE_LOADM_DIR_SECTOR		EQU		0

FSYS_FILE_LOADM			PSHS	U,Y,X,B
						;	[5+FSYS_FILE_LOADM_STACK_SIZE,S]	U
						;	[3+FSYS_FILE_LOADM_STACK_SIZE,S]	Y
						;	[1+FSYS_FILE_LOADM_STACK_SIZE,S]	X
						;	[FSYS_FILE_LOADM_STACK_SIZE,S]	B

						;	[15,S]	5 bytes	Last 5 bytes  [18,S] 2-bytes EXEC address.
						;	[14,S]	1 byte	0:Reading main body   FF:Reading last 5 bytes
						;	[12,S]	2 bytes	Current Data Pointer
						;	[10,S]	2 bytes	Bytes left	
						;	[9,S]	1 byte	Sector_is_loaded flag
						;	[8,S]	1 byte	Bytes into sector
						;	[7,S]	1 byte	Drive
						;	[6,S]	1 byte	Current Side
						;	[5,S]	1 byte	Current Sector
						;	[4,S]	1 byte	Current Track
						;	[3,S]	1 byte	Current Cluster
						;	[2,S]	1 byte	Directory Offset from Sector Top
						;	[1,S]	1 byte	Directory Side
						;	[,S]	1 byte	Directory Sector

						LEAS	-FSYS_FILE_LOADM_STACK_SIZE,S

						LDB		5,X
						LBSR	FSYS_FAT_LOAD
						LBCS	FSYS_FILE_LOADM_DEVICE_IO_ERROR

						LDX		FSYS_FILE_LOADM_FILE_INFO,S
						LDA		5,X		; Drive
						LDX		,X		; File name
						STA		FSYS_FILE_LOADM_DRIVE,S
						LEAY	FSYS_FILE_LOADM_DIR_SECTOR,S
						LDU		FSYS_FILE_LOADM_SECTOR_BUFFER,S
						LBSR	FSYS_DIR_FIND_FILE_ENTRY
						LBCS	FSYS_FILE_LOADM_DEVICE_IO_ERROR_OR_FILE_NOT_FOUND

						; [directory_ptr]		File name pointer
						; [directory_ptr+11]  	File Type (2=Machine Go)
						; [directory_ptr+12]  	Binary or ASCII (0=Binary)
						; [directory_ptr+14]  	First Cluster

						; Check for Maching-Go Binary
						LDB		FSYS_FILE_LOADM_DIR_OFFSET,S
						CLRA
						LEAX	D,U
						LDD		$0B,X
						CMPD	#$0200
						LBNE	FSYS_FILE_LOADM_BAD_FILE_MODE

						; First cluster
						LDA		$0E,X
						STA		FSYS_FILE_LOADM_CLUSTER,S

						LBSR	FSYS_FAT_CLUSTER_TO_TRACK_SIDE_SECTOR

						STD		FSYS_FILE_LOADM_TRACK,S		; [4,S]=Track, [5,S]=Sector
						LDB		FSYS_FILE_LOADM_DRIVE,S
						ABX									; Low Byte of X=Drive
						STX		FSYS_FILE_LOADM_SIDE,S
						LDB		FSYS_FILE_LOADM_SECTOR,S		; Recover B=Sector
						LBSR	FSYS_BIOS_READSECTOR

						; First 5 bytes:
						;   +0	1-byte	$00
						;	+1	2-bytes	Binary Size (# of bytes loaded to the RAM)
						;	+3	2-bytes	Start Address
						LDX		FSYS_FILE_LOADM_FILE_INFO,S
						LDD		3,U
						ADDD	2,X		; Offset
						STD		FSYS_FILE_LOADM_DATA_POINTER,S
						STD		8,X		; Top Address
						LDD		1,U
						STD		FSYS_FILE_LOADM_BYTE_LEFT,S
						STD		6,X		; Size in bytes

						; Already used 5 bytes into the sector data.
						LDA		#5
						STA		FSYS_FILE_LOADM_BYTE_INTO_SEC,S
						STA		FSYS_FILE_LOADM_SECTOR_LOADED,S	; Non-Zero means sector is loaded.

						CLR		FSYS_FILE_LOADM_STAGE,S	; Reading the main body


						;while(0<bytes_left)
						;{
						;	if(0==sector_is_loaded)
						;	{
						;		LoadSector();
						;		sector_is_loade=255;
						;	}
						;	while(0<bytes_left && 0!=(bytes_into_sector&255))
						;	{
						;		CopyOneByte();
						;	}
						;	sector_is_loade=0;
						;	if(0==bytes_left)
						;	{
						;		break;
						;	}
						;	for(;;)
						;	{
						;		MoveToNextSector();
						;		if(true==EndOfCluster())
						;		{
						;			break;
						;		}
						;		if(bytes_left<256)
						;		{
						;			goto NEXT_BYTE;
						;		}
						;		ReadSectorDirectlyToTheDestination();
						;		bytes_left-=256;
						;	}
						;	for(;;)
						;	{
						;		MoveToNextCluster();
						;		if(bytes_left<0x800)
						;		{
						;			break;
						;		}
						;		ReadClusterDirectlyToTheDestination();
						;		bytes_left-=0x800;
						;	}
						;
						;NEXT_BYTE:
						;	;
						;}


						; While(0<word ptr [BYTES_LEFT,S])
FSYS_FILE_LOADM_BYTE_LEFT_LOOP
						; FSYS_FILE_LOADM_SECTOR_LOADED, is needed for:
						; (1) Avoiding to read the first sector twice.
						; (2) When bytes-left and bytes-into-sector become zero together at the end of
						;     loading the main body (before reading the trailing 5 bytes), the loop 
						;     for reading the trailing 5 bytes starts with bytes-into-sector==0,
						;     which appears that a whole sector 256 bytes is available, but actually
						;     the sector was exhausted.
						LDU		FSYS_FILE_LOADM_SECTOR_BUFFER,S

						LDA		FSYS_FILE_LOADM_SECTOR_LOADED,S
						BNE		FSYS_FILE_LOADM_LOADED_SECTOR

						LDD		FSYS_FILE_LOADM_TRACK,S
						LDX		FSYS_FILE_LOADM_SIDE,S
						LBSR	FSYS_BIOS_READSECTOR
						COM		FSYS_FILE_LOADM_SECTOR_LOADED,S	; Non-Zero means sector is loaded.
FSYS_FILE_LOADM_LOADED_SECTOR

						LDX		FSYS_FILE_LOADM_DATA_POINTER,S
						LDB		FSYS_FILE_LOADM_BYTE_INTO_SEC,S		; (#5 only for the first sector)
						CLRA
						LEAY	D,U		; 

FSYS_FILE_LOADM_BYTE_LEFT_INSIDE_LOOP
						LDD		FSYS_FILE_LOADM_BYTE_LEFT,S
						BEQ		FSYS_FILE_LOADM_BYTE_LEFT_LOOP_EXIT
						SUBD	#1
						STD		FSYS_FILE_LOADM_BYTE_LEFT,S

						LDA		,Y+
						STA		,X+
						STX		FSYS_FILE_LOADM_DATA_POINTER,S

						INC		FSYS_FILE_LOADM_BYTE_INTO_SEC,S
						BNE		FSYS_FILE_LOADM_BYTE_LEFT_INSIDE_LOOP

						; Used up a sector.

						CLR		FSYS_FILE_LOADM_SECTOR_LOADED,S	; Zero means sector buffer is exhausted.

						LDD		FSYS_FILE_LOADM_BYTE_LEFT,S
						BEQ		FSYS_FILE_LOADM_BYTE_LEFT_LOOP_EXIT



						; while(0x100<=bytes_left && not the last sector of the cluster)
						LDU		FSYS_FILE_LOADM_DATA_POINTER,S

FSYS_FILE_LOADM_SECTOR_LOOP
						LDB		FSYS_FILE_LOADM_SECTOR,S
						INC		FSYS_FILE_LOADM_SECTOR,S
						BITB	#7
						BEQ		FSYS_FILE_LOADM_NEXT_CLUSTER

						LDA		FSYS_FILE_LOADM_BYTE_LEFT,S	; Higher-byte of the bytes left
						BEQ		FSYS_FILE_LOADM_BYTE_LEFT_LOOP	; Fall back to byte-by-byte reading.

						LDD		FSYS_FILE_LOADM_TRACK,S
						LDX		FSYS_FILE_LOADM_SIDE,S
						LBSR	FSYS_BIOS_READSECTOR

						LEAU	$100,U
						STU		FSYS_FILE_LOADM_DATA_POINTER,S
						DEC		FSYS_FILE_LOADM_BYTE_LEFT,S
						BRA		FSYS_FILE_LOADM_SECTOR_LOOP
FSYS_FILE_LOADM_NEXT_CLUSTER



						; while(0x800<bytes_left)
FSYS_FILE_LOADM_BURST_READ_MID_CLUSTER_LOOP
						; Next cluster
						LDX		FSYS_FILE_LOADM_FAT_BUFFER,S
						LEAY	FSYS_FILE_LOADM_CLUSTER,S
						LBSR	FSYS_FAT_NEXT_CLUSTER
						BCS		FSYS_FILE_LOADM_DEVICE_IO_ERROR

						; Mid-cluster?  If so can I burst-read?
						LDA		FSYS_FILE_LOADM_BYTE_LEFT,S	; Higher-byte of the bytes left
						SUBA	#8
						BCS		FSYS_FILE_LOADM_BYTE_LEFT_LOOP	; Less than $800 bytes left.
						STA		FSYS_FILE_LOADM_BYTE_LEFT,S

						LDB		,Y	; 	Cluster
						LDA		FSYS_FILE_LOADM_DRIVE,S
						LDY		FSYS_FILE_LOADM_FAT_BUFFER,S
						LBSR	FSYS_READ_CLUSTER
						STU		FSYS_FILE_LOADM_DATA_POINTER,S

						BRA		FSYS_FILE_LOADM_BURST_READ_MID_CLUSTER_LOOP



FSYS_FILE_LOADM_BYTE_LEFT_LOOP_EXIT
						LEAX	FSYS_FILE_LOADM_LAST_5_BUF,S
						STX		FSYS_FILE_LOADM_DATA_POINTER,S
						LDB		#5		; Word Ptr [FSYS_FILE_LOADM_BYTE_LEFT,S] is zero at this point.
						STB		1+FSYS_FILE_LOADM_BYTE_LEFT,S	; Only needs lower byte to be set.

						COM		FSYS_FILE_LOADM_STAGE,S
						BEQ		FSYS_FILE_LOADM_FINISHED_LAST_5_BYTE

						LBRA	FSYS_FILE_LOADM_BYTE_LEFT_LOOP



FSYS_FILE_LOADM_FINISHED_LAST_5_BYTE
						LDX		FSYS_FILE_LOADM_FILE_INFO,S

						; Last 5 bytes
						; +0	3-bytes	$FF,$00,$00
						; +3	2-bytes	EXEC ADDR
						LDD		FSYS_FILE_LOADM_EXEC_ADDR,S
						ADDD	2,X
						STD		10,X

						CLRA
FSYS_FILE_LOADM_EXIT
						LEAS	FSYS_FILE_LOADM_STACK_SIZE,S
						PULS	B,X,Y,U,PC

FSYS_FILE_LOADM_DEVICE_IO_ERROR_OR_FILE_NOT_FOUND
						TSTA
						BEQ		FSYS_FILE_LOADM_FILE_NOT_FOUND
FSYS_FILE_LOADM_DEVICE_IO_ERROR
						LDA		#FBASIC_ERROR_DEVICE_IO_ERROR
						FCB		$8C		; Make it COMPX #$86xx
FSYS_FILE_LOADM_FILE_NOT_FOUND
						LDA		#FBASIC_ERROR_FILE_NOT_FOUND
						BRA		FSYS_FILE_LOADM_EXIT
FSYS_FILE_LOADM_BAD_FILE_MODE
						LDA		#FBASIC_ERROR_BAD_FILE_MODE
						BRA		FSYS_FILE_LOADM_EXIT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

