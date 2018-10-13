#pragma once

///
//// InfoCacheArray
///
//// The goal of this class is to create an array of info pointers used for looping xml file info.
//// Unlike normal looping, InfoCacheArray is designed to loop a subset and that way increase performance.
/// 
//// Usage:
///
//// get(i): returns a pointer to the info at index i. Null if out of scope
//// getLength() returns the number of elements in the array
///  loop control: either loop getLength() amounts of time or until get() returns NULL.
//// assign(BoolArray or JustInTimeArray): the only way to write contents
///
///
//// Usage example:
///  Goal: only check citizens for yield consumption, which they can consume.
///  Make a BoolArray of length yields and loop CvUnitInfos
///  For each yield, which can be consumed, set the BoolArray to true
///  Make a YieldCacheArray and assign the BoolArray
///  From now on whenever all consumable yields are looped, use the YieldCacheArray
///  This way yields, which aren't consumed by any unit will not be checked, hence reducing loop iterations.
///  This also provides the CvYieldInfo without looking each up in GC.
///  

class CvInfoBase;
class BoolArray;

template<typename> class JustInTimeArray;

///
//// InfoCacheArray is an abstract class in the sense that it shouldn't be declared anywhere
//// Use a file specific class at the end of this file instead
///

class InfoCacheArray : private boost::noncopyable
{
public:
	int getLength() const { return m_iLength; }

protected:
	InfoCacheArray();
	~InfoCacheArray();

	inline const CvInfoBase* get(int i) const
	{
		if (i >= 0 && i < m_iLength)
		{
			return m_pInfo[i];
		}
		return NULL;
	}

	void assign(const BoolArray& kArray);

	template<typename T>
	void assign(const JustInTimeArray<T>& kArray);

private:
	const CvInfoBase **m_pInfo;
	int m_iLength;
};

///
////
//// Access classes for each xml file
////
//// The goal of these classes is to ensure InfoCacheArray will return pointers to the infos from the correct xml files.
////
///  Note: since this header is included before a lot of other headers, static_cast doesn't work
///  However the simple C style typecast does
///  Also assign is written in the header despite being a bit messy. The reason is to avoid template assignment mess.
///  


class CvBuildingInfo;
template<typename> class BuildingArray;
class BuildingInfoArray : public InfoCacheArray
{
public:
	BuildingInfoArray() {}

	inline const CvBuildingInfo* get(int i) const
	{
		return (const CvBuildingInfo*)InfoCacheArray::get(i);
	}

	void assign(const BoolArray& kArray);
	template<typename T>
	void assign(const BuildingArray<T>& kArray)
	{
		InfoCacheArray::assign((const JustInTimeArray<T>&)kArray);
	}
};

class CvProfessionInfo;
template<typename> class ProfessionArray;
class ProfessionInfoArray : public InfoCacheArray
{
public:
	ProfessionInfoArray() {}

	inline const CvProfessionInfo* get(int i) const
	{
		return (const CvProfessionInfo*)InfoCacheArray::get(i);
	}

	void assign(const BoolArray& kArray);
	template<typename T>
	void assign(const ProfessionArray<T>& kArray)
	{
		InfoCacheArray::assign((const JustInTimeArray<T>&)kArray);
	}
};

class CvPromotionInfo;
template<typename> class PromotionArray;
class PromotionInfoArray : public InfoCacheArray
{
public:
	PromotionInfoArray() {}

	inline const CvPromotionInfo* get(int i) const
	{
		return (const CvPromotionInfo*)InfoCacheArray::get(i);
	}

	void assign(const BoolArray& kArray);
	template<typename T>
	void assign(const PromotionArray<T>& kArray)
	{
		InfoCacheArray::assign((const JustInTimeArray<T>&)kArray);
	}
};

class CvYieldInfo;
template<typename> class YieldArray;
class YieldInfoArray : public InfoCacheArray
{
public:
	YieldInfoArray() {}
	
	inline const CvYieldInfo* get(int i) const
	{
		return (const CvYieldInfo*)InfoCacheArray::get(i);
	}

	void assign(const BoolArray& kArray);
	template<typename T>
	void assign(const YieldArray<T>& kArray)
	{
		InfoCacheArray::assign((const JustInTimeArray<T>&)kArray);
	}
};

