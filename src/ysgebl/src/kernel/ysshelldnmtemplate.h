/* ////////////////////////////////////////////////////////////

File Name: ysshelldnmtemplate.h
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

#ifndef YSSHELLDNMTEMPLATE_IS_INCLUDED
#define YSSHELLDNMTEMPLATE_IS_INCLUDED
/* { */

////////////////////////////////////////////////////////////

#include "ysshellext.h"
#include "ysshellextio.h"

/*!
YsShellDnmTemplate takes two template parameters.  One is a shell class, which stores the geometry information of the shell.  It can be YsShellExt or YsShellExtEdit.

NODESTATE_EXTRA_INFO is an additional node-state information.  In most cases, each node (YsShellExt or YsShellExtEdit) can own a YsShellExtDrawingBuffer.  
However, in some cases, such as when it is a VASI or a PAPI, the appearance should vary depending on from where the object is looked.
For VASI and PAPI, it is possible to write a special shader program.  But, the problem of shader programs is it grows.  
I need to create so many different shader programs for different situations, which is not good.  Debugging a shader program is difficult enough.  
Maintaining too many shader programs is a nightmare.

The reasonable solution is to use a general purpose shader programs to render such special kind of objects.  
But, in that case, each instance, not SHLCLASS, needs to own a YsShellExtDrawingBuffer and maybe VBO for the drawing buffer.
Then, it needs to be a part of DnmState, and eacn NodeState must own one.  

To be flexible, what information needs to be stored per instance per node is made a template parameter.  If unnecessary, dummy type of integer is used.
*/
template <class SHLCLASS,class NODESTATE_EXTRA_INFO=int>
class YsShellDnmContainer
{
public:
	class NodeState
	{
	private:
		enum
		{
			FLAG_SHOW=1
		};
		unsigned int flags;
	public:
		YsVec3 relPos;
		YsAtt3 relAtt;
		YsMatrix4x4 tfmCache;

		NODESTATE_EXTRA_INFO extInfo;

	public:
		NodeState();
		void Initialize(void);
		void SetShow(YSBOOL showFlag);
		void SetHeading(const double heading);
		void SetPitch(const double pitch);
		void SetBank(const double bank);
		const YSBOOL GetShow(void) const;
	};

	class Node : public SHLCLASS
	{
	private:
		YSHASHKEY searchKey;  // Unique within a DNM object.
	public:
		using SHLCLASS::GetSearchKey;

		YsString nodeName;
		int dnmClassType;
		YsVec3 posInParent;
		YsAtt3 attInParent;
		YsVec3 localRotationCenter,localRotationAxis;

		Node *parent;
		YsArray <Node *> children;

		YsArray <NodeState> stateArray;

		YsArray <YsString> childNameTag; // Used during read/write DNM

		Node();
		void Initialize(void);
		YSHASHKEY GetSearchKey(void) const;

		void SetSearchKey(YSHASHKEY key); // Do not call except from Create function.
	};

	class DnmState
	{
	private:
		mutable YsArray <NodeState> stateArray;
		mutable YsHashTable <YSSIZE_T> nodeKeyToStateArrayIndex;
	public:
		DnmState();
		DnmState(DnmState &&incoming);
		DnmState(const DnmState &incoming);
		DnmState &operator=(const DnmState &incoming);
		DnmState &operator=(DnmState &&incoming);

		void CleanUp(void);

		NodeState &GetState(const Node *node);
		const NodeState &GetState(const Node *node) const;
		YSBOOL GetShow(const Node *node) const;
		void CacheTransformation(const Node *node) const;

private:
		/*! Returns an index to the node state.  If it doesn't exist, it creates one.
		*/
		inline YSSIZE_T GetStateIndex(const Node *ndoe) const;

public:
		/*! Returns a transformation from the node to its immediate parent node.
		*/
		const YsMatrix4x4 &GetTransformation(const Node *node) const;

		/*! Returns a transformation from the node to the root node.
		*/
		const YsMatrix4x4 GetNodeToRootTransformation(const Node *node) const;
	};

private:
	class PackedTextFile : public YsTextInputStream
	{
	private:
		YSSIZE_T filePointer;
		YsArray <YsString> dataFile;
	public:
		YsString fn;

		PackedTextFile()
		{
			Initialize();
		}
		void Initialize(void)
		{
			filePointer=0;
			dataFile.CleanUp();
		}
		void AddString(YsString &&incoming)
		{
			dataFile.Increment();
			dataFile.Last().MoveFrom((YsString &)incoming); // Why g++ requires this cast?  Isn't it obvious?
		}
		void AddString(const YsString &incoming)
		{
			dataFile.Increment();
			dataFile.Last()=incoming;
		}
		void BeginRead(void)
		{
			filePointer=0;
		}
		virtual const char *Gets(YsString &str)
		{
			if(filePointer<dataFile.GetN())
			{
				str=dataFile[filePointer];
				++filePointer;
				return str;
			}
			return NULL;
		}
	};
	class ReadDnmVariable
	{
	public:
		enum {
			READSTATE_ERROR=-1,
			READSTATE_NORMAL=0,
			READSTATE_VERTEXARRAY=1,
			READSTATE_PACKEDFILE=99,
			READSTATE_ENDOFFILE=999
		};

		Node *allParent;
		int readState; // -1:Error!!  0:Normal  1:Reading PLG  99:Loading Packed File  999:End of file
		int dnmVersion; // 0: Old dnm  1: Version 1
		int nPackedFileRemain,nVtxRemain;
		YsString readPath;
		int currentNodeType;
		Node *currentNode;
		PackedTextFile *currentPack;

		YsArray <Node *> newNodeArray;

		YsSegmentedArray <PackedTextFile,4> packedFileList;
	};

	static const char *const keyWordSource[];
	static class YsKeyWordList keyWordList;
	YSHASHKEY searchKeySeed;

protected:
	/*! Pointer to the root node. */
	YsArray <Node *> rootArray;

	/*! Hash table for searching Node pointer from the search key. */
	YsHashTable <Node *> nodeSearchTable;

public:
	/*! Default constructor. */
	YsShellDnmContainer();

	/*! Default destructor. */
	~YsShellDnmContainer();

	/*! Returns the bounding box. */
	void GetBoundingBox(YsVec3 bbx[2]) const;

	/*! Returns the bounding box. */
	void GetBoundingBox(YsVec3 &min,YsVec3 &max) const;

	/*! This function cleans up all contents of this object. */
	void CleanUp(void);
private:
	void DeleteShellRecursive(Node *ptr);

public:
	/*! This function creates a shell node. */
	Node *CreateShell(Node *parentPtr);

	/*! Finds a shell by a hash key. */
	Node *FindNode(YSHASHKEY key);

	/*! Finds a shell by a hash key. */
	const Node *FindNode(YSHASHKEY key) const;

	/*! This function deletes a shell node. */
	YSRESULT DeleteShell(Node *ptr);

	/*! This function returns the number of children of the node 'ptr'.  If ptr==NULL, it returns the number of root nodes. */
	int GetNumChildNode(Node *ptr) const;

	/*! This function returns the first root node pointer. */
	Node *GetFirstRootNode(void) const;

	/*! Returns the parent of the given ndoe. */
	Node *GetParent(const Node *node);

	/*! Returns the parent of the given ndoe. */
	const Node *GetParent(const Node *node) const;

	/*! This function returns an array of root node pointers. */
	template <const int N>
	void GetRootNodeArray(YsArray <Node *,N> &rootArray) const;

	/*! This function returns an array of root node pointers. */
	YsArray <Node *> GetRootNodeArray(void) const;

	/*! This function returns a constant pointer to all shell nodes in this DNM container. */
	template <const int N>
	void GetNodePointerAll(YsArray <const Node *,N> &allNode) const;

	/*! This function returns a non-constant pointer to all shell nodes in this DNM container. */
	template <const int N>
	void GetNodePointerAll(YsArray <Node *,N> &allNode);

	/*! This function returns a non-constant pointer to all shell nodes in this DNM container. */
	YsArray <Node *> GetNodePointerAll(void);

	/*! This function makes the tree structure flat (all nodes are root nodes.) */
	void MakeFlatTreeStructure(void);

	/*! Makes the node childPtr a child of parentPtr.  If nullptr==parentPtr, childPtr will become one of the root nodes. */
	void Reconnect(Node *childPtr,Node *parentPtr);

	/*! This function returns an empty DNM state. */
	DnmState EmptyDnmState(void) const;

	/*! This function clears the state. */
	void ClearState(DnmState &dnmState);

	/*! This function sets zero-transformation. */
	void SetZeroTransformation(DnmState &dnmState);
private:
	void SetZeroTransformationRecursive(DnmState &dnmState,Node *node);

public:
	/*! This function sets up DNM state to 'stateId'. */
	void SetState(DnmState &dnmState,int stateId);
private:
	void SetStateRecursive(DnmState &dnmState,Node *node,int stateId);

public:
	/*! This function sets up DNM state to 'stateId'. */
	void SetClassState(DnmState &dnmState,int dnmClassType,int stateId);
private:
	void SetClassStateRecursive(DnmState &dnmState,int dnmClassType,Node *node,int stateId);

public:
	/*! This function sets up DNM state of a specific dnm-ID node by interpolating stateIds. */
	void SetClassState(DnmState &dnmState,int dnmClassType,int stateId0,int stateId1,const double param) const;
private:
	void SetClassStateRecursive(DnmState &dnmState,int dnmClassType,Node *node,int stateId0,int stateId1,const double param) const;

public:
	void SetClassShow(DnmState &dnmState,int dnmClassType,YSBOOL show);
private:
	void SetClassShowRecursive(DnmState &dnmState,int dnmClassType,Node *node,YSBOOL show);

public:
	void SetClassHeading(DnmState &dnmState,int dnmClassType,const double heading);
private:
	void SetClassHeadingRecursive(DnmState &dnmState,int dnmClassType,Node *node,const double heading);

public:
	void SetClassPitch(DnmState &dnmState,int dnmClassType,const double pitch);
private:
	void SetClassPitchRecursive(DnmState &dnmState,int dnmClassType,Node *node,const double pitch);

public:
	void SetNodeAngle(DnmState &dnmState,Node *node,const double heading,const double pitch);

public:
	void SetClassRotation(DnmState &dnmState,int dnmClassType,const double rotAngle);
private:
	void SetClassRotationRecursive(DnmState &dnmState,int dnmClassType,Node *node,const double rotAngle);

public:
	/*! This function caches the transformation in each node. */
	void CacheTransformation(DnmState &dnmState) const;
private:
	void CacheTransformationRecursive(DnmState &dnmState,const Node *node) const;

public:
	/*! This function loads a .DNM file. */
	YSRESULT LoadDnm(YsTextInputStream &inStream,Node *allParent,const char baseFileName[]=NULL);
private:
	YSRESULT BeginReadDnm(ReadDnmVariable &var,Node *allParent,const char fn[]);
	YSRESULT ReadDnmOneLine(ReadDnmVariable &var,const YsString &str);
	YSRESULT EndReadDnm(ReadDnmVariable &var);
	void DeleteChildNodeNameTag(void);
	YSRESULT MakeConnectionAndRootNodeListByChildNameTag(ReadDnmVariable &var);

public:
	/*! This function writes a .DNM file. */
	YSRESULT SaveDnm(YsTextOutputStream &outStream) const;
private:
	void CreateUniqueDnmName(YsHashTable <YsString> &dnmKeyToDnmName) const;
	void SaveNodeToMemory(YsTextMemoryOutputStream &outStream,const Node &node) const;
	void WritePackage(YsTextOutputStream &outStream,const YsHashTable <YsString> &nodeKeyToDnmName) const;
};

////////////////////////////////////////////////////////////

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::NodeState::NodeState()
{
	Initialize();
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::NodeState::Initialize(void)
{
	flags=FLAG_SHOW;
	relPos=YsOrigin();
	relAtt=YsZeroAtt();
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::NodeState::SetShow(YSBOOL showFlag)
{
	if(YSTRUE==showFlag)
	{
		flags|=(unsigned int)FLAG_SHOW;
	}
	else
	{
		flags&=(~(unsigned int)FLAG_SHOW);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::NodeState::SetHeading(const double heading)
{
	relAtt.SetH(heading);
}
template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::NodeState::SetPitch(const double pitch)
{
	relAtt.SetP(pitch);
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::NodeState::SetBank(const double bank)
{
	relAtt.SetB(bank);
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
const YSBOOL YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::NodeState::GetShow(void) const
{
	if(0!=(flags&FLAG_SHOW))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

////////////////////////////////////////////////////////////

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node::Node()
{
	Initialize();
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node::Initialize(void)
{
	nodeName="";
	dnmClassType=0;

	posInParent=YsOrigin();
	attInParent=YsZeroAtt();
	localRotationCenter=YsOrigin();
	localRotationAxis=YsXVec();

	parent=NULL;
	children.CleanUp();

	stateArray.CleanUp();
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YSHASHKEY YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node::GetSearchKey(void) const
{
	return searchKey;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node::SetSearchKey(YSHASHKEY key)
{
	searchKey=key;
}

////////////////////////////////////////////////////////////

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::DnmState()
{
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::DnmState(const DnmState &incoming)
{
	stateArray.CopyFrom(incoming.stateArray);
	nodeKeyToStateArrayIndex=incoming.nodeKeyToStateArrayIndex;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::DnmState(DnmState &&incoming)
{
	stateArray.MoveFrom(incoming.stateArray);
	nodeKeyToStateArrayIndex.MoveFrom(incoming.nodeKeyToStateArrayIndex);
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState &YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::operator=(const DnmState &incoming)
{
	stateArray.CopyFrom(incoming.stateArray);
	nodeKeyToStateArrayIndex=incoming.nodeKeyToStateArrayIndex;
	return *this;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState &YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::operator=(DnmState &&incoming)
{
	stateArray.MoveFrom(incoming.stateArray);
	nodeKeyToStateArrayIndex.MoveFrom(incoming.nodeKeyToStateArrayIndex);
	return *this;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::CleanUp(void)
{
	stateArray.CleanUp();
	nodeKeyToStateArrayIndex.CleanUp();
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::NodeState &YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::GetState(const Node *node)
{
	return stateArray[GetStateIndex(node)];
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
const typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::NodeState &YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::GetState(const Node *node) const
{
	return stateArray[GetStateIndex(node)];
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
inline YSSIZE_T YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::GetStateIndex(const Node *node) const
{
	YSSIZE_T idx;
	if(YSOK!=nodeKeyToStateArrayIndex.FindElement(idx,node->GetSearchKey()))
	{
		idx=stateArray.GetN();
		stateArray.Increment();
		stateArray.Last().Initialize();
		nodeKeyToStateArrayIndex.AddElement(node->GetSearchKey(),idx);
		return idx;
	}
	return idx;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YSBOOL YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::GetShow(const Node *node) const
{
	YSSIZE_T idx;
	if(YSOK==nodeKeyToStateArrayIndex.FindElement(idx,node->GetSearchKey()))
	{
		return stateArray[idx].GetShow();
	}
	return YSTRUE; // 2016/02/28 Default value is TRUE.  Was returning FALSE.
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::CacheTransformation(const Node *node) const
{
	auto &state=stateArray[GetStateIndex(node)];

	state.tfmCache.LoadIdentity();
	state.tfmCache.Translate(node->posInParent);

	state.tfmCache.RotateXZ(node->attInParent.h());
	state.tfmCache.RotateZY(node->attInParent.p());
	state.tfmCache.RotateXY(node->attInParent.b());

	state.tfmCache.Translate(state.relPos);

	state.tfmCache.RotateXZ(state.relAtt.h());
	state.tfmCache.RotateZY(state.relAtt.p());
	state.tfmCache.RotateXY(state.relAtt.b());

	state.tfmCache.Translate(-node->localRotationCenter);
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
const YsMatrix4x4 &YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::GetTransformation(const Node *node) const
{
	YSSIZE_T idx;
	if(YSOK==nodeKeyToStateArrayIndex.FindElement(idx,node->GetSearchKey()))
	{
		return stateArray[idx].tfmCache;
	}
	return YsIdentity4x4();
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
const YsMatrix4x4 YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState::GetNodeToRootTransformation(const Node *node) const
{
	YsMatrix4x4 tfm;
	while(nullptr!=node)
	{
		tfm=GetTransformation(node)*tfm;
		node=node->parent;
	}
	return tfm;
}

////////////////////////////////////////////////////////////

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::YsShellDnmContainer()
{
	searchKeySeed=1;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::~YsShellDnmContainer()
{
	CleanUp();
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::GetBoundingBox(YsVec3 bbx[2]) const
{
	return GetBoundingBox(bbx[0],bbx[1]);
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::GetBoundingBox(YsVec3 &min,YsVec3 &max) const
{
	YsArray <const Node *> allNode;
	GetNodePointerAll(allNode);

	YsBoundingBoxMaker3 mkBbx;
	for(auto &node : allNode)
	{
		YsVec3 shlBbx[2];
		node->GetBoundingBox(shlBbx);
		mkBbx.Add(shlBbx[0]);
		mkBbx.Add(shlBbx[1]);
	}

	mkBbx.Get(min,max);
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::CleanUp(void)
{
	for(auto rootPtr : rootArray)
	{
		DeleteShellRecursive(rootPtr);
	}
	rootArray.CleanUp();
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DeleteShellRecursive(
    typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node *ptr)
{
	if(NULL!=ptr)
	{
		for(YSSIZE_T childIdx=0; childIdx<ptr->children.GetN(); ++childIdx)
		{
			DeleteShellRecursive(ptr->children[childIdx]);
		}
		ptr->children.CleanUp();
		nodeSearchTable.DeleteElement(ptr->GetSearchKey(),ptr);
		delete ptr;
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
int YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::GetNumChildNode(Node *ptr) const
{
	if(NULL!=ptr)
	{
		return (int)ptr->children.GetN();
	}
	else
	{
		return (int)rootArray.GetN();
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node *YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::GetFirstRootNode(void) const
{
	if(0<rootArray.GetN())
	{
		return rootArray[0];
	}
	return NULL;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
typename YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::Node *YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::GetParent(const Node *node)
{
	return node->parent;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
const typename YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::Node *YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::GetParent(const Node *node) const
{
	return node->parent;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
template <const int N>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::GetRootNodeArray(YsArray <Node *,N> &rootArray) const
{
	rootArray=this->rootArray;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YsArray <typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node *> YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::GetRootNodeArray(void) const
{
	return rootArray;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node *YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::CreateShell(
    typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node *parentPtr)
{
	Node *newShl=new Node;
	newShl->SetSearchKey(searchKeySeed++);
	nodeSearchTable.AddElement(newShl->GetSearchKey(),newShl);

	if(NULL!=parentPtr)
	{
		// Verify parentPtr belongs to this DNM.
		Node *ptr=parentPtr;
		while(NULL!=ptr->parent)
		{
			ptr=ptr->parent;
		}

		if(YSTRUE!=rootArray.IsIncluded(ptr))
		{
			YsPrintf("%s %d\n",__FUNCTION__,__LINE__);
			YsPrintf("  Given parentPtr does not belong to this DNM.\n");
			return NULL;
		}

		parentPtr->children.Append(newShl);
		newShl->parent=parentPtr;
	}
	else 
	{
		rootArray.Append(newShl);
		newShl->parent=NULL;
	}

	return newShl;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node *YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::FindNode(YSHASHKEY key)
{
	Node *node;
	if(YSOK==nodeSearchTable.FindElement(node,key))
	{
		return node;
	}
	return NULL;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
const typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node *YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::FindNode(YSHASHKEY key) const
{
	Node *node;
	if(YSOK==nodeSearchTable.FindElement(node,key))
	{
		return node;
	}
	return NULL;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YSRESULT YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DeleteShell(typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node *ptr)
{
	if(NULL!=ptr)
	{
		auto parentPtr=ptr->parent;
		if(NULL!=parentPtr)
		{
			for(YSSIZE_T idx=parentPtr->children.GetN()-1; 0<=idx; --idx)
			{
				if(parentPtr->children[idx]==ptr)
				{
					parentPtr->children.Delete(idx);
				}
			}
		}
		else
		{
			for(YSSIZE_T idx=rootArray.GetN()-1; 0<=idx; --idx)
			{
				if(rootArray[idx]==ptr)
				{
					rootArray.Delete(idx);
				}
			}
		}

		DeleteShellRecursive(ptr);
		return YSOK;
	}
	return YSERR;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
template <const int N>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::GetNodePointerAll(
    YsArray <const typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node *,N> &allNode) const
{
	allNode.CleanUp();

	for(auto rootPtr : rootArray)
	{
		auto i0=allNode.GetN();
		allNode.Append(rootPtr);
		for(YSSIZE_T todoIdx=i0; todoIdx<allNode.GetN(); ++todoIdx)
		{
			const Node *node=allNode[todoIdx];
			for(YSSIZE_T childIdx=0; childIdx<node->children.GetN(); ++childIdx)
			{
				allNode.Append(node->children[childIdx]);
			}
		}
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
template <const int N>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::GetNodePointerAll(
    YsArray <typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node *,N> &allNode)
{
	allNode.CleanUp();

	for(auto rootPtr : rootArray)
	{
		auto i0=allNode.GetN();
		allNode.Append(rootPtr);
		for(YSSIZE_T todoIdx=i0; todoIdx<allNode.GetN(); ++todoIdx)
		{
			Node *node=allNode[todoIdx];
			for(YSSIZE_T childIdx=0; childIdx<node->children.GetN(); ++childIdx)
			{
				allNode.Append(node->children[childIdx]);
			}
		}
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YsArray <typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Node *> YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::GetNodePointerAll(void)
{
	YsArray <Node *> allNode;
	GetNodePointerAll(allNode);
	return allNode;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::MakeFlatTreeStructure(void)
{
	auto allNode=GetNodePointerAll();
	for(auto nodePtr : allNode)
	{
		nodePtr->parent=nullptr;
		nodePtr->children.CleanUp();
	}
	rootArray=allNode;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::Reconnect(Node *childPtr,Node *parentPtr)
{
	auto currentParentPtr=childPtr->parent;
	if(currentParentPtr!=parentPtr)
	{
		if(nullptr!=currentParentPtr)
		{
			for(YSSIZE_T idx=currentParentPtr->children.GetN()-1; 0<=idx; --idx)
			{
				if(currentParentPtr->children[idx]==childPtr)
				{
					currentParentPtr->children.Delete(idx);
				}
			}
		}
		else
		{
			for(YSSIZE_T idx=rootArray.GetN()-1; 0<=idx; --idx)
			{
				if(childPtr==rootArray[idx])
				{
					rootArray.Delete(idx);
				}
			}
		}

		childPtr->parent=parentPtr;
		if(nullptr!=parentPtr)
		{
			parentPtr->children.Add(childPtr);
		}
		else
		{
			rootArray.Add(childPtr);
		}
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
typename YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::DnmState YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::EmptyDnmState(void) const
{
	DnmState state;
	return state;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::ClearState(DnmState &dnmState)
{
	dnmState.Initialize();
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetZeroTransformation(DnmState &dnmState)
{
	for(auto node : rootArray)
	{
		SetZeroTransformationRecursive(dnmState,node);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetZeroTransformationRecursive(DnmState &dnmState,Node *node)
{
	auto &state=dnmState.GetState(node);
	state.relPos=YsVec3::Origin();
	state.relAtt=YsZeroAtt();
	state.tfmCache.LoadIdentity();
	for(auto childNode : node->children)
	{
		SetZeroTransformationRecursive(dnmState,childNode);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::SetState(DnmState &dnmState,int stateId)
{
	for(auto node : rootArray)
	{
		SetStateRecursive(dnmState,node,stateId);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::SetStateRecursive(DnmState &dnmState,Node *node,int stateId)
{
	auto &state=dnmState.GetState(node);
	if(YSTRUE==node->stateArray.IsInRange(stateId))
	{
		state=node->stateArray[stateId];
	}
	for(auto childNode : node->children)
	{
		SetStateRecursive(dnmState,childNode,stateId);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetClassState(DnmState &dnmState,int dnmClassType,int stateId)
{
	for(auto node : rootArray)
	{
		SetClassStateRecursive(dnmState,dnmClassType,node,stateId);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetClassStateRecursive(DnmState &dnmState,int dnmClassType,Node *node,int stateId)
{
	auto &state=dnmState.GetState(node);
	if(dnmClassType==node->dnmClassType && YSTRUE==node->stateArray.IsInRange(stateId))
	{
		state=node->stateArray[stateId];
	}
	for(auto childNode : node->children)
	{
		SetClassStateRecursive(dnmState,dnmClassType,childNode,stateId);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetClassState(DnmState &dnmState,int dnmClassType,int stateId0,int stateId1,const double param) const
{
	for(auto node : rootArray)
	{
		SetClassStateRecursive(dnmState,dnmClassType,node,stateId0,stateId1,param);
	}
}
template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetClassStateRecursive(DnmState &dnmState,int dnmClassType,Node *node,int stateId0,int stateId1,const double param) const
{
	if(dnmClassType==node->dnmClassType)
	{
		auto &state=dnmState.GetState(node);
		if(stateId0==stateId1)
		{
			if(YSTRUE==node->stateArray.IsInRange(stateId0))
			{
				state=node->stateArray[stateId0];
			}
		}
		else if(YSTRUE==node->stateArray.IsInRange(stateId0) && YSTRUE==node->stateArray.IsInRange(stateId1))
		{
			auto &state0=node->stateArray[stateId0];
			auto &state1=node->stateArray[stateId1];

			state.relPos=state0.relPos*(1.0-param)+state1.relPos*param;

			const YsVec3 ev0=state0.relAtt.GetForwardVector();
			const YsVec3 uv0=state0.relAtt.GetUpVector();
			const YsVec3 ev1=state1.relAtt.GetForwardVector();
			const YsVec3 uv1=state1.relAtt.GetUpVector();
			const YsVec3 ev=YsUnitVector(ev0*(1.0-param)+ev1*param);
			const YsVec3 uv=YsUnitVector(uv0*(1.0-param)+uv1*param);
			state.relAtt.SetTwoVector(ev,uv);

			if(YSTRUE==state0.GetShow() && YSTRUE!=state1.GetShow() && 0.99<param)
			{
				state.SetShow(YSFALSE);
			}
			else if(YSTRUE!=state0.GetShow() && YSTRUE==state1.GetShow() && param<0.01)
			{
				state.SetShow(YSFALSE);
			}
			else
			{
				state.SetShow(YSTRUE);
			}
		}
		else if(YSTRUE==node->stateArray.IsInRange(stateId0))
		{
			state=node->stateArray[stateId0];
		}
		else if(YSTRUE==node->stateArray.IsInRange(stateId1))
		{
			state=node->stateArray[stateId1];
		}
	}

	for(auto childNode : node->children)
	{
		SetClassStateRecursive(dnmState,dnmClassType,childNode,stateId0,stateId1,param);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetClassShow(DnmState &dnmState,int dnmClassType,YSBOOL show)
{
	for(auto node : rootArray)
	{
		SetClassShowRecursive(dnmState,dnmClassType,node,show);
	}
}
template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetClassShowRecursive(DnmState &dnmState,int dnmClassType,Node *node,YSBOOL show)
{
	if(dnmClassType==node->dnmClassType)
	{
		auto &state=dnmState.GetState(node);
		state.SetShow(show);
	}

	for(auto childNode : node->children)
	{
		SetClassShowRecursive(dnmState,dnmClassType,childNode,show);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetClassHeading(DnmState &dnmState,int dnmClassType,const double Heading)
{
	for(auto node : rootArray)
	{
		SetClassHeadingRecursive(dnmState,dnmClassType,node,Heading);
	}
}
template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetClassHeadingRecursive(DnmState &dnmState,int dnmClassType,Node *node,const double Heading)
{
	if(dnmClassType==node->dnmClassType)
	{
		auto &state=dnmState.GetState(node);
		state.SetHeading(Heading);
	}

	for(auto childNode : node->children)
	{
		SetClassHeadingRecursive(dnmState,dnmClassType,childNode,Heading);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetClassPitch(DnmState &dnmState,int dnmClassType,const double pitch)
{
	for(auto node : rootArray)
	{
		SetClassPitchRecursive(dnmState,dnmClassType,node,pitch);
	}
}
template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetClassPitchRecursive(DnmState &dnmState,int dnmClassType,Node *node,const double pitch)
{
	if(dnmClassType==node->dnmClassType)
	{
		auto &state=dnmState.GetState(node);
		state.SetPitch(pitch);
	}

	for(auto childNode : node->children)
	{
		SetClassPitchRecursive(dnmState,dnmClassType,childNode,pitch);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetClassRotation(DnmState &dnmState,int dnmClassType,const double rotAngle)
{
	for(auto node : rootArray)
	{
		SetClassRotationRecursive(dnmState,dnmClassType,node,rotAngle);
	}
}
template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetClassRotationRecursive(DnmState &dnmState,int dnmClassType,Node *node,const double rotAngle)
{
	if(dnmClassType==node->dnmClassType)
	{
		auto &state=dnmState.GetState(node);
		YsRotation rot(node->localRotationAxis,rotAngle);
		auto ev=YsZeroAtt().GetForwardVector();
		auto uv=YsZeroAtt().GetUpVector();
		rot.RotateNegative(ev,ev);
		rot.RotateNegative(uv,uv);
		YsAtt3 att;
		att.SetTwoVector(ev,uv);
		state.SetHeading(att.h());
		state.SetPitch(att.p());
		state.SetBank(att.b());
	}
	for(auto childNode : node->children)
	{
		SetClassRotationRecursive(dnmState,dnmClassType,childNode,rotAngle);
	}
}


template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SetNodeAngle(DnmState &dnmState,Node *node,const double heading,const double pitch)
{
	auto &state=dnmState.GetState(node);
	state.SetHeading(heading);
	state.SetPitch(pitch);
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::CacheTransformation(DnmState &dnmState) const
{
	for(auto node : rootArray)
	{
		CacheTransformationRecursive(dnmState,node);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer <SHLCLASS,NODESTATE_EXTRA_INFO>::CacheTransformationRecursive(DnmState &dnmState,const Node *node) const
{
	dnmState.CacheTransformation(node);
	for(auto childNode : node->children)
	{
		CacheTransformationRecursive(dnmState,childNode);
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YSRESULT YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::LoadDnm(YsTextInputStream &inStream,Node *allParent,const char dnmFileName[])
{
	YsObjectFromHeap <ReadDnmVariable> var;

	YsString idStr;
	int line=1;
	if(NULL!=inStream.Gets(idStr))
	{
		++line;

		idStr.Capitalize();
		if(YSTRUE==idStr.DOESSTARTWITH("DYNAMODEL"))
		{
			if(BeginReadDnm(var,allParent,dnmFileName)==YSOK)
			{
				YsString buf;
				while(NULL!=inStream.Gets(buf))
				{
					line++;
					if(ReadDnmOneLine(var,buf)!=YSOK)
					{
						printf("Line %d\n",line);
						EndReadDnm(var);
						goto ERREND;
					}
				}
				if(EndReadDnm(var)==YSOK)
				{
					return YSOK;
				}
			}
		}
		else if(YSTRUE==idStr.DOESSTARTWITH("SURF"))
		{
			CleanUp();
			auto newNode=CreateShell(nullptr);
			newNode->LoadSrf(inStream);
		}
	}

ERREND:
	return YSERR;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YSRESULT YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::BeginReadDnm(ReadDnmVariable &var,Node *allParent,const char fn[])
{
	var.allParent=allParent;
	var.readState=0;
	var.currentNode=NULL;
	var.nPackedFileRemain=0;
	var.dnmVersion=0;

	if(NULL!=fn)
	{
		YsString ful(fn),fil;
		ful.SeparatePathFile(var.readPath,fil);
	}
	else
	{
		var.readPath.Set(".");
	}
	return YSOK;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
const char *const YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::keyWordSource[]=
{
	"DYNAMODEL", // 0
	"DNMVER",    // 1
	"PCK",       // 2
	"SRF",       // 3
	"PLG",       // 4
	"LIN",       // 5
	"CLA",       // 6
	"NST",       // 7
	"STA",       // 8
	"NCH",       // 9
	"REL",       // 10
	"CLD",       // 11
	"FIL",       // 12
	"VTX",       // 13
	"NVT",       // 14
	"COL",       // 15
	"POS",       // 16
	"CNT",       // 17
	"END",       // 18
	"PAX",       // 19
	NULL
};

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YsKeyWordList YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::keyWordList;

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YSRESULT YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::ReadDnmOneLine(ReadDnmVariable &var,const YsString &str)
{
	YsArray <YsString,16> av;

	if(keyWordList.GetN()==0)
	{
		keyWordList.MakeList(keyWordSource);
	}

	if(var.readState==ReadDnmVariable::READSTATE_PACKEDFILE/*99*/)
	{
		var.currentPack->AddString(str);
		var.nPackedFileRemain--;
		if(var.nPackedFileRemain==0)
		{
			var.readState=ReadDnmVariable::READSTATE_NORMAL/*0*/;
		}
		return YSOK;
	}
	else if(YSOK==str.Arguments(av) && 0<av.GetN())
	{
		auto ac=av.GetN();
		av[0].Capitalize();
		if(var.readState==ReadDnmVariable::READSTATE_NORMAL/*0*/)
		{
			if(av[0][0]=='#')    // 2003/03/25
			{                         // 2003/03/25
				// Comment line       // 2003/03/25
				return YSOK;          // 2003/03/25
			}                         // 2003/03/25

			const int cmdId=keyWordList.GetId(av[0]);
			switch(cmdId)
			{
			case 0:   // "DYNAMODEL", // 0
				// Ignore
				return YSOK;
			case 1:   // "DNMVER",    // 1
				if(ac>=2)
				{
					var.dnmVersion=atoi(av[1]);
					return YSOK;
				}
				break;
			case 2:   // "PCK",       // 2
				if(ac>=3)
				{
					var.packedFileList.Increment();
					var.packedFileList.Last().Initialize();
					var.packedFileList.Last().fn=av[1];
					var.currentPack=&var.packedFileList.Last();
					var.nPackedFileRemain=atoi(av[2]);
					if(var.nPackedFileRemain>0)
					{
						var.readState=ReadDnmVariable::READSTATE_PACKEDFILE/*99*/;
					}
					return YSOK;
				}
				break;
			case 3:   // "SRF",       // 3
				if(ac>=2)
				{
					auto newNode=CreateShell(NULL);
					var.newNodeArray.Append(newNode);
					var.currentNode=newNode;
					var.currentNode->nodeName=av[1];
					var.currentNodeType=0;
					return YSOK;
				}
				break;
			case 4:   // "PLG",       // 4
				if(ac>=2)
				{
					auto newNode=CreateShell(NULL);
					var.newNodeArray.Append(newNode);
					var.currentNode=newNode;
					var.currentNode->nodeName=av[1];
					var.currentNodeType=1;
					return YSOK;
				}
				break;
			case 5:   // "LIN",       // 5
				if(ac>=2)
				{
					auto newNode=CreateShell(NULL);
					var.newNodeArray.Append(newNode);
					var.currentNode=newNode;
					var.currentNode->nodeName=av[1];
					var.currentNodeType=2;
					return YSOK;
				}
				break;
			case 6:   // "CLA",       // 6
				if(ac>=2 && var.currentNode!=NULL)
				{
					var.currentNode->dnmClassType=atoi(av[1]);
					return YSOK;
				}
				break;
			case 7:   // "NST",       // 7
				// Ignore
				return YSOK;
			case 8:   // "STA",       // 8
				if(ac>=8 && var.currentNode!=NULL)
				{
					NodeState sta;
					sta.relPos.Set(atof(av[1]),atof(av[2]),atof(av[3]));
					sta.relAtt.Set(atof(av[4])*YsPi/32768,atof(av[5])*YsPi/32768,atof(av[6])*YsPi/32768);
					sta.SetShow(atoi(av[7])==0 ? YSFALSE : YSTRUE);
					var.currentNode->stateArray.Append(sta);
					return YSOK;
				}
				break;
			case 9:   // "NCH",       // 9
				// Nothing to do
				return YSOK;
			case 10:  // "REL",       // 10
				// Only "DEP" is accepted, thus, ignored
				return YSOK;
			case 11:  // "CLD",       // 11
				if(var.currentNode!=NULL && ac>=2)
				{
					var.currentNode->childNameTag.Increment();
					var.currentNode->childNameTag.Last()=av[1];
					return YSOK;
				}
				break;
			case 12:  // "FIL",       // 12
				if(var.currentNode!=NULL && ac>=2)
				{
					if(var.currentNodeType==0)
					{
						YSBOOL isPacked=YSFALSE;
						for(auto &pack : var.packedFileList)
						{
							if(0==av[1].STRCMP(pack.fn))
							{
								pack.BeginRead();
								var.currentNode->LoadSrf(pack);
								isPacked=YSTRUE;
								break;
							}
						}
						if(isPacked!=YSTRUE)
						{
							YsString ful;
							ful.MakeFullPathName(var.readPath,av[1]);

							FILE *fp=fopen(ful,"r");
							if(NULL!=fp)
							{
								YsTextFileInputStream srfInStream(fp);
								if(var.currentNode->LoadSrf(srfInStream)!=YSOK)
								{
									YsPrintf("Cannot read %s\n",ful.Txt());
									var.readState=ReadDnmVariable::READSTATE_ERROR/*-1*/;
									fclose(fp);
									return YSERR;
								}
								fclose(fp);
							}
						}
					}
					return YSOK;
				}
				break;
			case 13:  // "VTX",       // 13
				if(var.currentNode!=NULL && ac==7)
				{
					if(var.currentNodeType==2)
					{
						double l;
						YsVec3 p1,p2,u,v,w,vv,ww;
						p1.Set(atof(av[1]),atof(av[2]),atof(av[3]));
						p2.Set(atof(av[4]),atof(av[5]),atof(av[6]));
						u=p2-p1;
						u.Normalize();
						v=u.GetArbitraryPerpendicularVector();
						v.Normalize();
						w=u^v;
						l=(p2-p1).GetLength();
						vv=v*0.015;  // vv=v*l/50.0;  2003/03/26
						ww=w*0.015;  // ww=w*l/50.0;  2003/03/26

						YsShellVertexHandle vtHd[8],quad[4];
						YsShellPolygonHandle plHd;
						vtHd[0]=var.currentNode->AddVertex(p1+vv+ww);
						vtHd[1]=var.currentNode->AddVertex(p1+vv-ww);
						vtHd[2]=var.currentNode->AddVertex(p1-vv-ww);
						vtHd[3]=var.currentNode->AddVertex(p1-vv+ww);
						vtHd[4]=var.currentNode->AddVertex(p2+vv+ww);
						vtHd[5]=var.currentNode->AddVertex(p2+vv-ww);
						vtHd[6]=var.currentNode->AddVertex(p2-vv-ww);
						vtHd[7]=var.currentNode->AddVertex(p2-vv+ww);

						quad[0]=vtHd[0];
						quad[1]=vtHd[1];
						quad[2]=vtHd[2];
						quad[3]=vtHd[3];
						plHd=var.currentNode->AddPolygon(4,quad);
						var.currentNode->SetPolygonColor(plHd,YsWhite());
						var.currentNode->SetPolygonNormal(plHd,-u);

						quad[0]=vtHd[7];
						quad[1]=vtHd[6];
						quad[2]=vtHd[5];
						quad[3]=vtHd[4];
						plHd=var.currentNode->AddPolygon(4,quad);
						var.currentNode->SetPolygonColor(plHd,YsWhite());
						var.currentNode->SetPolygonNormal(plHd,u);

						quad[0]=vtHd[1];
						quad[1]=vtHd[0];
						quad[2]=vtHd[4];
						quad[3]=vtHd[5];
						plHd=var.currentNode->AddPolygon(4,quad);
						var.currentNode->SetPolygonColor(plHd,YsWhite());
						var.currentNode->SetPolygonNormal(plHd,v);

						quad[0]=vtHd[2];
						quad[1]=vtHd[1];
						quad[2]=vtHd[5];
						quad[3]=vtHd[6];
						plHd=var.currentNode->AddPolygon(4,quad);
						var.currentNode->SetPolygonColor(plHd,YsWhite());
						var.currentNode->SetPolygonNormal(plHd,-w);

						quad[0]=vtHd[3];
						quad[1]=vtHd[2];
						quad[2]=vtHd[6];
						quad[3]=vtHd[7];
						plHd=var.currentNode->AddPolygon(4,quad);
						var.currentNode->SetPolygonColor(plHd,YsWhite());
						var.currentNode->SetPolygonNormal(plHd,-v);

						quad[0]=vtHd[0];
						quad[1]=vtHd[3];
						quad[2]=vtHd[7];
						quad[3]=vtHd[4];
						plHd=var.currentNode->AddPolygon(4,quad);
						var.currentNode->SetPolygonColor(plHd,YsWhite());
						var.currentNode->SetPolygonNormal(plHd,w);

						// var.currentNode->RecomputeNormalAndFlipDirection();
						// var.currentNode->SortPolygonByColor();
						// var.currentNode->PrecomputePolygCenterAfterSortingByColor();
					}
					return YSOK;
				}
				break;
			case 14:  //  "NVT",       // 14
				if(var.currentNode!=NULL && ac>=2)
				{
					var.nVtxRemain=atoi(av[1]);
					if(var.nVtxRemain>0)
					{
						var.readState=ReadDnmVariable::READSTATE_VERTEXARRAY/*1*/;
					}
					return YSOK;
				}
				break;
			case 15:  // "COL",       // 15
				if(var.currentNode!=NULL && ac>=2)
				{
					YsColor col;
					if(ac>=4)
					{
						col.SetIntRGB(atoi(av[1]),atoi(av[2]),atoi(av[3]));
					}
					else
					{
						col.Set15BitRGB(atoi(av[1]));
					}
					for(auto plHd : var.currentNode->AllPolygon())
					{
						var.currentNode->SetPolygonColor(plHd,col);
					}
					return YSOK;
				}
				break;
			case 16:  // "POS",       // 16
				if(var.currentNode!=NULL && ac>=7)
				{
					double x,y,z,h,p,b;
					x=atof(av[1]);
					y=atof(av[2]);
					z=atof(av[3]);
					h=atof(av[4])*YsPi/32768;
					p=atof(av[5])*YsPi/32768;
					b=atof(av[6])*YsPi/32768;
					var.currentNode->posInParent.Set(x,y,z);
					var.currentNode->attInParent.Set(h,p,b);
					return YSOK;
				}
				break;
			case 17:  // "CNT",       // 17
				if(var.currentNode!=NULL && ac>=4)
				{
					double x,y,z;
					x=atof(av[1]);
					y=atof(av[2]);
					z=atof(av[3]);
					var.currentNode->localRotationCenter.Set(x,y,z);
					return YSOK;
				}
				break;
			case 18:  // "END",       // 18
				if(var.currentNode!=NULL)
				{
					var.currentNode=NULL;
				}
				else
				{
					var.readState=ReadDnmVariable::READSTATE_ENDOFFILE/*999*/;
				}
				return YSOK;
			case 19:  // "PAX",       // 19
				if(var.currentNode!=NULL && ac>=4)
				{
					double x,y,z;
					x=atof(av[1]);
					y=atof(av[2]);
					z=atof(av[3]);
					var.currentNode->localRotationAxis.Set(x,y,z);
					return YSOK;
				}
				break;
			}
		}
		else if(var.readState==ReadDnmVariable::READSTATE_VERTEXARRAY/*1*/)
		{
			YsShellVertexHandle vtHd;
			if(ac>=4)
			{
				YsVec3 pos;
				pos.Set(atof(av[1]),atof(av[2]),atof(av[3]));
				var.currentNode->AddVertex(pos);
				var.nVtxRemain--;
				if(var.nVtxRemain==0)
				{
					vtHd=NULL;
					YsArray <YsShellVertexHandle,4> plVtHd;
					YsArray <YsVec3> plVtPos;
					while(YSOK==var.currentNode->MoveToNextVertex(vtHd))
					{
						plVtHd.Append(vtHd);
						var.currentNode->GetVertexPosition(pos,vtHd);
						plVtPos.Append(pos);
					}

					auto plHd=var.currentNode->AddPolygon(plVtHd);
					var.currentNode->SetPolygonTwoSidedFlag(plHd,YSTRUE);

					YsVec3 nom;
					YsGetAverageNormalVector(nom,plVtPos.GetN(),plVtPos);
					nom.Normalize();
					var.currentNode->SetPolygonNormal(plHd,nom);

					// var.currentNode->RecomputeNormalAndFlipDirection();
					// var.currentNode->SortPolygonByColor();
					// var.currentNode->PrecomputePolygCenterAfterSortingByColor();

					var.readState=ReadDnmVariable::READSTATE_NORMAL/*0*/;
				}
				return YSOK;
			}
			else
			{
				YsPrintf("Error during reading polygon node.\n");
				YsPrintf("  >> %s\n",str.Txt());
				var.readState=ReadDnmVariable::READSTATE_ERROR/*-1*/;
				return YSERR;
			}
		}
		else if(var.readState==ReadDnmVariable::READSTATE_ENDOFFILE/*999*/)
		{
			return YSOK;
		}

		YsPrintf("Error during reading DNM\n");
		YsPrintf("  >> %s\n",str.Txt());
		return YSERR;
	}
	return YSOK;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YSRESULT YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::EndReadDnm(ReadDnmVariable &var)
{
	MakeConnectionAndRootNodeListByChildNameTag(var);
	DeleteChildNodeNameTag();
	return YSOK;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::DeleteChildNodeNameTag(void)
{
	YsArray <Node *> allNode;
	GetNodePointerAll(allNode);
	for(auto node : allNode)
	{
		node->childNameTag.ClearDeep();
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YSRESULT YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::MakeConnectionAndRootNodeListByChildNameTag(ReadDnmVariable &var)
{
	// At this point, all nodes are tentatively root nodes.
	auto dnmVersion=var.dnmVersion;
	auto allParent=var.allParent;

	YsArray <Node *> allNode=var.newNodeArray;
	YsKeyStore newNodeKey;
	for(auto node : allNode)
	{
		node->parent=NULL;
		node->children.CleanUp();
		newNodeKey.AddKey(node->GetSearchKey());
	}
	for(YSSIZE_T idx=rootArray.GetN()-1; 0<=idx; --idx)
	{
		if(YSTRUE==newNodeKey.IsIncluded(rootArray[idx]->GetSearchKey()))
		{
			rootArray.Delete(idx);
		}
	}

	for(auto parent : allNode)
	{
		for(auto child : allNode)
		{
			for(auto &childName : parent->childNameTag)
			{
				if(0==strcmp(childName,child->nodeName))
				{
					YSBOOL isLoop=YSFALSE;
					Node *loopCheck=parent;
					while(loopCheck!=NULL)
					{
						if(loopCheck==child)
						{
							YsPrintf("Loop in DNM file!\n");
							isLoop=YSTRUE;
							break;
						}
						loopCheck=loopCheck->parent;
					}

					if(isLoop!=YSTRUE)
					{
						child->parent=parent;
						parent->children.Append(child);
					}
					break;
				}
			}
		}
	}

	YsArray <Node *> newRootNode;
	for(auto node : allNode)
	{
		if(node->parent==NULL)
		{
			newRootNode.Append(node);
		}
	}

	if(dnmVersion==0)  
	{
		// Mimicing the bug of the old DNM class.  
		//   Older version of DNM format allowed only one root node.  
		//   All node except the very first root node are automatically made a child of the first root node.
		for(YSSIZE_T i=newRootNode.GetN()-1; i>0; i--)
		{
			newRootNode[0]->children.Append(newRootNode[i]);
			newRootNode[i]->parent=newRootNode[0];
			newRootNode.Delete(i);
		}
	}

	if(NULL==allParent)
	{
		rootArray.Append(newRootNode);
	}
	else
	{
		allParent->children.Append(newRootNode);
	}

	return YSOK;
}

////////////////////////////////////////////////////////////

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
YSRESULT YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SaveDnm(YsTextOutputStream &outStream) const
{
	YsHashTable <YsString> nodeKeyToDnmName;
	CreateUniqueDnmName(nodeKeyToDnmName);

	outStream.Printf("DYNAMODEL\n");
	outStream.Printf("DNMVER 1\n");
	WritePackage(outStream,nodeKeyToDnmName);

	return YSOK;
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::CreateUniqueDnmName(YsHashTable <YsString> &nodeKeyToDnmName) const
{
	YsDictionary <YsString,int> usedDnmName;  // From dnm name to use count.
	usedDnmName.SetCaseSensitivity(YSFALSE);

	YsArray <const Node *> allNodePtr;
	GetNodePointerAll(allNodePtr);

	for(auto nodePtr : allNodePtr)
	{
		auto &node=*nodePtr;

		YsString nodeName=node.nodeName;
		if(0==nodeName.Strlen())
		{
			nodeName="Node";
		}

		if(YSTRUE!=usedDnmName.IsWordIncluded(nodeName))
		{
			nodeKeyToDnmName.AddElement(node.GetSearchKey(),nodeName);
			usedDnmName.AddWord(nodeName,1);
		}
		else
		{
			const auto orgNodeName=nodeName;
			int useCount=*usedDnmName.FindAttrib(nodeName);
			for(;;)
			{
				++useCount;
				YsString n;
				n.Printf("%s(%d)",nodeName.Txt(),useCount);
				if(YSTRUE!=usedDnmName.IsWordIncluded(n))
				{
					nodeName=n;
					break;
				}
			}
			nodeKeyToDnmName.AddElement(node.GetSearchKey(),nodeName);
			usedDnmName.AddWord(nodeName,1);
			usedDnmName.UpdateAttrib(orgNodeName,useCount);
		}
	}
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::SaveNodeToMemory(YsTextMemoryOutputStream &outStream,const Node &node) const
{
	outStream.CleanUp();
	YsShellExtWriter writer;
	writer.SaveSrf(outStream,node.Conv());
}

template <class SHLCLASS,class NODESTATE_EXTRA_INFO>
void YsShellDnmContainer<SHLCLASS,NODESTATE_EXTRA_INFO>::WritePackage(YsTextOutputStream &outStream,const YsHashTable <YsString> &nodeKeyToDnmName) const
{
	YsArray <const Node *> allNodePtr;
	GetNodePointerAll(allNodePtr);

	for(auto nodePtr : allNodePtr)
	{
		auto &node=*nodePtr;

		YsString nodeName;
		nodeKeyToDnmName.FindElement(nodeName,node.GetSearchKey());  // Not supposed to fail.

		printf("Writing [%s]\n",nodeName.Txt());

		YsTextMemoryOutputStream memOut;
		SaveNodeToMemory(memOut,node);

		outStream.Printf("PCK \"%s\" %d\n",nodeName.Txt(),(int)memOut.GetNumLine());
		for(YSSIZE_T line=0; line<memOut.GetNumLine(); ++line)
		{
			outStream.Printf("%s\n",memOut[line].Txt());
		}
	}

	for(auto nodePtr : allNodePtr)
	{
		auto &node=*nodePtr;

		YsString nodeName;
		nodeKeyToDnmName.FindElement(nodeName,node.GetSearchKey());  // Not supposed to fail.

		outStream.Printf("SRF \"%s\"\n",nodeName.Txt());
		outStream.Printf("FIL \"%s\"\n",nodeName.Txt());
		outStream.Printf("CLA %d\n",node.dnmClassType);
		outStream.Printf("NST %d\n",node.stateArray.GetN());
		for(auto &sta : node.stateArray)
		{
			outStream.Printf("STA %lf %lf %lf %lf %lf %lf %d\n",
			    sta.relPos.x(),sta.relPos.y(),sta.relPos.z(),
			    sta.relAtt.h()*32768.0/YsPi,sta.relAtt.p()*32768.0/YsPi,sta.relAtt.b()*32768.0/YsPi,
			    (int)sta.GetShow());
		}
		outStream.Printf("NCH %d\n",node.children.GetN());
		for(auto childPtr : node.children)
		{
			auto &child=*childPtr;
			YsString childName;
			nodeKeyToDnmName.FindElement(childName,child.GetSearchKey());
			outStream.Printf("CLD \"%s\"\n",childName.Txt());
		}
		outStream.Printf("POS %lf %lf %lf %lf %lf %lf\n",
			node.posInParent.x(),node.posInParent.y(),node.posInParent.z(),
			node.attInParent.h()*32768.0/YsPi,node.attInParent.p()*32768.0/YsPi,node.attInParent.b()*32768.0/YsPi);
		outStream.Printf("CNT %lf %lf %lf\n",
			node.localRotationCenter.x(),
			node.localRotationCenter.y(),
			node.localRotationCenter.z());
		outStream.Printf("PAX %lf %lf %lf\n",
			node.localRotationAxis.x(),
			node.localRotationAxis.y(),
			node.localRotationAxis.z());
		outStream.Printf("END\n");
	}
}

/* } */
#endif
