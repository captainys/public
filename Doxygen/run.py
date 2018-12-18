#!/usr/bin/python

import os
import subprocess
import shutil

import yscmd
import yspackaging
import ysutil

def Run(doxfile):
	subprocess.Popen(["doxygen",doxfile]).wait()

if not os.path.isdir("../../yshelp"):
	os.mkdir("../../yshelp")

shutil.copyfile("index.html","../../yshelp/index.html")

path=os.path.realpath(__file__)
os.chdir(os.path.dirname(path))
Run("ysclass.doxygen")
Run("ysport.doxygen") # Depends on ysclass
Run("ysgebl.doxygen") # Depends on ysclass

Run("fssimplewindow.doxygen")
Run("fslazywindow.doxygen") # Dpeends on fssimplewindow

Run("ysgl.doxygen")



os.chdir("../..")

zipCmd=yscmd.FindFolderArchiver()
dstDir=yspackaging.ZipCopyDir()

zipFile="yshelp"+str(ysutil.Today())+".zip"
zipFile=os.path.join(dstDir,zipFile)

print("Zip File="+zipFile)

subprocess.Popen(zipCmd+[
	zipFile,
	"yshelp"
]).wait()

print("Zip Archive is written in "+zipFile)



