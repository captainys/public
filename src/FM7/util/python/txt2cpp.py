import sys
import os


# Limitation: header and cpp needs to be in the same directory.


def TextFileToCpp(cppFName,hFName,varName,binFName):
	strArray=[]
	fp=open(binFName,"r")
	for str in fp:
		str=str.replace('\n','')
		str=str.replace('\r','')
		strArray.append(str)
	fp.close()
	TextToCpp(cppFName,hFName,varName,strArray)



def TextToCpp(cppFName,hFName,varName,strArray):
	HFNAME=hFName.upper().replace(".","_")
	HFNAME=HFNAME.replace("/","_")
	HFNAME=HFNAME.replace("\\","_")

	hFp=open(hFName,"w")
	hFp.write("#ifndef "+HFNAME+"_IS_INCLUDED\n")
	hFp.write("#define "+HFNAME+"_IS_INCLUDED\n")
	hFp.write("\n");
	hFp.write("extern const char * const "+varName+"[];\n");
	hFp.write("\n");
	hFp.write("#endif\n")

	hFp.close()

	cppFp=open(cppFName,"w")
	cppFp.write('#include "'+os.path.basename(hFName)+'"\n')
	cppFp.write("\n")
	cppFp.write("const char * const "+varName+"[]=\n")
	cppFp.write("{\n");

	for s in strArray:
		cppFp.write('\t"'+s+'",\n')
	cppFp.write("\tnullptr,\n")

	cppFp.write("};\n");
	cppFp.close();



def main():
	TextFileToCpp(sys.argv[1],sys.argv[2],sys.argv[3],sys.argv[4])



if __name__=="__main__":
	main()
