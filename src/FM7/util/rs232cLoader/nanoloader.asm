					ORG		$2000

					ORCC	#$50
					LDX		#$6000			; 3 clocks
					LDB		#$02			; 2 clocks

LOAD_LOOP			BITB	$FD07
					BEQ		LOAD_LOOP
					LDA		$FD06
					STA		,X+
					CMPX	#$6100
					BNE		LOAD_LOOP

					RTS

END_OF_PROGRAM

					; Usage:
					;   FM-7 to FM77AV1/2
					;     Just type in and,
					;     OPEN "I",#1,"COM0:(F8N1)"
					;     EXEC &H2000
					;   FM77AV20/40 and newer
					;     POKE &HFD0C,5
					;	  POKE &HFD0B,16
					;     POKE &HFD07,0
					;     POKE &HFD07,0
					;     POKE &HFD07,0
					;     POKE &HFD07,&H40
					;     POKE &HFD07,&H4E
					;     POKE &HFD07,&HB7
					;	  EXEC &H2000

					; IO_RS232C_CMD		$FD07
					; IO_RS232C_DATA	$FD06

