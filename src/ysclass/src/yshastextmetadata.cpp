#include "yshastextmetadata.h"



YsHasTextMetaData::YsHasTextMetaData()
{
	CommonInitializer();
}

YsHasTextMetaData::YsHasTextMetaData(const YsHasTextMetaData &incoming)
{
	CommonInitializer();
	CopyFrom(incoming);
}

YsHasTextMetaData &YsHasTextMetaData::operator=(const YsHasTextMetaData &incoming)
{
	CopyFrom(incoming);
	return *this;
}

void YsHasTextMetaData::CopyFrom(const YsHasTextMetaData &incoming)
{
	if(this!=&incoming)
	{
		mdt.CopyFrom(incoming.mdt);
		ReconstructKeyToHandle();
	}
}

YsHasTextMetaData::YsHasTextMetaData(YsHasTextMetaData &&incoming)
{
	CommonInitializer();
	MoveFrom(incoming);
}

YsHasTextMetaData &YsHasTextMetaData::operator=(YsHasTextMetaData &&incoming)
{
	MoveFrom(incoming);
	return *this;
}

void YsHasTextMetaData::MoveFrom(YsHasTextMetaData &incoming)
{
	if(this!=&incoming)
	{
		mdt.MoveFrom(incoming.mdt);
		ReconstructKeyToHandle();
	}
}

void YsHasTextMetaData::ReconstructKeyToHandle(void)
{
	mdtKeyToMdtHandle.CleanUp();
	for(auto mdHd : AllMetaData())
	{
		auto foundTreeNdHd=mdtKeyToMdtHandle.FindNode(mdt[mdHd]->key);
		if(mdtKeyToMdtHandle.Null()==foundTreeNdHd)
		{
			YsEditArray<MetaData>::HandleStore mdHdStore;
			mdHdStore.Add(mdHd);
			mdtKeyToMdtHandle.Insert(mdt[mdHd]->key,(YsEditArray<MetaData>::HandleStore &&)mdHdStore);
		}
		else
		{
			auto &foundMdHdPtr=mdtKeyToMdtHandle[foundTreeNdHd];
			foundMdHdPtr.Add(mdHd);
		}
	}
}

void YsHasTextMetaData::CommonInitializer(void)
{
	mdt.EnableSearch();
}

void YsHasTextMetaData::UseSharedSearchKeySeed(YSHASHKEY &seed)
{
	mdt.UseSharedSearchKeySeed(seed);
}

YsHasTextMetaData::MetaDataHandle YsHasTextMetaData::AddMetaData(const char key[],const char value[])
{
	auto mdHd=mdt.Create();
	mdt[mdHd]->key=key;
	mdt[mdHd]->value=value;

	auto foundTreeNdHd=mdtKeyToMdtHandle.FindNode(mdt[mdHd]->key);
	if(mdtKeyToMdtHandle.Null()==foundTreeNdHd)
	{
		YsEditArray<MetaData>::HandleStore mdHdStore;
		mdHdStore.Add(mdHd);
		mdtKeyToMdtHandle.Insert(mdt[mdHd]->key,(YsEditArray<MetaData>::HandleStore &&)mdHdStore);
	}
	else
	{
		auto &foundMdHdPtr=mdtKeyToMdtHandle[foundTreeNdHd];
		foundMdHdPtr.Add(mdHd);
	}

	return mdHd;
}
YsHasTextMetaData::MetaDataHandle YsHasTextMetaData::AddMetaData(const YsString &key,const YsString &value)
{
	return AddMetaData(key.c_str(),value.c_str());
}

void YsHasTextMetaData::CleanUp(void)
{
	mdt.CleanUp();
	mdtKeyToMdtHandle.CleanUp();
}

YSRESULT YsHasTextMetaData::SetMetaDataValue(MetaDataHandle mdHd,const YsString &value)
{
	if(nullptr!=mdt[mdHd])
	{
		mdt[mdHd]->value=value;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsHasTextMetaData::DeleteMetaData(MetaDataHandle mdHd)
{
	if(nullptr!=mdHd)
	{
		auto foundTreeNdHd=mdtKeyToMdtHandle.FindNode(mdt[mdHd]->key);
		if(mdtKeyToMdtHandle.Null()!=foundTreeNdHd)
		{
			auto &foundMdHdPtr=mdtKeyToMdtHandle[foundTreeNdHd];
			foundMdHdPtr.Delete(mdHd);
		}
	}
	return mdt.Delete(mdHd);
}

YSRESULT YsHasTextMetaData::FreezeMetaData(MetaDataHandle mdHd)
{
	if(nullptr!=mdt[mdHd])
	{
		auto foundTreeNdHd=mdtKeyToMdtHandle.FindNode(mdt[mdHd]->key);
		if(mdtKeyToMdtHandle.Null()!=foundTreeNdHd)
		{
			auto &foundMdHdPtr=mdtKeyToMdtHandle[foundTreeNdHd];
			foundMdHdPtr.Delete(mdHd);
		}

		mdt.Freeze(mdHd);
		return YSOK;
	}
	return YSERR;
}
YSRESULT YsHasTextMetaData::MeltMetaData(MetaDataHandle mdHd)
{
	if(YSTRUE==mdt.IsFrozen(mdHd))
	{
		mdt.Melt(mdHd);

		auto foundTreeNdHd=mdtKeyToMdtHandle.FindNode(mdt[mdHd]->key);
		if(mdtKeyToMdtHandle.Null()==foundTreeNdHd)
		{
			YsEditArray<MetaData>::HandleStore mdHdStore;
			mdHdStore.Add(mdHd);
			mdtKeyToMdtHandle.Insert(mdt[mdHd]->key,(YsEditArray<MetaData>::HandleStore &&)mdHdStore);
		}
		else
		{
			auto &foundMdHdPtr=mdtKeyToMdtHandle[foundTreeNdHd];
			foundMdHdPtr.Add(mdHd);
		}

		return YSOK;
	}
	return YSERR;
}
YSRESULT YsHasTextMetaData::DeleteFrozenMetaData(MetaDataHandle mdHd)
{
	if(YSTRUE==mdt.IsFrozen(mdHd))
	{
		mdt.DeleteFrozen(mdHd);
		return YSOK;
	}
	return YSERR;
}

YSHASHKEY YsHasTextMetaData::GetSearchKey(MetaDataHandle mdHd) const
{
	return mdt.GetSearchKey(mdHd);
}

YSSIZE_T YsHasTextMetaData::GetNumMetaData(void) const
{
	return mdt.GetN();
}

YsHasTextMetaData::MetaDataHandle YsHasTextMetaData::FindMetaData(YSHASHKEY searchKey) const
{
	return mdt.FindObject(searchKey);
}
YsString YsHasTextMetaData::GetMetaDataKey(MetaDataHandle mdHd) const
{
	return mdt[mdHd]->key;
}
YsString YsHasTextMetaData::GetMetaDataValue(MetaDataHandle mdHd) const
{
	return mdt[mdHd]->value;
}
YsHasTextMetaData::MetaData YsHasTextMetaData::GetMetaData(MetaDataHandle mdHd) const
{
	return *(mdt[mdHd]);
}
YsHasTextMetaData::MetaDataEnumerator YsHasTextMetaData::AllMetaData(void) const
{
	return mdt.AllHandle();
}

const YsHasTextMetaData::MetaDataStore *YsHasTextMetaData::FindMetaData(const YsString &key) const
{
	auto foundTreeNdHd=mdtKeyToMdtHandle.FindNode(key);
	if(mdtKeyToMdtHandle.Null()==foundTreeNdHd || 0==mdtKeyToMdtHandle[foundTreeNdHd].size())
	{
		return nullptr;
	}
	else
	{
		return &mdtKeyToMdtHandle[foundTreeNdHd];
	}
}

