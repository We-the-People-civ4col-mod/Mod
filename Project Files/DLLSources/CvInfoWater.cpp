//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvInfoWater.cpp
//
//  AUTHOR:
//
//
//
//  PURPOSE: The base class for all info classes to inherit from.
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvInfoWater.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvGlobals.h"

//======================================================================================================
//					CvWaterPlaneInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvWaterPlaneInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvWaterPlaneInfo::CvWaterPlaneInfo()
{

}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvWaterPlaneInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvWaterPlaneInfo::~CvWaterPlaneInfo()
{
}
//------------------------------------------------------------------------------------------------------
float CvWaterPlaneInfo::getMaterialAlpha() const		// The water plane's material alpha
{
	return m_fMaterialAlpha;
}
//------------------------------------------------------------------------------------------------------
const TCHAR * CvWaterPlaneInfo::getBaseTexture() const
{
	return m_szBaseTexture;
}
//------------------------------------------------------------------------------------------------------
const TCHAR * CvWaterPlaneInfo::getNormalTexture() const
{
	return m_szNormalTexture;
}
//------------------------------------------------------------------------------------------------------
const TCHAR * CvWaterPlaneInfo::getTransitionTexture() const
{
	return m_szTransitionTexture;
}
//------------------------------------------------------------------------------------------------------
const TCHAR * CvWaterPlaneInfo::getEnvironmentTexture() const
{
	return m_szEnvironmentTexture;
}
//------------------------------------------------------------------------------------------------------
const TCHAR * CvWaterPlaneInfo::getGridTexture() const
{
	return m_szGridTexture;
}
//------------------------------------------------------------------------------------------------------
float CvWaterPlaneInfo::getTextureScaling() const
{
	return m_BaseTextureScale;
}
//------------------------------------------------------------------------------------------------------
float CvWaterPlaneInfo::getTextureScrollRateU() const
{
	return m_fURate;
}
//------------------------------------------------------------------------------------------------------
float CvWaterPlaneInfo::getTextureScrollRateV() const
{
	return m_fVRate;
}
//------------------------------------------------------------------------------------------------------
bool CvWaterPlaneInfo::read(CvXMLLoadUtility* pXML)
{
	CvString  szTextVal;
	if (!CvInfoBase::read(pXML))
		return false;

	pXML->GetChildXmlValByName( &m_fMaterialAlpha, "MaterialAlpha");
	pXML->GetChildXmlValByName( &m_BaseTextureScale, "TextureScaling");
	pXML->GetChildXmlValByName( &m_fURate, "URate");
	pXML->GetChildXmlValByName( &m_fVRate, "VRate");
	pXML->GetChildXmlValByName( m_szBaseTexture, "WaterBaseTexture");
	pXML->GetChildXmlValByName( m_szNormalTexture, "WaterNormalTexture");
	pXML->GetChildXmlValByName( m_szTransitionTexture, "WaterTransitionTexture");
	pXML->GetChildXmlValByName( m_szEnvironmentTexture, "WaterEnvironmentTexture");
	pXML->GetChildXmlValByName( m_szGridTexture, "WaterGridTexture");

	return true;
}
//------------------------------------------------------------------------------------------------------

//======================================================================================================
//					CvTerrainPlaneInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTerrainPlaneInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTerrainPlaneInfo::CvTerrainPlaneInfo()
{

}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTerrainPlaneInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTerrainPlaneInfo::~CvTerrainPlaneInfo()
{
}
//------------------------------------------------------------------------------------------------------
bool CvTerrainPlaneInfo::isVisible() const
{
	return m_bVisible;
}
//------------------------------------------------------------------------------------------------------
bool CvTerrainPlaneInfo::isGroundPlane() const
{
	return m_bGroundPlane;
}
//------------------------------------------------------------------------------------------------------
bool CvTerrainPlaneInfo::isCitySelection() const
{
	return m_bCitySelection;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getMaterialAlpha() const		// The water plane's material alpha
{
	return m_fMaterialAlpha;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getCloseAlpha() const		// The water plane's material alpha
{
	return m_fCloseAlpha;
}
//------------------------------------------------------------------------------------------------------
const TCHAR * CvTerrainPlaneInfo::getBaseTexture() const
{
	return m_szBaseTexture;
}
//------------------------------------------------------------------------------------------------------
void CvTerrainPlaneInfo::setBaseTexture(const TCHAR* szVal)			// The filename of the base texture
{
	m_szBaseTexture=szVal;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getTextureScalingU() const
{
	return m_BaseTextureScaleU;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getTextureScalingV() const
{
	return m_BaseTextureScaleV;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getTextureScrollRateU() const
{
	return m_fURate;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getTextureScrollRateV() const
{
	return m_fVRate;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getZHeight() const
{
	return m_fZHeight;
}
//------------------------------------------------------------------------------------------------------
FogTypes CvTerrainPlaneInfo::getFogType() const
{
	return m_eFogType;
}
//------------------------------------------------------------------------------------------------------
bool CvTerrainPlaneInfo::read(CvXMLLoadUtility* pXML)
{
	CvString  szTextVal;
	if (!CvInfoBase::read(pXML))
		return false;

	pXML->GetChildXmlValByName( &m_bVisible, "bVisible");
	pXML->GetChildXmlValByName( &m_bGroundPlane, "bGroundPlane");
	pXML->GetChildXmlValByName( &m_bCitySelection, "bCitySelection");
	pXML->GetChildXmlValByName( &m_fMaterialAlpha, "MaterialAlpha");
	pXML->GetChildXmlValByName( &m_fCloseAlpha, "CloseAlpha");

	pXML->GetChildXmlValByName( szTextVal, "TextureFile");
	setBaseTexture(szTextVal);

	pXML->GetChildXmlValByName( &m_BaseTextureScaleU, "TextureScalingU");
	pXML->GetChildXmlValByName( &m_BaseTextureScaleV, "TextureScalingV");
	pXML->GetChildXmlValByName( &m_fURate, "URate");
	pXML->GetChildXmlValByName( &m_fVRate, "VRate");
	pXML->GetChildXmlValByName( &m_fZHeight, "ZHeight");

	pXML->GetChildXmlValByName( szTextVal, "FogType");
	if(szTextVal.CompareNoCase("FOG_TYPE_NONE") == 0)
		m_eFogType = FOG_TYPE_NONE;
	else if(szTextVal.CompareNoCase("FOG_TYPE_PARALLEL") == 0)
		m_eFogType = FOG_TYPE_PARALLEL;
	else if(szTextVal.CompareNoCase("FOG_TYPE_PROJECTED") == 0)
		m_eFogType = FOG_TYPE_PROJECTED;
	else
	{
		FAssertMsg(false, "[Jason] Unknown fog type.");
		m_eFogType = FOG_TYPE_NONE;
	}

	return true;
}
//------------------------------------------------------------------------------------------------------

//======================================================================================================
//					CvCameraOverlayInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCameraOverlayInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCameraOverlayInfo::CvCameraOverlayInfo()
{

}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCameraOverlayInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCameraOverlayInfo::~CvCameraOverlayInfo()
{
}
//------------------------------------------------------------------------------------------------------
bool CvCameraOverlayInfo::isVisible() const
{
	return m_bVisible;
}
//------------------------------------------------------------------------------------------------------
const TCHAR * CvCameraOverlayInfo::getBaseTexture() const
{
	return m_szBaseTexture;
}
//------------------------------------------------------------------------------------------------------
void CvCameraOverlayInfo::setBaseTexture(const TCHAR* szVal)			// The filename of the base texture
{
	m_szBaseTexture = szVal;
}
//------------------------------------------------------------------------------------------------------
CameraOverlayTypes CvCameraOverlayInfo::getCameraOverlayType() const
{
	return m_eCameraOverlayType;
}
//------------------------------------------------------------------------------------------------------
bool CvCameraOverlayInfo::read(CvXMLLoadUtility* pXML)
{
	CvString  szTextVal;
	if (!CvInfoBase::read(pXML))
		return false;

	pXML->GetChildXmlValByName( &m_bVisible, "bVisible");

	pXML->GetChildXmlValByName( szTextVal, "TextureFile");
	setBaseTexture(szTextVal);

	pXML->GetChildXmlValByName( szTextVal, "CameraOverlayType");
	if(szTextVal.CompareNoCase("CAMERA_OVERLAY_DECAL") == 0)
		m_eCameraOverlayType = CAMERA_OVERLAY_DECAL;
	else if(szTextVal.CompareNoCase("CAMERA_OVERLAY_ADDITIVE") == 0)
		m_eCameraOverlayType = CAMERA_OVERLAY_ADDITIVE;
	else
	{
		FAssertMsg(false, "[Jason] Unknown camera overlay type.");
		m_eCameraOverlayType = CAMERA_OVERLAY_DECAL;
	}

	return true;
}
//------------------------------------------------------------------------------------------------------
