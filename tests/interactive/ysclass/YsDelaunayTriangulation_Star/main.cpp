/* ////////////////////////////////////////////////////////////

File Name: main.cpp
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

#include <fslazywindow.h>
#include <ysclass.h>
#include <ysdelaunaytri.h>
#include <ysgl.h>

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

	enum HIGHLIGHT_TYPE
	{
		NONE,
		ENCLOSING_TRI,
		CAVITY_TRI,
		TRI_USING_NODE,
		TRI_PIPE,
		CURRENT_PIPE
	};
	YsUnconstrainedDelaunayTriangulation delTri;
	int iniNode[4],iniTri[2][3];
	HIGHLIGHT_TYPE highlight;
	YSSIZE_T selectedVtIdx[2];
	YsArray <int> curPipe;

	enum STATE
	{
		STATE_INSERTION,
		STATE_BOUNDARY_RECOVERY,
		STATE_FINISHING,
		STATE_FINISHED
	};
	STATE state;
	YsShell::VertexHandle curVtHd;
	YsShell shl;
	YsShellSearchTable search;
	YsShellEdgeEnumHandle curEdHd;
	YsShellVertexAttribTable <int> vtHdToNodeIdx;

public:
	void PrintMenu(void)
	{
		printf("SPACE... Insert a vertex.\n");
	}




	FsLazyWindowApplication();
	virtual void BeforeEverything(int argc,char *argv[]);
	virtual void GetOpenWindowOption(FsOpenWindowOption &OPT) const;
	virtual void Initialize(int argc,char *argv[]);
	virtual void Interval(void);
	virtual void BeforeTerminate(void);
	virtual void Draw(void);
	virtual bool UserWantToCloseProgram(void);
	virtual bool MustTerminate(void) const;
	virtual long long int GetMinimumSleepPerInterval(void) const;
	virtual bool NeedRedraw(void) const;
};

FsLazyWindowApplication::FsLazyWindowApplication()
{
	needRedraw=false;
}

/* virtual */ void FsLazyWindowApplication::BeforeEverything(int argc,char *argv[])
{
}
/* virtual */ void FsLazyWindowApplication::GetOpenWindowOption(FsOpenWindowOption &opt) const
{
	opt.x0=0;
	opt.y0=0;
	opt.wid=1200;
	opt.hei=800;
}
/* virtual */ void FsLazyWindowApplication::Initialize(int argc,char *argv[])
{
	FsChangeToProgramDir();
	if(YSOK!=shl.LoadSrf("star_remesh.srf"))
	{
		printf("Cannot load star_remesh.srf!!!!\n");
	}
	vtHdToNodeIdx.SetShell(shl);
	shl.AttachSearchTable(&search);
	state=STATE_INSERTION;
	curVtHd=nullptr;
	curEdHd=nullptr;

	iniNode[0]=delTri.AddInitialNode(YsVec2( 10.0, 10.0));
	iniNode[1]=delTri.AddInitialNode(YsVec2(790.0, 10.0));
	iniNode[2]=delTri.AddInitialNode(YsVec2(790.0,500.0));
	iniNode[3]=delTri.AddInitialNode(YsVec2( 10.0,500.0));

	iniTri[0][0]=iniNode[0];
	iniTri[0][1]=iniNode[1];
	iniTri[0][2]=iniNode[2];
	iniTri[1][0]=iniNode[2];
	iniTri[1][1]=iniNode[3];
	iniTri[1][2]=iniNode[0];

	delTri.AddInitialTri(iniTri[0]);
	delTri.AddInitialTri(iniTri[1]);

	highlight=ENCLOSING_TRI;
	selectedVtIdx[0]=-1;
	selectedVtIdx[1]=-1;

	PrintMenu();

}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}

	FsPollDevice();
	int lb,mb,rb,mx,my;
	const int mosEvt=FsGetMouseEvent(lb,mb,rb,mx,my);


	switch(key)
	{
	case FSKEY_1:
		highlight=ENCLOSING_TRI;
		break;
	case FSKEY_2:
		highlight=CAVITY_TRI;
		break;
	case FSKEY_3:
		highlight=TRI_USING_NODE;
		break;
	case FSKEY_4:
		highlight=TRI_PIPE;
		break;
	case FSKEY_5:
		highlight=CURRENT_PIPE;
		break;
	case FSKEY_R:
		if(0<=selectedVtIdx[0] && 0<=selectedVtIdx[1])
		{
			curPipe=delTri.FindPipe(selectedVtIdx[0],selectedVtIdx[1]);
			if(1<curPipe.GetN())
			{
				while(YSOK==delTri.ReducePipe(curPipe,selectedVtIdx[0],selectedVtIdx[1]))
				{
					// curPipe=delTri.FindPipe(selectedVtIdx[0],selectedVtIdx[1]);
				}
			}
		}
		break;
	case FSKEY_SPACE:
		switch(state)
		{
		case STATE_INSERTION:
			curVtHd=shl.FindNextVertex(curVtHd);
			if(nullptr!=curVtHd)
			{
				auto vtPos=shl.GetVertexPosition(curVtHd);
				vtPos*=20.0;
				vtPos.MulZ(-1.0);
				vtPos+=YsVec3(400.0,0.0,250.0);
				int ndIdx=delTri.InsertNode(vtPos.xz());
				vtHdToNodeIdx.SetAttrib(curVtHd,ndIdx);
			}
			else
			{
				search.FindNextEdge(shl,curEdHd);
				state=STATE_BOUNDARY_RECOVERY;
			}
			break;
		case STATE_BOUNDARY_RECOVERY:
			while(nullptr!=curEdHd && 1!=search.GetNumPolygonUsingEdge(shl,curEdHd))
			{
				search.FindNextEdge(shl,curEdHd);
			}
			if(nullptr!=curEdHd)
			{
				auto edge=search.GetEdge(shl,curEdHd);
				int *ndIdxPtr[2];
				ndIdxPtr[0]=vtHdToNodeIdx[edge[0]];
				ndIdxPtr[1]=vtHdToNodeIdx[edge[1]];
				delTri.RecoverBoundaryEdge(*ndIdxPtr[0],*ndIdxPtr[1]);
				search.FindNextEdge(shl,curEdHd);
				printf("Recover edge.\n");
			}
			else
			{
				state=STATE_FINISHING;
			}
			break;
		case STATE_FINISHING:
			{
				YsShellEdgeEnumHandle edHd=nullptr;
				YsArray <YsEdgeTemplate <int,int> > boundaryEdge;
				while(YSOK==search.FindNextEdge(shl,edHd))
				{
					if(1==search.GetNumPolygonUsingEdge(shl,edHd))
					{
						auto edge=search.GetEdge(shl,edHd);
						YsEdgeTemplate <int,int> edgePiece;
						edgePiece.vt[0]=*vtHdToNodeIdx[edge[0]];
						edgePiece.vt[1]=*vtHdToNodeIdx[edge[1]];
						boundaryEdge.push_back(edgePiece);
					}
				}
				YsArray <const YsEdgeTemplate <int,int> *> loopEdge;
				YsArray <int> loopVtx;
				if(YSOK==YsAssembleLoopFromEdgeList<int,int>(loopEdge,loopVtx,boundaryEdge.size(),boundaryEdge))
				{
					YsArray <YsArray <int> > contour;
					contour.push_back(loopVtx);
					delTri.DeleteOutsideTri(contour);
				}
				else
				{
					printf("Failed to assemble a loop.\n");
				}
			}
			break;
		}
		break;
	}

	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	int lb,mb,rb,mx,my;
	FsGetMouseState(lb,mb,rb,mx,my);

	int wid,hei;
	FsGetWindowSize(wid,hei);

	glViewport(0,0,wid,hei);

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	switch(highlight)
	{
	case ENCLOSING_TRI:
		{
			const int onTriIdx=delTri.FindEnclosingTriangleIndex(YsVec2(mx,my));
			if(0<=onTriIdx)
			{
				YsVec2 tri[3];
				delTri.GetTriangleNodePos(tri,onTriIdx);
				glColor3ub(0,0,128);
				glBegin(GL_TRIANGLES);
				glVertex2dv(tri[0]);
				glVertex2dv(tri[1]);
				glVertex2dv(tri[2]);
				glEnd();
			}
		}
		break;
	case CAVITY_TRI:
		{
			auto cavity=delTri.BuildCavity(YsVec2(mx,my));
			if(0<cavity.GetN())
			{
				glColor3ub(0,0,128);
				glBegin(GL_TRIANGLES);
				for(auto triIdx : cavity)
				{
					YsVec2 tri[3];
					delTri.GetTriangleNodePos(tri,triIdx);
					glVertex2dv(tri[0]);
					glVertex2dv(tri[1]);
					glVertex2dv(tri[2]);
				}
				glEnd();
			}
		}
		break;
	case TRI_USING_NODE:
		{
			for(YSSIZE_T ndIdx=0; ndIdx<delTri.GetNumNode(); ++ndIdx)
			{
				if((delTri.GetNodePos(ndIdx)-YsVec2(mx,my)).GetLength()<10.0)
				{
					auto onTriIdx=delTri.FindTriangleUsingNode(ndIdx);
					if(0<=onTriIdx)
					{
						YsVec2 tri[3];
						delTri.GetTriangleNodePos(tri,onTriIdx);
						glColor3ub(0,0,128);
						glBegin(GL_TRIANGLES);
						glVertex2dv(tri[0]);
						glVertex2dv(tri[1]);
						glVertex2dv(tri[2]);
						glEnd();
					}
					break;
				}
			}
		}
		break;
	case TRI_PIPE:
		{
			if(0<=selectedVtIdx[0] && 0<=selectedVtIdx[1])
			{
				auto pipe=delTri.FindPipe(selectedVtIdx[0],selectedVtIdx[1]);
				for(auto onTriIdx : pipe)
				{
					YsVec2 tri[3];
					delTri.GetTriangleNodePos(tri,onTriIdx);
					glColor3ub(0,0,128);
					glBegin(GL_TRIANGLES);
					glVertex2dv(tri[0]);
					glVertex2dv(tri[1]);
					glVertex2dv(tri[2]);
					glEnd();
				}
				break;
			}
		}
		break;
	case CURRENT_PIPE:
		{
			auto pipe=curPipe;
			glColor3ub(200,80,80);
			glBegin(GL_TRIANGLES);
			for(auto onTriIdx : pipe)
			{
				YsVec2 tri[3];
				delTri.GetTriangleNodePos(tri,onTriIdx);
				glVertex2dv(tri[0]);
				glVertex2dv(tri[1]);
				glVertex2dv(tri[2]);
			}
			glEnd();
		}
		break;
	}



	glColor3ub(255,128,128);
	glBegin(GL_LINES);
	for(int idx=0; idx<delTri.GetNumTri(); ++idx)
	{
		if(YSTRUE==delTri.IsTriangleAlive(idx))
		{
			auto cen0=delTri.GetTriangleCenter(idx);
			for(int edIdx=0; edIdx<3; ++edIdx)
			{
				auto neiTriIdx=delTri.GetNeighborTriangleIndex(idx,edIdx);
				if(0<=neiTriIdx)
				{
					auto cen1=delTri.GetTriangleCenter(neiTriIdx);
					glVertex2dv(cen0);
					glVertex2dv(cen1);
				}
			}
		}
	}
	glEnd();


	glColor3ub(0,0,0);
	glBegin(GL_LINES);
	for(int idx=0; idx<delTri.GetNumTri(); ++idx)
	{
		if(YSTRUE==delTri.IsTriangleAlive(idx))
		{
			YsVec2 tri[3];
			delTri.GetTriangleNodePos(tri,idx);

			const YsVec2 cen=(tri[0]+tri[1]+tri[2])/3.0;

			glVertex2dv(tri[0]);
			glVertex2dv(tri[1]);
			glVertex2dv(tri[1]);
			glVertex2dv(tri[2]);
			glVertex2dv(tri[2]);
			glVertex2dv(tri[0]);

			tri[0]=cen*0.1+tri[0]*0.9;
			tri[1]=cen*0.1+tri[1]*0.9;
			tri[2]=cen*0.1+tri[2]*0.9;

			glVertex2dv(tri[0]);
			glVertex2dv(tri[1]);
			glVertex2dv(tri[1]);
			glVertex2dv(tri[2]);
			glVertex2dv(tri[2]);
			glVertex2dv(tri[0]);
		}
	}
	glEnd();

	glPointSize(10);
	glColor3ub(0,0,255);
	glBegin(GL_POINTS);
	for(auto sel : selectedVtIdx)
	{
		if(0<=sel)
		{
			glVertex2dv(delTri.GetNodePos(sel));
		}
		glColor3ub(255,0,0);
	}
	glEnd();


	FsSwapBuffers();

	needRedraw=false;
}
/* virtual */ bool FsLazyWindowApplication::UserWantToCloseProgram(void)
{
	return true; // Returning true will just close the program.
}
/* virtual */ bool FsLazyWindowApplication::MustTerminate(void) const
{
	return FsLazyWindowApplicationBase::MustTerminate();
}
/* virtual */ long long int FsLazyWindowApplication::GetMinimumSleepPerInterval(void) const
{
	return 10;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
}
/* virtual */ bool FsLazyWindowApplication::NeedRedraw(void) const
{
	return needRedraw;
}


static FsLazyWindowApplication *appPtr=nullptr;

/* static */ FsLazyWindowApplicationBase *FsLazyWindowApplicationBase::GetApplication(void)
{
	if(nullptr==appPtr)
	{
		appPtr=new FsLazyWindowApplication;
	}
	return appPtr;
}
