import subprocess
import zipfile

subprocess.Popen([
	"cmake",
	"../src",
	'-DCMAKE_CONFIGURATION_TYPES=Release',
	'-G',
	'Visual Studio 14',
	"-T",
	"v140_xp",
	]).wait()


subprocess.Popen([
	"msbuild",
	"Project.sln",
	"-p:Configuration=Release",
	"-p:Platform=win32",
	]).wait()


zip=zipfile.ZipFile("FM77AVKeyboard_XP32.zip","w")
zip.write("FM77AVKeyboard/Release/FM77AVKeyboard_gl1.exe")
zip.close()


print("Windows XP binary requires:")
print("https://www.microsoft.com/en-US/download/details.aspx?id=48145")


