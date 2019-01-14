					; Must be linked after other code .asm files

					ORG		$2000
					; Default Standard Formatting (16 sectors per track, 256 bytes per sector)
TRACKINFO			FCB		16			; 16 sectors
					FCB		 1,1,0			; Sector 1  SizeShift=1
					FCB		 2,1,0			; Sector 1  SizeShift=1
					FCB		 3,1,0			; Sector 1  SizeShift=1
					FCB		 4,1,0			; Sector 1  SizeShift=1
					FCB		 5,1,0			; Sector 1  SizeShift=1
					FCB		 6,1,0			; Sector 1  SizeShift=1
					FCB		 7,1,0			; Sector 1  SizeShift=1
					FCB		 8,1,0			; Sector 1  SizeShift=1
					FCB		 9,1,0			; Sector 1  SizeShift=1
					FCB		10,1,0			; Sector 1  SizeShift=1
					FCB		11,1,0			; Sector 1  SizeShift=1
					FCB		12,1,0			; Sector 1  SizeShift=1
					FCB		13,1,0			; Sector 1  SizeShift=1
					FCB		14,1,0			; Sector 1  SizeShift=1
					FCB		15,1,0			; Sector 1  SizeShift=1
					FCB		16,1,0			; Sector 1  SizeShift=1

					FCB		0				; Same format til the last track



					ORG		$4000
TRACKIMAGE
