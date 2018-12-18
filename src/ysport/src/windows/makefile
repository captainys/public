PROJECTNAME=ysport
SRC_ROOT=..\..\..
OWN_HEADER_PATH=..
DEPEND_ON=$(SRC_ROOT)\ysclass\src\makefile.win
IS_LIBRARY_PROJECT=1



# {
# BEGIN YS Standard NMAKE MAKEFILE HEADER 20140801 >>
#
# Variable PROJECTNAME must be defined before the header.
# Variable IS_LIBRARY_PROJECT must be set 0 or 1
#

!IFNDEF IS_LIBRARY_PROJECT
!ERROR IS_LIBRARY_PROJECT not defined.
!ENDIF

!IFNDEF PROJECTNAME
!ERROR PROJECTNAME not defined.
!ENDIF

!IFNDEF SRC_ROOT
!ERROR SRC_ROOT not defined.
!ENDIF

!IFNDEF LINKSUBSYS
LINKSUBSYS=CONSOLE
!ENDIF

CC=cl
INCOPT=/I
LIBPREFIX=
LIBEXT=.lib
RM=del
MD=mkdir
CP=copy
LIBEXE=lib.exe
MAKEEXE=nmake -nologo
C_OPTION_AVAILABLE=0
SLASH=\
CHAIN=&

!IFNDEF BITNESSARCH

BITNESSUTIL=ys_temp_util_sizeofpointer
BITNESSUTILCPP=$(BITNESSUTIL).cpp
BITNESSUTILEXE=$(BITNESSUTIL).exe

!IF [@echo int main(void){return sizeof(void *);} > $(BITNESSUTILCPP)]==0
!MESSAGE Created $(BITNESSUTILCPP)
!ELSE
!ERROR Cannot create $(BITNESSUTILCPP)
!ENDIF

!IF [@cl.exe /nologo $(BITNESSUTILCPP)]==0
!MESSAGE Compiled $(BITNESSUTILCPP)
!ELSE
!ERROR Cannot compile bitness identifier!
!ENDIF

!IF [$(BITNESSUTILEXE)]==4
!MESSAGE Detected 32-bit compiler
BITNESS=32
BITNESSARCH=win32
!ELSEIF [$(BITNESSUTILEXE)]==8
!MESSAGE Detected 64-bit compiler
BITNESS=64
BITNESSARCH=win64
!ELSE
!ERROR Cannot identify bitness
!ENDIF

!IF [del $(BITNESSUTIL).*]==0
!ENDIF

!ENDIF
#Closing IFNDEF BITNESSARCH



!IF "$(BITNESSARCH)"=="win32"
SYSTEMID=5.01
LINKMACHINE=/MACHINE:X86
!ELSEIF "$(BITNESSARCH)"=="win64"
SYSTEMID=5.02
LINKMACHINE=/MACHINE:x64
!ELSE
!ERROR Unknown bitness-architecture.
!ENDIF



!IFDEF USERLIBDIR
#!MESSAGE User Library Directory=$(USERLIBDIR)
!ELSE
!MESSAGE Environment variable USERLIBDIR is not defined.  Default to C:\lib
USERLIBDIR=C:\lib
!ENDIF

!IFDEF USERINCDIR
#!MESSAGE User Include Directory=$(USERINCDIR)
!ELSE
!MESSAGE Environment variable USERINCDIR is not defined.  Default to C:\include
USERINCDIR=C:\include
!ENDIF



!IFDEF USEROBJDIR
#!MESSAGE User Object Directory=$(USEROBJDIR)
!ELSE
!MESSAGE Environment variable USEROBJDIR is not defined.  Default to C:\tmp\obj
USEROBJDIR=C:\tmp\obj
!ENDIF



OBJDIR=$(USEROBJDIR)\$(BITNESSARCH)\$(PROJECTNAME)
LIBDIR=$(USERLIBDIR)\$(BITNESSARCH)
BINDIR=.\bin\$(BITNESSARCH)
PDBFILE=$(OBJDIR)\$(PROJECTNAME).pdb
PCHFILE=$(OBJDIR)\$(PROJECTNAME).pch
#
# END YS Standard NMAKE MAKEFILE HEADER <<
# }



TARGET="$(LIBDIR)\$(PROJECTNAME).lib"



OBJS=$(OBJDIR)\ysfileio.obj $(OBJDIR)\ysfileinfo.obj $(OBJDIR)\yslocale.obj $(OBJDIR)\ysspecialpath.obj $(OBJDIR)\yssubsectimer.obj $(OBJDIR)\ysthread.obj $(OBJDIR)\yswin32fileinfo.obj $(OBJDIR)\yswin32fileio.obj $(OBJDIR)\yswin32locale.obj $(OBJDIR)\yswin32specialpath.obj $(OBJDIR)\yswin32subsectimer.obj $(OBJDIR)\yswin32runprocess.obj $(OBJDIR)\yswin32encoding.obj $(OBJDIR)\ysopenurl.obj



# OBJS automatically split into four groups by cleandep.exe
U_OBJS0=$(OBJDIR)\ysfileio.obj $(OBJDIR)\ysfileinfo.obj $(OBJDIR)\yslocale.obj 

U_OBJS1=$(OBJDIR)\ysspecialpath.obj $(OBJDIR)\yssubsectimer.obj $(OBJDIR)\ysthread.obj $(OBJDIR)\yswin32fileinfo.obj 

U_OBJS2=$(OBJDIR)\yswin32fileio.obj $(OBJDIR)\yswin32locale.obj $(OBJDIR)\yswin32specialpath.obj 

U_OBJS3=$(OBJDIR)\yswin32subsectimer.obj $(OBJDIR)\yswin32runprocess.obj $(OBJDIR)\yswin32encoding.obj $(OBJDIR)\ysopenurl.obj 



ADDITIONAL_INCLUDE_PATH=/I $(SRC_ROOT)\ysclass\src\. 

ADDITIONAL_LIBRARIES=$(LIBDIR)\ysclass.lib 

CFLAGS=/W3 /Zi /DWIN32 /c /O2 /nologo /wd4996 /I .. /EHsc $(ADDITIONAL_INCLUDE_PATH)



all : dependencies
	nmake -nologo $(TARGET)

targetonly : $(TARGET)

$(TARGET) : $(OBJDIR) $(LIBDIR) $(OBJS)
	$(LIBEXE) @<<
	$(LIBFLAGS) /OUT:"$(TARGET)" $(OBJS)
<<

dependencies :
	cd $(SRC_ROOT)\ysclass\src\ & $(MAKEEXE) -f makefile.win targetonly

clean_dependencies :
	cd $(SRC_ROOT)\ysclass\src\ & $(MAKEEXE) -f makefile.win clean_self

clean_self :
	$(RM) $(OBJS)
	$(RM) $(TARGET)
	$(RM) *.pdb

objset0 : $(OBJDIR) $(U_OBJS0)

objset1 : $(OBJDIR) $(U_OBJS1)

objset2 : $(OBJDIR) $(U_OBJS2)

objset3 : $(OBJDIR) $(U_OBJS3)


$(OBJDIR) :
	md $(OBJDIR)

$(LIBDIR) :
	md $(LIBDIR)


inst :
	$(CP) ..\*.h $(INCLUDEDIR)
	$(CP) $(TARGET) $(LIBDIR)

cln : clean_dependencies clean_self


rebuild: cln $(TARGET)


$(OBJDIR)\ysfileio.obj : ..\ysfileio.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\ysfileinfo.obj : ..\ysfileinfo.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\yslocale.obj : ..\yslocale.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\ysspecialpath.obj : ..\ysspecialpath.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\yssubsectimer.obj : ..\yssubsectimer.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\ysthread.obj : ..\ysthread.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\yswin32fileinfo.obj : yswin32fileinfo.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\yswin32fileio.obj : yswin32fileio.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\yswin32locale.obj : yswin32locale.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\yswin32specialpath.obj : yswin32specialpath.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\yswin32subsectimer.obj : yswin32subsectimer.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\ysthread11.obj : ..\ysthread11.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\yswin32runprocess.obj : yswin32runprocess.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\yswin32encoding.obj : yswin32encoding.cpp
	$(CC) $? $(CFLAGS) /Fo$@

$(OBJDIR)\ysopenurl.obj : ysopenurl.cpp
	$(CC) $? $(CFLAGS) /Fo$@

