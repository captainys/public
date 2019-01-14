
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


					; A value
					; X string pointer
ItoA				LDB		#'0'
					STB		,X
					STB		1,X
ItoA_LOOP			CMPA	#10
					BCS		ItoA_LOOPOUT
					SUBA	#10
					INC		,X
					BRA		ItoA_LOOP
ItoA_LOOPOUT		ADDA	#'0'
					STA		1,X
					RTS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


					; A value
					; X string pointer
XtoA				TFR		A,B
					LSRA
					LSRA
					LSRA
					LSRA
					BSR		XtoA_ONELETTER
					TFR		B,A
					ANDA	#$0F
					LEAX	1,X
					BSR		XtoA_ONELETTER
					RTS

XtoA_ONELETTER		CMPA	#10
					BGE		XtoA_ONELETTER_A_F
					ADDA	#'0'
					STA		,X
					RTS
XtoA_ONELETTER_A_F	SUBA	#10
					ADDA	#'A'
					STA		,X
					RTS


