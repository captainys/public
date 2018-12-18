2D Retro RPG Map Tool by CaptainYS



Author:      CaptainYS (PEB01130@nifty.com)
Web Site:    http://www.ysflight.com
License:     Free
Source Code: Open Source, BSD License
Platform:    Windows, macOS, Linux (Please download a package for your platform.)





[Disclaimer!]
This program saves a 1x1 scale bitmap of retro (not necessarily retro, but any 2-dimensional) Role-Playing Games.  You can start your game-solution site by using this program, if you choose to.

But, I STRONGLY RECOMMEND NOT TO POST MAP DATA IN ITS NATIVE DATA FORMAT (.retromap format).  Instead, this program can save maps as a large .PNG image.  If you want to post maps, I suggest to upload those .PNG format.  That way, the visitors can see the image without installing this program on their computers.

The reason is because the .retromap data format stores the original screenshots.  You can trim them in the program, but the original image is remaining in the data so that you can un-trim the screenshot later.  What if your screenshots are showing what you don't want other people see?

Actually, this program turned out to be practically useful.  When you want to make an image of a web site that you need to scroll down to see all contents, and print-to-PDF gives you a mess, you can take screenshots of the web site, and stitch them together in this program.

But, your web-site screenshot may include your bookmarks.  Bookmarks may leak important information.  If I end up leaking like grading information of my students, my real career will be at risk.  Or, you may end up exposing bookmarks that you wanted to keep to yourself.

You should pay extreme attention before making a screenshot public in general.

I won't be held responsible for any damage or career change or life-style change caused by this program.  Unless you can agree with this condition, please stop using this program immediately.





[Introduction]
I originally wrote this program for a very specific purpose: clear a retro role-playing game called "Xanadu Revival 2" by Nihon Falcom.

Why don't I look at a game-solution or cheating site?  I rather want to explore myself.

The idea is so simple.  I take screenshots, trim, and stitch them together to make a 1x1 pixel-scale map.  Therefore, this program must work for other 2D games if you are able to take a screenshot in .PNG format.  This program works for iOS games because you can use Photo Stream to send screenshots to your PC as a PNG image.

When I wrote the basic functionalities of this program, I didn't have a package "Xanadu Complete Collection" that included the Windows port of Xanadu Revival 2.  I was waiting for a co-worker who was visiting Japan then and was picking up the package for me.  But, I had a different package called "Dragon Slayer Chronicles" which had a Windows ports of Xanadu and Xanadu Scenario 2.

By the way, if you haven't heard about Xanadu, it is a legend in a Japanese Retro Role-Playing Games.  That was when I was an 8th grade, so it must be around 1985.  Personal Computers were not popular at all.  It was rare to have a computer at home.  Many people were skeptical about computers or even saying computers were harmful without any scientific evidence.

In that circumstance, Xanadu Scenario 1 sold 0.4 million copies.  If you scale it up to the current market size, it would be a billion-hit.  I played through so many rounds hours and hours in the ruin of Xanadu.  Actually I ended up with purchasing three re-make versions.

Anyways, I dived into the ruin of Xanadu again to test this program.  It was very successful.  Even it gave me an idea of an assignment for my programming course.  It was enough to test this program before Revival 2, but I wanted to do the same with Scenario 2.  So I did, and it was very much fun.

When Falcom released Xanadu, I had an 8-bit computer called FM-7.  My parents didn't buy me a Nintendo Entertainment System (called Family Computer in Japan).  But, they purchased a computer.  By the way, FM-7 had two CPUs running simultaneously.  One CPU was in charge of graphics.  It was also possible to send custom program to the CPU that was mainly in charge of graphics to do general-purpose processing.  Yes, it was doing GPGPU in 1982.  It's nothing new.  Computer got faster.  Better managed.  But essentially it is the same.

In the following winter, I switched to an upgraded version called FM-77AV.  Xanadu was the first game I bought for 77AV, and that time I played through by looking at a cheating book.

Then Falcom developed Xanadu Scenario 2, and I really wanted to explore by myself.  But, I was young, fool, and inexperienced.  Xanadu Scenario 2 was too much for me.  I tried, but I was not good enough to save the world.  I had to use a cheating book to finish it.

Almost 30 years later, I found that a package of Xanadu re-make versions called "Xanadu Complete Collection" was available.  It included the last in the series "Xanadu Revival 2".  Revival 2 was only available for PC-9801, which I didn't have.  This was my veyr first opportunity to play Revival 2.

I was determined to finish it without looking at a cheating book or cheating site.  To do so, I need to be able to make a map.  Well, I am not young and fool anymore.  I can combat it with my programming.

So, I made this program and defeated King Dragon in Revival 2, which disfigured from the original and looked pretty like Godzilla.

Then, my friend suggested to play "Chrono Trigger" on iPhone.  I used this program to make maps, and finished it without looking at solutions.  Only solution I looked at was for "Death Mountain" because my friend told that it was unreasonably difficult to do it with touch UI.  I played through two rounds without looking at any other solution information.  After I finished two rounds I finally looked general solution sites to see if I played right or I was doing in a hard way.

Well, maybe someone else has already written something similar.  But, what's wrong with adding one?



[What this program can do]
Creating a 1x1 scale map of a 2D game.
When you want to save a web site but saving to PDF breaks the layout, you can take multiple screenshots of the web site and stitch them together to make a bitmap of the web site.
When you want to take a screenshot of Google map, but the street names disappear if you unzoom to have all you want in one window, you can zoom in and take multiple screenshots and stitch them together to make a large bitmap of the map.



[Install]
Just expand files to wherever you want.  There is no installer.  You can just delete the files to uninstall.



[Starting the Program]
In Windows    Double click bin/2DRetroMapTool.exe
In macOS      Double click bin/2DRetroMapTool.app
In Linux      Start bin/2DRetroMapTool.app/Contents/Resources/2DRetroMapTool




[Initial Set Up]
What this program can do also can be done in any other picture-processing programs if you are patient.  The point of this program is that this program remembers files in the screenshot directory, and can import a new screenshot to your map with just one push of button.  The program makes a file list every time you start a program, so you need to make sure the program is up while you are playing a game.  (If you missed, you can still manually select a file in your screenshot directory.)

So, what's important is to tell the location of your screenshot directory to this program.  By default ~/Dropbox/Screenshots, ~/OneDrive/Pictures/Screenshots, ~/Desktop are selected as a screenshot directory.  If you are using Windows, make sure to set up Windows so that it stores a new screenshot in .PNG format in one of the two directories.

If a screenshot is created in a non-default location, you need to select [Config]->[Config] and specify the screenshot directory.  It is particularly important if you are playing an emulator-based retro games.  The window size of those games may be different from the original resolution, and the image may be stretched to show in a larger window.  In such a case, the emulator may have its own screenshot-taking function that gives the original-resolution image, which is much better for the purpose of making a 1x1 map.  You should specify the screenshot directory of the emulator.

iOS games might be a bit tricky.  I figured that if I synchronize screenshot via iCloud Photo Stream, the images are created in a sub-directory of:
    ~/Pictures/fromIPhone5S.photoslibrary/Masters
(What's going to happen if I have two iPhone5S on my account?)

To read a new screenshot from the Photo Stream with one push of button, you should add this directory in [Config]->[Config].  Also make sure to check [Sub-Folder].  Unless this box is checked the program won't look into sub-folders.





[Basic Operations]
Scrolling:  Left-Button drag while press and holding a Shift key.  Or, two-finger touch (if it comes with a touch screen)
Box Selection:  Left-button drag.
Single Selection:  Left-button click.
Add/Remove Selection:  Click on an item while the Control key is held down.
Deletion:    Delete key or [Edit]->[Delete]
Move:  Click on the selected item and drag.




[Control Dialog]
Top-Left:
    Field selection.
Arrows:
    Scroll screen.
Add Image:
    Add an image selected by a file dialog.
Highlight Diff:
    If this box is checked, difference between the selected images and the other image will be highlighted.
    If you have multiple selection, only first a few will be highlighted.
Threshold:
    RGB intensity threshold for highlighting diff.
Add New Screenshot: 
    Add a new screenshot taken in the pre-configured screenshot directory.
Add Comment:
    Add text comment.  You can make a multi-line comment by inserting \n .
Make Adjustment:
    After roughly place an image, this button will make small movement so that the diff (average per pixel) is minimized.




[Field and World]
A game usually has multiple stages.  In this program, a stage is a unit called a "Field".

A collection of the fields, or entire game, is called a gWorldh.  One data file (.retromap format file) stores one world.  Therefore, you don't have to close a file and open a new one again when you switch between fields.  (Down side is it eats more memory.)

On each field, you can paste screenshots and mark ups.  A screenshot is called "Map Piece" in this program.  There are two types of mark ups, (1) text and (2) drawing.  By using mark ups, you can add a memo like the item you can get at a particular location in your map.

If you have configured the system and this program right, you can insert a new screenshot just by clicking on "Insert New Screenshot" button.

Neighboring screenshots should overlap a little bit so that you can adjust the location.  To facilitate this, the program highlights differences on the overlapping part.




[About Screenshots]
If the game has its own screenshot function, it is better to use it rather than the operating system's screenshot functionality in general.

Probably the game uses some part of the window for showing status.  Those part are not needed for mapping.  So, those parts must be trimmed.  You can trim an image by [Edit]->[Trim].  You'll see four fingers around the image.  You can pick them and drag them to change the range for trimming.

You probably need to trim the same region for the subsequent screenshots.  So, you have an option to apply same trimming to the all new incoming images.  Or, you can apply to the one you selected only, or to all existing screenshots on the same field.

You can also specify the minimum unit that the screenshots can move.  Many retro games scrolls the map 8 pixels at a time or even 40 pixels at a time.  Then, you really don't want to align your bitmaps on those unit boundaries.  The images should snap to those unit boundaries.  To do so, you can select [World]->[Set Unit] to specify the unit.






[Making it 3D]
It is totally a useless feature of this program.  But, the GUI framework is originally made for editing 3D objects.  By un-checking the orientation lock on the view-control dialog, you can rotate the map by Shift + Rightbutton drag.
