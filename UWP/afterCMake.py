# As of CMake 3.6.2, NuGet packages are not supported.
# Only way is to manipulate .vcxproj after running CMake.
# Run this script from the build directory after running CMake.


import os
import sys
import subprocess


def CorrectVCXProj(vcxprojFn,buildRootDir):
	fp=open(vcxprojFn,"r",encoding="utf-8")
	fileContent=[]
	changed=False
	needAngle=False
	hasAngle=False
	needWin2D=False
	hasWin2D=False
	for s in fp:
		s=s.replace("\n","")
		if "<PROJECTPRIFULLPATH>$(TARGETDIR)RESOURCES.PRI</PROJECTPRIFULLPATH>" in s.upper():
			s="<ProjectPriFullPath>$(ProjectDir)$(IntDir)resources.pri</ProjectPriFullPath>"
			changed=True
		if "NUGET" in s.upper() and "NEEDANGLE" in s.upper():
			needAngle=True
		if "NUGET" in s.upper() and "NEEDWIN2D" in s.upper():
			needWin2D=True
		if "ANGLE.WINDOWSSTORE" in s.upper():
			hasAngle=True
		if "WIN2D.UWP" in s.upper():
			hasWin2D=True
		fileContent.append(s)
	fp.close()

	mustAddAngle=False
	mustAddWin2D=False
	if True==needAngle and True!=hasAngle:
		mustAddAngle=True
	if True==needWin2D and True!=hasWin2D:
		mustAddWin2D=True

	if True==mustAddAngle or True==mustAddWin2D:
		lastProject=-1
		for i in range(0,len(fileContent)):
			if "</PROJECT>" in fileContent[i].upper():
				lastProject=i
		if 0<=lastProject:
			fileContent[lastProject]=""
			if True==mustAddAngle:
				targetFn=os.path.join(
					buildRootDir,
					"nuget_packages",
					"ANGLE.WindowsStore",
					"build",
					"native",
					"ANGLE.WindowsStore.targets")
				fileContent.append('<Import Project="'+targetFn+'" />')
				changed=True
			if True==mustAddWin2D:
				targetFn=os.path.join(
					buildRootDir,
					"nuget_packages",
					"Win2D.uwp",
					"build",
					"native",
					"Win2D.uwp.targets")
				fileContent.append('<Import Project="'+targetFn+'" />')
				changed=True
			fileContent.append("</Project>")

	if(True==changed):
		fp=open(vcxprojFn,"w",encoding="utf-8")
		for s in fileContent:
			fp.write(s+"\n")
		fp.close()
		print("Corrected")


def CorrectCMake3_6_3Bug(buildDir,buildRootDir):
	files=os.listdir(buildDir)
	for f in files:
		ful=os.path.join(buildDir,f)
		if os.path.isfile(ful):
			(main,ext)=os.path.splitext(f)
			if ext.upper()==".VCXPROJ" and main.upper()!="ZERO_CHECK" and main.upper()!="ALL_BUILD":
				print("    Found: "+ful)
				CorrectVCXProj(ful,buildRootDir)
		elif os.path.isdir(ful):
			if f.startswith(".") or f.upper()=="CMAKEFILES":
				print("    Skipping "+ful)
			else:
				CorrectCMake3_6_3Bug(os.path.join(buildDir,f),buildRootDir)



CorrectCMake3_6_3Bug(".",os.path.realpath("."))
