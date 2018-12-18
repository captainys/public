/* ////////////////////////////////////////////////////////////

File Name: ysoctree.h
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

#ifndef YSOCTREE_IS_INCLUDED
#define YSOCTREE_IS_INCLUDED
/* { */

/*
  Octree order
    int order;
    order=0;
    for(z=0; z<2; z++)
    {
        for(y=0; y<2; y++)
        {
            for(x=0; x<2; x++)
            {
                order++;
            }
        }
    }
*/



template <class T>
class YsOctreeNode
{
public:
	YsOctreeNode();
	// ~YsOctreeNode();  must be virtual, but to save memory space, the nodes are de-allocated in YsOctree class

	T dat;

	YSRESULT SubdivideByPolygon
	    (const YsVec3 &min,const YsVec3 &max,
	     class YsCollisionOfPolygon &coll,int maxDepth,const T &intersection,const T & nonIntersection);
	YSRESULT Subdivide(void);

	static void GetSubRegionRange
	    (YsVec3 &subMin,YsVec3 &subMax,const YsVec3 &min,const YsVec3 &max,int x,int y,int z);

	YsOctreeNode <T> *Seek(const YsVec3 &pos,const YsVec3 &min,const YsVec3 &max) const;
	YsOctreeNode <T> *Seek(YsVec3 &nodeMin,YsVec3 &nodeMax,const YsVec3 &pos,const YsVec3 &min,const YsVec3 &max) const;

	void GetBottomMostNodeList(YsArray <YsOctreeNode <T> *> &list) const;

	YsOctreeNode *GetChild(int i,int j,int k);
	YsOctreeNode *GetParent(void);
	const YsOctreeNode *GetChild(int i,int j,int k) const;
	const YsOctreeNode *GetParent(void) const;

	YSBOOL IsBottomMostNode(void) const;

	void DeallocateNode(void);

	YSRESULT GetIndexNumberOf(int &x,int &y,int &z,YsOctreeNode <T> *node);

protected:

	YSBOOL IntersectWith(class YsCollisionOfPolygon &coll,const YsVec3 &min,const YsVec3 &max) const;
	YSBOOL ChildIsAlreadyAllocated(void) const;

	YsOctreeNode *child,*parent;

public:
	void SetParent(YsOctreeNode *par);
};

template <class T>
YsOctreeNode <T>::YsOctreeNode()
{
	child=NULL;
	parent=NULL;
}

template <class T>
YSRESULT YsOctreeNode <T>::SubdivideByPolygon
    (const YsVec3 &min,const YsVec3 &max,
     YsCollisionOfPolygon &coll,int maxDepth,const T &intersection,const T & nonIntersection)
{
	int i,j,k;
	if(IntersectWith(coll,min,max)==YSTRUE)
	{
		if(maxDepth<=0)
		{
			// This is already a leaf node
			dat=intersection;
			return YSOK;
		}
		else
		{
			if(ChildIsAlreadyAllocated()!=YSTRUE)
			{
				child=new YsOctreeNode <T> [8];
				if(child!=NULL)
				{
					for(i=0; i<8; i++)
					{
						child[i].dat=nonIntersection; // By default, pretend no intersection;
						child[i].SetParent(this);
					}
				}
				else
				{
					YsErrOut("YsOctreeNode::SubdivideByPolygon()\n");
					YsErrOut("  Low memory warning!\n");
					return YSERR;
				}
			}

			for(k=0; k<2; k++)
			{
				for(j=0; j<2; j++)
				{
					for(i=0; i<2; i++)
					{
						YsVec3 subMin,subMax;
						GetSubRegionRange(subMin,subMax,min,max,i,j,k);
						if(GetChild(i,j,k)->SubdivideByPolygon
						     (subMin,subMax,coll,maxDepth-1,intersection,nonIntersection)!=YSOK)
						{
							return YSERR;
						}
					}
				}
			}
			return YSOK;
		}
	}
	else
	{
		// Do nothing;
		return YSOK;
	}
}

template <class T>
YSRESULT YsOctreeNode <T>::Subdivide(void)
{
	if(ChildIsAlreadyAllocated()!=YSTRUE)
	{
		child=new YsOctreeNode <T> [8];
		if(child!=NULL)
		{
			return YSOK;
		}
		else
		{
			return YSERR;
		}
	}
	return YSOK;
}

template <class T>
void YsOctreeNode <T>::GetSubRegionRange
	    (YsVec3 &subMin,YsVec3 &subMax,const YsVec3 &min,const YsVec3 &max,int x,int y,int z)
{
	YsVec3 u,v,w,d;
	d=(max-min)/2.0;
	u.Set(d.x(),0.0,0.0);
	v.Set(0.0,d.y(),0.0);
	w.Set(0.0,0.0,d.z());

	subMin=min+u*double(x  )+v*double(y  )+w*double(z  );
	subMax=min+u*double(x+1)+v*double(y+1)+w*double(z+1);
}

template <class T>
YsOctreeNode <T> *YsOctreeNode <T>::Seek(const YsVec3 &pos,const YsVec3 &min,const YsVec3 &max) const
{
	YsVec3 nodeMin,nodeMax;
	return Seek(nodeMin,nodeMax,pos,min,max);
}

template <class T>
YsOctreeNode <T> *YsOctreeNode <T>::Seek
    (YsVec3 &nodeMin,YsVec3 &nodeMax,const YsVec3 &pos,const YsVec3 &min,const YsVec3 &max) const
{
	if(YsCheckInsideBoundingBox3(pos,min,max)==YSTRUE)
	{
		if(IsBottomMostNode()==YSTRUE)
		{
			nodeMin=min;
			nodeMax=max;
			return (YsOctreeNode <T> *)this;
		}
		else
		{
			int i,j,k;
			YsVec3 subMin,subMax,cen;

			cen=(min+max)/2.0;
			i=(pos.x()<cen.x() ? 0 : 1);
			j=(pos.y()<cen.y() ? 0 : 1);
			k=(pos.z()<cen.z() ? 0 : 1);
			GetSubRegionRange(subMin,subMax,min,max,i,j,k);
			return GetChild(i,j,k)->Seek(nodeMin,nodeMax,pos,subMin,subMax);

//			for(k=0; k<2; k++)
//			{
//				for(j=0; j<2; j++)
//				{
//					for(i=0; i<2; i++)
//					{
//						YsOctreeNode <T> *subNode;
//						GetSubRegionRange(subMin,subMax,min,max,i,j,k);
//						if(YsCheckInsideBoundingBox3(pos,subMin,subMax)==YSTRUE &&
//						   (subNode=GetChild(i,j,k)->Seek(pos,subMin,subMax))!=NULL)
//						{
//							return subNode;
//						}
//					}
//				}
//			}
		}
		// Unreachable return NULL;
	}
	else
	{
		return NULL;
	}
}

template <class T>
void YsOctreeNode <T>::GetBottomMostNodeList(YsArray <YsOctreeNode <T> *> &list) const
{
	if(IsBottomMostNode()==YSTRUE)
	{
		list.AppendItem((YsOctreeNode <T> *)this);
	}
	else
	{
		int i,j,k;
		for(k=0; k<2; k++)
		{
			for(j=0; j<2; j++)
			{
				for(i=0; i<2; i++)
				{
					GetChild(i,j,k)->GetBottomMostNodeList(list);
				}
			}
		}
	}
}

template <class T>
YsOctreeNode <T> *YsOctreeNode <T>::GetChild(int i,int j,int k)
{
	return (YsOctreeNode <T> *)(((const YsOctreeNode <T> *)this)->GetChild(i,j,k));
}


template <class T>
YsOctreeNode <T> *YsOctreeNode <T>::GetParent(void)
{
	return (YsOctreeNode <T> *)(((const YsOctreeNode <T> *)this)->GetParent());
}

template <class T>
const YsOctreeNode <T> *YsOctreeNode <T>::GetChild(int x,int y,int z) const
{
	if(child!=NULL && 0<=x && x<2 && 0<=y && y<2 && 0<=z && z<2)
	{
		return &child[z*4+y*2+x];
	}
	else
	{
		return NULL;
	}
}

template <class T>
const YsOctreeNode <T> *YsOctreeNode <T>::GetParent(void) const
{
	return parent;
}

template <class T>
YSBOOL YsOctreeNode <T>::IsBottomMostNode(void) const
{
	if(child==NULL)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

template <class T>
void YsOctreeNode <T>::DeallocateNode(void)
{
	if(child!=NULL)
	{
		int i;
		for(i=0; i<8; i++)
		{
			child[i].DeallocateNode();
		}
		delete [] child;
		child=NULL;
	}
}

template <class T>
YSRESULT YsOctreeNode <T>::GetIndexNumberOf(int &x,int &y,int &z,YsOctreeNode <T> *node)
{
	int i;
	for(i=0; i<8; i++)
	{
		if(node==&child[i])
		{
			x=(i&1);
			y=(i&2)>>1;
			z=(i&4)>>2;
			return YSOK;
		}
	}
	return YSERR;
}

template <class T>
YSBOOL YsOctreeNode <T>::IntersectWith(YsCollisionOfPolygon &coll,const YsVec3 &min,const YsVec3 &max) const
{
	int i;
	YsVec3 p1,p2,cube[8],face[4];
	int cubeFaceIdx[6][4]=
	{
		{0,1,2,3},
		{4,5,6,7},
		{0,1,5,4},
		{1,2,6,5},
		{2,3,7,6},
		{3,0,4,7}
	};
	int np;
	const YsVec3 *p;

	np=coll.GetNumVertexOfPolygon1();
	p=coll.GetVertexListOfPolygon1();

	// One of the point is contained inside the region
	for(i=0; i<np; i++)
	{
		if(YsCheckInsideBoundingBox3(p[i],min,max)==YSTRUE)
		{
			return YSTRUE;
		}
	}

	// One of the face of the box intersect with the polygon (kind of corner cutting)
	cube[0].Set(min.x(),min.y(),min.z());
	cube[1].Set(max.x(),min.y(),min.z());
	cube[2].Set(max.x(),max.y(),min.z());
	cube[3].Set(min.x(),max.y(),min.z());
	cube[4].Set(min.x(),min.y(),max.z());
	cube[5].Set(max.x(),min.y(),max.z());
	cube[6].Set(max.x(),max.y(),max.z());
	cube[7].Set(min.x(),max.y(),max.z());

	for(i=0; i<6; i++)
	{
		face[0]=cube[cubeFaceIdx[i][0]];
		face[1]=cube[cubeFaceIdx[i][1]];
		face[2]=cube[cubeFaceIdx[i][2]];
		face[3]=cube[cubeFaceIdx[i][3]];
		coll.SetPolygon2(4,face);
		if(coll.CheckCollision()==YSTRUE)
		{
			return YSTRUE;
		}
	}

	return YSFALSE;
}

template <class T>
YSBOOL YsOctreeNode <T>::ChildIsAlreadyAllocated(void) const
{
	if(child!=NULL)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

template <class T>
void YsOctreeNode <T>::SetParent(YsOctreeNode <T> *par)
{
	parent=par;
}


////////////////////////////////////////////////////////////
template <class T>
class YsOctree
{
public:
	YsOctree();
	virtual ~YsOctree();

	YSRESULT SetDomain(const YsVec3 &min,const YsVec3 &max,const T &def);

	YSRESULT SubdivideByPolygon(int np,const YsVec3 p[],int maxDepth,const T &intersection,const T & nonIntersection);
	// YSRESULT SubdivideByPointAndSize(const YsVec3 &pos,double xsize,double ysize,double zsize);
	YSRESULT Subdivide(YsOctreeNode <T> *node);

	YSRESULT GetBottomMostNodeList(YsArray <YsOctreeNode <T> *> &list) const;

	YsOctreeNode <T> *GetRootNode(void);
	const YsOctreeNode <T> *GetRootNode(void) const;
	YsOctreeNode <T> *GetNode(const YsVec3 &pos);
	const YsOctreeNode <T> *GetNode(const YsVec3 &pos) const;

	YsOctreeNode <T> *GetNode(YsVec3 &nodeMin,YsVec3 &nodeMax,const YsVec3 &pos);
	const YsOctreeNode <T> *GetNode(YsVec3 &nodeMin,YsVec3 &nodeMax,const YsVec3 &pos) const;

	YSRESULT GoTraverse(YSBOOL onlyBottomMost,int param1,int param2);

	YSRESULT CleanUp(void);

protected:
	YsVec3 min,max;
	YsOctreeNode <T> *top;

	YSRESULT CallTraverse
	    (YSBOOL onlyBottomMost,YsOctreeNode <T> *node,const YsVec3 &min,const YsVec3 &max,int param1,int param2);
	virtual YSRESULT TraverseFunc(YsOctreeNode <T> *node,const YsVec3 &min,const YsVec3 &max,int param1,int param2);

	void DeallocateNode(void);
};

template <class T>
YsOctree <T>::YsOctree()
{
	top=NULL;
	min.Set(0.0,0.0,0.0);
	max.Set(0.0,0.0,0.0);
}

template <class T>
YsOctree <T>::~YsOctree()
{
	DeallocateNode();
}

template <class T>
YSRESULT YsOctree <T>::SetDomain(const YsVec3 &mn,const YsVec3 &mx,const T &def)
{
	min=mn;
	max=mx;
	top=new YsOctreeNode <T>;
	if(top!=NULL)
	{
		top->dat=def;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <class T>
YSRESULT YsOctree <T>::SubdivideByPolygon
    (int np,const YsVec3 p[],int maxDepth,const T &intersection,const T & nonIntersection)
{
	if(top!=NULL)
	{
		YsCollisionOfPolygon coll;
		coll.SetPolygon1(np,p);
		return top->SubdivideByPolygon(min,max,coll,maxDepth,intersection,nonIntersection);
	}
	else
	{
		YsErrOut("YsOctree::SubdivideByPolygon()\n");
		YsErrOut("  Domain is not set.\n");
		return YSERR;
	}
}

template <class T>
YSRESULT YsOctree <T>::Subdivide(YsOctreeNode <T> *node)
{
	return node->Subdivide();
}

template <class T>
YSRESULT YsOctree <T>::GetBottomMostNodeList(YsArray <YsOctreeNode <T> *> &list) const
{
	list.Set(0,NULL);
	if(top!=NULL)
	{
		top->GetBottomMostNodeList(list);
		return YSOK;
	}
	else
	{
		YsErrOut("YsOctree::GetBottomMostNodeList()\n");
		YsErrOut("  Domain is not set.\n");
		return YSERR;
	}
}

template <class T>
YsOctreeNode <T> *YsOctree <T>::GetRootNode(void)
{
	return top;
}

template <class T>
const YsOctreeNode <T> *YsOctree <T>::GetRootNode(void) const
{
	return top;
}

template <class T>
YsOctreeNode <T> *YsOctree <T>::GetNode(const YsVec3 &pos)
{
	const YsOctreeNode <T> *target;
	target=((const YsOctree <T> *)this)->GetNode(pos);
	return (YsOctreeNode <T> *)target;
}

template <class T>
YSRESULT YsOctree <T>::GoTraverse(YSBOOL onlyBottomMost,int param1,int param2)
{
	return CallTraverse(onlyBottomMost,top,min,max,param1,param2);
}

template <class T>
YSRESULT YsOctree <T>::CleanUp(void)
{
	DeallocateNode();
	min.Set(0.0,0.0,0.0);
	max.Set(0.0,0.0,0.0);
	return YSOK;
}

template <class T>
const YsOctreeNode <T> *YsOctree <T>::GetNode(const YsVec3 &pos) const
{
	return top->Seek(pos,min,max);
}

template <class T>
YsOctreeNode <T> *YsOctree <T>::GetNode(YsVec3 &nodeMin,YsVec3 &nodeMax,const YsVec3 &pos)
{
	return top->Seek(nodeMin,nodeMax,pos,min,max);
}

template <class T>
const YsOctreeNode <T> *YsOctree <T>::GetNode(YsVec3 &nodeMin,YsVec3 &nodeMax,const YsVec3 &pos) const
{
	return top->Seek(nodeMin,nodeMax,pos,min,max);
}

template <class T>
YSRESULT YsOctree <T>::CallTraverse
    (YSBOOL onlyBottomMost,YsOctreeNode <T> *node,const YsVec3 &min,const YsVec3 &max,int param1,int param2)
{
	int i,j,k;

	if(node==NULL)
	{
		return YSOK;
	}

	if(onlyBottomMost!=YSTRUE || node->IsBottomMostNode()==YSTRUE)
	{
		if(TraverseFunc(node,min,max,param1,param2)!=YSOK)
		{
			return YSERR;
		}
	}

	for(k=0; k<2; k++)
	{
		for(j=0; j<2; j++)
		{
			for(i=0; i<2; i++)
			{
				YsVec3 subMin,subMax;
				YsOctreeNode<T>::GetSubRegionRange(subMin,subMax,min,max,i,j,k);
				if(CallTraverse(onlyBottomMost,node->GetChild(i,j,k),subMin,subMax,param1,param2)!=YSOK)
				{
					return YSERR;
				}
			}
		}
	}

	return YSOK;
}

template <class T>
YSRESULT YsOctree <T>::TraverseFunc(YsOctreeNode <T> *,const YsVec3 &,const YsVec3 &,int,int)
{
	YsErrOut("YsOctree::TraverseFunc()\n");
	YsErrOut("  This function must be overrided.\n");
	return YSOK;
}

template <class T>
void YsOctree <T>::DeallocateNode()
{
	if(top!=NULL)
	{
		top->DeallocateNode();
		delete top;
		top=NULL;
	}
}

/* } */
#endif


