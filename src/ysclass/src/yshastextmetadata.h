#ifndef YSHASTEXTMETADATA_IS_INCLUDED
#define YSHASTEXTMETADATA_IS_INCLUDED
/* { */


#include "yseditarray.h"
#include "ysstring.h"
#include "ysbinarytree2.h"

class YsHasTextMetaData
{
public:
	/*! A class for storing meta-data. */
	class MetaData
	{
	public:
		YsString key,value;
	};

	/*! A handle type for a meta data. */
	typedef YsEditArrayObjectHandle <MetaData> MetaDataHandle;

	/*! An unordered set of MetaData handles. */
	typedef YsEditArray<MetaData>::HandleStore MetaDataStore;



private:
	YsEditArray <MetaData> mdt;
	YsBinaryTree2 <YsString,YsEditArray<MetaData>::HandleStore,YsStringComparer<YsString> > mdtKeyToMdtHandle;

	void CommonInitializer(void);


public:
	YsHasTextMetaData();

	YsHasTextMetaData(const YsHasTextMetaData &incoming);

	YsHasTextMetaData &operator=(const YsHasTextMetaData &incoming);

	void CopyFrom(const YsHasTextMetaData &incoming);

	YsHasTextMetaData(YsHasTextMetaData &&incoming);

	YsHasTextMetaData &operator=(YsHasTextMetaData &&incoming);

	void MoveFrom(YsHasTextMetaData &incoming);


protected:
	void ReconstructKeyToHandle(void);


	void UseSharedSearchKeySeed(YSHASHKEY &seed);


public:
	/*! Adds a meta data. */
	MetaDataHandle AddMetaData(const char key[],const char value[]);
	MetaDataHandle AddMetaData(const YsString &key,const YsString &value);

	/*! Cleans up */
	void CleanUp(void);

	/*! Updates a meta-data value. */
	YSRESULT SetMetaDataValue(MetaDataHandle mdHd,const YsString &value);

	/*! Permanently deletes a meta data. */
	YSRESULT DeleteMetaData(MetaDataHandle mdHd);

	/*! Freezes (temporarily delets) a meta data. */
	YSRESULT FreezeMetaData(MetaDataHandle mdHd);

	/*! Melts (undeletes) a meta data. */
	YSRESULT MeltMetaData(MetaDataHandle mdHd);

	/*! Permanently deletes a meta data that is temporarily deleted. */
	YSRESULT DeleteFrozenMetaData(MetaDataHandle mdHd);

	/*! Returns a search key for a meta data. */
	YSHASHKEY GetSearchKey(MetaDataHandle mdHd) const;

	/*! Returns the number of meta data. */
	YSSIZE_T GetNumMetaData(void) const;

	/*! Finds a meta data from a search key. */
	MetaDataHandle FindMetaData(YSHASHKEY searchKey) const;

	/*! Returns a key of the meta data. */
	YsString GetMetaDataKey(MetaDataHandle mdHd) const;

	/*! Returns a value of the meta data. */
	YsString GetMetaDataValue(MetaDataHandle mdHd) const;

	/*! Returns key-value pair of the meta data. */
	MetaData GetMetaData(MetaDataHandle mdHd) const;

	typedef YsEditArray <MetaData>::HandleEnumerator MetaDataEnumerator;
	/*! Returns a meta-data enumerator.
	    With the returned enumerator, you can iterate through meta data key-value pairs by:
	        for(auto mdHd : shl.AllMetaData())
	        {
	        }
	    where shl is a YsShell object.
	*/
	MetaDataEnumerator AllMetaData(void) const;

	/*! Finds a set of meta-data from a key.  If none is found, it returns nullptr.
	*/
	const MetaDataStore *FindMetaData(const YsString &key) const;
};


/* } */
#endif
