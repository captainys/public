/* ////////////////////////////////////////////////////////////

File Name: ysunitconv.h
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

#ifndef YSUNITCONV_IS_INCLUDED
#define YSUNITCONV_IS_INCLUDED
/* { */

class YsUnitConv
{
public:
	static inline double MtoFT(const double M);
	static inline double MtoIN(const double M);
	static inline double MtoYD(const double M);
	static inline double MtoSM(const double M);
	static inline double MtoNM(const double M);

	static inline double FTtoM(const double FT);
	static inline double FTtoIN(const double FT);
	static inline double FTtoYD(const double FT);

	static inline double INtoM(const double INCH);
	static inline double INtoMM(const double INCH);
	static inline double INtoFT(const double INCH);
	static inline double INtoYD(const double INCH);

	static inline double YDtoM(const double YD);
	static inline double YDtoFT(const double YD);
	static inline double YDtoIN(const double YD);

	static inline double MMtoIN(const double MM);


	static inline double KMtoSM(const double KM);
	static inline double KMtoNM(const double KM);
	static inline double KMtoM(const double KM);

	static inline double SMtoKM(const double SM);
	static inline double SMtoM(const double SM);
	static inline double SMtoNM(const double SM);

	static inline double NMtoKM(const double NM);
	static inline double NMtoM(const double NM);
	static inline double NMtoSM(const double NM);


	static inline double LBtoKG(const double LB);
	static inline double KGtoLB(const double KG);

	static inline double KTtoMPH(const double KT);
	static inline double KTtoKMPH(const double KT);
	static inline double KTtoMPS(const double KT);

	static inline double MPStoKT(const double MPS);
	static inline double MPStoKMPH(const double MPS);
	static inline double MPStoMPH(const double MPS);

	static inline double MPHtoKT(const double MPH);
	static inline double MPHtoKMPH(const double MPH);
	static inline double MPHtoMPS(const double MPH);

	static inline double KMPHtoKT(const double KMPH);
	static inline double KMPHtoMPH(const double KMPH);
	static inline double KMPHtoMPS(const double KMPH);
};

const double YsFTperM=3.2808399;
const double YsINperM=39.3700787;
const double YsYDperM=1.0936133;
const double YsSMperM=1.0/1609.344;
const double YsNMperM=1.0/1852.0;

inline double YsUnitConv::MtoFT(const double M)
{
	return M*YsFTperM;
}

inline double YsUnitConv::MtoIN(const double M)
{
	return M*YsINperM;
}

inline double YsUnitConv::MtoYD(const double M)
{
	return M*YsYDperM;
}

inline double YsUnitConv::MtoSM(const double M)
{
	return M*YsSMperM;
}

inline double YsUnitConv::MtoNM(const double M)
{
	return M*YsNMperM;
}

const double YsMperFT=1.0/YsFTperM;
const double YsINperFT=12.0;
const double YsYDperFT=1.0/3.0;

inline double YsUnitConv::FTtoM(const double FT)
{
	return FT*YsMperFT;
}

inline double YsUnitConv::FTtoIN(const double FT)
{
	return FT*YsINperFT;
}

inline double YsUnitConv::FTtoYD(const double FT)
{
	return FT*YsYDperFT;
}

const double YsMperIN=1.0/YsINperM;
const double YsMMperIN=1000.0/YsINperM;
const double YsFTperIN=1.0/12.0;
const double YsYDperIN=1.0/36.0;

// Don't use a variable called IN.  Windows.h defines a macro called IN.
inline double YsUnitConv::INtoM(const double INCH)
{
	return INCH*YsMperIN;
}

inline double YsUnitConv::INtoMM(const double INCH)
{
	return INCH*YsMMperIN;
}

inline double YsUnitConv::INtoFT(const double INCH)
{
	return INCH*YsFTperIN;
}

inline double YsUnitConv::INtoYD(const double INCH)
{
	return INCH*YsYDperIN;
}

const double YsMperYD=1.0/YsYDperM;
const double YsFTperYD=3.0;
const double YsINperYD=36.0;

inline double YsUnitConv::YDtoM(const double YD)
{
	return YD*YsMperYD;
}

inline double YsUnitConv::YDtoFT(const double YD)
{
	return YD*YsFTperYD;
}

inline double YsUnitConv::YDtoIN(const double YD)
{
	return YD*YsINperYD;
}

const double YsINperMM=YsINperM/1000.0;

inline double YsUnitConv::MMtoIN(const double MM)
{
	return MM*YsINperMM;
}


const double YsSMperKM=1.0/1.609344;
const double YsNMperKM=1.0/1.85200;
const double YsMperKM=1000.0;

inline double YsUnitConv::KMtoSM(const double KM)
{
	return KM*YsSMperKM;
}

inline double YsUnitConv::KMtoNM(const double KM)
{
	return KM*YsNMperKM;
}

inline double YsUnitConv::KMtoM(const double KM)
{
	return KM*YsMperKM;
}

const double YsKMperSM=1.609344;
const double YsMperSM=1609.344;
const double YsNMperSM=0.868976242;

inline double YsUnitConv::SMtoKM(const double SM)
{
	return SM*YsKMperSM;
}

inline double YsUnitConv::SMtoM(const double SM)
{
	return SM*YsMperSM;
}

inline double YsUnitConv::SMtoNM(const double SM)
{
	return SM*YsNMperSM;
}


const double YsKMperNM=1.85200;
const double YsMperNM=1852.00;
const double YsSMperNM=1.15077945;

inline double YsUnitConv::NMtoKM(const double NM)
{
	return NM*YsKMperNM;
}

inline double YsUnitConv::NMtoM(const double NM)
{
	return NM*YsMperNM;
}

inline double YsUnitConv::NMtoSM(const double NM)
{
	return NM*YsSMperNM;
}



const double YsKGperLB=0.45359237;
const double YsLBperKG=1.0/YsKGperLB;

inline double YsUnitConv::LBtoKG(const double LB)
{
	return LB*YsKGperLB;
}

inline double YsUnitConv::KGtoLB(const double KG)
{
	return KG*YsLBperKG;
}


const double YsMPSperKT=0.51444444;

inline double YsUnitConv::KTtoMPH(const double KT)
{
	return KT*YsSMperNM;
}

inline double YsUnitConv::KTtoKMPH(const double KT)
{
	return KT*YsKMperNM;
}

inline double YsUnitConv::KTtoMPS(const double KT)
{
	return KT*YsMPSperKT;
}

inline double YsUnitConv::MPStoKT(const double MPS)
{
	return MPS/YsMPSperKT;
}

const double YsKMPHperMPS=3.6;

inline double YsUnitConv::MPStoKMPH(const double MPS)
{
	return MPS*YsKMPHperMPS;
}

const double YsMPHperMPS=2.236936;

inline double YsUnitConv::MPStoMPH(const double MPS)
{
	return MPS*YsMPHperMPS;
}

const double YsMPSperMPH=0.44704;

inline double YsUnitConv::MPHtoKT(const double MPH)
{
	return MPH*YsNMperSM;
}

inline double YsUnitConv::MPHtoKMPH(const double MPH)
{
	return MPH*YsKMperSM;
}

inline double YsUnitConv::MPHtoMPS(const double MPH)
{
	return MPH*YsMPSperMPH;
}


inline double YsUnitConv::KMPHtoKT(const double KMPH)
{
	return KMPH*YsNMperKM;
}

inline double YsUnitConv::KMPHtoMPH(const double KMPH)
{
	return KMPH*YsSMperKM;
}

inline double YsUnitConv::KMPHtoMPS(const double KMPH)
{
	return KMPH*1000.0/3600.0;
}

/* } */
#endif
