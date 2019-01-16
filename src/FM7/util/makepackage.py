import subprocess
import shutil
import os
import build


# This Python script assumes that asm6809.exe, CMake.exe, MSBuild.exe are in the PATH-pointing directory.


fm7Code=True
winCode=True
makeZip=True
clearBuild=True

build.makeD77=True
build.makeWAV=True



THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)

import sys
sys.path.insert(0,os.path.join(THISDIR,"python"))
import fbasic



def PackagingDir():
	return os.path.expanduser(os.path.join("~","Packaging","FM7_RESURRECTION_UTIL"))



def FM7CodeDir():
	return os.path.join(PackagingDir(),"FM-7");



def MakePackagingDir():
	os.makedirs(PackagingDir())
	os.makedirs(os.path.join(PackagingDir(),"exe32"))
	os.makedirs(os.path.join(PackagingDir(),"exe64"))
	os.makedirs(FM7CodeDir())



def ClearDir(packdir):
	if os.path.isdir(packdir):
		while True:
			try:
				shutil.rmtree(packdir)
				break
			except:
				print("ATTENTION!")
				print("Could not delete the packaging directory.")
				print("A file in the packaginging directory may be open.")
				print("Close the file, and press enter.")
				Input("PRESS ENTER TO CONTINUE>")


	if os.path.isdir(packdir):
		print("Could not clean the packaging directory.")
		print("Close those programs and source files and try again.")
		quit()



def CopySource():
	ClearDir(os.path.join(PackagingDir(),"src"))
	proc=subprocess.Popen([
		"svn",
		"export",
		"https://ysflightsvn/svn/main/trunk/FM7/util",
		os.path.join(PackagingDir(),"src")])
	proc.communicate()
	if 0!=proc.returncode:
		print("SubVersion returned error.")
		raise



def main():
	ClearDir(PackagingDir())
	MakePackagingDir()
	# CopySource() Sources are now from github
	if True==winCode:
		if True==clearBuild:
			ClearDir("build32")
			ClearDir("build64")

		build.RunCMakeAndDeleteExe()
		build.BuildForWin()

		for fn in os.listdir("build32/exe"):
			ext=os.path.splitext(fn)[1].upper()
			print(fn,ext)
			if ext==".EXE":
				shutil.copyfile(os.path.join("build32","exe",fn),os.path.join(PackagingDir(),"exe32",fn))

		for fn in os.listdir("build64/exe"):
			ext=os.path.splitext(fn)[1].upper()
			print(fn,ext)
			if ext==".EXE":
				shutil.copyfile(os.path.join("build64","exe",fn),os.path.join(PackagingDir(),"exe64",fn))

		# Source files are open in github
		# shutil.copytree(os.path.join("..","DiskFormat"),os.path.join(PackagingDir(),"src","fm7code","DiskFormat"))


	if True==fm7Code:
		build.BuildForFM7(FM7CodeDir())
		if True==winCode:
			build.UpdateWinSource()
			build.BuildForWin()


	shutil.copyfile(
		"readme.txt",
		os.path.expanduser(os.path.join("~","Packaging","FM7_RESURRECTION_UTIL","readme.txt")))
	shutil.copyfile(
		"readme_e.txt",
		os.path.expanduser(os.path.join("~","Packaging","FM7_RESURRECTION_UTIL","readme_e.txt")))
	shutil.copyfile(
		os.path.join("D77ToRS232C","readme.txt"),
		os.path.expanduser(os.path.join("~","Packaging","FM7_RESURRECTION_UTIL","readme_D77ToRS232C.txt")))
	shutil.copyfile(
		os.path.join("D77ToRS232C","readme_e.txt"),
		os.path.expanduser(os.path.join("~","Packaging","FM7_RESURRECTION_UTIL","readme_D77ToRS232C_e.txt")))
	shutil.copyfile(
		os.path.join("rawReadToRS232C","readme.txt"),
		os.path.expanduser(os.path.join("~","Packaging","FM7_RESURRECTION_UTIL","readme_rawReadToRS232C.txt")))


	if True==makeZip:
		zipFn=os.path.expanduser(os.path.join("~","Packaging","FM7_RESURRECTION_UTIL"))
		zipRoot=os.path.expanduser(os.path.join("~","Packaging"))
		zipBase="FM7_RESURRECTION_UTIL"
		shutil.make_archive(zipFn,'zip',zipRoot,zipBase)





if __name__=="__main__":
	i=0
	while i<len(sys.argv):
		if sys.argv[i]=="-fm7only":
			fm7Code=True
			winCode=False
			makeZip=False
		if sys.argv[i]=="-dontclear":
			clearBuild=False
			makeZip=False
		i=i+1

	main()
