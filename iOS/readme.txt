Build environment for iOS.

HIGHLY EXPERIMENTAL.  DON'T EXPECT IT TO WORK.

I wish I could use cmake.





Self Memo.

- Create an Application project
From iOS->Application->Game
Then select Objective-C as Language and OpenGL ES as Game Technology


- Creating a project
Do not manually create a sub-directory.  XCode will do it for you.


- Creating an iOS library project
See https://developer.apple.com/library/ios/technotes/iOSStaticLibraries/Articles/creating.html for how to create an iOS library project.

Header file copy destination shouldn't include ${PRODUCT_NAME}

When selecting header files to copy, search for ".h".


- Adding a library project
Do NOT drag & drop.  Use right-click and select add file to something from the context menu, then select a .xcodeproj file.

Add project after closing it.  A project that is open cannot be added as a depending project (apparently).



- Exclude something.xcodeproj/xcuserdata  and something.xcodeproj/project.xcworkspace/xcuserdata from version control.



- BE CAREFUL!  XCODE WILL AUTOMATICALLY ADD NEW PROJECT TO SVN IF IT IS UNDER SVN'S CONTROL!



- Starting from OpenGL ES application template
    Make sure to delete all OpenGL calls in the default template first.
    It enables VBO at the beginning and keep it on, and the program crashes at your own glDrawArrays call because it looks at the enabled VBO, not your arrays.



- Can I actually take control of the run-loop?
  With swapbuffers taken by the run-loop, it is limited, but it seems that I can make a modal message dialog.
  http://stackoverflow.com/questions/6625872/uialertview-runmodal

