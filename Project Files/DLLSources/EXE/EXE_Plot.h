#pragma once


//
//  Provides access to the EXE for CvPlot
//
// This will provide an overview of EXE features, which can be used by plots all collected in one dedicated location.
// Vanilla has mixed in the functions with various unrelated functions and spread them across multiple headers.
// This file also provides consistency to vanillas many and often onconsistent arguments.
// Arguments are assert checked and only givent to the EXE if valid. This avoids crashes due to out of range enum types.
//
// Access is provided by calling CvPlot::EXE()
//



// todo reduce what it needed to be included
// strictly speaking it's only absolutely needed in order to get wchar
#include "../CvGameCoreDLL.h"

class EXE_Plot
{
public:
	enum FogOfWarTypes
	{
		FOG_OF_WAR_REVEALED,
		FOG_OF_WAR_DARKENED,
		FOG_OF_WAR_UNDISCOVERED,
	};

	class EXE_Plot_YieldSymbol
	{
	public:
		EXE_Plot_YieldSymbol(CvPlot&);

		void clear();
		void visible(bool bVisible);
		void offset(float fXOffset);
		void appearance(YieldTypes eYield, float fAlpha, float fScale, bool bHighlight);
		void amount(YieldTypes eYield, int iCount);

	private:
		CvPlot& m_plot;
	};

	class EXE_Plot_EventReporter
	{
	public:
		EXE_Plot_EventReporter(CvPlot&);

		void improvementChanged(ImprovementTypes eOldImprovement);
		void routeBuilt();
		void featureRemoved(FeatureTypes eOldFeature, CvCity* pCity);

		void revealed(TeamTypes eTeam);

		void picked();
		void gotoPlot(PlayerTypes ePlayer);


	private:
		CvPlot& m_plot;
	};

	class EXE_Plot_Texture
	{
	public:
		EXE_Plot_Texture(CvPlot&);

		void RebuildPlot(bool bRebuildHeights, bool bRebuildTextures);
		void RebuildRiverPlotTile(bool bRebuildHeights, bool bRebuildTextures);
		void RebuildTileArt();

		float GetHeightmapZ(bool bClampAboveWater = true);

		void MarkDirty();

	private:
		CvPlot& m_plot;
	};

	EXE_Plot(CvPlot&);

	EXE_Plot_YieldSymbol YieldSymbol();
	EXE_Plot_EventReporter reportEvent();
	EXE_Plot_Texture texture();

	void setPlotHighlight(const NiColorA& color, PlotStyles plotStyle, PlotLandscapeLayers layer);
	void setPlotHighlight(ColorTypes color, PlotStyles plotStyle, PlotLandscapeLayers layer);
	void setBorderColor(const NiColorA& color, AreaBorderLayers layer);
	void setBorderColor(ColorTypes color, AreaBorderLayers layer);
	void setMinimapColor(MinimapModeTypes eMinimapMode, ColorTypes eColor, float fAlpha);

	void setFogOfWar(FogOfWarTypes eFOW);

	void showEffect(EffectTypes eEffect, float rotation = 0.0f);
	void playSound(LPCTSTR pszSound);
	void playSound(int iSoundId, int iSoundType = 1);

	void addLandmark(const wchar* caption);

	CvUnit* getInterfaceUnit(int iIndex);

	void cameraLookAt();
	void cameraLookAt(CameraLookAtTypes type, NiPoint3 attackDirection = NiPoint3(0, 1, 0));

private:
	CvPlot& m_plot;
};
