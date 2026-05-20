#include "../CvGameCoreDLL.h"
#include "Vanilla_CvDLLEngineIFaceBase.h"
#include "Vanilla_CvDLLEventReporterIFaceBase.h"
#include "Vanilla_CvDLLInterfaceIFaceBase.h"
#include "EXE_Plot.h"


EXE_Plot::EXE_Plot_YieldSymbol::EXE_Plot_YieldSymbol(CvPlot& kPlot)
	: m_plot(kPlot)
{
}

void EXE_Plot::EXE_Plot_YieldSymbol::clear()
{
	gDLL->getEngineIFace()->clearYieldSymbol(&m_plot);
}

void EXE_Plot::EXE_Plot_YieldSymbol::visible(bool bVisible)
{
	gDLL->getEngineIFace()->setYieldSymbolVisible(&m_plot, bVisible);
}

void EXE_Plot::EXE_Plot_YieldSymbol::offset(float fXOffset)
{
	gDLL->getEngineIFace()->setYieldSymbolOffset(&m_plot, fXOffset);
}

void EXE_Plot::EXE_Plot_YieldSymbol::appearance(YieldTypes eYield, float fAlpha, float fScale, bool bHighlight)
{
	FAssert(isInRange(eYield));

	if (!isInRange(eYield))
	{
		return;
	}

	gDLL->getEngineIFace()->setYieldSymbolAppearance(&m_plot, eYield, fAlpha, fScale, bHighlight);
}

void EXE_Plot::EXE_Plot_YieldSymbol::amount(YieldTypes eYield, int iCount)
{
	FAssert(isInRange(eYield));

	if (!isInRange(eYield))
	{
		return;
	}

	gDLL->getEngineIFace()->setYieldSymbolYieldAmount(&m_plot, eYield, iCount);
}


///////////////////

EXE_Plot::EXE_Plot_EventReporter::EXE_Plot_EventReporter(CvPlot& kPlot)
	: m_plot(kPlot)
{
}

void EXE_Plot::EXE_Plot_EventReporter::improvementChanged(ImprovementTypes eOldImprovement)
{
	const ImprovementTypes eImprovement = m_plot.getImprovementType();

	if (isInRange(eImprovement))
	{
		gDLL->getEventReporterIFace()->improvementBuilt(eImprovement, m_plot.getX_INLINE(), m_plot.getY_INLINE());
	}
	else
	{
		FAssert(eOldImprovement != NO_IMPROVEMENT);

		if (eOldImprovement != NO_IMPROVEMENT)
		{
			gDLL->getEventReporterIFace()->improvementDestroyed(eOldImprovement, m_plot.getOwnerINLINE(), m_plot.getX_INLINE(), m_plot.getY_INLINE());
		}
	}
}

void EXE_Plot::EXE_Plot_EventReporter::routeBuilt()
{
	const RouteTypes eRoute = m_plot.getRouteType();
	FAssert(isInRange(eRoute));

	if (!isInRange(eRoute))
	{
		return;
	}

	gDLL->getEventReporterIFace()->routeBuilt(eRoute, m_plot.getX_INLINE(), m_plot.getY_INLINE());
}

void EXE_Plot::EXE_Plot_EventReporter::featureRemoved(FeatureTypes eOldFeature, CvCity* pCity)
{
	FAssert(eOldFeature != NO_FEATURE);

	if (eOldFeature == NO_FEATURE)
	{
		return;
	}

	gDLL->getEventReporterIFace()->plotFeatureRemoved(&m_plot, eOldFeature, pCity);
}


void EXE_Plot::EXE_Plot_EventReporter::revealed(TeamTypes eTeam)
{
	FAssert(isInRange(eTeam));

	if (!isInRange(eTeam))
	{
		return;
	}
	
	gDLL->getEventReporterIFace()->plotRevealed(&m_plot, eTeam);
}

void EXE_Plot::EXE_Plot_EventReporter::picked()
{
	gDLL->getEventReporterIFace()->plotPicked(&m_plot);
}

void EXE_Plot::EXE_Plot_EventReporter::gotoPlot(PlayerTypes ePlayer)
{
	FAssert(isInRange(ePlayer));

	if (!isInRange(ePlayer))
	{
		return;
	}
	gDLL->getEventReporterIFace()->gotoPlotSet(&m_plot, ePlayer);
}

///////////////////

EXE_Plot::EXE_Plot_Texture::EXE_Plot_Texture(CvPlot& kPlot)
	: m_plot(kPlot)
{
}

void EXE_Plot::EXE_Plot_Texture::RebuildPlot(bool bRebuildHeights, bool bRebuildTextures)
{
	gDLL->getEngineIFace()->RebuildPlot(m_plot.getX_INLINE(), m_plot.getY_INLINE(), bRebuildHeights, bRebuildTextures);
}

void EXE_Plot::EXE_Plot_Texture::RebuildRiverPlotTile(bool bRebuildHeights, bool bRebuildTextures)
{
	gDLL->getEngineIFace()->RebuildRiverPlotTile(m_plot.getX_INLINE(), m_plot.getY_INLINE(), bRebuildHeights, bRebuildTextures);
}

void EXE_Plot::EXE_Plot_Texture::RebuildTileArt()
{
	gDLL->getEngineIFace()->RebuildTileArt(m_plot.getX_INLINE(), m_plot.getY_INLINE());
}

float EXE_Plot::EXE_Plot_Texture::GetHeightmapZ(bool bClampAboveWater)
{
	return gDLL->getEngineIFace()->GetHeightmapZ(m_plot.getPoint(), bClampAboveWater);
}

void EXE_Plot::EXE_Plot_Texture::MarkDirty()
{
	gDLL->getEngineIFace()->MarkPlotTextureAsDirty(m_plot.getX_INLINE(), m_plot.getY_INLINE());
}

///////////////////

EXE_Plot::EXE_Plot(CvPlot& kPlot)
	: m_plot(kPlot)
{
}

EXE_Plot::EXE_Plot_YieldSymbol EXE_Plot::YieldSymbol()
{
	return EXE_Plot_YieldSymbol(m_plot);
}

EXE_Plot::EXE_Plot_EventReporter EXE_Plot::reportEvent()
{
	return EXE_Plot_EventReporter(m_plot);
}

EXE_Plot::EXE_Plot_Texture EXE_Plot::texture()
{
	return EXE_Plot_Texture(m_plot);
}

void EXE_Plot::setPlotHighlight(const NiColorA& color, PlotStyles plotStyle, PlotLandscapeLayers layer)
{
	FAssert(isInRange(plotStyle));
	FAssert((int)layer >= -1 && (int)layer <= 2);

	if (!isInRange(plotStyle))
	{
		return;
	}

	if ((int)layer < -1)
	{
		return;
	}

	if ((int)layer > 2)
	{
		return;
	}

	gDLL->getEngineIFace()->addColoredPlot(m_plot.getX_INLINE(), m_plot.getY_INLINE(), color, plotStyle, layer);
}

void EXE_Plot::setPlotHighlight(ColorTypes color, PlotStyles plotStyle, PlotLandscapeLayers layer)
{
	FAssert(isInRange(color));
	FAssert(isInRange(plotStyle));
	FAssert((int)layer >= -1 && (int)layer <= 2);

	if (!isInRange(color))
	{
		return;
	}

	setPlotHighlight(GC.getColorInfo(color).getColor(), plotStyle, layer);
}

void EXE_Plot::setBorderColor(const NiColorA& color, AreaBorderLayers layer)
{
	FAssert(isInRange(layer));

	if (!isInRange(layer))
	{
		return;
	}

	gDLL->getEngineIFace()->fillAreaBorderPlot(m_plot.getX_INLINE(), m_plot.getY_INLINE(), color, layer);
}

void EXE_Plot::setBorderColor(ColorTypes color, AreaBorderLayers layer)
{
	FAssert(isInRange(color));

	if (!isInRange(color))
	{
		return;
	}

	setBorderColor(GC.getColorInfo(color).getColor(), layer);
}

void EXE_Plot::setMinimapColor(MinimapModeTypes eMinimapMode, ColorTypes eColor, float fAlpha)
{
	gDLL->getInterfaceIFace()->setMinimapColor(eMinimapMode, m_plot.getX_INLINE(), m_plot.getY_INLINE(), eColor, fAlpha);
}

void EXE_Plot::setFogOfWar(FogOfWarTypes eFOW)
{
	switch (eFOW)
	{
	case FOG_OF_WAR_REVEALED:
		gDLL->getEngineIFace()->LightenVisibility(m_plot.getFOWIndex());
		break;
	case FOG_OF_WAR_DARKENED:
		gDLL->getEngineIFace()->DarkenVisibility(m_plot.getFOWIndex());
		break;
	case FOG_OF_WAR_UNDISCOVERED:
		gDLL->getEngineIFace()->BlackenVisibility(m_plot.getFOWIndex());
		break;
	default:
		FAssertMsg(false, "missing case");
	}
}

void EXE_Plot::showEffect(EffectTypes eEffect, float rotation)
{
	// Note: the exe is confirmed to crash if the effect is not in range
	// with the crash being inside the exe, debugging is practically to impossible
	if (!isInRange(eEffect))
	{
		return;
	}
	
	gDLL->getEngineIFace()->TriggerEffect(eEffect, m_plot.getPoint(), rotation);
}

void EXE_Plot::playSound(LPCTSTR pszSound)
{
	gDLL->getInterfaceIFace()->playGeneralSound(pszSound, m_plot.getPoint());
}

void EXE_Plot::playSound(int iSoundId, int iSoundType)
{
	gDLL->getInterfaceIFace()->playGeneralSound(iSoundId, iSoundType, m_plot.getPoint());
}

void EXE_Plot::addLandmark(const wchar* caption)
{
	gDLL->getEngineIFace()->addLandmark(&m_plot, caption);
}

CvUnit* EXE_Plot::getInterfaceUnit(int iIndex)
{
	return gDLL->getInterfaceIFace()->getInterfacePlotUnit(&m_plot, iIndex);
}

void EXE_Plot::cameraLookAt()
{
	gDLL->getEngineIFace()->cameraLookAt(m_plot.getPoint());
}

void EXE_Plot::cameraLookAt(CameraLookAtTypes type, NiPoint3 attackDirection)
{
	gDLL->getInterfaceIFace()->lookAt(m_plot.getPoint(), type, attackDirection);
}
