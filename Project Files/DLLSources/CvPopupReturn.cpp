#include "CvGameCoreDLL.h"
#include "CvPopupReturn.h"

PopupReturn::PopupReturn()
{
}


void PopupReturn::setSelectedRadioButton(int iValue, int iGroup)
{
	m_aiSelectedRadioButton[iGroup] = iValue;
}

int PopupReturn::getSelectedRadioButton(int iGroup) const
{
	std::map<int, int>::const_iterator it = m_aiSelectedRadioButton.find(iGroup);
	if (it == m_aiSelectedRadioButton.end())
	{
		return -1;
	}

	return (*it).second;
}

void PopupReturn::setCheckboxBitfield(int iValue, int iGroup)
{
	m_aiBitField[iGroup] = iValue;
}

int PopupReturn::getCheckboxBitfield(int iGroup) const
{
	std::map<int, int>::const_iterator it = m_aiBitField.find(iGroup);
	if (it == m_aiBitField.end())
	{
		return -1;
	}

	return (*it).second;
}

void PopupReturn::setEditBoxString(const wchar* szValue, int iGroup)
{
	m_aszEditBoxString[iGroup] = szValue;
}

const wchar* PopupReturn::getEditBoxString(int iGroup) const
{
	std::map<int, CvWString>::const_iterator it = m_aszEditBoxString.find(iGroup);
	if (it == m_aszEditBoxString.end())
	{
		return NULL;
	}

	return (*it).second.GetCString();
}

void PopupReturn::setSpinnerWidgetValue(int iValue, int iGroup)
{
	m_aiSpinnerWidgetValues[iGroup] = iValue;
}

int PopupReturn::getSpinnerWidgetValue(int iGroup) const
{
	std::map<int, int>::const_iterator it = m_aiSpinBoxID.find(iGroup);
	if (it == m_aiSpinBoxID.end())
	{
		return -1;
	}

	return (*it).second;
}

void PopupReturn::setSelectedPulldownValue(int iValue, int iGroup)
{
	m_aiPulldownID[iGroup] = iValue;
}

int PopupReturn::getSelectedPullDownValue(int iGroup) const
{
	std::map<int, int>::const_iterator it = m_aiPulldownID.find(iGroup);
	if (it == m_aiPulldownID.end())
	{
		return -1;
	}

	return (*it).second;
}

void PopupReturn::setSelectedListBoxValue(int iValue, int iGroup)
{
	m_aiListBoxID[iGroup] = iValue;
}

int PopupReturn::getSelectedListBoxValue(int iGroup) const
{
	std::map<int, int>::const_iterator it = m_aiListBoxID.find(iGroup);
	if (it == m_aiListBoxID.end())
	{
		return -1;
	}

	return (*it).second;
}

void PopupReturn::setCurrentSpinBoxValue(int iValue, int iIndex)
{
	m_aiSpinBoxID[iIndex] = iValue;
}

int PopupReturn::getCurrentSpinBoxValue(int iIndex) const
{
	std::map<int, int>::const_iterator it = m_aiSpinBoxID.find(iIndex);
	if (it == m_aiSpinBoxID.end())
	{
		return -1;
	}

	return (*it).second;
}

void PopupReturn::setButtonClicked(int iValue, int iGroup)
{
	m_aiButtonID[iGroup] = iValue;
}

int PopupReturn::getButtonClicked(int iGroup) const
{
	std::map<int, int>::const_iterator it = m_aiButtonID.find(iGroup);
	if (it == m_aiButtonID.end())
	{
		return -1;
	}

	return (*it).second;
}

//
// read object from a stream
//
void PopupReturn::read(FDataStreamBase* pStream)
{
	uint iSize;
	int iValue;
	int iIndex;
	CvWString szValue;
	uint i;

	pStream->Read( &iSize );
	for (i = 0; i < iSize; i++)
	{
		pStream->Read( &iIndex );
		pStream->Read( &iValue );
		m_aiSelectedRadioButton[iIndex] = iValue;
	}

	pStream->Read( &iSize );
	for (i = 0; i < iSize; i++)
	{
		pStream->Read( &iIndex );
		pStream->Read( &iValue );
		m_aiBitField[iIndex] = iValue;
	}

	pStream->Read( &iSize );
	for (i = 0; i < iSize; i++)
	{
		pStream->Read( &iIndex );
		pStream->ReadString(szValue);
		m_aszEditBoxString[iIndex] = szValue;
	}

	pStream->Read( &iSize );
	for (i = 0; i < iSize; i++)
	{
		pStream->Read( &iIndex );
		pStream->Read( &iValue );
		m_aiSpinnerWidgetValues[iIndex] = iValue;
	}

	pStream->Read( &iSize );
	for (i = 0; i < iSize; i++)
	{
		pStream->Read( &iIndex );
		pStream->Read( &iValue );
		m_aiPulldownID[iIndex] = iValue;
	}

	pStream->Read( &iSize );
	for (i = 0; i < iSize; i++)
	{
		pStream->Read( &iIndex );
		pStream->Read( &iValue );
		m_aiListBoxID[iIndex] = iValue;
	}

	pStream->Read( &iSize );
	for (i = 0; i < iSize; i++)
	{
		pStream->Read( &iIndex );
		pStream->Read( &iValue );
		m_aiSpinBoxID[iIndex] = iValue;
	}

	pStream->Read( &iSize );
	for (i = 0; i < iSize; i++)
	{
		pStream->Read( &iIndex );
		pStream->Read( &iValue );
		m_aiButtonID[iIndex] = iValue;
	}
}

//
// write object to a stream
//
void PopupReturn::write(FDataStreamBase* pStream) const
{
	pStream->Write( m_aiSelectedRadioButton.size() );
	for (std::map<int, int>::const_iterator it = m_aiSelectedRadioButton.begin(); it != m_aiSelectedRadioButton.end(); ++it)
	{
		pStream->Write((*it).first);
		pStream->Write((*it).second);
	}

	pStream->Write( m_aiBitField.size() );
	for (std::map<int, int>::const_iterator it = m_aiBitField.begin(); it != m_aiBitField.end(); ++it)
	{
		pStream->Write((*it).first);
		pStream->Write((*it).second);
	}

	pStream->Write( m_aszEditBoxString.size() );
	for (std::map<int, CvWString>::const_iterator it = m_aszEditBoxString.begin(); it != m_aszEditBoxString.end(); ++it)
	{
		pStream->Write((*it).first);
		pStream->WriteString((*it).second);
	}

	pStream->Write( m_aiSpinnerWidgetValues.size() );
	for (std::map<int, int>::const_iterator it = m_aiSpinnerWidgetValues.begin(); it != m_aiSpinnerWidgetValues.end(); ++it)
	{
		pStream->Write((*it).first);
		pStream->Write((*it).second);
	}

	pStream->Write( m_aiPulldownID.size() );
	for (std::map<int, int>::const_iterator it = m_aiPulldownID.begin(); it != m_aiPulldownID.end(); ++it)
	{
		pStream->Write((*it).first);
		pStream->Write((*it).second);
	}

	pStream->Write( m_aiListBoxID.size() );
	for (std::map<int, int>::const_iterator it = m_aiListBoxID.begin(); it != m_aiListBoxID.end(); ++it)
	{
		pStream->Write((*it).first);
		pStream->Write((*it).second);
	}

	pStream->Write( m_aiSpinBoxID.size() );
	for (std::map<int, int>::const_iterator it = m_aiSpinBoxID.begin(); it != m_aiSpinBoxID.end(); ++it)
	{
		pStream->Write((*it).first);
		pStream->Write((*it).second);
	}

	pStream->Write( m_aiButtonID.size() );
	for (std::map<int, int>::const_iterator it = m_aiButtonID.begin(); it != m_aiButtonID.end(); ++it)
	{
		pStream->Write((*it).first);
		pStream->Write((*it).second);
	}
}
