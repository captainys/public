/* ////////////////////////////////////////////////////////////

File Name: ysshellext_constedgeslider.cpp
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

#include "ysshellext_constedgeslider.h"

YsShellExt_ConstEdgeSlider::State::State()
{
	edge[0]=nullptr;
	edge[1]=nullptr;
	onVertex=-1;
}



YSRESULT YsShellExt_ConstEdgeSlider::SlideTo(State &sta,const YsShellExt &shl,const YsVec3 &goal,const Condition &) const
{
	auto currentEdge=sta.edge;
	if(sta.edge[0]==sta.edge[1])
	{
		auto connVtHd=shl.GetConstEdgeConnectedVertex(sta.edge[0]);
		if(1==connVtHd.GetN() || 2==connVtHd.GetN())
		{
			currentEdge[1]=connVtHd[0];
		}
		else
		{
			return YSERR;
		}
	}

	auto lastEdge=sta.edge;
	for(;;)
	{
		decltype(sta.edge) nextEdge;
		nextEdge[0]=nullptr;
		nextEdge[1]=nullptr;

		const YsVec3 edVtPos[2]=
		{
			shl.GetVertexPosition(currentEdge[0]),
			shl.GetVertexPosition(currentEdge[1]),
		};

		auto nearPos=YsGetNearestPointOnLine3(edVtPos,goal);
		if(YSTRUE==YsCheckInBetween3(nearPos,edVtPos))
		{
			sta.edge=currentEdge;
			sta.pos=nearPos;
			sta.onVertex=-1;
			return YSOK;
		}
		else
		{
			int across,opp;
			if(0.0<(edVtPos[1]-edVtPos[0])*(nearPos-edVtPos[0]))
			{
				// Across currentEdge[1];
				across=1;
				opp=0;
			}
			else
			{
				// Across currentEdge[0];
				across=0;
				opp=1;
			}

			//   {connVtHd[0]}-----{currentEdge[across]}-----{connVtHd[1]}

			//       {oppVtHd}-----{currentEdge[across]}
			//                              OR
			//                     {currentEdge[across]}-----{oppVtHd}

			auto connVtHd=shl.GetConstEdgeConnectedVertex(currentEdge[across]);
			auto oppVtHd=currentEdge[opp];
			if(2==connVtHd.GetN())
			{
				nextEdge[0]=currentEdge[across];
				nextEdge[1]=YsPickOneOfAandBThatIsNotC(connVtHd[0],connVtHd[1],oppVtHd);

				if(YSTRUE==YsSameEdge<YsShell::VertexHandle>(lastEdge,nextEdge))  // Moving back & forth
				{
					sta.edge=currentEdge;
					sta.pos=shl.GetVertexPosition(currentEdge[across]);
					sta.onVertex=across;
					return YSOK;
				}
			}
			else  // Cannot go any farther.
			{
				sta.edge=currentEdge;
				sta.pos=shl.GetVertexPosition(currentEdge[across]);
				sta.onVertex=across;
				return YSOK;
			}
		}
		lastEdge=currentEdge;
		currentEdge=nextEdge;
	}
}

