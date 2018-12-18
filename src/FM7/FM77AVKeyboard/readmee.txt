FM77AV/AV40 Infra Red Wireless Keyboard Emulator with IRToy

by CaptainYS (http://www.ysflight.com)





Introduction

This program emulates Infra-Red signals of a wireless keyboard for a Japanese Classic PC called FM77AV and FM77AV40.  You also need a deviced called IRToy (http://dangerousprototypes.com/docs/USB_Infrared_Toy).  You can buy from Amazon (https://www.amazon.com/dp/B01M03RQEQ).

I only have FM77AV2 and FM77AV40.  So, I cannot test with a FM77AV20, but it probably work with FM77AV20 as well.  At least it works perfectly with my test machines.  I could finish and see the ending staff roll of Xanadu Scenario 2 with this program.  I used a cheat data though.

It cannot be used for FM77AV40EX and later models because those models don't have an Infra-Red sensor.

Although Japanese keyboard is slightly different from US keyboard, if you set this program into the "Translation" mode, the program interprets what you typed and sends a correct key code to FM77AV.  Or, for playing games you may want to use "Direct" mode.  If you can read Katakana letters and want to type as Romaji, "RKANA" mode will interpret your Romaji typing and sends keycode for corresponding Katakana letters.





Usage

First, plug your IRToy.  Make sure the LED of IRToy is OFF.  

Once you plug your IRToy, start this FM77AV Keyboard Emulator.  If you are worried about securty of computer virus, read entire souce code and compile the source code by yourself.  If you still cannot be comfortable, please don't use it.

Then the program will scan COM ports and look for IRToy.

Then put your IRToy IR LED near FM77AV Infra-Red sensor and type to the program.  You will see letters on the monitor of FM77AV.

I am testing this program with IRToy v2 with firmware version v22.  If you buy a unit now, probably the unit comes with firmware v07.  This program may work with v07, but if it doesn't, you can let the device commit suicide and try upgrading the firmware.  (Unfortunately, this unit needs to die once to get firmware upgraded.)  But, the Priest who casts a resurrection spell is not of high level, the magic may fail, and the unit may become in the state of ASH.  If the second resurrection spell failed again, the unit may be permanently lost.  Unfortunately, pressing a reset button won't save the unit.  So, good luck!




Acknowledgement

For developing this program, Mr. Apollo (http://www.retropc.net/apollo/) helped me a lot.  Especially, he sampled some IR signal from his FM77AV keyboard, and suggested that the keyboard looked to be using 100us pulse.  Mr. Kobayashi of Classic PC and Retro Gaming JAPAN told me how to calculate key-release code from key-press code.  Without his help, I couldn't eliminate beeps when FM77AV complains that the keyboard signal is lost without key-release code.  Mr. Deskuma (http://d.hatena.ne.jp/deskuma/20110425/1303694255) published FM77AV keyboard serial connection, which was very accurate and helpful.





Updates
2018/11/14
- Auto typing.  Basically for alphabets and numbers only, but you can use escape \\\ to enter a special key, like \\\BREAK for break key.
  By using it, you should be able to type anything.  Also \\\WAIT for waiting 50ms.  It is useful to transfer a short program to FM77AV/AV40/AV20.
- Translation mode suppots <>_'{}, which were missing.

2018/02/14
- Confirmed to work with IRToy Firmward version 1.2 (have been tested for V2.2 before)
- Bug fix: Repeat-timer was not initialized, and the program may not send any keycode depending on when you start the program.

2018/02/04
- I was thinking that running the loop as fast as possible would be better.  But, when I compile in console-less mode, my FM77AV dropped the strokes very often.  I wondered if the program was not fast enough, but it was opposite.  Apparently, my program was taking 100% of CPU (or core) time, and the COM driver didn't have time to actually send packets.  By adding 1ms sleep every iteration, my FM77AV doesn't drop key strokes any more.

2018/01/31
- An option for using ZXC as left-mid-right space keys.
- Options for using arrow keys as 2,4,6,8 or 1,2,3,4,6,7,8,9 keys.

2018/01/26
- Added colon and semi-colon in the Translation mode.  I was forgetting about it.

2018/01/23 Version 1

Next Plan: 
(1) Select a text file and let this keyboard emulator type into FM77AV.
(2) Translate GamePad input to FM77AV keyboard.




About Source Code

It is BSD license.  You can use it for free.  But, I won't be responsible for any outcome from this program and the source code.





