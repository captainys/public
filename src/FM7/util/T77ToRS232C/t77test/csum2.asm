						ORG		$1400

						PSHS	A,B,X,Y,U
						LDD		#0
						LEAX	CHECKSUM,PCR
						STD		,X++
						STD		,X++
						STA		,X+

						; Hunting down why Delphis does not start in actual AV40
						; DATA     $4A00 bytes from $2000 
						; DELPH.M  $5100 bytes from $1600

						LDU		#$1600
						LDY		#$5100
LOOP					LDB		,U+
						BNE		NONZERO
						LDX		CHECKZERO,PCR
						LEAX	1,X
						STX		CHECKZERO,PCR

NONZERO					LDX		CHECKSUM,PCR
						ABX
						STX		CHECKSUM,PCR

						EORB	CHECKXOR,PCR
						STB		CHECKXOR,PCR

						LEAY	-1,Y
						BNE		LOOP
						PULS	A,B,X,Y,U,PC

						FCB		$FF
CHECKSUM				FDB		0
CHECKXOR				FCB		0
CHECKZERO				FDB		0
