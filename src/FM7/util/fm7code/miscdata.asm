;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RESTORECMD			FCB		8
					FCB		0			; Error Return
					FCB		0
					FCB		0
					FCB		0
					FCB		0
					FCB		0
RESTORE_DRIVE		FCB		0			; Input from BASIC.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BIOSSTRINGOUT		FCB		20,0
BIOSSTRINGOUT_PTR	FDB		0
BIOSSTRINGOUT_LEN	FDB		0
					FDB		0	; For the sake of 8-byte buffer

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BIOSKEYIN			FCB		21,0
BIOSKEYIN_PTR		FDB		0
BIOSKEYIN_LEN		FDB		0
					FDB		0	; For the sake of 8-byte buffer
BIOSKEYIN_OUTBUF	FCB		0,0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ENTERKEYMSG			FCB		"Insert Target Disk in Drive "
ENTERKEYMSG_DRIVE	FCB		"0 and"
					FCB		$0d,$0a
					FCB		"Press RETURN"
					FCB		$0d,$0a
ENTERKEYMSG_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ERRORMSG			FCB		"ERROR! SOMETHING WENT WRONG!"
ERRORMSG_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

WRITEERROR_MSG		FCB		"ERROR! BIOS RETURNED:"
WRITEERROR_MSG_CODE	FCB		"XX"
					FCB		$0d,$0a
WRITEERROR_MSG_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

POSITIONMSG			FCB		"Track:"
TRACK_STR_PTR		FCB		"00"
					FCB		"  Side:"
SIDE_STR_PTR		FCB		"00"
					FCB		"  Sector:"
SECTOR_STR_PTR		FCB		"00"
					FCB		$0d,$0a
POSITIONMSG_END			

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RESTORE_ERRORMSG	FCB		"ERROR! RESTORE command failed."
					FCB		$0d,$0a
RESTORE_ERRORMSG_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BIOSERRMSG			FCB		"BIOS ERROR CODE="
BIOSERRMSG_CODE		FCB		"00"
					FCB		$0d,$0a
BIOSERRMSG_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

TRACKMSG			FCB		"TRACK:"
TRACK_DIGIT			FCB		0x20,0x20
					FCB		" SIDE:"
SIDE_DIGIT			FCB		0x20
					FCB		$0d,$0a
TRACKMSG_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BINITCMD			FCB		23
					FCB		0,0,0,0,0,0,0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


ENTERKEYMSG_STRONG			FCB		"Insert Target Disk in Drive "
ENTERKEYMSG_STRONG_DRIVE	FCB		"0 and"
					FCB		$0d,$0a
					FCB		"Press RETURN"
					FCB		$0d,$0a
					FCB		"Cancel to press ESC"
					FCB		$0d,$0a
					FCB		"CAUTION! Current content of the disk"
					FCB		$0d,$0a
					FCB		"will be erased!"
					FCB		$0d,$0a
ENTERKEYMSG_STRONG_END


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


ERRORRETURN			FCB		0


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


BIOSERRORCODE		FCB		0
