					EXPORT	RS232C_OPEN
					EXPORT	RS232C_CLOSE



					; Initialization based on disassembly of D77IMG by Mr. Apollo
RS232C_OPEN
					PSHS	A,B

					CLR		$FD02			; Turn off RS232C IRQ
											; This actually turns off other IRQs, but there's no way of knowing which IRQs were enabled.

					LDA		#5
					STA		$FD0C			; AV20/40 DTR OFF  RS232C ON
					MUL						; Wait 11 cycles
					LDA		#$10
					STA		$FD0B			; AV20/40 Baud Rate  100=4x300=1200bps
					MUL						; Wait 11 cycles
					LDA		$FD0F			; FM-7 does not distinguish FD0B and FD0F

					CLR		$FD07			; Reset
					MUL						; Wait 11 cycles
					CLR		$FD07			; Reset
					MUL						; Wait 11 cycles
					CLR		$FD07			; Reset
					MUL						; Wait 11 cycles

					LDA		#$40
					STA		$FD07			; Internal Reset
					MUL						; Wait 11 cycles

					LDA		#$4E			; (Immediately after reset)
					STA		$FD07			; 1 stop-bit, Odd parity, Parity Disabled, 8-bit data, 1/16 baud-rate division (? what is it)
											; Presumably baud rate is 4x300x16=19200bps in AV20/AV40.
											; Baud Rate must be set by hardware dip switches (if I remember correctly) in earlier models.
											; Lowest two bits seem to decide Async or Sync mode.
											; Will be in Sync mode if the lowest two bits are both zero.
					MUL						; Wait 11 cycles

					LDA		#$B7			; ON	Sync Char search (?), 
											; OFF	Internal Reset,
											; ON	RTS request
											; ON	Clear Error Flags
											; OFF	Break
											; ON	RXE Receive Enable
											; ON	DTR Treminal Ready
											; ON	TXE Transmission Enable
					STA		$FD07			

					MUL						; Wait 11 cycles
					PULS	A,B,PC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RS232C_CLOSE		PSHS	A
					LDA		#$40
					STA		$FD07
					PULS	A,PC

