						ORG		$1400

						LDX		#$2000
						CLRA
LOOP					CMPA	,X+
						BNE		FOUND
						ADDA	#$10
						CMPX	#$6000
						BNE		LOOP
						LDX		#$FFFF
FOUND					STX		ADDR,PCR
						RTS
						FCB		$FF
ADDR					FDB		0
						FCB		$FF
						RZB		32

