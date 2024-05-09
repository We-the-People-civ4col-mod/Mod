#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvInfosWater.h
//
//  AUTHOR:	tomw
//
//
//  PURPOSE: All Civ4 info classes and the base class for them
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef CV_WATERINFO_H
#define CV_WATERINFO_H

#include "CvInfos.h"
#include "CvEnums.h"

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvXMLLoadUtility;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvWaterPlaneInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvWaterPlaneInfo :public CvInfoBase
{
public:

	DllExport CvWaterPlaneInfo();
	DllExport virtual ~CvWaterPlaneInfo();

	DllExport float getMaterialAlpha() const;				// The water plane's material alpha

	DllExport float getTextureScaling() const;				// The water plane's texture scale
	DllExport float getTextureScrollRateU() const;			// The water plane's texture scroll rate in U
	DllExport float getTextureScrollRateV() const;			// The water plane's texture scroll rate in V

	DllExport char const* getBaseTexture() const;
	DllExport char const* getNormalTexture() const;
	DllExport char const* getTransitionTexture() const;
	DllExport char const* getEnvironmentTexture() const;
	DllExport char const* getGridTexture() const;

	DllExport bool read(CvXMLLoadUtility*);

protected:

	float m_fMaterialAlpha;			// The water plane's material alpha

	CvString m_szBaseTexture;		// The filename of the base texture
	CvString m_szNormalTexture;		// The filename of the normal texture
	CvString m_szTransitionTexture;	// The filename of the transition texture
	CvString m_szEnvironmentTexture;	// The filename of the transition texture
	CvString m_szGridTexture;	// The filename of the transition texture

	float m_BaseTextureScale;		// Texture scaling
	float m_fURate;					// Texture scroll rate
	float m_fVRate;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTerrainPlaneInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTerrainPlaneInfo :public CvInfoBase
{
public:

	DllExport CvTerrainPlaneInfo();
	DllExport virtual ~CvTerrainPlaneInfo();

	DllExport bool isVisible() const;				// The terrain plane's material alpha
	DllExport bool isGroundPlane() const;				// The terrain plane's material alpha
	DllExport bool isCitySelection() const;				// The terrain plane's material alpha
	DllExport float getMaterialAlpha() const;				// The terrain plane's material alpha
	DllExport float getCloseAlpha() const;				// The terrain plane's material alpha

	DllExport float getTextureScalingU() const;				// The terrain plane's texture scale
	DllExport float getTextureScalingV() const;				// The terrain plane's texture scale
	DllExport float getTextureScrollRateU() const;			// The terrain plane's texture scroll rate in U
	DllExport float getTextureScrollRateV() const;			// The terrain plane's texture scroll rate in V
	DllExport float getZHeight() const;						// The terrain plane's z height in world units
	DllExport FogTypes getFogType() const;

	DllExport char const* getBaseTexture() const;
	DllExport void setBaseTexture(char const* szVal);		// The filename of the base texture

	DllExport bool read(CvXMLLoadUtility*);

protected:

	bool m_bVisible;
	bool m_bGroundPlane;
	bool m_bCitySelection;
	float m_fMaterialAlpha;			// The terrain plane's material alpha
	float m_fCloseAlpha;

	CvString m_szBaseTexture;		// The filename of the base texture

	float m_BaseTextureScaleU;		// Texture scaling
	float m_BaseTextureScaleV;		// Texture scaling
	float m_fURate;					// Texture scroll rate
	float m_fVRate;
	float m_fZHeight;
	FogTypes m_eFogType;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTerrainPlaneInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCameraOverlayInfo :public CvInfoBase
{
public:

	DllExport CvCameraOverlayInfo();
	DllExport virtual ~CvCameraOverlayInfo();

	DllExport bool isVisible() const;				// The terrain plane's material alpha
	DllExport CameraOverlayTypes getCameraOverlayType() const;

	DllExport char const* getBaseTexture() const;
	DllExport void setBaseTexture(char const* szVal);		// The filename of the base texture

	DllExport bool read(CvXMLLoadUtility*);

protected:

	bool m_bVisible;
	CvString m_szBaseTexture;		// The filename of the base texture
	CameraOverlayTypes m_eCameraOverlayType;
};


#endif
