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

FSYS_FILE_LOADM			PSHS	U,Y,X,B
						;	[5+FSYS_FILE_LOADM_STACK_SIZE,S]	U
						;	[3+FSYS_FILE_LOADM_STACK_SIZE,S]	Y
						;	[1+FSYS_FILE_LOADM_STACK_SIZE,S]	X
						;	[FSYS_FILE_LOADM_STACK_SIZE,S]	B

						;	[15,S]	5 bytes	Last 5 bytes
						;	[14,S]	1 byte	0:Reading main body   FF:Reading last 5 bytes
						;	[12,S]	2 bytes	Current Data Pointer
						;	[10,S]	2 bytes	Bytes left	
						;	[9,S]	1 byte	# Clusters
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
						STA		7,S
						LEAY	,S
						LDU		FSYS_FILE_LOADM_SECTOR_BUFFER,S
						LBSR	FSYS_DIR_FIND_FILE_ENTRY
						LBCS	FSYS_FILE_LOADM_DEVICE_IO_ERROR_OR_FILE_NOT_FOUND

						; [directory_ptr]		File name pointer
						; [directory_ptr+11]  	File Type (2=Machine Go)
						; [directory_ptr+12]  	Binary or ASCII (0=Binary)
						; [directory_ptr+14]  	First Cluster

						; Check for Maching-Go Binary
						LDB		2,S
						CLRA
						LEAX	D,U
						LDD		$0B,X
						CMPD	#$0200
						LBNE	FSYS_FILE_LOADM_BAD_FILE_MODE

						; First cluster
						LDA		$0E,X
						STA		3,S

						LBSR	FSYS_FAT_CLUSTER_TO_TRACK_SIDE_SECTOR

						STD		4,S		; [4,S]=Track, [5,S]=Sector
						LDB		7,S		; Drive
						ABX				; Low Byte of X=Drive
						STX		6,S
						LDB		5,S		; Recover B=Sector
						LBSR	FSYS_BIOS_READSECTOR

						; First 5 bytes:
						;   +0	1-byte	$00
						;	+1	2-bytes	Binary Size (# of bytes loaded to the RAM)
						;	+3	2-bytes	Start Address
						LDX		FSYS_FILE_LOADM_FILE_INFO,S
						LDD		3,U
						ADDD	2,X		; Offset
						STD		12,S	; Current Data Pointer
						STD		8,X		; Top Address
						LDD		1,U
						STD		10,S	; Bytes left
						STD		6,X		; Size in bytes

						; Already used 5 bytes into the sector data.
						LDA		#5
						STA		8,S

						CLR		14,S	; Reading the main body

						; While(0<word ptr [10,S])
FSYS_FILE_LOADM_BYTE_LEFT_LOOP

						LDX		12,S	; Current data pointer
						LDB		8,S		; Bytes into the sector (#5 only for the first sector)
						CLRA
						LEAY	D,U		; 


FSYS_FILE_LOADM_BYTE_LEFT_INSIDE_LOOP
						LDD		10,S
						BEQ		FSYS_FILE_LOADM_BYTE_LEFT_LOOP_EXIT
						SUBD	#1
						STD		10,S

						LDA		,Y+
						STA		,X+

						INC		8,S
						BNE		FSYS_FILE_LOADM_BYTE_LEFT_INSIDE_LOOP

						STX		12,S

						LDD		10,S
						BEQ		FSYS_FILE_LOADM_BYTE_LEFT_LOOP_EXIT
						; I have a feeling that exception should be handled here.
						; Rather than at the end (with backward GOTO.)


FSYS_FILE_LOADM_BYTE_LEFT_LOOP_EXCEPTION_ENTRY
						; Used up a sector.
						; while(0x100<=bytes_left && not the last sector of the cluster)
						LDU		12,S	; Current Data Pointer

FSYS_FILE_LOADM_SECTOR_LOOP
						LDB		5,S
						INC		5,S
						BITB	#7
						BEQ		FSYS_FILE_LOADM_END_OF_CLUSTER

						LDA		10,S	; Higher-byte of the bytes left
						BEQ		FSYS_FILE_LOADM_NOT_END_OF_CLUSTER

						LDD		4,S
						LDX		6,S
						LBSR	FSYS_BIOS_READSECTOR

						LEAU	$100,U
						STU		12,S
						DEC		10,S
						BRA		FSYS_FILE_LOADM_SECTOR_LOOP



FSYS_FILE_LOADM_END_OF_CLUSTER

						; while(0x800<bytes_left)
FSYS_FILE_LOADM_BURST_READ_MID_CLUSTER_LOOP
						; Next cluster
						LDX		FSYS_FILE_LOADM_FAT_BUFFER,S
						LEAY	3,S
						LBSR	FSYS_FAT_NEXT_CLUSTER
						BCS		FSYS_FILE_LOADM_DEVICE_IO_ERROR

						; Mid-cluster?  If so can I burst-read?
						LDA		10,S	; Higher-byte of the bytes left
						SUBA	#8
						BCS		FSYS_FILE_LOADM_CLUSTER_BURST_READ_END	; Less than $800 bytes left.
						STA		10,S

						LDB		,Y	; 	Cluster
						LDA		7,S	; 	Drive
						LDY		FSYS_FILE_LOADM_FAT_BUFFER,S
						LBSR	FSYS_READ_CLUSTER
						STU		12,S

						BRA		FSYS_FILE_LOADM_BURST_READ_MID_CLUSTER_LOOP
FSYS_FILE_LOADM_CLUSTER_BURST_READ_END



FSYS_FILE_LOADM_NOT_END_OF_CLUSTER
						LDU		FSYS_FILE_LOADM_SECTOR_BUFFER,S
						LDD		4,S
						LDX		6,S
						LBSR	FSYS_BIOS_READSECTOR

						BRA		FSYS_FILE_LOADM_BYTE_LEFT_LOOP



FSYS_FILE_LOADM_BYTE_LEFT_LOOP_EXIT
						LEAX	15,S
						STX		12,S
						LDB		#5		; Word Ptr [10,S] is zero at this point.
						STB		11,S	; Only needs lower byte to be set.

						COM		14,S
						BEQ		FSYS_FILE_LOADM_FINISHED_LAST_5_BYTE

						; End of data and end of sector may come together.
						; In that case, the loop exits with [8,S] (bytes into sector)=0
						; [8,S]==0 in the previous loop means there are 256 bytes of data to look at.
						; But, at this point, [8,S]==0 means the sector buffer is empty.
						TST		8,S
						LBEQ	FSYS_FILE_LOADM_BYTE_LEFT_LOOP_EXCEPTION_ENTRY
						LBRA	FSYS_FILE_LOADM_BYTE_LEFT_LOOP


FSYS_FILE_LOADM_FINISHED_LAST_5_BYTE
						LDX		FSYS_FILE_LOADM_FILE_INFO,S

						; Last 5 bytes
						; +0	3-bytes	$FF,$00,$00
						; +3	2-bytes	EXEC ADDR
						LDD		18,S
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

