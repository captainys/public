#F-BASIC Pre-Processor by Soji Yamakawa 2017/10/27

import os
import sys


usedFile={}
symbolMap={}
symPath=[]


def ReadFile(basInFn,depth):
	source=[]
	ifp=open(basInFn,"r")
	realBasInFn=os.path.realpath(basInFn)
	basInDir=os.path.dirname(realBasInFn)
	for s in ifp:
		s=s.replace('\n','').replace('\r','').replace('\t','  ')
		argv=s.split(' ')
		if 0<len(argv) and argv[0].upper()=="##INCLUDE":
			source=source+ReadFile(os.path.join(basInDir,argv[1]),depth+1)
		else:
			source.append(s)
	ifp.close()
	return source


def ReadSymFile(symSearchPath,symFileBase):
	for path in symSearchPath:
		symFile=os.path.join(path,symFileBase)
		if os.path.isfile(symFile):
			ifp=open(symFile,"r")
			for s in ifp:
				s=s.replace('\t',' ')
				argv=s.split(' ')
				if 3<=len(argv) and argv[1].upper()=='EQU':
					addr=int(argv[2].replace("#",""))
					symbolMap[argv[0]]=addr
					print(argv[0]+'='+hex(addr).replace('0x','&H'))
			ifp.close()
			return
	print("SYM file not found:")
	for path in symSearchPath:
		symFile=os.path.join(path,symFileBase)
		print("  Tried:"+symFile)
	raise


def PreProc(basInFn,basOutFn):
	code=[]
	labelToLine={}
	usedFile={}
	basInDir=os.path.dirname(basInFn)
	symFileLoaded=False

	rawList=ReadFile(basInFn,0)
	lineNum=10
	for s in rawList:
		s=s.replace('\n','')
		s=s.replace('\r','')
		s=s.replace('\t','  ')

		argv=s.split(' ')
		if '' in argv:
			argv.remove('')

		if 0==len(s):
			continue

		directive=""
		if 0<len(argv):
			directive=argv[0]

		if directive.startswith("$$"):  # Jump label
			print("["+directive+"]"+"->"+str(lineNum))
			labelToLine[directive]=lineNum
		elif directive.startswith("##"):  # Line num reset
			if directive.upper()=='##SYMFILE':
				allSymSearchPath=[basInDir]+symPath
				ReadSymFile(allSymSearchPath,argv[1])
				symFileLoaded=True
			else:
				newLineNum=int(directive[2:])
				if newLineNum<lineNum:
					print("Line number must not decrease.")
					raise
				lineNum=newLineNum
		else:
			code.append([lineNum,s])
			lineNum=lineNum+10

	codeIdx=0
	while codeIdx<len(code):
		s=code[codeIdx][1]
		state=0
		letterIdx=0
		label=""
		newS=""
		while letterIdx<len(s):
			if 0==state:
				if s[letterIdx:letterIdx+2]=="$$":
					label="$$"
					letterIdx=letterIdx+1
					state=1
				elif s[letterIdx:letterIdx+2]=="&&":
					if True!=symFileLoaded:
						print(".SYM file hasn't been loaded.\n")
						raise
					label=""
					letterIdx=letterIdx+1
					state=2
				else:
					newS=newS+s[letterIdx]
			elif 1==state:
				label=label+s[letterIdx]
				if letterIdx==len(s)-1 or s[letterIdx+1]==' ' or s[letterIdx+1]==':' or s[letterIdx+1]=='\t':
					if label in labelToLine:
						labelLine=labelToLine[label]
						newS=newS+str(labelLine)
						state=0
					else:
						print("Label "+label+" is not defined.\n")
						print("Source: "+code[codeIdx][1])
						raise
			elif 2==state:
				if s[letterIdx:letterIdx+2]=="&&":
					letterIdx=letterIdx+1 # Skip the first '&'

					fulUpDown=0
					addrOffset=0
					if label.startswith("UP:"):
						fulUpDown=1
						label=label.replace("UP:","")
					elif label.startswith("DN:"):
						fulUpDown=2
						label=label.replace("DN:","")
					elif label.startswith("1:"):
						addrOffset=1
						label=label.replace("1:","")

					if label in symbolMap:
						addr=symbolMap[label]+addrOffset
						if 1==fulUpDown:
							addr=((addr & 0xff00)>>8)
						elif 2==fulUpDown:
							addr=addr&0xff
						newS=newS+hex(addr).replace("0x","&H")
					else:
						print("Symbol "+label+" is not defined.\n")
						print("Source: "+code[codeIdx][1])
						raise
					state=0
				else:
					label=label+s[letterIdx]

			letterIdx=letterIdx+1
		code[codeIdx][1]=newS
		codeIdx=codeIdx+1

	ofp=open(basOutFn,"w")
	for c in code:
		ofp.write(str(c[0])+" "+c[1]+"\n")




def ProcessCommandLineParameter(argv):
	i=0
	while i<len(argv):
		if "-sympath"==argv[i] and i+1<len(argv):
			symPath=symPath+[argv[i+1]]
			i=i+2
		else:
			print("Unrecognized command parameter: "+argv[i])
			raise




def main():
	ProcessCommandLineParameter(sys.argv[3:])
	if 3<=len(sys.argv):
		PreProc(sys.argv[1],sys.argv[2])
	else:
		print("Usage: fbasic.py <source> <output> [options]")
		print("  Options:")
		print("    -sympath <path>")
		print("       Add .SYM search path")



if __name__=="__main__":
	main()
