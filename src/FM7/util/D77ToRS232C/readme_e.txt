D77toRS232C - D77 Disk Image Burning Utility via Serial RS232C for FM-7/77 PCs

by CaptainYS (http://www.ysflight.com)




Introduction

DISCLAIMER: If you want to try this program, do it on your own risks.  I won't take any responsibilities for the consequences.

If you are interested in owning and keeping a Fujitsu FM-7/77 series 8-bit computer, still a working unit is very often on Yahoo auction.  Now a service called buyee.jp (https://buyee.jp) bit, pay, and forward the item on behalf of you.  It is possible to own a piece of Japanese computer history.

But, the problem is how to get a working floppy disk.  If you have a disk image that you want to play on an actual unit, the options are limited.

You can stream disk image via audio-cassette interface (http://########).  To do so, you need to fabricate or somehow get a audio-cassette interface cable for FM-7/77.  The bad news is FM-7 and FM-77 use different connector.  It is very difficult to find an audio-cassette interface cable for FM-77.

Let's say somehow you fabricated or acquired one.  Then, you need to play .WAV file on your PC and feed it to FM-7/77.  The problem is some PCs works fine, but some don't.  My 2008 MacBook Pro and Dell XPS desktop work fine.  I play .WAV file at max volume, and my FM-7/77 have no problem recognizing it.  All of my ThinkPads doesn't.  Also a new Dell desktop that came to my desk this year doesn't work, either.  I have a feeling that you have less than 50% chance that your PC can play .WAV file loud enough for FM-7/77.

Or, if you happens to have a rare software title for FM-7/77 and want to make an image for back up.  There used to be a DOS tool called DITT.exe.  But, it runs on DOS.  In my environment, it didn't run on DOS for Windows 95 installation.  It really needs to be a pure DOS.  There is another tool called NDITT.exe that runs on Windows XP.  However, this tool fails to recognize sectors with altered CHRN for copy protection.  If it at least reads sector data and writes to the image file, you can look into it and find how the copy protection worked.  It's also a piece of history.  After many failures from NDITT, I wrote a program that captures a disk image on actual FM-7/77 and transfer to a PC via a serial cable (http://########).  For me, I have already transferred the program to a 3.5 inch floppy, but for you, you somehow need to write to a 3.5 inch 2D floppy disk that can be recognized by FM-7/77.

What about transferring a disk image to actual FM-7/77 via serial connection?  Sure.  It can be done.  But, FM-7 series PCs, before FM77AV20/40, does not have on-board serial RS232C port.  It is virtually impossible to find an original Fujitsu RS232C expansion card for FM-7/77.  But, now we can fabricate it.  (http://########)

So, to free FM-7/77 series computers from isolation, I wrote a disk-image server that runs on Windows, and a client that runs on FM-7/77 and write an image to a disk.





Salmon or Ikura Problem

So you set up your FM-77 with RS232C card.  Your floppy drive is working in perfect condition.  You have a serial cross-cable.  If you can run this client on actual FM-7, you are good to burn D77 disk images into actual disks.

But, how can you transfer the client program to an actual FM-7/77?  Once you have a client, no problem, you can transfer a D77 image with the client to FM-7/77, but to transfer the image, you need a client.  You get stuck in a situation known as salmon and ikura.  (Was it chicken and egg?)

So, for the first step, you need to write a very short F-BASIC program to make a disk with RS232C utilities (including the client program).  Follow the steps below.

(1) Connect your FM-7/77 and a PC with a RS232C (serial) cross cable
First of all, you need to physically connect your FM-7/77 and a PC.  Make sure to use a serial cross cable.  Not a straight cable.  Or, you can use a straight cable plus a null-modem adapter.  But, the straight cable alone won't give you a disk.

If you somehow own a Fujitsu original RS232C expansion card for FM-7, make sure you turn on only dip switches #5 and #6.  All other switches must be off.  In particular, after Carnegie Mellon Computer Club looked at the original card, they concluded that there is a good chance of burning a chip if you turn on multiple of dip switch #1 to #5.  Make sure #1 to #4 are all off, and #5 is on.  Or, if you fabricated from the schematic, make sure you put a jumper only on JP5.

This way, you will have 19200bps connection.

(1.5) Enable RS232C if you are using FM77AV20/40 or newer model.
FM77AV20/40 and newer models come with an onboard RS232C connector.  But the RS232C is default off.  To enable RS232C from F-BASIC, you need to type:

POKE &HFD0C,5
POKE &HFD0B,16
EXEC -512

And wait for 10 or 20 seconds until the unit restarts.  This step is only necessary if you want to use a serial connection from F-BASIC (and you need F-BASIC for making the fisrt utility disk.).  Once you have a utility disk, you don't have to do it again.

(2) Start server
On Windows PC, go to command prompt, and change directory to the location where you store the server (D77Server.exe), and type:

    .\D77Server "####UTILDISK####" 3 -fbasic

By the way, the second parameter "3" is the port number.  Please replace with the number in your environment.

(3) Type the following program on FM-7/77

100 CLEAR ,&H17FF:DEFINT A-Z:WIDTH 80,25:A=&H1800
110 OPEN "I",#1,"COM0:(F8N1)"
120 OPEN "O",#2,"COM0:(F8N1)":PRINT #2,"REQCLI":CLOSE #2
130 INPUT #1,D$:PRINT ".";
140 POKE A,VAL("&H"+D$):A=A+1
150 IF LEFT$(D$,1)<>"Q" THEN 130
160 ' POKE &H1802,0
170 EXEC &H1800

And then type:

RUN

to start the program.  It receives about 2.5KB program from the Windows PC.  In this case, F-BASIC does not run fast enough to take full advantage of 19200bps.  It would take about a minute to receive all the bytes.

Once it is done, insert a raw 3.5inch 2D or 2DD (they are really the same thing) disk in Drive 1, and press key.  The bootable utility disk will be created.

Then, move the disk to Drive 0 and reset.  You'll boot into Disk BASIC (F-BASIC with disk extension.)



