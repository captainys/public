import sys
import os


# Limitation: header and cpp needs to be in the same directory.


def BinaryFileToCpp(cppFName,hFName,varName,binFName):
	fp=open(binFName,"rb")
	byteArray=fp.read()
	fp.close()
	BinaryToCpp(cppFName,hFName,varName,byteArray)



def BinaryToCpp(cppFName,hFName,varName,byteArray):
	HFNAME=hFName.upper().replace(".","_")
	HFNAME=HFNAME.replace("/","_")
	HFNAME=HFNAME.replace("\\","_")

	hFp=open(hFName,"w")
	hFp.write("#ifndef "+HFNAME+"_IS_INCLUDED\n")
	hFp.write("#define "+HFNAME+"_IS_INCLUDED\n")
	hFp.write("\n");
	hFp.write("const long long int "+varName+"_size="+str(len(byteArray))+";\n");
	hFp.write("extern const unsigned char "+varName+"[];\n");
	hFp.write("\n");
	hFp.write("#endif\n")

	hFp.close()

	cppFp=open(cppFName,"w")
	cppFp.write('#include "'+os.path.basename(hFName)+'"\n')
	cppFp.write("\n")
	cppFp.write("const unsigned char "+varName+"[]=\n")
	cppFp.write("{\n");

	column=0;
	for b in byteArray:
		if 0==column:
			cppFp.write("\t")

		h="0x{:02x}".format(b)
		cppFp.write(h+",")

		column+=1
		if 16==column:
			cppFp.write("\n")
			column=0
	if 0<column:
		cppFp.write("\n")

	cppFp.write("};\n");
	cppFp.close();



def main():
	BinaryFileToCpp(sys.argv[1],sys.argv[2],sys.argv[3],sys.argv[4])



if __name__=="__main__":
	main()
