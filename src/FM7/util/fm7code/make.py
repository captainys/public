import subprocess




def Assemble(asmFn,srecFn):
	proc=subprocess.Popen(["asm6809",asmFn,"-S","-o",srecFn])
	proc.communicate()
	if 0!=proc.returncode:
		raise



def MakeCpp(cppFn,srecFnVarPair,headerFn):
	cpp=[]

	cpp.append('#include "'+headerFn+'"')

	for pair in srecFnVarPair:
		srecFn=pair[0]
		varNm=pair[1]

		cpp.append('const char *const '+varNm+'[]=')
		cpp.append('{')

		ifp=open(srecFn,"r")
		for str in ifp:
			stripped=""

			for c in str:
				if c=="" or c=="\n" or c=="\r":
					continue
				stripped=stripped+c

			cpp.append('"'+stripped+'",')
		ifp.close()

		cpp.append('nullptr')
		cpp.append('};')


	for l in cpp:
		print(">>"+l)

	ofp=open(cppFn,"w")
	for l in cpp:
		ofp.write(l)
		ofp.write('\n')
	ofp.close()



Assemble("tommr.asm","tommr.srec")
Assemble("dskwrite.asm","dskwrite.srec")
Assemble("ura1.asm","ura1.srec")
Assemble("ura2.asm","ura2.srec")
Assemble("../../DiskFormat/formatm.asm","formatm.srec")

MakeCpp("../disk2tape/dskwrite.cpp",[["dskwrite.srec","dskWriteSREC"]],"dskwrite.h")
MakeCpp("../disk2tape/uraram.cpp",[["ura1.srec","ura1SREC"],["ura2.srec","ura2SREC"]],"uraram.h")
MakeCpp("../disk2tape/formatm.cpp",[["formatm.srec","formatmSREC"]],"formatm.h")
MakeCpp("../disk2tape/tommr.cpp",[["tommr.srec","toMMRSREC"]],"tommr.h")

