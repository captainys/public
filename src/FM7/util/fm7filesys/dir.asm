

	; Directory starts from track 1 side 0 sector 4, 
	; and continues all the way to the end of track 1 side 1.

	; File List
	; +0B	00 -> BASIC
	;		01 -> Data file?
	;    	02 -> Machine Language?
	; 
	; +0C	FF->ASCII
	; 		00->Binary
	; 
	; +0E	First Cluster
	; 
	; Track 1 Side 0 Sector 4
	; 0000  59 53 46 4C  49 47 48 54  00 00 00 00  FF 00 00 00 YSFLIGHT........
	; 0010  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00 ................
	; BASIC Data Files
	; 0000  48 31 20 20  20 20 20 20  00 00 00 01  FF 00 1B 00 XYZ     ........
	; 0010  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00 ................

FSYS_DIR_TRACK			EQU		1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Input
	;	X		File name pointer (File name is always 8 bytes.  " " must fill if the length<8.)
	;    	   For finding match.
	;   Y		Data-Return pointer (4 bytes)
	;	U		Sector Data Buffer Pointer (256-byte buffer)
	;	[2,S]	2-bytes	Call-Back Function pointer
	;	A		Drive

	; Output
	;	Carry Clear		Found Match
	;   Carry Set		Not Found
	;	A	BIOS Error Code
	;	[Y  ]	byte	Sector
	;	[Y+1]	byte	Side
	;	[Y+2]	byte	Offset from the sector top

FSYS_DIR_PRIVATE_SCAN_DIR_STACK_SIZE	EQU		2
FSYS_DIR_PRIVATE_SCAN_DIR_CALL_BACK		EQU		FSYS_DIR_PRIVATE_SCAN_DIR_STACK_SIZE+9

FSYS_DIR_PRIVATE_SCAN_DIR
						PSHS	U,Y,X,A
						LEAS	-FSYS_DIR_PRIVATE_SCAN_DIR_STACK_SIZE,S

						;	[FSYS_DIR_PRIVATE_SCAN_DIR_STACK_SIZE+9,S]	Call-Back Function
						;	[FSYS_DIR_PRIVATE_SCAN_DIR_STACK_SIZE+7,S]	Return Address
						;	[FSYS_DIR_PRIVATE_SCAN_DIR_STACK_SIZE+5,S]	U
						;	[FSYS_DIR_PRIVATE_SCAN_DIR_STACK_SIZE+3,S]	Y
						;	[FSYS_DIR_PRIVATE_SCAN_DIR_STACK_SIZE+1,S]	X
						;	[FSYS_DIR_PRIVATE_SCAN_DIR_STACK_SIZE,S]	A

						;	[7,S]	U=Sector Data Buffer
						;	[5,S]	Y=Data Return Pointer
						;	[3,S]	X=File Name Pointer
						;	[2,S]	A=Drive -> BIOS Error Code

						;	[1,S]	Side
						;	[,S]	Sector
						LDD		#$0400
						STD		,S
FSYS_DIR_PRIVATE_SCAN_SECTOR_LOOP
						LDA		#FSYS_DIR_TRACK		; Track 1
						LDB		,S					; Sector
						LDX		1,S					; Side,Drive
						LDU		7,S					; Buffer Location
						BSR		FSYS_BIOS_READSECTOR
						BCS		FSYS_DIR_PRIVATE_SCAN_SECTOR_EXIT


						LDX		7,S
						CLRB
FSYS_DIR_PRIVATE_SCAN_SECTOR_INSIDE_LOOP

FSYS_DIR_PRIVATE_CALL_TEST_FUNC
						JSR		[FSYS_DIR_PRIVATE_SCAN_DIR_CALL_BACK,S]	; Preserves X,B.  Destroys A,U

						BNE		FSYS_DIR_PRIVATE_SCAN_SECTOR_NEXT_32BYTE

						; Zero Flag=Found Match
						LDX		5,S
						STB		2,X		; Offset in sector
						LDD		,S
						STD		,X
						CLRA	; Error Code Clear ,Carry Clear
						BRA		FSYS_DIR_PRIVATE_SCAN_SECTOR_EXIT


FSYS_DIR_PRIVATE_SCAN_SECTOR_NEXT_32BYTE
						ADDB	#32
						BNE		FSYS_DIR_PRIVATE_SCAN_SECTOR_INSIDE_LOOP

						INC		,S
						LDA		,S
						SUBA	#17
						BNE		FSYS_DIR_PRIVATE_SCAN_SECTOR_LOOP

						INCA
						STA		,S
						INC		1,S
						LDA		1,S
						CMPA	#2
						BNE		FSYS_DIR_PRIVATE_SCAN_SECTOR_LOOP

						LDA		#$FF
						COMA	; A=0, Carry set.


FSYS_DIR_PRIVATE_SCAN_SECTOR_EXIT
						LEAS	1+FSYS_DIR_PRIVATE_SCAN_DIR_STACK_SIZE,S	; Add 1 so that it skips A register.
						PULS	U,Y,X,PC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Input
	;	X	File name pointer (File name is always 8 bytes.  " " must fill if the length<8.)
	;   Y	Data-Return pointer (3 bytes)
	;	U	Sector Data Buffer Pointer (256-byte buffer)
	;	A	Drive
	; Output
	;	A	BIOS Error Code
	;	[Y  ]	byte	Sector
	;	[Y+1]	byte	Side
	;	[Y+2]	byte	Offset from the sector top
	;   If nothing found, all zero will be returned.
FSYS_DIR_FIND_FILE_ENTRY
						PSHS	U,Y,X,B

						BSR		FSYS_DIR_CLEAR_DIR_RETURN_BUFFER

						LEAX	FSYS_DIR_CALLBACK_MATCH_FILENAME,PCR
						PSHS	X

						; [7,S] U
						; [5,S] Y
						; [3,S] X
						; [2,S]	B
						; [,S]	Call-Back function

						LDX		3,S
						BSR		FSYS_DIR_PRIVATE_SCAN_DIR

						LEAS	2,S

						PULS	U,Y,X,B,PC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; Input
	;	X	File name pointer (File name is always 8 bytes.  " " must fill if the length<8.)
	;   Y	Data-Return pointer (3 bytes)
	;	U	Sector Data Buffer Pointer (256-byte buffer)
	;	A	Drive
	; Output
	;	A	BIOS Error Code
	;	[Y  ]	byte	Sector
	;	[Y+1]	byte	Side
	;	[Y+2]	byte	Offset from the sector top
	;   If nothing found, all zero will be returned.
FSYS_DIR_FIND_EMPTY_ENTRY
						PSHS	U,Y,X,B

						BSR		FSYS_DIR_CLEAR_DIR_RETURN_BUFFER

						LEAX	FSYS_DIR_CALLBACK_EMPTY_ENTRY,PCR
						PSHS	X

						; [7,S] U
						; [5,S] Y
						; [3,S] X
						; [2,S]	B
						; [,S]	Call-Back function

						LDX		3,S
						BSR		FSYS_DIR_PRIVATE_SCAN_DIR

						LEAS	2,S

						PULS	U,Y,X,B,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FSYS_DIR_CALLBACK_MATCH_FILENAME
						;	[12,S]	Sector Data Buffer
						;	[10,S]	Data Return Pointer
						;	[8,S]	File Name Pointer
						;	[7,S]	Drive
						;	[6,S]	Side
						;	[5,S]	Sector
						;	[3,S]	Return Address
						;	[1,S]	X
						;	[,S]	B
						PSHS	B,X
						ABX

						LDU		8,S
						LDB		#8
FSYS_DIR_CALLBACK_MATCH_FILENAME_LOOP
						LDA		,X+
						CMPA	,U+
						BNE		FSYS_DIR_CALLBACK_MATCH_FILENAME_NO_MATCH
						DECB
						BNE		FSYS_DIR_CALLBACK_MATCH_FILENAME_LOOP

FSYS_DIR_CALLBACK_MATCH_FILENAME_NO_MATCH
						PULS	B,X,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FSYS_DIR_CLEAR_DIR_RETURN_BUFFER
						CLR		,Y
						CLR		1,Y
						CLR		2,Y
						RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FSYS_DIR_CALLBACK_EMPTY_ENTRY
						PSHS	B,X
						ABX
						LDB		,X
						BEQ		FSYS_DIR_CALLBACK_EMPTY_ENTRY_IS_EMPTY
						INCB
FSYS_DIR_CALLBACK_EMPTY_ENTRY_IS_EMPTY
						PULS	B,X,PC




;;;;;;;;;;;;;;;;;;;;;;;;;;;;IMAKOKO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;IMAKOKO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;IMAKOKO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;IMAKOKO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;IMAKOKO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;IMAKOKO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;IMAKOKO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;IMAKOKO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;	; Input
;;;;	;   Y	Data-Return pointer (4 bytes)
;;;;	;	A	Drive
;;;;	; Output
;;;;	;	A	BIOS Error Code
;;;;	;	[Y]		byte	Track
;;;;	;	[Y+1]	byte	Side
;;;;	;	[Y+2]	byte	Sector
;;;;	;	[Y+3]	byte	Offset from the sector top
;;;;	;   If nothing found, all zero will be returned.
;;;;	;	If successful, the sector of the available directory entry is loaded from [FSYS_DIR_BUFFER].
;;;;FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY
;;;;						PSHS	B,X,Y,U,CC,DP
;;;;						LDU		#0
;;;;						STU		,Y
;;;;						STU		2,Y
;;;;
;;;;						STA		FSYS_DIR_DRIVE,PCR
;;;;						LBSR	FSYS_DIR_REWIND
;;;;
;;;;FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY_OUTER_LOOP
;;;;						BSR		FSYS_DIR_READ_SECTOR
;;;;						LDA		FSYS_DIR_ERR,PCR
;;;;						BNE		FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY_RTS
;;;;
;;;;						CLRA
;;;;						LEAU	FSYS_DIR_BUFFER,PCR
;;;;FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY_INNER_LOOP
;;;;						; File name first letter to be 0 (deleted) or $ff (unused)
;;;;						LDB		,U
;;;;						BEQ		FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY_FOUND
;;;;						INCB
;;;;						BNE		FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY_NOT_AVAILABLE
;;;;
;;;;FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY_FOUND
;;;;						STA		3,Y
;;;;						LDA		FSYS_DIR_TRACK,PCR
;;;;						STA		,Y
;;;;						LDA		FSYS_DIR_SIDE,PCR
;;;;						STA		1,Y
;;;;						LDA		FSYS_DIR_SECTOR,PCR
;;;;						STA		2,Y
;;;;						CLRA
;;;;						BRA		FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY_RTS
;;;;
;;;;FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY_NOT_AVAILABLE
;;;;						LEAU	32,U
;;;;						ADDA	#32
;;;;						BCC		FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY_INNER_LOOP
;;;;
;;;;						LBSR	FSYS_DIR_NEXT_SECTOR
;;;;						BCC		FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY_OUTER_LOOP
;;;;
;;;;FSYS_DIR_FIND_AVAILABLE_FILE_ENTRY_RTS
;;;;						LDA		FSYS_DIR_ERR,PCR
;;;;						PULS	B,X,Y,U,CC,DP,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;						; Input
;;;;						;	A				Drive
;;;;						;	[Y]		byte	Track
;;;;						;	[Y+1]	byte	Side
;;;;						;	[Y+2]	byte	Sector
;;;;						;	[Y+3]	byte	Offset from the sector top (Unused in this function)
;;;;						; This function loads given sector to the directory buffer
;;;;FSYS_DIR_FETCH_DIR		PSHS	Y
;;;;						STA		FSYS_DIR_DRIVE,PCR
;;;;						LDA		,Y
;;;;						STA		FSYS_DIR_TRACK,PCR
;;;;						LDA		1,Y
;;;;						STA		FSYS_DIR_SIDE,PCR
;;;;						LDA		2,Y
;;;;						STA		FSYS_DIR_SECTOR,PCR
;;;;						LEAY	FSYS_DIR_BUFFER,PCR
;;;;						STY		FSYS_DIR_DATAPTR,PCR
;;;;						BSR		FSYS_DIR_READ_SECTOR
;;;;						LDA		FSYS_DIR_ERR,PCR
;;;;						PULS	Y,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;FSYS_DIR_BIOSCMD		FCB		BIOSCMD_READSECTOR
;;;;FSYS_DIR_ERR			FCB		0
;;;;FSYS_DIR_DATAPTR		FDB		FSYS_DIR_BUFFER
;;;;FSYS_DIR_TRACK			FCB		1
;;;;FSYS_DIR_SECTOR			FCB		1
;;;;FSYS_DIR_SIDE			FCB		0
;;;;FSYS_DIR_DRIVE			FCB		0
;;;;
;;;;FSYS_DIR_FILENAME		RZB		8
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;FSYS_DIR_READ_SECTOR	PSHS	A,B,X,Y,U,CC,DP
;;;;						LEAX	FSYS_DIR_BUFFER,PCR
;;;;						STX		FSYS_DIR_DATAPTR,PCR
;;;;
;;;;						LEAX	FSYS_DIR_BIOSCMD,PCR
;;;;						LDA		#BIOSCMD_READSECTOR
;;;;						STA		,X
;;;;
;;;;						ORCC	#$50
;;;;						LDA		#$FD
;;;;						TFR		A,DP
;;;;						JSR		BIOSCALL_READSECTOR
;;;;
;;;;						PULS	A,B,X,Y,U,CC,DP,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;						; Calling this function will write back loaded directory entry.
;;;;						; The location depends on the last read/write in FSYS_DIR_* function.
;;;;FSYS_DIR_WRITE_SECTOR	PSHS	A,B,X,Y,U,CC,DP
;;;;						LEAX	FSYS_DIR_BUFFER,PCR
;;;;						STX		FSYS_DIR_DATAPTR,PCR
;;;;
;;;;						LEAX	FSYS_DIR_BIOSCMD,PCR
;;;;						LDA		#BIOSCMD_WRITESECTOR
;;;;						STA		,X
;;;;
;;;;						ORCC	#$50
;;;;						LDA		#$FD
;;;;						TFR		A,DP
;;;;						JSR		BIOSCALL_WRITESECTOR
;;;;
;;;;						PULS	A,B,X,Y,U,CC,DP,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;FSYS_DIR_REWIND			PSHS	A
;;;;						LDA		#1
;;;;						STA		FSYS_DIR_TRACK,PCR
;;;;						CLR		FSYS_DIR_SIDE,PCR
;;;;						LDA		#4
;;;;						STA		FSYS_DIR_SECTOR,PCR
;;;;						PULS	A,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;						; Carry Set -> End of directory
;;;;FSYS_DIR_NEXT_SECTOR	PSHS	A
;;;;						LDA		FSYS_DIR_SECTOR,PCR
;;;;						INCA
;;;;						STA		FSYS_DIR_SECTOR,PCR
;;;;						BITA	#$10
;;;;						BEQ		FSYS_DIR_NEXT_SECTOR_RTS
;;;;
;;;;						LDA		#1
;;;;						STA		FSYS_DIR_SECTOR,PCR
;;;;						LDA		FSYS_DIR_SIDE,PCR
;;;;						INCA
;;;;						STA		FSYS_DIR_SIDE,PCR
;;;;						BITA	#2
;;;;						BEQ		FSYS_DIR_NEXT_SECTOR_RTS
;;;;
;;;;						CLR		FSYS_DIR_SIDE,PCR
;;;;						LDA		FSYS_DIR_TRACK,PCR
;;;;						INCA
;;;;						STA		FSYS_DIR_TRACK,PCR
;;;;
;;;;						CMPA	#2
;;;;						BGE		FSYS_DIR_NEXT_SECTOR_END_OF_DIRECTORY
;;;;
;;;;FSYS_DIR_NEXT_SECTOR_RTS
;;;;						ANDCC	#$FE
;;;;						PULS	A,PC
;;;;
;;;;FSYS_DIR_NEXT_SECTOR_END_OF_DIRECTORY
;;;;						ORCC	#1
;;;;						PULS	A,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;						; X input file-name address
;;;;FSYS_DIR_PREP_FILENAME	PSHS	A,B,X,U
;;;;
;;;;						LDB		#8
;;;;						LEAU	FSYS_DIR_FILENAME,PCR
;;;;
;;;;FSYS_DIR_PREP_FILENAME_LOOP1
;;;;						LDA		,X+
;;;;						BEQ		FSYS_DIR_PREP_FILENAME_LOOP1_BREAK
;;;;						STA		,U+
;;;;						DECB
;;;;						BNE		FSYS_DIR_PREP_FILENAME_LOOP1
;;;;						BRA		FSYS_DIR_PREP_FILENAME_RTS
;;;;FSYS_DIR_PREP_FILENAME_LOOP1_BREAK
;;;;
;;;;						LDA		#' '
;;;;FSYS_DIR_PREP_FILENAME_LOOP2
;;;;						STA		,U+
;;;;						DECB
;;;;						BNE		FSYS_DIR_PREP_FILENAME_LOOP2
;;;;
;;;;FSYS_DIR_PREP_FILENAME_RTS
;;;;						PULS	A,B,X,U,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;						; Input
;;;;						;    U top address in directory sector buffer
;;;;						; Output
;;;;						;    Zero flag			Match
;;;;						;    Not Zero flag		Not match
;;;;FSYS_DIR_MATCH_FILENAME
;;;;						PSHS	A,B,X,U
;;;;
;;;;						LDB		#8
;;;;						LEAX	FSYS_DIR_FILENAME,PCR
;;;;
;;;;FSYS_DIR_MATCH_FILENAME_LOOP
;;;;						LDA		,U+
;;;;						CMPA	,X+
;;;;						BNE		FSYS_DIR_MATCH_FILENAME_RTS
;;;;						DECB
;;;;						BNE		FSYS_DIR_MATCH_FILENAME_LOOP
;;;;
;;;;FSYS_DIR_MATCH_FILENAME_RTS
;;;;						PULS	A,B,X,U,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;						; Input
;;;;						;	X	File name
;;;;						;	Y	Destination
;;;;						;	8 letters will be writen to [Y:]
;;;;FSYS_DIR_COPY_FILE_NAME
;;;;						PSHS	A,B,X,Y
;;;;						LDB		#8
;;;;
;;;;FSYS_DIR_COPY_FILE_NAME_LOOP1
;;;;						LDA		,X+
;;;;						BEQ		FSYS_DIR_COPY_FILE_NAME_LOOP1_BREAK
;;;;						STA		,Y+
;;;;						DECB
;;;;						BNE		FSYS_DIR_COPY_FILE_NAME_LOOP1
;;;;
;;;;FSYS_DIR_COPY_FILE_NAME_LOOP1_BREAK
;;;;						TSTB
;;;;						BEQ		FSYS_DIR_COPY_FILE_NAME_RTS
;;;;
;;;;						LDA		#' '
;;;;FSYS_DIR_COPY_FILE_NAME_LOOP2
;;;;						STA		,Y+
;;;;						DECB
;;;;						BNE		FSYS_DIR_COPY_FILE_NAME_LOOP2
;;;;
;;;;FSYS_DIR_COPY_FILE_NAME_RTS
;;;;						PULS	A,B,X,Y,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;						; Input
;;;;						;	Y	Directry Entry address
;;;;FSYS_DIR_CLEAR_ENTRY
;;;;						PSHS	A,B,Y
;;;;
;;;;						LDA		#' '
;;;;						LDB		#8
;;;;FSYS_DIR_CLEAR_ENTRY_LOOP1
;;;;						STA		,Y+
;;;;						DECB
;;;;						BNE		FSYS_DIR_CLEAR_ENTRY_LOOP1
;;;;
;;;;						LDB		#24
;;;;FSYS_DIR_CLEAR_ENTRY_LOOP2
;;;;						CLR		,Y+
;;;;						DECB
;;;;						BNE		FSYS_DIR_CLEAR_ENTRY_LOOP2
;;;;
;;;;						PULS	A,B,Y,PC
