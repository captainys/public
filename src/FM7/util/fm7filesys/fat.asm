
		; I still cannot find the meaning of the first byte of Track 1 Side 0 Sector 1.
		; It looks to be 0xE5 immediately after formatting, and turns 0x00 when the first file is saved???


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
						; Input
						;	Y		FAT Buffer (152 bytes)
						;	U		Sector Read Buffer (256 bytes)
						;	B		Drive
						; Output
						;   A		BIOS Error Code
						;	Registers won't be preserved.
						;   Carry set if error.
FSYS_FAT_LOAD
						PSHS	Y
						LEAY	FSYS_BIOS_READSECTOR,PCR
						BSR		FSYS_FAT_READ_WRITE_FAT_SECTOR
						PULS	Y
						BCS		FSYS_FAT_LOAD_EXIT

						LEAU	5,U
						LDB		#152
FSYS_FAT_LOAD_TFR_LOOP
						LDA		,U+
						STA		,Y+
						DECB
						BNE		FSYS_FAT_LOAD_TFR_LOOP
						CLRA

FSYS_FAT_LOAD_EXIT		RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
						; Input
						;	Y		FAT Buffer (152 bytes)
						;	U		Sector Read Buffer (256 bytes)
						;	B		Drive
						; Output
						;   A		BIOS Error Code
						;	Registers won't be preserved.
						;   Carry set if error.

FSYS_FAT_SAVE
						PSHS	Y,B
						LEAY	FSYS_BIOS_READSECTOR,PCR
						BSR		FSYS_FAT_READ_WRITE_FAT_SECTOR
						BCS		FSYS_FAT_SAVE_EXIT

						LDY		1,S
						LEAU	5,U
						LDB		#152
FSYS_FAT_SAVE_TFR_LOOP
						LDA		,Y+
						STA		,U+
						DECB
						BNE		FSYS_FAT_SAVE_TFR_LOOP

						LEAU	-157,U
						LDB		,S
						LEAY	FSYS_BIOS_WRITESECTOR,PCR
						BSR		FSYS_FAT_READ_WRITE_FAT_SECTOR

FSYS_FAT_SAVE_EXIT		PULS	B,Y,PC



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FSYS_FAT_READ_WRITE_FAT_SECTOR
										; B=Drive
						LDA		#0		; A=Side
						TFR		D,X
						LDD		#$0101	; Track 1 Sector 1
						JSR		,Y
						RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
					; Input
					;	A	Cluster
					; Output
					;	A	Track
					;	B	Sector
					;	X	Higher8=Side		Lower8=0
FSYS_FAT_CLUSTER_TO_TRACK_SIDE_SECTOR
	; Track 2 Side 0 Sector 1=Cluster 0
	;                Sector 9=Cluster 1
	; Track 2 Side 1 Sector 1=Cluster 2
	;                Sector 9=Cluster 3
	; Track 3 Side 0 Sector 1=Cluster 4
	;                Sector 9=Cluster 5
	; Track 3 Side 1 Sector 1=Cluster 6
	;                Sector 9=Cluster 7
	; Track 4 Side 0 Sector 1=Cluster 8
	; 				:
	; 				:
	; Cluster A-> Track 2+A/4
	;             Side  (A/2)%2
	;             Sector 1+8*(A%2)

						LDX		#0

						PSHS	X
						TFR		A,B
						LSRB
						ANDB	#1		; Side
						STB		,S		; Will be pulled to high-byte of X.

						TFR		A,B
						ANDB	#1
						LSLB
						LSLB
						LSLB
						INCB			; Sector

						LSRA
						LSRA
						ADDA	#2		; Track

						PULS	X,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Input
;		X				FAT pointer
;		Y				Poiner to Cluster, Track, Sector, Side
;		[,Y]			Cluster
;		[1,Y]			Track
;		[2,Y]			Sector
;		[3,Y]			Side
;	Output
;		Carry Clear		No Error
;		Carry Set		Error
;	Preserves Y,U
;	Destroys A,B
;
FSYS_FAT_NEXT_CLUSTER
						LDB		,Y
						CMPB	#$C0
						BCC		FSYS_FAT_NEXT_CLUSTER_ERROR

						CLRA
						LDA		D,X

						STA		,Y
						BSR		FSYS_FAT_CLUSTER_TO_TRACK_SIDE_SECTOR
						STD		1,Y

						TFR		X,D
						STA		3,Y
						CLRA
						RTS
FSYS_FAT_NEXT_CLUSTER_ERROR
						COMA
						RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Input
;		Y				FAT buffer
;		D				Number of bytes
;	Output
;		A				First cluster
;		B				Number of clusters
;		Carry Clear		Reserved
;		Carry Set		Disk full
FSYS_FAT_RESERVE_CLUSTER_CHAIN_BY_BYTE_COUNT
						PSHS	B,A

						; Numbef of used sectors in the last cluster -1.
						ANDA	#7

						PSHS	A

						LDD		1,S
						ADDD	#$7FF
						LSRA
						LSRA
						LSRA	; A=Required number of clusters.

						PULS	B

						STA		1,S
						BSR		FSYS_FAT_RESERVE_CLUSTER_CHAIN

						LEAS	1,S	; Skip A
						PULS	B,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Input
;		Y				FAT buffer
;		A				Number of clusters
;		B				Number of sectors in the last cluster

;	Output
;		A				First cluster
;		Carry Clear		Reserved
;		Carry Set		Disk full

FSYS_FAT_RESERVE_CLUSTER_CHAIN_STACK_SIZE		EQU		3

FSYS_FAT_RESERVE_CLUSTER_CHAIN
						PSHS	Y,X,B,A
						LEAS	-FSYS_FAT_RESERVE_CLUSTER_CHAIN_STACK_SIZE,S

						; [FSYS_FAT_RESERVE_CLUSTER_CHAIN_STACK_SIZE+4,S]	FAT buffer pointer
						; [FSYS_FAT_RESERVE_CLUSTER_CHAIN_STACK_SIZE+2,S]	X
						; [FSYS_FAT_RESERVE_CLUSTER_CHAIN_STACK_SIZE+1,S]	Number of sectors in the last cluster
						; [FSYS_FAT_RESERVE_CLUSTER_CHAIN_STACK_SIZE,S]		Number of clusters

						; [1,S] FAT buffer +152
						; [,S]	First cluster

						LEAX	152,Y
						STX		1,S
						LEAX	-152,X

						LEAY	,S	; <- First available cluster will be written here.



						CLRA
FSYS_FAT_RESERVE_CLUSTER_CHAIN_FIND_LOOP
						LDB		,X+
						INCB	; CMPB	#$FF
						BNE		FSYS_FAT_RESERVE_CLUSTER_CHAIN_NEXT

						; Found one.
						STA		,Y
						LEAY	-1,X

						DEC		FSYS_FAT_RESERVE_CLUSTER_CHAIN_STACK_SIZE,S
						BEQ		FSYS_FAT_RESERVE_CLUSTER_CHAIN_LAST_CLUSTER

FSYS_FAT_RESERVE_CLUSTER_CHAIN_NEXT
						INCA
						CMPX	1,S
						BNE		FSYS_FAT_RESERVE_CLUSTER_CHAIN_FIND_LOOP
						BRA		FSYS_FAT_DISK_FULL



FSYS_FAT_RESERVE_CLUSTER_CHAIN_LAST_CLUSTER
						LDB		FSYS_FAT_RESERVE_CLUSTER_CHAIN_STACK_SIZE+1,S
						ANDB	#$0F
						ORB		#$C0
						STB		-1,X

						CLRA			; A=0, Carry clear.
						LDA		,S

						FCB		$8E		; Making it LDX #$@@@@
FSYS_FAT_DISK_FULL		ORCC	#1

						LEAS	1+FSYS_FAT_RESERVE_CLUSTER_CHAIN_STACK_SIZE,S	; Skip A by adding 1.
						PULS	B,X,Y,PC



;;;;
;;;;					; Input
;;;;					;	A		Drive
;;;;					; Output
;;;;					;   A		BIOS Error Code
;;;;FSYS_FAT_LOAD		PSHS	B,X,Y,U,DP,CC
;;;;					LEAX	FSYS_FAT_BUFFER,PCR
;;;;					STX		FSYS_FAT_DATAPTR,PCR
;;;;					STA		FSYS_FAT_DRIVE,PCR
;;;;
;;;;					LDA		#BIOSCMD_READSECTOR
;;;;					STA		FSYS_FAT_BIOSCMD,PCR
;;;;
;;;;					LDA		#1
;;;;					STA		FSYS_FAT_TRACK,PCR
;;;;					STA		FSYS_FAT_SECTOR,PCR
;;;;					CLR		FSYS_FAT_SIDE,PCR
;;;;
;;;;					ORCC	#$50
;;;;					LDA		#$FD
;;;;					TFR		A,DP
;;;;					LEAX	FSYS_FAT_BIOSCMD,PCR
;;;;					JSR		BIOSCALL_READSECTOR
;;;;
;;;;					LDA		FSYS_FAT_ERR,PCR
;;;;
;;;;					PULS	B,X,Y,U,DP,CC,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;					; Must call after FSYS_LOADFAT
;;;;					; Input
;;;;					;   A		First cluster
;;;;					; Output
;;;;					;   A		Number of clusters
;;;;					;	B		Number of sectors used in the last cluster
;;;;					;   If the first cluster is unused, A and B returns zero.
;;;;FSYS_FAT_MAKE_CLUSTER_CHAIN
;;;;					PSHS	X,Y,U
;;;;
;;;;					TFR		A,B
;;;;					LEAU	FSYS_FAT_CHAINBUFFER,PCR
;;;;					CLR		FSYS_FAT_N_IN_CHAIN,PCR
;;;;
;;;;FSYS_FAT_MAKE_CLUSTER_CHAIN_LOOP
;;;;					LEAX	5+FSYS_FAT_BUFFER,PCR
;;;;					ABX
;;;;
;;;;					LDA		,X
;;;;					CMPA	#$FF		; Unused?
;;;;					BEQ		FSYS_FAT_MAKE_CLUSTER_CHAIN_ERROR  ; Unterminated chain
;;;;
;;;;					BSR		FSYS_FAT_MAKE_CLUSTER_CHAIN_CHECK_CYCLE
;;;;					BCS		FSYS_FAT_MAKE_CLUSTER_CHAIN_ERROR  ; Cycle
;;;;					STB		,U+
;;;;					INC		FSYS_FAT_N_IN_CHAIN,PCR
;;;;
;;;;					ANDA	#$F0
;;;;					CMPA	#$C0
;;;;					BEQ		FSYS_FAT_MAKE_CLUSTER_CHAIN_TERMINATED
;;;;
;;;;					; Not #$FF, not #$Cx, then it is the next cluster.
;;;;					LDB		,X
;;;;					BRA		FSYS_FAT_MAKE_CLUSTER_CHAIN_LOOP
;;;;
;;;;FSYS_FAT_MAKE_CLUSTER_CHAIN_TERMINATED
;;;;					LDA		,X
;;;;					ANDA	#$0F
;;;;					TFR		A,B
;;;;					STB		FSYS_FAT_NSEC_IN_LAST,PCR
;;;;
;;;;FSYS_FAT_MAKE_CLUSTER_CHAIN_RTS
;;;;					LDA		FSYS_FAT_N_IN_CHAIN,PCR
;;;;					PULS	X,Y,U,PC
;;;;
;;;;FSYS_FAT_MAKE_CLUSTER_CHAIN_ERROR
;;;;					CLR		FSYS_FAT_N_IN_CHAIN,PCR
;;;;					CLRB
;;;;					BRA		FSYS_FAT_MAKE_CLUSTER_CHAIN_RTS
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;					; Check if B is included in [FSYS_FAT_CHAINBUFFER] to [U-1]
;;;;FSYS_FAT_MAKE_CLUSTER_CHAIN_CHECK_CYCLE
;;;;					PSHS	U,B
;;;;
;;;;					LEAU	FSYS_FAT_CHAINBUFFER,PCR
;;;;					STU		,--S
;;;;
;;;;					;	[3,S]	U
;;;;					;	[2,S]	B
;;;;					;	[,S]	FSYS_FAT_CHAINBUFFER
;;;;
;;;;					LDU		3,S
;;;;
;;;;FSYS_FAT_MAKE_CLUSTER_CHAIN_CHECK_CYCLE_LOOP
;;;;					CMPB	,-U
;;;;					BEQ		FSYS_FAT_MAKE_CLUSTER_CHAIN_CHECK_CYCLE_IS_CYCLE
;;;;					CMPU	,S
;;;;					BHI		FSYS_FAT_MAKE_CLUSTER_CHAIN_CHECK_CYCLE_LOOP
;;;;
;;;;					LEAS	2,S
;;;;					ANDCC	#$FE
;;;;					PULS	B,U,PC
;;;;FSYS_FAT_MAKE_CLUSTER_CHAIN_CHECK_CYCLE_IS_CYCLE
;;;;					LEAS	2,S
;;;;					ORCC	#1
;;;;					PULS	B,U,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;					; Call after FSYS_FAT_LOAD
;;;;					; Input
;;;;					;   D		Size in bytes, not number of clusters
;;;;					; Return
;;;;					;	A number of clusters found.
;;;;					;	[FSYS_FAT_CHAINBUFFER] Clusters
;;;;					;   [FSYS_FAT_N_IN_CHAIN] Number of clusters
;;;;					;	[FSYS_FAT_NSEC_IN_LAST] Number of sectors used in the last cluster
;;;;					;	A will be zero if clusters for storing D bytes is not available.
;;;;FSYS_FAT_FIND_AVAILABLE_CLUSTER
;;;;					PSHS	A,B,X,Y,U
;;;;
;;;;					CLR		FSYS_FAT_NSEC_IN_LAST,PCR
;;;;					CLR		FSYS_FAT_N_IN_CHAIN,PCR
;;;;					CMPD	#0
;;;;					BEQ		FSYS_FAT_FIND_AVAILABLE_CLUSTER_RTS
;;;;
;;;;					PSHS	A
;;;;					ANDA	#7
;;;;					STA		FSYS_FAT_NSEC_IN_LAST,PCR
;;;;					PULS	A
;;;;
;;;;					;  1 cluster=$800 bytes 
;;;;					ADDD	#$7FF
;;;;					LSRA
;;;;					LSRA
;;;;					LSRA
;;;;					STA		FSYS_FAT_N_IN_CHAIN,PCR
;;;;
;;;;					; Need to find A unused clusters
;;;;					LEAU	FSYS_FAT_BUFFER+5,PCR
;;;;					LEAX	FSYS_FAT_CHAINBUFFER,PCR
;;;;					TFR		A,B
;;;;					CLRA
;;;;					TFR		D,Y
;;;;
;;;;FSYS_FAT_FIND_AVAILABLE_CLUSTER_LOOP
;;;;					LDB		,U+
;;;;					CMPB	#$FF
;;;;					BNE		FSYS_FAT_FIND_AVAILABLE_CLUSTER_LOOP_NEXT
;;;;
;;;;					STA		,X+
;;;;					LEAY	-1,Y
;;;;					BEQ		FSYS_FAT_FIND_AVAILABLE_CLUSTER_RTS
;;;;
;;;;FSYS_FAT_FIND_AVAILABLE_CLUSTER_LOOP_NEXT
;;;;					INCA
;;;;					CMPA	#152
;;;;					BNE		FSYS_FAT_FIND_AVAILABLE_CLUSTER_LOOP
;;;;
;;;;					; Falling to this line means not enough clusters.
;;;;					CLR		FSYS_FAT_NSEC_IN_LAST,PCR
;;;;					CLR		FSYS_FAT_N_IN_CHAIN,PCR
;;;;
;;;;FSYS_FAT_FIND_AVAILABLE_CLUSTER_RTS
;;;;					LDA		FSYS_FAT_N_IN_CHAIN,PCR
;;;;					PULS	A,B,X,Y,U,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;					; Reserve cluster chain previously found by 
;;;;					; FSYS_FAT_FIND_AVAILABLE_CLUSTER
;;;;FSYS_FAT_RESERVE_CLUSTER_CHAIN
;;;;					PSHS	A,B,X,Y,U
;;;;
;;;;					LEAU	FSYS_FAT_CHAINBUFFER+1,PCR
;;;;					STU		,--S
;;;;
;;;;					LDB		FSYS_FAT_N_IN_CHAIN,PCR
;;;;					BEQ		FSYS_FAT_RESERVE_CLUSTER_CHAIN_RTS
;;;;
;;;;					LEAU	FSYS_FAT_CHAINBUFFER,PCR
;;;;					CLRA
;;;;					LEAU	D,U
;;;;					LDB		,-U	; B=Last cluster
;;;;
;;;;					LEAX	FSYS_FAT_BUFFER+5,PCR
;;;;					ABX
;;;;					LDA		#$C0
;;;;					ORA		FSYS_FAT_NSEC_IN_LAST,PCR
;;;;					STA		,X
;;;;
;;;;FSYS_FAT_RESERVE_CLUSTER_CHAIN_LOOP
;;;;					CMPU	,S
;;;;					BCS		FSYS_FAT_RESERVE_CLUSTER_CHAIN_RTS
;;;;
;;;;					LDA		,U
;;;;					LDB		,-U
;;;;					LEAX	FSYS_FAT_BUFFER+5,PCR
;;;;					ABX
;;;;					STA		,X
;;;;					BRA		FSYS_FAT_RESERVE_CLUSTER_CHAIN_LOOP
;;;;
;;;;FSYS_FAT_RESERVE_CLUSTER_CHAIN_RTS
;;;;					LEAS	2,S
;;;;					PULS	A,B,X,Y,U,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;					; Input
;;;;					;   X 	data pointer
;;;;					;	D 	data size in bytes
;;;;					;	Y	Drive (Lower 2 bits only)
;;;;					;	FSYS_FAT_CHAINBUFFER must be populated.
;;;;					; Output
;;;;					;	BIOS Error Code
;;;;FSYS_FAT_WRITE_CLUSTER_CHAIN
;;;;					PSHS	B,X,Y,U
;;;;
;;;;					CLR		FSYS_FAT_CLUSTER_COUNTER,PCR
;;;;					CLR		FSYS_FAT_CLUSTER_SECTOR_COUNTER,PCR
;;;;
;;;;FSYS_FAT_WRITE_CLUSTER_CHAIN_LOOP
;;;;					EXG		D,Y
;;;;					TFR		B,A
;;;;					BSR		FSYS_FAT_WRITE_CLUSTER_CHAIN_ONE_SECTOR
;;;;					TSTA
;;;;					BNE		FSYS_FAT_WRITE_CLUSTER_CHAIN_RTS
;;;;
;;;;					EXG		D,Y
;;;;					LEAX	$100,X
;;;;					SUBD	#$100
;;;;					BHI		FSYS_FAT_WRITE_CLUSTER_CHAIN_LOOP
;;;;
;;;;					CLRA
;;;;FSYS_FAT_WRITE_CLUSTER_CHAIN_RTS
;;;;					PULS	B,X,Y,U,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;					; Input
;;;;					;   A	Cluster
;;;;					;	B	Number of sectors
;;;;					;	X	Data pointer
;;;;					;	Y	Drive (Lower 2 bits)
;;;;					; Output
;;;;					;	A	BIOS Error Code
;;;;FSYS_FAT_WRITE_CLUSTER
;;;;					PSHS	B,X,Y,U,CC,DP
;;;;
;;;;					LEAU	FSYS_FAT_BIOSCMD,PCR
;;;;
;;;;					PSHS	B
;;;;					STX		2,U
;;;;
;;;;					LBSR	FSYS_FAT_CLUSTER_TO_TRACK_SIDE_SECTOR	; A=Track B=Side X=Sector
;;;;					STA		4,U	;	Track
;;;;					STB		6,U	;	Side
;;;;					TFR		X,D
;;;;					STB		5,U	;	Sector
;;;;
;;;;					TFR		Y,D
;;;;					ANDB	#3
;;;;					STB		7,U
;;;;
;;;;					LDB		#BIOSCMD_WRITESECTOR
;;;;					STB		,U
;;;;
;;;;					PULS	B
;;;;
;;;;					ORCC	#$50
;;;;					LDA		#$FD
;;;;					TFR		A,DP
;;;;
;;;;					TFR		U,X
;;;;
;;;;FSYS_FAT_WRITE_CLUSTER_LOOP
;;;;					PSHS	X,B
;;;;					JSR		BIOSCALL_WRITESECTOR
;;;;					PULS	X,B
;;;;					LDU		2,X
;;;;					LEAU	$100,U
;;;;					STU		2,X
;;;;					INC		5,X
;;;;					LDA		1,X
;;;;					BNE		FSYS_FAT_WRITE_CLUSTER_RTS
;;;;					DECB
;;;;					BNE		FSYS_FAT_WRITE_CLUSTER_LOOP
;;;;
;;;;FSYS_FAT_WRITE_CLUSTER_RTS
;;;;					PULS	B,X,Y,U,CC,DP,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;					; Input
;;;;					;	X 		Data Pointer
;;;;					;	A		Drive
;;;;					;	[FSYS_FAT_CLUSTER_COUNTER]
;;;;					;	[FSYS_FAT_CLUSTER_SECTOR_COUNTER]
;;;;FSYS_FAT_WRITE_CLUSTER_CHAIN_ONE_SECTOR
;;;;					PSHS	B,X
;;;;					LDB		#BIOSCMD_WRITESECTOR
;;;;					STB		FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR_BIOSCMD+1,PCR
;;;;					LDX		#BIOSCALL_WRITESECTOR
;;;;					STX		FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR_JSR+1,PCR
;;;;					PULS	B,X
;;;;					BRA		FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR
;;;;
;;;;;;;;;;;;
;;;;
;;;;FSYS_FAT_READ_CLUSTER_CHAIN_ONE_SECTOR
;;;;					PSHS	B,X
;;;;					LDB		#BIOSCMD_READSECTOR
;;;;					STB		FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR_BIOSCMD+1,PCR
;;;;					LDX		#BIOSCALL_READSECTOR
;;;;					STX		FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR_JSR+1,PCR
;;;;					PULS	B,X
;;;;					BRA		FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR
;;;;
;;;;;;;;;;;;
;;;;
;;;;FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR
;;;;					PSHS	B,X,Y,U,CC,DP
;;;;					STX		FSYS_FAT_DATAPTR,PCR
;;;;					STA		FSYS_FAT_DRIVE,PCR
;;;;
;;;;					ORCC	#$50
;;;;					LDA		#$FD
;;;;					TFR		A,DP
;;;;
;;;;					LEAX	FSYS_FAT_CHAINBUFFER,PCR
;;;;					LDB		FSYS_FAT_CLUSTER_COUNTER,PCR
;;;;					ABX
;;;;
;;;;					PSHS	X
;;;;					LDA		,X
;;;;					BSR		FSYS_FAT_CLUSTER_TO_TRACK_SIDE_SECTOR
;;;;
;;;;					STA		FSYS_FAT_TRACK,PCR
;;;;					STB		FSYS_FAT_SIDE,PCR
;;;;					TFR		X,D
;;;;					ADDB	FSYS_FAT_CLUSTER_SECTOR_COUNTER,PCR
;;;;					STB		FSYS_FAT_SECTOR,PCR
;;;;
;;;;					LEAX	FSYS_FAT_BIOSCMD,PCR
;;;;FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR_BIOSCMD
;;;;					LDA		#BIOSCMD_WRITESECTOR
;;;;					STA		,X
;;;;					
;;;;FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR_JSR
;;;;					JSR		BIOSCALL_WRITESECTOR
;;;;
;;;;					PULS	X
;;;;
;;;;					LDA		FSYS_FAT_CLUSTER_SECTOR_COUNTER,PCR
;;;;					INCA
;;;;					ANDA	#7
;;;;					STA		FSYS_FAT_CLUSTER_SECTOR_COUNTER,PCR
;;;;					BNE		FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR_END_READ_WRITE
;;;;
;;;;					; Next cluster
;;;;					LDA		,X
;;;;					ANDA	#$F0
;;;;					CMPA	#$C0
;;;;					BNE		FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR_NEXT
;;;;
;;;;					; Overflow
;;;;					LDA		#$FF
;;;;					BRA		FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR_RTS
;;;;
;;;;FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR_NEXT
;;;;					INC		FSYS_FAT_CLUSTER_COUNTER,PCR
;;;;
;;;;FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR_END_READ_WRITE
;;;;					LDA		FSYS_FAT_ERR,PCR
;;;;FSYS_FAT_READ_WRITE_CLUSTER_CHAIN_ONE_SECTOR_RTS
;;;;					PULS	B,X,Y,U,CC,DP,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;					; Input
;;;;					;	A		Drive
;;;;					; Output
;;;;					;   A		BIOS Error Code
;;;;FSYS_FAT_SAVE		PSHS	X,DP,CC
;;;;					LEAX	FSYS_FAT_BUFFER,PCR
;;;;					STX		FSYS_FAT_DATAPTR,PCR
;;;;					STA		FSYS_FAT_DRIVE,PCR
;;;;
;;;;					LDA		#BIOSCMD_WRITESECTOR
;;;;					STA		FSYS_FAT_BIOSCMD,PCR
;;;;
;;;;					LDA		#1
;;;;					STA		FSYS_FAT_TRACK,PCR
;;;;					STA		FSYS_FAT_SECTOR,PCR
;;;;					CLR		FSYS_FAT_SIDE,PCR
;;;;
;;;;					ORCC	#$50
;;;;					LDA		#$FD
;;;;					TFR		A,DP
;;;;					LEAX	FSYS_FAT_BIOSCMD,PCR
;;;;					JSR		BIOSCALL_WRITESECTOR
;;;;
;;;;					LDA		FSYS_FAT_ERR,PCR
;;;;
;;;;					PULS	X,DP,CC,PC
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;					; F-BASIC FAT is at Track 1 Side 0 Sector 1
;;;;					; FAT will start 5 bytes offset continues for 152 bytes.
;;;;FSYS_FAT_BIOSCMD		FCB		BIOSCMD_READSECTOR
;;;;FSYS_FAT_ERR			FCB		0
;;;;FSYS_FAT_DATAPTR		FDB		0
;;;;FSYS_FAT_TRACK			FCB		1
;;;;FSYS_FAT_SECTOR			FCB		1
;;;;FSYS_FAT_SIDE			FCB		0
;;;;FSYS_FAT_DRIVE			FCB		0
;;;;
;;;;FSYS_FAT_CLUSTER_COUNTER		FCB		0
;;;;FSYS_FAT_CLUSTER_SECTOR_COUNTER	FCB		0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;