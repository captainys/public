.SUFFIXES: .c .cpp .obj
PROJECTNAME=ysport
SRC_ROOT=../../..
IS_LIBRARY_PROJECT=1
OWN_HEADER_PATH=..
DEPEND_ON=$(SRC_ROOT)/ysclass/src/makefile.unx



# {
# YS Standard Makefile Header for Unix and MacOSX 20140801 >>
#
# Variable PROJECTNAME and SRC_ROOT must be defined before the header.
#

# Self-memo: $* $> $@ etc are officially (?) called 'automatic variables'
# Self-memo: $(lastword $(MAKEFILE_LIST)) will give the current makefile name


# Identifying compiler >>
# Priority is given to clang over g++
TESTCLANG:=$(shell which clang 1> /dev/null; echo $$?)
TESTGPP:=$(shell which g++ 1> /dev/null; echo $$?)

ifeq ($(TESTCLANG),0)
CC=clang
else ifeq ($(TESTGPP),0)
CC=g++
else
$(error C++ Compiler not found.)
endif
# Identifying compiler >>


RM=rm
MD=mkdir
CP=cp
INCOPT=-I
LIBPREFIX=lib
LIBEXT=.a
CPPFLAGS=-std=c++0x
MAKEEXE=make
C_OPTION_AVAILABLE=1
SLASH=/
CHAIN=;

BITNESSTESTSRC=ysbitnesstest.cpp
BITNESSTESTEXE=ysbitnesstest

ifndef PROJECTNAME
$(error Variable PROJECTNAME not defined.)
endif
ifndef SRC_ROOT
$(error Variable SRC_ROOT not defined.)
endif
ifndef IS_LIBRARY_PROJECT
$(error Variable IS_LIBRARY_PROJECT not defined.)
endif


ifndef YS_BITNESS
GENSRC:=$(shell echo 'int main(void){printf("%d",8*(int)sizeof(void *));return 0;} ' 1> $(BITNESSTESTSRC))
BITNESSCOMPILE:=$(shell g++ $(BITNESSTESTSRC) -o $(BITNESSTESTEXE) -include stdio.h)
SIZEOFPOINTER:=$(shell ./$(BITNESSTESTEXE))
YS_BITNESS:=$(SIZEOFPOINTER)
CLEANTEMPORARYCPP:=$(shell rm $(BITNESSTESTSRC) $(BITNESSTESTEXE))
endif

ifeq ($(YS_BITNESS),32)
  CBITNESSFLAG=-m32
else
  ifeq ($(YS_BITNESS),64)
    CBITNESSFLAG=-m64
  endif
endif

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
PLATFORM=linux
PLATFORMSHORT=unx
PLATFORMLIB=$(X11LIB) -lGLU -lGL
APPDIR=.
APPDIRSIGNATURE=
APPDIRSIGNATURERULE=dummyRule
OBJSIMPLEWIN=$(OBJDIR)/fsglxwrapper.o $(OBJDIR)/fsglxkeymap.o
endif

ifeq ($(UNAME), Darwin)
PLATFORM=macosx
PLATFORMSHORT=mac
PLATFORMLIB=-framework Cocoa -framework OpenGL
APPDIR=$(PROJECTNAME).app/Contents/MacOS
APPDIRSIGNATURE=$(APPDIR)/signature
APPDIRSIGNATURERULE=$(APPDIRSIGNATURE)
OBJSIMPLEWIN=$(OBJDIR)/fsmacosxwrapperobjc.o $(OBJDIR)/fsmacosxwrappercpp.o
ifeq ($(IS_LIBRARY_PROJECT),0)
MAKEBUNDLEDIR:=$(shell mkdir -p $(APPDIR))
endif
endif

ifndef YSBIN_ROOT
YSBIN_ROOT=$(HOME)/ysbin
endif

YSOBJ_ROOT=$(YSBIN_ROOT)/$(PLATFORM)$(YS_BITNESS)/obj
USERLIBDIR=$(YSBIN_ROOT)/$(PLATFORM)$(YS_BITNESS)/lib

OBJDIR=$(YSOBJ_ROOT)/$(PROJECTNAME)
LIBDIR=$(USERLIBDIR)
BINDIR=./bin/$(PLATFORM)$(YS_BITNESS)
MAKEFILE=$(lastword $(MAKEFILE_LIST))
#
# YS Standard Makefile Header for Unix and MacOSX <<
# }



TARGET=$(LIBDIR)/lib$(PROJECTNAME).a
INCLUDE=.



ADDITIONAL_INCLUDE_PATH=-I $(SRC_ROOT)/ysclass/src/ 

ADDITIONAL_LIBRARIES=$(LIBDIR)/libysclass.a 

CC = g++
CPP= g++
CFLAGS = -c -O3 $(CBITNESSFLAG) -fopenmp $(ADDITIONAL_INCLUDE_PATH)
#-m32 to force 32-bit executable

# .IGNORE:

OBJS=$(OBJDIR)/ysfileio.o $(OBJDIR)/ysfileinfo.o $(OBJDIR)/yslocale.o $(OBJDIR)/ysspecialpath.o $(OBJDIR)/yssubsectimer.o $(OBJDIR)/ysthread.o $(OBJDIR)/ysunixfileinfo.o $(OBJDIR)/ysunixfileio.o $(OBJDIR)/ysunixlocale.o $(OBJDIR)/ysunixspecialpath.o $(OBJDIR)/ysunixsubsectimer.o $(OBJDIR)/ysunixrunprocess.o $(OBJDIR)/ysunixencoding.o $(OBJDIR)/ysopenurl.o




all : dependencies $(TARGET)

targetonly : $(TARGET)

dependencies :
	$(MAKEEXE) -C $(SRC_ROOT)/ysclass/src/ -f makefile.unx targetonly

clean_dependencies :
	$(MAKEEXE) -C $(SRC_ROOT)/ysclass/src/ -f makefile.unx cln

clean_self :
	rm -f $(OBJS)
	rm -f $(TARGET)

$(TARGET) : $(LIBDIR)/signature $(OBJDIR)/signature $(OBJS)
	ar cru $(TARGET) $(OBJS)
	ranlib $(TARGET)

rebuild : cln $(TARGET)

cln : clean_dependencies clean_self

$(LIBDIR)/signature :
	mkdir -p $(LIBDIR)
	echo signature > $@

$(OBJDIR)/signature :
	mkdir -p $(OBJDIR)
	echo signature > $@

#.c.o:
#	$(CC) $(CFLAGS) $*.c
#
#.cpp.o:
#	$(CPP) $(CFLAGS) $*.cpp
#

$(OBJDIR)/ysfileio.o : ../ysfileio.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/ysfileinfo.o : ../ysfileinfo.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/yslocale.o : ../yslocale.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/ysspecialpath.o : ../ysspecialpath.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/yssubsectimer.o : ../yssubsectimer.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/ysthread.o : ../ysthread.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/ysunixfileinfo.o : ysunixfileinfo.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/ysunixfileio.o : ysunixfileio.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/ysunixlocale.o : ysunixlocale.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/ysunixspecialpath.o : ysunixspecialpath.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/ysunixsubsectimer.o : ysunixsubsectimer.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/ysunixthread.o : ysunixthread.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/ysunixrunprocess.o : ysunixrunprocess.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/ysunixencoding.o : ysunixencoding.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

$(OBJDIR)/ysopenurl.o : ysopenurl.cpp
	$(CC) $? $(CPPFLAGS) $(CFLAGS) -o $@

