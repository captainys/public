import subprocess
import os
import shutil
import sys


# Run this command where something.xcodeproj is located as:
#
#     % python ignoreuserdata.py something.xcodeproj
#

def CleanXcodeproj():
	if(os.path.isdir(os.path.join("project.xcworkspace","xcuserdata"))):
		shutil.rmtree(os.path.join("project.xcworkspace","xcuserdata"))

	if(os.path.isdir(os.path.join("project.xcworkspace","xcshareddata"))):
		shutil.rmtree(os.path.join("project.xcworkspace","xcshareddata"))

	if(os.path.isdir("xcuserdata")):
		shutil.rmtree("xcuserdata")



	subprocess.Popen([
		"svn",
		"delete",
		"project.xcworkspace"
	]).wait()


	subprocess.Popen([
		"svn",
		"propset",
		"svn:ignore",
		"xcuserdata\nxcshareddata\nproject.xcworkspace",
		"."
	]).wait()

	subprocess.Popen([
		"svn",
		"propget",
		"svn:ignore",
		"."
	]).wait()





#main
for dir in sys.argv:
	if os.path.isdir(dir):
		print("#################")
		print("Cleaning:"+dir)
		cwd=os.getcwd()
		os.chdir(dir)
		print("PUSHD="+os.getcwd())
		CleanXcodeproj()
		os.chdir(cwd)
		print("POPD="+os.getcwd())

