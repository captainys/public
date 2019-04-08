FM-7/77 Disk BIOS Redirector
Run a Disk-Based Titles on Fujitsu FM-7 8-bit Retro PC via RS232C
by CaptainYS
http://www.ysflight.com


10 REM  -------- Introduction

This is a program for running a floppy disk-based titles on actual Fujitsu FM-7 8-bit computer without having a disk drive via RS232C connection.  If your unit is earlier model than FM77AV20/40, you need to add an RS232C interface card.  It is very difficult to find a Fujitsu-original RS232C card for FM-7 series in working condition (very often a ribbon cable is gone).  But, you can fabricate a compatible card with the information here.

I am planning to some of the compatible cards on Yahoo auction, but if you want one in the United States, I can sell one to you when I have a stock.  Please contact  if you are interested.

The original FM-7 does not come with an internal floppy disk.  Getting an external floppy disk drive and floppy-disk interface card for FM-7 is very difficult and pricy.  But, with this program, you can run some disk-based titles if you have a D77 disk-image file.

The disk titles you can run are limited to the ones that access disk via BIOS calls only.  Memory-mapped I/O addresses of FM-7 was open, and therefore many programs directly accesses the I/O addresses.  Such titles do not call BIOS functions, and there is no way to redirect such I/O accesses.  Also the server program alters sector data so that the executable binary calls BIOS hook functions instead of BIOS functions.  Therefore the redirector does not work if the program is encrypted.  I suppose majority of the programs using F-BASIC should work with the redirector.





20 REM  -------- Usage

To use this program you need a working Fujitsu FM-7 hardware.  I am not talking about an emulator.  I am talking about an actual hardware.  You also need a Windows PC that works as a server.  The Windows PC and the FM-7 must be connected by an RS232C (aka serial) cross cable.  Modern PCs don't have an onboard RS232C connector.  You probably want to get a USB to RS232C adapter like Sabrent USB to RS232C adapter:

You need to type a short program on FM-7.

10 OPEN "I",#1,"COM0:(F8N1)"
20 LINE INPUT #1,A$
30 CLOSE
40 EXEC VARPTR(A$)

Then type RUN to start the program.  Your FM-7 waits for the installer from the server.

On Windows PC, start CMD.EXE or PowerShell and type:

    RS232CDiskBios.exe diskimage.d77 1

to start the server.  I am assuming that the PATH is set to the location where you store RS232CDiskBios.exe, but if not type full- or relative-path to the executable.  The number "1" here is the port number.  You can find it in the Device Manager.

Once the server starts, type:

IL

on the server prompt.  FM-7 will restart and boot from the disk image you specify in the server command line.

If your FM-7 series hardware is FM77AV20/40 or newer, you first need to enable RS232C before the above steps.  Start your unit, and type the following.

POKE &HFD0C,5
POKE &HFD0B,16
EXEC -512

If you are using FM77AV1/2 or earlier model, don't do this.  If RS232C card is already installed, it is enabled as soon as you power on.  Also POKE &HFD0B,16 will hang your FM-7 because $FD0B works the same as $FD0F where writing to this address will switch F-BASIC ROM into RAM mode, and immediately your prompt will be gone.





30 REM  -------- Advanced Server Usage
The BIOS redirector by default resides $FC00-$FC63.  If the program you want to run also uses this area, the redirector will be destroyed, obviously.  To avoid the collision, you can specify different address where the redirector is installed by the server command parameter.

    RS232CDiskBios.exe diskimage.d77 1 -install $6F80

If you type like this the redirector will be installed from $6F80.  Or, if you are running a Disk BASIC based program, most likely you can install the redirector from $7F25.  This is the address where Disk BASIC stores error messages.  As long as your program doesn't run into an error, you don't need error messages.  However, the redirector needs to stay somewhere else while the IPL is reading Disk BASIC, or the IPL will overwrite the redirector and crash.  For this purpose, you can use the following option.

    RS232CDiskBios.exe diskimage.d77 1 -install2 $7F25

If you start the server this way, the redirector will stay $FC00- while the IPL is loading Disk BASIC, and then it is installed again to $7F25- once IPL finishes loading Disk BASIC.





40 REM  -------- Server Command Options

[Basic Usage]
    RS232CDiskBios.exe diskimage.d77 portNumber

Start server and make ready to send the specified disk image.  If the disk image is a multi-disk image, the first disk is mounted on virtual drive 0, and the second disk is mounted on virtual drive 1.  BIOS redirector will be installed $FC80-.

You can specify image-file options by writing # after .D77.  Please see below for D77-file options.



[Redirector Install Address (-install)]
    RS232CDiskBios.exe diskimage.d77 portNumber -install 7F90

This example will install the BIOS redirector to $7F90-.  Don't put anything before the 4 hexa-decimal digits.  No $, 0x, or &H.  If the FM-7 program that you want to run ends up using $FC80-, this option may help.  But, to specify install address in Disk BASIC, use the next -install2 option instead.



[Redirector Install Address for Disk BASIC (-install2)]
    RS232CDiskBios.exe diskimage.d77 portNumber -install 7F25

Specify where the redirector must reside in Disk BASIC.  In this example the redirector will be installed from $7F25-.



[Specifying Disk Image for Virtual Drive 1 (-1)]
    RS232CDiskBios.exe diskimage.d77 portNumber -1 drive1.d77

This example specifies a disk image mounted on virtual-drive 1.  If the primary disk image is a multi-disk image, and if you want to mount a disk other than the second in the image, you can write #DSK0#3 instead of the file name.  #DSK0#3 takes the fourth disk in the primary disk image file.

If you want to use the same disk-image file for drives 0 and 1, do not write file name twice.  If you do so, I don't guarantee what's going to happen when you write to a disk from FM-7.



[Redirect LD? #$FE0?]
    RS232CDiskBios.exe diskimage.d77 portNumber -ldxFExx
    RS232CDiskBios.exe diskimage.d77 portNumber -ldyFExx
    RS232CDiskBios.exe diskimage.d77 portNumber -lduFExx

Some programs call BIOS as LDY #$FE08  JSR ,Y  These options also redirects such instructions.



[Encoder]
    RS232CDiskBios.exe diskimage.d77 portNumber -encoder XOR xx
    RS232CDiskBios.exe diskimage.d77 portNumber -encoder NEG
    RS232CDiskBios.exe diskimage.d77 portNumber -encoder COM

Well, it's more like a encryption.  In some programs the server needs to take byte-by-byte XOR, NEG, or COM to recognize the program and redirect.  This option specifies what kind of encoding the server must test.  xx should be two digit hexa-decimal number.  Nothing in front of the number.



[Sector Substitute]
    RS232CDiskBios.exe diskimage.d77 portNumber -subst drv trk sid sec data.srec/.txt

The server will substitute a sector contents with the specified data.  The data can be either .SREC format file or a tightly-packed hexadecimal numbers written in a text file.  This option does not modify the input .D77 file.  When you need a special IPL for booting a program from RS232C, you can use this option like:

    RS232CDiskBios.exe diskimage.d77 portNumber -subst 0 0 0 1 ipl_for_rs232c.txt

Track 0 Side 0 Sector 1 is where FM-7 reads when powered on.



50 REM  -------- Image File Name Options

You can specify file-name options by writing # following .D77 without space.  Also possible to specify multiple options by concatinating.

[New Disk (#NEW)]
    diskimage.d77#NEW

This creates a standard F-BASIC format (256 byte per sector, 16 sectors per track) and mount.  If the same-name D77 file exists, the server will give you an error.



[Force New Disk (#FNEW)]
    diskimage.d77#FNEW

This creates a standard F-BASIC format (256 byte per sector, 16 sectors per track) and mount.  If the same-name D77 file exists, the fill will be overwritten when FM-7 writes something to the disk.



[Disk Number (#0)(#1)(#2)(#3)...]
    diskimage.d77#3

Specifies the disk-number of the multi-disk image.



[èëÇ´çûÇ›ã÷é~ (#WP)]
    diskimage.d77#WP

Mount disk image write-protected.





60 REM  -------- Server Command Prompt

[Transmit BIOS Redirector Loader Binary (IL)]
    IL

Type this command while FM-7 is waiting for the redirector loader.  The server will send 0x7E-byte loader to FM-7.  From there the loader will communicate with the server, download the redirector installer, and then jumps to the installer, from which FM-7 boot into the D77 disk image specified on the server command line.  It works much faster than IA command.

FM-7 wait program for IL command looks like:

10 OPEN "I",#1,"COM0:(F8N1)"
20 LINE INPUT #1,A$
30 CLOSE
40 EXEC VARPTR(A$)



[Transmit BIOS Redirector Installer (IA)]
    IA

Type this command while FM-7 is waiting for the redirector installer.  The server will send 256-byte installer to FM-7.  Due to F-BASIC limitation sent all ASCII.  Don't worry.  The redirector itself transmit/receive binary.

FM-7 wait program for IA command looks like:

10 OPEN "I",#1,"COM0:(F8N1)"
20 FOR I=0 TO 255
30 INPUT #1,A%:POKE &H6000+I,A%
40 NEXT
50 CLOSE
60 EXEC &H6000


[Verbose Mode On/Off (V)]
    V

V command toggles verbose mode.





70 REM -------- 

80 REM -------- Confirmed Programs

[ ] tells tested hardware.

"2" in the command examples is the port number.  It should be replaced according to your environment.

(*) means you need to somehow disassemble and remove direct Disk I/O access.



F-BASIC 3.0 (Fujitsu) [FM-7,FM77AV40]
Starts no problem.
If the program you want to run crashes, you may salvage it by adding the option:
    -install2 7F25
This will install the BIOS redirector in the error-message area.  No problem as long as your program won't run into an error.
From Disk BASIC I confirmed:
    FM-7 DEMO1 [FM-7,FM77AV40]
    FM-7 DEMO2 [FM-7,FM77AV40]
    Emergency [FM-7,FM77AV40]



URADOS (I/O) [FM-7,FM77AV40]
Starts no problem.  However, the conventional RAM area available will be up to $7F7F.  128-byte fewer than the original URADOS, which allows all the way up to $7FFF.  URADOS uses $FC00-$FC7F.  Therefore the BIOS redirector needs to reside $7F80-.



R-DOS (I/O) [FM-7]
R-DOS needs the option:
    -dosmode
By the way, DOS mode starts FM-7 with $8000-$FBFF RAM mode (otherwise F-BASIC ROM uses this area).  The installer will emulate this setting.



(*)Thexder (Gamearts) [FM-7,FM77AV40]
$FC00-$FCFF is used by Thexder.  The BIOS redirector needs to reside $0040-.
    Command Example:
        RS232CDiskBios thexder.d77 2 -install 0040



(*)MAGUS (Soft Pro) [FM-7,FM77AV40]
$FC00-$FCFF is used by MAGUS.  The BIOS redirector needs to reside $0040-
    Command Example:
        RS232CDiskBios magus.d77 2 -install 0040



(*)Take the A-Train (Artdink) [FM-7,FM77AV40]
Thie program uses almost entire RAM area while starting up.  But, this is Disk BASIC-based.  So, you can run this program by installing the BIOS redirector from $7F25.
    Command Example: (Option is -install2 not -install)
        RS232CDiskBios magus.d77 2 -install2 7F25



(*)Strategic Confrontation (aka Daisenryaku) (System Soft) FM [FM-7,FM77AV40]
Starts with no problem.



(*)Nobunaga Whole-Country Version (Koei) [FM77AV40]
I confirmed to enter the hex-battle mode and out.  Probably it runs all the way.
    Command Example:
        RS232CDiskBios.exe nobuzen.d77 2 -install DE40 -encoder XOR 1 -ldyFExx -dosmode



(*)Laydock (T&E)
IPL needs to be modified.  Started with the default settings.
Confirmed at least the game starts.





90 REM -------- Technical Information

Fujitsu FM-7 BIOS provides with the three types of disk access functions, RESTORE ($08), DWRITE ($09), and DREAD ($0A).  F-BASIC 3.0 uses these three service functions for reading/writing floppy disk sectors.  If you redirect these three service functions to RS232C, you can run Disk BASIC-based programs without having a floppy disk drive hardware.  Non-Disk BASIC-based titles can run if it uses the three service functions and does not access floppy-disk I/O directly.

However, if the program calls BIOS directly, having BIOS redirector on the RAM doesn't do anything good.  Therefore, the server program that sends sector data to FM-7 look for the 6809 instructions that calls BIOS and alters the sector data before sending to FM-7.  By the way, because the server alters the program if you LOADM and SAVEM back again, your program may be altered, and therefore not recommended.

Namely, the server looks for the following instructions:

	JSR		[$FBFA]
	JMP		[$FBFA]
	JSR		$F17D
	JMP		$F17D

Also the internal routines addresses are published, and many programs used those addresses.  Therefore the server also looks for:

	JSR		$FE02
	JMP		$FE02
	JSR		$FE05
	JMP		$FE05
	JSR		$FE08
	JMP		$FE08

If the program is calling BIOS by relative jump, LBSR or LBRA, it is impossible to redirect unless the server knows the location the sector is loaded.  URADOS (I/O) calls DREAD by LBEQ $FE08.  But, the server program positively identifies URADOS boot IPL and modifies accordingly.  Otherwise, if it is an IPL sector (Track 0 Side 0 Sector 1), it is known to be loaded to $0100.  Then I can calculate the relative jump destination.  So, LBSR and LBRA will be redirected only if it is in the IPL sector.

Some programs call DREAD, DWRITE, and RESTORE routines as:

	LDY		#$FE08
	JSR		,Y

In most cases it is safe to re-direct LDY #$FE08 to LDY #REDIRECTOR_ADDRESS.  But, to be safe this redirection is enabled only from server command parameters.

Also F-BASIC calls the BIOS via the following vector:

00DE	7EF17D		JMP		$F17D

The redirector installer also writes the redirector address to [$00DF,$00E0].



The next challenge was to support Disk BASIC.  The Disk BASIC IPL loads Disk BASIC binary from $6E00 to $7FFF, and then sets #$6E00 to X register, raise the carry flag, and JMP [$FBFE].  X register is then used as a call-back address after the F-BASIC ROM initializes.  The problem is that F-BASIC clears the conventional RAM before the call-back address.  Also resets [$00DF,$00E0].

To work around, the server alters the IPL so that X register is set to the redirector-installer address instead of #$6E00 at the end of IPL.  So, in this case, the redirector installer is called twice.  The first time, the redirector reads the IPL to $0100 and jump there.  The second time, it needs to behave differently.

To let it behave differently for the second time, the installer re-writes itself after the first installation.  When it is called second time, it installs the redirector and jump to #$6E00.  Actually #$6E00 will be configured when the server transmits the installer.  URADOS uses #$4D00, but the server sets up the redirector installer to call $4D00 after installation.



This allowed to boot Disk BASIC and URADOS from RS232C.  It was unbelievable.  But soon I encountered the programs that use RAM area $FC00- where the redirector is installed by default.  I had to make the install-address configurable.  That was easy, but some programs really used up pretty much entire RAM space except the Disk-BASIC area $7000-$7FFF.  I found roughly 128 bytes of unused (unused as long as there is no error) space from $7F25.  This area is for storing Disk Error messages.

But, that area is overwritten while IPL is loading the Disk BASIC.  The redirector needed to reside in a different location, and then during the second installation it needed to be installed to $7F25.  To make it possible the redirector installer takes two install addresses.  With -install2 option, the redirector takes a different install address for the second installation.  During the first installation, the installer modifies itself so that the second install address is used in the second installation.



Regarding the redirector installation with the IL option.  F-BASIC stores an ASCII string, say A$, in the RAM as:

    [VARPTR(A$)]    1 byte    Number of characters
    [VARPTR(A$)+1]  2 bytes   Address points to the first letter of the string

If the server sends a string that consists of exactly 0x7E (126) bytes, because 0x7E is JMP instruction of 6809, I can EXEC VARPTR(A$) to jump to the program stored as a string.

Challenge is how to transmit a program with INPUT # or LINE INPUT # command.  F-BASIC interprets 0x0D as the end of string, and ignores all other byte less than 0x20 unsigned.  To get around, the server uses 0x20 as an escape.  When sending a byte X, the server actually sends:

    X               if 0x20<X
    0x20 and COM(X) if X<=0x20

The beginning of the program is the decoder.  The decoder part is written with instructions all above 0x20.  It was tricky.  Like if I write LDA #2, the assembler will output 0x86 0x02.  But, I need to avoid 0x02.  What I can do was CLRA INCA INCA or LDA #-2 NEGA.  LEAX LOADER_PART,PCR gave me too-small offset.  So, I did LEAX LOADER_PART+$2F2F,PCR and then later LEAX -$2F2F,X.  I had one BNE NO_ESCAPE that required an offset of 2.  I had to tentatively do BNE DIFFERENT_LOCATION and then overwrote the offset.

When you set FM-7 on wait and type IL on the server, the server sends a loader program with this method.  When EXEC VARPTR(A$), FM-7 jumps to the program stored in the string, decode it, and then sends a binary redirector request command "YAMAKAWA" to the server.  The server then sends 256-byte redirector installer to the client.  The loader expands the installer from 0x6000 to 0x60FF and JMP to 0x6000.  The installer installs the redirector and the boot from the D77 image you specify on the server command line.
