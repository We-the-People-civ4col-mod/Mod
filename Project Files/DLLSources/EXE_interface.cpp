//
// EXE interface
// This file is used to contain all the functions the exe can call in the DLL and redirect them to whatever needs to be called in the DL.
//
// The benefits of doing this is that the exe relies on functions staying as they were in vanilla.
// Using this file will let the functions stay as vanilla while we can mod the DLL code, like making functions type strict or alter arguments.
// Also if we alter a function, which renders it exe incompatible, if the call goes through this file it will be failure to compile instead of runtime crash.
//
//
// Steps for adding a function:
// 1: find the function in the comments below
// 2: add the function there as it is written in vanilla, including DllExport
// 3: write a body for the call, usually a function call to the parent class
// 4: copy paste the redirect string to CvGameCoreDLL.def to let the DLL linking know the call should go to this file
//      no need to understand the redirect string. They are copy pasted from what the exe looks for.
//      the weird string part tells how many arguments and stuff like that. It just has to stay to match exe expectations.
//
// Note: once a function is added here it will cause a compile error if it still has DllExport in the header file
//       this is due to our custom DllExport check, which won't accept anything, which might cause the exe to crash
//

// WARNING: As of now (2023-04-16) there is a bug in the compile script:
// The function here have to be indented with exactly one tab, otherwise the script will complain
// about DLLExport errors

#include "CvGameCoreDLL.h"
#include "CvDiploParameters.h"
#include "CvDLLButtonPopup.h"
#include "CvDLLWidgetData.h"
#include "CvFractal.h"
#include "CvGameTextMgr.h"
#include "CvInfoWater.h"
#include "CvInitCore.h"
#include "CvMapGenerator.h"
#include "CvPopupReturn.h"
#include "CvReplayInfo.h"
#include "CvXMLLoadUtility.h"

#include "CyArgsList.h"
#include "CyPlot.h"

#include "SavegameConstants.h"


class EXE_CvActionInfo : public CvActionInfo
{
public:
	/*
		getHotKeyPriority
			?getHotKeyPriority@CvActionInfo@@QBEHXZ=?getHotKeyPriority@EXE_CvActionInfo@@QBEHXZ

		getHotKeyPriorityAlt
			?getHotKeyPriorityAlt@CvActionInfo@@QBEHXZ=?getHotKeyPriorityAlt@EXE_CvActionInfo@@QBEHXZ

		getHotKeyVal
			?getHotKeyVal@CvActionInfo@@QBEHXZ=?getHotKeyVal@EXE_CvActionInfo@@QBEHXZ

		getHotKeyValAlt
			?getHotKeyValAlt@CvActionInfo@@QBEHXZ=?getHotKeyValAlt@EXE_CvActionInfo@@QBEHXZ

		getSubType
			?getSubType@CvActionInfo@@QBE?AW4ActionSubTypes@@XZ=?getSubType@EXE_CvActionInfo@@QBE?AW4ActionSubTypes@@XZ

		isAltDown
			?isAltDown@CvActionInfo@@QBE_NXZ=?isAltDown@EXE_CvActionInfo@@QBE_NXZ

		isAltDownAlt
			?isAltDownAlt@CvActionInfo@@QBE_NXZ=?isAltDownAlt@EXE_CvActionInfo@@QBE_NXZ

		isCtrlDown
			?isCtrlDown@CvActionInfo@@QBE_NXZ=?isCtrlDown@EXE_CvActionInfo@@QBE_NXZ

		isCtrlDownAlt
			?isCtrlDownAlt@CvActionInfo@@QBE_NXZ=?isCtrlDownAlt@EXE_CvActionInfo@@QBE_NXZ

		isShiftDown
			?isShiftDown@CvActionInfo@@QBE_NXZ=?isShiftDown@EXE_CvActionInfo@@QBE_NXZ

		isShiftDownAlt
			?isShiftDownAlt@CvActionInfo@@QBE_NXZ=?isShiftDownAlt@EXE_CvActionInfo@@QBE_NXZ

		isVisible
			?isVisible@CvActionInfo@@QBE_NXZ=?isVisible@EXE_CvActionInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvActionInfo) == sizeof(CvActionInfo));


class EXE_CvAnimationCategoryInfo : public CvAnimationCategoryInfo
{
public:
	/*
		getCategoryBaseID
			?getCategoryBaseID@CvAnimationCategoryInfo@@QAEHXZ=?getCategoryBaseID@EXE_CvAnimationCategoryInfo@@QAEHXZ

		getCategoryDefaultTo
			?getCategoryDefaultTo@CvAnimationCategoryInfo@@QAEHXZ=?getCategoryDefaultTo@EXE_CvAnimationCategoryInfo@@QAEHXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvAnimationCategoryInfo) == sizeof(CvAnimationCategoryInfo));


class EXE_CvAnimationPathInfo : public CvAnimationPathInfo
{
public:
	/*
		getNumPathDefinitions
			?getNumPathDefinitions@CvAnimationPathInfo@@QAEHXZ=?getNumPathDefinitions@EXE_CvAnimationPathInfo@@QAEHXZ

		getPath
			?getPath@CvAnimationPathInfo@@QAEPAV?$vector@U?$pair@HM@std@@V?$allocator@U?$pair@HM@std@@@2@@std@@XZ=?getPath@EXE_CvAnimationPathInfo@@QAEPAV?$vector@U?$pair@HM@std@@V?$allocator@U?$pair@HM@std@@@2@@std@@XZ

		getPathCategory
			?getPathCategory@CvAnimationPathInfo@@QAEHH@Z=?getPathCategory@EXE_CvAnimationPathInfo@@QAEHH@Z

		isMissionPath
			?isMissionPath@CvAnimationPathInfo@@QBE_NXZ=?isMissionPath@EXE_CvAnimationPathInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvAnimationPathInfo) == sizeof(CvAnimationPathInfo));


class EXE_CvArtFileMgr : public CvArtFileMgr
{
public:
	/*
		DeInit
			?DeInit@CvArtFileMgr@@QAEXXZ=?DeInit@EXE_CvArtFileMgr@@QAEXXZ

		GetInstance
			?GetInstance@CvArtFileMgr@@SAAAV1@XZ=?GetInstance@EXE_CvArtFileMgr@@SAAAV1@XZ

		Init
			?Init@CvArtFileMgr@@QAEXXZ=?Init@EXE_CvArtFileMgr@@QAEXXZ

		Reset
			?Reset@CvArtFileMgr@@QAEXXZ=?Reset@EXE_CvArtFileMgr@@QAEXXZ

		buildArtFileInfoMaps
			?buildArtFileInfoMaps@CvArtFileMgr@@QAEXXZ=?buildArtFileInfoMaps@EXE_CvArtFileMgr@@QAEXXZ

		getInterfaceArtInfo
			?getInterfaceArtInfo@CvArtFileMgr@@QBEPAVCvArtInfoInterface@@PBD@Z=?getInterfaceArtInfo@EXE_CvArtFileMgr@@QBEPAVCvArtInfoInterface@@PBD@Z

		getMiscArtInfo
			?getMiscArtInfo@CvArtFileMgr@@QBEPAVCvArtInfoMisc@@PBD@Z=?getMiscArtInfo@EXE_CvArtFileMgr@@QBEPAVCvArtInfoMisc@@PBD@Z

		getTerrainArtInfo
			?getTerrainArtInfo@CvArtFileMgr@@QBEPAVCvArtInfoTerrain@@PBD@Z=?getTerrainArtInfo@EXE_CvArtFileMgr@@QBEPAVCvArtInfoTerrain@@PBD@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvArtFileMgr) == sizeof(CvArtFileMgr));


class EXE_CvArtInfoAsset : public CvArtInfoAsset
{
public:
	DllExport const char* getNIF() const
	{
		if (GLOBAL_DEFINE_USE_NIF_LOGGING)
		{
			CvString szError;
			szError.Format("Opening nif for entry: %s", getTag());
			gDLL->logMsg("resmgr.log", szError);
		}

		return CvArtInfoAsset::getNIF();
	}

	DllExport const char* getKFM() const
	{
		return CvArtInfoAsset::getKFM();
	}
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvArtInfoAsset) == sizeof(CvArtInfoAsset));


class EXE_CvArtInfoBonus : public CvArtInfoBonus
{
public:
	/*
		getFontButtonIndex
			?getFontButtonIndex@CvArtInfoBonus@@QBEHXZ=?getFontButtonIndex@EXE_CvArtInfoBonus@@QBEHXZ

		isRefractionCastor
			?isRefractionCastor@CvArtInfoBonus@@QBE_NXZ=?isRefractionCastor@EXE_CvArtInfoBonus@@QBE_NXZ

		isShadowCastor
			?isShadowCastor@CvArtInfoBonus@@QBE_NXZ=?isShadowCastor@EXE_CvArtInfoBonus@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvArtInfoBonus) == sizeof(CvArtInfoBonus));


class EXE_CvArtInfoBuilding : public CvArtInfoBuilding
{
public:
	/*
		getLSystemName
			?getLSystemName@CvArtInfoBuilding@@QBEPBDXZ=?getLSystemName@EXE_CvArtInfoBuilding@@QBEPBDXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvArtInfoBuilding) == sizeof(CvArtInfoBuilding));


class EXE_CvArtInfoCivilization : public CvArtInfoCivilization
{
public:
	/*
		getFontButtonIndex
			?getFontButtonIndex@CvArtInfoCivilization@@QBEHXZ=?getFontButtonIndex@EXE_CvArtInfoCivilization@@QBEHXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvArtInfoCivilization) == sizeof(CvArtInfoCivilization));


class EXE_CvArtInfoFeature : public CvArtInfoFeature
{
public:
	/*
		getNumVarieties
			?getNumVarieties@CvArtInfoFeature@@QBEHXZ=?getNumVarieties@EXE_CvArtInfoFeature@@QBEHXZ

		getTileArtType
			?getTileArtType@CvArtInfoFeature@@QBE?AW4TileArtTypes@@XZ=?getTileArtType@EXE_CvArtInfoFeature@@QBE?AW4TileArtTypes@@XZ

		getVariety
			?getVariety@CvArtInfoFeature@@QBEABVFeatureVariety@1@H@Z=?getVariety@EXE_CvArtInfoFeature@@QBEABVFeatureVariety@1@H@Z

		isAnimated
			?isAnimated@CvArtInfoFeature@@QBE_NXZ=?isAnimated@EXE_CvArtInfoFeature@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvArtInfoFeature) == sizeof(CvArtInfoFeature));


class EXE_CvArtInfoLeaderhead : public CvArtInfoLeaderhead
{
public:
	/*
		getBackgroundKFM
			?getBackgroundKFM@CvArtInfoLeaderhead@@QBEPBDXZ=?getBackgroundKFM@EXE_CvArtInfoLeaderhead@@QBEPBDXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvArtInfoLeaderhead) == sizeof(CvArtInfoLeaderhead));


class EXE_CvArtInfoTerrain : public CvArtInfoTerrain
{
public:
	/*
		getBaseTexture
			?getBaseTexture@CvArtInfoTerrain@@QAEPBDXZ=?getBaseTexture@EXE_CvArtInfoTerrain@@QAEPBDXZ

		getBlendList
			?getBlendList@CvArtInfoTerrain@@QAEAAV?$vector@U?$pair@HH@std@@V?$allocator@U?$pair@HH@std@@@2@@std@@H@Z=?getBlendList@EXE_CvArtInfoTerrain@@QAEAAV?$vector@U?$pair@HH@std@@V?$allocator@U?$pair@HH@std@@@2@@std@@H@Z

		getDetailTexture
			?getDetailTexture@CvArtInfoTerrain@@QAEPBDXZ=?getDetailTexture@EXE_CvArtInfoTerrain@@QAEPBDXZ

		getGridTexture
			?getGridTexture@CvArtInfoTerrain@@QAEPBDXZ=?getGridTexture@EXE_CvArtInfoTerrain@@QAEPBDXZ

		getLayerOrder
			?getLayerOrder@CvArtInfoTerrain@@QBEHXZ=?getLayerOrder@EXE_CvArtInfoTerrain@@QBEHXZ

		getTerrainGroup
			?getTerrainGroup@CvArtInfoTerrain@@QBE?AW4TerrainGroupTypes@@XZ=?getTerrainGroup@EXE_CvArtInfoTerrain@@QBE?AW4TerrainGroupTypes@@XZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvArtInfoTerrain) == sizeof(CvArtInfoTerrain));


class EXE_CvArtInfoUnit : public CvArtInfoUnit
{
public:
	/*
		getActAsRanged
			?getActAsRanged@CvArtInfoUnit@@QBE_NXZ=?getActAsRanged@EXE_CvArtInfoUnit@@QBE_NXZ

		getBankRate
			?getBankRate@CvArtInfoUnit@@QBEMXZ=?getBankRate@EXE_CvArtInfoUnit@@QBEMXZ

		getBattleDistance
			?getBattleDistance@CvArtInfoUnit@@QBEMXZ=?getBattleDistance@EXE_CvArtInfoUnit@@QBEMXZ

		getCombatExempt
			?getCombatExempt@CvArtInfoUnit@@QBE_NXZ=?getCombatExempt@EXE_CvArtInfoUnit@@QBE_NXZ

		getDamageStates
			?getDamageStates@CvArtInfoUnit@@QBEHXZ=?getDamageStates@EXE_CvArtInfoUnit@@QBEHXZ

		getExchangeAngle
			?getExchangeAngle@CvArtInfoUnit@@QBEMXZ=?getExchangeAngle@EXE_CvArtInfoUnit@@QBEMXZ

		getRangedDeathTime
			?getRangedDeathTime@CvArtInfoUnit@@QBEMXZ=?getRangedDeathTime@EXE_CvArtInfoUnit@@QBEMXZ

		getRunEndSoundTag
			?getRunEndSoundTag@CvArtInfoUnit@@QBEHXZ=?getRunEndSoundTag@EXE_CvArtInfoUnit@@QBEHXZ

		getRunLoopSoundTag
			?getRunLoopSoundTag@CvArtInfoUnit@@QBEHXZ=?getRunLoopSoundTag@EXE_CvArtInfoUnit@@QBEHXZ

		getSelectionSoundScriptId
			?getSelectionSoundScriptId@CvArtInfoUnit@@QBEHXZ=?getSelectionSoundScriptId@EXE_CvArtInfoUnit@@QBEHXZ

		getTrailFadeFalloff
			?getTrailFadeFalloff@CvArtInfoUnit@@QBEMXZ=?getTrailFadeFalloff@EXE_CvArtInfoUnit@@QBEMXZ

		getTrailFadeStarTime
			?getTrailFadeStarTime@CvArtInfoUnit@@QBEMXZ=?getTrailFadeStarTime@EXE_CvArtInfoUnit@@QBEMXZ

		getTrailLength
			?getTrailLength@CvArtInfoUnit@@QBEMXZ=?getTrailLength@EXE_CvArtInfoUnit@@QBEMXZ

		getTrailTaper
			?getTrailTaper@CvArtInfoUnit@@QBEMXZ=?getTrailTaper@EXE_CvArtInfoUnit@@QBEMXZ

		getTrailTexture
			?getTrailTexture@CvArtInfoUnit@@QBEPBDXZ=?getTrailTexture@EXE_CvArtInfoUnit@@QBEPBDXZ

		getTrailWidth
			?getTrailWidth@CvArtInfoUnit@@QBEMXZ=?getTrailWidth@EXE_CvArtInfoUnit@@QBEMXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvArtInfoUnit) == sizeof(CvArtInfoUnit));


class EXE_CvAssetInfoBase : public CvAssetInfoBase
{
public:
	/*
		getPath
			?getPath@CvAssetInfoBase@@QBEPBDXZ=?getPath@EXE_CvAssetInfoBase@@QBEPBDXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvAssetInfoBase) == sizeof(CvAssetInfoBase));


class EXE_CvAttachableInfo : public CvAttachableInfo
{
public:
	/*
		getPath
			?getPath@CvAttachableInfo@@QBEPBDXZ=?getPath@EXE_CvAttachableInfo@@QBEPBDXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvAttachableInfo) == sizeof(CvAttachableInfo));


class EXE_CvBattleDefinition : public CvBattleDefinition
{
public:
	/*
		addBattleRound
			?addBattleRound@CvBattleDefinition@@QAEXABVCvBattleRound@@@Z=?addBattleRound@EXE_CvBattleDefinition@@QAEXABVCvBattleRound@@@Z

		clearBattleRounds
			?clearBattleRounds@CvBattleDefinition@@QAEXXZ=?clearBattleRounds@EXE_CvBattleDefinition@@QAEXXZ

		getBattleRound
			?getBattleRound@CvBattleDefinition@@QAEAAVCvBattleRound@@H@Z=?getBattleRound@EXE_CvBattleDefinition@@QAEAAVCvBattleRound@@H@Z

		getBattleRound
			?getBattleRound@CvBattleDefinition@@QBEABVCvBattleRound@@H@Z=?getBattleRound@EXE_CvBattleDefinition@@QBEABVCvBattleRound@@H@Z

		getDamage
			?getDamage@CvBattleDefinition@@QBEHW4BattleUnitTypes@@W4BattleTimeTypes@@@Z=?getDamage@EXE_CvBattleDefinition@@QBEHW4BattleUnitTypes@@W4BattleTimeTypes@@@Z

		getNumBattleRounds
			?getNumBattleRounds@CvBattleDefinition@@QBEHXZ=?getNumBattleRounds@EXE_CvBattleDefinition@@QBEHXZ

		isAdvanceSquare
			?isAdvanceSquare@CvBattleDefinition@@QBE_NXZ=?isAdvanceSquare@EXE_CvBattleDefinition@@QBE_NXZ

		isOneStrike
			?isOneStrike@CvBattleDefinition@@QBE_NXZ=?isOneStrike@EXE_CvBattleDefinition@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvBattleDefinition) == sizeof(CvBattleDefinition));


class EXE_CvBattleRound : public CvBattleRound
{
public:
	/*
		getNumAlive
			?getNumAlive@CvBattleRound@@QBEHW4BattleUnitTypes@@@Z=?getNumAlive@EXE_CvBattleRound@@QBEHW4BattleUnitTypes@@@Z

		getNumKilled
			?getNumKilled@CvBattleRound@@QBEHW4BattleUnitTypes@@@Z=?getNumKilled@EXE_CvBattleRound@@QBEHW4BattleUnitTypes@@@Z

		getWaveSize
			?getWaveSize@CvBattleRound@@QBEHXZ=?getWaveSize@EXE_CvBattleRound@@QBEHXZ

		isRangedRound
			?isRangedRound@CvBattleRound@@QBE_NXZ=?isRangedRound@EXE_CvBattleRound@@QBE_NXZ

		setNumKilled
			?setNumKilled@CvBattleRound@@QAEXW4BattleUnitTypes@@H@Z=?setNumKilled@EXE_CvBattleRound@@QAEXW4BattleUnitTypes@@H@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvBattleRound) == sizeof(CvBattleRound));


class EXE_CvBonusInfo : public CvBonusInfo
{
public:
	/*
		getArtInfo
			?getArtInfo@CvBonusInfo@@QBEPBVCvArtInfoBonus@@XZ=?getArtInfo@EXE_CvBonusInfo@@QBEPBVCvArtInfoBonus@@XZ

		getChar
			?getChar@CvBonusInfo@@QBEHXZ=?getChar@EXE_CvBonusInfo@@QBEHXZ

		isFeature
			?isFeature@CvBonusInfo@@QBE_NH@Z=?isFeature@EXE_CvBonusInfo@@QBE_NH@Z

		isTerrain
			?isTerrain@CvBonusInfo@@QBE_NH@Z=?isTerrain@EXE_CvBonusInfo@@QBE_NH@Z

		setChar
			?setChar@CvBonusInfo@@QAEXH@Z=?setChar@EXE_CvBonusInfo@@QAEXH@Z

		useLSystem
			?useLSystem@CvBonusInfo@@QBE_NXZ=?useLSystem@EXE_CvBonusInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvBonusInfo) == sizeof(CvBonusInfo));


class EXE_CvBuildInfo : public CvBuildInfo
{
public:
	/*
		getEntityEvent
			?getEntityEvent@CvBuildInfo@@QBEHXZ=?getEntityEvent@EXE_CvBuildInfo@@QBEHXZ

		getMissionType
			?getMissionType@CvBuildInfo@@QBEHXZ=?getMissionType@EXE_CvBuildInfo@@QBEHXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvBuildInfo) == sizeof(CvBuildInfo));


class EXE_CvBuildingInfo : public CvBuildingInfo
{
public:
	/*
		getArtInfo
			?getArtInfo@CvBuildingInfo@@QBEPBVCvArtInfoBuilding@@XZ=?getArtInfo@EXE_CvBuildingInfo@@QBEPBVCvArtInfoBuilding@@XZ

		getBuildingClassType
			?getBuildingClassType@CvBuildingInfo@@QBEHXZ=?getBuildingClassType@EXE_CvBuildingInfo@@QBEHXZ

		getFreeStartEra
			?getFreeStartEra@CvBuildingInfo@@QBEHXZ=?getFreeStartEra@EXE_CvBuildingInfo@@QBEHXZ

		getVisibilityPriority
			?getVisibilityPriority@CvBuildingInfo@@QBEMXZ=?getVisibilityPriority@EXE_CvBuildingInfo@@QBEMXZ

		isWater
			?isWater@CvBuildingInfo@@QBE_NXZ=?isWater@EXE_CvBuildingInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvBuildingInfo) == sizeof(CvBuildingInfo));


class EXE_CvCameraOverlayInfo : public CvCameraOverlayInfo
{
public:
	/*
		getBaseTexture
			?getBaseTexture@CvCameraOverlayInfo@@QBEPBDXZ=?getBaseTexture@EXE_CvCameraOverlayInfo@@QBEPBDXZ

		getCameraOverlayType
			?getCameraOverlayType@CvCameraOverlayInfo@@QBE?AW4CameraOverlayTypes@@XZ=?getCameraOverlayType@EXE_CvCameraOverlayInfo@@QBE?AW4CameraOverlayTypes@@XZ

		isVisible
			?isVisible@CvCameraOverlayInfo@@QBE_NXZ=?isVisible@EXE_CvCameraOverlayInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvCameraOverlayInfo) == sizeof(CvCameraOverlayInfo));


class EXE_CvCity : public CvCity
{
public:
	/*
		area
			?area@CvCity@@QBEPAVCvArea@@XZ=?area@EXE_CvCity@@QBEPAVCvArea@@XZ

		canBeSelected
			?canBeSelected@CvCity@@QBE_NXZ=?canBeSelected@EXE_CvCity@@QBE_NXZ

		canConstruct
			?canConstruct@CvCity@@QBE_NW4BuildingTypes@@_N11@Z=?canConstruct@EXE_CvCity@@QBE_NW4BuildingTypes@@_N11@Z

		canConvince
			?canConvince@CvCity@@QBE_NW4FatherPointTypes@@_N1@Z=?canConvince@EXE_CvCity@@QBE_NW4FatherPointTypes@@_N1@Z

		canHurry
			?canHurry@CvCity@@QBE_NW4HurryTypes@@_N@Z=?canHurry@EXE_CvCity@@QBE_NW4HurryTypes@@_N@Z

		canTrain
			?canTrain@CvCity@@QBE_NW4UnitTypes@@_N111@Z=?canTrain@EXE_CvCity@@QBE_NW4UnitTypes@@_N111@Z

		chooseProduction
			?chooseProduction@CvCity@@QAEXW4UnitTypes@@W4BuildingTypes@@_N2@Z=?chooseProduction@EXE_CvCity@@QAEXW4UnitTypes@@W4BuildingTypes@@_N2@Z

		doTask
			?doTask@CvCity@@QAEXW4TaskTypes@@HH_N111@Z=?doTask@EXE_CvCity@@QAEXW4TaskTypes@@HH_N111@Z

		getCityBillboardBottomBarValues
			?getCityBillboardBottomBarValues@CvCity@@QBE_NAAM00@Z=?getCityBillboardBottomBarValues@EXE_CvCity@@QBE_NAAM00@Z

		getCityBillboardProductionIcon
			?getCityBillboardProductionIcon@CvCity@@QBEPBDXZ=?getCityBillboardProductionIcon@EXE_CvCity@@QBEPBDXZ

		getCityBillboardSizeIconColors
			?getCityBillboardSizeIconColors@CvCity@@QBEXAAVNiColorA@@0@Z=?getCityBillboardSizeIconColors@EXE_CvCity@@QBEXAAVNiColorA@@0@Z

		getCityBillboardTopBarValues
			?getCityBillboardTopBarValues@CvCity@@QBE_NAAM00@Z=?getCityBillboardTopBarValues@EXE_CvCity@@QBE_NAAM00@Z

		getCityPlotIndex
			?getCityPlotIndex@CvCity@@QBEHPBVCvPlot@@@Z=?getCityPlotIndex@EXE_CvCity@@QBEHPBVCvPlot@@@Z

		getCitySizeType
			?getCitySizeType@CvCity@@QBE?AW4CitySizeTypes@@XZ=?getCitySizeType@EXE_CvCity@@QBE?AW4CitySizeTypes@@XZ

		getCivilizationType
			?getCivilizationType@CvCity@@QBE?AW4CivilizationTypes@@XZ=?getCivilizationType@EXE_CvCity@@QBE?AW4CivilizationTypes@@XZ

		getID
			?getID@CvCity@@QBEHXZ=?getID@EXE_CvCity@@QBEHXZ

		getIDInfo
			?getIDInfo@CvCity@@QBE?AUIDInfo@@XZ=?getIDInfo@EXE_CvCity@@QBE?AUIDInfo@@XZ

		getName
			?getName@CvCity@@QBE?BVCvWString@@I@Z=?getName@EXE_CvCity@@QBE?BVCvWString@@I@Z

		getNameKey
			?getNameKey@CvCity@@QBEPBGXZ=?getNameKey@EXE_CvCity@@QBEPBGXZ

		getOwner
			?getOwner@CvCity@@QBE?AW4PlayerTypes@@XZ=?getOwner@EXE_CvCity@@QBE?AW4PlayerTypes@@XZ

		getProductionTurnsLeft
			?getProductionTurnsLeft@CvCity@@QBEHW4BuildingTypes@@H@Z=?getProductionTurnsLeft@EXE_CvCity@@QBEHW4BuildingTypes@@H@Z

		getProductionTurnsLeft
			?getProductionTurnsLeft@CvCity@@QBEHW4UnitTypes@@H@Z=?getProductionTurnsLeft@EXE_CvCity@@QBEHW4UnitTypes@@H@Z

		getTeam
			?getTeam@CvCity@@QBE?AW4TeamTypes@@XZ=?getTeam@EXE_CvCity@@QBE?AW4TeamTypes@@XZ

		getVisibleBuildings
			?getVisibleBuildings@CvCity@@QBEXAAV?$list@W4BuildingTypes@@V?$allocator@W4BuildingTypes@@@std@@@std@@AAH@Z=?getVisibleBuildings@EXE_CvCity@@QBEXAAV?$list@W4BuildingTypes@@V?$allocator@W4BuildingTypes@@@std@@@std@@AAH@Z

		getVisibleEffects
			?getVisibleEffects@CvCity@@QBEXW4ZoomLevelTypes@@AAV?$vector@PBDV?$allocator@PBD@std@@@std@@@Z=?getVisibleEffects@EXE_CvCity@@QBEXW4ZoomLevelTypes@@AAV?$vector@PBDV?$allocator@PBD@std@@@std@@@Z

		getWallOverridePoints
			?getWallOverridePoints@CvCity@@QBEABV?$vector@U?$pair@MM@std@@V?$allocator@U?$pair@MM@std@@@2@@std@@XZ=?getWallOverridePoints@EXE_CvCity@@QBEABV?$vector@U?$pair@MM@std@@V?$allocator@U?$pair@MM@std@@@2@@std@@XZ

		getX
			?getX@CvCity@@QBEHXZ=?getX@EXE_CvCity@@QBEHXZ

		getY
			?getY@CvCity@@QBEHXZ=?getY@EXE_CvCity@@QBEHXZ

		headOrderQueueNode
			?headOrderQueueNode@CvCity@@QBEPAV?$CLLNode@UOrderData@@@@XZ=?headOrderQueueNode@EXE_CvCity@@QBEPAV?$CLLNode@UOrderData@@@@XZ

		hurryGold
			?hurryGold@CvCity@@QBEHW4HurryTypes@@@Z=?hurryGold@EXE_CvCity@@QBEHW4HurryTypes@@@Z

		hurryPopulation
			?hurryPopulation@CvCity@@QBEHW4HurryTypes@@@Z=?hurryPopulation@EXE_CvCity@@QBEHW4HurryTypes@@@Z

		isBillboardDirty
			?isBillboardDirty@CvCity@@QBE_NXZ=?isBillboardDirty@EXE_CvCity@@QBE_NXZ

		isCapital
			?isCapital@CvCity@@QBE_NXZ=?isCapital@EXE_CvCity@@QBE_NXZ

		isHasBuilding
			?isHasBuilding@CvCity@@QBE_NW4BuildingTypes@@@Z=?isHasBuilding@EXE_CvCity@@QBE_NW4BuildingTypes@@@Z

		isLayoutDirty
			?isLayoutDirty@CvCity@@QBE_NXZ=?isLayoutDirty@EXE_CvCity@@QBE_NXZ

		isProduction
			?isProduction@CvCity@@QBE_NXZ=?isProduction@EXE_CvCity@@QBE_NXZ

		isRevealed
			?isRevealed@CvCity@@QBE_NW4TeamTypes@@_N@Z=?isRevealed@EXE_CvCity@@QBE_NW4TeamTypes@@_N@Z

		isVisible
			?isVisible@CvCity@@QBE_NW4TeamTypes@@_N@Z=?isVisible@EXE_CvCity@@QBE_NW4TeamTypes@@_N@Z

		isWallOverride
			?isWallOverride@CvCity@@QBE_NXZ=?isWallOverride@EXE_CvCity@@QBE_NXZ

		nextOrderQueueNode
			?nextOrderQueueNode@CvCity@@QBEPAV?$CLLNode@UOrderData@@@@PAV2@@Z=?nextOrderQueueNode@EXE_CvCity@@QBEPAV?$CLLNode@UOrderData@@@@PAV2@@Z

		plot
			?plot@CvCity@@QBEPAVCvPlot@@XZ=?plot@EXE_CvCity@@QBEPAVCvPlot@@XZ
			*/
	DllExport void pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce = false)
	{
		CvCity::pushOrder(eOrder, iData1, iData2, bSave, bPop, bAppend, bForce);
	}
	DllExport void popOrder(int iNum, bool bFinish = false, bool bChoose = false)
	{
		CvCity::popOrder(iNum, bFinish, bChoose);
	}
	/*
		setBillboardDirty
			?setBillboardDirty@CvCity@@QAEX_N@Z=?setBillboardDirty@EXE_CvCity@@QAEX_N@Z

		setLayoutDirty
			?setLayoutDirty@CvCity@@QAEX_N@Z=?setLayoutDirty@EXE_CvCity@@QAEX_N@Z

		updateSelectedCity
			?updateSelectedCity@CvCity@@QAEXXZ=?updateSelectedCity@EXE_CvCity@@QAEXXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvCity) == sizeof(CvCity));


class EXE_CvCivilizationInfo : public CvCivilizationInfo
{
public:
	/*
		getAdjective
			?getAdjective@CvCivilizationInfo@@QAEPBGI@Z=?getAdjective@EXE_CvCivilizationInfo@@QAEPBGI@Z

		getArtInfo
			?getArtInfo@CvCivilizationInfo@@QBEPBVCvArtInfoCivilization@@XZ=?getArtInfo@EXE_CvCivilizationInfo@@QBEPBVCvArtInfoCivilization@@XZ

		getCivilizationBuildings
			?getCivilizationBuildings@CvCivilizationInfo@@QBEHH@Z=?getCivilizationBuildings@EXE_CvCivilizationInfo@@QBEHH@Z

		getCivilizationUnits
			?getCivilizationUnits@CvCivilizationInfo@@QBEHH@Z=?getCivilizationUnits@EXE_CvCivilizationInfo@@QBEHH@Z

		getDefaultPlayerColor
			?getDefaultPlayerColor@CvCivilizationInfo@@QBEHXZ=?getDefaultPlayerColor@EXE_CvCivilizationInfo@@QBEHXZ

		getDerivativeCiv
			?getDerivativeCiv@CvCivilizationInfo@@QBEHXZ=?getDerivativeCiv@EXE_CvCivilizationInfo@@QBEHXZ

		getFlagTexture
			?getFlagTexture@CvCivilizationInfo@@QBEPBDXZ=?getFlagTexture@EXE_CvCivilizationInfo@@QBEPBDXZ

		getMissionaryChar
			?getMissionaryChar@CvCivilizationInfo@@QBEHXZ=?getMissionaryChar@EXE_CvCivilizationInfo@@QBEHXZ

		getSelectionSoundScriptId
			?getSelectionSoundScriptId@CvCivilizationInfo@@QBEHXZ=?getSelectionSoundScriptId@EXE_CvCivilizationInfo@@QBEHXZ

		getShortDescription
			?getShortDescription@CvCivilizationInfo@@QAEPBGI@Z=?getShortDescription@EXE_CvCivilizationInfo@@QAEPBGI@Z

		isAIPlayable
			?isAIPlayable@CvCivilizationInfo@@QBE_NXZ=?isAIPlayable@EXE_CvCivilizationInfo@@QBE_NXZ

		isEurope
			?isEurope@CvCivilizationInfo@@QBE_NXZ=?isEurope@EXE_CvCivilizationInfo@@QBE_NXZ

		isLeaders
			?isLeaders@CvCivilizationInfo@@QBE_NH@Z=?isLeaders@EXE_CvCivilizationInfo@@QBE_NH@Z

		isNative
			?isNative@CvCivilizationInfo@@QBE_NXZ=?isNative@EXE_CvCivilizationInfo@@QBE_NXZ

		isOpenBorders
			?isOpenBorders@CvCivilizationInfo@@QBE_NXZ=?isOpenBorders@EXE_CvCivilizationInfo@@QBE_NXZ

		isPlayable
			?isPlayable@CvCivilizationInfo@@QBE_NXZ=?isPlayable@EXE_CvCivilizationInfo@@QBE_NXZ

		setMissionaryChar
			?setMissionaryChar@CvCivilizationInfo@@QAEXH@Z=?setMissionaryChar@EXE_CvCivilizationInfo@@QAEXH@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvCivilizationInfo) == sizeof(CvCivilizationInfo));


class EXE_CvColorInfo : public CvColorInfo
{
public:
	/*
		getColor
			?getColor@CvColorInfo@@QBEABVNiColorA@@XZ=?getColor@EXE_CvColorInfo@@QBEABVNiColorA@@XZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvColorInfo) == sizeof(CvColorInfo));


class EXE_CvCommandInfo : public CvCommandInfo
{
public:
	/*
		getAll
			?getAll@CvCommandInfo@@QBE_NXZ=?getAll@EXE_CvCommandInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvCommandInfo) == sizeof(CvCommandInfo));


class EXE_CvCursorInfo : public CvCursorInfo
{
public:
	/*
		getPath
			?getPath@CvCursorInfo@@QAEPBDXZ=?getPath@EXE_CvCursorInfo@@QAEPBDXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvCursorInfo) == sizeof(CvCursorInfo));


class EXE_CvDLLButtonPopup : public CvDLLButtonPopup
{
public:
	/*
		freeInstance
			?freeInstance@CvDLLButtonPopup@@SAXXZ=?freeInstance@EXE_CvDLLButtonPopup@@SAXXZ

		getInstance
			?getInstance@CvDLLButtonPopup@@SAAAV1@XZ=?getInstance@EXE_CvDLLButtonPopup@@SAAAV1@XZ

		launchButtonPopup
			?launchButtonPopup@CvDLLButtonPopup@@QAE_NPAVCvPopup@@AAVCvPopupInfo@@@Z=?launchButtonPopup@EXE_CvDLLButtonPopup@@QAE_NPAVCvPopup@@AAVCvPopupInfo@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvDLLButtonPopup) == sizeof(CvDLLButtonPopup));


class EXE_CvDLLEntity : public CvDLLEntity
{
public:
	/*
		IsSelected
			?IsSelected@CvDLLEntity@@QBE_NXZ=?IsSelected@EXE_CvDLLEntity@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvDLLEntity) == sizeof(CvDLLEntity));


class EXE_CvDLLWidgetData : public CvDLLWidgetData
{
public:
	/*
		executeAction
			?executeAction@CvDLLWidgetData@@QAE_NAAUCvWidgetDataStruct@@@Z=?executeAction@EXE_CvDLLWidgetData@@QAE_NAAUCvWidgetDataStruct@@@Z

		executeAltAction
			?executeAltAction@CvDLLWidgetData@@QAE_NAAUCvWidgetDataStruct@@@Z=?executeAltAction@EXE_CvDLLWidgetData@@QAE_NAAUCvWidgetDataStruct@@@Z

		executeDoubleClick
			?executeDoubleClick@CvDLLWidgetData@@QAE_NABUCvWidgetDataStruct@@@Z=?executeDoubleClick@EXE_CvDLLWidgetData@@QAE_NABUCvWidgetDataStruct@@@Z

		executeDropOn
			?executeDropOn@CvDLLWidgetData@@QAE_NABUCvWidgetDataStruct@@0@Z=?executeDropOn@EXE_CvDLLWidgetData@@QAE_NABUCvWidgetDataStruct@@0@Z

		freeInstance
			?freeInstance@CvDLLWidgetData@@SAXXZ=?freeInstance@EXE_CvDLLWidgetData@@SAXXZ

		getInstance
			?getInstance@CvDLLWidgetData@@SAAAV1@XZ=?getInstance@EXE_CvDLLWidgetData@@SAAAV1@XZ

		parseHelp
			?parseHelp@CvDLLWidgetData@@QAEXAAVCvWStringBuffer@@AAUCvWidgetDataStruct@@@Z=?parseHelp@EXE_CvDLLWidgetData@@QAEXAAVCvWStringBuffer@@AAUCvWidgetDataStruct@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvDLLWidgetData) == sizeof(CvDLLWidgetData));


class EXE_CvDeal : public CvDeal
{
public:
	/*
		getFirstPlayer
			?getFirstPlayer@CvDeal@@QBE?AW4PlayerTypes@@XZ=?getFirstPlayer@EXE_CvDeal@@QBE?AW4PlayerTypes@@XZ

		getGoldItem
			?getGoldItem@CvDeal@@SA?AW4TradeableItems@@XZ=?getGoldItem@EXE_CvDeal@@SA?AW4TradeableItems@@XZ

		getID
			?getID@CvDeal@@QBEHXZ=?getID@EXE_CvDeal@@QBEHXZ

		getPeaceItem
			?getPeaceItem@CvDeal@@SA?AW4TradeableItems@@XZ=?getPeaceItem@EXE_CvDeal@@SA?AW4TradeableItems@@XZ

		getSecondPlayer
			?getSecondPlayer@CvDeal@@QBE?AW4PlayerTypes@@XZ=?getSecondPlayer@EXE_CvDeal@@QBE?AW4PlayerTypes@@XZ

		hasData
			?hasData@CvDeal@@SA_NW4TradeableItems@@@Z=?hasData@EXE_CvDeal@@SA_NW4TradeableItems@@@Z

		headFirstTradesNode
			?headFirstTradesNode@CvDeal@@QBEPAV?$CLLNode@UTradeData@@@@XZ=?headFirstTradesNode@EXE_CvDeal@@QBEPAV?$CLLNode@UTradeData@@@@XZ

		headSecondTradesNode
			?headSecondTradesNode@CvDeal@@QBEPAV?$CLLNode@UTradeData@@@@XZ=?headSecondTradesNode@EXE_CvDeal@@QBEPAV?$CLLNode@UTradeData@@@@XZ

		isCancelable
			?isCancelable@CvDeal@@QAE_NW4PlayerTypes@@PAVCvWString@@@Z=?isCancelable@EXE_CvDeal@@QAE_NW4PlayerTypes@@PAVCvWString@@@Z

		isDual
			?isDual@CvDeal@@SA_NW4TradeableItems@@_N@Z=?isDual@EXE_CvDeal@@SA_NW4TradeableItems@@_N@Z

		isEndWar
			?isEndWar@CvDeal@@SA_NW4TradeableItems@@@Z=?isEndWar@EXE_CvDeal@@SA_NW4TradeableItems@@@Z

		isGold
			?isGold@CvDeal@@SA_NW4TradeableItems@@@Z=?isGold@EXE_CvDeal@@SA_NW4TradeableItems@@@Z

		kill
			?kill@CvDeal@@QAEX_NW4TeamTypes@@@Z=?kill@EXE_CvDeal@@QAEX_NW4TeamTypes@@@Z

		nextFirstTradesNode
			?nextFirstTradesNode@CvDeal@@QBEPAV?$CLLNode@UTradeData@@@@PAV2@@Z=?nextFirstTradesNode@EXE_CvDeal@@QBEPAV?$CLLNode@UTradeData@@@@PAV2@@Z

		nextSecondTradesNode
			?nextSecondTradesNode@CvDeal@@QBEPAV?$CLLNode@UTradeData@@@@PAV2@@Z=?nextSecondTradesNode@EXE_CvDeal@@QBEPAV?$CLLNode@UTradeData@@@@PAV2@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvDeal) == sizeof(CvDeal));


class EXE_CvDiploParameters : public CvDiploParameters
{
public:
	/*
		getAIContact
			?getAIContact@CvDiploParameters@@QBE_NXZ=?getAIContact@EXE_CvDiploParameters@@QBE_NXZ

		getChatText
			?getChatText@CvDiploParameters@@QBEPBGXZ=?getChatText@EXE_CvDiploParameters@@QBEPBGXZ

		getCity
			?getCity@CvDiploParameters@@QBEABUIDInfo@@XZ=?getCity@EXE_CvDiploParameters@@QBEABUIDInfo@@XZ

		getData
			?getData@CvDiploParameters@@QBEHXZ=?getData@EXE_CvDiploParameters@@QBEHXZ

		getDiploComment
			?getDiploComment@CvDiploParameters@@QBE?AW4DiploCommentTypes@@XZ=?getDiploComment@EXE_CvDiploParameters@@QBE?AW4DiploCommentTypes@@XZ

		getHumanDiplo
			?getHumanDiplo@CvDiploParameters@@QBE_NXZ=?getHumanDiplo@EXE_CvDiploParameters@@QBE_NXZ

		getOurOfferList
			?getOurOfferList@CvDiploParameters@@QBEABV?$CLinkList@UTradeData@@@@XZ=?getOurOfferList@EXE_CvDiploParameters@@QBEABV?$CLinkList@UTradeData@@@@XZ

		getOurOffering
			?getOurOffering@CvDiploParameters@@QBE_NXZ=?getOurOffering@EXE_CvDiploParameters@@QBE_NXZ

		getPendingDelete
			?getPendingDelete@CvDiploParameters@@QBE_NXZ=?getPendingDelete@EXE_CvDiploParameters@@QBE_NXZ

		getRenegotiate
			?getRenegotiate@CvDiploParameters@@QBE_NXZ=?getRenegotiate@EXE_CvDiploParameters@@QBE_NXZ

		getTheirOfferList
			?getTheirOfferList@CvDiploParameters@@QBEABV?$CLinkList@UTradeData@@@@XZ=?getTheirOfferList@EXE_CvDiploParameters@@QBEABV?$CLinkList@UTradeData@@@@XZ

		getTheirOffering
			?getTheirOffering@CvDiploParameters@@QBE_NXZ=?getTheirOffering@EXE_CvDiploParameters@@QBE_NXZ

		getTransport
			?getTransport@CvDiploParameters@@QBEABUIDInfo@@XZ=?getTransport@EXE_CvDiploParameters@@QBEABUIDInfo@@XZ

		getWhoTalkingTo
			?getWhoTalkingTo@CvDiploParameters@@QBE?AW4PlayerTypes@@XZ=?getWhoTalkingTo@EXE_CvDiploParameters@@QBE?AW4PlayerTypes@@XZ

		read
			?read@CvDiploParameters@@QAEXAAVFDataStreamBase@@@Z=?read@EXE_CvDiploParameters@@QAEXAAVFDataStreamBase@@@Z

		setChatText
			?setChatText@CvDiploParameters@@QAEXPBG@Z=?setChatText@EXE_CvDiploParameters@@QAEXPBG@Z

		setHumanDiplo
			?setHumanDiplo@CvDiploParameters@@QAEX_N@Z=?setHumanDiplo@EXE_CvDiploParameters@@QAEX_N@Z

		setOurOfferList
			?setOurOfferList@CvDiploParameters@@QAEXABV?$CLinkList@UTradeData@@@@@Z=?setOurOfferList@EXE_CvDiploParameters@@QAEXABV?$CLinkList@UTradeData@@@@@Z

		setOurOffering
			?setOurOffering@CvDiploParameters@@QAEX_N@Z=?setOurOffering@EXE_CvDiploParameters@@QAEX_N@Z

		setPendingDelete
			?setPendingDelete@CvDiploParameters@@QAEX_N@Z=?setPendingDelete@EXE_CvDiploParameters@@QAEX_N@Z

		setTheirOfferList
			?setTheirOfferList@CvDiploParameters@@QAEXABV?$CLinkList@UTradeData@@@@@Z=?setTheirOfferList@EXE_CvDiploParameters@@QAEXABV?$CLinkList@UTradeData@@@@@Z

		setTheirOffering
			?setTheirOffering@CvDiploParameters@@QAEX_N@Z=?setTheirOffering@EXE_CvDiploParameters@@QAEX_N@Z

		setWhoTalkingTo
			?setWhoTalkingTo@CvDiploParameters@@QAEXW4PlayerTypes@@@Z=?setWhoTalkingTo@EXE_CvDiploParameters@@QAEXW4PlayerTypes@@@Z

		write
			?write@CvDiploParameters@@QBEXAAVFDataStreamBase@@@Z=?write@EXE_CvDiploParameters@@QBEXAAVFDataStreamBase@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvDiploParameters) == sizeof(CvDiploParameters));


class EXE_CvEffectInfo : public CvEffectInfo
{
public:
	/*
		getPath
			?getPath@CvEffectInfo@@QBEPBDXZ=?getPath@EXE_CvEffectInfo@@QBEPBDXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvEffectInfo) == sizeof(CvEffectInfo));


class EXE_CvEntityEventInfo : public CvEntityEventInfo
{
public:
	/*
		getAnimationPathType
			?getAnimationPathType@CvEntityEventInfo@@QBE?AW4AnimationPathTypes@@H@Z=?getAnimationPathType@EXE_CvEntityEventInfo@@QBE?AW4AnimationPathTypes@@H@Z

		getEffectType
			?getEffectType@CvEntityEventInfo@@QBE?AW4EffectTypes@@H@Z=?getEffectType@EXE_CvEntityEventInfo@@QBE?AW4EffectTypes@@H@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvEntityEventInfo) == sizeof(CvEntityEventInfo));


class EXE_CvEraInfo : public CvEraInfo
{
public:
	/*
		getCitySoundscapeSciptId
			?getCitySoundscapeSciptId@CvEraInfo@@QBEHH@Z=?getCitySoundscapeSciptId@EXE_CvEraInfo@@QBEHH@Z

		getNumSoundtracks
			?getNumSoundtracks@CvEraInfo@@QBEHXZ=?getNumSoundtracks@EXE_CvEraInfo@@QBEHXZ

		getSoundtrackSpace
			?getSoundtrackSpace@CvEraInfo@@QBEHXZ=?getSoundtrackSpace@EXE_CvEraInfo@@QBEHXZ

		getSoundtracks
			?getSoundtracks@CvEraInfo@@QBEHH@Z=?getSoundtracks@EXE_CvEraInfo@@QBEHH@Z

		isFirstSoundtrackFirst
			?isFirstSoundtrackFirst@CvEraInfo@@QBE_NXZ=?isFirstSoundtrackFirst@EXE_CvEraInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvEraInfo) == sizeof(CvEraInfo));


class EXE_CvFatherPointInfo : public CvFatherPointInfo
{
public:
	/*
		getChar
			?getChar@CvFatherPointInfo@@QBEHXZ=?getChar@EXE_CvFatherPointInfo@@QBEHXZ

		getFontButtonIndex
			?getFontButtonIndex@CvFatherPointInfo@@QBEHXZ=?getFontButtonIndex@EXE_CvFatherPointInfo@@QBEHXZ

		setChar
			?setChar@CvFatherPointInfo@@QAEXH@Z=?setChar@EXE_CvFatherPointInfo@@QAEXH@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvFatherPointInfo) == sizeof(CvFatherPointInfo));


class EXE_CvFeatureInfo : public CvFeatureInfo
{
public:
	/*
		get3DAudioScriptFootstepIndex
			?get3DAudioScriptFootstepIndex@CvFeatureInfo@@QBEHH@Z=?get3DAudioScriptFootstepIndex@EXE_CvFeatureInfo@@QBEHH@Z

		getArtInfo
			?getArtInfo@CvFeatureInfo@@QBEPBVCvArtInfoFeature@@XZ=?getArtInfo@EXE_CvFeatureInfo@@QBEPBVCvArtInfoFeature@@XZ

		getWorldSoundscapeScriptId
			?getWorldSoundscapeScriptId@CvFeatureInfo@@QBEHXZ=?getWorldSoundscapeScriptId@EXE_CvFeatureInfo@@QBEHXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvFeatureInfo) == sizeof(CvFeatureInfo));


class EXE_CvFractal : public CvFractal
{
public:
	/*
		fracInit
			?fracInit@CvFractal@@QAEXHHHAAVCvRandom@@HPAV1@HH@Z=?fracInit@EXE_CvFractal@@QAEXHHHAAVCvRandom@@HPAV1@HH@Z

		fracInitHinted
			?fracInitHinted@CvFractal@@QAEXHHHAAVCvRandom@@PAEHHPAV1@HH@Z=?fracInitHinted@EXE_CvFractal@@QAEXHHHAAVCvRandom@@PAEHHPAV1@HH@Z

		getHeight
			?getHeight@CvFractal@@QAEHHH@Z=?getHeight@EXE_CvFractal@@QAEHHH@Z

		getHeightFromPercent
			?getHeightFromPercent@CvFractal@@QAEHH@Z=?getHeightFromPercent@EXE_CvFractal@@QAEHH@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvFractal) == sizeof(CvFractal));


class EXE_CvGame : public CvGame
{
public:
	/*
		calculateOptionsChecksum
			?calculateOptionsChecksum@CvGame@@QAEHXZ=?calculateOptionsChecksum@EXE_CvGame@@QAEHXZ

		calculateSyncChecksum
			?calculateSyncChecksum@CvGame@@QAEHPAVCvString@@@Z=?calculateSyncChecksum@EXE_CvGame@@QAEHPAVCvString@@@Z

		canHandleAction
			?canHandleAction@CvGame@@QAE_NHPAVCvPlot@@_N1@Z=?canHandleAction@EXE_CvGame@@QAE_NHPAVCvPlot@@_N1@Z

		cityPushOrder
			?cityPushOrder@CvGame@@QAEXPAVCvCity@@W4OrderTypes@@H_N22@Z=?cityPushOrder@EXE_CvGame@@QAEXPAVCvCity@@W4OrderTypes@@H_N22@Z

		countNumHumanGameTurnActive
			?countNumHumanGameTurnActive@CvGame@@QAEHXZ=?countNumHumanGameTurnActive@EXE_CvGame@@QAEHXZ

		cycleCities
			?cycleCities@CvGame@@QAEX_N0@Z=?cycleCities@EXE_CvGame@@QAEX_N0@Z

		cyclePlotUnits
			?cyclePlotUnits@CvGame@@QAE_NPAVCvPlot@@_N1H@Z=?cyclePlotUnits@EXE_CvGame@@QAE_NPAVCvPlot@@_N1H@Z

		doControl
			?doControl@CvGame@@QAEXW4ControlTypes@@@Z=?doControl@EXE_CvGame@@QAEXW4ControlTypes@@@Z

		getActiveCivilizationType
			?getActiveCivilizationType@CvGame@@QAE?AW4CivilizationTypes@@XZ=?getActiveCivilizationType@EXE_CvGame@@QAE?AW4CivilizationTypes@@XZ

		getActivePlayer
			?getActivePlayer@CvGame@@QBE?AW4PlayerTypes@@XZ=?getActivePlayer@EXE_CvGame@@QBE?AW4PlayerTypes@@XZ

		getActiveTeam
			?getActiveTeam@CvGame@@QAE?AW4TeamTypes@@XZ=?getActiveTeam@EXE_CvGame@@QAE?AW4TeamTypes@@XZ

		getBestLandUnitCombat
			?getBestLandUnitCombat@CvGame@@QAEHXZ=?getBestLandUnitCombat@EXE_CvGame@@QAEHXZ

		getDeal
			?getDeal@CvGame@@QAEPAVCvDeal@@H@Z=?getDeal@EXE_CvGame@@QAEPAVCvDeal@@H@Z

		getGameState
			?getGameState@CvGame@@QBE?AW4GameStateTypes@@XZ=?getGameState@EXE_CvGame@@QBE?AW4GameStateTypes@@XZ

		getGameTurn
			?getGameTurn@CvGame@@QAEHXZ=?getGameTurn@EXE_CvGame@@QAEHXZ

		getGlobeviewConfigurationParameters
			?getGlobeviewConfigurationParameters@CvGame@@QAEXW4TeamTypes@@AA_N1@Z=?getGlobeviewConfigurationParameters@EXE_CvGame@@QAEXW4TeamTypes@@AA_N1@Z

		getHotPbemBetweenTurns
			?getHotPbemBetweenTurns@CvGame@@QBE_NXZ=?getHotPbemBetweenTurns@EXE_CvGame@@QBE_NXZ

		getIndexAfterLastDeal
			?getIndexAfterLastDeal@CvGame@@QAEHXZ=?getIndexAfterLastDeal@EXE_CvGame@@QAEHXZ

		getName
			?getName@CvGame@@QAEABVCvWString@@XZ=?getName@EXE_CvGame@@QAEABVCvWString@@XZ

		getNumHumanPlayers
			?getNumHumanPlayers@CvGame@@QAEHXZ=?getNumHumanPlayers@EXE_CvGame@@QAEHXZ

		getPausePlayer
			?getPausePlayer@CvGame@@QAE?AW4PlayerTypes@@XZ=?getPausePlayer@EXE_CvGame@@QAE?AW4PlayerTypes@@XZ

		getPlayerScore
			?getPlayerScore@CvGame@@QAEHW4PlayerTypes@@@Z=?getPlayerScore@EXE_CvGame@@QAEHW4PlayerTypes@@@Z

		getPlotUnit
			?getPlotUnit@CvGame@@QAEPAVCvUnit@@PBVCvPlot@@H@Z=?getPlotUnit@EXE_CvGame@@QAEPAVCvUnit@@PBVCvPlot@@H@Z

		getPlotUnits
			?getPlotUnits@CvGame@@QAEXPBVCvPlot@@AAV?$vector@PAVCvUnit@@V?$allocator@PAVCvUnit@@@std@@@std@@@Z=?getPlotUnits@EXE_CvGame@@QAEXPBVCvPlot@@AAV?$vector@PAVCvUnit@@V?$allocator@PAVCvUnit@@@std@@@std@@@Z

		getStartEra
			?getStartEra@CvGame@@QBE?AW4EraTypes@@XZ=?getStartEra@EXE_CvGame@@QBE?AW4EraTypes@@XZ

		getTurnSlice
			?getTurnSlice@CvGame@@QBEHXZ=?getTurnSlice@EXE_CvGame@@QBEHXZ

		getTurnSlicesRemaining
			?getTurnSlicesRemaining@CvGame@@QBEHXZ=?getTurnSlicesRemaining@EXE_CvGame@@QBEHXZ

		getTurnTimerText
			?getTurnTimerText@CvGame@@QBEXAAVCvWString@@@Z=?getTurnTimerText@EXE_CvGame@@QBEXAAVCvWString@@@Z

		getVictory
			?getVictory@CvGame@@QBE?AW4VictoryTypes@@XZ=?getVictory@EXE_CvGame@@QBE?AW4VictoryTypes@@XZ

		getWinner
			?getWinner@CvGame@@QBE?AW4TeamTypes@@XZ=?getWinner@EXE_CvGame@@QBE?AW4TeamTypes@@XZ

		handleAction
			?handleAction@CvGame@@QAEXH@Z=?handleAction@EXE_CvGame@@QAEXH@Z

		implementDeal
			?implementDeal@CvGame@@QAEXW4PlayerTypes@@0PAV?$CLinkList@UTradeData@@@@1_N@Z=?implementDeal@EXE_CvGame@@QAEXW4PlayerTypes@@0PAV?$CLinkList@UTradeData@@@@1_N@Z

		incrementEndTurnMessagesSent
			?incrementEndTurnMessagesSent@CvGame@@QAEXXZ=?incrementEndTurnMessagesSent@EXE_CvGame@@QAEXXZ

		init
			?init@CvGame@@QAEXW4HandicapTypes@@@Z=?init@EXE_CvGame@@QAEXW4HandicapTypes@@@Z

		initDiplomacy
			?initDiplomacy@CvGame@@QAEXXZ=?initDiplomacy@EXE_CvGame@@QAEXXZ

		initEvents
			?initEvents@CvGame@@QAEXXZ=?initEvents@EXE_CvGame@@QAEXXZ

		initScoreCalculation
			?initScoreCalculation@CvGame@@QAEXXZ=?initScoreCalculation@EXE_CvGame@@QAEXXZ

		isDebugMode
			?isDebugMode@CvGame@@QBE_NXZ=?isDebugMode@EXE_CvGame@@QBE_NXZ

		isFinalInitialized
			?isFinalInitialized@CvGame@@QBE_NXZ=?isFinalInitialized@EXE_CvGame@@QBE_NXZ

		isGameMultiPlayer
			?isGameMultiPlayer@CvGame@@QBE_NXZ=?isGameMultiPlayer@EXE_CvGame@@QBE_NXZ

		isHotSeat
			?isHotSeat@CvGame@@QBE_NXZ=?isHotSeat@EXE_CvGame@@QBE_NXZ

		isMPOption
			?isMPOption@CvGame@@QBE_NW4MultiplayerOptionTypes@@@Z=?isMPOption@EXE_CvGame@@QBE_NW4MultiplayerOptionTypes@@@Z

		isNetworkMultiPlayer
			?isNetworkMultiPlayer@CvGame@@QBE_NXZ=?isNetworkMultiPlayer@EXE_CvGame@@QBE_NXZ

		isPaused
			?isPaused@CvGame@@QAE_NXZ=?isPaused@EXE_CvGame@@QAE_NXZ

		isPbem
			?isPbem@CvGame@@QBE_NXZ=?isPbem@EXE_CvGame@@QBE_NXZ

		isPitboss
			?isPitboss@CvGame@@QBE_NXZ=?isPitboss@EXE_CvGame@@QBE_NXZ

		isSimultaneousTeamTurns
			?isSimultaneousTeamTurns@CvGame@@QBE_NXZ=?isSimultaneousTeamTurns@EXE_CvGame@@QBE_NXZ

		isTeamGame
			?isTeamGame@CvGame@@QBE_NXZ=?isTeamGame@EXE_CvGame@@QBE_NXZ

		regenerateMap
			?regenerateMap@CvGame@@QAEXXZ=?regenerateMap@EXE_CvGame@@QAEXXZ

		reset
			?reset@CvGame@@QAEXW4HandicapTypes@@_N@Z=?reset@EXE_CvGame@@QAEXW4HandicapTypes@@_N@Z

		reviveActivePlayer
			?reviveActivePlayer@CvGame@@QAEXXZ=?reviveActivePlayer@EXE_CvGame@@QAEXXZ

		selectAll
			?selectAll@CvGame@@QAEXPAVCvPlot@@@Z=?selectAll@EXE_CvGame@@QAEXPAVCvPlot@@@Z

		selectGroup
			?selectGroup@CvGame@@QAEXPAVCvUnit@@_N11@Z=?selectGroup@EXE_CvGame@@QAEXPAVCvUnit@@_N11@Z

		selectUnit
			?selectUnit@CvGame@@QAEXPAVCvUnit@@_N11@Z=?selectUnit@EXE_CvGame@@QAEXPAVCvUnit@@_N11@Z

		selectedCitiesGameNetMessage
			?selectedCitiesGameNetMessage@CvGame@@QAEXHHHH_N000@Z=?selectedCitiesGameNetMessage@EXE_CvGame@@QAEXHHHH_N000@Z

		selectionListGameNetMessage
			?selectionListGameNetMessage@CvGame@@QAEXHHHHH_N0@Z=?selectionListGameNetMessage@EXE_CvGame@@QAEXHHHHH_N0@Z

		selectionListMove
			?selectionListMove@CvGame@@QAEXPAVCvPlot@@_N11@Z=?selectionListMove@EXE_CvGame@@QAEXPAVCvPlot@@_N11@Z

		setAIAutoPlay
			?setAIAutoPlay@CvGame@@QAEXH@Z=?setAIAutoPlay@EXE_CvGame@@QAEXH@Z

		setActivePlayer
			?setActivePlayer@CvGame@@QAEXW4PlayerTypes@@_N@Z=?setActivePlayer@EXE_CvGame@@QAEXW4PlayerTypes@@_N@Z

		setFinalInitialized
			?setFinalInitialized@CvGame@@QAEX_N@Z=?setFinalInitialized@EXE_CvGame@@QAEX_N@Z

		setGameState
			?setGameState@CvGame@@QAEXW4GameStateTypes@@@Z=?setGameState@EXE_CvGame@@QAEXW4GameStateTypes@@@Z

		setInitialItems
			?setInitialItems@CvGame@@QAEX_N@Z=?setInitialItems@EXE_CvGame@@QAEX_N@Z

		setInitialTime
			?setInitialTime@CvGame@@QAEXI@Z=?setInitialTime@EXE_CvGame@@QAEXI@Z

		setPausePlayer
			?setPausePlayer@CvGame@@QAEXW4PlayerTypes@@@Z=?setPausePlayer@EXE_CvGame@@QAEXW4PlayerTypes@@@Z

		setPbemTurnSent
			?setPbemTurnSent@CvGame@@QAEX_N@Z=?setPbemTurnSent@EXE_CvGame@@QAEX_N@Z

		setReplayInfo
			?setReplayInfo@CvGame@@QAEXPAVCvReplayInfo@@@Z=?setReplayInfo@EXE_CvGame@@QAEXPAVCvReplayInfo@@@Z

		setupActionCache
			?setupActionCache@CvGame@@QAEXXZ=?setupActionCache@EXE_CvGame@@QAEXXZ

		testExtendedGame
			?testExtendedGame@CvGame@@QAEXXZ=?testExtendedGame@EXE_CvGame@@QAEXXZ

		toggleDebugMode
			?toggleDebugMode@CvGame@@QAEXXZ=?toggleDebugMode@EXE_CvGame@@QAEXXZ

		update
			?update@CvGame@@QAEXXZ=?update@EXE_CvGame@@QAEXXZ

		updateColoredPlots
			?updateColoredPlots@CvGame@@QAEXXZ=?updateColoredPlots@EXE_CvGame@@QAEXXZ

		updateDebugModeCache
			?updateDebugModeCache@CvGame@@QAEXXZ=?updateDebugModeCache@EXE_CvGame@@QAEXXZ

		updateSelectionList
			?updateSelectionList@CvGame@@QAEXXZ=?updateSelectionList@EXE_CvGame@@QAEXXZ

		updateTestEndTurn
			?updateTestEndTurn@CvGame@@QAEXXZ=?updateTestEndTurn@EXE_CvGame@@QAEXXZ

		updateUnitEnemyGlow
			?updateUnitEnemyGlow@CvGame@@QAEXXZ=?updateUnitEnemyGlow@EXE_CvGame@@QAEXXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvGame) == sizeof(CvGame));


class EXE_CvGameOptionInfo : public CvGameOptionInfo
{
public:
	/*
		getDefault
			?getDefault@CvGameOptionInfo@@QBE_NXZ=?getDefault@EXE_CvGameOptionInfo@@QBE_NXZ

		getVisible
			?getVisible@CvGameOptionInfo@@QBE_NXZ=?getVisible@EXE_CvGameOptionInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvGameOptionInfo) == sizeof(CvGameOptionInfo));


class EXE_CvGameText : public CvGameText
{
public:
	/*
		getNumLanguages
			?getNumLanguages@CvGameText@@QBEHXZ=?getNumLanguages@EXE_CvGameText@@QBEHXZ

		setNumLanguages
			?setNumLanguages@CvGameText@@QAEXH@Z=?setNumLanguages@EXE_CvGameText@@QAEXH@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvGameText) == sizeof(CvGameText));


class EXE_CvGameTextMgr : public CvGameTextMgr
{
public:
	/*
		DeInitialize
			?DeInitialize@CvGameTextMgr@@QAEXXZ=?DeInitialize@EXE_CvGameTextMgr@@QAEXXZ

		GetInstance
			?GetInstance@CvGameTextMgr@@SAAAV1@XZ=?GetInstance@EXE_CvGameTextMgr@@SAAAV1@XZ

		Initialize
			?Initialize@CvGameTextMgr@@QAEXXZ=?Initialize@EXE_CvGameTextMgr@@QAEXXZ

		Reset
			?Reset@CvGameTextMgr@@QAEXXZ=?Reset@EXE_CvGameTextMgr@@QAEXXZ

		buildCityBillboardCityNameString
			?buildCityBillboardCityNameString@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PAVCvCity@@@Z=?buildCityBillboardCityNameString@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PAVCvCity@@@Z

		buildCityBillboardCitySizeString
			?buildCityBillboardCitySizeString@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PAVCvCity@@ABVNiColorA@@@Z=?buildCityBillboardCitySizeString@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PAVCvCity@@ABVNiColorA@@@Z

		buildCityBillboardIconString
			?buildCityBillboardIconString@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PAVCvCity@@@Z=?buildCityBillboardIconString@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PAVCvCity@@@Z

		buildCityBillboardProductionString
			?buildCityBillboardProductionString@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PAVCvCity@@@Z=?buildCityBillboardProductionString@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PAVCvCity@@@Z

		getTradeScreenHeader
			?getTradeScreenHeader@CvGameTextMgr@@QAEXAAVCvWString@@W4PlayerTypes@@1_NPAVCvCity@@@Z=?getTradeScreenHeader@EXE_CvGameTextMgr@@QAEXAAVCvWString@@W4PlayerTypes@@1_NPAVCvCity@@@Z

		getTradeScreenIcons
			?getTradeScreenIcons@CvGameTextMgr@@QAEXAAV?$vector@U?$pair@VCvString@@UCvWidgetDataStruct@@@std@@V?$allocator@U?$pair@VCvString@@UCvWidgetDataStruct@@@std@@@2@@std@@W4PlayerTypes@@@Z=?getTradeScreenIcons@EXE_CvGameTextMgr@@QAEXAAV?$vector@U?$pair@VCvString@@UCvWidgetDataStruct@@@std@@V?$allocator@U?$pair@VCvString@@UCvWidgetDataStruct@@@std@@@2@@std@@W4PlayerTypes@@@Z

		getTradeScreenTitleIcon
			?getTradeScreenTitleIcon@CvGameTextMgr@@QAEXAAVCvString@@AAUCvWidgetDataStruct@@W4PlayerTypes@@@Z=?getTradeScreenTitleIcon@EXE_CvGameTextMgr@@QAEXAAVCvString@@AAUCvWidgetDataStruct@@W4PlayerTypes@@@Z

		parseCivInfos
			?parseCivInfos@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4CivilizationTypes@@_N2@Z=?parseCivInfos@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4CivilizationTypes@@_N2@Z

		parseCivShortTraits
			?parseCivShortTraits@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4CivilizationTypes@@@Z=?parseCivShortTraits@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4CivilizationTypes@@@Z

		parseLeaderShortTraits
			?parseLeaderShortTraits@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4LeaderHeadTypes@@@Z=?parseLeaderShortTraits@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4LeaderHeadTypes@@@Z

		parseLeaderTraits
			?parseLeaderTraits@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4LeaderHeadTypes@@W4CivilizationTypes@@_N3@Z=?parseLeaderTraits@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4LeaderHeadTypes@@W4CivilizationTypes@@_N3@Z

		setBonusHelp
			?setBonusHelp@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4BonusTypes@@_N@Z=?setBonusHelp@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4BonusTypes@@_N@Z

		setBuildingHelp
			?setBuildingHelp@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4BuildingTypes@@_N2PAVCvCity@@@Z=?setBuildingHelp@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4BuildingTypes@@_N2PAVCvCity@@@Z

		setCityBarHelp
			?setCityBarHelp@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PAVCvCity@@@Z=?setCityBarHelp@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PAVCvCity@@@Z

		setCombatPlotHelp
			?setCombatPlotHelp@CvGameTextMgr@@QAE_NAAVCvWStringBuffer@@PAVCvPlot@@@Z=?setCombatPlotHelp@EXE_CvGameTextMgr@@QAE_NAAVCvWStringBuffer@@PAVCvPlot@@@Z

		setFeatureHelp
			?setFeatureHelp@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4FeatureTypes@@_N@Z=?setFeatureHelp@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4FeatureTypes@@_N@Z

		setImprovementHelp
			?setImprovementHelp@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4ImprovementTypes@@_N@Z=?setImprovementHelp@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4ImprovementTypes@@_N@Z

		setMinimizePopupHelp
			?setMinimizePopupHelp@CvGameTextMgr@@QAEXAAVCvWString@@ABVCvPopupInfo@@@Z=?setMinimizePopupHelp@EXE_CvGameTextMgr@@QAEXAAVCvWString@@ABVCvPopupInfo@@@Z

		setPlotHelp
			?setPlotHelp@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PAVCvPlot@@@Z=?setPlotHelp@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PAVCvPlot@@@Z

		setPlotListHelp
			?setPlotListHelp@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PBVCvPlot@@_N2@Z=?setPlotListHelp@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PBVCvPlot@@_N2@Z

		setPromotionHelp
			?setPromotionHelp@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4PromotionTypes@@_N@Z=?setPromotionHelp@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4PromotionTypes@@_N@Z

		setResourceLayerInfo
			?setResourceLayerInfo@CvGameTextMgr@@QAEXW4ResourceLayerOptions@@AAVCvWString@@AAVCvString@@@Z=?setResourceLayerInfo@EXE_CvGameTextMgr@@QAEXW4ResourceLayerOptions@@AAVCvWString@@AAVCvString@@@Z

		setTerrainHelp
			?setTerrainHelp@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4TerrainTypes@@_N@Z=?setTerrainHelp@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4TerrainTypes@@_N@Z

		setTimeStr
			?setTimeStr@CvGameTextMgr@@QAEXAAVCvWString@@H_N@Z=?setTimeStr@EXE_CvGameTextMgr@@QAEXAAVCvWString@@H_N@Z

		setUnitHelp
			?setUnitHelp@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PBVCvUnit@@_N2@Z=?setUnitHelp@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@PBVCvUnit@@_N2@Z

		setUnitHelp
			?setUnitHelp@CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4UnitTypes@@_N2PAVCvCity@@@Z=?setUnitHelp@EXE_CvGameTextMgr@@QAEXAAVCvWStringBuffer@@W4UnitTypes@@_N2PAVCvCity@@@Z

		setUnitLayerInfo
			?setUnitLayerInfo@CvGameTextMgr@@QAEXW4UnitLayerOptionTypes@@AAVCvWString@@AAVCvString@@@Z=?setUnitLayerInfo@EXE_CvGameTextMgr@@QAEXW4UnitLayerOptionTypes@@AAVCvWString@@AAVCvString@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvGameTextMgr) == sizeof(CvGameTextMgr));


class EXE_CvGlobals : public CvGlobals
{
public:
	/*
		IsGraphicsInitialized
			?IsGraphicsInitialized@CvGlobals@@QBE_NXZ=?IsGraphicsInitialized@EXE_CvGlobals@@QBE_NXZ

		SetGraphicsInitialized
			?SetGraphicsInitialized@CvGlobals@@QAEX_N@Z=?SetGraphicsInitialized@EXE_CvGlobals@@QAEX_N@Z

		addToInfosVectors
			?addToInfosVectors@CvGlobals@@QAEXPAX@Z=?addToInfosVectors@EXE_CvGlobals@@QAEXPAX@Z

		getASyncRand
			?getASyncRand@CvGlobals@@QAEAAVCvRandom@@XZ=?getASyncRand@EXE_CvGlobals@@QAEAAVCvRandom@@XZ

		getActionInfo
			?getActionInfo@CvGlobals@@QAEAAVCvActionInfo@@H@Z=?getActionInfo@EXE_CvGlobals@@QAEAAVCvActionInfo@@H@Z

		getActiveLandscapeID
			?getActiveLandscapeID@CvGlobals@@QAEHXZ=?getActiveLandscapeID@EXE_CvGlobals@@QAEHXZ

		getAnimationCategoryInfo
			?getAnimationCategoryInfo@CvGlobals@@QAEAAVCvAnimationCategoryInfo@@W4AnimationCategoryTypes@@@Z=?getAnimationCategoryInfo@EXE_CvGlobals@@QAEAAVCvAnimationCategoryInfo@@W4AnimationCategoryTypes@@@Z

		getAnimationPathInfo
			?getAnimationPathInfo@CvGlobals@@QAEAAVCvAnimationPathInfo@@W4AnimationPathTypes@@@Z=?getAnimationPathInfo@EXE_CvGlobals@@QAEAAVCvAnimationPathInfo@@W4AnimationPathTypes@@@Z

		getAreaFinder
			?getAreaFinder@CvGlobals@@QAEAAVFAStar@@XZ=?getAreaFinder@EXE_CvGlobals@@QAEAAVFAStar@@XZ

		getArtStyleTypes
			?getArtStyleTypes@CvGlobals@@QAEAAVCvString@@W4ArtStyleTypes@@@Z=?getArtStyleTypes@EXE_CvGlobals@@QAEAAVCvString@@W4ArtStyleTypes@@@Z

		getAttachableInfo
			?getAttachableInfo@CvGlobals@@QAEAAVCvAttachableInfo@@H@Z=?getAttachableInfo@EXE_CvGlobals@@QAEAAVCvAttachableInfo@@H@Z

		getBonusInfo
			?getBonusInfo@CvGlobals@@QAEAAV?$vector@PAVCvBonusInfo@@V?$allocator@PAVCvBonusInfo@@@std@@@std@@XZ=?getBonusInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvBonusInfo@@V?$allocator@PAVCvBonusInfo@@@std@@@std@@XZ

		getBonusInfo
			?getBonusInfo@CvGlobals@@QAEAAVCvBonusInfo@@W4BonusTypes@@@Z=?getBonusInfo@EXE_CvGlobals@@QAEAAVCvBonusInfo@@W4BonusTypes@@@Z

		getBorderFinder
			?getBorderFinder@CvGlobals@@QAEAAVFAStar@@XZ=?getBorderFinder@EXE_CvGlobals@@QAEAAVFAStar@@XZ

		getBuildInfo
			?getBuildInfo@CvGlobals@@QAEAAVCvBuildInfo@@W4BuildTypes@@@Z=?getBuildInfo@EXE_CvGlobals@@QAEAAVCvBuildInfo@@W4BuildTypes@@@Z
*/
	DllExport std::vector<CvBuildingInfo*>& getBuildingInfo()
	{
		static std::vector<CvBuildingInfo*> vector;

		if (vector.size() == 0)
		{
			vector = CvGlobals::getBuildingInfo();
			while(vector.size() < static_cast<unsigned int>(getNumBuildingInfos()))
			{
				vector.push_back(&CvGlobals::getBuildingInfo(BUILDING_PLACEHOLDER));
			}
		}
		return vector;
	}

	DllExport CvBuildingInfo& getBuildingInfo(BuildingTypes eBuildingNum)
	{
		if (eBuildingNum < NUM_BUILDING_TYPES)
		{
			return CvGlobals::getBuildingInfo(eBuildingNum);
		}
		return CvGlobals::getBuildingInfo(BUILDING_PLACEHOLDER);
	}
			/*
		getCAMERA_FAR_CLIP_Z_HEIGHT
			?getCAMERA_FAR_CLIP_Z_HEIGHT@CvGlobals@@QAEMXZ=?getCAMERA_FAR_CLIP_Z_HEIGHT@EXE_CvGlobals@@QAEMXZ

		getCAMERA_LOWER_PITCH
			?getCAMERA_LOWER_PITCH@CvGlobals@@QAEMXZ=?getCAMERA_LOWER_PITCH@EXE_CvGlobals@@QAEMXZ

		getCAMERA_MAX_TRAVEL_DISTANCE
			?getCAMERA_MAX_TRAVEL_DISTANCE@CvGlobals@@QAEMXZ=?getCAMERA_MAX_TRAVEL_DISTANCE@EXE_CvGlobals@@QAEMXZ

		getCAMERA_MAX_TURN_OFFSET
			?getCAMERA_MAX_TURN_OFFSET@CvGlobals@@QAEMXZ=?getCAMERA_MAX_TURN_OFFSET@EXE_CvGlobals@@QAEMXZ

		getCAMERA_MAX_YAW
			?getCAMERA_MAX_YAW@CvGlobals@@QAEMXZ=?getCAMERA_MAX_YAW@EXE_CvGlobals@@QAEMXZ

		getCAMERA_MIN_DISTANCE
			?getCAMERA_MIN_DISTANCE@CvGlobals@@QAEMXZ=?getCAMERA_MIN_DISTANCE@EXE_CvGlobals@@QAEMXZ

		getCAMERA_MIN_YAW
			?getCAMERA_MIN_YAW@CvGlobals@@QAEMXZ=?getCAMERA_MIN_YAW@EXE_CvGlobals@@QAEMXZ

		getCAMERA_SPECIAL_PITCH
			?getCAMERA_SPECIAL_PITCH@CvGlobals@@QAEMXZ=?getCAMERA_SPECIAL_PITCH@EXE_CvGlobals@@QAEMXZ

		getCAMERA_START_DISTANCE
			?getCAMERA_START_DISTANCE@CvGlobals@@QAEMXZ=?getCAMERA_START_DISTANCE@EXE_CvGlobals@@QAEMXZ

		getCAMERA_UPPER_PITCH
			?getCAMERA_UPPER_PITCH@CvGlobals@@QAEMXZ=?getCAMERA_UPPER_PITCH@EXE_CvGlobals@@QAEMXZ

		getCameraOverlayInfo
			?getCameraOverlayInfo@CvGlobals@@QAEAAVCvCameraOverlayInfo@@H@Z=?getCameraOverlayInfo@EXE_CvGlobals@@QAEAAVCvCameraOverlayInfo@@H@Z

		getCivicInfo
			?getCivicInfo@CvGlobals@@QAEAAV?$vector@PAVCvCivicInfo@@V?$allocator@PAVCvCivicInfo@@@std@@@std@@XZ=?getCivicInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvCivicInfo@@V?$allocator@PAVCvCivicInfo@@@std@@@std@@XZ

		getCivilizationInfo
			?getCivilizationInfo@CvGlobals@@QAEAAV?$vector@PAVCvCivilizationInfo@@V?$allocator@PAVCvCivilizationInfo@@@std@@@std@@XZ=?getCivilizationInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvCivilizationInfo@@V?$allocator@PAVCvCivilizationInfo@@@std@@@std@@XZ

		getCivilizationInfo
			?getCivilizationInfo@CvGlobals@@QAEAAVCvCivilizationInfo@@W4CivilizationTypes@@@Z=?getCivilizationInfo@EXE_CvGlobals@@QAEAAVCvCivilizationInfo@@W4CivilizationTypes@@@Z

		getClimateInfo
			?getClimateInfo@CvGlobals@@QAEAAVCvClimateInfo@@W4ClimateTypes@@@Z=?getClimateInfo@EXE_CvGlobals@@QAEAAVCvClimateInfo@@W4ClimateTypes@@@Z

		getColorInfo
			?getColorInfo@CvGlobals@@QAEAAVCvColorInfo@@W4ColorTypes@@@Z=?getColorInfo@EXE_CvGlobals@@QAEAAVCvColorInfo@@W4ColorTypes@@@Z

		getCommandInfo
			?getCommandInfo@CvGlobals@@QAEAAVCvCommandInfo@@W4CommandTypes@@@Z=?getCommandInfo@EXE_CvGlobals@@QAEAAVCvCommandInfo@@W4CommandTypes@@@Z

		getCursorInfo
			?getCursorInfo@CvGlobals@@QAEAAVCvCursorInfo@@W4CursorTypes@@@Z=?getCursorInfo@EXE_CvGlobals@@QAEAAVCvCursorInfo@@W4CursorTypes@@@Z

		getDLLIFaceNonInl
			?getDLLIFaceNonInl@CvGlobals@@QAEPAVCvDLLUtilityIFaceBase@@XZ=?getDLLIFaceNonInl@EXE_CvGlobals@@QAEPAVCvDLLUtilityIFaceBase@@XZ

		getDefineFLOAT
			?getDefineFLOAT@CvGlobals@@QBEMPBD@Z=?getDefineFLOAT@EXE_CvGlobals@@QBEMPBD@Z

		getDefineINT
			?getDefineINT@CvGlobals@@QBEHPBD@Z=?getDefineINT@EXE_CvGlobals@@QBEHPBD@Z

		getDefineSTRING
			?getDefineSTRING@CvGlobals@@QBEPBDPBD@Z=?getDefineSTRING@EXE_CvGlobals@@QBEPBDPBD@Z

		getDefinesVarSystem
			?getDefinesVarSystem@CvGlobals@@QAEPAVFVariableSystem@@XZ=?getDefinesVarSystem@EXE_CvGlobals@@QAEPAVFVariableSystem@@XZ

		getDiplomacyInfo
			?getDiplomacyInfo@CvGlobals@@QAEAAV?$vector@PAVCvDiplomacyInfo@@V?$allocator@PAVCvDiplomacyInfo@@@std@@@std@@XZ=?getDiplomacyInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvDiplomacyInfo@@V?$allocator@PAVCvDiplomacyInfo@@@std@@@std@@XZ

		getDiplomacyScreen
			?getDiplomacyScreen@CvGlobals@@QAEPAVCvDiplomacyScreen@@XZ=?getDiplomacyScreen@EXE_CvGlobals@@QAEPAVCvDiplomacyScreen@@XZ

		getDropMgr
			?getDropMgr@CvGlobals@@QAEAAVCvDropMgr@@XZ=?getDropMgr@EXE_CvGlobals@@QAEAAVCvDropMgr@@XZ

		getEVENT_MESSAGE_TIME
			?getEVENT_MESSAGE_TIME@CvGlobals@@QAEHXZ=?getEVENT_MESSAGE_TIME@EXE_CvGlobals@@QAEHXZ

		getEffectInfo
			?getEffectInfo@CvGlobals@@QAEAAVCvEffectInfo@@H@Z=?getEffectInfo@EXE_CvGlobals@@QAEAAVCvEffectInfo@@H@Z

		getEntityEventInfo
			?getEntityEventInfo@CvGlobals@@QAEAAVCvEntityEventInfo@@W4EntityEventTypes@@@Z=?getEntityEventInfo@EXE_CvGlobals@@QAEAAVCvEntityEventInfo@@W4EntityEventTypes@@@Z

		getEraInfo
			?getEraInfo@CvGlobals@@QAEAAVCvEraInfo@@W4EraTypes@@@Z=?getEraInfo@EXE_CvGlobals@@QAEAAVCvEraInfo@@W4EraTypes@@@Z

		getEuropeInfo
			?getEuropeInfo@CvGlobals@@QAEAAVCvEuropeInfo@@W4EuropeTypes@@@Z=?getEuropeInfo@EXE_CvGlobals@@QAEAAVCvEuropeInfo@@W4EuropeTypes@@@Z

		getEventInfo
			?getEventInfo@CvGlobals@@QAEAAV?$vector@PAVCvEventInfo@@V?$allocator@PAVCvEventInfo@@@std@@@std@@XZ=?getEventInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvEventInfo@@V?$allocator@PAVCvEventInfo@@@std@@@std@@XZ

		getEventTriggerInfo
			?getEventTriggerInfo@CvGlobals@@QAEAAV?$vector@PAVCvEventTriggerInfo@@V?$allocator@PAVCvEventTriggerInfo@@@std@@@std@@XZ=?getEventTriggerInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvEventTriggerInfo@@V?$allocator@PAVCvEventTriggerInfo@@@std@@@std@@XZ

		getFIELD_OF_VIEW
			?getFIELD_OF_VIEW@CvGlobals@@QAEMXZ=?getFIELD_OF_VIEW@EXE_CvGlobals@@QAEMXZ

		getFMPMgrPtr
			?getFMPMgrPtr@CvGlobals@@QAEAAPAVFMPIManager@@XZ=?getFMPMgrPtr@EXE_CvGlobals@@QAEAAPAVFMPIManager@@XZ

		getFatherInfo
			?getFatherInfo@CvGlobals@@QAEAAV?$vector@PAVCvFatherInfo@@V?$allocator@PAVCvFatherInfo@@@std@@@std@@XZ=?getFatherInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvFatherInfo@@V?$allocator@PAVCvFatherInfo@@@std@@@std@@XZ

		getFatherPointInfo
			?getFatherPointInfo@CvGlobals@@QAEAAVCvFatherPointInfo@@W4FatherPointTypes@@@Z=?getFatherPointInfo@EXE_CvGlobals@@QAEAAVCvFatherPointInfo@@W4FatherPointTypes@@@Z

		getFeatureInfo
			?getFeatureInfo@CvGlobals@@QAEAAVCvFeatureInfo@@W4FeatureTypes@@@Z=?getFeatureInfo@EXE_CvGlobals@@QAEAAVCvFeatureInfo@@W4FeatureTypes@@@Z

		getFootstepAudioTags
			?getFootstepAudioTags@CvGlobals@@QAEAAVCvString@@H@Z=?getFootstepAudioTags@EXE_CvGlobals@@QAEAAVCvString@@H@Z

		getGame
			?getGame@CvGlobals@@QAEAAVCvGameAI@@XZ=?getGame@EXE_CvGlobals@@QAEAAVCvGameAI@@XZ

		getGameOptionInfo
			?getGameOptionInfo@CvGlobals@@QAEAAVCvGameOptionInfo@@W4GameOptionTypes@@@Z=?getGameOptionInfo@EXE_CvGlobals@@QAEAAVCvGameOptionInfo@@W4GameOptionTypes@@@Z

		getGamePointer
			?getGamePointer@CvGlobals@@QAEPAVCvGameAI@@XZ=?getGamePointer@EXE_CvGlobals@@QAEPAVCvGameAI@@XZ

		getGameSpeedInfo
			?getGameSpeedInfo@CvGlobals@@QAEAAVCvGameSpeedInfo@@W4GameSpeedTypes@@@Z=?getGameSpeedInfo@EXE_CvGlobals@@QAEAAVCvGameSpeedInfo@@W4GameSpeedTypes@@@Z

		getGraphicOptionInfo
			?getGraphicOptionInfo@CvGlobals@@QAEAAVCvGraphicOptionInfo@@W4GraphicOptionTypes@@@Z=?getGraphicOptionInfo@EXE_CvGlobals@@QAEAAVCvGraphicOptionInfo@@W4GraphicOptionTypes@@@Z

		getHandicapInfo
			?getHandicapInfo@CvGlobals@@QAEAAV?$vector@PAVCvHandicapInfo@@V?$allocator@PAVCvHandicapInfo@@@std@@@std@@XZ=?getHandicapInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvHandicapInfo@@V?$allocator@PAVCvHandicapInfo@@@std@@@std@@XZ

		getHandicapInfo
			?getHandicapInfo@CvGlobals@@QAEAAVCvHandicapInfo@@W4HandicapTypes@@@Z=?getHandicapInfo@EXE_CvGlobals@@QAEAAVCvHandicapInfo@@W4HandicapTypes@@@Z

		getHints
			?getHints@CvGlobals@@QAEAAVCvInfoBase@@H@Z=?getHints@EXE_CvGlobals@@QAEAAVCvInfoBase@@H@Z

		getHotMessageQueue
			?getHotMessageQueue@CvGlobals@@QAEAAVCMessageQueue@@XZ=?getHotMessageQueue@EXE_CvGlobals@@QAEAAVCMessageQueue@@XZ

		getImprovementInfo
			?getImprovementInfo@CvGlobals@@QAEAAV?$vector@PAVCvImprovementInfo@@V?$allocator@PAVCvImprovementInfo@@@std@@@std@@XZ=?getImprovementInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvImprovementInfo@@V?$allocator@PAVCvImprovementInfo@@@std@@@std@@XZ

		getImprovementInfo
			?getImprovementInfo@CvGlobals@@QAEAAVCvImprovementInfo@@W4ImprovementTypes@@@Z=?getImprovementInfo@EXE_CvGlobals@@QAEAAVCvImprovementInfo@@W4ImprovementTypes@@@Z

		getInfoTypeForString
			?getInfoTypeForString@CvGlobals@@QBEHPBD_N@Z=?getInfoTypeForString@EXE_CvGlobals@@QBEHPBD_N@Z

		getIniInitCore
			?getIniInitCore@CvGlobals@@QAEAAVCvInitCore@@XZ=?getIniInitCore@EXE_CvGlobals@@QAEAAVCvInitCore@@XZ

		getInitCore
			?getInitCore@CvGlobals@@QAEAAVCvInitCore@@XZ=?getInitCore@EXE_CvGlobals@@QAEAAVCvInitCore@@XZ

		getInstance
			?getInstance@CvGlobals@@SAAAV1@XZ=?getInstance@EXE_CvGlobals@@SAAAV1@XZ

		getInterface
			?getInterface@CvGlobals@@QAEAAVCvInterface@@XZ=?getInterface@EXE_CvGlobals@@QAEAAVCvInterface@@XZ

		getInterfaceModeInfo
			?getInterfaceModeInfo@CvGlobals@@QAEAAV?$vector@PAVCvInterfaceModeInfo@@V?$allocator@PAVCvInterfaceModeInfo@@@std@@@std@@XZ=?getInterfaceModeInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvInterfaceModeInfo@@V?$allocator@PAVCvInterfaceModeInfo@@@std@@@std@@XZ

		getInterfaceModeInfo
			?getInterfaceModeInfo@CvGlobals@@QAEAAVCvInterfaceModeInfo@@W4InterfaceModeTypes@@@Z=?getInterfaceModeInfo@EXE_CvGlobals@@QAEAAVCvInterfaceModeInfo@@W4InterfaceModeTypes@@@Z

		getInterfacePathFinder
			?getInterfacePathFinder@CvGlobals@@QAEAAVFAStar@@XZ=?getInterfacePathFinder@EXE_CvGlobals@@QAEAAVFAStar@@XZ

		getInterfacePtr
			?getInterfacePtr@CvGlobals@@QAEPAVCvInterface@@XZ=?getInterfacePtr@EXE_CvGlobals@@QAEPAVCvInterface@@XZ

		getLandscapeInfo
			?getLandscapeInfo@CvGlobals@@QAEAAVCvLandscapeInfo@@H@Z=?getLandscapeInfo@EXE_CvGlobals@@QAEAAVCvLandscapeInfo@@H@Z

		getLeaderHeadInfo
			?getLeaderHeadInfo@CvGlobals@@QAEAAV?$vector@PAVCvLeaderHeadInfo@@V?$allocator@PAVCvLeaderHeadInfo@@@std@@@std@@XZ=?getLeaderHeadInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvLeaderHeadInfo@@V?$allocator@PAVCvLeaderHeadInfo@@@std@@@std@@XZ

		getLeaderHeadInfo
			?getLeaderHeadInfo@CvGlobals@@QAEAAVCvLeaderHeadInfo@@W4LeaderHeadTypes@@@Z=?getLeaderHeadInfo@EXE_CvGlobals@@QAEAAVCvLeaderHeadInfo@@W4LeaderHeadTypes@@@Z

		getLoadedInitCore
			?getLoadedInitCore@CvGlobals@@QAEAAVCvInitCore@@XZ=?getLoadedInitCore@EXE_CvGlobals@@QAEAAVCvInitCore@@XZ

		getLogging
			?getLogging@CvGlobals@@QAEAA_NXZ=?getLogging@EXE_CvGlobals@@QAEAA_NXZ

		getMAX_CIV_PLAYERS
			?getMAX_CIV_PLAYERS@CvGlobals@@QAEHXZ=?getMAX_CIV_PLAYERS@EXE_CvGlobals@@QAEHXZ

		getMAX_PLOT_LIST_ROWS
			?getMAX_PLOT_LIST_ROWS@CvGlobals@@QAEHXZ=?getMAX_PLOT_LIST_ROWS@EXE_CvGlobals@@QAEHXZ

		getMPDiplomacyScreen
			?getMPDiplomacyScreen@CvGlobals@@QAEPAVCMPDiplomacyScreen@@XZ=?getMPDiplomacyScreen@EXE_CvGlobals@@QAEPAVCMPDiplomacyScreen@@XZ

		getMPOptionInfo
			?getMPOptionInfo@CvGlobals@@QAEAAVCvMPOptionInfo@@W4MultiplayerOptionTypes@@@Z=?getMPOptionInfo@EXE_CvGlobals@@QAEAAVCvMPOptionInfo@@W4MultiplayerOptionTypes@@@Z

		getMainMenus
			?getMainMenus@CvGlobals@@QAEAAVCvMainMenuInfo@@H@Z=?getMainMenus@EXE_CvGlobals@@QAEAAVCvMainMenuInfo@@H@Z
	*/
	DllExport CvMap& getMap()
	{
		return CvGlobals::getMap();
	}
	/*
		getMaxCivPlayers
			?getMaxCivPlayers@CvGlobals@@QBEHXZ=?getMaxCivPlayers@EXE_CvGlobals@@QBEHXZ

		getMaxNumSymbols
			?getMaxNumSymbols@CvGlobals@@QBEHXZ=?getMaxNumSymbols@EXE_CvGlobals@@QBEHXZ

		getMessageCodes
			?getMessageCodes@CvGlobals@@QAEAAVCvMessageCodeTranslator@@XZ=?getMessageCodes@EXE_CvGlobals@@QAEAAVCvMessageCodeTranslator@@XZ

		getMessageControl
			?getMessageControl@CvGlobals@@QAEAAVCMessageControl@@XZ=?getMessageControl@EXE_CvGlobals@@QAEAAVCMessageControl@@XZ

		getMessageQueue
			?getMessageQueue@CvGlobals@@QAEAAVCMessageQueue@@XZ=?getMessageQueue@EXE_CvGlobals@@QAEAAVCMessageQueue@@XZ

		getMissionInfo
			?getMissionInfo@CvGlobals@@QAEAAVCvMissionInfo@@W4MissionTypes@@@Z=?getMissionInfo@EXE_CvGlobals@@QAEAAVCvMissionInfo@@W4MissionTypes@@@Z

		getNUM_ENGINE_DIRTY_BITS
			?getNUM_ENGINE_DIRTY_BITS@CvGlobals@@QBEHXZ=?getNUM_ENGINE_DIRTY_BITS@EXE_CvGlobals@@QBEHXZ

		getNUM_FORCECONTROL_TYPES
			?getNUM_FORCECONTROL_TYPES@CvGlobals@@QBEHXZ=?getNUM_FORCECONTROL_TYPES@EXE_CvGlobals@@QBEHXZ

		getNUM_HEALTHBAR_TYPES
			?getNUM_HEALTHBAR_TYPES@CvGlobals@@QBEHXZ=?getNUM_HEALTHBAR_TYPES@EXE_CvGlobals@@QBEHXZ

		getNUM_INFOBAR_TYPES
			?getNUM_INFOBAR_TYPES@CvGlobals@@QBEHXZ=?getNUM_INFOBAR_TYPES@EXE_CvGlobals@@QBEHXZ

		getNUM_INTERFACE_DIRTY_BITS
			?getNUM_INTERFACE_DIRTY_BITS@CvGlobals@@QBEHXZ=?getNUM_INTERFACE_DIRTY_BITS@EXE_CvGlobals@@QBEHXZ

		getNUM_LEADERANIM_TYPES
			?getNUM_LEADERANIM_TYPES@CvGlobals@@QBEHXZ=?getNUM_LEADERANIM_TYPES@EXE_CvGlobals@@QBEHXZ
		*/

	DllExport int getNUM_YIELD_TYPES() const
	{
		return NUM_YIELD_TYPES;
	}

		/*

		getNumActionInfos
			?getNumActionInfos@CvGlobals@@QAEHXZ=?getNumActionInfos@EXE_CvGlobals@@QAEHXZ

		getNumArtStyleTypes
			?getNumArtStyleTypes@CvGlobals@@QAEAAHXZ=?getNumArtStyleTypes@EXE_CvGlobals@@QAEAAHXZ

		getNumBasicItems
			?getNumBasicItems@CvGlobals@@QBEHXZ=?getNumBasicItems@EXE_CvGlobals@@QBEHXZ

		getNumBonusInfos
			?getNumBonusInfos@CvGlobals@@QAEHXZ=?getNumBonusInfos@EXE_CvGlobals@@QAEHXZ

		getNumBuildingClassInfos
			?getNumBuildingClassInfos@CvGlobals@@QAEHXZ=?getNumBuildingClassInfos@EXE_CvGlobals@@QAEHXZ
			*/

	DllExport int getNumBuildingInfos()
	{
		// show the exe an arbitrarily high number of building types because that somehow affects savegame padding
		// the free slots will be directed to the BUILDING_PLACEHOLDER by getBuildingInfo()
		// This is meant to be a workaround for savegames becoming incompatible each time a building is added or removed
		if (!m_bExeXmlLengthOverride)
		{
			return BUILDING_PLACEHOLDER;
		}
		static int iNumBuildings = 0;
		if (iNumBuildings == 0)
		{
			iNumBuildings = SAVEGAME_EXE_FALSE_XML_BUILDINGS;
			while (iNumBuildings <= NUM_BUILDING_TYPES)
			{
				iNumBuildings += 25;
			}
		}
		return iNumBuildings;
	}

	/*
		getNumCameraOverlayInfos
			?getNumCameraOverlayInfos@CvGlobals@@QAEHXZ=?getNumCameraOverlayInfos@EXE_CvGlobals@@QAEHXZ

		getNumCivicOptionInfos
			?getNumCivicOptionInfos@CvGlobals@@QAEHXZ=?getNumCivicOptionInfos@EXE_CvGlobals@@QAEHXZ

		getNumCivilizationInfos
			?getNumCivilizationInfos@CvGlobals@@QAEHXZ=?getNumCivilizationInfos@EXE_CvGlobals@@QAEHXZ

		getNumClimateInfos
			?getNumClimateInfos@CvGlobals@@QAEHXZ=?getNumClimateInfos@EXE_CvGlobals@@QAEHXZ

		getNumColorInfos
			?getNumColorInfos@CvGlobals@@QAEHXZ=?getNumColorInfos@EXE_CvGlobals@@QAEHXZ

		getNumCursorInfos
			?getNumCursorInfos@CvGlobals@@QAEHXZ=?getNumCursorInfos@EXE_CvGlobals@@QAEHXZ

		getNumDirections
			?getNumDirections@CvGlobals@@QBEHXZ=?getNumDirections@EXE_CvGlobals@@QBEHXZ

		getNumEraInfos
			?getNumEraInfos@CvGlobals@@QAEHXZ=?getNumEraInfos@EXE_CvGlobals@@QAEHXZ

		getNumEuropeInfos
			?getNumEuropeInfos@CvGlobals@@QAEHXZ=?getNumEuropeInfos@EXE_CvGlobals@@QAEHXZ

		getNumFatherPointInfos
			?getNumFatherPointInfos@CvGlobals@@QAEHXZ=?getNumFatherPointInfos@EXE_CvGlobals@@QAEHXZ

		getNumFeatureInfos
			?getNumFeatureInfos@CvGlobals@@QAEHXZ=?getNumFeatureInfos@EXE_CvGlobals@@QAEHXZ

		getNumFootstepAudioTypes
			?getNumFootstepAudioTypes@CvGlobals@@QAEAAHXZ=?getNumFootstepAudioTypes@EXE_CvGlobals@@QAEAAHXZ

		getNumGameOptions
			?getNumGameOptions@CvGlobals@@QBEHXZ=?getNumGameOptions@EXE_CvGlobals@@QBEHXZ

		getNumGameSpeedInfos
			?getNumGameSpeedInfos@CvGlobals@@QAEHXZ=?getNumGameSpeedInfos@EXE_CvGlobals@@QAEHXZ

		getNumGraphicLevels
			?getNumGraphicLevels@CvGlobals@@QBEHXZ=?getNumGraphicLevels@EXE_CvGlobals@@QBEHXZ

		getNumGraphicOptions
			?getNumGraphicOptions@CvGlobals@@QBEHXZ=?getNumGraphicOptions@EXE_CvGlobals@@QBEHXZ

		getNumHandicapInfos
			?getNumHandicapInfos@CvGlobals@@QAEHXZ=?getNumHandicapInfos@EXE_CvGlobals@@QAEHXZ

		getNumHints
			?getNumHints@CvGlobals@@QAEHXZ=?getNumHints@EXE_CvGlobals@@QAEHXZ

		getNumHurryInfos
			?getNumHurryInfos@CvGlobals@@QAEHXZ=?getNumHurryInfos@EXE_CvGlobals@@QAEHXZ

		getNumImprovementInfos
			?getNumImprovementInfos@CvGlobals@@QAEHXZ=?getNumImprovementInfos@EXE_CvGlobals@@QAEHXZ

		getNumLeaderHeadInfos
			?getNumLeaderHeadInfos@CvGlobals@@QAEHXZ=?getNumLeaderHeadInfos@EXE_CvGlobals@@QAEHXZ

		getNumMPOptions
			?getNumMPOptions@CvGlobals@@QBEHXZ=?getNumMPOptions@EXE_CvGlobals@@QBEHXZ

		getNumPlayerColorInfos
			?getNumPlayerColorInfos@CvGlobals@@QAEHXZ=?getNumPlayerColorInfos@EXE_CvGlobals@@QAEHXZ

		getNumPlayerOptionInfos
			?getNumPlayerOptionInfos@CvGlobals@@QBEHXZ=?getNumPlayerOptionInfos@EXE_CvGlobals@@QBEHXZ

		getNumPromotionInfos
			?getNumPromotionInfos@CvGlobals@@QAEHXZ=?getNumPromotionInfos@EXE_CvGlobals@@QAEHXZ

		getNumResourceLayers
			?getNumResourceLayers@CvGlobals@@QBEHXZ=?getNumResourceLayers@EXE_CvGlobals@@QBEHXZ

		getNumRiverModelInfos
			?getNumRiverModelInfos@CvGlobals@@QAEHXZ=?getNumRiverModelInfos@EXE_CvGlobals@@QAEHXZ

		getNumRouteInfos
			?getNumRouteInfos@CvGlobals@@QAEHXZ=?getNumRouteInfos@EXE_CvGlobals@@QAEHXZ

		getNumRouteModelInfos
			?getNumRouteModelInfos@CvGlobals@@QAEHXZ=?getNumRouteModelInfos@EXE_CvGlobals@@QAEHXZ

		getNumSeaLevelInfos
			?getNumSeaLevelInfos@CvGlobals@@QAEHXZ=?getNumSeaLevelInfos@EXE_CvGlobals@@QAEHXZ

		getNumSlideShowInfos
			?getNumSlideShowInfos@CvGlobals@@QAEHXZ=?getNumSlideShowInfos@EXE_CvGlobals@@QAEHXZ

		getNumSlideShowRandomInfos
			?getNumSlideShowRandomInfos@CvGlobals@@QAEHXZ=?getNumSlideShowRandomInfos@EXE_CvGlobals@@QAEHXZ

		getNumSpecialBuildingInfos
			?getNumSpecialBuildingInfos@CvGlobals@@QAEHXZ=?getNumSpecialBuildingInfos@EXE_CvGlobals@@QAEHXZ

		getNumSpecialOptions
			?getNumSpecialOptions@CvGlobals@@QBEHXZ=?getNumSpecialOptions@EXE_CvGlobals@@QBEHXZ

		getNumTerrainInfos
			?getNumTerrainInfos@CvGlobals@@QAEHXZ=?getNumTerrainInfos@EXE_CvGlobals@@QAEHXZ

		getNumTerrainPlaneInfos
			?getNumTerrainPlaneInfos@CvGlobals@@QAEHXZ=?getNumTerrainPlaneInfos@EXE_CvGlobals@@QAEHXZ

		getNumTradeableHeadings
			?getNumTradeableHeadings@CvGlobals@@QBEHXZ=?getNumTradeableHeadings@EXE_CvGlobals@@QBEHXZ

		getNumTradeableItems
			?getNumTradeableItems@CvGlobals@@QBEHXZ=?getNumTradeableItems@EXE_CvGlobals@@QBEHXZ

		getNumTurnTimerInfos
			?getNumTurnTimerInfos@CvGlobals@@QAEHXZ=?getNumTurnTimerInfos@EXE_CvGlobals@@QAEHXZ

		getNumUnitClassInfos
			?getNumUnitClassInfos@CvGlobals@@QAEHXZ=?getNumUnitClassInfos@EXE_CvGlobals@@QAEHXZ

		getNumUnitFormationInfos
			?getNumUnitFormationInfos@CvGlobals@@QAEHXZ=?getNumUnitFormationInfos@EXE_CvGlobals@@QAEHXZ
			*/

	DllExport int getNumUnitInfos()
	{
		// show the exe an arbitrarily high number of unit types because that somehow affects savegame padding
		// the free slots will be directed to the UNIT_PLACEHOLDER by getUnitInfo()
		// This is meant to be a workaround for savegames becoming incompatible each time a unit is added or removed
		if (!m_bExeXmlLengthOverride)
		{
			return UNIT_PLACEHOLDER;
		}
		static int iNumUnits = 0;
		if (iNumUnits == 0)
		{
			iNumUnits = SAVEGAME_EXE_FALSE_XML_UNITS;
			while (iNumUnits <= NUM_UNIT_TYPES)
			{
				iNumUnits += 25;
			}
		}
		return iNumUnits;
	}

			/*
		getNumUnitLayerOptionTypes
			?getNumUnitLayerOptionTypes@CvGlobals@@QBEHXZ=?getNumUnitLayerOptionTypes@EXE_CvGlobals@@QBEHXZ

		getNumVictoryInfos
			?getNumVictoryInfos@CvGlobals@@QAEHXZ=?getNumVictoryInfos@EXE_CvGlobals@@QAEHXZ

		getNumWorldInfos
			?getNumWorldInfos@CvGlobals@@QAEHXZ=?getNumWorldInfos@EXE_CvGlobals@@QAEHXZ

		getNumWorldPickerInfos
			?getNumWorldPickerInfos@CvGlobals@@QAEHXZ=?getNumWorldPickerInfos@EXE_CvGlobals@@QAEHXZ

		getPLOT_SIZE
			?getPLOT_SIZE@CvGlobals@@QAEMXZ=?getPLOT_SIZE@EXE_CvGlobals@@QAEMXZ

		getPathFinder
			?getPathFinder@CvGlobals@@QAEAAVFAStar@@XZ=?getPathFinder@EXE_CvGlobals@@QAEAAVFAStar@@XZ

		getPlayerColorInfo
			?getPlayerColorInfo@CvGlobals@@QAEAAVCvPlayerColorInfo@@W4PlayerColorTypes@@@Z=?getPlayerColorInfo@EXE_CvGlobals@@QAEAAVCvPlayerColorInfo@@W4PlayerColorTypes@@@Z

		getPlayerOptionInfo
			?getPlayerOptionInfo@CvGlobals@@QAEAAVCvPlayerOptionInfo@@W4PlayerOptionTypes@@@Z=?getPlayerOptionInfo@EXE_CvGlobals@@QAEAAVCvPlayerOptionInfo@@W4PlayerOptionTypes@@@Z

		getPlotDirectionX
			?getPlotDirectionX@CvGlobals@@QAEPAHXZ=?getPlotDirectionX@EXE_CvGlobals@@QAEPAHXZ

		getPlotDirectionY
			?getPlotDirectionY@CvGlobals@@QAEPAHXZ=?getPlotDirectionY@EXE_CvGlobals@@QAEPAHXZ

		getPortal
			?getPortal@CvGlobals@@QAEAAVCvPortal@@XZ=?getPortal@EXE_CvGlobals@@QAEAAVCvPortal@@XZ

		getProfessionInfo
			?getProfessionInfo@CvGlobals@@QAEAAV?$vector@PAVCvProfessionInfo@@V?$allocator@PAVCvProfessionInfo@@@std@@@std@@XZ=?getProfessionInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvProfessionInfo@@V?$allocator@PAVCvProfessionInfo@@@std@@@std@@XZ

		getPromotionInfo
			?getPromotionInfo@CvGlobals@@QAEAAV?$vector@PAVCvPromotionInfo@@V?$allocator@PAVCvPromotionInfo@@@std@@@std@@XZ=?getPromotionInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvPromotionInfo@@V?$allocator@PAVCvPromotionInfo@@@std@@@std@@XZ

		getPromotionInfo
			?getPromotionInfo@CvGlobals@@QAEAAVCvPromotionInfo@@W4PromotionTypes@@@Z=?getPromotionInfo@EXE_CvGlobals@@QAEAAVCvPromotionInfo@@W4PromotionTypes@@@Z

		getRandLogging
			?getRandLogging@CvGlobals@@QAEAA_NXZ=?getRandLogging@EXE_CvGlobals@@QAEAA_NXZ

		getRiverModelInfo
			?getRiverModelInfo@CvGlobals@@QAEAAVCvRiverModelInfo@@H@Z=?getRiverModelInfo@EXE_CvGlobals@@QAEAAVCvRiverModelInfo@@H@Z

		getRouteFinder
			?getRouteFinder@CvGlobals@@QAEAAVFAStar@@XZ=?getRouteFinder@EXE_CvGlobals@@QAEAAVFAStar@@XZ

		getRouteInfo
			?getRouteInfo@CvGlobals@@QAEAAVCvRouteInfo@@W4RouteTypes@@@Z=?getRouteInfo@EXE_CvGlobals@@QAEAAVCvRouteInfo@@W4RouteTypes@@@Z

		getRouteModelInfo
			?getRouteModelInfo@CvGlobals@@QAEAAVCvRouteModelInfo@@H@Z=?getRouteModelInfo@EXE_CvGlobals@@QAEAAVCvRouteModelInfo@@H@Z

		getSeaLevelInfo
			?getSeaLevelInfo@CvGlobals@@QAEAAVCvSeaLevelInfo@@W4SeaLevelTypes@@@Z=?getSeaLevelInfo@EXE_CvGlobals@@QAEAAVCvSeaLevelInfo@@W4SeaLevelTypes@@@Z

		getSetupData
			?getSetupData@CvGlobals@@QAEAAVCvSetupData@@XZ=?getSetupData@EXE_CvGlobals@@QAEAAVCvSetupData@@XZ

		getSlideShowInfo
			?getSlideShowInfo@CvGlobals@@QAEAAVCvSlideShowInfo@@H@Z=?getSlideShowInfo@EXE_CvGlobals@@QAEAAVCvSlideShowInfo@@H@Z

		getSlideShowRandomInfo
			?getSlideShowRandomInfo@CvGlobals@@QAEAAVCvSlideShowRandomInfo@@H@Z=?getSlideShowRandomInfo@EXE_CvGlobals@@QAEAAVCvSlideShowRandomInfo@@H@Z

		getSpecialBuildingInfo
			?getSpecialBuildingInfo@CvGlobals@@QAEAAVCvSpecialBuildingInfo@@W4SpecialBuildingTypes@@@Z=?getSpecialBuildingInfo@EXE_CvGlobals@@QAEAAVCvSpecialBuildingInfo@@W4SpecialBuildingTypes@@@Z

		getStatsReporter
			?getStatsReporter@CvGlobals@@QAEAAVCvStatsReporter@@XZ=?getStatsReporter@EXE_CvGlobals@@QAEAAVCvStatsReporter@@XZ

		getStepFinder
			?getStepFinder@CvGlobals@@QAEAAVFAStar@@XZ=?getStepFinder@EXE_CvGlobals@@QAEAAVFAStar@@XZ

		getSynchLogging
			?getSynchLogging@CvGlobals@@QAEAA_NXZ=?getSynchLogging@EXE_CvGlobals@@QAEAA_NXZ

		getTerrainInfo
			?getTerrainInfo@CvGlobals@@QAEAAVCvTerrainInfo@@W4TerrainTypes@@@Z=?getTerrainInfo@EXE_CvGlobals@@QAEAAVCvTerrainInfo@@W4TerrainTypes@@@Z

		getTerrainPlaneInfo
			?getTerrainPlaneInfo@CvGlobals@@QAEAAVCvTerrainPlaneInfo@@H@Z=?getTerrainPlaneInfo@EXE_CvGlobals@@QAEAAVCvTerrainPlaneInfo@@H@Z

		getTraitInfo
			?getTraitInfo@CvGlobals@@QAEAAV?$vector@PAVCvTraitInfo@@V?$allocator@PAVCvTraitInfo@@@std@@@std@@XZ=?getTraitInfo@EXE_CvGlobals@@QAEAAV?$vector@PAVCvTraitInfo@@V?$allocator@PAVCvTraitInfo@@@std@@@std@@XZ

		getTurnTimerInfo
			?getTurnTimerInfo@CvGlobals@@QAEAAVCvTurnTimerInfo@@W4TurnTimerTypes@@@Z=?getTurnTimerInfo@EXE_CvGlobals@@QAEAAVCvTurnTimerInfo@@W4TurnTimerTypes@@@Z

		getUNIT_MULTISELECT_DISTANCE
			?getUNIT_MULTISELECT_DISTANCE@CvGlobals@@QAEMXZ=?getUNIT_MULTISELECT_DISTANCE@EXE_CvGlobals@@QAEMXZ

		getUNIT_MULTISELECT_MAX
			?getUNIT_MULTISELECT_MAX@CvGlobals@@QAEHXZ=?getUNIT_MULTISELECT_MAX@EXE_CvGlobals@@QAEHXZ

		getUSE_FINISH_TEXT_CALLBACK
			?getUSE_FINISH_TEXT_CALLBACK@CvGlobals@@QAEHXZ=?getUSE_FINISH_TEXT_CALLBACK@EXE_CvGlobals@@QAEHXZ

		getUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK
			?getUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK@CvGlobals@@QAEHXZ=?getUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK@EXE_CvGlobals@@QAEHXZ

		getUSE_ON_UNIT_CREATED_CALLBACK
			?getUSE_ON_UNIT_CREATED_CALLBACK@CvGlobals@@QAEHXZ=?getUSE_ON_UNIT_CREATED_CALLBACK@EXE_CvGlobals@@QAEHXZ

		getUSE_ON_UNIT_LOST_CALLBACK
			?getUSE_ON_UNIT_LOST_CALLBACK@CvGlobals@@QAEHXZ=?getUSE_ON_UNIT_LOST_CALLBACK@EXE_CvGlobals@@QAEHXZ

		getUSE_ON_UNIT_SELECTED_CALLBACK
			?getUSE_ON_UNIT_SELECTED_CALLBACK@CvGlobals@@QAEHXZ=?getUSE_ON_UNIT_SELECTED_CALLBACK@EXE_CvGlobals@@QAEHXZ

		getUSE_ON_UNIT_SET_XY_CALLBACK
			?getUSE_ON_UNIT_SET_XY_CALLBACK@CvGlobals@@QAEHXZ=?getUSE_ON_UNIT_SET_XY_CALLBACK@EXE_CvGlobals@@QAEHXZ

		getUSE_ON_UPDATE_CALLBACK
			?getUSE_ON_UPDATE_CALLBACK@CvGlobals@@QAEHXZ=?getUSE_ON_UPDATE_CALLBACK@EXE_CvGlobals@@QAEHXZ

		getUnitFormationInfo
			?getUnitFormationInfo@CvGlobals@@QAEAAVCvUnitFormationInfo@@H@Z=?getUnitFormationInfo@EXE_CvGlobals@@QAEAAVCvUnitFormationInfo@@H@Z
			*/

	// this one might not be necessary
	DllExport std::vector<CvUnitInfo*>& getUnitInfo()
	{
		static std::vector<CvUnitInfo*> vector;

		if (vector.size() == 0)
		{
			vector = CvGlobals::getUnitInfo();
			while(vector.size() < static_cast<unsigned int>(getNumUnitInfos()))
			{
				vector.push_back(&CvGlobals::getUnitInfo(UNIT_PLACEHOLDER));
			}
		}
		return vector;
	}

	// this is probably the relevant one
	DllExport	CvUnitInfo& getUnitInfo(UnitTypes eUnitNum)
	{
		if (eUnitNum < NUM_UNIT_TYPES)
		{
			return CvGlobals::getUnitInfo(eUnitNum);
		}
		return CvGlobals::getUnitInfo(UNIT_PLACEHOLDER);
	}

			/*
		getVictoryInfo
			?getVictoryInfo@CvGlobals@@QAEAAVCvVictoryInfo@@W4VictoryTypes@@@Z=?getVictoryInfo@EXE_CvGlobals@@QAEAAVCvVictoryInfo@@W4VictoryTypes@@@Z

		getWaterPlaneInfo
			?getWaterPlaneInfo@CvGlobals@@QAEAAVCvWaterPlaneInfo@@H@Z=?getWaterPlaneInfo@EXE_CvGlobals@@QAEAAVCvWaterPlaneInfo@@H@Z

		getWorldInfo
			?getWorldInfo@CvGlobals@@QAEAAVCvWorldInfo@@W4WorldSizeTypes@@@Z=?getWorldInfo@EXE_CvGlobals@@QAEAAVCvWorldInfo@@W4WorldSizeTypes@@@Z

		getWorldPickerInfo
			?getWorldPickerInfo@CvGlobals@@QAEAAVCvWorldPickerInfo@@H@Z=?getWorldPickerInfo@EXE_CvGlobals@@QAEAAVCvWorldPickerInfo@@H@Z

		getXYDirection
			?getXYDirection@CvGlobals@@QAE?AW4DirectionTypes@@HH@Z=?getXYDirection@EXE_CvGlobals@@QAE?AW4DirectionTypes@@HH@Z

		getYieldInfo
			?getYieldInfo@CvGlobals@@QAEAAVCvYieldInfo@@W4YieldTypes@@@Z=?getYieldInfo@EXE_CvGlobals@@QAEAAVCvYieldInfo@@W4YieldTypes@@@Z

		infoTypeFromStringReset
			?infoTypeFromStringReset@CvGlobals@@QAEXXZ=?infoTypeFromStringReset@EXE_CvGlobals@@QAEXXZ

		infosReset
			?infosReset@CvGlobals@@QAEXXZ=?infosReset@EXE_CvGlobals@@QAEXXZ

		init
			?init@CvGlobals@@QAEXXZ=?init@EXE_CvGlobals@@QAEXXZ

		overwriteLogs
			?overwriteLogs@CvGlobals@@QAEAA_NXZ=?overwriteLogs@EXE_CvGlobals@@QAEAA_NXZ

		setActiveLandscapeID
			?setActiveLandscapeID@CvGlobals@@QAEXH@Z=?setActiveLandscapeID@EXE_CvGlobals@@QAEXH@Z

		setAreaFinder
			?setAreaFinder@CvGlobals@@QAEXPAVFAStar@@@Z=?setAreaFinder@EXE_CvGlobals@@QAEXPAVFAStar@@@Z

		setBorderFinder
			?setBorderFinder@CvGlobals@@QAEXPAVFAStar@@@Z=?setBorderFinder@EXE_CvGlobals@@QAEXPAVFAStar@@@Z

		setDLLIFace
			?setDLLIFace@CvGlobals@@QAEXPAVCvDLLUtilityIFaceBase@@@Z=?setDLLIFace@EXE_CvGlobals@@QAEXPAVCvDLLUtilityIFaceBase@@@Z

		setDiplomacyScreen
			?setDiplomacyScreen@CvGlobals@@QAEXPAVCvDiplomacyScreen@@@Z=?setDiplomacyScreen@EXE_CvGlobals@@QAEXPAVCvDiplomacyScreen@@@Z

		setDropMgr
			?setDropMgr@CvGlobals@@QAEXPAVCvDropMgr@@@Z=?setDropMgr@EXE_CvGlobals@@QAEXPAVCvDropMgr@@@Z

		setHotJoinMessageQueue
			?setHotJoinMessageQueue@CvGlobals@@QAEXPAVCMessageQueue@@@Z=?setHotJoinMessageQueue@EXE_CvGlobals@@QAEXPAVCMessageQueue@@@Z

		setInfoTypeFromString
			?setInfoTypeFromString@CvGlobals@@QAEXPBDH@Z=?setInfoTypeFromString@EXE_CvGlobals@@QAEXPBDH@Z

		setInterface
			?setInterface@CvGlobals@@QAEXPAVCvInterface@@@Z=?setInterface@EXE_CvGlobals@@QAEXPAVCvInterface@@@Z

		setInterfacePathFinder
			?setInterfacePathFinder@CvGlobals@@QAEXPAVFAStar@@@Z=?setInterfacePathFinder@EXE_CvGlobals@@QAEXPAVFAStar@@@Z

		setMPDiplomacyScreen
			?setMPDiplomacyScreen@CvGlobals@@QAEXPAVCMPDiplomacyScreen@@@Z=?setMPDiplomacyScreen@EXE_CvGlobals@@QAEXPAVCMPDiplomacyScreen@@@Z

		setMessageCodeTranslator
			?setMessageCodeTranslator@CvGlobals@@QAEXPAVCvMessageCodeTranslator@@@Z=?setMessageCodeTranslator@EXE_CvGlobals@@QAEXPAVCvMessageCodeTranslator@@@Z

		setMessageControl
			?setMessageControl@CvGlobals@@QAEXPAVCMessageControl@@@Z=?setMessageControl@EXE_CvGlobals@@QAEXPAVCMessageControl@@@Z

		setMessageQueue
			?setMessageQueue@CvGlobals@@QAEXPAVCMessageQueue@@@Z=?setMessageQueue@EXE_CvGlobals@@QAEXPAVCMessageQueue@@@Z

		setPathFinder
			?setPathFinder@CvGlobals@@QAEXPAVFAStar@@@Z=?setPathFinder@EXE_CvGlobals@@QAEXPAVFAStar@@@Z

		setPortal
			?setPortal@CvGlobals@@QAEXPAVCvPortal@@@Z=?setPortal@EXE_CvGlobals@@QAEXPAVCvPortal@@@Z

		setRouteFinder
			?setRouteFinder@CvGlobals@@QAEXPAVFAStar@@@Z=?setRouteFinder@EXE_CvGlobals@@QAEXPAVFAStar@@@Z

		setSetupData
			?setSetupData@CvGlobals@@QAEXPAVCvSetupData@@@Z=?setSetupData@EXE_CvGlobals@@QAEXPAVCvSetupData@@@Z

		setStatsReport
			?setStatsReport@CvGlobals@@QAEXPAVCvStatsReporter@@@Z=?setStatsReport@EXE_CvGlobals@@QAEXPAVCvStatsReporter@@@Z

		setStepFinder
			?setStepFinder@CvGlobals@@QAEXPAVFAStar@@@Z=?setStepFinder@EXE_CvGlobals@@QAEXPAVFAStar@@@Z

		uninit
			?uninit@CvGlobals@@QAEXXZ=?uninit@EXE_CvGlobals@@QAEXXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvGlobals) == sizeof(CvGlobals));


class EXE_CvGraphicOptionInfo : public CvGraphicOptionInfo
{
public:
	/*
		getDefault
			?getDefault@CvGraphicOptionInfo@@QBE_NXZ=?getDefault@EXE_CvGraphicOptionInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvGraphicOptionInfo) == sizeof(CvGraphicOptionInfo));


class EXE_CvImprovementInfo : public CvImprovementInfo
{
public:
	/*
		getArtInfo
			?getArtInfo@CvImprovementInfo@@QBEPBVCvArtInfoImprovement@@XZ=?getArtInfo@EXE_CvImprovementInfo@@QBEPBVCvArtInfoImprovement@@XZ

		getWorldSoundscapeScriptId
			?getWorldSoundscapeScriptId@CvImprovementInfo@@QBEHXZ=?getWorldSoundscapeScriptId@EXE_CvImprovementInfo@@QBEHXZ

		isImprovementBonusMakesValid
			?isImprovementBonusMakesValid@CvImprovementInfo@@QBE_NH@Z=?isImprovementBonusMakesValid@EXE_CvImprovementInfo@@QBE_NH@Z

		isRequiresRiverSide
			?isRequiresRiverSide@CvImprovementInfo@@QBE_NXZ=?isRequiresRiverSide@EXE_CvImprovementInfo@@QBE_NXZ

		isWater
			?isWater@CvImprovementInfo@@QBE_NXZ=?isWater@EXE_CvImprovementInfo@@QBE_NXZ

		useLSystem
			?useLSystem@CvImprovementInfo@@QBE_NXZ=?useLSystem@EXE_CvImprovementInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvImprovementInfo) == sizeof(CvImprovementInfo));


class EXE_CvInfoBase : public CvInfoBase
{
public:
	/*
		getDescription
			?getDescription@CvInfoBase@@QBEPBGI@Z=?getDescription@EXE_CvInfoBase@@QBEPBGI@Z

		getHelp
			?getHelp@CvInfoBase@@QBEPBGXZ=?getHelp@EXE_CvInfoBase@@QBEPBGXZ

		getText
			?getText@CvInfoBase@@QBEPBGXZ=?getText@EXE_CvInfoBase@@QBEPBGXZ

		getTextKeyWide
			?getTextKeyWide@CvInfoBase@@QBEPBGXZ=?getTextKeyWide@EXE_CvInfoBase@@QBEPBGXZ

		getType
			?getType@CvInfoBase@@QBEPBDXZ=?getType@EXE_CvInfoBase@@QBEPBDXZ

		isGraphicalOnly
			?isGraphicalOnly@CvInfoBase@@QBE_NXZ=?isGraphicalOnly@EXE_CvInfoBase@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvInfoBase) == sizeof(CvInfoBase));


class EXE_CvInitCore : public CvInitCore
{
public:
	/*
		closeInactiveSlots
			?closeInactiveSlots@CvInitCore@@QAEXXZ=?closeInactiveSlots@EXE_CvInitCore@@QAEXXZ

		getArtStyle
			?getArtStyle@CvInitCore@@QBE?AW4ArtStyleTypes@@W4PlayerTypes@@@Z=?getArtStyle@EXE_CvInitCore@@QBE?AW4ArtStyleTypes@@W4PlayerTypes@@@Z

		getAvailableSlot
			?getAvailableSlot@CvInitCore@@QAE?AW4PlayerTypes@@XZ=?getAvailableSlot@EXE_CvInitCore@@QAE?AW4PlayerTypes@@XZ

		getCiv
			?getCiv@CvInitCore@@QBE?AW4CivilizationTypes@@W4PlayerTypes@@@Z=?getCiv@EXE_CvInitCore@@QBE?AW4CivilizationTypes@@W4PlayerTypes@@@Z

		getCivAdjective
			?getCivAdjective@CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@I@Z=?getCivAdjective@EXE_CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@I@Z

		getCivAdjectiveKey
			?getCivAdjectiveKey@CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@@Z=?getCivAdjectiveKey@EXE_CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@@Z

		getCivDescription
			?getCivDescription@CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@I@Z=?getCivDescription@EXE_CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@I@Z

		getCivDescriptionKey
			?getCivDescriptionKey@CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@@Z=?getCivDescriptionKey@EXE_CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@@Z

		getCivPassword
			?getCivPassword@CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@@Z=?getCivPassword@EXE_CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@@Z

		getCivShortDesc
			?getCivShortDesc@CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@I@Z=?getCivShortDesc@EXE_CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@I@Z

		getCivShortDescKey
			?getCivShortDescKey@CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@@Z=?getCivShortDescKey@EXE_CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@@Z

		getClimateKey
			?getClimateKey@CvInitCore@@QBEABVCvWString@@AAV2@@Z=?getClimateKey@EXE_CvInitCore@@QBEABVCvWString@@AAV2@@Z

		getColor
			?getColor@CvInitCore@@QBE?AW4PlayerColorTypes@@W4PlayerTypes@@@Z=?getColor@EXE_CvInitCore@@QBE?AW4PlayerColorTypes@@W4PlayerTypes@@@Z

		getCustomMapOption
			?getCustomMapOption@CvInitCore@@QBE?AW4CustomMapOptionTypes@@H@Z=?getCustomMapOption@EXE_CvInitCore@@QBE?AW4CustomMapOptionTypes@@H@Z

		getEmail
			?getEmail@CvInitCore@@QBEABVCvString@@W4PlayerTypes@@@Z=?getEmail@EXE_CvInitCore@@QBEABVCvString@@W4PlayerTypes@@@Z

		getEraKey
			?getEraKey@CvInitCore@@QBEABVCvWString@@AAV2@@Z=?getEraKey@EXE_CvInitCore@@QBEABVCvWString@@AAV2@@Z

		getForceControl
			?getForceControl@CvInitCore@@QBE_NW4ForceControlTypes@@@Z=?getForceControl@EXE_CvInitCore@@QBE_NW4ForceControlTypes@@@Z

		getGameMultiplayer
			?getGameMultiplayer@CvInitCore@@QBE_NXZ=?getGameMultiplayer@EXE_CvInitCore@@QBE_NXZ

		getGameSpeedKey
			?getGameSpeedKey@CvInitCore@@QBEABVCvWString@@AAV2@@Z=?getGameSpeedKey@EXE_CvInitCore@@QBEABVCvWString@@AAV2@@Z

		getHandicap
			?getHandicap@CvInitCore@@QBE?AW4HandicapTypes@@W4PlayerTypes@@@Z=?getHandicap@EXE_CvInitCore@@QBE?AW4HandicapTypes@@W4PlayerTypes@@@Z

		getHotseat
			?getHotseat@CvInitCore@@QBE_NXZ=?getHotseat@EXE_CvInitCore@@QBE_NXZ

		getHuman
			?getHuman@CvInitCore@@QBE_NW4PlayerTypes@@@Z=?getHuman@EXE_CvInitCore@@QBE_NW4PlayerTypes@@@Z

		getLeader
			?getLeader@CvInitCore@@QBE?AW4LeaderHeadTypes@@W4PlayerTypes@@@Z=?getLeader@EXE_CvInitCore@@QBE?AW4LeaderHeadTypes@@W4PlayerTypes@@@Z

		getLeaderName
			?getLeaderName@CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@I@Z=?getLeaderName@EXE_CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@I@Z

		getLeaderNameKey
			?getLeaderNameKey@CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@@Z=?getLeaderNameKey@EXE_CvInitCore@@QBEABVCvWString@@W4PlayerTypes@@@Z

		getMPOption
			?getMPOption@CvInitCore@@QBE_NW4MultiplayerOptionTypes@@@Z=?getMPOption@EXE_CvInitCore@@QBE_NW4MultiplayerOptionTypes@@@Z

		getMapRandSeed
			?getMapRandSeed@CvInitCore@@QBEIXZ=?getMapRandSeed@EXE_CvInitCore@@QBEIXZ

		getMapScriptName
			?getMapScriptName@CvInitCore@@QBE?AVCvWString@@XZ=?getMapScriptName@EXE_CvInitCore@@QBE?AVCvWString@@XZ

		getMaxEuropePlayers
			?getMaxEuropePlayers@CvInitCore@@QBEHXZ=?getMaxEuropePlayers@EXE_CvInitCore@@QBEHXZ

		getMinorNationCiv
			?getMinorNationCiv@CvInitCore@@QBE_NW4PlayerTypes@@@Z=?getMinorNationCiv@EXE_CvInitCore@@QBE_NW4PlayerTypes@@@Z

		getMultiplayer
			?getMultiplayer@CvInitCore@@QBE_NXZ=?getMultiplayer@EXE_CvInitCore@@QBE_NXZ

		getNetID
			?getNetID@CvInitCore@@QBEHW4PlayerTypes@@@Z=?getNetID@EXE_CvInitCore@@QBEHW4PlayerTypes@@@Z

		getNewGame
			?getNewGame@CvInitCore@@QBE_NXZ=?getNewGame@EXE_CvInitCore@@QBE_NXZ

		getNumDefinedPlayers
			?getNumDefinedPlayers@CvInitCore@@QBEHXZ=?getNumDefinedPlayers@EXE_CvInitCore@@QBEHXZ

		getOption
			?getOption@CvInitCore@@QBE_NW4GameOptionTypes@@@Z=?getOption@EXE_CvInitCore@@QBE_NW4GameOptionTypes@@@Z

		getPbem
			?getPbem@CvInitCore@@QBE_NXZ=?getPbem@EXE_CvInitCore@@QBE_NXZ

		getPitboss
			?getPitboss@CvInitCore@@QBE_NXZ=?getPitboss@EXE_CvInitCore@@QBE_NXZ

		getPitbossTurnTime
			?getPitbossTurnTime@CvInitCore@@QBEHXZ=?getPitbossTurnTime@EXE_CvInitCore@@QBEHXZ

		getPlayableCiv
			?getPlayableCiv@CvInitCore@@QBE_NW4PlayerTypes@@@Z=?getPlayableCiv@EXE_CvInitCore@@QBE_NW4PlayerTypes@@@Z

		getPythonCheck
			?getPythonCheck@CvInitCore@@QBEABVCvString@@W4PlayerTypes@@@Z=?getPythonCheck@EXE_CvInitCore@@QBEABVCvString@@W4PlayerTypes@@@Z

		getReady
			?getReady@CvInitCore@@QBE_NW4PlayerTypes@@@Z=?getReady@EXE_CvInitCore@@QBE_NW4PlayerTypes@@@Z

		getSavedGame
			?getSavedGame@CvInitCore@@QBE_NXZ=?getSavedGame@EXE_CvInitCore@@QBE_NXZ

		getScenario
			?getScenario@CvInitCore@@QBE_NXZ=?getScenario@EXE_CvInitCore@@QBE_NXZ

		getSeaLevelKey
			?getSeaLevelKey@CvInitCore@@QBEABVCvWString@@AAV2@@Z=?getSeaLevelKey@EXE_CvInitCore@@QBEABVCvWString@@AAV2@@Z

		getSlotClaim
			?getSlotClaim@CvInitCore@@QBE?AW4SlotClaim@@W4PlayerTypes@@@Z=?getSlotClaim@EXE_CvInitCore@@QBE?AW4SlotClaim@@W4PlayerTypes@@@Z

		getSlotStatus
			?getSlotStatus@CvInitCore@@QBE?AW4SlotStatus@@W4PlayerTypes@@@Z=?getSlotStatus@EXE_CvInitCore@@QBE?AW4SlotStatus@@W4PlayerTypes@@@Z

		getSlotVacant
			?getSlotVacant@CvInitCore@@QBE_NW4PlayerTypes@@@Z=?getSlotVacant@EXE_CvInitCore@@QBE_NW4PlayerTypes@@@Z

		getSmtpHost
			?getSmtpHost@CvInitCore@@QBEABVCvString@@W4PlayerTypes@@@Z=?getSmtpHost@EXE_CvInitCore@@QBEABVCvString@@W4PlayerTypes@@@Z

		getSyncRandSeed
			?getSyncRandSeed@CvInitCore@@QBEIXZ=?getSyncRandSeed@EXE_CvInitCore@@QBEIXZ

		getTeam
			?getTeam@CvInitCore@@QBE?AW4TeamTypes@@W4PlayerTypes@@@Z=?getTeam@EXE_CvInitCore@@QBE?AW4TeamTypes@@W4PlayerTypes@@@Z

		getType
			?getType@CvInitCore@@QBE?AW4GameType@@XZ=?getType@EXE_CvInitCore@@QBE?AW4GameType@@XZ

		getVictory
			?getVictory@CvInitCore@@QBE_NW4VictoryTypes@@@Z=?getVictory@EXE_CvInitCore@@QBE_NW4VictoryTypes@@@Z

		getWBMapScript
			?getWBMapScript@CvInitCore@@QBE_NXZ=?getWBMapScript@EXE_CvInitCore@@QBE_NXZ

		getWorldSizeKey
			?getWorldSizeKey@CvInitCore@@QBEABVCvWString@@AAV2@@Z=?getWorldSizeKey@EXE_CvInitCore@@QBEABVCvWString@@AAV2@@Z

		getXMLCheck
			?getXMLCheck@CvInitCore@@QBEABVCvString@@W4PlayerTypes@@@Z=?getXMLCheck@EXE_CvInitCore@@QBEABVCvString@@W4PlayerTypes@@@Z

		init
			?init@CvInitCore@@QAEXW4GameMode@@@Z=?init@EXE_CvInitCore@@QAEXW4GameMode@@@Z

		reassignPlayer
			?reassignPlayer@CvInitCore@@QAEXW4PlayerTypes@@0@Z=?reassignPlayer@EXE_CvInitCore@@QAEXW4PlayerTypes@@0@Z

		reopenInactiveSlots
			?reopenInactiveSlots@CvInitCore@@QAEXXZ=?reopenInactiveSlots@EXE_CvInitCore@@QAEXXZ

		resetAdvancedStartPoints
			?resetAdvancedStartPoints@CvInitCore@@QAEXXZ=?resetAdvancedStartPoints@EXE_CvInitCore@@QAEXXZ

		resetGame
			?resetGame@CvInitCore@@QAEXPAV1@_N1@Z=?resetGame@EXE_CvInitCore@@QAEXPAV1@_N1@Z

		resetPlayer
			?resetPlayer@CvInitCore@@QAEXW4PlayerTypes@@PAV1@_N2@Z=?resetPlayer@EXE_CvInitCore@@QAEXW4PlayerTypes@@PAV1@_N2@Z

		resetPlayers
			?resetPlayers@CvInitCore@@QAEXPAV1@_N1@Z=?resetPlayers@EXE_CvInitCore@@QAEXPAV1@_N1@Z

		resetPlayers
			?resetPlayers@CvInitCore@@QAEXXZ=?resetPlayers@EXE_CvInitCore@@QAEXXZ

		setActivePlayer
			?setActivePlayer@CvInitCore@@QAEXW4PlayerTypes@@@Z=?setActivePlayer@EXE_CvInitCore@@QAEXW4PlayerTypes@@@Z

		setAdminPassword
			?setAdminPassword@CvInitCore@@QAEXABVCvWString@@_N@Z=?setAdminPassword@EXE_CvInitCore@@QAEXABVCvWString@@_N@Z

		setArtStyle
			?setArtStyle@CvInitCore@@QAEXW4PlayerTypes@@W4ArtStyleTypes@@@Z=?setArtStyle@EXE_CvInitCore@@QAEXW4PlayerTypes@@W4ArtStyleTypes@@@Z

		setCiv
			?setCiv@CvInitCore@@QAEXW4PlayerTypes@@W4CivilizationTypes@@@Z=?setCiv@EXE_CvInitCore@@QAEXW4PlayerTypes@@W4CivilizationTypes@@@Z

		setCivAdjective
			?setCivAdjective@CvInitCore@@QAEXW4PlayerTypes@@ABVCvWString@@@Z=?setCivAdjective@EXE_CvInitCore@@QAEXW4PlayerTypes@@ABVCvWString@@@Z

		setCivDescription
			?setCivDescription@CvInitCore@@QAEXW4PlayerTypes@@ABVCvWString@@@Z=?setCivDescription@EXE_CvInitCore@@QAEXW4PlayerTypes@@ABVCvWString@@@Z

		setCivPassword
			?setCivPassword@CvInitCore@@QAEXW4PlayerTypes@@ABVCvWString@@_N@Z=?setCivPassword@EXE_CvInitCore@@QAEXW4PlayerTypes@@ABVCvWString@@_N@Z

		setCivShortDesc
			?setCivShortDesc@CvInitCore@@QAEXW4PlayerTypes@@ABVCvWString@@@Z=?setCivShortDesc@EXE_CvInitCore@@QAEXW4PlayerTypes@@ABVCvWString@@@Z

		setClimate
			?setClimate@CvInitCore@@QAEXABVCvWString@@@Z=?setClimate@EXE_CvInitCore@@QAEXABVCvWString@@@Z

		setColor
			?setColor@CvInitCore@@QAEXW4PlayerTypes@@W4PlayerColorTypes@@@Z=?setColor@EXE_CvInitCore@@QAEXW4PlayerTypes@@W4PlayerColorTypes@@@Z

		setCustomMapOption
			?setCustomMapOption@CvInitCore@@QAEXHW4CustomMapOptionTypes@@@Z=?setCustomMapOption@EXE_CvInitCore@@QAEXHW4CustomMapOptionTypes@@@Z

		setCustomMapOptions
			?setCustomMapOptions@CvInitCore@@QAEXHPBW4CustomMapOptionTypes@@@Z=?setCustomMapOptions@EXE_CvInitCore@@QAEXHPBW4CustomMapOptionTypes@@@Z

		setEmail
			?setEmail@CvInitCore@@QAEXW4PlayerTypes@@ABVCvString@@@Z=?setEmail@EXE_CvInitCore@@QAEXW4PlayerTypes@@ABVCvString@@@Z

		setEra
			?setEra@CvInitCore@@QAEXABVCvWString@@@Z=?setEra@EXE_CvInitCore@@QAEXABVCvWString@@@Z

		setForceControl
			?setForceControl@CvInitCore@@QAEXW4ForceControlTypes@@_N@Z=?setForceControl@EXE_CvInitCore@@QAEXW4ForceControlTypes@@_N@Z

		setGameName
			?setGameName@CvInitCore@@QAEXABVCvWString@@@Z=?setGameName@EXE_CvInitCore@@QAEXABVCvWString@@@Z

		setGamePassword
			?setGamePassword@CvInitCore@@QAEXABVCvWString@@@Z=?setGamePassword@EXE_CvInitCore@@QAEXABVCvWString@@@Z

		setGameSpeed
			?setGameSpeed@CvInitCore@@QAEXABVCvWString@@@Z=?setGameSpeed@EXE_CvInitCore@@QAEXABVCvWString@@@Z

		setHandicap
			?setHandicap@CvInitCore@@QAEXW4PlayerTypes@@W4HandicapTypes@@@Z=?setHandicap@EXE_CvInitCore@@QAEXW4PlayerTypes@@W4HandicapTypes@@@Z

		setLeader
			?setLeader@CvInitCore@@QAEXW4PlayerTypes@@W4LeaderHeadTypes@@@Z=?setLeader@EXE_CvInitCore@@QAEXW4PlayerTypes@@W4LeaderHeadTypes@@@Z

		setLeaderName
			?setLeaderName@CvInitCore@@QAEXW4PlayerTypes@@ABVCvWString@@@Z=?setLeaderName@EXE_CvInitCore@@QAEXW4PlayerTypes@@ABVCvWString@@@Z

		setMPOption
			?setMPOption@CvInitCore@@QAEXW4MultiplayerOptionTypes@@_N@Z=?setMPOption@EXE_CvInitCore@@QAEXW4MultiplayerOptionTypes@@_N@Z

		setMapRandSeed
			?setMapRandSeed@CvInitCore@@QAEXI@Z=?setMapRandSeed@EXE_CvInitCore@@QAEXI@Z

		setMapScriptName
			?setMapScriptName@CvInitCore@@QAEXABVCvWString@@@Z=?setMapScriptName@EXE_CvInitCore@@QAEXABVCvWString@@@Z

		setMinorNationCiv
			?setMinorNationCiv@CvInitCore@@QAEXW4PlayerTypes@@_N@Z=?setMinorNationCiv@EXE_CvInitCore@@QAEXW4PlayerTypes@@_N@Z

		setMode
			?setMode@CvInitCore@@QAEXW4GameMode@@@Z=?setMode@EXE_CvInitCore@@QAEXW4GameMode@@@Z

		setNetID
			?setNetID@CvInitCore@@QAEXW4PlayerTypes@@H@Z=?setNetID@EXE_CvInitCore@@QAEXW4PlayerTypes@@H@Z

		setOption
			?setOption@CvInitCore@@QAEXW4GameOptionTypes@@_N@Z=?setOption@EXE_CvInitCore@@QAEXW4GameOptionTypes@@_N@Z

		setPitbossTurnTime
			?setPitbossTurnTime@CvInitCore@@QAEXH@Z=?setPitbossTurnTime@EXE_CvInitCore@@QAEXH@Z

		setPlayableCiv
			?setPlayableCiv@CvInitCore@@QAEXW4PlayerTypes@@_N@Z=?setPlayableCiv@EXE_CvInitCore@@QAEXW4PlayerTypes@@_N@Z

		setPythonCheck
			?setPythonCheck@CvInitCore@@QAEXW4PlayerTypes@@ABVCvString@@@Z=?setPythonCheck@EXE_CvInitCore@@QAEXW4PlayerTypes@@ABVCvString@@@Z

		setReady
			?setReady@CvInitCore@@QAEXW4PlayerTypes@@_N@Z=?setReady@EXE_CvInitCore@@QAEXW4PlayerTypes@@_N@Z

		setSeaLevel
			?setSeaLevel@CvInitCore@@QAEXABVCvWString@@@Z=?setSeaLevel@EXE_CvInitCore@@QAEXABVCvWString@@@Z

		setSlotClaim
			?setSlotClaim@CvInitCore@@QAEXW4PlayerTypes@@W4SlotClaim@@@Z=?setSlotClaim@EXE_CvInitCore@@QAEXW4PlayerTypes@@W4SlotClaim@@@Z

		setSlotStatus
			?setSlotStatus@CvInitCore@@QAEXW4PlayerTypes@@W4SlotStatus@@@Z=?setSlotStatus@EXE_CvInitCore@@QAEXW4PlayerTypes@@W4SlotStatus@@@Z

		setSmtpHost
			?setSmtpHost@CvInitCore@@QAEXW4PlayerTypes@@ABVCvString@@@Z=?setSmtpHost@EXE_CvInitCore@@QAEXW4PlayerTypes@@ABVCvString@@@Z

		setSyncRandSeed
			?setSyncRandSeed@CvInitCore@@QAEXI@Z=?setSyncRandSeed@EXE_CvInitCore@@QAEXI@Z

		setTeam
			?setTeam@CvInitCore@@QAEXW4PlayerTypes@@W4TeamTypes@@@Z=?setTeam@EXE_CvInitCore@@QAEXW4PlayerTypes@@W4TeamTypes@@@Z

		setType
			?setType@CvInitCore@@QAEXABVCvWString@@@Z=?setType@EXE_CvInitCore@@QAEXABVCvWString@@@Z

		setType
			?setType@CvInitCore@@QAEXW4GameType@@@Z=?setType@EXE_CvInitCore@@QAEXW4GameType@@@Z

		setVictories
			?setVictories@CvInitCore@@QAEXHPB_N@Z=?setVictories@EXE_CvInitCore@@QAEXHPB_N@Z

		setVictory
			?setVictory@CvInitCore@@QAEXW4VictoryTypes@@_N@Z=?setVictory@EXE_CvInitCore@@QAEXW4VictoryTypes@@_N@Z

		setWorldSize
			?setWorldSize@CvInitCore@@QAEXABVCvWString@@@Z=?setWorldSize@EXE_CvInitCore@@QAEXABVCvWString@@@Z

		setXMLCheck
			?setXMLCheck@CvInitCore@@QAEXW4PlayerTypes@@ABVCvString@@@Z=?setXMLCheck@EXE_CvInitCore@@QAEXW4PlayerTypes@@ABVCvString@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvInitCore) == sizeof(CvInitCore));


class EXE_CvInterfaceModeInfo : public CvInterfaceModeInfo
{
public:
	/*
		getCursorIndex
			?getCursorIndex@CvInterfaceModeInfo@@QBEHXZ=?getCursorIndex@EXE_CvInterfaceModeInfo@@QBEHXZ

		getGotoPlot
			?getGotoPlot@CvInterfaceModeInfo@@QBE_NXZ=?getGotoPlot@EXE_CvInterfaceModeInfo@@QBE_NXZ

		getHighlightPlot
			?getHighlightPlot@CvInterfaceModeInfo@@QBE_NXZ=?getHighlightPlot@EXE_CvInterfaceModeInfo@@QBE_NXZ

		getMissionType
			?getMissionType@CvInterfaceModeInfo@@QBEHXZ=?getMissionType@EXE_CvInterfaceModeInfo@@QBEHXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvInterfaceModeInfo) == sizeof(CvInterfaceModeInfo));


class EXE_CvLandscapeInfo : public CvLandscapeInfo
{
public:
	/*
		getCellVertsHigh
			?getCellVertsHigh@CvLandscapeInfo@@QBEHXZ=?getCellVertsHigh@EXE_CvLandscapeInfo@@QBEHXZ

		getCellVertsWide
			?getCellVertsWide@CvLandscapeInfo@@QBEHXZ=?getCellVertsWide@EXE_CvLandscapeInfo@@QBEHXZ

		getEnvironmentTexture
			?getEnvironmentTexture@CvLandscapeInfo@@QAEPBDXZ=?getEnvironmentTexture@EXE_CvLandscapeInfo@@QAEPBDXZ

		getHillScale
			?getHillScale@CvLandscapeInfo@@QBEMXZ=?getHillScale@EXE_CvLandscapeInfo@@QBEMXZ

		getPeakScale
			?getPeakScale@CvLandscapeInfo@@QBEMXZ=?getPeakScale@EXE_CvLandscapeInfo@@QBEMXZ

		getPlotVertsHigh
			?getPlotVertsHigh@CvLandscapeInfo@@QBEHXZ=?getPlotVertsHigh@EXE_CvLandscapeInfo@@QBEHXZ

		getPlotVertsWide
			?getPlotVertsWide@CvLandscapeInfo@@QBEHXZ=?getPlotVertsWide@EXE_CvLandscapeInfo@@QBEHXZ

		getPlotsPerCellX
			?getPlotsPerCellX@CvLandscapeInfo@@QBEHXZ=?getPlotsPerCellX@EXE_CvLandscapeInfo@@QBEHXZ

		getPlotsPerCellY
			?getPlotsPerCellY@CvLandscapeInfo@@QBEHXZ=?getPlotsPerCellY@EXE_CvLandscapeInfo@@QBEHXZ

		getTextureScaleX
			?getTextureScaleX@CvLandscapeInfo@@QBEMXZ=?getTextureScaleX@EXE_CvLandscapeInfo@@QBEMXZ

		getTextureScaleY
			?getTextureScaleY@CvLandscapeInfo@@QBEMXZ=?getTextureScaleY@EXE_CvLandscapeInfo@@QBEMXZ

		getWaterHeight
			?getWaterHeight@CvLandscapeInfo@@QBEHXZ=?getWaterHeight@EXE_CvLandscapeInfo@@QBEHXZ

		getZScale
			?getZScale@CvLandscapeInfo@@QBEMXZ=?getZScale@EXE_CvLandscapeInfo@@QBEMXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvLandscapeInfo) == sizeof(CvLandscapeInfo));


class EXE_CvLeaderHeadInfo : public CvLeaderHeadInfo
{
public:
	/*
		getArtInfo
			?getArtInfo@CvLeaderHeadInfo@@QBEPBVCvArtInfoLeaderhead@@XZ=?getArtInfo@EXE_CvLeaderHeadInfo@@QBEPBVCvArtInfoLeaderhead@@XZ

		getDiploPeaceMusicScriptIds
			?getDiploPeaceMusicScriptIds@CvLeaderHeadInfo@@QBEHH@Z=?getDiploPeaceMusicScriptIds@EXE_CvLeaderHeadInfo@@QBEHH@Z

		getDiploWarMusicScriptIds
			?getDiploWarMusicScriptIds@CvLeaderHeadInfo@@QBEHH@Z=?getDiploWarMusicScriptIds@EXE_CvLeaderHeadInfo@@QBEHH@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvLeaderHeadInfo) == sizeof(CvLeaderHeadInfo));


class EXE_CvMainMenuInfo : public CvMainMenuInfo
{
public:
	/*
		getLoading
			?getLoading@CvMainMenuInfo@@QBE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ=?getLoading@EXE_CvMainMenuInfo@@QBE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ

		getLoadingSlideshow
			?getLoadingSlideshow@CvMainMenuInfo@@QBE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ=?getLoadingSlideshow@EXE_CvMainMenuInfo@@QBE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ

		getScene
			?getScene@CvMainMenuInfo@@QBE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ=?getScene@EXE_CvMainMenuInfo@@QBE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ

		getSoundtrack
			?getSoundtrack@CvMainMenuInfo@@QBE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ=?getSoundtrack@EXE_CvMainMenuInfo@@QBE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvMainMenuInfo) == sizeof(CvMainMenuInfo));


class EXE_CvMap : public CvMap
{
public:
	/*
		erasePlots
			?erasePlots@CvMap@@QAEXXZ=?erasePlots@EXE_CvMap@@QAEXXZ

		findCity
			?findCity@CvMap@@QAEPAVCvCity@@HHW4PlayerTypes@@W4TeamTypes@@_N21W4DirectionTypes@@PAV2@@Z=?findCity@EXE_CvMap@@QAEPAVCvCity@@HHW4PlayerTypes@@W4TeamTypes@@_N21W4DirectionTypes@@PAV2@@Z

		getGridHeight
			?getGridHeight@CvMap@@QBEHXZ=?getGridHeight@EXE_CvMap@@QBEHXZ

		getGridWidth
			?getGridWidth@CvMap@@QBEHXZ=?getGridWidth@EXE_CvMap@@QBEHXZ

		getWorldSize
			?getWorldSize@CvMap@@QAE?AW4WorldSizeTypes@@XZ=?getWorldSize@EXE_CvMap@@QAE?AW4WorldSizeTypes@@XZ

		init
			?init@CvMap@@QAEXPAUCvMapInitData@@@Z=?init@EXE_CvMap@@QAEXPAUCvMapInitData@@@Z

		isPlot
			?isPlot@CvMap@@QBE_NHH@Z=?isPlot@EXE_CvMap@@QBE_NHH@Z

		isWrap
			?isWrap@CvMap@@QAE_NXZ=?isWrap@EXE_CvMap@@QAE_NXZ

		isWrapX
			?isWrapX@CvMap@@QAE_NXZ=?isWrapX@EXE_CvMap@@QAE_NXZ

		isWrapY
			?isWrapY@CvMap@@QAE_NXZ=?isWrapY@EXE_CvMap@@QAE_NXZ

		numPlots
			?numPlots@CvMap@@QBEHXZ=?numPlots@EXE_CvMap@@QBEHXZ

		plot
			?plot@CvMap@@QBEPAVCvPlot@@HH@Z=?plot@EXE_CvMap@@QBEPAVCvPlot@@HH@Z

		plotByIndex
			?plotByIndex@CvMap@@QBEPAVCvPlot@@H@Z=?plotByIndex@EXE_CvMap@@QBEPAVCvPlot@@H@Z

		plotNum
			?plotNum@CvMap@@QBEHHH@Z=?plotNum@EXE_CvMap@@QBEHHH@Z

		plotXToPointX
			?plotXToPointX@CvMap@@QAEMH@Z=?plotXToPointX@EXE_CvMap@@QAEMH@Z

		plotYToPointY
			?plotYToPointY@CvMap@@QAEMH@Z=?plotYToPointY@EXE_CvMap@@QAEMH@Z

		pointToPlot
			?pointToPlot@CvMap@@QAEPAVCvPlot@@MM@Z=?pointToPlot@EXE_CvMap@@QAEPAVCvPlot@@MM@Z

		reset
			?reset@CvMap@@QAEXPAUCvMapInitData@@@Z=?reset@EXE_CvMap@@QAEXPAUCvMapInitData@@@Z

		setupGraphical
			?setupGraphical@CvMap@@QAEXXZ=?setupGraphical@EXE_CvMap@@QAEXXZ

		updateCenterUnit
			?updateCenterUnit@CvMap@@QAEXXZ=?updateCenterUnit@EXE_CvMap@@QAEXXZ

		updateFlagSymbols
			?updateFlagSymbols@CvMap@@QAEXXZ=?updateFlagSymbols@EXE_CvMap@@QAEXXZ

		updateFog
			?updateFog@CvMap@@QAEXXZ=?updateFog@EXE_CvMap@@QAEXXZ

		updateMinimapColor
			?updateMinimapColor@CvMap@@QAEXXZ=?updateMinimapColor@EXE_CvMap@@QAEXXZ

		updateSymbolVisibility
			?updateSymbolVisibility@CvMap@@QAEXXZ=?updateSymbolVisibility@EXE_CvMap@@QAEXXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvMap) == sizeof(CvMap));


class EXE_CvMapGenerator : public CvMapGenerator
{
public:
	/*
		FreeInstance
			?FreeInstance@CvMapGenerator@@SAXXZ=?FreeInstance@EXE_CvMapGenerator@@SAXXZ

		GetInstance
			?GetInstance@CvMapGenerator@@SAAAV1@XZ=?GetInstance@EXE_CvMapGenerator@@SAAAV1@XZ

		addBonuses
			?addBonuses@CvMapGenerator@@QAEXXZ=?addBonuses@EXE_CvMapGenerator@@QAEXXZ

		addFeatures
			?addFeatures@CvMapGenerator@@QAEXXZ=?addFeatures@EXE_CvMapGenerator@@QAEXXZ

		addGameElements
			?addGameElements@CvMapGenerator@@QAEXXZ=?addGameElements@EXE_CvMapGenerator@@QAEXXZ

		addGoodies
			?addGoodies@CvMapGenerator@@QAEXXZ=?addGoodies@EXE_CvMapGenerator@@QAEXXZ

		addRivers
			?addRivers@CvMapGenerator@@QAEXXZ=?addRivers@EXE_CvMapGenerator@@QAEXXZ

		eraseBonuses
			?eraseBonuses@CvMapGenerator@@QAEXXZ=?eraseBonuses@EXE_CvMapGenerator@@QAEXXZ

		eraseEurope
			?eraseEurope@CvMapGenerator@@QAEXXZ=?eraseEurope@EXE_CvMapGenerator@@QAEXXZ

		eraseFeatures
			?eraseFeatures@CvMapGenerator@@QAEXXZ=?eraseFeatures@EXE_CvMapGenerator@@QAEXXZ

		eraseGoodies
			?eraseGoodies@CvMapGenerator@@QAEXXZ=?eraseGoodies@EXE_CvMapGenerator@@QAEXXZ

		eraseRivers
			?eraseRivers@CvMapGenerator@@QAEXXZ=?eraseRivers@EXE_CvMapGenerator@@QAEXXZ

		generateRandomMap
			?generateRandomMap@CvMapGenerator@@QAEXXZ=?generateRandomMap@EXE_CvMapGenerator@@QAEXXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvMapGenerator) == sizeof(CvMapGenerator));


class EXE_CvMissionDefinition : public CvMissionDefinition
{
public:
	/*
		getMissionTime
			?getMissionTime@CvMissionDefinition@@QBEMXZ=?getMissionTime@EXE_CvMissionDefinition@@QBEMXZ

		getMissionType
			?getMissionType@CvMissionDefinition@@QBE?AW4MissionTypes@@XZ=?getMissionType@EXE_CvMissionDefinition@@QBE?AW4MissionTypes@@XZ

		getPlot
			?getPlot@CvMissionDefinition@@QBEPBVCvPlot@@XZ=?getPlot@EXE_CvMissionDefinition@@QBEPBVCvPlot@@XZ

		getUnit
			?getUnit@CvMissionDefinition@@QBEPAVCvUnit@@W4BattleUnitTypes@@@Z=?getUnit@EXE_CvMissionDefinition@@QBEPAVCvUnit@@W4BattleUnitTypes@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvMissionDefinition) == sizeof(CvMissionDefinition));


class EXE_CvMissionInfo : public CvMissionInfo
{
public:
	/*
		getEntityEvent
			?getEntityEvent@CvMissionInfo@@QBE?AW4EntityEventTypes@@XZ=?getEntityEvent@EXE_CvMissionInfo@@QBE?AW4EntityEventTypes@@XZ

		getTime
			?getTime@CvMissionInfo@@QBEHXZ=?getTime@EXE_CvMissionInfo@@QBEHXZ

		isTarget
			?isTarget@CvMissionInfo@@QBE_NXZ=?isTarget@EXE_CvMissionInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvMissionInfo) == sizeof(CvMissionInfo));


class EXE_CvPlayer : public CvPlayer
{
public:
	/*
		addDiplomacy
			?addDiplomacy@CvPlayer@@QAEXPAVCvDiploParameters@@@Z=?addDiplomacy@EXE_CvPlayer@@QAEXPAVCvDiploParameters@@@Z

		addMessage
			?addMessage@CvPlayer@@QAEXABVCvTalkingHeadMessage@@@Z=?addMessage@EXE_CvPlayer@@QAEXABVCvTalkingHeadMessage@@@Z

		addPopup
			?addPopup@CvPlayer@@QAEXPAVCvPopupInfo@@_N@Z=?addPopup@EXE_CvPlayer@@QAEXPAVCvPopupInfo@@_N@Z

		buildTradeTable
			?buildTradeTable@CvPlayer@@QBEXW4PlayerTypes@@AAV?$CLinkList@UTradeData@@@@ABUIDInfo@@@Z=?buildTradeTable@EXE_CvPlayer@@QBEXW4PlayerTypes@@AAV?$CLinkList@UTradeData@@@@ABUIDInfo@@@Z

		canContact
			?canContact@CvPlayer@@QBE_NW4PlayerTypes@@@Z=?canContact@EXE_CvPlayer@@QBE_NW4PlayerTypes@@@Z

		canTradeItem
			?canTradeItem@CvPlayer@@QBE_NW4PlayerTypes@@UTradeData@@_N@Z=?canTradeItem@EXE_CvPlayer@@QBE_NW4PlayerTypes@@UTradeData@@_N@Z

		changeGold
			?changeGold@CvPlayer@@QAEXH@Z=?changeGold@EXE_CvPlayer@@QAEXH@Z

		changeUnitMoveChange
			?changeUnitMoveChange@CvPlayer@@QAEXW4UnitClassTypes@@H@Z=?changeUnitMoveChange@EXE_CvPlayer@@QAEXW4UnitClassTypes@@H@Z

		doAction
			?doAction@CvPlayer@@QAEXW4PlayerActionTypes@@HHH@Z=?doAction@EXE_CvPlayer@@QAEXW4PlayerActionTypes@@HHH@Z

		doAdvancedStartAction
			?doAdvancedStartAction@CvPlayer@@QAEXW4AdvancedStartActionTypes@@HHH_N@Z=?doAdvancedStartAction@EXE_CvPlayer@@QAEXW4AdvancedStartActionTypes@@HHH_N@Z

		firstCity
			?firstCity@CvPlayer@@QBEPAVCvCity@@PAH_N@Z=?firstCity@EXE_CvPlayer@@QBEPAVCvCity@@PAH_N@Z

		firstUnit
			?firstUnit@CvPlayer@@QBEPAVCvUnit@@PAH@Z=?firstUnit@EXE_CvPlayer@@QBEPAVCvUnit@@PAH@Z

		getAdvancedStartBuildingCost
			?getAdvancedStartBuildingCost@CvPlayer@@QAEHW4BuildingTypes@@_NPAVCvCity@@@Z=?getAdvancedStartBuildingCost@EXE_CvPlayer@@QAEHW4BuildingTypes@@_NPAVCvCity@@@Z

		getAdvancedStartCityCost
			?getAdvancedStartCityCost@CvPlayer@@QAEH_NPAVCvPlot@@@Z=?getAdvancedStartCityCost@EXE_CvPlayer@@QAEH_NPAVCvPlot@@@Z

		getAdvancedStartCultureCost
			?getAdvancedStartCultureCost@CvPlayer@@QAEH_NPAVCvCity@@@Z=?getAdvancedStartCultureCost@EXE_CvPlayer@@QAEH_NPAVCvCity@@@Z

		getAdvancedStartImprovementCost
			?getAdvancedStartImprovementCost@CvPlayer@@QAEHW4ImprovementTypes@@_NPAVCvPlot@@@Z=?getAdvancedStartImprovementCost@EXE_CvPlayer@@QAEHW4ImprovementTypes@@_NPAVCvPlot@@@Z

		getAdvancedStartPoints
			?getAdvancedStartPoints@CvPlayer@@QBEHXZ=?getAdvancedStartPoints@EXE_CvPlayer@@QBEHXZ

		getAdvancedStartPopCost
			?getAdvancedStartPopCost@CvPlayer@@QAEH_NPAVCvCity@@@Z=?getAdvancedStartPopCost@EXE_CvPlayer@@QAEH_NPAVCvCity@@@Z

		getAdvancedStartRouteCost
			?getAdvancedStartRouteCost@CvPlayer@@QAEHW4RouteTypes@@_NPAVCvPlot@@@Z=?getAdvancedStartRouteCost@EXE_CvPlayer@@QAEHW4RouteTypes@@_NPAVCvPlot@@@Z

		getAdvancedStartUnitCost
			?getAdvancedStartUnitCost@CvPlayer@@QAEHW4UnitTypes@@_NPAVCvPlot@@@Z=?getAdvancedStartUnitCost@EXE_CvPlayer@@QAEHW4UnitTypes@@_NPAVCvPlot@@@Z

		getAdvancedStartVisibilityCost
			?getAdvancedStartVisibilityCost@CvPlayer@@QAEH_NPAVCvPlot@@@Z=?getAdvancedStartVisibilityCost@EXE_CvPlayer@@QAEH_NPAVCvPlot@@@Z

		getArtStyleType
			?getArtStyleType@CvPlayer@@QBE?AW4ArtStyleTypes@@XZ=?getArtStyleType@EXE_CvPlayer@@QBE?AW4ArtStyleTypes@@XZ

		getCity
			?getCity@CvPlayer@@QBEPAVCvCity@@H@Z=?getCity@EXE_CvPlayer@@QBEPAVCvCity@@H@Z

		getCivilizationAdjective
			?getCivilizationAdjective@CvPlayer@@QBEPBGI@Z=?getCivilizationAdjective@EXE_CvPlayer@@QBEPBGI@Z

		getCivilizationDescription
			?getCivilizationDescription@CvPlayer@@QBEPBGI@Z=?getCivilizationDescription@EXE_CvPlayer@@QBEPBGI@Z

		getCivilizationShortDescription
			?getCivilizationShortDescription@CvPlayer@@QBEPBGI@Z=?getCivilizationShortDescription@EXE_CvPlayer@@QBEPBGI@Z

		getCivilizationType
			?getCivilizationType@CvPlayer@@QBE?AW4CivilizationTypes@@XZ=?getCivilizationType@EXE_CvPlayer@@QBE?AW4CivilizationTypes@@XZ

		getCurrentEra
			?getCurrentEra@CvPlayer@@QBE?AW4EraTypes@@XZ=?getCurrentEra@EXE_CvPlayer@@QBE?AW4EraTypes@@XZ

		getDiplomacy
			?getDiplomacy@CvPlayer@@QBEABV?$list@PAVCvDiploParameters@@V?$allocator@PAVCvDiploParameters@@@std@@@std@@XZ=?getDiplomacy@EXE_CvPlayer@@QBEABV?$list@PAVCvDiploParameters@@V?$allocator@PAVCvDiploParameters@@@std@@@std@@XZ

		getEuropeUnitById
			?getEuropeUnitById@CvPlayer@@QBEPAVCvUnit@@H@Z=?getEuropeUnitById@EXE_CvPlayer@@QBEPAVCvUnit@@H@Z

		getFlagDecal
			?getFlagDecal@CvPlayer@@QBEPBDXZ=?getFlagDecal@EXE_CvPlayer@@QBEPBDXZ

		getGameMessages
			?getGameMessages@CvPlayer@@QBEABV?$list@VCvTalkingHeadMessage@@V?$allocator@VCvTalkingHeadMessage@@@std@@@std@@XZ=?getGameMessages@EXE_CvPlayer@@QBEABV?$list@VCvTalkingHeadMessage@@V?$allocator@VCvTalkingHeadMessage@@@std@@@std@@XZ

		getGold
			?getGold@CvPlayer@@QBEHXZ=?getGold@EXE_CvPlayer@@QBEHXZ

		getHandicapType
			?getHandicapType@CvPlayer@@QBE?AW4HandicapTypes@@XZ=?getHandicapType@EXE_CvPlayer@@QBE?AW4HandicapTypes@@XZ

		getHeadingTradeString
			?getHeadingTradeString@CvPlayer@@QBE_NW4PlayerTypes@@W4TradeableItems@@AAVCvWString@@AAVCvString@@@Z=?getHeadingTradeString@EXE_CvPlayer@@QBE_NW4PlayerTypes@@W4TradeableItems@@AAVCvWString@@AAVCvString@@@Z

		getID
			?getID@CvPlayer@@QBE?AW4PlayerTypes@@XZ=?getID@EXE_CvPlayer@@QBE?AW4PlayerTypes@@XZ

		getItemTradeString
			?getItemTradeString@CvPlayer@@QBE_NW4PlayerTypes@@_N1ABUTradeData@@ABUIDInfo@@AAVCvWString@@AAVCvString@@@Z=?getItemTradeString@EXE_CvPlayer@@QBE_NW4PlayerTypes@@_N1ABUTradeData@@ABUIDInfo@@AAVCvWString@@AAVCvString@@@Z

		getLeaderType
			?getLeaderType@CvPlayer@@QBE?AW4LeaderHeadTypes@@XZ=?getLeaderType@EXE_CvPlayer@@QBE?AW4LeaderHeadTypes@@XZ

		getMaxGoldTrade
			?getMaxGoldTrade@CvPlayer@@QBEHW4PlayerTypes@@ABUIDInfo@@@Z=?getMaxGoldTrade@EXE_CvPlayer@@QBEHW4PlayerTypes@@ABUIDInfo@@@Z

		getName
			?getName@CvPlayer@@QBEPBGI@Z=?getName@EXE_CvPlayer@@QBEPBGI@Z

		getNameKey
			?getNameKey@CvPlayer@@QBEPBGXZ=?getNameKey@EXE_CvPlayer@@QBEPBGXZ

		getNetID
			?getNetID@CvPlayer@@QBEHXZ=?getNetID@EXE_CvPlayer@@QBEHXZ

		getNumCities
			?getNumCities@CvPlayer@@QBEHXZ=?getNumCities@EXE_CvPlayer@@QBEHXZ

		getNumUnits
			?getNumUnits@CvPlayer@@QBEHXZ=?getNumUnits@EXE_CvPlayer@@QBEHXZ

		getPbemEmailAddress
			?getPbemEmailAddress@CvPlayer@@QBEABVCvString@@XZ=?getPbemEmailAddress@EXE_CvPlayer@@QBEABVCvString@@XZ

		getPlayerColor
			?getPlayerColor@CvPlayer@@QBE?AW4PlayerColorTypes@@XZ=?getPlayerColor@EXE_CvPlayer@@QBE?AW4PlayerColorTypes@@XZ

		getPlayerTextColorB
			?getPlayerTextColorB@CvPlayer@@QBEHXZ=?getPlayerTextColorB@EXE_CvPlayer@@QBEHXZ

		getPlayerTextColorG
			?getPlayerTextColorG@CvPlayer@@QBEHXZ=?getPlayerTextColorG@EXE_CvPlayer@@QBEHXZ

		getPlayerTextColorR
			?getPlayerTextColorR@CvPlayer@@QBEHXZ=?getPlayerTextColorR@EXE_CvPlayer@@QBEHXZ

		getPopups
			?getPopups@CvPlayer@@QBEABV?$list@PAVCvPopupInfo@@V?$allocator@PAVCvPopupInfo@@@std@@@std@@XZ=?getPopups@EXE_CvPlayer@@QBEABV?$list@PAVCvPopupInfo@@V?$allocator@PAVCvPopupInfo@@@std@@@std@@XZ

		getPrimaryCity
			?getPrimaryCity@CvPlayer@@QBEPAVCvCity@@XZ=?getPrimaryCity@EXE_CvPlayer@@QBEPAVCvCity@@XZ

		getSmtpHost
			?getSmtpHost@CvPlayer@@QBEABVCvString@@XZ=?getSmtpHost@EXE_CvPlayer@@QBEABVCvString@@XZ

		getTeam
			?getTeam@CvPlayer@@QBE?AW4TeamTypes@@XZ=?getTeam@EXE_CvPlayer@@QBE?AW4TeamTypes@@XZ

		getTotalTimePlayed
			?getTotalTimePlayed@CvPlayer@@QBEIXZ=?getTotalTimePlayed@EXE_CvPlayer@@QBEIXZ

		getTradeDenial
			?getTradeDenial@CvPlayer@@QBE?AW4DenialTypes@@W4PlayerTypes@@UTradeData@@@Z=?getTradeDenial@EXE_CvPlayer@@QBE?AW4DenialTypes@@W4PlayerTypes@@UTradeData@@@Z

		getUnit
			?getUnit@CvPlayer@@QBEPAVCvUnit@@H@Z=?getUnit@EXE_CvPlayer@@QBEPAVCvUnit@@H@Z
*/

	DllExport const TCHAR* getUnitButton(UnitTypes eUnit) const
	{
		if (eUnit <= UNIT_PLACEHOLDER)
		{
			return CvPlayer::getUnitButton(eUnit);
		}
		return CvPlayer::getUnitButton(UNIT_PLACEHOLDER);
	}


/*
		getWorstEnemyName
			?getWorstEnemyName@CvPlayer@@QBE?BVCvWString@@XZ=?getWorstEnemyName@EXE_CvPlayer@@QBE?BVCvWString@@XZ

		handleDiploEvent
			?handleDiploEvent@CvPlayer@@QAEXW4DiploEventTypes@@W4PlayerTypes@@HH@Z=?handleDiploEvent@EXE_CvPlayer@@QAEXW4DiploEventTypes@@W4PlayerTypes@@HH@Z

		hasBusyUnit
			?hasBusyUnit@CvPlayer@@QBE_NXZ=?hasBusyUnit@EXE_CvPlayer@@QBE_NXZ

		init
			?init@CvPlayer@@QAEXW4PlayerTypes@@@Z=?init@EXE_CvPlayer@@QAEXW4PlayerTypes@@@Z

		initUnit
			?initUnit@CvPlayer@@QAEPAVCvUnit@@W4UnitTypes@@W4ProfessionTypes@@HHW4UnitAITypes@@W4DirectionTypes@@H@Z=?initUnit@EXE_CvPlayer@@QAEPAVCvUnit@@W4UnitTypes@@W4ProfessionTypes@@HHW4UnitAITypes@@W4DirectionTypes@@H@Z

		isAlive
			?isAlive@CvPlayer@@QBE_NXZ=?isAlive@EXE_CvPlayer@@QBE_NXZ

		isEverAlive
			?isEverAlive@CvPlayer@@QBE_NXZ=?isEverAlive@EXE_CvPlayer@@QBE_NXZ

		isHuman
			?isHuman@CvPlayer@@QBE_NXZ=?isHuman@EXE_CvPlayer@@QBE_NXZ

		isInvertFlag
			?isInvertFlag@CvPlayer@@QBE_NXZ=?isInvertFlag@EXE_CvPlayer@@QBE_NXZ

		isOption
			?isOption@CvPlayer@@QBE_NW4PlayerOptionTypes@@@Z=?isOption@EXE_CvPlayer@@QBE_NW4PlayerOptionTypes@@@Z

		isPbemNewTurn
			?isPbemNewTurn@CvPlayer@@QBE_NXZ=?isPbemNewTurn@EXE_CvPlayer@@QBE_NXZ

		isTurnActive
			?isTurnActive@CvPlayer@@QBE_NXZ=?isTurnActive@EXE_CvPlayer@@QBE_NXZ

		isTurnDone
			?isTurnDone@CvPlayer@@QBE_NXZ=?isTurnDone@EXE_CvPlayer@@QBE_NXZ

		isWhiteFlag
			?isWhiteFlag@CvPlayer@@QBE_NXZ=?isWhiteFlag@EXE_CvPlayer@@QBE_NXZ

		makeExtendedGame
			?makeExtendedGame@CvPlayer@@QAEXXZ=?makeExtendedGame@EXE_CvPlayer@@QAEXXZ

		nextCity
			?nextCity@CvPlayer@@QBEPAVCvCity@@PAH_N@Z=?nextCity@EXE_CvPlayer@@QBEPAVCvCity@@PAH_N@Z

		nextUnit
			?nextUnit@CvPlayer@@QBEPAVCvUnit@@PAH@Z=?nextUnit@EXE_CvPlayer@@QBEPAVCvUnit@@PAH@Z

		popFrontDiplomacy
			?popFrontDiplomacy@CvPlayer@@QAEPAVCvDiploParameters@@XZ=?popFrontDiplomacy@EXE_CvPlayer@@QAEPAVCvDiploParameters@@XZ

		popFrontPopup
			?popFrontPopup@CvPlayer@@QAEPAVCvPopupInfo@@XZ=?popFrontPopup@EXE_CvPlayer@@QAEPAVCvPopupInfo@@XZ

		reset
			?reset@CvPlayer@@QAEXW4PlayerTypes@@_N@Z=?reset@EXE_CvPlayer@@QAEXW4PlayerTypes@@_N@Z

		setCivic
			?setCivic@CvPlayer@@QAEXW4CivicOptionTypes@@W4CivicTypes@@@Z=?setCivic@EXE_CvPlayer@@QAEXW4CivicOptionTypes@@W4CivicTypes@@@Z

		setEndTurn
			?setEndTurn@CvPlayer@@QAEX_N@Z=?setEndTurn@EXE_CvPlayer@@QAEX_N@Z

		setGold
			?setGold@CvPlayer@@QAEXH@Z=?setGold@EXE_CvPlayer@@QAEXH@Z

		setNetID
			?setNetID@CvPlayer@@QAEXH@Z=?setNetID@EXE_CvPlayer@@QAEXH@Z

		setPbemEmailAddress
			?setPbemEmailAddress@CvPlayer@@QAEXPBD@Z=?setPbemEmailAddress@EXE_CvPlayer@@QAEXPBD@Z

		setPbemNewTurn
			?setPbemNewTurn@CvPlayer@@QAEX_N@Z=?setPbemNewTurn@EXE_CvPlayer@@QAEX_N@Z

		setStartTime
			?setStartTime@CvPlayer@@QAEXI@Z=?setStartTime@EXE_CvPlayer@@QAEXI@Z

		setTurnActive
			?setTurnActive@CvPlayer@@QAEX_N0@Z=?setTurnActive@EXE_CvPlayer@@QAEX_N0@Z

		setTurnActiveForPbem
			?setTurnActiveForPbem@CvPlayer@@QAEX_N@Z=?setTurnActiveForPbem@EXE_CvPlayer@@QAEX_N@Z

		setupGraphical
			?setupGraphical@CvPlayer@@QAEXXZ=?setupGraphical@EXE_CvPlayer@@QAEXXZ

		updateHuman
			?updateHuman@CvPlayer@@QAEXXZ=?updateHuman@EXE_CvPlayer@@QAEXXZ

		updateTradeList
			?updateTradeList@CvPlayer@@QBEXW4PlayerTypes@@AAV?$CLinkList@UTradeData@@@@ABV3@2ABUIDInfo@@@Z=?updateTradeList@EXE_CvPlayer@@QBEXW4PlayerTypes@@AAV?$CLinkList@UTradeData@@@@ABV3@2ABUIDInfo@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvPlayer) == sizeof(CvPlayer));


class EXE_CvPlayerAI : public CvPlayerAI
{
public:
	/*
		AI_getGreeting
			?AI_getGreeting@CvPlayerAI@@QAE?AW4DiploCommentTypes@@W4PlayerTypes@@@Z=?AI_getGreeting@EXE_CvPlayerAI@@QAE?AW4DiploCommentTypes@@W4PlayerTypes@@@Z

		areStaticsInitialized
			?areStaticsInitialized@CvPlayerAI@@SA_NXZ=?areStaticsInitialized@EXE_CvPlayerAI@@SA_NXZ

		getPlayerNonInl
			?getPlayerNonInl@CvPlayerAI@@SAAAV1@W4PlayerTypes@@@Z=?getPlayerNonInl@EXE_CvPlayerAI@@SAAAV1@W4PlayerTypes@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvPlayerAI) == sizeof(CvPlayerAI));


class EXE_CvPlayerColorInfo : public CvPlayerColorInfo
{
public:
	/*
		getColorTypePrimary
			?getColorTypePrimary@CvPlayerColorInfo@@QBEHXZ=?getColorTypePrimary@EXE_CvPlayerColorInfo@@QBEHXZ

		getColorTypeSecondary
			?getColorTypeSecondary@CvPlayerColorInfo@@QBEHXZ=?getColorTypeSecondary@EXE_CvPlayerColorInfo@@QBEHXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvPlayerColorInfo) == sizeof(CvPlayerColorInfo));


class EXE_CvPlayerOptionInfo : public CvPlayerOptionInfo
{
public:
	/*
		getDefault
			?getDefault@CvPlayerOptionInfo@@QBE_NXZ=?getDefault@EXE_CvPlayerOptionInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvPlayerOptionInfo) == sizeof(CvPlayerOptionInfo));


class EXE_CvPlot : public CvPlot
{
public:
	/*
		countTotalCulture
			?countTotalCulture@CvPlot@@QBEHXZ=?countTotalCulture@EXE_CvPlot@@QBEHXZ

		getBonusType
			?getBonusType@CvPlot@@QBE?AW4BonusTypes@@XZ=?getBonusType@EXE_CvPlot@@QBE?AW4BonusTypes@@XZ

		getCenterUnit
			?getCenterUnit@CvPlot@@QBEPAVCvUnit@@XZ=?getCenterUnit@EXE_CvPlot@@QBEPAVCvUnit@@XZ

		getCulture
			?getCulture@CvPlot@@QBEHW4PlayerTypes@@@Z=?getCulture@EXE_CvPlot@@QBEHW4PlayerTypes@@@Z

		getDebugCenterUnit
			?getDebugCenterUnit@CvPlot@@QBEPAVCvUnit@@XZ=?getDebugCenterUnit@EXE_CvPlot@@QBEPAVCvUnit@@XZ

		getFeatureType
			?getFeatureType@CvPlot@@QBE?AW4FeatureTypes@@XZ=?getFeatureType@EXE_CvPlot@@QBE?AW4FeatureTypes@@XZ

		getFeatureVariety
			?getFeatureVariety@CvPlot@@QBEHXZ=?getFeatureVariety@EXE_CvPlot@@QBEHXZ

		getFlagSymbol
			?getFlagSymbol@CvPlot@@QBEPAVCvFlagEntity@@XZ=?getFlagSymbol@EXE_CvPlot@@QBEPAVCvFlagEntity@@XZ

		getImprovementType
			?getImprovementType@CvPlot@@QBE?AW4ImprovementTypes@@XZ=?getImprovementType@EXE_CvPlot@@QBE?AW4ImprovementTypes@@XZ

		getOwner
			?getOwner@CvPlot@@QBE?AW4PlayerTypes@@XZ=?getOwner@EXE_CvPlot@@QBE?AW4PlayerTypes@@XZ

		getPlotCity
			?getPlotCity@CvPlot@@QBEPAVCvCity@@XZ=?getPlotCity@EXE_CvPlot@@QBEPAVCvCity@@XZ

		getPoint
			?getPoint@CvPlot@@QBE?AVNiPoint3@@XZ=?getPoint@EXE_CvPlot@@QBE?AVNiPoint3@@XZ

		getPointX
			?getPointX@CvPlot@@QBEMXZ=?getPointX@EXE_CvPlot@@QBEMXZ

		getPointY
			?getPointY@CvPlot@@QBEMXZ=?getPointY@EXE_CvPlot@@QBEMXZ

		getResourceLayerIcon
			?getResourceLayerIcon@CvPlot@@QBEPBDW4ResourceLayerOptions@@AAVCvWStringBuffer@@AAW4PlotIndicatorVisibilityFlags@@AAW4ColorTypes@@@Z=?getResourceLayerIcon@EXE_CvPlot@@QBEPBDW4ResourceLayerOptions@@AAVCvWStringBuffer@@AAW4PlotIndicatorVisibilityFlags@@AAW4ColorTypes@@@Z

		getRevealedOwner
			?getRevealedOwner@CvPlot@@QBE?AW4PlayerTypes@@W4TeamTypes@@_N@Z=?getRevealedOwner@EXE_CvPlot@@QBE?AW4PlayerTypes@@W4TeamTypes@@_N@Z

		getRiverNSDirection
			?getRiverNSDirection@CvPlot@@QBE?AW4CardinalDirectionTypes@@XZ=?getRiverNSDirection@EXE_CvPlot@@QBE?AW4CardinalDirectionTypes@@XZ

		getRiverSymbol
			?getRiverSymbol@CvPlot@@QBEPAVCvRiver@@XZ=?getRiverSymbol@EXE_CvPlot@@QBEPAVCvRiver@@XZ

		getRiverWEDirection
			?getRiverWEDirection@CvPlot@@QBE?AW4CardinalDirectionTypes@@XZ=?getRiverWEDirection@EXE_CvPlot@@QBE?AW4CardinalDirectionTypes@@XZ

		getRouteSymbol
			?getRouteSymbol@CvPlot@@QBEPAVCvRoute@@XZ=?getRouteSymbol@EXE_CvPlot@@QBEPAVCvRoute@@XZ

		getTeam
			?getTeam@CvPlot@@QBE?AW4TeamTypes@@XZ=?getTeam@EXE_CvPlot@@QBE?AW4TeamTypes@@XZ

		getTerrainType
			?getTerrainType@CvPlot@@QBE?AW4TerrainTypes@@XZ=?getTerrainType@EXE_CvPlot@@QBE?AW4TerrainTypes@@XZ

		getUnitLayerUnit
			?getUnitLayerUnit@CvPlot@@QBEPAVCvUnit@@W4UnitLayerOptionTypes@@AAVCvWStringBuffer@@AAW4PlotIndicatorVisibilityFlags@@AAW4ColorTypes@@AA_N@Z=?getUnitLayerUnit@EXE_CvPlot@@QBEPAVCvUnit@@W4UnitLayerOptionTypes@@AAVCvWStringBuffer@@AAW4PlotIndicatorVisibilityFlags@@AAW4ColorTypes@@AA_N@Z

		getVisibleBonusState
			?getVisibleBonusState@CvPlot@@QAEXAAW4BonusTypes@@AA_N1@Z=?getVisibleBonusState@EXE_CvPlot@@QAEXAAW4BonusTypes@@AA_N1@Z

		getVisibleImprovementState
			?getVisibleImprovementState@CvPlot@@QAEXAAW4ImprovementTypes@@AA_N@Z=?getVisibleImprovementState@EXE_CvPlot@@QAEXAAW4ImprovementTypes@@AA_N@Z

		getWorkingCity
			?getWorkingCity@CvPlot@@QBEPAVCvCity@@XZ=?getWorkingCity@EXE_CvPlot@@QBEPAVCvCity@@XZ

		getX
			?getX@CvPlot@@QBEHXZ=?getX@EXE_CvPlot@@QBEHXZ

		getY
			?getY@CvPlot@@QBEHXZ=?getY@EXE_CvPlot@@QBEHXZ

		getYield
			?getYield@CvPlot@@QBEHW4YieldTypes@@@Z=?getYield@EXE_CvPlot@@QBEHW4YieldTypes@@@Z

		headUnitNode
			?headUnitNode@CvPlot@@QBEPAV?$CLLNode@UIDInfo@@@@XZ=?headUnitNode@EXE_CvPlot@@QBEPAV?$CLLNode@UIDInfo@@@@XZ

		isActiveVisible
			?isActiveVisible@CvPlot@@QBE_N_N@Z=?isActiveVisible@EXE_CvPlot@@QBE_N_N@Z

		isCity
			?isCity@CvPlot@@QBE_N_NW4TeamTypes@@@Z=?isCity@EXE_CvPlot@@QBE_N_NW4TeamTypes@@@Z

		isFighting
			?isFighting@CvPlot@@QBE_NXZ=?isFighting@EXE_CvPlot@@QBE_NXZ

		isGoody
			?isGoody@CvPlot@@QBE_NW4TeamTypes@@@Z=?isGoody@EXE_CvPlot@@QBE_NW4TeamTypes@@@Z

		isHills
			?isHills@CvPlot@@QBE_NXZ=?isHills@EXE_CvPlot@@QBE_NXZ

		isLake
			?isLake@CvPlot@@QBE_NXZ=?isLake@EXE_CvPlot@@QBE_NXZ

		isLayoutDirty
			?isLayoutDirty@CvPlot@@QBE_NXZ=?isLayoutDirty@EXE_CvPlot@@QBE_NXZ

		isLayoutStateDifferent
			?isLayoutStateDifferent@CvPlot@@QBE_NXZ=?isLayoutStateDifferent@EXE_CvPlot@@QBE_NXZ

		isNOfRiver
			?isNOfRiver@CvPlot@@QBE_NXZ=?isNOfRiver@EXE_CvPlot@@QBE_NXZ

		isPeak
			?isPeak@CvPlot@@QBE_NXZ=?isPeak@EXE_CvPlot@@QBE_NXZ

		isRevealed
			?isRevealed@CvPlot@@QBE_NW4TeamTypes@@_N@Z=?isRevealed@EXE_CvPlot@@QBE_NW4TeamTypes@@_N@Z

		isRiverCrossing
			?isRiverCrossing@CvPlot@@QBE_NW4DirectionTypes@@@Z=?isRiverCrossing@EXE_CvPlot@@QBE_NW4DirectionTypes@@@Z

		isRiverCrossingFlowClockwise
			?isRiverCrossingFlowClockwise@CvPlot@@QBE_NW4DirectionTypes@@@Z=?isRiverCrossingFlowClockwise@EXE_CvPlot@@QBE_NW4DirectionTypes@@@Z

		isRoute
			?isRoute@CvPlot@@QBE_NXZ=?isRoute@EXE_CvPlot@@QBE_NXZ

		isVisibleEnemyUnit
			?isVisibleEnemyUnit@CvPlot@@QBE_NW4PlayerTypes@@@Z=?isVisibleEnemyUnit@EXE_CvPlot@@QBE_NW4PlayerTypes@@@Z

		isWOfRiver
			?isWOfRiver@CvPlot@@QBE_NXZ=?isWOfRiver@EXE_CvPlot@@QBE_NXZ

		isWater
			?isWater@CvPlot@@QBE_NXZ=?isWater@EXE_CvPlot@@QBE_NXZ

		nextUnitNode
			?nextUnitNode@CvPlot@@QBEPAV?$CLLNode@UIDInfo@@@@PAV2@@Z=?nextUnitNode@EXE_CvPlot@@QBEPAV?$CLLNode@UIDInfo@@@@PAV2@@Z

		plotCount
			?plotCount@CvPlot@@QBEHP6A_NPBVCvUnit@@HH@ZHHW4PlayerTypes@@W4TeamTypes@@1HH@Z=?plotCount@EXE_CvPlot@@QBEHP6A_NPBVCvUnit@@HH@ZHHW4PlayerTypes@@W4TeamTypes@@1HH@Z

		setLayoutDirty
			?setLayoutDirty@CvPlot@@QAEX_N@Z=?setLayoutDirty@EXE_CvPlot@@QAEX_N@Z

		setLayoutStateToCurrent
			?setLayoutStateToCurrent@CvPlot@@QAEXXZ=?setLayoutStateToCurrent@EXE_CvPlot@@QAEXXZ

		updateFlagSymbol
			?updateFlagSymbol@CvPlot@@QAEXXZ=?updateFlagSymbol@EXE_CvPlot@@QAEXXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvPlot) == sizeof(CvPlot));


class EXE_CvPopupInfo : public CvPopupInfo
{
public:
	/*
		addPythonButton
			?addPythonButton@CvPopupInfo@@QAEXPBGPBD@Z=?addPythonButton@EXE_CvPopupInfo@@QAEXPBGPBD@Z

		getButtonPopupType
			?getButtonPopupType@CvPopupInfo@@QBE?AW4ButtonPopupTypes@@XZ=?getButtonPopupType@EXE_CvPopupInfo@@QBE?AW4ButtonPopupTypes@@XZ

		getData1
			?getData1@CvPopupInfo@@QBEHXZ=?getData1@EXE_CvPopupInfo@@QBEHXZ

		getData2
			?getData2@CvPopupInfo@@QBEHXZ=?getData2@EXE_CvPopupInfo@@QBEHXZ

		getData3
			?getData3@CvPopupInfo@@QBEHXZ=?getData3@EXE_CvPopupInfo@@QBEHXZ

		getFlags
			?getFlags@CvPopupInfo@@QBEHXZ=?getFlags@EXE_CvPopupInfo@@QBEHXZ

		getNumPythonButtons
			?getNumPythonButtons@CvPopupInfo@@QBEHXZ=?getNumPythonButtons@EXE_CvPopupInfo@@QBEHXZ

		getOnClickedPythonCallback
			?getOnClickedPythonCallback@CvPopupInfo@@QBEABVCvString@@XZ=?getOnClickedPythonCallback@EXE_CvPopupInfo@@QBEABVCvString@@XZ

		getOnFocusPythonCallback
			?getOnFocusPythonCallback@CvPopupInfo@@QBEABVCvString@@XZ=?getOnFocusPythonCallback@EXE_CvPopupInfo@@QBEABVCvString@@XZ

		getOption1
			?getOption1@CvPopupInfo@@QBE_NXZ=?getOption1@EXE_CvPopupInfo@@QBE_NXZ

		getOption2
			?getOption2@CvPopupInfo@@QBE_NXZ=?getOption2@EXE_CvPopupInfo@@QBE_NXZ

		getPendingDelete
			?getPendingDelete@CvPopupInfo@@QBE_NXZ=?getPendingDelete@EXE_CvPopupInfo@@QBE_NXZ

		getPythonButtonArt
			?getPythonButtonArt@CvPopupInfo@@QBEABVCvString@@H@Z=?getPythonButtonArt@EXE_CvPopupInfo@@QBEABVCvString@@H@Z

		getPythonButtonText
			?getPythonButtonText@CvPopupInfo@@QBEABVCvWString@@H@Z=?getPythonButtonText@EXE_CvPopupInfo@@QBEABVCvWString@@H@Z

		getPythonModule
			?getPythonModule@CvPopupInfo@@QBEABVCvString@@XZ=?getPythonModule@EXE_CvPopupInfo@@QBEABVCvString@@XZ

		getText
			?getText@CvPopupInfo@@QBEABVCvWString@@XZ=?getText@EXE_CvPopupInfo@@QBEABVCvWString@@XZ

		read
			?read@CvPopupInfo@@QAEXAAVFDataStreamBase@@@Z=?read@EXE_CvPopupInfo@@QAEXAAVFDataStreamBase@@@Z

		setButtonPopupType
			?setButtonPopupType@CvPopupInfo@@QAEXW4ButtonPopupTypes@@@Z=?setButtonPopupType@EXE_CvPopupInfo@@QAEXW4ButtonPopupTypes@@@Z

		setData1
			?setData1@CvPopupInfo@@QAEXH@Z=?setData1@EXE_CvPopupInfo@@QAEXH@Z

		setData2
			?setData2@CvPopupInfo@@QAEXH@Z=?setData2@EXE_CvPopupInfo@@QAEXH@Z

		setData3
			?setData3@CvPopupInfo@@QAEXH@Z=?setData3@EXE_CvPopupInfo@@QAEXH@Z

		setFlags
			?setFlags@CvPopupInfo@@QAEXH@Z=?setFlags@EXE_CvPopupInfo@@QAEXH@Z

		setOnClickedPythonCallback
			?setOnClickedPythonCallback@CvPopupInfo@@QAEXPBD@Z=?setOnClickedPythonCallback@EXE_CvPopupInfo@@QAEXPBD@Z

		setOnFocusPythonCallback
			?setOnFocusPythonCallback@CvPopupInfo@@QAEXPBD@Z=?setOnFocusPythonCallback@EXE_CvPopupInfo@@QAEXPBD@Z

		setOption1
			?setOption1@CvPopupInfo@@QAEX_N@Z=?setOption1@EXE_CvPopupInfo@@QAEX_N@Z

		setOption2
			?setOption2@CvPopupInfo@@QAEX_N@Z=?setOption2@EXE_CvPopupInfo@@QAEX_N@Z

		setPendingDelete
			?setPendingDelete@CvPopupInfo@@QAEX_N@Z=?setPendingDelete@EXE_CvPopupInfo@@QAEX_N@Z

		setPythonModule
			?setPythonModule@CvPopupInfo@@QAEXPBD@Z=?setPythonModule@EXE_CvPopupInfo@@QAEXPBD@Z

		setText
			?setText@CvPopupInfo@@QAEXPBG@Z=?setText@EXE_CvPopupInfo@@QAEXPBG@Z

		write
			?write@CvPopupInfo@@QBEXAAVFDataStreamBase@@@Z=?write@EXE_CvPopupInfo@@QBEXAAVFDataStreamBase@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvPopupInfo) == sizeof(CvPopupInfo));


class EXE_CvRandom : public CvRandom
{
public:
	/*
		get
			?get@CvRandom@@QAEGGPBD@Z=?get@EXE_CvRandom@@QAEGGPBD@Z

		getFloat
			?getFloat@CvRandom@@QAEMXZ=?getFloat@EXE_CvRandom@@QAEMXZ

		getGaussian
			?getGaussian@CvRandom@@QAEMMM@Z=?getGaussian@EXE_CvRandom@@QAEMMM@Z

		init
			?init@CvRandom@@QAEXK@Z=?init@EXE_CvRandom@@QAEXK@Z

		shuffleSequence
			?shuffleSequence@CvRandom@@QAEXAAV?$vector@HV?$allocator@H@std@@@std@@PBD@Z=?shuffleSequence@EXE_CvRandom@@QAEXAAV?$vector@HV?$allocator@H@std@@@std@@PBD@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvRandom) == sizeof(CvRandom));


class EXE_CvReplayInfo : public CvReplayInfo
{
public:
	/*
		getMapHeight
			?getMapHeight@CvReplayInfo@@QBEHXZ=?getMapHeight@EXE_CvReplayInfo@@QBEHXZ

		getMapWidth
			?getMapWidth@CvReplayInfo@@QBEHXZ=?getMapWidth@EXE_CvReplayInfo@@QBEHXZ

		getMinimapPixels
			?getMinimapPixels@CvReplayInfo@@QBEPBEXZ=?getMinimapPixels@EXE_CvReplayInfo@@QBEPBEXZ

		getModName
			?getModName@CvReplayInfo@@QBEPBDXZ=?getModName@EXE_CvReplayInfo@@QBEPBDXZ

		read
			?read@CvReplayInfo@@QAE_NAAVFDataStreamBase@@@Z=?read@EXE_CvReplayInfo@@QAE_NAAVFDataStreamBase@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvReplayInfo) == sizeof(CvReplayInfo));


class EXE_CvRiverModelInfo : public CvRiverModelInfo
{
public:
	/*
		getBorderFile
			?getBorderFile@CvRiverModelInfo@@QBEPBDXZ=?getBorderFile@EXE_CvRiverModelInfo@@QBEPBDXZ

		getConnectString
			?getConnectString@CvRiverModelInfo@@QBEPBDXZ=?getConnectString@EXE_CvRiverModelInfo@@QBEPBDXZ

		getDeltaString
			?getDeltaString@CvRiverModelInfo@@QBEPBDXZ=?getDeltaString@EXE_CvRiverModelInfo@@QBEPBDXZ

		getModelFile
			?getModelFile@CvRiverModelInfo@@QBEPBDXZ=?getModelFile@EXE_CvRiverModelInfo@@QBEPBDXZ

		getRotateString
			?getRotateString@CvRiverModelInfo@@QBEPBDXZ=?getRotateString@EXE_CvRiverModelInfo@@QBEPBDXZ

		getTextureIndex
			?getTextureIndex@CvRiverModelInfo@@QBEHXZ=?getTextureIndex@EXE_CvRiverModelInfo@@QBEHXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvRiverModelInfo) == sizeof(CvRiverModelInfo));


class EXE_CvRouteModelInfo : public CvRouteModelInfo
{
public:
	/*
		getConnectString
			?getConnectString@CvRouteModelInfo@@QBEPBDXZ=?getConnectString@EXE_CvRouteModelInfo@@QBEPBDXZ

		getModelConnectString
			?getModelConnectString@CvRouteModelInfo@@QBEPBDXZ=?getModelConnectString@EXE_CvRouteModelInfo@@QBEPBDXZ

		getModelFile
			?getModelFile@CvRouteModelInfo@@QBEPBDXZ=?getModelFile@EXE_CvRouteModelInfo@@QBEPBDXZ

		getRotateString
			?getRotateString@CvRouteModelInfo@@QBEPBDXZ=?getRotateString@EXE_CvRouteModelInfo@@QBEPBDXZ

		getRouteType
			?getRouteType@CvRouteModelInfo@@QBE?AW4RouteTypes@@XZ=?getRouteType@EXE_CvRouteModelInfo@@QBE?AW4RouteTypes@@XZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvRouteModelInfo) == sizeof(CvRouteModelInfo));


class EXE_CvScalableInfo : public CvScalableInfo
{
public:
	/*
		getInterfaceScale
			?getInterfaceScale@CvScalableInfo@@QBEMXZ=?getInterfaceScale@EXE_CvScalableInfo@@QBEMXZ

		getScale
			?getScale@CvScalableInfo@@QBEMXZ=?getScale@EXE_CvScalableInfo@@QBEMXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvScalableInfo) == sizeof(CvScalableInfo));


class EXE_CvSelectionGroup : public CvSelectionGroup
{
public:
	/*
		NotifyEntity
			?NotifyEntity@CvSelectionGroup@@QAEXW4MissionTypes@@@Z=?NotifyEntity@EXE_CvSelectionGroup@@QAEXW4MissionTypes@@@Z

		addUnit
			?addUnit@CvSelectionGroup@@QAE_NPAVCvUnit@@_N@Z=?addUnit@EXE_CvSelectionGroup@@QAE_NPAVCvUnit@@_N@Z

		autoMission
			?autoMission@CvSelectionGroup@@QAEXXZ=?autoMission@EXE_CvSelectionGroup@@QAEXXZ

		canAllMove
			?canAllMove@CvSelectionGroup@@QAE_NXZ=?canAllMove@EXE_CvSelectionGroup@@QAE_NXZ

		canDoInterfaceMode
			?canDoInterfaceMode@CvSelectionGroup@@QAE_NW4InterfaceModeTypes@@@Z=?canDoInterfaceMode@EXE_CvSelectionGroup@@QAE_NW4InterfaceModeTypes@@@Z

		canDoInterfaceModeAt
			?canDoInterfaceModeAt@CvSelectionGroup@@QAE_NW4InterfaceModeTypes@@PAVCvPlot@@@Z=?canDoInterfaceModeAt@EXE_CvSelectionGroup@@QAE_NW4InterfaceModeTypes@@PAVCvPlot@@@Z

		canMoveInto
			?canMoveInto@CvSelectionGroup@@QAE_NPAVCvPlot@@_N@Z=?canMoveInto@EXE_CvSelectionGroup@@QAE_NPAVCvPlot@@_N@Z

		canMoveOrAttackInto
			?canMoveOrAttackInto@CvSelectionGroup@@QAE_NPAVCvPlot@@_N@Z=?canMoveOrAttackInto@EXE_CvSelectionGroup@@QAE_NPAVCvPlot@@_N@Z

		clearUnits
			?clearUnits@CvSelectionGroup@@QAEXXZ=?clearUnits@EXE_CvSelectionGroup@@QAEXXZ

		deleteUnitNode
			?deleteUnitNode@CvSelectionGroup@@QAEPAV?$CLLNode@UIDInfo@@@@PAV2@@Z=?deleteUnitNode@EXE_CvSelectionGroup@@QAEPAV?$CLLNode@UIDInfo@@@@PAV2@@Z

		getHeadUnit
			?getHeadUnit@CvSelectionGroup@@QBEPAVCvUnit@@XZ=?getHeadUnit@EXE_CvSelectionGroup@@QBEPAVCvUnit@@XZ

		getNumUnits
			?getNumUnits@CvSelectionGroup@@QBEHXZ=?getNumUnits@EXE_CvSelectionGroup@@QBEHXZ

		getOwner
			?getOwner@CvSelectionGroup@@QBE?AW4PlayerTypes@@XZ=?getOwner@EXE_CvSelectionGroup@@QBE?AW4PlayerTypes@@XZ

		getUnitIndex
			?getUnitIndex@CvSelectionGroup@@QBEHPAVCvUnit@@H@Z=?getUnitIndex@EXE_CvSelectionGroup@@QBEHPAVCvUnit@@H@Z

		headMissionQueueNode
			?headMissionQueueNode@CvSelectionGroup@@QBEPAV?$CLLNode@UMissionData@@@@XZ=?headMissionQueueNode@EXE_CvSelectionGroup@@QBEPAV?$CLLNode@UMissionData@@@@XZ

		headUnitNode
			?headUnitNode@CvSelectionGroup@@QBEPAV?$CLLNode@UIDInfo@@@@XZ=?headUnitNode@EXE_CvSelectionGroup@@QBEPAV?$CLLNode@UIDInfo@@@@XZ

		init
			?init@CvSelectionGroup@@QAEXHW4PlayerTypes@@@Z=?init@EXE_CvSelectionGroup@@QAEXHW4PlayerTypes@@@Z

		isBusy
			?isBusy@CvSelectionGroup@@QAE_NXZ=?isBusy@EXE_CvSelectionGroup@@QAE_NXZ

		lastMissionPlot
			?lastMissionPlot@CvSelectionGroup@@QAEPAVCvPlot@@XZ=?lastMissionPlot@EXE_CvSelectionGroup@@QAEPAVCvPlot@@XZ

		nextMissionQueueNode
			?nextMissionQueueNode@CvSelectionGroup@@QBEPAV?$CLLNode@UMissionData@@@@PAV2@@Z=?nextMissionQueueNode@EXE_CvSelectionGroup@@QBEPAV?$CLLNode@UMissionData@@@@PAV2@@Z

		nextUnitNode
			?nextUnitNode@CvSelectionGroup@@QBEPAV?$CLLNode@UIDInfo@@@@PAV2@@Z=?nextUnitNode@EXE_CvSelectionGroup@@QBEPAV?$CLLNode@UIDInfo@@@@PAV2@@Z

		plot
			?plot@CvSelectionGroup@@QBEPAVCvPlot@@XZ=?plot@EXE_CvSelectionGroup@@QBEPAVCvPlot@@XZ

		pushMission
			?pushMission@CvSelectionGroup@@QAEXW4MissionTypes@@HHH_N1W4MissionAITypes@@PAVCvPlot@@PAVCvUnit@@@Z=?pushMission@EXE_CvSelectionGroup@@QAEXW4MissionTypes@@HHH_N1W4MissionAITypes@@PAVCvPlot@@PAVCvUnit@@@Z

		readyToSelect
			?readyToSelect@CvSelectionGroup@@QAE_N_N@Z=?readyToSelect@EXE_CvSelectionGroup@@QAE_N_N@Z

		reset
			?reset@CvSelectionGroup@@QAEXHW4PlayerTypes@@_N@Z=?reset@EXE_CvSelectionGroup@@QAEXHW4PlayerTypes@@_N@Z

		uninit
			?uninit@CvSelectionGroup@@QAEXXZ=?uninit@EXE_CvSelectionGroup@@QAEXXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvSelectionGroup) == sizeof(CvSelectionGroup));


class EXE_CvSlideShowInfo : public CvSlideShowInfo
{
public:
	/*
		getPath
			?getPath@CvSlideShowInfo@@QAEPBDXZ=?getPath@EXE_CvSlideShowInfo@@QAEPBDXZ

		getStartTime
			?getStartTime@CvSlideShowInfo@@QAEMXZ=?getStartTime@EXE_CvSlideShowInfo@@QAEMXZ

		getTransitionType
			?getTransitionType@CvSlideShowInfo@@QAEPBDXZ=?getTransitionType@EXE_CvSlideShowInfo@@QAEPBDXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvSlideShowInfo) == sizeof(CvSlideShowInfo));


class EXE_CvSlideShowRandomInfo : public CvSlideShowRandomInfo
{
public:
	/*
		getPath
			?getPath@CvSlideShowRandomInfo@@QAEPBDXZ=?getPath@EXE_CvSlideShowRandomInfo@@QAEPBDXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvSlideShowRandomInfo) == sizeof(CvSlideShowRandomInfo));


class EXE_CvSpecialBuildingInfo : public CvSpecialBuildingInfo
{
public:
	/*
		getFontButtonIndex
			?getFontButtonIndex@CvSpecialBuildingInfo@@QBEHXZ=?getFontButtonIndex@EXE_CvSpecialBuildingInfo@@QBEHXZ

		setChar
			?setChar@CvSpecialBuildingInfo@@QAEXH@Z=?setChar@EXE_CvSpecialBuildingInfo@@QAEXH@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvSpecialBuildingInfo) == sizeof(CvSpecialBuildingInfo));


class EXE_CvTalkingHeadMessage : public CvTalkingHeadMessage
{
public:
	/*
		getDescription
			?getDescription@CvTalkingHeadMessage@@QBEPBGXZ=?getDescription@EXE_CvTalkingHeadMessage@@QBEPBGXZ

		getFlashColor
			?getFlashColor@CvTalkingHeadMessage@@QBE?AW4ColorTypes@@XZ=?getFlashColor@EXE_CvTalkingHeadMessage@@QBE?AW4ColorTypes@@XZ

		getFromPlayer
			?getFromPlayer@CvTalkingHeadMessage@@QBE?AW4PlayerTypes@@XZ=?getFromPlayer@EXE_CvTalkingHeadMessage@@QBE?AW4PlayerTypes@@XZ

		getIcon
			?getIcon@CvTalkingHeadMessage@@QBEABVCvString@@XZ=?getIcon@EXE_CvTalkingHeadMessage@@QBEABVCvString@@XZ

		getLength
			?getLength@CvTalkingHeadMessage@@QBEHXZ=?getLength@EXE_CvTalkingHeadMessage@@QBEHXZ

		getMessageType
			?getMessageType@CvTalkingHeadMessage@@QBE?AW4InterfaceMessageTypes@@XZ=?getMessageType@EXE_CvTalkingHeadMessage@@QBE?AW4InterfaceMessageTypes@@XZ

		getOffScreenArrows
			?getOffScreenArrows@CvTalkingHeadMessage@@QBE_NXZ=?getOffScreenArrows@EXE_CvTalkingHeadMessage@@QBE_NXZ

		getOnScreenArrows
			?getOnScreenArrows@CvTalkingHeadMessage@@QBE_NXZ=?getOnScreenArrows@EXE_CvTalkingHeadMessage@@QBE_NXZ

		getSound
			?getSound@CvTalkingHeadMessage@@QBEABVCvString@@XZ=?getSound@EXE_CvTalkingHeadMessage@@QBEABVCvString@@XZ

		getTarget
			?getTarget@CvTalkingHeadMessage@@QBE?AW4ChatTargetTypes@@XZ=?getTarget@EXE_CvTalkingHeadMessage@@QBE?AW4ChatTargetTypes@@XZ

		getTurn
			?getTurn@CvTalkingHeadMessage@@QBEHXZ=?getTurn@EXE_CvTalkingHeadMessage@@QBEHXZ

		getX
			?getX@CvTalkingHeadMessage@@QBEHXZ=?getX@EXE_CvTalkingHeadMessage@@QBEHXZ

		getY
			?getY@CvTalkingHeadMessage@@QBEHXZ=?getY@EXE_CvTalkingHeadMessage@@QBEHXZ

		setFromPlayer
			?setFromPlayer@CvTalkingHeadMessage@@QAEXW4PlayerTypes@@@Z=?setFromPlayer@EXE_CvTalkingHeadMessage@@QAEXW4PlayerTypes@@@Z

		setLength
			?setLength@CvTalkingHeadMessage@@QAEXH@Z=?setLength@EXE_CvTalkingHeadMessage@@QAEXH@Z

		setShown
			?setShown@CvTalkingHeadMessage@@QAEX_N@Z=?setShown@EXE_CvTalkingHeadMessage@@QAEX_N@Z

		setTarget
			?setTarget@CvTalkingHeadMessage@@QAEXW4ChatTargetTypes@@@Z=?setTarget@EXE_CvTalkingHeadMessage@@QAEXW4ChatTargetTypes@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvTalkingHeadMessage) == sizeof(CvTalkingHeadMessage));


class EXE_CvTeam : public CvTeam
{
public:
	/*
		canDeclareWar
			?canDeclareWar@CvTeam@@QBE_NW4TeamTypes@@@Z=?canDeclareWar@EXE_CvTeam@@QBE_NW4TeamTypes@@@Z

		countNumHumanGameTurnActive
			?countNumHumanGameTurnActive@CvTeam@@QBEHXZ=?countNumHumanGameTurnActive@EXE_CvTeam@@QBEHXZ

		declareWar
			?declareWar@CvTeam@@QAEXW4TeamTypes@@_NW4WarPlanTypes@@@Z=?declareWar@EXE_CvTeam@@QAEXW4TeamTypes@@_NW4WarPlanTypes@@@Z

		getAliveCount
			?getAliveCount@CvTeam@@QBEHXZ=?getAliveCount@EXE_CvTeam@@QBEHXZ

		getName
			?getName@CvTeam@@QBE?AVCvWString@@XZ=?getName@EXE_CvTeam@@QBE?AVCvWString@@XZ

		getNumMembers
			?getNumMembers@CvTeam@@QBEHXZ=?getNumMembers@EXE_CvTeam@@QBEHXZ

		init
			?init@CvTeam@@QAEXW4TeamTypes@@@Z=?init@EXE_CvTeam@@QAEXW4TeamTypes@@@Z

		isAtWar
			?isAtWar@CvTeam@@QBE_NW4TeamTypes@@@Z=?isAtWar@EXE_CvTeam@@QBE_NW4TeamTypes@@@Z

		isMapCentering
			?isMapCentering@CvTeam@@QBE_NXZ=?isMapCentering@EXE_CvTeam@@QBE_NXZ

		makePeace
			?makePeace@CvTeam@@QAEXW4TeamTypes@@_N@Z=?makePeace@EXE_CvTeam@@QAEXW4TeamTypes@@_N@Z

		reset
			?reset@CvTeam@@QAEXW4TeamTypes@@_N@Z=?reset@EXE_CvTeam@@QAEXW4TeamTypes@@_N@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvTeam) == sizeof(CvTeam));


class EXE_CvTeamAI : public CvTeamAI
{
public:
	/*
		getTeamNonInl
			?getTeamNonInl@CvTeamAI@@SAAAV1@W4TeamTypes@@@Z=?getTeamNonInl@EXE_CvTeamAI@@SAAAV1@W4TeamTypes@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvTeamAI) == sizeof(CvTeamAI));


class EXE_CvTerrainInfo : public CvTerrainInfo
{
public:
	/*
		get3DAudioScriptFootstepIndex
			?get3DAudioScriptFootstepIndex@CvTerrainInfo@@QBEHH@Z=?get3DAudioScriptFootstepIndex@EXE_CvTerrainInfo@@QBEHH@Z

		getArtDefineTag
			?getArtDefineTag@CvTerrainInfo@@QBEPBDXZ=?getArtDefineTag@EXE_CvTerrainInfo@@QBEPBDXZ

		getArtInfo
			?getArtInfo@CvTerrainInfo@@QBEPBVCvArtInfoTerrain@@XZ=?getArtInfo@EXE_CvTerrainInfo@@QBEPBVCvArtInfoTerrain@@XZ

		getWorldSoundscapeScriptId
			?getWorldSoundscapeScriptId@CvTerrainInfo@@QBEHXZ=?getWorldSoundscapeScriptId@EXE_CvTerrainInfo@@QBEHXZ

		isWater
			?isWater@CvTerrainInfo@@QBE_NXZ=?isWater@EXE_CvTerrainInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvTerrainInfo) == sizeof(CvTerrainInfo));


class EXE_CvTerrainPlaneInfo : public CvTerrainPlaneInfo
{
public:
	/*
		getBaseTexture
			?getBaseTexture@CvTerrainPlaneInfo@@QBEPBDXZ=?getBaseTexture@EXE_CvTerrainPlaneInfo@@QBEPBDXZ

		getCloseAlpha
			?getCloseAlpha@CvTerrainPlaneInfo@@QBEMXZ=?getCloseAlpha@EXE_CvTerrainPlaneInfo@@QBEMXZ

		getFogType
			?getFogType@CvTerrainPlaneInfo@@QBE?AW4FogTypes@@XZ=?getFogType@EXE_CvTerrainPlaneInfo@@QBE?AW4FogTypes@@XZ

		getMaterialAlpha
			?getMaterialAlpha@CvTerrainPlaneInfo@@QBEMXZ=?getMaterialAlpha@EXE_CvTerrainPlaneInfo@@QBEMXZ

		getTextureScalingU
			?getTextureScalingU@CvTerrainPlaneInfo@@QBEMXZ=?getTextureScalingU@EXE_CvTerrainPlaneInfo@@QBEMXZ

		getTextureScalingV
			?getTextureScalingV@CvTerrainPlaneInfo@@QBEMXZ=?getTextureScalingV@EXE_CvTerrainPlaneInfo@@QBEMXZ

		getTextureScrollRateU
			?getTextureScrollRateU@CvTerrainPlaneInfo@@QBEMXZ=?getTextureScrollRateU@EXE_CvTerrainPlaneInfo@@QBEMXZ

		getTextureScrollRateV
			?getTextureScrollRateV@CvTerrainPlaneInfo@@QBEMXZ=?getTextureScrollRateV@EXE_CvTerrainPlaneInfo@@QBEMXZ

		getZHeight
			?getZHeight@CvTerrainPlaneInfo@@QBEMXZ=?getZHeight@EXE_CvTerrainPlaneInfo@@QBEMXZ

		isCitySelection
			?isCitySelection@CvTerrainPlaneInfo@@QBE_NXZ=?isCitySelection@EXE_CvTerrainPlaneInfo@@QBE_NXZ

		isGroundPlane
			?isGroundPlane@CvTerrainPlaneInfo@@QBE_NXZ=?isGroundPlane@EXE_CvTerrainPlaneInfo@@QBE_NXZ

		isVisible
			?isVisible@CvTerrainPlaneInfo@@QBE_NXZ=?isVisible@EXE_CvTerrainPlaneInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvTerrainPlaneInfo) == sizeof(CvTerrainPlaneInfo));


class EXE_CvUnit : public CvUnit
{
public:
	/*
		NotifyEntity
			?NotifyEntity@CvUnit@@QAEXW4MissionTypes@@@Z=?NotifyEntity@EXE_CvUnit@@QAEXW4MissionTypes@@@Z

		atPlot
			?atPlot@CvUnit@@QBE_NPBVCvPlot@@@Z=?atPlot@EXE_CvUnit@@QBE_NPBVCvPlot@@@Z

		canFight
			?canFight@CvUnit@@QBE_NXZ=?canFight@EXE_CvUnit@@QBE_NXZ

		canFortify
			?canFortify@CvUnit@@QBE_NPBVCvPlot@@@Z=?canFortify@EXE_CvUnit@@QBE_NPBVCvPlot@@@Z

		canMove
			?canMove@CvUnit@@QBE_NXZ=?canMove@EXE_CvUnit@@QBE_NXZ

		canSleep
			?canSleep@CvUnit@@QBE_NPBVCvPlot@@@Z=?canSleep@EXE_CvUnit@@QBE_NPBVCvPlot@@@Z

		currCombatStrFloat
			?currCombatStrFloat@CvUnit@@QBEMPBVCvPlot@@PBV1@@Z=?currCombatStrFloat@EXE_CvUnit@@QBEMPBVCvPlot@@PBV1@@Z

		doCommand
			?doCommand@CvUnit@@QAEXW4CommandTypes@@HH@Z=?doCommand@EXE_CvUnit@@QAEXW4CommandTypes@@HH@Z
*/

	DllExport const CvArtInfoUnit* getArtInfo(int i) const
	{
		return CvUnit::getArtInfo(i);
	}
/*
		getBuildType
			?getBuildType@CvUnit@@QBE?AW4BuildTypes@@XZ=?getBuildType@EXE_CvUnit@@QBE?AW4BuildTypes@@XZ

		getButton
			?getButton@CvUnit@@QBEPBDXZ=?getButton@EXE_CvUnit@@QBEPBDXZ

		getCivilizationType
			?getCivilizationType@CvUnit@@QBE?AW4CivilizationTypes@@XZ=?getCivilizationType@EXE_CvUnit@@QBE?AW4CivilizationTypes@@XZ

		getCombatTimer
			?getCombatTimer@CvUnit@@QBEHXZ=?getCombatTimer@EXE_CvUnit@@QBEHXZ

		getCombatUnit
			?getCombatUnit@CvUnit@@QBEPAV1@XZ=?getCombatUnit@EXE_CvUnit@@QBEPAV1@XZ

		getDamage
			?getDamage@CvUnit@@QBEHXZ=?getDamage@EXE_CvUnit@@QBEHXZ

		getDomainType
			?getDomainType@CvUnit@@QBE?AW4DomainTypes@@XZ=?getDomainType@EXE_CvUnit@@QBE?AW4DomainTypes@@XZ

		getFacingDirection
			?getFacingDirection@CvUnit@@QBE?AW4DirectionTypes@@_N@Z=?getFacingDirection@EXE_CvUnit@@QBE?AW4DirectionTypes@@_N@Z

		getGroup
			?getGroup@CvUnit@@QBEPAVCvSelectionGroup@@XZ=?getGroup@EXE_CvUnit@@QBEPAVCvSelectionGroup@@XZ

		getHotKeyNumber
			?getHotKeyNumber@CvUnit@@QAEHXZ=?getHotKeyNumber@EXE_CvUnit@@QAEHXZ

		getID
			?getID@CvUnit@@QBEHXZ=?getID@EXE_CvUnit@@QBEHXZ

		getIDInfo
			?getIDInfo@CvUnit@@QBE?AUIDInfo@@XZ=?getIDInfo@EXE_CvUnit@@QBE?AUIDInfo@@XZ

		getLeaderUnitType
			?getLeaderUnitType@CvUnit@@QBE?AW4UnitTypes@@XZ=?getLeaderUnitType@EXE_CvUnit@@QBE?AW4UnitTypes@@XZ

		getName
			?getName@CvUnit@@QBE?BVCvWString@@I@Z=?getName@EXE_CvUnit@@QBE?BVCvWString@@I@Z

		getOwner
			?getOwner@CvUnit@@QBE?AW4PlayerTypes@@XZ=?getOwner@EXE_CvUnit@@QBE?AW4PlayerTypes@@XZ

		getPlayerColor
			?getPlayerColor@CvUnit@@QBE?AW4PlayerColorTypes@@W4TeamTypes@@@Z=?getPlayerColor@EXE_CvUnit@@QBE?AW4PlayerColorTypes@@W4TeamTypes@@@Z

		getPostCombatPlot
			?getPostCombatPlot@CvUnit@@QBEPAVCvPlot@@XZ=?getPostCombatPlot@EXE_CvUnit@@QBEPAVCvPlot@@XZ

		getProfession
			?getProfession@CvUnit@@QBE?AW4ProfessionTypes@@XZ=?getProfession@EXE_CvUnit@@QBE?AW4ProfessionTypes@@XZ

		getSubUnitsAlive
			?getSubUnitsAlive@CvUnit@@QBEHXZ=?getSubUnitsAlive@EXE_CvUnit@@QBEHXZ

		getTeam
			?getTeam@CvUnit@@QBE?AW4TeamTypes@@XZ=?getTeam@EXE_CvUnit@@QBE?AW4TeamTypes@@XZ

		getTransportUnit
			?getTransportUnit@CvUnit@@QBEPAV1@XZ=?getTransportUnit@EXE_CvUnit@@QBEPAV1@XZ

		getUnitInfo
			?getUnitInfo@CvUnit@@QBEAAVCvUnitInfo@@XZ=?getUnitInfo@EXE_CvUnit@@QBEAAVCvUnitInfo@@XZ

		getUnitType
			?getUnitType@CvUnit@@QBE?AW4UnitTypes@@XZ=?getUnitType@EXE_CvUnit@@QBE?AW4UnitTypes@@XZ

		getVisualCiv
			?getVisualCiv@CvUnit@@QBE?AW4CivilizationTypes@@W4TeamTypes@@@Z=?getVisualCiv@EXE_CvUnit@@QBE?AW4CivilizationTypes@@W4TeamTypes@@@Z

		getVisualOwner
			?getVisualOwner@CvUnit@@QBE?AW4PlayerTypes@@W4TeamTypes@@@Z=?getVisualOwner@EXE_CvUnit@@QBE?AW4PlayerTypes@@W4TeamTypes@@@Z

		getX
			?getX@CvUnit@@QBEHXZ=?getX@EXE_CvUnit@@QBEHXZ

		getY
			?getY@CvUnit@@QBEHXZ=?getY@EXE_CvUnit@@QBEHXZ

		hasMoved
			?hasMoved@CvUnit@@QBE_NXZ=?hasMoved@EXE_CvUnit@@QBE_NXZ

		isAttacking
			?isAttacking@CvUnit@@QBE_NXZ=?isAttacking@EXE_CvUnit@@QBE_NXZ

		isDead
			?isDead@CvUnit@@QBE_NXZ=?isDead@EXE_CvUnit@@QBE_NXZ

		isDefending
			?isDefending@CvUnit@@QBE_NXZ=?isDefending@EXE_CvUnit@@QBE_NXZ

		isEnemy
			?isEnemy@CvUnit@@QBE_NW4TeamTypes@@PBVCvPlot@@@Z=?isEnemy@EXE_CvUnit@@QBE_NW4TeamTypes@@PBVCvPlot@@@Z

		isFighting
			?isFighting@CvUnit@@QBE_NXZ=?isFighting@EXE_CvUnit@@QBE_NXZ

		isFortifyable
			?isFortifyable@CvUnit@@QBE_NXZ=?isFortifyable@EXE_CvUnit@@QBE_NXZ

		isGroupHead
			?isGroupHead@CvUnit@@QBE_NXZ=?isGroupHead@EXE_CvUnit@@QBE_NXZ

		isInfoBarDirty
			?isInfoBarDirty@CvUnit@@QBE_NXZ=?isInfoBarDirty@EXE_CvUnit@@QBE_NXZ

		isInvisible
			?isInvisible@CvUnit@@QBE_NW4TeamTypes@@_N1@Z=?isInvisible@EXE_CvUnit@@QBE_NW4TeamTypes@@_N1@Z

		isOnMap
			?isOnMap@CvUnit@@QBE_NXZ=?isOnMap@EXE_CvUnit@@QBE_NXZ

		isPromotionReady
			?isPromotionReady@CvUnit@@QBE_NXZ=?isPromotionReady@EXE_CvUnit@@QBE_NXZ

		isRanged
			?isRanged@CvUnit@@QBE_NXZ=?isRanged@EXE_CvUnit@@QBE_NXZ

		isWaiting
			?isWaiting@CvUnit@@QBE_NXZ=?isWaiting@EXE_CvUnit@@QBE_NXZ

		joinGroup
			?joinGroup@CvUnit@@QAEXPAVCvSelectionGroup@@_N1@Z=?joinGroup@EXE_CvUnit@@QAEXPAVCvSelectionGroup@@_N1@Z

		maxCombatStrFloat
			?maxCombatStrFloat@CvUnit@@QBEMPBVCvPlot@@PBV1@@Z=?maxCombatStrFloat@EXE_CvUnit@@QBEMPBVCvPlot@@PBV1@@Z

		maxHitPoints
			?maxHitPoints@CvUnit@@QBEHXZ=?maxHitPoints@EXE_CvUnit@@QBEHXZ

		plot
			?plot@CvUnit@@QBEPAVCvPlot@@XZ=?plot@EXE_CvUnit@@QBEPAVCvPlot@@XZ

		setInfoBarDirty
			?setInfoBarDirty@CvUnit@@QAEX_N@Z=?setInfoBarDirty@EXE_CvUnit@@QAEX_N@Z

		setPromotionReady
			?setPromotionReady@CvUnit@@QAEX_N@Z=?setPromotionReady@EXE_CvUnit@@QAEX_N@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvUnit) == sizeof(CvUnit));


class EXE_CvUnitFormationInfo : public CvUnitFormationInfo
{
public:
	/*
		addUnitEntry
			?addUnitEntry@CvUnitFormationInfo@@QAEXABVCvUnitEntry@@@Z=?addUnitEntry@EXE_CvUnitFormationInfo@@QAEXABVCvUnitEntry@@@Z

		getEventTypes
			?getEventTypes@CvUnitFormationInfo@@QBEABV?$vector@W4EntityEventTypes@@V?$allocator@W4EntityEventTypes@@@std@@@std@@XZ=?getEventTypes@EXE_CvUnitFormationInfo@@QBEABV?$vector@W4EntityEventTypes@@V?$allocator@W4EntityEventTypes@@@std@@@std@@XZ

		getFormationType
			?getFormationType@CvUnitFormationInfo@@QBEPBDXZ=?getFormationType@EXE_CvUnitFormationInfo@@QBEPBDXZ

		getGreatUnitEntry
			?getGreatUnitEntry@CvUnitFormationInfo@@QBEABVCvUnitEntry@@H@Z=?getGreatUnitEntry@EXE_CvUnitFormationInfo@@QBEABVCvUnitEntry@@H@Z

		getNumUnitEntries
			?getNumUnitEntries@CvUnitFormationInfo@@QBEHXZ=?getNumUnitEntries@EXE_CvUnitFormationInfo@@QBEHXZ

		getUnitEntry
			?getUnitEntry@CvUnitFormationInfo@@QBEABVCvUnitEntry@@H@Z=?getUnitEntry@EXE_CvUnitFormationInfo@@QBEABVCvUnitEntry@@H@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvUnitFormationInfo) == sizeof(CvUnitFormationInfo));


class EXE_CvUnitInfo : public CvUnitInfo
{
public:

	DllExport const CvArtInfoUnit* getArtInfo(int i, int iProfession) const
	{
		return CvUnitInfo::getArtInfo(i, iProfession);
	}

	DllExport int getDefaultProfession() const
	{
		return CvUnitInfo::getDefaultProfession();
	}

	DllExport int getDomainType() const
	{
		return CvUnitInfo::getDomainType();
	}

/*
		getFormationType
			?getFormationType@CvUnitInfo@@QBEPBDXZ=?getFormationType@EXE_CvUnitInfo@@QBEPBDXZ

		getGroupDefinitions
			?getGroupDefinitions@CvUnitInfo@@QBEHH@Z=?getGroupDefinitions@EXE_CvUnitInfo@@QBEHH@Z

		getGroupSize
			?getGroupSize@CvUnitInfo@@QBEHH@Z=?getGroupSize@EXE_CvUnitInfo@@QBEHH@Z
*/
	DllExport int getUnitClassType() const
	{
		return CvUnitInfo::getUnitClassType();
	}

/*
		getUnitGroupRequired
			?getUnitGroupRequired@CvUnitInfo@@QBEHHH@Z=?getUnitGroupRequired@EXE_CvUnitInfo@@QBEHHH@Z

		getUnitMaxSpeed
			?getUnitMaxSpeed@CvUnitInfo@@QBEMH@Z=?getUnitMaxSpeed@EXE_CvUnitInfo@@QBEMH@Z

		getUnitPadTime
			?getUnitPadTime@CvUnitInfo@@QBEMH@Z=?getUnitPadTime@EXE_CvUnitInfo@@QBEMH@Z

		isMechUnit
			?isMechUnit@CvUnitInfo@@QBE_NXZ=?isMechUnit@EXE_CvUnitInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvUnitInfo) == sizeof(CvUnitInfo));


class EXE_CvVictoryInfo : public CvVictoryInfo
{
public:
	/*
		getDefault
			?getDefault@CvVictoryInfo@@QBE_NXZ=?getDefault@EXE_CvVictoryInfo@@QBE_NXZ

		isPermanent
			?isPermanent@CvVictoryInfo@@QBE_NXZ=?isPermanent@EXE_CvVictoryInfo@@QBE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvVictoryInfo) == sizeof(CvVictoryInfo));


class EXE_CvWaterPlaneInfo : public CvWaterPlaneInfo
{
public:
	/*
		getBaseTexture
			?getBaseTexture@CvWaterPlaneInfo@@QBEPBDXZ=?getBaseTexture@EXE_CvWaterPlaneInfo@@QBEPBDXZ

		getEnvironmentTexture
			?getEnvironmentTexture@CvWaterPlaneInfo@@QBEPBDXZ=?getEnvironmentTexture@EXE_CvWaterPlaneInfo@@QBEPBDXZ

		getGridTexture
			?getGridTexture@CvWaterPlaneInfo@@QBEPBDXZ=?getGridTexture@EXE_CvWaterPlaneInfo@@QBEPBDXZ

		getMaterialAlpha
			?getMaterialAlpha@CvWaterPlaneInfo@@QBEMXZ=?getMaterialAlpha@EXE_CvWaterPlaneInfo@@QBEMXZ

		getNormalTexture
			?getNormalTexture@CvWaterPlaneInfo@@QBEPBDXZ=?getNormalTexture@EXE_CvWaterPlaneInfo@@QBEPBDXZ

		getTextureScaling
			?getTextureScaling@CvWaterPlaneInfo@@QBEMXZ=?getTextureScaling@EXE_CvWaterPlaneInfo@@QBEMXZ

		getTextureScrollRateU
			?getTextureScrollRateU@CvWaterPlaneInfo@@QBEMXZ=?getTextureScrollRateU@EXE_CvWaterPlaneInfo@@QBEMXZ

		getTextureScrollRateV
			?getTextureScrollRateV@CvWaterPlaneInfo@@QBEMXZ=?getTextureScrollRateV@EXE_CvWaterPlaneInfo@@QBEMXZ

		getTransitionTexture
			?getTransitionTexture@CvWaterPlaneInfo@@QBEPBDXZ=?getTransitionTexture@EXE_CvWaterPlaneInfo@@QBEPBDXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvWaterPlaneInfo) == sizeof(CvWaterPlaneInfo));


class EXE_CvWorldInfo : public CvWorldInfo
{
public:
	/*
		getDefaultNativePlayers
			?getDefaultNativePlayers@CvWorldInfo@@QBEHXZ=?getDefaultNativePlayers@EXE_CvWorldInfo@@QBEHXZ

		getDefaultPlayers
			?getDefaultPlayers@CvWorldInfo@@QBEHXZ=?getDefaultPlayers@EXE_CvWorldInfo@@QBEHXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvWorldInfo) == sizeof(CvWorldInfo));


class EXE_CvWorldPickerInfo : public CvWorldPickerInfo
{
public:
	/*
		getClimatePath
			?getClimatePath@CvWorldPickerInfo@@QAEPBDH@Z=?getClimatePath@EXE_CvWorldPickerInfo@@QAEPBDH@Z

		getMapName
			?getMapName@CvWorldPickerInfo@@QAEPBDXZ=?getMapName@EXE_CvWorldPickerInfo@@QAEPBDXZ

		getModelFile
			?getModelFile@CvWorldPickerInfo@@QAEPBDXZ=?getModelFile@EXE_CvWorldPickerInfo@@QAEPBDXZ

		getNumClimates
			?getNumClimates@CvWorldPickerInfo@@QAEHXZ=?getNumClimates@EXE_CvWorldPickerInfo@@QAEHXZ

		getNumSizes
			?getNumSizes@CvWorldPickerInfo@@QAEHXZ=?getNumSizes@EXE_CvWorldPickerInfo@@QAEHXZ

		getNumWaterLevelDecals
			?getNumWaterLevelDecals@CvWorldPickerInfo@@QAEHXZ=?getNumWaterLevelDecals@EXE_CvWorldPickerInfo@@QAEHXZ

		getNumWaterLevelGloss
			?getNumWaterLevelGloss@CvWorldPickerInfo@@QAEHXZ=?getNumWaterLevelGloss@EXE_CvWorldPickerInfo@@QAEHXZ

		getSize
			?getSize@CvWorldPickerInfo@@QAEMH@Z=?getSize@EXE_CvWorldPickerInfo@@QAEMH@Z

		getWaterLevelDecalPath
			?getWaterLevelDecalPath@CvWorldPickerInfo@@QAEPBDH@Z=?getWaterLevelDecalPath@EXE_CvWorldPickerInfo@@QAEPBDH@Z

		getWaterLevelGlossPath
			?getWaterLevelGlossPath@CvWorldPickerInfo@@QAEPBDH@Z=?getWaterLevelGlossPath@EXE_CvWorldPickerInfo@@QAEPBDH@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvWorldPickerInfo) == sizeof(CvWorldPickerInfo));


class EXE_CvXMLLoadUtility : public CvXMLLoadUtility
{
public:
	/*
		CleanUpGlobalVariables
			?CleanUpGlobalVariables@CvXMLLoadUtility@@QAEXXZ=?CleanUpGlobalVariables@EXE_CvXMLLoadUtility@@QAEXXZ

		LoadBasicInfos
			?LoadBasicInfos@CvXMLLoadUtility@@QAE_NXZ=?LoadBasicInfos@EXE_CvXMLLoadUtility@@QAE_NXZ

		LoadGlobalText
			?LoadGlobalText@CvXMLLoadUtility@@QAE_NXZ=?LoadGlobalText@EXE_CvXMLLoadUtility@@QAE_NXZ

		LoadGraphicOptions
			?LoadGraphicOptions@CvXMLLoadUtility@@QAE_NXZ=?LoadGraphicOptions@EXE_CvXMLLoadUtility@@QAE_NXZ

		LoadPlayerOptions
			?LoadPlayerOptions@CvXMLLoadUtility@@QAE_NXZ=?LoadPlayerOptions@EXE_CvXMLLoadUtility@@QAE_NXZ

		LoadPostMenuGlobals
			?LoadPostMenuGlobals@CvXMLLoadUtility@@QAE_NXZ=?LoadPostMenuGlobals@EXE_CvXMLLoadUtility@@QAE_NXZ

		LoadPreMenuGlobals
			?LoadPreMenuGlobals@CvXMLLoadUtility@@QAE_NXZ=?LoadPreMenuGlobals@EXE_CvXMLLoadUtility@@QAE_NXZ

		ResetGlobalEffectInfo
			?ResetGlobalEffectInfo@CvXMLLoadUtility@@QAEXXZ=?ResetGlobalEffectInfo@EXE_CvXMLLoadUtility@@QAEXXZ

		ResetLandscapeInfo
			?ResetLandscapeInfo@CvXMLLoadUtility@@QAEXXZ=?ResetLandscapeInfo@EXE_CvXMLLoadUtility@@QAEXXZ

		SetGlobalArtDefines
			?SetGlobalArtDefines@CvXMLLoadUtility@@QAE_NXZ=?SetGlobalArtDefines@EXE_CvXMLLoadUtility@@QAE_NXZ

		SetGlobalDefines
			?SetGlobalDefines@CvXMLLoadUtility@@QAE_NXZ=?SetGlobalDefines@EXE_CvXMLLoadUtility@@QAE_NXZ

		SetGlobalTypes
			?SetGlobalTypes@CvXMLLoadUtility@@QAE_NXZ=?SetGlobalTypes@EXE_CvXMLLoadUtility@@QAE_NXZ

		SetPostGlobalsGlobalDefines
			?SetPostGlobalsGlobalDefines@CvXMLLoadUtility@@QAE_NXZ=?SetPostGlobalsGlobalDefines@EXE_CvXMLLoadUtility@@QAE_NXZ

		SetupGlobalLandscapeInfo
			?SetupGlobalLandscapeInfo@CvXMLLoadUtility@@QAE_NXZ=?SetupGlobalLandscapeInfo@EXE_CvXMLLoadUtility@@QAE_NXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvXMLLoadUtility) == sizeof(CvXMLLoadUtility));


class EXE_CvYieldInfo : public CvYieldInfo
{
public:
	/*
		getChar
			?getChar@CvYieldInfo@@QBEHXZ=?getChar@EXE_CvYieldInfo@@QBEHXZ

		getColorType
			?getColorType@CvYieldInfo@@QBEHXZ=?getColorType@EXE_CvYieldInfo@@QBEHXZ

		getTextureIndex
			?getTextureIndex@CvYieldInfo@@QBEHXZ=?getTextureIndex@EXE_CvYieldInfo@@QBEHXZ

		getWaterTextureIndex
			?getWaterTextureIndex@CvYieldInfo@@QBEHXZ=?getWaterTextureIndex@EXE_CvYieldInfo@@QBEHXZ

		setChar
			?setChar@CvYieldInfo@@QAEXH@Z=?setChar@EXE_CvYieldInfo@@QAEXH@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CvYieldInfo) == sizeof(CvYieldInfo));


class EXE_CyArgsList : public CyArgsList
{
public:
	/*
		add
			?add@CyArgsList@@QAEXH@Z=?add@EXE_CyArgsList@@QAEXH@Z

		add
			?add@CyArgsList@@QAEXM@Z=?add@EXE_CyArgsList@@QAEXM@Z

		add
			?add@CyArgsList@@QAEXPAX@Z=?add@EXE_CyArgsList@@QAEXPAX@Z

		add
			?add@CyArgsList@@QAEXPBD@Z=?add@EXE_CyArgsList@@QAEXPBD@Z

		add
			?add@CyArgsList@@QAEXPBDH@Z=?add@EXE_CyArgsList@@QAEXPBDH@Z

		add
			?add@CyArgsList@@QAEXPBG@Z=?add@EXE_CyArgsList@@QAEXPBG@Z

		add
			?add@CyArgsList@@QAEXPBHH@Z=?add@EXE_CyArgsList@@QAEXPBHH@Z

		makeFunctionArgs
			?makeFunctionArgs@CyArgsList@@QAEPAXXZ=?makeFunctionArgs@EXE_CyArgsList@@QAEPAXXZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CyArgsList) == sizeof(CyArgsList));


class EXE_CyPlot : public CyPlot
{
public:
	/*
		getPoint
			?getPoint@CyPlot@@QAE?AVNiPoint3@@XZ=?getPoint@EXE_CyPlot@@QAE?AVNiPoint3@@XZ

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_CyPlot) == sizeof(CyPlot));


class EXE_PopupReturn : public PopupReturn
{
public:
	/*
		getButtonClicked
			?getButtonClicked@PopupReturn@@QBEHH@Z=?getButtonClicked@EXE_PopupReturn@@QBEHH@Z

		getCheckboxBitfield
			?getCheckboxBitfield@PopupReturn@@QBEHH@Z=?getCheckboxBitfield@EXE_PopupReturn@@QBEHH@Z

		getCurrentSpinBoxValue
			?getCurrentSpinBoxValue@PopupReturn@@QBEHH@Z=?getCurrentSpinBoxValue@EXE_PopupReturn@@QBEHH@Z

		getEditBoxString
			?getEditBoxString@PopupReturn@@QBEPBGH@Z=?getEditBoxString@EXE_PopupReturn@@QBEPBGH@Z

		getSelectedListBoxValue
			?getSelectedListBoxValue@PopupReturn@@QBEHH@Z=?getSelectedListBoxValue@EXE_PopupReturn@@QBEHH@Z

		getSelectedPullDownValue
			?getSelectedPullDownValue@PopupReturn@@QBEHH@Z=?getSelectedPullDownValue@EXE_PopupReturn@@QBEHH@Z

		getSelectedRadioButton
			?getSelectedRadioButton@PopupReturn@@QBEHH@Z=?getSelectedRadioButton@EXE_PopupReturn@@QBEHH@Z

		getSpinnerWidgetValue
			?getSpinnerWidgetValue@PopupReturn@@QBEHH@Z=?getSpinnerWidgetValue@EXE_PopupReturn@@QBEHH@Z

		read
			?read@PopupReturn@@QAEXPAVFDataStreamBase@@@Z=?read@EXE_PopupReturn@@QAEXPAVFDataStreamBase@@@Z

		setButtonClicked
			?setButtonClicked@PopupReturn@@QAEXHH@Z=?setButtonClicked@EXE_PopupReturn@@QAEXHH@Z

		setCheckboxBitfield
			?setCheckboxBitfield@PopupReturn@@QAEXHH@Z=?setCheckboxBitfield@EXE_PopupReturn@@QAEXHH@Z

		setCurrentSpinBoxValue
			?setCurrentSpinBoxValue@PopupReturn@@QAEXHH@Z=?setCurrentSpinBoxValue@EXE_PopupReturn@@QAEXHH@Z

		setEditBoxString
			?setEditBoxString@PopupReturn@@QAEXPBGH@Z=?setEditBoxString@EXE_PopupReturn@@QAEXPBGH@Z

		setSelectedListBoxValue
			?setSelectedListBoxValue@PopupReturn@@QAEXHH@Z=?setSelectedListBoxValue@EXE_PopupReturn@@QAEXHH@Z

		setSelectedPulldownValue
			?setSelectedPulldownValue@PopupReturn@@QAEXHH@Z=?setSelectedPulldownValue@EXE_PopupReturn@@QAEXHH@Z

		setSelectedRadioButton
			?setSelectedRadioButton@PopupReturn@@QAEXHH@Z=?setSelectedRadioButton@EXE_PopupReturn@@QAEXHH@Z

		write
			?write@PopupReturn@@QBEXPAVFDataStreamBase@@@Z=?write@EXE_PopupReturn@@QBEXPAVFDataStreamBase@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_PopupReturn) == sizeof(PopupReturn));


class EXE_TradeData : public TradeData
{
public:
	/*
		read
			?read@TradeData@@QAEXPAVFDataStreamBase@@@Z=?read@EXE_TradeData@@QAEXPAVFDataStreamBase@@@Z

		write
			?write@TradeData@@QBEXPAVFDataStreamBase@@@Z=?write@EXE_TradeData@@QBEXPAVFDataStreamBase@@@Z

	*/
};
BOOST_STATIC_ASSERT(sizeof(EXE_TradeData) == sizeof(TradeData));
