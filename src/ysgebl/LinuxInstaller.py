#!/usr/bin/python

import sys
import os
import shutil
import platform
import stat

THISPATH=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISPATH)

def PressEnter():
	sys.stdout.write("Press Enter>")
	try:
		xyz=input()
	except:
		print(".")




PROGNAME="Polygon Crest"
ICONTITLE="Polygon Crest"
ICONFILE="icon.png"

if os.path.isdir(os.path.join(THISDIR,"bin","Polygon Crest.app")):
	SRCSUBDIR="bin/Polygon Crest.app/Contents/Resources"
elif os.path.isdir(os.path.join(THISDIR,"bin")):
	SRCSUBDIR="bin"

DSTSUBDIR="bin"
EXE32="ysgebl32"
EXE64="ysgebl64"

SRCDIR=os.path.join(THISDIR,SRCSUBDIR)


print("***********************************************************")
print("This script installs "+PROGNAME+" in the Linux environment.")
print("It is not for other environments.")
print("***********************************************************")

PressEnter()


if not sys.platform.startswith('linux'):
	print("ERROR: This script is for Linux environment only.")
	PressEnter()
	quit()


if not os.path.isdir(SRCDIR):
	print("ERROR: Source directory not found.")
	print("       If you are directly running this from a zipped folder,")
	print("       Run this script after extracting all files somewhere.")
	PressEnter()
	quit()



DSTDIR=os.path.expanduser("~/YSFLIGHT.COM/PolygonCrest")
print("Destination Directory="+DSTDIR)

if not os.path.isdir(DSTDIR):
	try:
		os.makedirs(DSTDIR)
	except:
		print("ERROR: Cannot create the destination directory.")
		PressEnter()
		quit()



if os.path.isdir(os.path.join(DSTDIR,DSTSUBDIR)):
	try:
		print("Deleting existing installation...")
		shutil.rmtree(os.path.join(DSTDIR,DSTSUBDIR))
	except:
		print("ERROR: Cannot delete the existing installation.")
		PressEnter()
		quit()



try:
	print("Copying files...")
	shutil.copytree(os.path.join(THISDIR,SRCSUBDIR),os.path.join(DSTDIR,DSTSUBDIR))
except:
	print("ERROR: Cannot copy files.")
	PressEnter()
	quit()



try:
	print("Making icons...")

	if platform.architecture()[0].startswith("32"):
		exefile=EXE32
	elif platform.architecture()[0].startswith("64"):
		exefile=EXE64
	else:
		print("ERROR: Cannot identify the bitness.")
		PressEnter()
		quit()

	# For some reason icon.png in the same directory as executable doesn't appear.
	if os.path.isfile(os.path.join(THISDIR,ICONFILE)):
		iconpngsrc=os.path.join(THISDIR,ICONFILE)
	elif os.path.isfile(os.path.join(THISDIR,"bin",ICONFILE)):
		iconpngsrc=os.path.join(THISDIR,"bin",ICONFILE)
	elif os.path.isfile(os.path.join(THISDIR,SRCSUBDIR,ICONFILE)):
		iconpngsrc=os.path.join(THISDIR,SRCSUBDIR,ICONFILE)
	try:
		shutil.copyfile(iconpngsrc,os.path.join(DSTDIR,ICONFILE))
	except:
		print("WARNING: Could not copy the icon PNG.")

	try:
		desktop=os.path.expanduser("~/Desktop")
		iconfile=os.path.join(desktop,"PolygonCrest.desktop")
		fp=open(iconfile,"w")
	except:
		print("ERROR: Cannot create a desktop icon.")
		PressEnter()
		quit()

	fp.write("[Desktop Entry]\n")
	fp.write("Name="+ICONTITLE+"\n")
	fp.write("Exec="+os.path.join(DSTDIR,DSTSUBDIR,exefile)+"\n")
	fp.write("Type=Application\n")
	fp.write("StartupNotify=true\n")
	fp.write("Comment=YSFLIGHT.COM\n")
	fp.write("Path="+os.path.join(DSTDIR,DSTSUBDIR)+"\n")
	fp.write("Icon="+os.path.join(DSTDIR,ICONFILE)+"\n")

	fp.close()

	os.chmod(iconfile,stat.S_IREAD|stat.S_IEXEC|stat.S_IWRITE)
except:
	print("ERROR: Error while creating an icon.")
	PressEnter()
	quit()



print("Installation completed!")
print("Please double click "+ICONTITLE+" on the desktop to start the program!")
PressEnter()

