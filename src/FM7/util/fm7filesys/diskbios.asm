
BIOSCALL_RESTORE		EQU		$FE02
BIOSCALL_WRITESECTOR	EQU		$FE05
BIOSCALL_READSECTOR		EQU		$FE08

BIOSCMD_READSECTOR		EQU		10
BIOSCMD_WRITESECTOR		EQU		9
BIOSCMD_RESTORE			EQU		8

BIOSERR_DISK_NOTREADY		EQU		10
BIOSERR_DISK_WRITEPROTECTED	EQU	11
BIOSERR_DISK_HARDERROR		EQU		12	; SEEK ERROR, LOST DATA, RECORD NOT FOUND
BIOSERR_DISK_CRCERROR		EQU		13
BIOSERR_DISK_DDMARK			EQU		14
BIOSERR_DISK_TIMEOVER		EQU		15
BIOSERR_DISK_OVERFLOW		EQU		0x81



; Read Sector
;	Input	X	Higher8=Side		Lower8=Drive
;			A	Track
;			B	Sector
;			U	Location
;   Output
FSYS_BIOS_READSECTOR
						PSHS	B,X,Y,U,DP
						LEAS	-8,S
						BSR		FSYS_BIOS_PRIVATE_MAKEBIOSCALL
						LDA		#BIOSCMD_READSECTOR
						LDY		#BIOSCALL_READSECTOR
						BRA		FSYS_BIOS_CALL_DISK_BIOS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Write Sector
;	Input	X	Higher8=Side		Lower8=Drive
;			A	Track
;			B	Sector
;			U	Location
;   Output
;			Registers not preserved
FSYS_BIOS_WRITESECTOR
						PSHS	B,X,Y,U,DP
						LEAS	-8,S
						BSR		FSYS_BIOS_PRIVATE_MAKEBIOSCALL
						LDA		#BIOSCMD_WRITESECTOR
						LDY		#BIOSCALL_WRITESECTOR
FSYS_BIOS_CALL_DISK_BIOS
						STA		,S
						LEAX	,S
						LDB		#$FD
						TFR		B,DP
						JSR		,Y
						LEAS	8,S
						PULS	B,X,Y,U,DP,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Private function
;	Input	X	Higher8=Side		Lower8=Drive
;			A	Track
;			B	Sector
;			U	Location
FSYS_BIOS_PRIVATE_MAKEBIOSCALL
						; 		2+0,S		; Command
						;		2+1,S		; Error Return
						STU		2+2,S		; Data Pointer
						STD		2+4,S		; Track and Sector
						STX		2+6,S		; Side and Drive
						RTS
