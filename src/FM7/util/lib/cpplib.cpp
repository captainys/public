#include "cpplib.h"



int FM7Lib::Xtoi(const char str[])
{
	int n=0;
	while(0!=str[0])
	{
		if('0'<=*str && *str<='9')
		{
			n*=16;
			n+=(*str-'0');
		}
		else if('a'<=*str && *str<='f')
		{
			n*=16;
			n+=(10+*str-'a');
		}
		else if('A'<=*str && *str<='F')
		{
			n*=16;
			n+=(10+*str-'A');
		}
		else
		{
			break;
		}
		++str;
	}
	return n;
}

int FM7Lib::Atoi(const char str[])
{
	if(nullptr==str)
	{
		return 0;
	}

	int sign=1;
	if('-'==str[0])
	{
		sign=-1;
		++str;
	}

	int n=0;
	if(('0'==str[0] && ('x'==str[1] || 'X'==str[1])) ||
	   ('&'==str[0] && ('h'==str[1] || 'H'==str[1])))
	{
		n=Xtoi(str+2);
	}
	else if('$'==str[0])
	{
		n=Xtoi(str+1);
	}
	else
	{
		bool hexaDecimal=false;
		for(int i=0; str[i]!=0; ++i)
		{
			if(('h'==str[i] || 'H'==str[i]) &&
			   (str[i+1]<'a' || 'z'<str[i+1]) &&
			   (str[i+1]<'A' || 'Z'<str[i+1]) &&
			   (str[i+1]<'0' || '9'<str[i+1]))
			{
				hexaDecimal=true;
				break;
			}
		}
		if(true==hexaDecimal)
		{
			n=Xtoi(str);
		}
		else
		{
			while(0!=*str)
			{
				if('0'<=*str && *str<='9')
				{
					n*=10;
					n+=(*str-'0');
				}
				else
				{
					break;
				}
				++str;
			}
		}
	}
	return n*sign;
}


std::vector <unsigned char> FM7Lib::ReadBinaryFile(const char fn[])
{
	std::vector <unsigned char> dat;
	FILE *fp=fopen(fn,"rb");
	if(nullptr!=fp)
	{
		dat=ReadBinaryFile(fp);
		fclose(fp);
	}
	return dat;
}

void FM7Lib::UIntToIntelByteOrder(unsigned char bin[4],unsigned int i)
{
	bin[0]=i&255;
	bin[1]=(i>>8)&255;
	bin[2]=(i>>16)&255;
	bin[3]=(i>>24)&255;
}

void FM7Lib::UShortToIntelByteOrder(unsigned char bin[2],unsigned short i)
{
	bin[0]=i&255;
	bin[1]=(i>>8)&255;
}

std::vector <unsigned char> FM7Lib::ReadBinaryFile(FILE *fp)
{
	std::vector <unsigned char> dat;

	fseek(fp,0,SEEK_END);
	auto fSize=ftell(fp);
	fseek(fp,0,SEEK_SET);

	dat.resize(fSize);
	fread(dat.data(),1,fSize,fp);

	return dat;
}

bool FM7Lib::WriteBinaryFile(const char fn[],const std::vector <unsigned char> &dat)
{
	FILE *fp=fopen(fn,"wb");
	if(nullptr!=fp)
	{
		auto r=WriteBinaryFile(fp,dat);
		fclose(fp);
		return r;
	}
	return false;
}

bool FM7Lib::WriteBinaryFile(FILE *fp,const std::vector <unsigned char> &dat)
{
	if(dat.size()==fwrite(dat.data(),1,dat.size(),fp))
	{
		return true;
	}
	return false;
}



std::vector <std::string> FM7Lib::ReadTextFile(const char fn[])
{
	std::vector <std::string> dat;
	FILE *fp=fopen(fn,"rb");
	if(nullptr!=fp)
	{
		dat=ReadTextFile(fp);
		fclose(fp);
	}
	return dat;
}

std::vector <std::string> FM7Lib::ReadTextFile(FILE *fp)
{
	std::vector <std::string> dat;

	std::string curStr;
	char buf[256];
	while(nullptr!=fgets(buf,255,fp))
	{
		curStr.append(buf);
		if(curStr.back()=='\n' || curStr.back()=='\r')
		{
			while(0<curStr.size() && (curStr.back()=='\n' || curStr.back()=='\r'))
			{
				curStr.pop_back();
			}
			dat.push_back((std::string &&)curStr);
			curStr.clear();
		}
	}

	if(0<curStr.size())
	{
		dat.push_back((std::string &&)curStr);
	}

	return dat;
}

bool FM7Lib::WriteTextFile(const char fn[],const std::vector <std::string> &text)
{
	FILE *fp=fopen(fn,"w");
	if(nullptr!=fp)
	{
		auto r=FM7Lib::WriteTextFile(fp,text);
		fclose(fp);
		return r;
	}
	return false;
}

bool FM7Lib::WriteTextFile(FILE *fp,const std::vector <std::string> &text)
{
	for(auto & s : text)
	{
		fprintf(fp,"%s\n",s.c_str());
	}
	return true;
}

std::vector <unsigned char> FM7Lib::TextToByteArray(const std::vector <std::string> &text,const char lineBreak[])
{
	std::vector <unsigned char> dat;
	for(int i=0; 0!=lineBreak[i]; ++i)
	{
		dat.push_back(lineBreak[i]);
	}

	for(auto &s : text)
	{
		for(auto c : s)
		{
			dat.push_back(c);
		}
		for(int i=0; 0!=lineBreak[i]; ++i)
		{
			dat.push_back(lineBreak[i]);
		}
	}
	return dat;
}

int FM7Lib::LetterToNum(char c)
{
	if('0'<=c && c<='9')
	{
		return c-'0';
	}
	else if('A'<=c && c<='F')
	{
		return 10+c-'A';
	}
	else if('a'<=c && c<='f')
	{
		return 10+c-'a';
	}
	return 0;
}

std::vector <std::string> FM7Lib::QuickParser(const char str[])
{
	int state=0;
	std::string curStr;
	std::vector <std::string> argv;
	for(int i=0; 0!=str[i]; ++i)
	{
		if(0==state)
		{
			if(' '!=str[i] && '\t'!=str[i])
			{
				curStr.push_back(str[i]);
				state=1;
			}
		}
		else if(1==state)
		{
			if(' '==str[i] || '\t'==str[i] || 0==str[i+1])
			{
				if(' '!=str[i] && '\t'!=str[i])
				{
					curStr.push_back(str[i]);
				}
				argv.push_back((std::string &&)curStr);
				curStr="";
				state=0;
			}
			else
			{
				curStr.push_back(str[i]);
			}
		}
	}

	if(0<curStr.size())
	{
		argv.push_back((std::string &&)curStr);
	}

	return argv;
}

void FM7Lib::Capitalize(std::string &s)
{
	for(auto &c : s)
	{
		if('a'<=c && c<='z')
		{
			c=c+'A'-'a';
		}
	}
}

std::string FM7Lib::GetExtension(const char fName[])
{
	if(nullptr==fName)
	{
		return "";
	}

	int lastDot=-1;
	for(int i=0; 0!=fName[i]; ++i)
	{
		if('.'==fName[i])
		{
			lastDot=i;
		}
	}

	if(0<=lastDot)
	{
		std::string ext(fName+lastDot);
		return ext;
	}
	return "";
}

std::string FM7Lib::ChangeExtension(const char orgFName[],const char newExt[])
{
	int lastDot=-1;
	for(int i=0; 0!=orgFName[i]; ++i)
	{
		if('.'==orgFName[i])
		{
			lastDot=i;
		}
	}

	std::string newFn;

	if(0>lastDot)
	{
		newFn=orgFName;
	}
	else
	{
		for(int i=0; i<lastDot; ++i)
		{
			newFn.push_back(orgFName[i]);
		}
	}

	for(int i=0; 0!=newExt[i]; ++i)
	{
		newFn.push_back(newExt[i]);
	}

	return newFn;
}

std::string FM7Lib::RemoveExtension(const char orgFName[])
{
	int lastDot=-1;
	for(int i=0; 0!=orgFName[i]; ++i)
	{
		if('.'==orgFName[i])
		{
			lastDot=i;
		}
	}

	std::string newFn;

	if(0>lastDot)
	{
		newFn=orgFName;
	}
	else
	{
		for(int i=0; i<lastDot; ++i)
		{
			newFn.push_back(orgFName[i]);
		}
	}

	return newFn;
}



std::string FM7Lib::GetBaseName(const char path[])
{
	if(nullptr==path)
	{
		return "";
	}

	int lastSlash=0;
	for(int i=0; path[i]!=0; ++i)
	{
		if(':'==path[i] || '/'==path[i] || '\\'==path[i])
		{
			lastSlash=i;
		}
	}

	std::string basename;
	for(int i=lastSlash+1; 0!=path[i]; ++i)
	{
		basename.push_back(path[i]);
	}

	return basename;
}



bool FM7Lib::FileExist(const char fn[])
{
	FILE *fp=fopen(fn,"rb");
	if(nullptr!=fp)
	{
		fclose(fp);
		return true;
	}
	return false;
}



bool FM7Lib::StrStartsWith(const std::string &str,const char ptn[])
{
	int i;
	for(i=0; i<str.size() && 0!=ptn[i]; ++i)
	{
		if(str[i]!=ptn[i])
		{
			return false;
		}
	}
	if(0==ptn[i])
	{
		return true;
	}
	return false;
}

const char *FM7Lib::StrSkip(const char str[],const char ptn[])
{
	for(int i=0; 0!=str[i]; ++i)
	{
		if(str[i]==ptn[0])
		{
			int j;
			for(j=0; 0!=str[i+j] && 0!=ptn[j]; ++j)
			{
				if(str[i+j]!=ptn[j])
				{
					goto NEXTI;
				}
			}
			if(0==ptn[j])
			{
				return str+i+j;
			}
		}
	NEXTI:
		;
	}
	return nullptr;
}

const char *FM7Lib::StrSkipSpace(const char str[])
{
	int i;
	for(i=0; 0!=str[i]; ++i)
	{
		if(' '!=str[i] && '\t'!=str[i])
		{
			return str+i;
		}
	}
	return str+i;
}

const char *FM7Lib::StrSkipNonSpace(const char str[])
{
	int i;
	for(i=0; 0!=str[i]; ++i)
	{
		if(' '==str[i] || '\t'==str[i])
		{
			return str+i;
		}
	}
	return str+i;
}


std::vector <unsigned char> FM7Lib::MakeFMFileHeader(const char filenameInD77[],unsigned char byte10,unsigned char byte11)
{
	std::vector <unsigned char> dat;

	for(int i=0; 0!=filenameInD77[i] && i<8; ++i)
	{
		dat.push_back(filenameInD77[i]);
	}
	while(dat.size()<8)
	{
		dat.push_back(' ');
	}
	dat.push_back(0);
	dat.push_back(0);

	dat.push_back(byte10);
	dat.push_back(byte11);

	dat.push_back(0);
	dat.push_back('X');
	dat.push_back('M');
	dat.push_back('7');

	return dat;
}



std::vector <unsigned char> FM7Lib::TextTo0A0(const std::vector <std::string> &text,const char filenameInD77[])
{
	std::vector <unsigned char> dat=MakeFMFileHeader(filenameInD77,0x00,0xff);

	dat.push_back(0x0d);
	dat.push_back(0x0a);
	for(auto &s : text)
	{
		for(auto c : s)
		{
			dat.push_back(c);
		}
		dat.push_back(0x0d);
		dat.push_back(0x0a);
	}
	dat.push_back(0x1a);
	return dat;
}

std::vector <unsigned char> FM7Lib::TextTo0A0(const char * const text[],const char filenameInD77[])
{
	std::vector <unsigned char> dat=MakeFMFileHeader(filenameInD77,0x00,0xff);

	dat.push_back(0x0d);
	dat.push_back(0x0a);
	for(int i=0; nullptr!=text[i]; ++i)
	{
		auto s=text[i];
		for(int j=0; 0!=text[i][j]; ++j)
		{
			dat.push_back(text[i][j]);
		}
		dat.push_back(0x0d);
		dat.push_back(0x0a);
	}
	dat.push_back(0x1a);
	return dat;
}

std::vector <unsigned char> FM7Lib::BinaryTo2B0(const std::vector <unsigned char> &bin,const char filenameInD77[],int storeAddr,int execAddr)
{
	auto dat=MakeFMFileHeader(filenameInD77,0x02,0x00);
	auto img=BinaryToStoreImage(bin,storeAddr,execAddr);
	dat.insert(dat.end(),img.begin(),img.end());
	dat.push_back(0x1a);
	return dat;
}



std::vector <unsigned char> FM7Lib::BinaryToStoreImage(const std::vector <unsigned char> &dat,int storeAddr,int execAddr)
{
	std::vector <unsigned char> img;

	img.push_back(0);

	img.push_back((dat.size()&0xff00)>>8);
	img.push_back( dat.size()&0xff);

	img.push_back((storeAddr&0xff00)>>8);
	img.push_back( storeAddr&0xff);

	img.insert(img.end(),dat.begin(),dat.end());

	img.push_back(0xff);
	img.push_back(0);
	img.push_back(0);

	img.push_back((execAddr&0xff00)>>8);
	img.push_back( execAddr&0xff);

	return img;
}

std::string FM7Lib::GetFileNameFromFMFile(const std::vector <unsigned char> &dat)
{
	std::string fn;
	for(int i=0; i<8 && i<dat.size() && dat[i]!=0; ++i)
	{
		fn.push_back(dat[i]);
	}
	while(0<fn.size() && ' '==fn.back())
	{
		fn.pop_back();
	}
	return fn;
}

std::vector <unsigned char> FM7Lib::RawHexToByteData(const std::vector <std::string> &rawHexTxt)
{
	std::string allText;
	for(auto &s : rawHexTxt)
	{
		for(auto c : s)
		{
			if(('a'<=c && c<='f') || ('A'<=c && c<='F') || ('0'<=c && c<='9'))
			{
				allText.push_back(c);
			}
		}
	}

	std::vector <unsigned char> dat;
	for(int ptr=0; ptr+2<=allText.size(); ptr+=2)
	{
		char wd[3]={allText[ptr],allText[ptr+1],0};
		dat.push_back(FM7Lib::Xtoi(wd));
	}

	return dat;
}

const char *FM7Lib::BoolToStr(bool b)
{
	if(b==true)
	{
		return "TRUE";
	}
	else
	{
		return "FALSE";
	}
}

/*! Convert a string to a std::vector of unsigned chars.
    If the string starts with ' or ", it is taken as an ASCII string.
    Otherwise it is taken as a hexa-decimal numbers.

	818A9B9C -> {0x81,0x8A,0x9B,0x9C}
	"ABCDEF" -> {'A','B','C','D','E','F'}
*/
std::vector <unsigned char> FM7Lib::StrToByteArray(const char str[])
{
	std::vector <unsigned char> ptn;
	if('\"'!=str[0] && '\''!=str[0])
	{
		for(int i=0; 0!=str[i] && 0!=str[i+1]; i+=2)
		{
			char buf[3];
			buf[0]=str[i];
			buf[1]=str[i+1];
			buf[2]=0;
			ptn.push_back(FM7Lib::Xtoi(buf));
		}
	}
	else
	{
		for(int i=1; 0!=str[i] && '\"'!=str[i] && '\''!=str[i]; ++i)
		{
			if('\\'==str[i] && 0!=str[i+1])
			{
				++i;
			}
			ptn.push_back(str[i]);
		}
	}
	return ptn;
}

