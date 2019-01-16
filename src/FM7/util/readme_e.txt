Fujitsu FM-7/77 Series Resurrection Utility

-Programs that run on Windows
DISK2TAPE.EXE      Write Back D77 Disk Image through Data Record I/F
D77FILEUTIL.EXE    D77 Disk Image File Utility
T77SAVE.EXE        T77 Tape Image (and .WAV) Generator
T77LOAD.EXE        File Extraction from a T77 Tape Image

-Programs that run on FM-7 series computer
COMTEST            Serial connection tester
RAWREAD            Disk image reader and transmitter
232CFILE           File receiver for serial connection
MEM2COM            Memory dump to serial connection



by CaptainYS

http://www.ysflight.com



Updates

2018/01/31 DISK2TAPE: -fm77av option.  If this option is given, the writer program will use expanded RAM (64K) of FM77AV to reduce number of .WAV files.
  DISK2TAPE: -use7000 option.  If this option is given, the write program will use $7000-$7FFF to reduce number of .WAV files.  But, if this option is used, the writer program must be started from the tape mode.  If started from Disk-BASIC, it destroys the Disk-BASIC area.

2018/01/12 DISK2TAPE: Added an option for specifying the target disk drive, but I haven't tested it.  It may or may not work.

2018/01/10 DISK2TAPE: AV40 failed to write to a disk if running a program from DISK2TAPE after booting without a floppy disk in the disk drive.

2018/01/03 Version 1




Introduction

The goal of these utilities are for resurrect 8-bit computers from 1980s called Fujitsu FM-7/77 series.

For example, you can write a DEMO code for FM-7 on Windows, test it on the emulator, then you can make a .WAV file that can be read from an actual computer using T77SAVE.exe

Or, if you have a disk image for FM-7, but you don't have a physical disk of that image, you can use DISK2TAPE to create a set of .WAV file, which is read and run from the actual FM-7 with a disk drive to write the disk image back to the actual disk.  I succeeded in both standard F-BASIC format and non-standard 1024bytes-per-sector format.

For these goals, all I needed was to write something into T77/D77 and sending them to the actual disk.  I had no reason I needed a utility for reading files from T77/D77.  But, anyway I made that, too.  The source code works as a memo for future reference.

I totally forgot about the disk structure of F-BASIC until I reverse engineered disk images for this project, which means that I may not be able to fully understanding the disk structure.  So, make sure to take a back up copy of a D77/T77 file before editing it with my utilities.

I intensively used FTOOLS available from http://www.retropc.net/apollo/.  Also an excellent FM7-series emulator XM7 helped a lot while writing disk-formatting and sector-writing programs.  I appreciate all those developers involved in those projects.




////////////////////////////////////////////////////////////
D77FILEUTIL.EXE

To start a terminal mode with an existing .D77 file:
        d77fileutil d77file.d77
To start a terminal mode with a new .D77 file:
        d77fileutil
Command mode with an existing .D77 file:
        d77fileutil d77file.d77 <options>
Command mode with a new .D77 file:
        d77fileutil d77file.d77 -new <options>
Command options (case insensitive):
-h
-help
-?
        Show this help.
-new
        Start with a new .D77 file.  This option can only be immediately
        after the .D77 file name.  If -new is specified, and specified
        .D77 file already exists, and if the specified .D77 file is
        marked as write-protected, this option will fail and the program
        quits immediately.
-split
        Splits a multi-disk .D77 into multiple single-disk .D77 files.
        This option can only be immediately after the .D77 file name.
        File name will automatically be created.
        It does nothing if a new .D77 file name is already used.
        It does nothing if the input .D77 file is a single-disk image.
        All options following -split will be ignored.
-files
        Show files in .D77
-fat
        Show FAT dump.
-dir
        Show directory dump.
-rawread filename-in-d77 raw-file-name
        Read raw file from D77 and write to raw-file-name.
-fmread filename-in-d77 fm-file-name
        Read a file from D77 and write to FM-FILE format.
-binread filename-in-d77 bin-file-name
        Read a binary file from D77 and write to a file.
-kill filename-in-d77
        Delete a file.
-fmwrite fm-file-name filename-in-d77
        Write fm-file to D77.  If "()" or "[]" or " " or "" is specified as filename-in-d77,
        D77 file name will be taken from the name given in an fm-file.
-srecwrite srec-file-name filename-in-d77
        Write SREC file to D77.
        ASM6809 v2.9 writes wrong check sum (not taking a complement?).
        This program ignores the check sum to accept SREC from ASM6809 v2.9.
-save text-file-name filename-in-d77
        Save a text file as an ASCII-format BASIC file to D77.
-savem binary-file-name filename-in-d77 startAddr execAddr
        Save a binary file to D77.  Like to save a file you typed from I/O magazine.
        Unlike SAVEM command of F-BASIC endAddr is automatically calculated from
        the file size.  Therefore, you only need to give start and exec addresses.
        You can use &H of 0x to specify address in hexa-decimal number.
-unprotect
        Removes write-protected flag of the disk.
        It does not modify the original disk.
        You can un-write-protect, edit, and then save to a different .D77 file.
-protect
        Sets write-protected flag of the disk.



////////////////////////////////////////////////////////////
T77SAVE.EXE

Usage: t77save output.t77 <options>
Options will be interpreted from left to right.
Options:
-h
-help
-?
        Show this help.
-new
        Ignore existing output.t77 and make a fresh T77.
-fmwrite fm-file-name filename-in-T77
        Write fm-file to T77.  If "()" or "[]" or " " or "" is specified as filename-in-T77,
        T77 file name will be taken from the name given in an fm-file.
-srecwrite srec-file-name filename-in-T77
        Write SREC file to T77.
        ASM6809 v2.9 writes wrong check sum (not taking a complement?).
        This program ignores the check sum to accept SREC from ASM6809 v2.9.
-save text-file-name filename-in-T77
        Save a text file as an ASCII-format BASIC file to T77.
-savem binary-file-name filename-in-T77 startAddr execAddr
        Save a binary file to T77.  Like to save a file you typed from I/O magazine.
        Unlike SAVEM command of F-BASIC endAddr is automatically calculated from
        the file size.  Therefore, you only need to give start and exec addresses.
        You can use &H of 0x to specify address in hexa-decimal number.
-wav
        Save as .WAV.  Just specifying this option will create both .T77 and .WAV files.
        If you don't need .T77, also specify -not77
        By default, this program only saves .T77 file.
        This option only affects what file will be saved in the end.
        File name of the .WAV file will be same as argv[1] with .WAV extension.
-not77
        Don't save .T77 file.
        This option only affects what file will be saved in the end.





////////////////////////////////////////////////////////////
DISK2TAPE.EXE

This program generates a bunch of .WAV file from a .D77 disk-image file.  Output .WAV files can then be sent to the actual machine through a data-recorder interface cable.  If you have a bunch of disk images, but your original disks are already dead, you can re-create your disk by this utility.

However, it doesn't break the copy protection.  If the disk image includes F5, F6, or F7 sector, it won't write back.  There is an option to write them as E5, E6, and E7 sectors, but you also need to find the location of the protection and modify the code.  I believe that the law allows to break a copy-protection of historic games for the purpose of preservation.  However, I cannot write how to do it here.



[Usage]
DISK2TAPE image-filename.D77 <options>

[Options]
-deldupsec
  If the track has multiple sectors with the same number, drop duplicate ones.

-renumfx
  Write F5,F6, and F7 sectors as E5,E6, and E7 sectors.

-drive 0|1|2|3
  Specify which drive on FM-7 to use.  I haven't tested this option though.

[Output]
A bunch of .T77 files for dry-run on the emulator XM7, and .WAV files for actually writing.

[Writing Back Disk Images]
What are needed:

- Actual FM-7/77 machine with a floppy disk drive.
- A PC that pretends to be a data recorder.
- Data-Recorder cable.  If you don't have one, you make one.
- Patience.  One disk takes about 1.5 hours to write.

If you don't have a data-recorder cable, make one yourself.  I am a programmer and am not a hardware person.  I am not good at soldering at all, but I was able to make cables for FM-7 and FM-77.  You can do it as long as you have a very basic skill of soldering.

You can find pin assignments in the following URLs.

FM-7   http://www.retropc.net/apollo/knowhow/cable/index.htm
FM-77  https://www.beep-shop.com/blog/4274/

According to the above sources,

For FM-7 (8-pin DIN connector)
    +--+ +--+
   /   | |   \
  /    +-+    \
 /  7       6  \
/               \
|               |
| 3     8     1 |
|               |
\               /
 \  5       4  /
  \     2     /
   \         /
    +-------+
3 LOAD (GND)
5 LOAD (In)
1 SAVE (GND)
4 SAVE (Out)
6 Remote (+)
7 Remote (-)

This is a schematics of the connector on the PC from the outside.  Or, it is the soldering side of the connector.  You don't care about the remote, although it is very nice if I can write a hardware and program that can control play-back through remote.  If you mistake left and right, your phone and microphone connections are flipped, not a big deal.  If you flipped left and right, find a right connector to plug in.  What's important here are:

(1) Pins 1-4 and 3-5.
(2) Don't mistake inside and outside of the audio cable.  Most likely the outside wires of the audio cable is ground.  Use tester to make sure.  If you don't have a tester, buy a mini light bulb, 1.5V battery, and cord to make a tester.
(3) Probably there is no monaural audio connectors any more.  Use stereo cable.

By the way, GND is the root of the plug.  In/Out are the two tip connections.




For FM-77(mini 5-pin DIN)
    +--+ +--+
   /   | |   \
  /    +-+    \
 /  5       4  \
/               \
|      +-+      |
|      | |      |
|  3   +-+   2  |
\               /
 \             /
  \        1  /
   \         /
    +-------+
1 GND
2 Remote (+)
3 Remote (-)
4 Save
5 Load

There are left and right assymmetric.  You don't have to be worried about flipping left and right.  Pin 1 should be ground.  You don't need center 2 pins.
Once you have a cable, you need a PC that pretends to be a data recorder.  In my experience, you need to maximize the volume, or FM-7/77 won't hear the signal.  

So far, I have succeeded with 2008 MacBook Pro and Dell XPS desktop.  I failed with ThinkPad X250.  X250 looks to be outputing the correct wave signal.  My guess is its maximum volume was too low for the purpose.

By the way, it has nothing to do with disk write-back, but if you happen to have a .WAV saved from FM-7 through data-recorder cable, you need to maximize the volume of the sample, or FM-7 won't hear it.


Then, use DISK2TAPE.EXE to generate a bunch of .WAV files.  Again, DISK2TAPE.EXE does not break copy protection.  If the D77 disk include copy protection, you somehow reverse engineer the code and break copy protection yourself.  1-bit in .D77 is converted to roughly 10 to 20 bytes in .WAV.  So, 320KB floppy disk image will turn into 80MB of .WAV files.


Once you have .WAV files, use real FM-7 to write back to disk.  Connect your data-recorder pretender to real FM-7 and type in the real FM-7 as:

RUN "CAS0:"

and start play-back ???_01.wav.  The first program is for formatting a disk.  Once prompted, insert a new disk in drive 0 and press RETURN key.  (We used to call Enter key as RETURN key.)  By the way, it formats the disk, therefore the content of the disk will be lost.  Do not insert an important disk.  Keep your important disks always write-protected.

Once formatting is done, the do the same for the rest of .WAV files.  The order doesn't matter for the rest of the .WAV files, but not to confuse yourself it's a good idea to follow in the order of the file name.

I use Audacity for playing .WAV file.  If you do the same, make sure you DON'T zoom in to a part of the WAV file while playing.  If you do so, Audacity will move the window when the cursor moves out of the window.  That fraction of fraction of second will give you "Device I/O Error".  I never failed when I don't zoom in.

Good luck!




////////////////////////////////////////////////////////////

RS232C Utilties

RS232C utilties runs on FM-7 series computer and transmit data to/from a PC serial port.  Disk2tape program was sending a disk image file to actual FM-7 series computer, but one of the utilities RAWREAD can be used for creating a disk image from an actual floppy disk.  RAWREAD uses read track, read address mark, and read sector commands to read as much information as possible from a floppy disk, and the information is transmitted to a PC via the serial connection.  The information is received by an any COM terminal, which can then be converted to .D77 file format that can be understood by an emulator.

On Windows (or mac or Linux), you need to use a COM terminal software.  I am using TeraTerm in my environment.  The COM port setting must match FM-7 side and Windows side.  RS232C (serial) card for pre-FM77AV40 models require dip-switches to be set correct position.  FM77AV20/40 and later models can configure serial port programatically.  RAWREAD will set it up the following.

    19200bps
    1 Stop bit
    No parity
    Hardware Flow Control

The problem is how to send the programs to an FM-7 series computer.  Once you send 232CFILE (and machine-language part 232CFILM) to an FM-7 series computer, you can use 232CFILE to send other programs.  But, first you need to somehow send this program to an FM-7.

There are multiple options.  If you are using FM77AV Keyboard emulator, you can use an auto-typing feature.  Basic file (.bas) can be directly sent via the keyboard emulator.  Machine-language (assembled) part need some BASIC commands.  For that purpose this package has file with .type extensions, which can be fed to auto-typing of FM77AV keyboard emulator.

You can use my Disk2Tape program to send a disk image of the .D77 file in this package to actual FM7/77AV computer.

Or, you can use .WAV file to send the program via audio-cassette interface.

Once you have 232CFILE and 232CFILM on the FM-7 side, type RUN "232CFILE" to start the program, and then transmit other program from the Windows side.  To transmit a program, cut & paste texts in .232C files in this package into the terminal.

But, even before 232CFILE, you need to confirm you established a connection.  I recommend to send COMTEST and COMTESTM files for testing the connection.





- COMTEST
Files: COMTEST, COMTESTM

Running the program:
RUN "COMTEST"

On Windows side, start a COM terminal software.  Then on the FM-7 side, run this program.  If everything is set up correctly, "Hello from FM-7!" will be shown on the COM terminal.  If you type a few letters and enter key on the COM terminal, FM-7 will print your message, and send back "PING!" to the terminal.

If the COM port settings don't match, you may receive a meaning less text, or some letters may be missing.  Unless you confirm the connection with this tool, there is no point to use other utilities.





- 232CFILE
Files: 232CFILE, 232CFILM

Running the program:
RUN "232CFILE"

This program receives from the COM terminal text-encoded stream, and save to the FM-7 side disk or tape.  You start the program, and then you don't have to do anything on the FM-7 side.  From the COM terminal, you just need to paste an encoded text.  But, if you accidentally press a key on the terminal before sending a file, 232CFILE may get confused.  To avoid this problem, you can press ESC and then Enter to clear the FIFO buffer, and then transmit a file.

File encoder for Windows: rs232cTfr.exe
Usage:
    rs232cTfr machingo.txt -srecwrite binary.srec
    rs232cTfr machingo.txt -savem binary.bin 0x1000 0x1200
    rs232cTfr basic.txt -save asciitext.bas

It encodes a file to the format that 232CFILE can receive.  It supports SREC file, a binary file, or an ASCII test (F-BASIC program).  If you encode a plain binary, you need to specify the store-address and exec-address.

How it is done is written in the source code, which is included in this package.  You can read C/C++, right!?




- RAWREAD
Files: RAWREAD, RAWREADM

Running the program:
RUN "RAWREAD"

.D77 disk image format, used in some Japanese 8-bit emulators, can represent more complex formatting compared to a plain disk image file, which typically assumes same sector length and same number of sectors per track.  However, it does not perfectly re-enact tricky disk formats, and some very special copy protection cannot be simulated by the .D77 file.  Actually, copy-protection technologies is a very precious part of computer history.  It is an art.  I want to preserve those copy-protections for the future references.  However, once you encode it to .D77 disk image, too many information is lost.  To preserve a nearly-perfect copy of a disk, you can use a device like KryoFlux.  However, I have a problem with KryoFlux, which is, I don't own one.  So, I tried to do as close to KryoFlux as possible with actual FM-7 series computer.  The Floppy Disk Controller of FM-7 series computers use three kinds of read-commands, read track, read address mark, and read sector.  RAWREAD use all three to get as much information as possible.  It still doesn't get all the information if the copy protection is very special, but it does pretty good job.  The extracted information is sent to the COM terminal via the serial connection.  Once the information is received, you can choose to convert the data to .D77 file format by rawReadToD77 program.  It is good enough to simulate a simple copy-protection on the emulator.

To use RAWREAD, start a COM terminal on the Windows side, and start logging.  Then start RAWREAD on the FM-7 side.  Insert a disk that you want to transmit, specify drive number, disk name, starting and ending tracks, Y for transmit or N for just inspect, and press key to start reading.  One disk takes about 40 minutes to transmit with 19200bps.  If you only need a .D77 image, it can be done at 20 minutes or so, but this program sends extra information for preservation purpose.  If you don't like it, look at the BASIC program, and delete transmittion except SECTORINFO and SECTORDUMP.


RAWREAD to D77 converter: rawReadToD77
Once you have a log file of the transmission, go to command prompt and type:

    rawReadToD77 logfile.txt diskimage.d77

to create a .D77 image file.




- MEM2COM
Files: MEM2COM, MEM2COMM

é¿çsï˚ñ@:
RUN "MEM2COM"

Transmit FM-7 memory dump to a PC.




- D77ToRS232C

Please read readme_D77ToRS232C_e.txt for more details.
