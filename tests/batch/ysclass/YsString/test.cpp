/* ////////////////////////////////////////////////////////////

File Name: test.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include <ysclass.h>

template <class CHARTYPE>
YsArray <CHARTYPE> Conv(const char ascii[])
{
	YsArray <CHARTYPE> s;
	for(int i=0; 0!=ascii[i]; ++i)
	{
		s.push_back(ascii[i]);
	}
	s.push_back(0);
	return s;
}

template <class STRTYPE>
YSRESULT TestGeneral(void)
{
	YSRESULT res=YSOK;
	int nRepl=0;

	STRTYPE str(Conv<typename STRTYPE::CHAR_T>("ABCDEFG"));
	str.InsertString(100,Conv<typename STRTYPE::CHAR_T>("xyz"));
	if(0!=str.Strcmp(Conv<typename STRTYPE::CHAR_T>("ABCDEFGxyz")))
	{
		fprintf(stderr,"Error in InsertString as append.\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: InsertString as append\n");
	}

	str.Insert(100,'@',3);
	if(0!=str.Strcmp(Conv<typename STRTYPE::CHAR_T>("ABCDEFGxyz@@@")))
	{
		fprintf(stderr,"Error in Insert as append.\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: Insert as append\n");
	}

	str.InsertString(3,Conv<typename STRTYPE::CHAR_T>("#$%"));
	if(0!=str.Strcmp(Conv<typename STRTYPE::CHAR_T>("ABC#$%DEFGxyz@@@")))
	{
		fprintf(stderr,"Error in InsertString.\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: InsertString.\n");
	}

	str.Insert(6,'+',4);
	if(0!=str.Strcmp(Conv<typename STRTYPE::CHAR_T>("ABC#$%++++DEFGxyz@@@")))
	{
		fprintf(stderr,"Error in Insert.\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: Insert.\n");
	}

	nRepl=str.Replace(Conv<typename STRTYPE::CHAR_T>("xyz"),Conv<typename STRTYPE::CHAR_T>("012"));
	if(0!=str.Strcmp(Conv<typename STRTYPE::CHAR_T>("ABC#$%++++DEFG012@@@")) || 1!=nRepl)
	{
		fprintf(stderr,"Error in Replace (Same letter count).\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: Replace (Same letter count). (Count=%d)\n",nRepl);
	}

	nRepl=str.Replace(Conv<typename STRTYPE::CHAR_T>("G012"),Conv<typename STRTYPE::CHAR_T>("()"));
	if(0!=str.Strcmp(Conv<typename STRTYPE::CHAR_T>("ABC#$%++++DEF()@@@")) || 1!=nRepl)
	{
		fprintf(stderr,"Error in Replace (Make shorter).\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: Replace (Make shorter). (Count=%d)\n",nRepl);
	}

	nRepl=str.Replace(Conv<typename STRTYPE::CHAR_T>("()"),Conv<typename STRTYPE::CHAR_T>("()()"));
	if(0!=str.Strcmp(Conv<typename STRTYPE::CHAR_T>("ABC#$%++++DEF()()@@@")) || 1!=nRepl)
	{
		fprintf(stderr,"Error in Replace (Make longer).\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: Replace (Make longer). (Count=%d)\n",nRepl);
	}

	nRepl=str.Replace(Conv<typename STRTYPE::CHAR_T>("++++"),Conv<typename STRTYPE::CHAR_T>("pqrs"));
	if(0!=str.STRCMP(Conv<typename STRTYPE::CHAR_T>("abc#$%PQRSdef()()@@@")) || 1!=nRepl)
	{
		fprintf(stderr,"Error in case-insensitive compare.\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: case-insensitive compare.\n");
	}
	if(0!=str.Strcmp(Conv<typename STRTYPE::CHAR_T>("ABC#$%pqrsDEF()()@@@")))
	{
		fprintf(stderr,"Error in case-sensitive compare.\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: case-sensitive compare. (Count=%d)\n",nRepl);
	}

	str.InsertString(3,Conv<typename STRTYPE::CHAR_T>("@@@"));
	nRepl=str.Replace(Conv<typename STRTYPE::CHAR_T>("@@@"),Conv<typename STRTYPE::CHAR_T>("1234"));
	if(0!=str.Strcmp(Conv<typename STRTYPE::CHAR_T>("ABC1234#$%pqrsDEF()()1234")) || 2!=nRepl)
	{
		fprintf(stderr,"Error in multiple-replace.\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: multiple-replace. (Count=%d)\n",nRepl);
	}

	YSSIZE_T loc;
	if(YSTRUE!=str.FindWord(&loc,Conv<typename STRTYPE::CHAR_T>("DEF()()")) || 14!=loc)
	{
		fprintf(stderr,"Error in FindWord.\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: FindWord (loc=%d)\n",(int)loc);
	}
	if(YSTRUE!=str.FINDWORD(&loc,Conv<typename STRTYPE::CHAR_T>("$%PqRs")) || 8!=loc)
	{
		fprintf(stderr,"Error in FINDWORD.\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: FINDWORD (loc=%d)\n",(int)loc);
	}

	nRepl=str.REPLACE(Conv<typename STRTYPE::CHAR_T>("PQRSdef"),Conv<typename STRTYPE::CHAR_T>("&&&"));
	if(0!=str.Strcmp(Conv<typename STRTYPE::CHAR_T>("ABC1234#$%&&&()()1234")) || 1!=nRepl)
	{
		fprintf(stderr,"Error in case-insensitive-replace.\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: case-insensitive-replace. (Count=%d)\n",nRepl);
	}

	if(STRTYPE::Strlen(str.c_str())!=str.Strlen())
	{
		fprintf(stderr,"Error in Strlen.\n");
		res=YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: Strlen.\n");
	}

	if(str.c_str()!=str.data())
	{
		fprintf(stderr,"Error in c_str or data.\n");
		res=YSERR;
	}
	fprintf(stderr,"Pass: c_str and data.\n");


	printf("Final: ");
	for(auto c : str)
	{
		printf("%c",c);
	}
	printf("\n");

	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

template <class STRTYPE>
YSRESULT TestSTLLikeFunction(void)
{
	STRTYPE str;

	str.push_back('x');
	str.clear();
	if(0!=str.size() || 0!=str.length() || true!=str.empty())
	{
		fprintf(stderr,"Error in size, length, or empty\n");
		return YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: size,length, and empty\n");
	}


	str.push_back('a');
	str.push_back('b');
	str.push_back('c');
	str.push_back('d');
	str.push_back('e');
	if(5!=str.size() || 5!=str.length() || true==str.empty())
	{
		fprintf(stderr,"Error in size, length, or empty\n");
		return YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: size, length, and empty\n");
	}
	if('a'!=str.front() || 'e'!=str.back())
	{
		fprintf(stderr,"Error in front or back\n");
		return YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: front and back\n");
	}


	str.push_back('0');
	str.pop_back();
	if('e'!=str.back())
	{
		fprintf(stderr,"Error in pop_back\n");
		return YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: pop_back\n");
	}


	const char inputStr[6]="abcde";

	int ctr=0;
	for(auto c : str)
	{
		printf("%c %c\n",c,inputStr[ctr]);
		if(c!=inputStr[ctr])
		{
			fprintf(stderr,"Error in the iterator!\n");
			return YSERR;
		}
		++ctr;
	}
	if(5!=ctr)
	{
		fprintf(stderr,"Error in the iterator (overrun or underrun) ctr=%d!\n",ctr);
		return YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: iterator\n");
	}

	ctr=4;
	for(auto riter=str.rbegin(); riter!=str.rend(); riter++)
	{
		printf("%c %c\n",*riter,inputStr[ctr]);
		if(*riter!=inputStr[ctr])
		{
			fprintf(stderr,"Error in the reverse iterator!\n");
			return YSERR;
		}
		--ctr;
	}
	if(-1!=ctr)
	{
		fprintf(stderr,"Error in the reverse iterator (overrun or underrun) ctr=%d!\n",ctr);
		return YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: reverse iterator\n");
	}


	auto sub=str.substr(1,3);
	if(3!=sub.length() || 'b'!=sub[0] || 'c'!=sub[1] || 'd'!=sub[2])
	{
		fprintf(stderr,"Error in substr\n");
		return YSERR;
	}
	sub=str.substr(2);
	if(3!=sub.length() || 'c'!=sub[0] || 'd'!=sub[1] || 'e'!=sub[2])
	{
		fprintf(stderr,"Error in substr\n");
		return YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: substr\n");
	}


	typename STRTYPE::CHAR_T codeToAppend[]={'0','1','2','3',0};
	typename STRTYPE::CHAR_T codeToAppend2[]={'#','$','%','^',0};
	typename STRTYPE::CHAR_T codeAppended[]={'a','b','c','d','e','0','1','2','3',0};
	typename STRTYPE::CHAR_T codeAppended2[]={'a','b','c','d','e','0','1','2','3','#','$','%','^',0};
	STRTYPE toAppend(codeToAppend),toAppend2(codeToAppend2),appended(codeAppended),appended2(codeAppended2);

	auto plusEqual=str;
	plusEqual+=toAppend;
	if(appended!=plusEqual)
	{
		fprintf(stderr,"Error in operator+=\n");
		return YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: operator+=\n");
	}

	auto plus=str+toAppend+toAppend2;
	if(appended2!=plus)
	{
		fprintf(stderr,"Error in operator+\n");
		return YSERR;
	}
	else
	{
		fprintf(stderr,"Pass: operator+\n");
	}


	toAppend.swap(toAppend2);
	ctr=0;
	for(auto c : toAppend)
	{
		if(c!=codeToAppend2[ctr])
		{
			fprintf(stderr,"Error in swap\n");
			return YSERR;
		}
		++ctr;
	}
	ctr=0;
	for(auto c : toAppend2)
	{
		if(c!=codeToAppend[ctr])
		{
			fprintf(stderr,"Error in swap\n");
			return YSERR;
		}
		++ctr;
	}
	fprintf(stderr,"Pass: swap\n");



	typename STRTYPE::CHAR_T replTestSrc[]={'a','b','c','d','e','0','1','2','3','*','+','-','=',0};
	typename STRTYPE::CHAR_T replTestSubst1[]={'p','q','r','s',0};
	typename STRTYPE::CHAR_T replTestSrc1[]={'p','q','r','s','d','e','0','1','2','3','*','+','-','=',0};
	typename STRTYPE::CHAR_T replTestSubst2[]={'(',')','(',')',0};
	typename STRTYPE::CHAR_T replTestSrc2[]={'p','q','r','s','d','e','(',')','(',')','*','+','-','=',0};
	typename STRTYPE::CHAR_T replTestSubst3[]={'X','Y','Z',0};
	typename STRTYPE::CHAR_T replTestSrc3[]={'p','q','r','X','Y','Z','d','e','(',')','(',')','*','+','-','=',0};
	typename STRTYPE::CHAR_T replTestSubst4[]={'y','s','f',0};
	typename STRTYPE::CHAR_T replTestSrc4[]={'p','q','r','y','s','f',0};
	STRTYPE repl(replTestSrc);
	repl.replace(0,3,replTestSubst1);
	if(repl!=STRTYPE(replTestSrc1))
	{
		fprintf(stderr,"Error in replace.\n");
	}
	repl.replace(6,4,replTestSubst2);
	if(repl!=STRTYPE(replTestSrc2))
	{
		fprintf(stderr,"Error in replace.\n");
	}
	repl.replace(3,1,replTestSubst3);
	if(repl!=STRTYPE(replTestSrc3))
	{
		fprintf(stderr,"Error in replace.\n");
	}
	fprintf(stderr,"Pass: replace\n");
	repl.replace(3,repl.npos,replTestSubst4);
	if(repl!=STRTYPE(replTestSrc4))
	{
		fprintf(stderr,"Error in replace.\n");
	}

	fprintf(stderr,"Pass: STL interoperability functions\n");

	return YSOK;
}

template <class STRTYPE>
YSRESULT TestCompare(void)
{
	typename STRTYPE::CHAR_T src0[]={'A','B','C',0};
	typename STRTYPE::CHAR_T src1[]={'A','B','C','D',0};
	typename STRTYPE::CHAR_T src2[]={'X','Y','Z',0};

	STRTYPE str0(src0),str1(src1),str2(src2),str0copy(src0);

	if(str0!=str0copy)
	{
		fprintf(stderr,"Error in operator!=\n");
		return YSERR;
	}
	fprintf(stderr,"Pass: operator!=\n");

	if(!(str0==str0copy && str0>=str0copy && str0<=str0copy))
	{
		fprintf(stderr,"Error in operator==, >=, or <=\n");
		return YSERR;
	}
	fprintf(stderr,"Pass: operator==, >=, and <=\n");

	if(str0>str1 || str1>str2 || str0>=str1 || str1>=str2)
	{
		fprintf(stderr,"Error in operator> or >=\n");
		return YSERR;
	}
	if(!(str1>str0 && str2>str1 && str1>=str0 && str2>=str1))
	{
		fprintf(stderr,"Error in operator> or >=\n");
		return YSERR;
	}
	fprintf(stderr,"Pass: operator> and >=\n");

	if(str1<str0 || str2<str1 || str1<=str0 || str2<=str1)
	{
		fprintf(stderr,"Error in operator< or <=\n");
		return YSERR;
	}
	if(!(str0<str1 && str1<str2 && str0<=str1 && str1<=str2))
	{
		fprintf(stderr,"Error in operator< or <=\n");
		return YSERR;
	}
	fprintf(stderr,"Pass: operator< and <=\n");

	if(0>=str1.compare(str0) || str2.compare(str1)<=0 || 0!=str0.compare(str0copy))
	{
		fprintf(stderr,"Error in compare\n");
		return YSERR;
	}
	fprintf(stderr,"Pass: compare\n");

	return YSOK;
}

int main(void)
{
	int nFail=0;
	if(YSOK!=TestGeneral<YsWString>())
	{
		++nFail;
	}
	if(YSOK!=TestGeneral<YsString>())
	{
		++nFail;
	}

	if(YSOK!=TestSTLLikeFunction<YsString>())
	{
		++nFail;
	}
	if(YSOK!=TestSTLLikeFunction<YsWString>())
	{
		++nFail;
	}

	if(YSOK!=TestCompare<YsString>())
	{
		++nFail;
	}
	if(YSOK!=TestCompare<YsWString>())
	{
		++nFail;
	}

	printf("%d failed.\n",nFail);
	if(0<nFail)
	{
		return 1;
	}
	return 0;
}
