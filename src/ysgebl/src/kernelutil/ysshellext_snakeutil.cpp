#include "ysshellext_snakeutil.h"



double YsShellExt_SnakeUtil::Snake::CalculateEnergy(const YsShellExt &shl,const EnergyFunction &func) const
{
	if(vtHdChain.size()<=1)
	{
		fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
		fprintf(stderr,"Error: Snake of less-than-two vertices.\n");
		return 0;
	}

	double e=0.0;
	if(vtHdChain.front()==vtHdChain.back())
	{
		const YsShellExt::VertexHandle vtHd[3]=
		{
			vtHdChain[vtHdChain.size()-2],
			vtHdChain.front(),
			vtHdChain[1]
		};
		e+=func.CalculateEnergy(shl,vtHd);
	}

	for(int i=1; i<vtHdChain.size()-1; ++i)
	{
		e+=func.CalculateEnergy(shl,vtHdChain.data()+i-1);
	}

	return e;
}

YsShellExt_SnakeUtil::YsShellExt_SnakeUtil()
{
	vtxCond=nullptr;
}
YsShellExt_SnakeUtil::~YsShellExt_SnakeUtil()
{
	CleanUp();
}
void YsShellExt_SnakeUtil::CleanUp(void)
{
	snake.CleanUp();
	vtxCond=nullptr;
}

YSRESULT YsShellExt_SnakeUtil::BeginSnake(const YsShellExt &shl,const YsConstArrayMask <YsShellExt::VertexHandle> snakeVtHd)
{
	if(2<=snakeVtHd.size())
	{
		snake.vtHdChain=snakeVtHd;
		return YSOK;
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Error: Snake of less-than-two vertices.\n");
	return YSERR;
}

YSRESULT YsShellExt_SnakeUtil::BeginSnake(const YsShellExt &shl,YSSIZE_T nVt,const YsShellExt::VertexHandle snakeVtHd[],YSBOOL isLoop)
{
	if(nVt<2)
	{
		fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
		fprintf(stderr,"Error: Snake of less-than-two vertices.\n");
		return YSERR;
	}

	if(YSTRUE!=isLoop || snakeVtHd[0]==snakeVtHd[nVt-1])
	{
		return BeginSnake(shl,YsConstArrayMask<YsShellExt::VertexHandle> (nVt,snakeVtHd));
	}
	else
	{
		YsArray <YsShellExt::VertexHandle,16> vtHd(nVt,snakeVtHd);
		vtHd.push_back(snakeVtHd[0]);
		return BeginSnake(shl,vtHd);
	}
}

YSRESULT YsShellExt_SnakeUtil::SetVertexCondition(const YsShellExt::Condition *vtxCond)
{
	this->vtxCond=vtxCond;
	return YSOK;
}
