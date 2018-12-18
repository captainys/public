import sys
import os
import subprocess


source=[
	'plain2d',
	'texture2d',
	'experimentQuad2d',
	'plain3d',
	'shaded3d',
	'pointSprite3d',
]



def CompileCommand(src,out):
	if sys.platform.startswith('linux'):
		raise
	elif sys.platform.startswith('darwin'):
		vulkanSDK=os.environ['VULKAN_SDK']
		exe=os.path.join(vulkanSDK,"bin","glslangValidator")
		return [exe,"-V",src,"-o",out]
	elif sys.platform.startswith('win') or sys.platform.startswith('cygwin'):
		return ["glslangValidator.exe","-V",src,"-o",out]



def Compile(src,out):
	cmd=CompileCommand(src,out)
	print(cmd)
	proc=subprocess.Popen(cmd)
	proc.communicate()
	if 0!=proc.returncode:
		raise


def FNameToVarName(fName):
	base=os.path.basename(fName)
	return base.replace(".","_")


def main(argv):
	THISFILE=os.path.realpath(__file__)
	THISDIR=os.path.dirname(THISFILE)
	allBinaryFName=[]
	for fName in source:
		inFName=os.path.join(THISDIR,fName+".vert")
		outFName=os.path.join(THISDIR,"..","spirv",fName+".vert.spv")
		allBinaryFName.append(outFName);
		Compile(inFName,outFName)
		inFName=os.path.join(THISDIR,fName+".frag")
		outFName=os.path.join(THISDIR,"..","spirv",fName+".frag.spv")
		Compile(inFName,outFName)
		allBinaryFName.append(outFName);

	spvCFName=os.path.join(THISDIR,"..","..","fsvulkan_spirv.c")
	spvHFName=os.path.join(THISDIR,"..","..","fsvulkan_spirv.h")

	cFp=open(spvCFName,"w")
	hFp=open(spvHFName,"w")

	hFp.write("#ifndef FSVULKAN_SPIRV_H_IS_INCLUDED\n")
	hFp.write("#define FSVULKAN_SPIRV_H_IS_INCLUDED\n")
	hFp.write("#ifdef __cplusplus\n")
	hFp.write('extern "C" {\n')
	hFp.write("#endif\n")

	for fName in allBinaryFName:
		inFp=open(fName,"rb")
		binData=inFp.read()
		inFp.close()

		varName=FNameToVarName(fName)
		hFp.write("const int "+varName+"_len="+str(len(binData))+";\n")
		hFp.write("extern const unsigned char "+varName+"[];\n")

	hFp.write("#ifdef __cplusplus\n")
	hFp.write("}\n")
	hFp.write("#endif\n")

	hFp.write("#endif\n")

	cFp.write('#include "fsvulkan_spirv.h"\n')

	for fName in allBinaryFName:
		inFp=open(fName,"rb")
		if sys.version_info>=(3,0):
			binData=inFp.read()
		else:
			uselessStringFromPython2=inFp.read()
			binData=[]
			for uselessChrFromPython2 in uselessStringFromPython2:
				binData.append(ord(uselessChrFromPython2))
				# Python 2 is really REALLY EXTREMELY USELESS.
		inFp.close()

		varName=FNameToVarName(fName)
		cFp.write("const unsigned char "+varName+"[]=\n")
		cFp.write("{\n")
		byteCtr=0
		lineCtr=0
		for b in binData:
			if 0==(byteCtr%16):
				cFp.write("\t")
			elif 0==(byteCtr%4):
				cFp.write(" ")
			cFp.write("0x"+format(b,'02x')+",")
			if 15==(byteCtr%16):
				cFp.write("\n")
				lineCtr=lineCtr+1
				if 0==(lineCtr%16):
					cFp.write("\n")
			byteCtr=byteCtr+1
		cFp.write("\n};\n")



if __name__=='__main__':
	main(sys.argv)