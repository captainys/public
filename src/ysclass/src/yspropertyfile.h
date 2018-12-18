#ifndef YSPROPERTYFILE_IS_INCLUDED
#define YSPROPERTYFILE_IS_INCLUDED

#include <memory>

#include "ysdef.h"
#include "ysstring.h"
#include "ysavltree.h"

/*! This class is to save/load properties.
    I was writing the same code over and over again.  I decided to make a base code so that I can 
    reduce writing the same code again.
    Property values are linked to a tag by a pointer.  Therefore, the pointer to the property values
    must be valid as long as this class may be used for reading/writing the value.
    Tags will be written in the order added in the class.
    The file can be written in any order.  The format of the file will be:

      TAG value
*/
class YsPropertyFileIO
{
public:
	template <class T>
	static inline YsString ToString(const T &value);

	template <class T>
	static inline T FromString(const YsString &str);

private:
	class Property
	{
	friend class YsPropertyFileIO;
	private:
		YsString tag;
		virtual YsString FormatString(void) const=0;
		virtual YSRESULT Recognize(const YsConstArrayMask <YsString> &argv)=0;
	};

	template <class T>
	class PropertyTemplate : public Property
	{
	friend class YsPropertyFileIO;
	private:
		T *varPtr;
		virtual YsString FormatString(void) const
		{
			YsString str;
			str=tag;
			str.Append(" ");
			str.Append(ToString(*varPtr));
			return str;
		}
		virtual YSRESULT Recognize(const YsConstArrayMask <YsString> &argv)
		{
			if(0==argv[0].Strcmp(tag) && 2==argv.size())
			{
				*varPtr=FromString<T>(argv[1]);
				return YSOK;
			}
			return YSERR;
		}
	};

private:
	YsArray <std::shared_ptr <Property> > prop;
	YsAVLTree <YsString,std::shared_ptr <Property> > tagToPropPtr;
	YsString errStr;

public:
	/*! Add a property.  The pointer to propRef will be retained in this class object.
	*/
	template <class T>
	void AddProperty(T &propRef,const YsString &tag)
	{
		auto *propPtr=new PropertyTemplate<T>;
		propPtr->varPtr=&propRef;
		propPtr->tag=tag;

		this->prop.Increment();
		this->prop.back().reset(propPtr);
		tagToPropPtr.Insert(tag,this->prop.back());
	}

	/*! Save to a text-output stream.
	*/
	void Save(class YsTextOutputStream &outStream);

	/*! Read from a text-output stream.
	*/
	YSRESULT Load(class YsTextInputStream &inStream);
};

template <>
inline YsString YsPropertyFileIO::ToString<YSBOOL>(const YSBOOL &value)
{
	return YsString(YsBoolToStr(value));
}
template <>
inline YSBOOL YsPropertyFileIO::FromString<YSBOOL>(const YsString &str)
{
	return YsStrToBool(str);
}

#endif
