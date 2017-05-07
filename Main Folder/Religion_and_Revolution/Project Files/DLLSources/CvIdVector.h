#ifndef __CV_ID_VECTOR_H
#define __CV_ID_VECTOR_H

template<class T>
class CvIdVector : public stdext::hash_map<int, T*>
{
public:
	CvIdVector();
	~CvIdVector();

	void reset();

	T* addNew();
	T* getById(int iId) const;
	bool removeById(int iId);

	void Read(FDataStreamBase* pStream);
	void Write(FDataStreamBase* pStream) const;

protected:
	int m_iNextId;
};


template <class T>
CvIdVector<T>::CvIdVector() :
m_iNextId(0)
{
	reset();
}

template <class T>
CvIdVector<T>::~CvIdVector()
{
	reset();
}

template <class T>
void CvIdVector<T>::reset()
{
	m_iNextId = 0;

	CvIdVector<T>::iterator it;
	for (it = begin(); it != end(); ++it)
	{
		SAFE_DELETE(it->second);
	}

	clear();
}


template <class T>
T* CvIdVector<T>::addNew()
{
	T* pNew = new T();

	pNew->setID(m_iNextId);
	++m_iNextId;

	(*this)[pNew->getID()] = pNew;

	return pNew;
}

template <class T>
T* CvIdVector<T>::getById(int iId) const
{
	CvIdVector<T>::const_iterator it = find(iId);
	if (it != end())
	{
		return it->second;
	}

	return NULL;
}

template <class T>
bool CvIdVector<T>::removeById(int iId)
{
	CvIdVector<T>::iterator it = find(iId);
	if (it != end())
	{
		SAFE_DELETE(it->second);
		erase(it);
		return true;
	}

	return false;
}

template <class T>
void CvIdVector<T>::Read(FDataStreamBase* pStream)
{
	reset();

	pStream->Read(&m_iNextId);

	int iCount;
	pStream->Read(&iCount);

	for (int i = 0; i < iCount; ++i)
	{
		T* pData = new T;

		pData->read(pStream);

		(*this)[pData->getID()] = pData;
	}
}

template <class T>
void CvIdVector<T>::Write(FDataStreamBase* pStream) const
{
	pStream->Write(m_iNextId);
	pStream->Write(static_cast<int>(size()));

	CvIdVector<T>::const_iterator it;
	for (it = begin(); it != end(); ++it)
	{
		it->second->write(pStream);
	}
}

#endif