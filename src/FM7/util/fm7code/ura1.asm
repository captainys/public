					ORG		$1400

PROG_ENTRY			PSHS	A,B,X,U
					ORCC	#$50

					STA		$FD0F

					LDX		#$2000
					LDU		#$8000

COPY_LOOP			LDD		,X++
					STD		,U++
					CMPU	#$C000
					BNE		COPY_LOOP

					LDA		$FD0F

					ANDCC	#$AF
					PULS	A,B,X,U,PC

					END		PROG_ENTRY
