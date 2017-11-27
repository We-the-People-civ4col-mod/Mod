//
// XML Set functions
//

#include "CvGameCoreDLL.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvXMLLoadUtility.h"
#include "CvGlobals.h"
#include "CvArtFileMgr.h"
#include "CvGameTextMgr.h"
#include <algorithm>
#include "CvInfoWater.h"
#include "FProfiler.h"
#include "FVariableSystem.h"
#include "CvGameCoreUtils.h"

/// xml verification
#ifdef FASSERT_ENABLE

//
// function to loop xml files and assert on illegal settings
// this is usually variable combos, which will cause asserts and bugs later
static void verifyXMLsettings()
{
	for (int i=0; i < GC.getNumEventTriggerInfos(); ++i)
	{
		EventTriggerTypes eTrigger = (EventTriggerTypes)i;

		CvEventTriggerInfo& kInfo = GC.getEventTriggerInfo(eTrigger);
		
		if (kInfo.isUnitsOnPlot())
		{
			FAssertMsg(isPlotEventTrigger(eTrigger), CvString::format("XML error: %s has bUnitsOnPlot set, but failed isPlotEventTrigger()", kInfo.getType()));
		}
	}
}
#endif
/// xml verification



/// XML type preloading - start - Nightinggale

// the concept of preloading XML file types is as follows:
// As soon as possible, all XML files are read for their basic info.
// Just before reaching main menu, all files are read again, this time for their entire content.
//
// The result is that GC becomes aware of indexes for type strings and vector lengths right away.
// This mean functions like FindInInfoClass() and getNumUnitInfos() works much earlier in the startup process.
// Very importantly, they work for all types prior to actually reading any XML content or even CvPlayer constructor.
//
// The actual content is read just before reaching the main menu.
// This can't be done earlier due to other setup handled by the exe, but it is complete before reaching main menu.
//
// XML types are checked for name clashes. It will assert if a name is used more than once.
// Bugs caused by nameclashes are hard to debug and find the cause for. Example:
// If unit 17 clashes with a name giving the string ID 4, then it will overwrite unit 4.
// The bughunting will then focus on the missing unit 4, which is presumably bugfree.
//
// Effect on modules: unknown
// This feature can be turned off by not defining USE_XML_TYPE_PRELOAD


bool bFirstRead = false;

// same as LoadGlobalClassInfo, except that it clears vectors without types
template <class T>
void CvXMLLoadUtility::PreLoadGlobalClassInfo(std::vector<T*>& aInfos, const char* szFileRoot, const char* szFileDirectory, const char* szXmlPath, bool bTwoPass, CvCacheObject* (CvDLLUtilityIFaceBase::*pArgFunction) (const TCHAR*))
{
	if (!bFirstRead && aInfos.size() > 0 && aInfos[0]->getType() == NULL)
	{
		// Vector has content without types
		// with no types to find indexes for, all XML data will be added to the end
		// clear the vector to avoid dublicates (not to mention some with only basic info)
		aInfos.clear();
	}
	LoadGlobalClassInfo(aInfos, szFileRoot, szFileDirectory, szXmlPath, NULL);
}

// progress display when reading actual data, not just the types
void CvXMLLoadUtility::PreUpdateProgressCB(const char* szMessage)
{
	if (!bFirstRead)
	{
		UpdateProgressCB(szMessage);
	}
}

void CvXMLLoadUtility::readXMLfiles(bool bFirst)
{
	bFirstRead = bFirst;

	// 4 LoadBasicInfos
	PreLoadGlobalClassInfo(GC.getConceptInfo(), "CIV4BasicInfos", "BasicInfos", "Civ4BasicInfos/ConceptInfos/ConceptInfo", NULL);
	PreLoadGlobalClassInfo(GC.getCalendarInfo(), "CIV4CalendarInfos", "BasicInfos", "Civ4CalendarInfos/CalendarInfos/CalendarInfo", NULL);
	PreLoadGlobalClassInfo(GC.getSeasonInfo(), "CIV4SeasonInfos", "BasicInfos", "Civ4SeasonInfos/SeasonInfos/SeasonInfo", NULL);
	PreLoadGlobalClassInfo(GC.getMonthInfo(), "CIV4MonthInfos", "BasicInfos", "Civ4MonthInfos/MonthInfos/MonthInfo", NULL);
	PreLoadGlobalClassInfo(GC.getDenialInfo(), "CIV4DenialInfos", "BasicInfos", "Civ4DenialInfos/DenialInfos/DenialInfo", NULL);
	PreLoadGlobalClassInfo(GC.getInvisibleInfo(), "CIV4InvisibleInfos", "BasicInfos", "Civ4InvisibleInfos/InvisibleInfos/InvisibleInfo", NULL);
	PreLoadGlobalClassInfo(GC.getUnitCombatInfo(), "CIV4UnitCombatInfos", "BasicInfos", "Civ4UnitCombatInfos/UnitCombatInfos/UnitCombatInfo", NULL);
	PreLoadGlobalClassInfo(GC.getDomainInfo(), "CIV4DomainInfos", "BasicInfos", "Civ4DomainInfos/DomainInfos/DomainInfo", NULL);
	PreLoadGlobalClassInfo(GC.getUnitAIInfo(), "CIV4UnitAIInfos", "BasicInfos", "Civ4UnitAIInfos/UnitAIInfos/UnitAIInfo", NULL);
	PreLoadGlobalClassInfo(GC.getAttitudeInfo(), "CIV4AttitudeInfos", "BasicInfos", "Civ4AttitudeInfos/AttitudeInfos/AttitudeInfo", NULL);
	PreLoadGlobalClassInfo(GC.getMemoryInfo(), "CIV4MemoryInfos", "BasicInfos", "Civ4MemoryInfos/MemoryInfos/MemoryInfo", NULL);
	PreLoadGlobalClassInfo(GC.getFatherCategoryInfo(), "CIV4FatherCategoryInfos", "BasicInfos", "Civ4FatherCategoryInfos/FatherCategoryInfos/FatherCategoryInfo", NULL);

	// 5 LoadPreMenuGlobals
	PreLoadGlobalClassInfo(GC.getColorInfo(), "CIV4ColorVals", "Interface", "Civ4ColorVals/ColorVals/ColorVal", NULL);
	PreLoadGlobalClassInfo(GC.getUnitClassInfo(), "CIV4UnitClassInfos", "Units", "Civ4UnitClassInfos/UnitClassInfos/UnitClassInfo", NULL);
	PreLoadGlobalClassInfo(GC.getCultureLevelInfo(), "CIV4CultureLevelInfo", "GameInfo", "Civ4CultureLevelInfo/CultureLevelInfos/CultureLevelInfo", NULL);
	PreLoadGlobalClassInfo(GC.getVictoryInfo(), "CIV4VictoryInfo", "GameInfo", "Civ4VictoryInfo/VictoryInfos/VictoryInfo", NULL);
	PreLoadGlobalClassInfo(GC.getBuildingClassInfo(), "CIV4BuildingClassInfos", "Buildings", "Civ4BuildingClassInfos/BuildingClassInfos/BuildingClassInfo", NULL);
	PreLoadGlobalClassInfo(GC.getYieldInfo(), "CIV4YieldInfos", "Terrain", "Civ4YieldInfos/YieldInfos/YieldInfo", NULL);
	PreLoadGlobalClassInfo(GC.getAlarmInfo(), "CIV4AlarmInfos", "Civilizations", "Civ4AlarmInfos/AlarmInfos/AlarmInfo", NULL);
	PreLoadGlobalClassInfo(GC.getGameSpeedInfo(), "CIV4GameSpeedInfo", "GameInfo", "Civ4GameSpeedInfo/GameSpeedInfos/GameSpeedInfo", NULL);
	PreLoadGlobalClassInfo(GC.getTurnTimerInfo(), "CIV4TurnTimerInfo", "GameInfo", "Civ4TurnTimerInfo/TurnTimerInfos/TurnTimerInfo", NULL);
	PreLoadGlobalClassInfo(GC.getWorldInfo(), "CIV4WorldInfo", "GameInfo", "Civ4WorldInfo/WorldInfos/WorldInfo", NULL);
	PreLoadGlobalClassInfo(GC.getClimateInfo(), "CIV4ClimateInfo", "GameInfo", "Civ4ClimateInfo/ClimateInfos/ClimateInfo", NULL);
	PreLoadGlobalClassInfo(GC.getSeaLevelInfo(), "CIV4SeaLevelInfo", "GameInfo", "Civ4SeaLevelInfo/SeaLevelInfos/SeaLevelInfo", NULL);
	PreLoadGlobalClassInfo(GC.getEuropeInfo(), "CIV4EuropeInfo", "GameInfo", "Civ4EuropeInfo/EuropeInfos/EuropeInfo", NULL);
	PreLoadGlobalClassInfo(GC.getTerrainInfo(), "CIV4TerrainInfos", "Terrain", "Civ4TerrainInfos/TerrainInfos/TerrainInfo", NULL);
	PreLoadGlobalClassInfo(GC.getEraInfo(), "CIV4EraInfos", "GameInfo", "Civ4EraInfos/EraInfos/EraInfo", NULL);
	PreLoadGlobalClassInfo(GC.getFeatureInfo(), "Civ4FeatureInfos", "Terrain", "Civ4FeatureInfos/FeatureInfos/FeatureInfo", NULL);
	PreLoadGlobalClassInfo(GC.getPromotionInfo(), "CIV4PromotionInfos", "Units", "Civ4PromotionInfos/PromotionInfos/PromotionInfo", &CvDLLUtilityIFaceBase::createPromotionInfoCacheObject);
	PreLoadGlobalClassInfo(GC.getProfessionInfo(), "CIV4ProfessionInfos", "Units", "Civ4ProfessionInfos/ProfessionInfos/ProfessionInfo", &CvDLLUtilityIFaceBase::createProfessionInfoCacheObject);
	PreLoadGlobalClassInfo(GC.getGoodyInfo(), "CIV4GoodyInfo", "GameInfo", "Civ4GoodyInfo/GoodyInfos/GoodyInfo", NULL);
	PreLoadGlobalClassInfo(GC.getTraitInfo(), "CIV4TraitInfos", "Civilizations", "Civ4TraitInfos/TraitInfos/TraitInfo", &CvDLLUtilityIFaceBase::createTraitInfoCacheObject);
	PreLoadGlobalClassInfo(GC.getSpecialBuildingInfo(), "CIV4SpecialBuildingInfos", "Buildings", "Civ4SpecialBuildingInfos/SpecialBuildingInfos/SpecialBuildingInfo", NULL);
	if (!bFirstRead)
	{
		for (int i=0; i < GC.getNumProfessionInfos(); ++i)
		{
			GC.getProfessionInfo((ProfessionTypes)i).readPass3();
		}
	}
	PreLoadGlobalClassInfo(GC.getAnimationCategoryInfo(), "CIV4AnimationInfos", "Units", "Civ4AnimationInfos/AnimationCategories/AnimationCategory", NULL);
	PreLoadGlobalClassInfo(GC.getAnimationPathInfo(), "CIV4AnimationPathInfos", "Units", "Civ4AnimationPathInfos/AnimationPaths/AnimationPath", NULL);
	PreLoadGlobalClassInfo(GC.getHandicapInfo(), "CIV4HandicapInfo", "GameInfo", "Civ4HandicapInfo/HandicapInfos/HandicapInfo", &CvDLLUtilityIFaceBase::createHandicapInfoCacheObject);
	PreLoadGlobalClassInfo(GC.getCursorInfo(), "CIV4CursorInfo", "GameInfo", "Civ4CursorInfo/CursorInfos/CursorInfo", NULL);
	PreLoadGlobalClassInfo(GC.getCivicOptionInfo(), "CIV4CivicOptionInfos", "GameInfo", "Civ4CivicOptionInfos/CivicOptionInfos/CivicOptionInfo", NULL);
	PreLoadGlobalClassInfo(GC.getHurryInfo(), "CIV4HurryInfo", "GameInfo", "Civ4HurryInfo/HurryInfos/HurryInfo", NULL);
	PreLoadGlobalClassInfo(GC.getBuildingInfo(), "CIV4BuildingInfos", "Buildings", "Civ4BuildingInfos/BuildingInfos/BuildingInfo", &CvDLLUtilityIFaceBase::createBuildingInfoCacheObject);
	if (!bFirstRead)
	{
		for (int i=0; i < GC.getNumBuildingClassInfos(); ++i)
		{
			GC.getBuildingClassInfo((BuildingClassTypes)i).readPass3();
		}
	}
	PreLoadGlobalClassInfo(GC.getBonusInfo(), "CIV4BonusInfos", "Terrain", "Civ4BonusInfos/BonusInfos/BonusInfo", &CvDLLUtilityIFaceBase::createBonusInfoCacheObject);
	PreLoadGlobalClassInfo(GC.getRouteInfo(), "Civ4RouteInfos", "Misc", "Civ4RouteInfos/RouteInfos/RouteInfo", NULL);
	PreLoadGlobalClassInfo(GC.getImprovementInfo(), "CIV4ImprovementInfos", "Terrain", "Civ4ImprovementInfos/ImprovementInfos/ImprovementInfo", &CvDLLUtilityIFaceBase::createImprovementInfoCacheObject);
	PreLoadGlobalClassInfo(GC.getFatherPointInfo(), "CIV4FatherPointInfos", "GameInfo", "Civ4FatherPointInfos/FatherPointInfos/FatherPointInfo", NULL);
	PreLoadGlobalClassInfo(GC.getFatherInfo(), "CIV4FatherInfos", "GameInfo", "Civ4FatherInfos/FatherInfos/FatherInfo", &CvDLLUtilityIFaceBase::createFatherInfoCacheObject);
	PreLoadGlobalClassInfo(GC.getSpecialUnitInfo(), "CIV4SpecialUnitInfos", "Units", "Civ4SpecialUnitInfos/SpecialUnitInfos/SpecialUnitInfo", NULL);
	PreLoadGlobalClassInfo(GC.getCivicInfo(), "CIV4CivicInfos", "GameInfo", "Civ4CivicInfos/CivicInfos/CivicInfo", &CvDLLUtilityIFaceBase::createCivicInfoCacheObject);
	PreLoadGlobalClassInfo(GC.getLeaderHeadInfo(), "CIV4LeaderHeadInfos", "Civilizations", "Civ4LeaderHeadInfos/LeaderHeadInfos/LeaderHeadInfo", &CvDLLUtilityIFaceBase::createLeaderHeadInfoCacheObject);
	PreLoadGlobalClassInfo(GC.getPlayerColorInfo(), "CIV4PlayerColorInfos", "Interface", "Civ4PlayerColorInfos/PlayerColorInfos/PlayerColorInfo", NULL);
	PreLoadGlobalClassInfo(GC.getEffectInfo(), "CIV4EffectInfos", "Misc", "Civ4EffectInfos/EffectInfos/EffectInfo", NULL);
	PreLoadGlobalClassInfo(GC.getEntityEventInfo(), "CIV4EntityEventInfos", "Units", "Civ4EntityEventInfos/EntityEventInfos/EntityEventInfo", NULL);
	PreLoadGlobalClassInfo(GC.getBuildInfo(), "CIV4BuildInfos", "Units", "Civ4BuildInfos/BuildInfos/BuildInfo", NULL);
	PreLoadGlobalClassInfo(GC.getUnitInfo(), "CIV4UnitInfos", "Units", "Civ4UnitInfos/UnitInfos/UnitInfo", &CvDLLUtilityIFaceBase::createUnitInfoCacheObject);
	if (!bFirst)
	{
		for (int i=0; i < GC.getNumUnitClassInfos(); ++i)
		{
			GC.getUnitClassInfo((UnitClassTypes)i).readPass3();
		}
	}
	//Androrc UnitArtStyles
	PreLoadGlobalClassInfo(GC.getUnitArtStyleTypeInfo(), "CIV4UnitArtStyleTypeInfos", "Civilizations", "Civ4UnitArtStyleTypeInfos/UnitArtStyleTypeInfos/UnitArtStyleTypeInfo", false);
	//Androrc End
	PreLoadGlobalClassInfo(GC.getCivilizationInfo(), "CIV4CivilizationInfos", "Civilizations", "Civ4CivilizationInfos/CivilizationInfos/CivilizationInfo", &CvDLLUtilityIFaceBase::createCivilizationInfoCacheObject);
	PreLoadGlobalClassInfo(GC.getHints(), "CIV4Hints", "GameInfo", "Civ4Hints/HintInfos/HintInfo", NULL);
	PreLoadGlobalClassInfo(GC.getMainMenus(), "CIV4MainMenus", "Art", "Civ4MainMenus/MainMenus/MainMenu", NULL);
	PreLoadGlobalClassInfo(GC.getSlideShowInfo(), "CIV4SlideShowInfos", "Interface", "Civ4SlideShowInfos/SlideShowInfos/SlideShowInfo", NULL);
	PreLoadGlobalClassInfo(GC.getSlideShowRandomInfo(), "CIV4SlideShowRandomInfos", "Interface", "Civ4SlideShowRandomInfos/SlideShowRandomInfos/SlideShowRandomInfo", NULL);
	PreLoadGlobalClassInfo(GC.getWorldPickerInfo(), "CIV4WorldPickerInfos", "Interface", "Civ4WorldPickerInfos/WorldPickerInfos/WorldPickerInfo", NULL);

	PreLoadGlobalClassInfo(GC.getGameOptionInfo(), "CIV4GameOptionInfos", "GameInfo", "Civ4GameOptionInfos/GameOptionInfos/GameOptionInfo", NULL);
	PreLoadGlobalClassInfo(GC.getMPOptionInfo(), "CIV4MPOptionInfos", "GameInfo", "Civ4MPOptionInfos/MPOptionInfos/MPOptionInfo", NULL);
	PreLoadGlobalClassInfo(GC.getForceControlInfo(), "CIV4ForceControlInfos", "GameInfo", "Civ4ForceControlInfos/ForceControlInfos/ForceControlInfo", NULL);

	// PatchMod: Achievements START
	PreLoadGlobalClassInfo(GC.getAchieveInfo(), "CIV4AchieveInfos", "Events", "Civ4AchieveInfos/AchieveInfo", NULL);
	// PatchMod: Achievements END

	// 6 SetupGlobalLandscapeInfo
	PreLoadGlobalClassInfo(GC.getLandscapeInfo(), "CIV4TerrainSettings", "Terrain", "Civ4TerrainSettings/LandscapeInfos/LandscapeInfo", NULL);

	// 7 LoadPostMenuGlobals
	PreUpdateProgressCB("Global Events");

	PreLoadGlobalClassInfo(GC.getEventInfo(), "CIV4EventInfos", "Events", "Civ4EventInfos/EventInfos/EventInfo", &CvDLLUtilityIFaceBase::createEventInfoCacheObject);
	PreLoadGlobalClassInfo(GC.getEventTriggerInfo(), "CIV4EventTriggerInfos", "Events", "Civ4EventTriggerInfos/EventTriggerInfos/EventTriggerInfo", &CvDLLUtilityIFaceBase::createEventTriggerInfoCacheObject);

	PreUpdateProgressCB("Global Routes");

	PreLoadGlobalClassInfo(GC.getRouteModelInfo(), "Civ4RouteModelInfos", "Art", "Civ4RouteModelInfos/RouteModelInfos/RouteModelInfo", NULL);

	PreUpdateProgressCB("Global Rivers");

	PreLoadGlobalClassInfo(GC.getRiverModelInfo(), "CIV4RiverModelInfos", "Art", "Civ4RiverModelInfos/RiverModelInfos/RiverModelInfo", NULL);

	PreUpdateProgressCB("Global Other");

	PreLoadGlobalClassInfo(GC.getWaterPlaneInfo(), "CIV4WaterPlaneInfos", "Misc", "Civ4WaterPlaneInfos/WaterPlaneInfos/WaterPlaneInfo", NULL);
	PreLoadGlobalClassInfo(GC.getTerrainPlaneInfo(), "CIV4TerrainPlaneInfos", "Misc", "Civ4TerrainPlaneInfos/TerrainPlaneInfos/TerrainPlaneInfo", NULL);
	PreLoadGlobalClassInfo(GC.getCameraOverlayInfo(), "CIV4CameraOverlayInfos", "Misc", "Civ4CameraOverlayInfos/CameraOverlayInfos/CameraOverlayInfo", NULL);

	PreUpdateProgressCB("Global Emphasize");

	PreLoadGlobalClassInfo(GC.getEmphasizeInfo(), "CIV4EmphasizeInfo", "GameInfo", "Civ4EmphasizeInfo/EmphasizeInfos/EmphasizeInfo", NULL);

	PreUpdateProgressCB("Global Other");

	PreLoadGlobalClassInfo(GC.getMissionInfo(), "CIV4MissionInfos", "Units", "Civ4MissionInfos/MissionInfos/MissionInfo", NULL);
	PreLoadGlobalClassInfo(GC.getControlInfo(), "CIV4ControlInfos", "Units", "Civ4ControlInfos/ControlInfos/ControlInfo", NULL);
	PreLoadGlobalClassInfo(GC.getCommandInfo(), "CIV4CommandInfos", "Units", "Civ4CommandInfos/CommandInfos/CommandInfo", NULL);
	PreLoadGlobalClassInfo(GC.getAutomateInfo(), "CIV4AutomateInfos", "Units", "Civ4AutomateInfos/AutomateInfos/AutomateInfo", NULL);

	PreUpdateProgressCB("Global Interface");

	PreLoadGlobalClassInfo(GC.getInterfaceModeInfo(), "CIV4InterfaceModeInfos", "Interface", "Civ4InterfaceModeInfos/InterfaceModeInfos/InterfaceModeInfo", NULL);

	if (!bFirst)
	{
		SetGlobalActionInfo();
	}

	// Load the formation info
	PreLoadGlobalClassInfo(GC.getUnitFormationInfo(), "CIV4FormationInfos", "Units", "UnitFormations/UnitFormation", NULL);

	// Load the attachable infos
	PreLoadGlobalClassInfo(GC.getAttachableInfo(), "CIV4AttachableInfos", "Misc", "Civ4AttachableInfos/AttachableInfos/AttachableInfo", NULL);

	if (bFirst)
	{
		// preloading the types will not require the special case code
		PreLoadGlobalClassInfo(GC.getDiplomacyInfo(), "CIV4DiplomacyInfos", "GameInfo", "Civ4DiplomacyInfos/DiplomacyInfos/DiplomacyInfo", &CvDLLUtilityIFaceBase::createDiplomacyInfoCacheObject);
	}
	else
	{
		// Special Case Diplomacy Info due to double vectored nature and appending of Responses
		LoadDiplomacyInfo(GC.getDiplomacyInfo(), "CIV4DiplomacyInfos", "GameInfo", "Civ4DiplomacyInfos/DiplomacyInfos/DiplomacyInfo", &CvDLLUtilityIFaceBase::createDiplomacyInfoCacheObject);
	}


	bFirstRead = false;
}
/// XML type preloading - end - Nightinggale

bool CvXMLLoadUtility::ReadGlobalDefines(const TCHAR* szXMLFileName, CvCacheObject* cache)
{
	bool bLoaded = false;	// used to make sure that the xml file was loaded correctly

	if (!gDLL->cacheRead(cache, szXMLFileName))			// src data file name
	{
		// load normally
		if (!CreateFXml())
		{
			return false;
		}

		// load the new FXml variable with the szXMLFileName file
		bLoaded = LoadCivXml(m_pFXml, szXMLFileName);
		if (!bLoaded)
		{
			char	szMessage[1024];
			sprintf( szMessage, "LoadXML call failed for %s \n Current XML file is: %s", szXMLFileName, GC.getCurrentXMLFile().GetCString());
			gDLL->MessageBox(szMessage, "XML Load Error");
		}

		// if the load succeeded we will continue
		if (bLoaded)
		{
			// if the xml is successfully validated
			if (Validate())
			{
				// locate the first define tag in the xml
				if (gDLL->getXMLIFace()->LocateNode(m_pFXml,"Civ4Defines/Define"))
				{
					int i;	// loop counter
					// get the number of other Define tags in the xml file
					int iNumDefines = gDLL->getXMLIFace()->GetNumSiblings(m_pFXml);
					// add one to the total in order to include the current Define tag
					iNumDefines++;

					// loop through all the Define tags
					for (i=0;i<iNumDefines;i++)
					{
						char szNodeType[256];	// holds the type of the current node
						char szName[256];

						// Skip any comments and stop at the next value we might want
						if (SkipToNextVal())
						{
							// call the function that sets the FXml pointer to the first non-comment child of
							// the current tag and gets the value of that new node
							if (GetChildXmlVal(szName))
							{
								// set the FXml pointer to the next sibling of the current tag``
								if (gDLL->getXMLIFace()->NextSibling(GetXML()))
								{
									// Skip any comments and stop at the next value we might want
									if (SkipToNextVal())
									{
										// if we successfuly get the node type for the current tag
										if (gDLL->getXMLIFace()->GetLastLocatedNodeType(GetXML(),szNodeType))
										{
											// if the node type of the current tag isn't null
											if (strcmp(szNodeType,"")!=0)
											{
												// if the node type of the current tag is a float then
												if (strcmp(szNodeType,"float")==0)
												{
													// get the float value for the define
													float fVal;
													GetXmlVal(&fVal);
													GC.getDefinesVarSystem()->SetValue(szName, fVal);
												}
												// else if the node type of the current tag is an int then
												else if (strcmp(szNodeType,"int")==0)
												{
													// get the int value for the define
													int iVal;
													GetXmlVal(&iVal);
													GC.getDefinesVarSystem()->SetValue(szName, iVal);
												}
												// else if the node type of the current tag is a boolean then
												else if (strcmp(szNodeType,"boolean")==0)
												{
													// get the boolean value for the define
													bool bVal;
													GetXmlVal(&bVal);
													GC.getDefinesVarSystem()->SetValue(szName, bVal);
												}
												// otherwise we will assume it is a string/text value
												else
												{
													char szVal[256];
													// get the string/text value for the define
													GetXmlVal(szVal);
													GC.getDefinesVarSystem()->SetValue(szName, szVal);
												}
											}
											// otherwise we will default to getting the string/text value for the define
											else
											{
												char szVal[256];
												// get the string/text value for the define
												GetXmlVal(szVal);
												GC.getDefinesVarSystem()->SetValue(szName, szVal);
											}
										}
									}
								}

								// since we are looking at the children of a Define tag we will need to go up
								// one level so that we can go to the next Define tag.
								// Set the FXml pointer to the parent of the current tag
								gDLL->getXMLIFace()->SetToParent(GetXML());
							}
						}

						// now we set the FXml pointer to the sibling of the current tag, which should be the next
						// Define tag
						if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
						{
							break;
						}
					}

					// write global defines info to cache
					bool bOk = gDLL->cacheWrite(cache);
					if (!bOk)
					{
						char	szMessage[1024];
						sprintf( szMessage, "Failed writing to global defines cache. \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
						gDLL->MessageBox(szMessage, "XML Caching Error");
					}
					else
					{
						logMsg("Wrote GlobalDefines to cache");
					}
				}
			}
		}

		// delete the pointer to the FXml variable
		gDLL->getXMLIFace()->DestroyFXml(m_pFXml);
	}
	else
	{
		logMsg("Read GobalDefines from cache");
	}

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalDefines()
//
//  PURPOSE :   Initialize the variables located in globaldefines.cpp/h with the values in
//				GlobalDefines.xml
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetGlobalDefines()
{
	UpdateProgressCB("GlobalDefines");

	/////////////////////////////////
	//
	// use disk cache if possible.
	// if no cache or cache is older than xml file, use xml file like normal, else read from cache
	//

	CvCacheObject* cache = gDLL->createGlobalDefinesCacheObject("GlobalDefines.dat");	// cache file name

	if (!ReadGlobalDefines("xml\\Art\\GlobalArtDefines.xml", cache))  // read these first! Important to prevent cheating
	{
		return false;
	}

	if (!ReadGlobalDefines("xml\\GlobalDefines.xml", cache))
	{
		return false;
	}

	if (!ReadGlobalDefines("xml\\GlobalDefinesAlt.xml", cache))
	{
		return false;
	}

	if (!ReadGlobalDefines("xml\\PythonCallbackDefines.xml", cache))
	{
		return false;
	}

	if (gDLL->isModularXMLLoading())
	{
		std::vector<CvString> aszFiles;
		gDLL->enumerateFiles(aszFiles, "modules\\*_GlobalDefines.xml");

		for (std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
		{
			if (!ReadGlobalDefines(*it, cache))
			{
				return false;
			}
		}

		std::vector<CvString> aszModularFiles;
		gDLL->enumerateFiles(aszModularFiles, "modules\\*_PythonCallbackDefines.xml");

		for (std::vector<CvString>::iterator it = aszModularFiles.begin(); it != aszModularFiles.end(); ++it)
		{
			if (!ReadGlobalDefines(*it, cache))
			{
				return false;
			}
		}
	}


	gDLL->destroyCache(cache);
	////////////////////////////////////////////////////////////////////////

	GC.cacheGlobals();

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetPostGlobalsGlobalDefines()
//
//  PURPOSE :   This function assumes that the SetGlobalDefines function has already been called
//							it then loads the few global defines that needed to reference a global variable that
//							hadn't been loaded in prior to the SetGlobalDefines call
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetPostGlobalsGlobalDefines()
{
	const char* szVal=NULL;		// holds the string value from the define queue
	int idx;

	if (GC.getDefinesVarSystem()->GetSize() > 0)
	{
		SetGlobalDefine("LAND_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("LAND_TERRAIN", idx);

		SetGlobalDefine("DEEP_WATER_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEEP_WATER_TERRAIN", idx);

		SetGlobalDefine("SHALLOW_WATER_TERRAIN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("SHALLOW_WATER_TERRAIN", idx);

		SetGlobalDefine("LAND_IMPROVEMENT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("LAND_IMPROVEMENT", idx);

		SetGlobalDefine("WATER_IMPROVEMENT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("WATER_IMPROVEMENT", idx);

		SetGlobalDefine("RUINS_IMPROVEMENT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("RUINS_IMPROVEMENT", idx);

		SetGlobalDefine("CAPITAL_BUILDINGCLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("CAPITAL_BUILDINGCLASS", idx);

		SetGlobalDefine("DEFAULT_POPULATION_UNIT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEFAULT_POPULATION_UNIT", idx);

		SetGlobalDefine("INITIAL_CITY_ROUTE_TYPE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("INITIAL_CITY_ROUTE_TYPE", idx);

		SetGlobalDefine("STANDARD_HANDICAP", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_HANDICAP", idx);

		SetGlobalDefine("STANDARD_GAMESPEED", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_GAMESPEED", idx);

		SetGlobalDefine("STANDARD_TURNTIMER", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_TURNTIMER", idx);

		SetGlobalDefine("STANDARD_CLIMATE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_CLIMATE", idx);

		SetGlobalDefine("STANDARD_SEALEVEL", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_SEALEVEL", idx);

		SetGlobalDefine("STANDARD_ERA", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_ERA", idx);

		SetGlobalDefine("STANDARD_CALENDAR", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_CALENDAR", idx);

		SetGlobalDefine("AI_HANDICAP", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("AI_HANDICAP", idx);

		SetGlobalDefine("BARBARIAN_CIVILIZATION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BARBARIAN_CIVILIZATION", idx);

		// < JAnimals Mod Start >
		SetGlobalDefine("BARBARIAN_LEADER", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BARBARIAN_LEADER", idx);
		// < JAnimals Mod End >

		// R&R, ray, the Church - START
		SetGlobalDefine("CHURCH_CIVILIZATION", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("CHURCH_CIVILIZATION", idx);

		SetGlobalDefine("CHURCH_LEADER", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("CHURCH_LEADER", idx);
		// R&R, ray, the Church - END

		SetGlobalDefine("TREASURE_UNITCLASS", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("TREASURE_UNITCLASS", idx);

		SetGlobalDefine("CULTURE_YIELD", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("CULTURE_YIELD", idx);

		//TAC Native Mercs
		SetGlobalDefine("UNITCLASS_NATIVE_MERC", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_NATIVE_MERC", idx);
		//End TAC Native Mercs

		// R&R, ray, Native Slave, START
		SetGlobalDefine("UNITCLASS_NATIVE_SLAVE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_NATIVE_SLAVE", idx);
		// R&R, ray, Native Slave, END

		// R&R, ray, African Slaves, START
		SetGlobalDefine("UNITCLASS_AFRICAN_SLAVE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_AFRICAN_SLAVE", idx);
		// R&R, ray, African Slaves, END

		// R&R, ray, Prisons Crowded - START
		SetGlobalDefine("UNITCLASS_PRISONER", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_PRISONER", idx);
		// R&R, ray, Prisons Crowded - END

		// R&R, ray, Revolutionary Noble - START
		SetGlobalDefine("UNITCLASS_NOBLE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_NOBLE", idx);
		// R&R, ray, Revolutionary Noble - END

		// R&R, ray, Bishop - START
		SetGlobalDefine("UNITCLASS_BISHOP", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_BISHOP", idx);
		// R&R, ray, Bishop - END

		// R&R, ray, the Church - START
		SetGlobalDefine("UNITCLASS_CHURCH_REWARD1", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_CHURCH_REWARD1", idx);

		SetGlobalDefine("UNITCLASS_CHURCH_REWARD2", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_CHURCH_REWARD2", idx);
		// R&R, ray, the Church - END

		// R&R, ray, Smuggling - START
		SetGlobalDefine("UNITCLASS_SMUGGLING_SHIP", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_SMUGGLING_SHIP", idx);
		// R&R, ray, Smuggling - END

		// R&R, ray, Rangers - START
		SetGlobalDefine("UNITCLASS_RANGER", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_RANGER", idx);
		// R&R, ray, Rangers - END

		// R&R, ray, Conquistadors - START
		SetGlobalDefine("UNITCLASS_CONQUISTADOR", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_CONQUISTADOR", idx);

		SetGlobalDefine("UNITCLASS_MOUNTED_CONQUISTADOR", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_MOUNTED_CONQUISTADOR", idx);
		// R&R, ray, Conquistadors - END

		// R&R, ray, Pirates - START
		SetGlobalDefine("UNITCLASS_PIRATE_FRIGATE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_PIRATE_FRIGATE", idx);
		// R&R, ray, Pirates - END

		//TAC Revolution Support
		SetGlobalDefine("UNITCLASS_REV_SUPPORT_LAND", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_REV_SUPPORT_LAND", idx);

		SetGlobalDefine("UNITCLASS_REV_SUPPORT_SEA", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_REV_SUPPORT_SEA", idx);
		//End TAC Revolution Support

		//TAC European Wars
		SetGlobalDefine("UNITCLASS_KING_REINFORCEMENT_LAND", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_KING_REINFORCEMENT_LAND", idx);

		SetGlobalDefine("UNITCLASS_KING_REINFORCEMENT_ARTIL", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_KING_REINFORCEMENT_ARTIL", idx);

		SetGlobalDefine("UNITCLASS_KING_REINFORCEMENT_SEA", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_KING_REINFORCEMENT_SEA", idx);
		//End TAC European Wars

		// R&R, ray, European Peace - START
		SetGlobalDefine("UNITCLASS_DIPLOMAT_1", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_DIPLOMAT_1", idx);

		SetGlobalDefine("UNITCLASS_DIPLOMAT_2", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_DIPLOMAT_2", idx);
		// R&R, ray, European Peace - END
		
		//TAC Whaling, ray
		SetGlobalDefine("UNITCLASS_WHALING_BOAT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_WHALING_BOAT", idx);
		GC.m_UNITCLASS_WHALING_BOAT = idx;// R&R, ray, write this in CvGlobals because faster

		SetGlobalDefine("PROFESSION_WHALING_BOAT_WORKING", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("PROFESSION_WHALING_BOAT_WORKING", idx);
		GC.m_PROFESSION_WHALING_BOAT_WORKING = idx; // R&R, ray, write this in CvGlobals because faster

		SetGlobalDefine("BONUS_WHALE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BONUS_WHALE", idx);
		//End TAC Whaling, ray

		// R&R, ray, High Sea Fishing - START
		SetGlobalDefine("UNITCLASS_FISHING_BOAT", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_FISHING_BOAT", idx);
		GC.m_UNITCLASS_FISHING_BOAT = idx;// R&R, ray, write this in CvGlobals because faster

		SetGlobalDefine("PROFESSION_FISHING_BOAT_WORKING", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("PROFESSION_FISHING_BOAT_WORKING", idx);
		GC.m_PROFESSION_FISHING_BOAT_WORKING = idx; // R&R, ray, write this in CvGlobals because faster

		SetGlobalDefine("BONUS_HIGH_SEA_FISH", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BONUS_HIGH_SEA_FISH", idx);
		// R&R, ray, High Sea Fishing - END

		// R&R, ray, adjustment Domestic Markets - START
		SetGlobalDefine("SPECIALBUILDING_MARKET", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("SPECIALBUILDING_MARKET", idx);
		// R&R, ray, adjustment Domestic Markets - END

		// R&R, ray, Entertainment Buildings - START
		SetGlobalDefine("SPECIALBUILDING_TAVERN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("SPECIALBUILDING_TAVERN", idx);
		// R&R, ray, Entertainment Buildings - END

		// R&R, ray, Diplo-Events with other Kings - START
		SetGlobalDefine("UNITCLASS_CONTINENTAL_GUARD", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_CONTINENTAL_GUARD", idx);

		SetGlobalDefine("BUILDINGCLASS_PALACE", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BUILDINGCLASS_PALACE", idx);

		SetGlobalDefine("UNITCLASS_MORTAR", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_MORTAR", idx);
		// R&R, ray, Diplo-Events with other Kings - END

		// R&R, ray, Militia or Unrest - START
		SetGlobalDefine("UNITCLASS_MILITIA", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_MILITIA", idx);
		// R&R, ray, Militia or Unrest - END

		// R&R, ray, Veteran Unit used for upcoming features - START
		SetGlobalDefine("UNITCLASS_VETERAN", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_VETERAN", idx);
		// R&R, ray, Veteran Unit used for upcoming features - END

		// R&R, ray, Great Admirals - START
		SetGlobalDefine("UNITCLASS_GREAT_GENERAL", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_GREAT_GENERAL", idx);

		SetGlobalDefine("UNITCLASS_GREAT_ADMIRAL", szVal);
		idx = FindInInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("UNITCLASS_GREAT_ADMIRAL", idx);
		// R&R, ray, Great Admirals - END

		/// one/two city plot radius
#ifndef ONE_PLOT_CITY_RADIUS
		GC.setDefineINT("MIN_CITY_RANGE", GC.getDefineINT("MIN_CITY_RANGE_TWO_PLOT"));
		GC.setDefineFLOAT("CAMERA_CITY_ZOOM_IN_DISTANCE", GC.getDefineFLOAT("CAMERA_CITY_ZOOM_IN_DISTANCE_TWO_PLOT"));
#endif
		// city radius end

		SetGlobalDefine("WATER_UNIT_FACING_DIRECTION", szVal);
		bool bFound = false;
		for(int iDirection=0; iDirection < NUM_DIRECTION_TYPES; ++iDirection)
		{
			CvWString szDirectionString;
			getDirectionTypeString(szDirectionString, (DirectionTypes) iDirection);
			if (szDirectionString == CvWString(szVal))
			{
				GC.getDefinesVarSystem()->SetValue("WATER_UNIT_FACING_DIRECTION", iDirection);
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			FAssertMsg(false, "Could not match direction string.");
			GC.getDefinesVarSystem()->SetValue("WATER_UNIT_FACING_DIRECTION", DIRECTION_SOUTH);
		}

		return true;
	}

	char	szMessage[1024];
	sprintf( szMessage, "Size of Global Defines is not greater than 0. \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
	gDLL->MessageBox(szMessage, "XML Load Error");

	return false;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalTypes()
//
//  PURPOSE :   Initialize the variables located in globaltypes.cpp/h with the values in
//				GlobalTypes.xml
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetGlobalTypes()
{
	UpdateProgressCB("GlobalTypes");

	bool bLoaded = false;	// used to make sure that the xml file was loaded correctly
	if (!CreateFXml())
	{
		return false;
	}

	// load the new FXml variable with the GlobalTypes.xml file
	bLoaded = LoadCivXml(m_pFXml, "xml/GlobalTypes.xml");
	if (!bLoaded)
	{
		char	szMessage[1024];
		sprintf( szMessage, "LoadXML call failed for GlobalTypes.xml. \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Load Error");
	}

	// if the load succeeded we will continue
	if (bLoaded)
	{
		// if the xml is successfully validated
		if (Validate())
		{
			SetGlobalStringArray(&GC.getAnimationOperatorTypes(), "Civ4Types/AnimationOperatorTypes/AnimationOperatorType", &GC.getNumAnimationOperatorTypes());
			int iEnumVal = NUM_FUNC_TYPES;
			SetGlobalStringArray(&GC.getFunctionTypes(), "Civ4Types/FunctionTypes/FunctionType", &iEnumVal, true);
			SetGlobalStringArray(&GC.getArtStyleTypes(), "Civ4Types/ArtStyleTypes/ArtStyleType", &GC.getNumArtStyleTypes());
			SetGlobalStringArray(&GC.getCitySizeTypes(), "Civ4Types/CitySizeTypes/CitySizeType", &GC.getNumCitySizeTypes());
			iEnumVal = NUM_CONTACT_TYPES;
			SetGlobalStringArray(&GC.getContactTypes(), "Civ4Types/ContactTypes/ContactType", &iEnumVal, true);
			iEnumVal = NUM_DIPLOMACYPOWER_TYPES;
			SetGlobalStringArray(&GC.getDiplomacyPowerTypes(), "Civ4Types/DiplomacyPowerTypes/DiplomacyPowerType", &iEnumVal, true);
			iEnumVal = NUM_AUTOMATE_TYPES;
			SetGlobalStringArray(&GC.getAutomateTypes(), "Civ4Types/AutomateTypes/AutomateType", &iEnumVal, true);
			iEnumVal = NUM_DIRECTION_TYPES;
			SetGlobalStringArray(&GC.getDirectionTypes(), "Civ4Types/DirectionTypes/DirectionType", &iEnumVal, true);
			SetGlobalStringArray(&GC.getFootstepAudioTypes(), "Civ4Types/FootstepAudioTypes/FootstepAudioType", &GC.getNumFootstepAudioTypes());

			gDLL->getXMLIFace()->SetToParent(m_pFXml);
			gDLL->getXMLIFace()->SetToParent(m_pFXml);
			SetVariableListTagPair<CvString>(&GC.getFootstepAudioTags(), "FootstepAudioTags", GC.getNumFootstepAudioTypes(), "");
		}
	}

	// delete the pointer to the FXml variable
	DestroyFXml();

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetDiplomacyCommentTypes()
//
//  PURPOSE :   Creates a full list of Diplomacy Comments
//
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetDiplomacyCommentTypes(CvString** ppszString, int* iNumVals)
{
	FAssertMsg(false, "should never get here");
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetupGlobalLandscapeInfos()
//
//  PURPOSE :   Initialize the appropriate variables located in globals.cpp/h with the values in
//				Terrain\Civ4TerrainSettings.xml
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetupGlobalLandscapeInfo()
{
	// load order: 6
	if (!CreateFXml())
	{
		return false;
	}

	/*
	LoadGlobalClassInfo(GC.getLandscapeInfo(), "CIV4TerrainSettings", "Terrain", "Civ4TerrainSettings/LandscapeInfos/LandscapeInfo", NULL);
	*/

	// delete the pointer to the FXml variable
	DestroyFXml();

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalArtDefines()
//
//  PURPOSE :   Initialize the appropriate variables located in globals.cpp/h with the values in
//				Civ4ArtDefines.xml
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetGlobalArtDefines()
{
	// load order: 3
	if (!CreateFXml())
	{
		return false;
	}

	LoadGlobalClassInfo(ARTFILEMGR.getInterfaceArtInfo(), "CIV4ArtDefines_Interface", "Art", "Civ4ArtDefines/InterfaceArtInfos/InterfaceArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getMovieArtInfo(), "CIV4ArtDefines_Movie", "Art", "Civ4ArtDefines/MovieArtInfos/MovieArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getMiscArtInfo(), "CIV4ArtDefines_Misc", "Art", "Civ4ArtDefines/MiscArtInfos/MiscArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getUnitArtInfo(), "CIV4ArtDefines_Unit", "Art", "Civ4ArtDefines/UnitArtInfos/UnitArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getBuildingArtInfo(), "CIV4ArtDefines_Building", "Art", "Civ4ArtDefines/BuildingArtInfos/BuildingArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getCivilizationArtInfo(), "CIV4ArtDefines_Civilization", "Art", "Civ4ArtDefines/CivilizationArtInfos/CivilizationArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getLeaderheadArtInfo(), "CIV4ArtDefines_Leaderhead", "Art", "Civ4ArtDefines/LeaderheadArtInfos/LeaderheadArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getBonusArtInfo(), "CIV4ArtDefines_Bonus", "Art", "Civ4ArtDefines/BonusArtInfos/BonusArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getImprovementArtInfo(), "CIV4ArtDefines_Improvement", "Art", "Civ4ArtDefines/ImprovementArtInfos/ImprovementArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getTerrainArtInfo(), "CIV4ArtDefines_Terrain", "Art", "Civ4ArtDefines/TerrainArtInfos/TerrainArtInfo", NULL);
	LoadGlobalClassInfo(ARTFILEMGR.getFeatureArtInfo(), "CIV4ArtDefines_Feature", "Art", "Civ4ArtDefines/FeatureArtInfos/FeatureArtInfo", NULL);

	DestroyFXml();

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalText()
//
//  PURPOSE :   Handles all Global Text Infos
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::LoadGlobalText()
{
	CvCacheObject* cache = gDLL->createGlobalTextCacheObject("GlobalText.dat");	// cache file name
	if (!gDLL->cacheRead(cache))
	{
		bool bLoaded = false;

		if (!CreateFXml())
		{
			return false;
		}

		//
		// load all files in the xml text directory
		//
		std::vector<CvString> aszFiles;
		std::vector<CvString> aszModfiles;

		gDLL->enumerateFiles(aszFiles, "xml\\text\\*.xml");

		if (gDLL->isModularXMLLoading())
		{
			gDLL->enumerateFiles(aszModfiles, "modules\\*_CIV4GameText.xml");
			aszFiles.insert(aszFiles.end(), aszModfiles.begin(), aszModfiles.end());
		}

		for(std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
		{
			bLoaded = LoadCivXml(m_pFXml, *it); // Load the XML
			if (!bLoaded)
			{
				char	szMessage[1024];
				sprintf( szMessage, "LoadXML call failed for %s. \n Current XML file is: %s", (*it).c_str(), GC.getCurrentXMLFile().GetCString());
				gDLL->MessageBox(szMessage, "XML Load Error");
			}
			if (bLoaded)
			{
				// if the xml is successfully validated
				if (Validate())
				{
					SetGameText("Civ4GameText", "Civ4GameText/TEXT");
				}
			}
		}

		DestroyFXml();

		// write global text info to cache
		bool bOk = gDLL->cacheWrite(cache);
		if (!bLoaded)
		{
			char	szMessage[1024];
			sprintf( szMessage, "Failed writing to Global Text cache. \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
			gDLL->MessageBox(szMessage, "XML Caching Error");
		}
		if (bOk)
		{
			logMsg("Wrote GlobalText to cache");
		}
	}	// didn't read from cache
	else
	{
		logMsg("Read GlobalText from cache");
	}

	gDLL->destroyCache(cache);

	return true;
}

bool CvXMLLoadUtility::LoadBasicInfos()
{
	// load order: 4
	/*
	if (!CreateFXml())
	{
		return false;
	}

	LoadGlobalClassInfo(GC.getConceptInfo(), "CIV4BasicInfos", "BasicInfos", "Civ4BasicInfos/ConceptInfos/ConceptInfo", NULL);
	LoadGlobalClassInfo(GC.getCalendarInfo(), "CIV4CalendarInfos", "BasicInfos", "Civ4CalendarInfos/CalendarInfos/CalendarInfo", NULL);
	LoadGlobalClassInfo(GC.getSeasonInfo(), "CIV4SeasonInfos", "BasicInfos", "Civ4SeasonInfos/SeasonInfos/SeasonInfo", NULL);
	LoadGlobalClassInfo(GC.getMonthInfo(), "CIV4MonthInfos", "BasicInfos", "Civ4MonthInfos/MonthInfos/MonthInfo", NULL);
	LoadGlobalClassInfo(GC.getDenialInfo(), "CIV4DenialInfos", "BasicInfos", "Civ4DenialInfos/DenialInfos/DenialInfo", NULL);
	LoadGlobalClassInfo(GC.getInvisibleInfo(), "CIV4InvisibleInfos", "BasicInfos", "Civ4InvisibleInfos/InvisibleInfos/InvisibleInfo", NULL);
	LoadGlobalClassInfo(GC.getUnitCombatInfo(), "CIV4UnitCombatInfos", "BasicInfos", "Civ4UnitCombatInfos/UnitCombatInfos/UnitCombatInfo", NULL);
	LoadGlobalClassInfo(GC.getDomainInfo(), "CIV4DomainInfos", "BasicInfos", "Civ4DomainInfos/DomainInfos/DomainInfo", NULL);
	LoadGlobalClassInfo(GC.getUnitAIInfo(), "CIV4UnitAIInfos", "BasicInfos", "Civ4UnitAIInfos/UnitAIInfos/UnitAIInfo", NULL);
	LoadGlobalClassInfo(GC.getAttitudeInfo(), "CIV4AttitudeInfos", "BasicInfos", "Civ4AttitudeInfos/AttitudeInfos/AttitudeInfo", NULL);
	LoadGlobalClassInfo(GC.getMemoryInfo(), "CIV4MemoryInfos", "BasicInfos", "Civ4MemoryInfos/MemoryInfos/MemoryInfo", NULL);
	LoadGlobalClassInfo(GC.getFatherCategoryInfo(), "CIV4FatherCategoryInfos", "BasicInfos", "Civ4FatherCategoryInfos/FatherCategoryInfos/FatherCategoryInfo", NULL);

	DestroyFXml();
	*/
	return true;
}

//
// Globals which must be loaded before the main menus.
// Don't put anything in here unless it has to be loaded before the main menus,
// instead try to load things in LoadPostMenuGlobals()
//
bool CvXMLLoadUtility::LoadPreMenuGlobals()
{
	// load order: 5
	if (!CreateFXml())
	{
		return false;
	}

	/// XML type preloading - start - Nightinggale
	readXMLfiles(false);
	/// XML type preloading - end - Nightinggale

	/// xml verification
#ifdef FASSERT_ENABLE
	verifyXMLsettings();
#endif
	/// xml verification

	/*
	LoadGlobalClassInfo(GC.getColorInfo(), "CIV4ColorVals", "Interface", "Civ4ColorVals/ColorVals/ColorVal", NULL);
	LoadGlobalClassInfo(GC.getUnitClassInfo(), "CIV4UnitClassInfos", "Units", "Civ4UnitClassInfos/UnitClassInfos/UnitClassInfo", NULL);
	LoadGlobalClassInfo(GC.getCultureLevelInfo(), "CIV4CultureLevelInfo", "GameInfo", "Civ4CultureLevelInfo/CultureLevelInfos/CultureLevelInfo", NULL);
	LoadGlobalClassInfo(GC.getVictoryInfo(), "CIV4VictoryInfo", "GameInfo", "Civ4VictoryInfo/VictoryInfos/VictoryInfo", NULL);
	LoadGlobalClassInfo(GC.getBuildingClassInfo(), "CIV4BuildingClassInfos", "Buildings", "Civ4BuildingClassInfos/BuildingClassInfos/BuildingClassInfo", NULL);
	LoadGlobalClassInfo(GC.getYieldInfo(), "CIV4YieldInfos", "Terrain", "Civ4YieldInfos/YieldInfos/YieldInfo", NULL);
	LoadGlobalClassInfo(GC.getAlarmInfo(), "CIV4AlarmInfos", "Civilizations", "Civ4AlarmInfos/AlarmInfos/AlarmInfo", NULL);
	LoadGlobalClassInfo(GC.getGameSpeedInfo(), "CIV4GameSpeedInfo", "GameInfo", "Civ4GameSpeedInfo/GameSpeedInfos/GameSpeedInfo", NULL);
	LoadGlobalClassInfo(GC.getTurnTimerInfo(), "CIV4TurnTimerInfo", "GameInfo", "Civ4TurnTimerInfo/TurnTimerInfos/TurnTimerInfo", NULL);
	LoadGlobalClassInfo(GC.getWorldInfo(), "CIV4WorldInfo", "GameInfo", "Civ4WorldInfo/WorldInfos/WorldInfo", NULL);
	LoadGlobalClassInfo(GC.getClimateInfo(), "CIV4ClimateInfo", "GameInfo", "Civ4ClimateInfo/ClimateInfos/ClimateInfo", NULL);
	LoadGlobalClassInfo(GC.getSeaLevelInfo(), "CIV4SeaLevelInfo", "GameInfo", "Civ4SeaLevelInfo/SeaLevelInfos/SeaLevelInfo", NULL);
	LoadGlobalClassInfo(GC.getEuropeInfo(), "CIV4EuropeInfo", "GameInfo", "Civ4EuropeInfo/EuropeInfos/EuropeInfo", NULL);
	LoadGlobalClassInfo(GC.getTerrainInfo(), "CIV4TerrainInfos", "Terrain", "Civ4TerrainInfos/TerrainInfos/TerrainInfo", NULL);
	LoadGlobalClassInfo(GC.getEraInfo(), "CIV4EraInfos", "GameInfo", "Civ4EraInfos/EraInfos/EraInfo", NULL);
	LoadGlobalClassInfo(GC.getFeatureInfo(), "Civ4FeatureInfos", "Terrain", "Civ4FeatureInfos/FeatureInfos/FeatureInfo", NULL);
	LoadGlobalClassInfo(GC.getPromotionInfo(), "CIV4PromotionInfos", "Units", "Civ4PromotionInfos/PromotionInfos/PromotionInfo", &CvDLLUtilityIFaceBase::createPromotionInfoCacheObject);
	LoadGlobalClassInfo(GC.getProfessionInfo(), "CIV4ProfessionInfos", "Units", "Civ4ProfessionInfos/ProfessionInfos/ProfessionInfo", &CvDLLUtilityIFaceBase::createProfessionInfoCacheObject);
	LoadGlobalClassInfo(GC.getGoodyInfo(), "CIV4GoodyInfo", "GameInfo", "Civ4GoodyInfo/GoodyInfos/GoodyInfo", NULL);
	LoadGlobalClassInfo(GC.getTraitInfo(), "CIV4TraitInfos", "Civilizations", "Civ4TraitInfos/TraitInfos/TraitInfo", &CvDLLUtilityIFaceBase::createTraitInfoCacheObject);
	LoadGlobalClassInfo(GC.getSpecialBuildingInfo(), "CIV4SpecialBuildingInfos", "Buildings", "Civ4SpecialBuildingInfos/SpecialBuildingInfos/SpecialBuildingInfo", NULL);
	for (int i=0; i < GC.getNumProfessionInfos(); ++i)
	{
		GC.getProfessionInfo((ProfessionTypes)i).readPass3();
	}
	LoadGlobalClassInfo(GC.getAnimationCategoryInfo(), "CIV4AnimationInfos", "Units", "Civ4AnimationInfos/AnimationCategories/AnimationCategory", NULL);
	LoadGlobalClassInfo(GC.getAnimationPathInfo(), "CIV4AnimationPathInfos", "Units", "Civ4AnimationPathInfos/AnimationPaths/AnimationPath", NULL);
	LoadGlobalClassInfo(GC.getHandicapInfo(), "CIV4HandicapInfo", "GameInfo", "Civ4HandicapInfo/HandicapInfos/HandicapInfo", &CvDLLUtilityIFaceBase::createHandicapInfoCacheObject);
	LoadGlobalClassInfo(GC.getCursorInfo(), "CIV4CursorInfo", "GameInfo", "Civ4CursorInfo/CursorInfos/CursorInfo", NULL);
	LoadGlobalClassInfo(GC.getCivicOptionInfo(), "CIV4CivicOptionInfos", "GameInfo", "Civ4CivicOptionInfos/CivicOptionInfos/CivicOptionInfo", NULL);
	LoadGlobalClassInfo(GC.getHurryInfo(), "CIV4HurryInfo", "GameInfo", "Civ4HurryInfo/HurryInfos/HurryInfo", NULL);
	LoadGlobalClassInfo(GC.getBuildingInfo(), "CIV4BuildingInfos", "Buildings", "Civ4BuildingInfos/BuildingInfos/BuildingInfo", &CvDLLUtilityIFaceBase::createBuildingInfoCacheObject);
	for (int i=0; i < GC.getNumBuildingClassInfos(); ++i)
	{
		GC.getBuildingClassInfo((BuildingClassTypes)i).readPass3();
	}
	LoadGlobalClassInfo(GC.getBonusInfo(), "CIV4BonusInfos", "Terrain", "Civ4BonusInfos/BonusInfos/BonusInfo", &CvDLLUtilityIFaceBase::createBonusInfoCacheObject);
	LoadGlobalClassInfo(GC.getRouteInfo(), "Civ4RouteInfos", "Misc", "Civ4RouteInfos/RouteInfos/RouteInfo", NULL);
	LoadGlobalClassInfo(GC.getImprovementInfo(), "CIV4ImprovementInfos", "Terrain", "Civ4ImprovementInfos/ImprovementInfos/ImprovementInfo", &CvDLLUtilityIFaceBase::createImprovementInfoCacheObject);
	LoadGlobalClassInfo(GC.getFatherPointInfo(), "CIV4FatherPointInfos", "GameInfo", "Civ4FatherPointInfos/FatherPointInfos/FatherPointInfo", NULL);
	LoadGlobalClassInfo(GC.getFatherInfo(), "CIV4FatherInfos", "GameInfo", "Civ4FatherInfos/FatherInfos/FatherInfo", &CvDLLUtilityIFaceBase::createFatherInfoCacheObject);
	LoadGlobalClassInfo(GC.getSpecialUnitInfo(), "CIV4SpecialUnitInfos", "Units", "Civ4SpecialUnitInfos/SpecialUnitInfos/SpecialUnitInfo", NULL);
	LoadGlobalClassInfo(GC.getCivicInfo(), "CIV4CivicInfos", "GameInfo", "Civ4CivicInfos/CivicInfos/CivicInfo", &CvDLLUtilityIFaceBase::createCivicInfoCacheObject);
	LoadGlobalClassInfo(GC.getLeaderHeadInfo(), "CIV4LeaderHeadInfos", "Civilizations", "Civ4LeaderHeadInfos/LeaderHeadInfos/LeaderHeadInfo", &CvDLLUtilityIFaceBase::createLeaderHeadInfoCacheObject);
	LoadGlobalClassInfo(GC.getPlayerColorInfo(), "CIV4PlayerColorInfos", "Interface", "Civ4PlayerColorInfos/PlayerColorInfos/PlayerColorInfo", NULL);
	LoadGlobalClassInfo(GC.getEffectInfo(), "CIV4EffectInfos", "Misc", "Civ4EffectInfos/EffectInfos/EffectInfo", NULL);
	LoadGlobalClassInfo(GC.getEntityEventInfo(), "CIV4EntityEventInfos", "Units", "Civ4EntityEventInfos/EntityEventInfos/EntityEventInfo", NULL);
	LoadGlobalClassInfo(GC.getBuildInfo(), "CIV4BuildInfos", "Units", "Civ4BuildInfos/BuildInfos/BuildInfo", NULL);
	LoadGlobalClassInfo(GC.getUnitInfo(), "CIV4UnitInfos", "Units", "Civ4UnitInfos/UnitInfos/UnitInfo", &CvDLLUtilityIFaceBase::createUnitInfoCacheObject);
	for (int i=0; i < GC.getNumUnitClassInfos(); ++i)
	{
		GC.getUnitClassInfo((UnitClassTypes)i).readPass3();
	}
	//Androrc UnitArtStyles
	LoadGlobalClassInfo(GC.getUnitArtStyleTypeInfo(), "CIV4UnitArtStyleTypeInfos", "Civilizations", "Civ4UnitArtStyleTypeInfos/UnitArtStyleTypeInfos/UnitArtStyleTypeInfo", false);
	//Androrc End
	LoadGlobalClassInfo(GC.getCivilizationInfo(), "CIV4CivilizationInfos", "Civilizations", "Civ4CivilizationInfos/CivilizationInfos/CivilizationInfo", &CvDLLUtilityIFaceBase::createCivilizationInfoCacheObject);
	LoadGlobalClassInfo(GC.getHints(), "CIV4Hints", "GameInfo", "Civ4Hints/HintInfos/HintInfo", NULL);
	LoadGlobalClassInfo(GC.getMainMenus(), "CIV4MainMenus", "Art", "Civ4MainMenus/MainMenus/MainMenu", NULL);
	LoadGlobalClassInfo(GC.getSlideShowInfo(), "CIV4SlideShowInfos", "Interface", "Civ4SlideShowInfos/SlideShowInfos/SlideShowInfo", NULL);
	LoadGlobalClassInfo(GC.getSlideShowRandomInfo(), "CIV4SlideShowRandomInfos", "Interface", "Civ4SlideShowRandomInfos/SlideShowRandomInfos/SlideShowRandomInfo", NULL);
	LoadGlobalClassInfo(GC.getWorldPickerInfo(), "CIV4WorldPickerInfos", "Interface", "Civ4WorldPickerInfos/WorldPickerInfos/WorldPickerInfo", NULL);

	LoadGlobalClassInfo(GC.getGameOptionInfo(), "CIV4GameOptionInfos", "GameInfo", "Civ4GameOptionInfos/GameOptionInfos/GameOptionInfo", NULL);
	LoadGlobalClassInfo(GC.getMPOptionInfo(), "CIV4MPOptionInfos", "GameInfo", "Civ4MPOptionInfos/MPOptionInfos/MPOptionInfo", NULL);
	LoadGlobalClassInfo(GC.getForceControlInfo(), "CIV4ForceControlInfos", "GameInfo", "Civ4ForceControlInfos/ForceControlInfos/ForceControlInfo", NULL);

	// PatchMod: Achievements START
	LoadGlobalClassInfo(GC.getAchieveInfo(), "CIV4AchieveInfos", "Events", "Civ4AchieveInfos/AchieveInfo", NULL);
	// PatchMod: Achievements END
	*/

	// add types to global var system
	for (int i = 0; i < GC.getNumCursorInfos(); ++i)
	{
		int iVal;
		CvString szType = GC.getCursorInfo((CursorTypes)i).getType();
		if (GC.getDefinesVarSystem()->GetValue(szType, iVal))
		{
			char szMessage[1024];
			sprintf(szMessage, "cursor type already set? \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
			gDLL->MessageBox(szMessage, "XML Error");
		}
		GC.getDefinesVarSystem()->SetValue(szType, i);
	}

	UpdateProgressCB("GlobalOther");

	DestroyFXml();

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   LoadPostMenuGlobals()
//
//  PURPOSE :   loads global xml data which isn't needed for the main menus
//		this data is loaded as a secodn stage, when the game is launched
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::LoadPostMenuGlobals()
{
	// load order: 7
	PROFILE_FUNC();
	if (!CreateFXml())
	{
		return false;
	}

	/*
	UpdateProgressCB("Global Events");

	LoadGlobalClassInfo(GC.getEventInfo(), "CIV4EventInfos", "Events", "Civ4EventInfos/EventInfos/EventInfo", &CvDLLUtilityIFaceBase::createEventInfoCacheObject);
	LoadGlobalClassInfo(GC.getEventTriggerInfo(), "CIV4EventTriggerInfos", "Events", "Civ4EventTriggerInfos/EventTriggerInfos/EventTriggerInfo", &CvDLLUtilityIFaceBase::createEventTriggerInfoCacheObject);

	UpdateProgressCB("Global Routes");

	LoadGlobalClassInfo(GC.getRouteModelInfo(), "Civ4RouteModelInfos", "Art", "Civ4RouteModelInfos/RouteModelInfos/RouteModelInfo", NULL);

	UpdateProgressCB("Global Rivers");

	LoadGlobalClassInfo(GC.getRiverModelInfo(), "CIV4RiverModelInfos", "Art", "Civ4RiverModelInfos/RiverModelInfos/RiverModelInfo", NULL);

	UpdateProgressCB("Global Other");

	LoadGlobalClassInfo(GC.getWaterPlaneInfo(), "CIV4WaterPlaneInfos", "Misc", "Civ4WaterPlaneInfos/WaterPlaneInfos/WaterPlaneInfo", NULL);
	LoadGlobalClassInfo(GC.getTerrainPlaneInfo(), "CIV4TerrainPlaneInfos", "Misc", "Civ4TerrainPlaneInfos/TerrainPlaneInfos/TerrainPlaneInfo", NULL);
	LoadGlobalClassInfo(GC.getCameraOverlayInfo(), "CIV4CameraOverlayInfos", "Misc", "Civ4CameraOverlayInfos/CameraOverlayInfos/CameraOverlayInfo", NULL);

	UpdateProgressCB("Global Emphasize");

	LoadGlobalClassInfo(GC.getEmphasizeInfo(), "CIV4EmphasizeInfo", "GameInfo", "Civ4EmphasizeInfo/EmphasizeInfos/EmphasizeInfo", NULL);

	UpdateProgressCB("Global Other");

	LoadGlobalClassInfo(GC.getMissionInfo(), "CIV4MissionInfos", "Units", "Civ4MissionInfos/MissionInfos/MissionInfo", NULL);
	LoadGlobalClassInfo(GC.getControlInfo(), "CIV4ControlInfos", "Units", "Civ4ControlInfos/ControlInfos/ControlInfo", NULL);
	LoadGlobalClassInfo(GC.getCommandInfo(), "CIV4CommandInfos", "Units", "Civ4CommandInfos/CommandInfos/CommandInfo", NULL);
	LoadGlobalClassInfo(GC.getAutomateInfo(), "CIV4AutomateInfos", "Units", "Civ4AutomateInfos/AutomateInfos/AutomateInfo", NULL);

	UpdateProgressCB("Global Interface");

	LoadGlobalClassInfo(GC.getInterfaceModeInfo(), "CIV4InterfaceModeInfos", "Interface", "Civ4InterfaceModeInfos/InterfaceModeInfos/InterfaceModeInfo", NULL);

	SetGlobalActionInfo();


	// Load the formation info
	LoadGlobalClassInfo(GC.getUnitFormationInfo(), "CIV4FormationInfos", "Units", "UnitFormations/UnitFormation", NULL);

	// Load the attachable infos
	LoadGlobalClassInfo(GC.getAttachableInfo(), "CIV4AttachableInfos", "Misc", "Civ4AttachableInfos/AttachableInfos/AttachableInfo", NULL);

	// Specail Case Diplomacy Info due to double vectored nature and appending of Responses
	LoadDiplomacyInfo(GC.getDiplomacyInfo(), "CIV4DiplomacyInfos", "GameInfo", "Civ4DiplomacyInfos/DiplomacyInfos/DiplomacyInfo", &CvDLLUtilityIFaceBase::createDiplomacyInfoCacheObject);
	*/

	DestroyFXml();
	return true;
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalStringArray(TCHAR (**ppszString)[256], char* szTagName, int* iNumVals)
//
//  PURPOSE :   takes the szTagName parameter and if it finds it in the m_pFXml member variable
//				then it loads the ppszString parameter with the string values under it and the
//				iNumVals with the total number of tags with the szTagName in the xml file
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGlobalStringArray(CvString **ppszString, char* szTagName, int* iNumVals, bool bUseEnum)
{
	PROFILE_FUNC();
	logMsg("SetGlobalStringArray %s\n", szTagName);

	int i=0;					//loop counter
	CvString *pszString;	// hold the local pointer to the newly allocated string memory
	pszString = NULL;			// null out the local string pointer so that it can be checked at the
	// end of the function in an FAssert

	// if we locate the szTagName, the current node is set to the first instance of the tag name in the xml file
	if (gDLL->getXMLIFace()->LocateNode(m_pFXml,szTagName))
	{
		if (!bUseEnum)
		{
			// get the total number of times this tag appears in the xml
			*iNumVals = gDLL->getXMLIFace()->NumOfElementsByTagName(m_pFXml,szTagName);
		}
		// initialize the memory based on the total number of tags in the xml and the 256 character length we selected
		*ppszString = new CvString[*iNumVals];
		// set the local pointer to the memory just allocated
		pszString = *ppszString;

		// loop through each of the tags
		for (i=0;i<*iNumVals;i++)
		{
			// get the string value at the current node
			GetXmlVal(pszString[i]);
			GC.setInfoTypeFromString(pszString[i], i);

			// if can't set the current node to a sibling node we will break out of the for loop
			// otherwise we will keep looping
			if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
			{
				break;
			}
		}
	}

	// if the local string pointer is null then we weren't able to find the szTagName in the xml
	// so we will FAssert to let whoever know it
	if (!pszString)
	{
		char	szMessage[1024];
		sprintf( szMessage, "Error locating tag node in SetGlobalStringArray function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
}




//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalActionInfo(CvActionInfo** ppActionInfo, int* iNumVals)
//
//  PURPOSE :   Takes the szTagName parameter and if it exists in the xml it loads the ppActionInfo
//				with the value under it and sets the value of the iNumVals parameter to the total number
//				of occurances of the szTagName tag in the xml.
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGlobalActionInfo()
{
	PROFILE_FUNC();
	logMsg("SetGlobalActionInfo\n");
	int i=0;					//loop counter

	if(!(NUM_INTERFACEMODE_TYPES > 0))
	{
		char	szMessage[1024];
		sprintf( szMessage, "NUM_INTERFACE_TYPES is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumBuildInfos() > 0))
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumBuildInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumPromotionInfos() > 0))
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumPromotionInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumUnitClassInfos() > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumUnitClassInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumBuildingInfos() > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumBuildingInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(NUM_CONTROL_TYPES > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "NUM_CONTROL_TYPES is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumAutomateInfos() > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumAutomateInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(NUM_COMMAND_TYPES > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "NUM_COMMAND_TYPES is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(NUM_MISSION_TYPES > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "NUM_MISSION_TYPES is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}

	int* piOrderedIndex=NULL;

	int iNumOrigVals = GC.getNumActionInfos();

	int iNumActionInfos = iNumOrigVals +
		NUM_INTERFACEMODE_TYPES +
		GC.getNumBuildInfos() +
		GC.getNumPromotionInfos() +
		GC.getNumUnitInfos() +
		NUM_CONTROL_TYPES +
		NUM_COMMAND_TYPES +
		GC.getNumAutomateInfos() +
		NUM_MISSION_TYPES;

	int* piIndexList = new int[iNumActionInfos];
	int* piPriorityList = new int[iNumActionInfos];
	int* piActionInfoTypeList = new int[iNumActionInfos];

	int iTotalActionInfoCount = 0;

	// loop through control info
	for (i=0;i<NUM_COMMAND_TYPES;i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getCommandInfo((CommandTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_COMMAND;
		iTotalActionInfoCount++;
	}

	for (i=0;i<NUM_INTERFACEMODE_TYPES;i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInterfaceModeInfo((InterfaceModeTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_INTERFACEMODE;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumBuildInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getBuildInfo((BuildTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_BUILD;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumPromotionInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getPromotionInfo((PromotionTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_PROMOTION;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumUnitInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getUnitInfo((UnitTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_UNIT;
		iTotalActionInfoCount++;
	}

	for (i=0;i<NUM_CONTROL_TYPES;i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getControlInfo((ControlTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_CONTROL;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumAutomateInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getAutomateInfo(i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_AUTOMATE;
		iTotalActionInfoCount++;
	}

	for (i=0;i<NUM_MISSION_TYPES;i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getMissionInfo((MissionTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_MISSION;
		iTotalActionInfoCount++;
	}

	SAFE_DELETE_ARRAY(piOrderedIndex);
	piOrderedIndex = new int[iNumActionInfos];

	orderHotkeyInfo(&piOrderedIndex, piPriorityList, iNumActionInfos);
	for (i=0;i<iNumActionInfos;i++)
	{
		CvActionInfo* pActionInfo = new CvActionInfo;
		pActionInfo->setOriginalIndex(piIndexList[piOrderedIndex[i]]);
		pActionInfo->setSubType((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]]);
		if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_COMMAND)
		{
			GC.getCommandInfo((CommandTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_INTERFACEMODE)
		{
			GC.getInterfaceModeInfo((InterfaceModeTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_BUILD)
		{
			GC.getBuildInfo((BuildTypes)piIndexList[piOrderedIndex[i]]).setMissionType(FindInInfoClass("MISSION_BUILD"));
			GC.getBuildInfo((BuildTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_PROMOTION)
		{
			GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setCommandType(FindInInfoClass("COMMAND_PROMOTION"));
			GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
			GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setHotKeyDescription(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getTextKeyWide(), GC.getCommandInfo((CommandTypes)(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getCommandType())).getTextKeyWide(), CreateHotKeyFromDescription(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getHotKey(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isShiftDown(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isAltDown(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isCtrlDown()));
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_UNIT)
		{
			GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).setCommandType(FindInInfoClass("COMMAND_UPGRADE"));
			GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
			GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).setHotKeyDescription(GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).getTextKeyWide(), GC.getCommandInfo((CommandTypes)(GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).getCommandType())).getTextKeyWide(), CreateHotKeyFromDescription(GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).getHotKey(), GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).isShiftDown(), GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).isAltDown(), GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).isCtrlDown()));
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_CONTROL)
		{
			GC.getControlInfo((ControlTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_AUTOMATE)
		{
			GC.getAutomateInfo(piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_MISSION)
		{
			GC.getMissionInfo((MissionTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i + iNumOrigVals);
		}

		GC.getActionInfo().push_back(pActionInfo);
	}
	GC.addToInfosVectors(&GC.getActionInfo());

	SAFE_DELETE_ARRAY(piOrderedIndex);
	SAFE_DELETE_ARRAY(piIndexList);
	SAFE_DELETE_ARRAY(piPriorityList);
	SAFE_DELETE_ARRAY(piActionInfoTypeList);
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalAnimationPathInfo(CvAnimationPathInfo** ppAnimationPathInfo, char* szTagName, int* iNumVals)
//
//  PURPOSE :   Takes the szTagName parameter and if it exists in the xml it loads the ppAnimationPathInfo
//				with the value under it and sets the value of the iNumVals parameter to the total number
//				of occurances of the szTagName tag in the xml.
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGlobalAnimationPathInfo(CvAnimationPathInfo** ppAnimationPathInfo, char* szTagName, int* iNumVals)
{
	PROFILE_FUNC();
	logMsg( "SetGlobalAnimationPathInfo %s\n", szTagName );

	int		i;						// Loop counters
	CvAnimationPathInfo * pAnimPathInfo = NULL;	// local pointer to the domain info memory

	if ( gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName ))
	{
		// get the number of times the szTagName tag appears in the xml file
		*iNumVals = gDLL->getXMLIFace()->NumOfElementsByTagName(m_pFXml,szTagName);

		// allocate memory for the domain info based on the number above
		*ppAnimationPathInfo = new CvAnimationPathInfo[*iNumVals];
		pAnimPathInfo = *ppAnimationPathInfo;

		gDLL->getXMLIFace()->SetToParent(m_pFXml);
		gDLL->getXMLIFace()->SetToChild(m_pFXml);
		gDLL->getXMLIFace()->SetToChild(m_pFXml);


		// Loop through each tag.
		for (i=0;i<*iNumVals;i++)
		{
			SkipToNextVal();	// skip to the next non-comment node

			if (!pAnimPathInfo[i].read(this))
				break;
			GC.setInfoTypeFromString(pAnimPathInfo[i].getType(), i);	// add type to global info type hash map
			if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
			{
				break;
			}
		}
	}

	// if we didn't find the tag name in the xml then we never set the local pointer to the
	// newly allocated memory and there for we will FAssert to let people know this most
	// interesting fact
	if(!pAnimPathInfo )
	{
		char	szMessage[1024];
		sprintf( szMessage, "Error finding tag node in SetGlobalAnimationPathInfo function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalUnitScales(float* pfLargeScale, float* pfSmallScale, char* szTagName)
//
//  PURPOSE :   Takes the szTagName parameter and if it exists in the xml it loads the ppPromotionInfo
//				with the value under it and sets the value of the iNumVals parameter to the total number
//				of occurances of the szTagName tag in the xml.
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGlobalUnitScales(float* pfLargeScale, float* pfSmallScale, char* szTagName)
{
	PROFILE_FUNC();
	logMsg("SetGlobalUnitScales %s\n", szTagName);
	// if we successfully locate the szTagName node
	if (gDLL->getXMLIFace()->LocateNode(m_pFXml,szTagName))
	{
		// call the function that sets the FXml pointer to the first non-comment child of
		// the current tag and gets the value of that new node
		if (GetChildXmlVal(pfLargeScale))
		{
			// set the current xml node to it's next sibling and then
			// get the sibling's TCHAR value
			GetNextXmlVal(pfSmallScale);

			// set the current xml node to it's parent node
			gDLL->getXMLIFace()->SetToParent(m_pFXml);
		}
	}
	else
	{
		// if we didn't find the tag name in the xml then we never set the local pointer to the
		// newly allocated memory and there for we will FAssert to let people know this most
		// interesting fact
		char	szMessage[1024];
		sprintf( szMessage, "Error finding tag node in SetGlobalUnitScales function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGameText()
//
//  PURPOSE :   Reads game text info from XML and adds it to the translation manager
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGameText(const char* szTextGroup, const char* szTagName)
{
	PROFILE_FUNC();
	logMsg("SetGameText %s\n", szTagName);
	int i=0;		//loop counter - Index into pTextInfo

	if (gDLL->getXMLIFace()->LocateNode(m_pFXml, szTextGroup)) // Get the Text Group 1st
	{
		int iNumVals = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);	// Get the number of Children that the Text Group has
		gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName); // Now switch to the TEXT Tag
		gDLL->getXMLIFace()->SetToParent(m_pFXml);
		gDLL->getXMLIFace()->SetToChild(m_pFXml);

		// loop through each tag
		for (i=0; i < iNumVals; i++)
		{
			CvGameText textInfo;
			textInfo.read(this);

			gDLL->addText(textInfo.getType() /*id*/, textInfo.getText(), textInfo.getGender(), textInfo.getPlural());
			if (!gDLL->getXMLIFace()->NextSibling(m_pFXml) && i!=iNumVals-1)
			{
				char	szMessage[1024];
				sprintf( szMessage, "failed to find sibling \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
				gDLL->MessageBox(szMessage, "XML Error");
				break;
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalClassInfo - This is a template function that is USED FOR ALMOST ALL INFO CLASSES.
//		Each info class should have a read(CvXMLLoadUtility*) function that is responsible for initializing
//		the class from xml data.
//
//  PURPOSE :   takes the szTagName parameter and loads the ppszString with the text values
//				under the tags.  This will be the hints displayed during game initialization and load
//
//------------------------------------------------------------------------------------------------------
template <class T>
void CvXMLLoadUtility::SetGlobalClassInfo(std::vector<T*>& aInfos, const char* szTagName)
{
	char szLog[256];
	sprintf(szLog, "SetGlobalClassInfo (%s)", szTagName);
	PROFILE(szLog);
	logMsg(szLog);

	// if we successfully locate the tag name in the xml file
	if (gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName))
	{
		// loop through each tag
		do
		{
			SkipToNextVal();	// skip to the next non-comment node

			T* pClassInfo = new T;

			FAssert(NULL != pClassInfo);
			if (NULL == pClassInfo)
			{
				break;
			}

			/// XML type preloading - start - Nightinggale
			//bool bSuccess = pClassInfo->read(this);
			bool bSuccess = false;
			if (bFirstRead)
			{
				bSuccess = pClassInfo->CvInfoBase::read(this);
			}
			else
			{
				bSuccess = pClassInfo->read(this);
			}
			/// XML type preloading - end - Nightinggale

			FAssert(bSuccess);
			if (!bSuccess)
			{
				delete pClassInfo;
				break;
			}

			int iIndex = -1;
			if (NULL != pClassInfo->getType())
			{
				iIndex = GC.getInfoTypeForString(pClassInfo->getType(), true);
			}

			if (-1 == iIndex)
			{
				aInfos.push_back(pClassInfo);
				if (NULL != pClassInfo->getType())
				{
					GC.setInfoTypeFromString(pClassInfo->getType(), (int)aInfos.size() - 1);	// add type to global info type hash map
				}
			}
			else
			{
				/// XML type preloading - start - Nightinggale
				FAssertMsg(!bFirstRead, CvString::format("%s is used multiple times", pClassInfo->getType()).c_str());
				/// XML type preloading - end - Nightinggale
				SAFE_DELETE(aInfos[iIndex]);
				aInfos[iIndex] = pClassInfo;
			}


		} while (gDLL->getXMLIFace()->NextSibling(m_pFXml));

		//readPass2
		if (!bFirstRead) /// XML type preloading - Nightinggale
		{
			PROFILE("CvXMLLoadUtility::SetGlobalClassInfo::readPass2");

			// if we successfully locate the szTagName node
			if (gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName))
			{
				gDLL->getXMLIFace()->SetToParent(m_pFXml);
				gDLL->getXMLIFace()->SetToChild(m_pFXml);

				// loop through each tag
				for (std::vector<T*>::iterator it = aInfos.begin(); it != aInfos.end(); ++it)
				{
					(*it)->readPass2(this);

					if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
					{
						break;
					}
				}
			}
		}
	}
}

void CvXMLLoadUtility::SetDiplomacyInfo(std::vector<CvDiplomacyInfo*>& DiploInfos, const char* szTagName)
{
	char szLog[256];
	sprintf(szLog, "SetDiplomacyInfo (%s)", szTagName);
	PROFILE(szLog);
	logMsg(szLog);

	// if we successfully locate the tag name in the xml file
	if (gDLL->getXMLIFace()->LocateNode(m_pFXml, szTagName))
	{
		// loop through each tag
		do
		{
			SkipToNextVal();	// skip to the next non-comment node

			CvString szType;
			GetChildXmlValByName(szType, "Type");
			int iIndex = GC.getInfoTypeForString(szType, true);

			if (-1 == iIndex)
			{
				CvDiplomacyInfo* pClassInfo = new CvDiplomacyInfo;

				if (NULL == pClassInfo)
				{
					FAssert(false);
					break;
				}

				pClassInfo->read(this);
				if (NULL != pClassInfo->getType())
				{
					GC.setInfoTypeFromString(pClassInfo->getType(), (int)DiploInfos.size());	// add type to global info type hash map
				}
				DiploInfos.push_back(pClassInfo);
			}
			else
			{
				DiploInfos[iIndex]->read(this);
			}

		} while (gDLL->getXMLIFace()->NextSibling(m_pFXml));
	}
}

template <class T>
void CvXMLLoadUtility::LoadGlobalClassInfo(std::vector<T*>& aInfos, const char* szFileRoot, const char* szFileDirectory, const char* szXmlPath, CvCacheObject* (CvDLLUtilityIFaceBase::*pArgFunction) (const TCHAR*))
{
	bool bLoaded = false;
	bool bWriteCache = true;
	CvCacheObject* pCache = NULL;
	GC.addToInfosVectors(&aInfos);

	if (NULL != pArgFunction)
	{
		pCache = (gDLL->*pArgFunction)(CvString::format("%s.dat", szFileRoot));	// cache file name

		if (gDLL->cacheRead(pCache, CvString::format("xml\\\\%s\\\\%s.xml", szFileDirectory, szFileRoot)))
		{
			logMsg("Read %s from cache", szFileDirectory);
			bLoaded = true;
			bWriteCache = false;
		}
	}

	if (!bLoaded)
	{
		bLoaded = LoadCivXml(m_pFXml, CvString::format("xml\\%s/%s.xml", szFileDirectory, szFileRoot));

		if (!bLoaded)
		{
			char szMessage[1024];
			sprintf(szMessage, "LoadXML call failed for %s.", CvString::format("%s/%s.xml", szFileDirectory, szFileRoot).GetCString());
			gDLL->MessageBox(szMessage, "XML Load Error");
		}
		else if (Validate())
		{
			SetGlobalClassInfo(aInfos, szXmlPath);

			if (gDLL->isModularXMLLoading())
			{
				std::vector<CvString> aszFiles;
				gDLL->enumerateFiles(aszFiles, CvString::format("modules\\*_%s.xml", szFileRoot));  // search for the modular files

				for (std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
				{
					bLoaded = LoadCivXml(m_pFXml, *it);

					if (!bLoaded)
					{
						char szMessage[1024];
						sprintf(szMessage, "LoadXML call failed for %s.", (*it).GetCString());
						gDLL->MessageBox(szMessage, "XML Load Error");
					}
					else if (Validate())
					{
						SetGlobalClassInfo(aInfos, szXmlPath);
					}
				}
			}

			if (NULL != pArgFunction && bWriteCache)
			{
				// write info to cache
				bool bOk = gDLL->cacheWrite(pCache);
				if (!bOk)
				{
					char szMessage[1024];
					sprintf(szMessage, "Failed writing to %s cache. \n Current XML file is: %s", szFileDirectory, GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Caching Error");
				}
				if (bOk)
				{
					logMsg("Wrote %s to cache", szFileDirectory);
				}
			}
		}
	}

	if (NULL != pArgFunction)
	{
		gDLL->destroyCache(pCache);
	}
}


void CvXMLLoadUtility::LoadDiplomacyInfo(std::vector<CvDiplomacyInfo*>& DiploInfos, const char* szFileRoot, const char* szFileDirectory, const char* szXmlPath, CvCacheObject* (CvDLLUtilityIFaceBase::*pArgFunction) (const TCHAR*))
{
	bool bLoaded = false;
	bool bWriteCache = true;
	CvCacheObject* pCache = NULL;
	GC.addToInfosVectors(&DiploInfos);

	if (NULL != pArgFunction)
	{
		pCache = (gDLL->*pArgFunction)(CvString::format("%s.dat", szFileRoot));	// cache file name

		if (gDLL->cacheRead(pCache, CvString::format("xml\\\\%s\\\\%s.xml", szFileDirectory, szFileRoot)))
		{
			logMsg("Read %s from cache", szFileDirectory);
			bLoaded = true;
			bWriteCache = false;
		}
	}

	if (!bLoaded)
	{
		bLoaded = LoadCivXml(m_pFXml, CvString::format("xml\\%s/%s.xml", szFileDirectory, szFileRoot));

		if (!bLoaded)
		{
			char szMessage[1024];
			sprintf(szMessage, "LoadXML call failed for %s.", CvString::format("%s/%s.xml", szFileDirectory, szFileRoot).GetCString());
			gDLL->MessageBox(szMessage, "XML Load Error");
		}
		else if (Validate())
		{
			SetDiplomacyInfo(DiploInfos, szXmlPath);

			if (gDLL->isModularXMLLoading())
			{
				std::vector<CvString> aszFiles;
				gDLL->enumerateFiles(aszFiles, CvString::format("modules\\*_%s.xml", szFileRoot));  // search for the modular files

				for (std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
				{
					bLoaded = LoadCivXml(m_pFXml, *it);

					if (!bLoaded)
					{
						char szMessage[1024];
						sprintf(szMessage, "LoadXML call failed for %s.", (*it).GetCString());
						gDLL->MessageBox(szMessage, "XML Load Error");
					}
					else if (Validate())
					{
						SetDiplomacyInfo(DiploInfos, szXmlPath);
					}
				}
			}

			if (NULL != pArgFunction && bWriteCache)
			{
				// write info to cache
				bool bOk = gDLL->cacheWrite(pCache);
				if (!bOk)
				{
					char szMessage[1024];
					sprintf(szMessage, "Failed writing to %s cache. \n Current XML file is: %s", szFileDirectory, GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Caching Error");
				}
				if (bOk)
				{
					logMsg("Wrote %s to cache", szFileDirectory);
				}
			}
		}
	}

	if (NULL != pArgFunction)
	{
		gDLL->destroyCache(pCache);
	}
}

//
// helper sort predicate
//

struct OrderIndex {int m_iPriority; int m_iIndex;};
bool sortHotkeyPriority(const OrderIndex orderIndex1, const OrderIndex orderIndex2)
{
	return (orderIndex1.m_iPriority > orderIndex2.m_iPriority);
}

template <class T>
void CvXMLLoadUtility::orderHotkeyInfo(int** ppiSortedIndex, T* pHotkeyInfos, int iLength)
{
	int iI;
	int* piSortedIndex;
	std::vector<OrderIndex> viOrderPriority;

	viOrderPriority.resize(iLength);
	piSortedIndex = *ppiSortedIndex;

	// set up vector
	for(iI=0;iI<iLength;iI++)
	{
		viOrderPriority[iI].m_iPriority = pHotkeyInfos[iI].getOrderPriority();
		viOrderPriority[iI].m_iIndex = iI;
	}

	// sort the array
	std::sort(viOrderPriority.begin(), viOrderPriority.end(), sortHotkeyPriority);

	// insert new order into the array to return
	for (iI=0;iI<iLength;iI++)
	{
		piSortedIndex[iI] = viOrderPriority[iI].m_iIndex;
	}
}

void CvXMLLoadUtility::orderHotkeyInfo(int** ppiSortedIndex, int* pHotkeyIndex, int iLength)
{
	int iI;
	int* piSortedIndex;
	std::vector<OrderIndex> viOrderPriority;

	viOrderPriority.resize(iLength);
	piSortedIndex = *ppiSortedIndex;

	// set up vector
	for(iI=0;iI<iLength;iI++)
	{
		viOrderPriority[iI].m_iPriority = pHotkeyIndex[iI];
		viOrderPriority[iI].m_iIndex = iI;
	}

	// sort the array
	std::sort(viOrderPriority.begin(), viOrderPriority.end(), sortHotkeyPriority);

	// insert new order into the array to return
	for (iI=0;iI<iLength;iI++)
	{
		piSortedIndex[iI] = viOrderPriority[iI].m_iIndex;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetFeatureStruct(int** ppiFeatureTime, int*** ppiFeatureYield, bool** ppbFeatureRemove)
//
//  PURPOSE :   allocate and set the feature struct variables for the CvBuildInfo class
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetFeatureStruct(int** ppiFeatureTime, std::vector<std::vector<int> >& aaiFeatureYield, bool** ppbFeatureRemove)
{
	int iNumSibs;					// the number of siblings the current xml node has
	int iFeatureIndex;
	TCHAR szTextVal[256];	// temporarily hold the text value of the current xml node
	int* paiFeatureTime = NULL;
	bool* pabFeatureRemove = NULL;

	if(GC.getNumFeatureInfos() < 1)
	{
		char	szMessage[1024];
		sprintf( szMessage, "no feature infos set yet! \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	InitList(ppiFeatureTime, GC.getNumFeatureInfos(), 0);
	aaiFeatureYield.resize(GC.getNumFeatureInfos());
	for (int i = 0; i < GC.getNumFeatureInfos(); ++i)
	{
		aaiFeatureYield[i].resize(NUM_YIELD_TYPES, 0);
	}
	InitList(ppbFeatureRemove, GC.getNumFeatureInfos(), false);
	paiFeatureTime = *ppiFeatureTime;
	pabFeatureRemove = *ppbFeatureRemove;

	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,"FeatureStructs"))
	{
		iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);

		if (0 < iNumSibs)
		{
			if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,"FeatureStruct"))
			{
				if(!(iNumSibs <= GC.getNumFeatureInfos()))
				{
					char	szMessage[1024];
					sprintf( szMessage, "iNumSibs is greater than GC.getNumFeatureInfos in SetFeatureStruct function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Error");
				}
				for (i=0;i<iNumSibs;i++)
				{
					GetChildXmlValByName(szTextVal, "FeatureType");
					iFeatureIndex = FindInInfoClass(szTextVal);
					if(!(iFeatureIndex != -1))
					{
						char	szMessage[1024];
						sprintf( szMessage, "iFeatureIndex is -1 inside SetFeatureStruct function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
						gDLL->MessageBox(szMessage, "XML Error");
					}
					GetChildXmlValByName(&paiFeatureTime[iFeatureIndex], "iTime");
					int* aiFeatureYield;
					SetVariableListTagPair(&aiFeatureYield, "Yields", NUM_YIELD_TYPES, 0);
					for (int j = 0; j < NUM_YIELD_TYPES; ++j)
					{
						aaiFeatureYield[iFeatureIndex][j] = aiFeatureYield[j];
					}
					SAFE_DELETE_ARRAY(aiFeatureYield);
					GetChildXmlValByName(&pabFeatureRemove[iFeatureIndex], "bRemove");

					if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
					{
						break;
					}
				}

				gDLL->getXMLIFace()->SetToParent(m_pFXml);
			}
		}

		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetImprovementBonuses(CvImprovementBonusInfo** ppImprovementBonus)
//
//  PURPOSE :   Allocate memory for the improvement bonus pointer and fill it based on the
//				values in the xml.
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetImprovementBonuses(CvImprovementBonusInfo** ppImprovementBonus)
{
	int i=0;				//loop counter
	int iNumSibs;			// the number of siblings the current xml node has
	TCHAR szNodeVal[256];	// temporarily holds the string value of the current xml node
	CvImprovementBonusInfo* paImprovementBonus;	// local pointer to the bonus type struct in memory

	// Skip any comments and stop at the next value we might want
	if (SkipToNextVal())
	{
		// initialize the boolean list to the correct size and all the booleans to false
		InitImprovementBonusList(ppImprovementBonus, GC.getNumBonusInfos());
		// set the local pointer to the memory we just allocated
		paImprovementBonus = *ppImprovementBonus;

		// get the total number of children the current xml node has
		iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
		// if we can set the current xml node to the child of the one it is at now
		if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
		{
			if(!(iNumSibs <= GC.getNumBonusInfos()))
			{
				char	szMessage[1024];
				sprintf( szMessage, "For loop iterator is greater than array size \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
				gDLL->MessageBox(szMessage, "XML Error");
			}
			// loop through all the siblings
			for (i=0;i<iNumSibs;i++)
			{
				// skip to the next non-comment node
				if (SkipToNextVal())
				{
					// call the function that sets the FXml pointer to the first non-comment child of
					// the current tag and gets the value of that new node
					if (GetChildXmlVal(szNodeVal))
					{
						int iBonusIndex;	// index of the match in the bonus types list
						// call the find in list function to return either -1 if no value is found
						// or the index in the list the match is found at
						iBonusIndex = FindInInfoClass(szNodeVal);
						// if we found a match we will get the next sibling's boolean value at that match's index
						if (iBonusIndex >= 0)
						{
							GetNextXmlVal(&paImprovementBonus[iBonusIndex].m_bBonusMakesValid);
							GetNextXmlVal(&paImprovementBonus[iBonusIndex].m_iDiscoverRand);
							gDLL->getXMLIFace()->SetToParent(m_pFXml);

							SAFE_DELETE_ARRAY(paImprovementBonus[iBonusIndex].m_aiYieldChange);	// free memory - MT, since we are about to reallocate

							SetVariableListTagPair(&paImprovementBonus[iBonusIndex].m_aiYieldChange, "YieldChanges", NUM_YIELD_TYPES, 0);
						}
						else
						{
							gDLL->getXMLIFace()->SetToParent(m_pFXml);
						}

						// set the current xml node to it's parent node
					}

					// if we cannot set the current xml node to it's next sibling then we will break out of the for loop
					// otherwise we will continue looping
					if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
					{
						break;
					}
				}
			}
			// set the current xml node to it's parent node
			gDLL->getXMLIFace()->SetToParent(m_pFXml);
		}
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetAndLoadVar(int** ppiVar, int iDefault)
//
//  PURPOSE :   set the variable to a default and load it from the xml if there are any children
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetAndLoadVar(int** ppiVar, int iDefault)
{
	int iNumSibs;
	int* piVar;
	bool bReturn = false;
	int i; // loop counter

	// Skip any comments and stop at the next value we might want
	if (SkipToNextVal())
	{
		bReturn = true;

		// get the total number of children the current xml node has
		iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);

		// allocate memory
		InitList(ppiVar, iNumSibs, iDefault);

		// set the a local pointer to the newly allocated memory
		piVar = *ppiVar;

		// if the call to the function that sets the current xml node to it's first non-comment
		// child and sets the parameter with the new node's value succeeds
		if (GetChildXmlVal(&piVar[0]))
		{
			// loop through all the siblings, we start at 1 since we already got the first sibling
			for (i=1;i<iNumSibs;i++)
			{
				// if the call to the function that sets the current xml node to it's next non-comment
				// sibling and sets the parameter with the new node's value does not succeed
				// we will break out of this for loop
				if (!GetNextXmlVal(&piVar[i]))
				{
					break;
				}
			}

			// set the current xml node to it's parent node
			gDLL->getXMLIFace()->SetToParent(m_pFXml);
		}
	}

	return bReturn;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetVariableListTagPairForAudioScripts(int **ppiList, const TCHAR* szRootTagName,
//										int iInfoBaseLength, int iDefaultListVal)
//
//  PURPOSE :   allocate and initialize a list from a tag pair in the xml for audio scripts
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetVariableListTagPairForAudioScripts(int **ppiList, const TCHAR* szRootTagName, int iInfoBaseLength, int iDefaultListVal)
{
	int i;
	int iIndexVal;
	int iNumSibs;
	TCHAR szTextVal[256];
	int* piList;
	CvString szTemp;

	if (gDLL->getXMLIFace()->SetToChildByTagName(m_pFXml,szRootTagName))
	{
		if (SkipToNextVal())
		{
			iNumSibs = gDLL->getXMLIFace()->GetNumChildren(m_pFXml);
			if(!(0 < iInfoBaseLength))
			{
				char	szMessage[1024];
				sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPair \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
				gDLL->MessageBox(szMessage, "XML Error");
			}
			InitList(ppiList, iInfoBaseLength, iDefaultListVal);
			piList = *ppiList;
			if (0 < iNumSibs)
			{
				if(!(iNumSibs <= iInfoBaseLength))
				{
					char	szMessage[1024];
					sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPair \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Error");
				}
				if (gDLL->getXMLIFace()->SetToChild(m_pFXml))
				{
					for (i=0;i<iNumSibs;i++)
					{
						if (GetChildXmlVal(szTextVal))
						{
							iIndexVal = FindInInfoClass(szTextVal);
							GetNextXmlVal(&szTemp);
							if ( szTemp.GetLength() > 0 )
								piList[iIndexVal] = gDLL->getAudioTagIndex(szTemp);
							else
								piList[iIndexVal] = -1;

							gDLL->getXMLIFace()->SetToParent(m_pFXml);
						}

						if (!gDLL->getXMLIFace()->NextSibling(m_pFXml))
						{
							break;
						}
					}

					gDLL->getXMLIFace()->SetToParent(m_pFXml);
				}
			}
		}

		gDLL->getXMLIFace()->SetToParent(m_pFXml);
	}
}

DllExport bool CvXMLLoadUtility::LoadPlayerOptions()
{
	// load order: 1
	if (!CreateFXml())
		return false;

	/// XML type preloading - start - Nightinggale
	readXMLfiles(true);
	/// XML type preloading - end - Nightinggale

	LoadGlobalClassInfo(GC.getPlayerOptionInfo(), "CIV4PlayerOptionInfos", "GameInfo", "Civ4PlayerOptionInfos/PlayerOptionInfos/PlayerOptionInfo", NULL);
	FAssert(GC.getNumPlayerOptionInfos() == NUM_PLAYEROPTION_TYPES);

	DestroyFXml();
	return true;
}

DllExport bool CvXMLLoadUtility::LoadGraphicOptions()
{
	// load order: 2
	if (!CreateFXml())
		return false;

	LoadGlobalClassInfo(GC.getGraphicOptionInfo(), "CIV4GraphicOptionInfos", "GameInfo", "Civ4GraphicOptionInfos/GraphicOptionInfos/GraphicOptionInfo", NULL);
	FAssert(GC.getNumGraphicOptions() == NUM_GRAPHICOPTION_TYPES);

	DestroyFXml();
	return true;
}


