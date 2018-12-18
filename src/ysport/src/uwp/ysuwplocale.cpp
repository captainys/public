/* ////////////////////////////////////////////////////////////

File Name: ysuwplocale.cpp
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

#include "../yslocale.h"

// Test result:
//   LOCALE_SNAME works on Windows 7 does NOT work on Windows XP
//   LOCALE_ILANGUAGE works on Windows 7 and Windows XP

// Table created based on http://msdn.microsoft.com/en-us/library/dd373765%28v=vs.85%29.aspx
const char *const WindowsLanguageCodeToStandardLocaleStringTable[]=
{
	"0436","af-ZA",
	"041c","sq-AL",
	"0484","gsw-FR",
	"045e","am-ET",
	"1401","ar-DZ",
	"3c01","ar-BH",
	"0c01","ar-EG",
	"0801","ar-IQ",
	"2c01","ar-JO",
	"3401","ar-KW",
	"3001","ar-LB",
	"1001","ar-LY",
	"1801","ar-MA",
	"2001","ar-OM",
	"4001","ar-QA",
	"0401","ar-SA",
	"2801","ar-SY",
	"1c01","ar-TN",
	"3801","ar-AE",
	"2401","ar-YE",
	"042b","hy-AM",
	"044d","as-IN",
	"082c","az-Cyrl-AZ",
	"042c","az-Latn-AZ",
	"046d","ba-RU",
	"042d","eu-ES",
	"0423","be-BY",
	"0445","bn-IN",
	"201a","bs-Cyrl-BA",
	"141a","bs-Latn-BA",
	"047e","br-FR",
	"0402","bg-BG",
	"0403","ca-ES",
	"0c04","zh-HK",
	"1404","zh-MO",
	"0804","zh-CN",
	"1004","zh-SG",
	"0404","zh-TW",
	"101a","hr-BA",
	"041a","hr-HR",
	"0405","cs-CZ",
	"0406","da-DK",
	"048c","prs-AF",
	"0465","dv-MV",
	"0813","nl-BE",
	"0413","nl-NL",
	"0c09","en-AU",
	"2809","en-BZ",
	"1009","en-CA",
	"2409","en-029",
	"4009","en-IN",
	"1809","en-IE",
	"2009","en-JM",
	"4409","en-MY",
	"1409","en-NZ",
	"3409","en-PH",
	"4809","en-SG",
	"1c09","en-ZA",
	"2c09","en-TT",
	"0809","en-GB",
	"0409","en-US",
	"3009","en-ZW",
	"0425","et-EE",
	"0438","fo-FO",
	"0464","fil-PH",
	"040b","fi-FI",
	"080c","fr-BE",
	"0c0c","fr-CA",
	"040c","fr-FR",
	"140c","fr-LU",
	"180c","fr-MC",
	"100c","fr-CH",
	"0462","fy-NL",
	"0456","gl-ES",
	"0437","ka-GE",
	"0c07","de-AT",
	"0407","de-DE",
	"1407","de-LI",
	"1007","de-LU",
	"0807","de-CH",
	"0408","el-GR",
	"046f","kl-GL",
	"0447","gu-IN",
	"0468","ha-Latn-NG",
	"040d","he-IL",
	"0439","hi-IN",
	"040e","hu-HU",
	"040f","is-IS",
	"0470","ig-NG",
	"0421","id-ID",
	"085d","iu-Latn-CA",
	"045d","iu-Cans-CA",
	"083c","ga-IE",
	"0410","it-IT",
	"0810","it-CH",
	"0411","ja-JP",
	"044b","kn-IN",
	"043f","kk-KZ",
	"0453","kh-KH",
	"0486","qut-GT",
	"0487","rw-RW",
	"0457","kok-IN",
	"0412","ko-KR",
	"0440","ky-KG",
	"0454","lo-LA",
	"0426","lv-LV",
	"0427","lt-LT",
	"082e","dsb-DE",
	"046e","lb-LU",
	"042f","mk-MK",
	"083e","ms-BN",
	"043e","ms-MY",
	"044c","ml-IN",
	"043a","mt-MT",
	"0481","mi-NZ",
	"047a","arn-CL",
	"044e","mr-IN",
	"047c","moh-CA",
	"0450","mn-Cyrl-MN",
	"0850","mn-Mong-CN",
	"0461","ne-NP",
	"0414","nb-NO",
	"0814","nn-NO",
	"0482","oc-FR",
	"0448","or-IN",
	"0463","ps-AF",
	"0429","fa-IR",
	"0415","pl-PL",
	"0416","pt-BR",
	"0816","pt-PT",
	"0446","pa-IN",
	"046b","quz-BO",
	"086b","quz-EC",
	"0c6b","quz-PE",
	"0418","ro-RO",
	"0417","rm-CH",
	"0419","ru-RU",
	"243b","smn-FI",
	"103b","smj-NO",
	"143b","smj-SE",
	"0c3b","se-FI",
	"043b","se-NO",
	"083b","se-SE",
	"203b","sms-FI",
	"183b","sma-NO",
	"1c3b","sma-SE",
	"044f","sa-IN",
	"1c1a","sr-Cyrl-BA",
	"181a","sr-Latn-BA",
	"0c1a","sr-Cyrl-CS",
	"081a","sr-Latn-CS",
	"046c","ns-ZA",
	"0432","tn-ZA",
	"045b","si-LK",
	"041b","sk-SK",
	"0424","sl-SI",
	"2c0a","es-AR",
	"400a","es-BO",
	"340a","es-CL",
	"240a","es-CO",
	"140a","es-CR",
	"1c0a","es-DO",
	"300a","es-EC",
	"440a","es-SV",
	"100a","es-GT",
	"480a","es-HN",
	"080a","es-MX",
	"4c0a","es-NI",
	"180a","es-PA",
	"3c0a","es-PY",
	"280a","es-PE",
	"500a","es-PR",
	"0c0a","es-ES",
	"040a","es-ES_tradnl",
	"540a","es-US",
	"380a","es-UY",
	"200a","es-VE",
	"0441","sw-KE",
	"081d","sv-FI",
	"041d","sv-SE",
	"045a","syr-SY",
	"0428","tg-Cyrl-TJ",
	"085f","tzm-Latn-DZ",
	"0449","ta-IN",
	"0444","tt-RU",
	"044a","te-IN",
	"041e","th-TH",
	"0851","bo-BT",
	"0451","bo-CN",
	"041f","tr-TR",
	"0442","tk-TM",
	"0480","ug-CN",
	"0422","uk-UA",
	"042e","wen-DE",
	"0820","tr-IN",
	"0420","ur-PK",
	"0843","uz-Cyrl-UZ",
	"0443","uz-Latn-UZ",
	"042a","vi-VN",
	"0452","cy-GB",
	"0488","wo-SN",
	"0434","xh-ZA",
	"0485","sah-RU",
	"0478","ii-CN",
	"046a","yo-NG",
	"0435","zu-ZA",
	NULL,NULL
};

#ifndef LOCALE_SNAME
#define LOCALE_SNAME 0x5c
#endif



YsLocale::YsLocale()
{
	// Temporarily making it all English.
	// Default to en-US
	localeStr.Set("en_US");
	MakeLanguagePartFromLocaleString();
}


