						ORG		$1400

						LDX		#$2000
LOOP					LDA		,X+
						CMPA	#$4E
						BNE		FOUND
						CMPX	#$6000
						BNE		LOOP
						LDX		#$FFFF
FOUND					STX		ADDR,PCR
						RTS
						FCB		$FF
ADDR					FDB		0
						FCB		$FF
						RZB		32


