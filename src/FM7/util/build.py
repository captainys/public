# This Python script assumes that asm6809.exe, CMake.exe, MSBuild.exe are in the PATH-pointing directory.

import os
import subprocess
import shutil

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)

import sys
sys.path.insert(0,os.path.join(THISDIR,"python"))
import fbasic
import bin2cpp
import txt2cpp



makeD77=True
makeWAV=False



asmSource=[
	["D7CLIENT",
		["D77ToRS232C/d77client.asm",
		 "fm7code/dskwrite.asm",
		 "fm7code/formatm.asm",
		 "fm7code/miscfunc.asm",
		 "fm7code/miscdata.asm",
		 "rs232cTfr/rs232ctfr.asm",
		 "6809lib/rs232c_reset.asm",
		 "6809lib/rs232c_recv.asm",
		 "6809lib/rs232c_send.asm"]],
	["CAS0COM0",
		["T77ToRS232C/bioshook_install.asm",
		 "T77ToRS232C/bioshook_small.asm"]],
	["CAS0COMF",
		["T77ToRS232C/bioshook_install.asm",
		 "T77ToRS232C/bioshook_buffered.asm"]],
	["232CFILM",
		["rs232cTfr/org.asm",
		 "rs232cTfr/rs232ctfr.asm",
		 "6809lib/rs232c_reset.asm",
		 "6809lib/rs232c_recv.asm",
		 "rs232cTfr/end.asm"]],
	["MEM2COMM",
		["memToRS232C/org.asm",
		 "6809lib/rs232c_reset.asm",
		 "6809lib/rs232c_send.asm",
		 "memToRS232C/end.asm"]],
	["RAWREADM",
		["rawReadToRS232C/org.asm",
		 "rawReadToRS232C/readdisk.asm",
		 "6809lib/rs232c_reset.asm",
		 "6809lib/rs232c_send.asm"]],
	["COMTESTM",
		["rs232cTest/org.asm",
		 "6809lib/rs232c_reset.asm",
		 "6809lib/rs232c_recv.asm",
		 "6809lib/rs232c_send.asm"]],
	["TAPE2COM",
		["tapeToRS232C/tapedump.asm",
		 "6809lib/rs232c_reset.asm",
		 "6809lib/rs232c_send.asm",
		 "tapeToRS232C/tapedump_end.asm",]],
]

basSource=[
	["232CFILE","rs232cTfr/rs232ctfr.fbas"],
	["MEM2COM","memToRS232C/mem2com.fbas"],
	["RAWREAD","rawReadToRS232C/rawread.fbas"],
	["COMTEST","rs232cTest/comtest.fbas"],
]

wavSource=[
	["232CFILE","232CFILE","232CFILM"],
	["MEM2COM","MEM2COM","MEM2COMM"],
	["RAWREAD","RAWREAD","RAWREADM"],
	["COMTEST","COMTEST","COMTESTM"],
]


def BuildForFM7(outDir):
	if not os.path.isdir(outDir):
		os.makedirs(outDir)

	d77Fn=os.path.join(outDir,"FM7_rs232c_util.D77");
	shutil.copyfile("empty.D77",d77Fn)

	for asm in asmSource:
		symFn=asm[0]+".sym"
		srecFn=asm[0]+".srec"

		allcmd=["asm6809.exe","-v"]+asm[1]+["-S","-o",os.path.join(outDir,srecFn),"-s",os.path.join(outDir,symFn)]
		print(allcmd)
		proc=subprocess.Popen(allcmd)
		proc.communicate()

		if 0!=proc.returncode:
			raise

		fn232c=os.path.splitext(srecFn)[0]+".232c"
		subprocess.Popen([
			"build64/exe/rs232ctfr.exe",
			os.path.join(outDir,fn232c),
			"-srecwrite",
			os.path.join(outDir,srecFn),
		]).wait()

		dumpFn=os.path.splitext(srecFn)[0]+".dump"
		subprocess.Popen([
			"build64/exe/srecdump.exe",
			os.path.join(outDir,srecFn),
			os.path.join(outDir,dumpFn),
		]).wait()

		typerFn=os.path.splitext(srecFn)[0]+".type"
		subprocess.Popen([
			"build64/exe/77avtyper.exe",
			os.path.join(outDir,srecFn),
			"-o",
			os.path.join(outDir,typerFn),
		]).wait()

		if True==makeD77:
			subprocess.Popen([
				"build64/exe/d77fileutil.exe",
				d77Fn,
				"-srecwrite",
				os.path.join(outDir,srecFn),
				asm[0]
			]).wait()


	fbasic.symPath=[outDir]
	for bas in basSource:
		basFn=os.path.join(outDir,bas[0]+".bas")
		fbasic.PreProc(bas[1],basFn)

		fn232c=bas[0]+".232c"
		subprocess.Popen([
			"build64/exe/rs232ctfr.exe",
			os.path.join(outDir,fn232c),
			"-save",
			basFn,
			bas[0],
		]).wait()

		if True==makeD77:
			subprocess.Popen([
				"build64/exe/d77fileutil.exe",
				d77Fn,
				"-save",
				os.path.join(outDir,bas[0]+".bas"),
				bas[0]
			]).wait()

	if True==makeD77:
		subprocess.Popen([
			"build64/exe/d77fileutil.exe",
			d77Fn,
			"-save",
			"D77ToRS232C/chicken_egg.bas",
			"CHICKEN"
		]).wait()

	if True==makeWAV:
		for wav in wavSource:
			wavFn=os.path.join(outDir,wav[0]+".t77")
			basFn=os.path.join(outDir,wav[1]+".bas")
			srecFn=os.path.join(outDir,wav[2]+".srec")
			print(wavFn,basFn,srecFn)
			subprocess.Popen([
				"build64/exe/t77save.exe",
				wavFn,
				"-new",
				"-wav",
				"-save",
				basFn,
				wav[1],
				"-srecwrite",
				srecFn,
				wav[2]
			]).wait()



def RunCMakeAndDeleteExe():
	if not os.path.isdir("build32"):
		os.makedirs("build32")
	if not os.path.isdir("build64"):
		os.makedirs("build64")

	os.chdir("build32")

	if os.path.isdir("exe"):
		for fn in os.listdir("exe"):
			ext=os.path.splitext(fn)[1].upper()
			if ext==".EXE":
				os.remove(os.path.join("exe",fn))

	proc=subprocess.Popen(["CMake","..","-T","v140_xp","-DCMAKE_CONFIGURATION_TYPES=Release","-DCMAKE_BUILD_TYPE=Release"])
	proc.communicate()
	if 0!=proc.returncode:
		print("CMake returned error.");
		raise()

	os.chdir("..")

	os.chdir("build64")

	if os.path.isdir("exe"):
		for fn in os.listdir("exe"):
			ext=os.path.splitext(fn)[1].upper()
			if ext==".EXE":
				os.remove(os.path.join("exe",fn))

	proc=subprocess.Popen(["CMake","..","-DCMAKE_CONFIGURATION_TYPES=Release","-DCMAKE_BUILD_TYPE=Release","-A","x64"])
	proc.communicate()
	if 0!=proc.returncode:
		print("CMake returned error.");
		raise()

	os.chdir("..")



def BuildForWin():
	os.chdir("build32")

	build32proc=subprocess.Popen(["msbuild","Project.sln","/p:Configuration=Release","/p:Platform=Win32","/m"])
	build32proc.communicate()
	if 0!=build32proc.returncode:
		print("MSBuild returned an error.")
		raise

	os.chdir("..")


	os.chdir("build64")

	build64proc=subprocess.Popen(["msbuild","Project.sln","/m"])
	build64proc.communicate()
	if 0!=build64proc.returncode:
		print("MSBuild returned an error.")
		raise

	os.chdir("..")



def UpdateWinSource():
	if not os.path.isfile("buildFM7/FM7_rs232c_util.d77"):
		print("FM7_rs232c_util.d77 does not exist.")
		print("This file must be created during FM-7 build.")
		quit(1)
	if not os.path.isfile("buildFM7/D7CLIENT.srec"):
		print("D7CLIENT.srec does not exist.")
		print("This file must be created during FM-7 build.")
		quit(1)
	if not os.path.isfile("buildFM7/CAS0COM0.srec"):
		print("CAS0COM0.srec does not exist.")
		print("This file must be created during FM-7 build.")
		quit(1)
	if not os.path.isfile("buildFM7/CAS0COMF.srec"):
		print("CAS0COMF.srec does not exist.")
		print("This file must be created during FM-7 build.")
		quit(1)

	bin2cpp.BinaryFileToCpp("D77ToRS232C/diskimg.cpp","D77ToRS232C/diskimg.h","utilDiskImg","buildFM7/FM7_rs232c_util.d77")
	txt2cpp.TextFileToCpp("D77ToRS232C/clientbin.cpp","D77ToRS232C/clientbin.h","clientBinary","buildFM7/D7CLIENT.srec")
	txt2cpp.TextFileToCpp("T77ToRS232C/bioshook_small.cpp","T77ToRS232C/bioshook_small.h","clientBinary_small","buildFM7/CAS0COM0.srec")
	txt2cpp.TextFileToCpp("T77ToRS232C/bioshook_buffered.cpp","T77ToRS232C/bioshook_buffered.h","clientBinary_buffered","buildFM7/CAS0COMF.srec")



if __name__=="__main__":
	RunCMakeAndDeleteExe()
	BuildForWin()				# Requires some files from FM7 build.
	BuildForFM7("buildFM7")		# Requires some executables.
	UpdateWinSource()
	BuildForWin()				# Not super clean, but build again after updating sources.
