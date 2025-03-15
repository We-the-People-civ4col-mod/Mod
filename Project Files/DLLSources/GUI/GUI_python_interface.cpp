#include "../CvGameCoreDLL.h"

#include "GUI_python_interface.h"
#include "../CyArgsList.h"
#include "../CyPlot.h"

const char* const PY_MODULE = "CvGUIInterface";


GUI_python_interface::GUI_python_interface(std::string ScreenName, int iScreenEnum)
	: m_ScreenName(ScreenName)
	, m_iScreenEnum(iScreenEnum)
{	
}

void GUI_python_interface::addBonusGraphicGFC(std::string szName, BonusTypes eBonus, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, float fxRotation, float fzRotation, float fScale, bool bShowBackground)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(eBonus);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(fxRotation);
	argsList.add(fzRotation);
	argsList.add(fScale);
	argsList.add(bShowBackground);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addBonusGraphicGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addBuildingGraphicGFC(std::string szName, BuildingTypes eBuilding, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, float fxRotation, float fzRotation, float fScale, bool bShowBackground)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(eBuilding);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(fxRotation);
	argsList.add(fzRotation);
	argsList.add(fScale);
	argsList.add(bShowBackground);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addBuildingGraphicGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addCheckBoxGFC(std::string szName, std::string szTexture, std::string szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szTexture.c_str());
	argsList.add(szHiliteTexture.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addCheckBoxGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addCheckBoxGFCAt(std::string szName, std::string szTexture, std::string szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szTexture.c_str());
	argsList.add(szHiliteTexture.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addCheckBoxGFCAt", argsList.makeFunctionArgs());
}


void GUI_python_interface::addDDSGFC(std::string szName, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szTexture.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addDDSGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addDDSGFCAt(std::string szName, std::string szAttachTo, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bOption)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szAttachTo.c_str());
	argsList.add(szTexture.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(bOption);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addDDSGFCAt", argsList.makeFunctionArgs());
}


void GUI_python_interface::addDragableButton(const char* szName, const char* szTexture, const char* szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName);
	argsList.add(szTexture);
	argsList.add(szHiliteTexture);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addDragableButton", argsList.makeFunctionArgs());
}


void GUI_python_interface::addDragableButtonAt(const char* szAttachTo, const char* szName, const char* szTexture, const char* szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo);
	argsList.add(szName);
	argsList.add(szTexture);
	argsList.add(szHiliteTexture);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addDragableButtonAt", argsList.makeFunctionArgs());
}


void GUI_python_interface::addDrawControl(std::string szName, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szTexture.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addDrawControl", argsList.makeFunctionArgs());
}


void GUI_python_interface::addDrawControlAt(std::string szName, std::string szAttachTo, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szAttachTo.c_str());
	argsList.add(szTexture.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addDrawControlAt", argsList.makeFunctionArgs());
}



void GUI_python_interface::addDropDownBoxGFC(std::string szName, int iX, int iY, int iWidth, enum WidgetTypes eWidgetType, int iData1, int iData2, enum FontTypes eFontType)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(eFontType);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addDropDownBoxGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addEditBoxGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum FontTypes eFont)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(eFont);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addEditBoxGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addFlagWidgetGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, int iOwner, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(iOwner);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addFlagWidgetGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addImprovementGraphicGFC(std::string szName, ImprovementTypes eImprovement, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, float fxRotation, float fzRotation, float fScale, bool bShowBackground)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(eImprovement);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(fxRotation);
	argsList.add(fzRotation);
	argsList.add(fScale);
	argsList.add(bShowBackground);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addImprovementGraphicGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addItemToTableGFC(std::string szAttachTo, std::wstring szText, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szText.c_str());
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addItemToTableGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addLeaderheadGFC(std::string szName, int eWho, int eInitAttitude, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidget, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(eWho);
	argsList.add(eInitAttitude);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidget);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addLeaderheadGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addLineGFC(std::string szDrawCtrlName, std::string szName, int iStartX, int iStartY, int iEndX, int iEndY, ColorTypes eColor)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szDrawCtrlName.c_str());
	argsList.add(iStartX);
	argsList.add(iStartY);
	argsList.add(iEndX);
	argsList.add(iEndY);
	argsList.add(eColor);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addLineGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addListBoxGFC(std::string szName, std::wstring helpText, int iX, int iY, int iWidth, int iHeight, enum TableStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(helpText.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addListBoxGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addMovieModelWidgetGFC(std::string szName, std::string szFile, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szFile.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addMovieModelWidgetGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addMultiListControlGFC(std::string szName, std::wstring helpText, int iX, int iY, int iWidth, int iHeight, int numLists, int defaultWidth, int defaultHeight, enum TableStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(helpText.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(numLists);
	argsList.add(defaultWidth);
	argsList.add(defaultHeight);
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addMultiListControlGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addMultiListControlGFCAt(std::string szName, std::wstring helpText, int iX, int iY, int iWidth, int iHeight, int numLists, int defaultWidth, int defaultHeight, enum TableStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(helpText.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(numLists);
	argsList.add(defaultWidth);
	argsList.add(defaultHeight);
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addMultiListControlGFCAt", argsList.makeFunctionArgs());
}


void GUI_python_interface::addMultilineText(std::string szName, std::wstring szText, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eType, int iData1, int iData2, int iJustify)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szText.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(iJustify);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addMultilineText", argsList.makeFunctionArgs());
}


void GUI_python_interface::addPanel(std::string szName, std::wstring title, std::wstring helpText, bool bVerticalLayout, bool bScrollable, int iX, int iY, int iWidth, int iHeight, enum PanelStyles eStyle, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(title.c_str());
	argsList.add(helpText.c_str());
	argsList.add(bVerticalLayout);
	argsList.add(bScrollable);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eStyle);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addPanel", argsList.makeFunctionArgs());
}


void GUI_python_interface::addPlotGraphicGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, CyPlot kPlot, int iDistance, bool renderUnits, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(gDLL->getPythonIFace()->makePythonObject(&kPlot));
	argsList.add(iDistance);
	argsList.add(renderUnits);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addPlotGraphicGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addPullDownString(std::string szName, std::wstring szString, int iType, int iData, bool bSelected)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szString.c_str());
	argsList.add(iType);
	argsList.add(iData);
	argsList.add(bSelected);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addPullDownString", argsList.makeFunctionArgs());
}


void GUI_python_interface::addScrollPanel(std::string szName, std::wstring title, int iX, int iY, int iWidth, int iHeight, enum PanelStyles eStyle, bool bTechTreeStyle, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(title.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eStyle);
	argsList.add(bTechTreeStyle);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addScrollPanel", argsList.makeFunctionArgs());
}


void GUI_python_interface::addSimpleTableControlGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, enum TableStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addSimpleTableControlGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addSlider(std::string szName, int iX, int iY, int iWidth, int iHeight, int iDefault, int iMin, int iMax, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bIsVertical)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(iDefault);
	argsList.add(iMin);
	argsList.add(iMax);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(bIsVertical);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addSlider", argsList.makeFunctionArgs());
}


void GUI_python_interface::addStackedBarGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, int iNumBars, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(iNumBars);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addStackedBarGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addStackedBarGFCAt(std::string szName, std::string szAttachTo, int iX, int iY, int iWidth, int iHeight, int iNumBars, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szAttachTo.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(iNumBars);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addStackedBarGFCAt", argsList.makeFunctionArgs());
}


void GUI_python_interface::addTableControlGFC(std::string szName, int numColumns, int iX, int iY, int iWidth, int iHeight, bool bIncludeHeaders, bool bDrawGrid, int iconWidth, int iconHeight, enum TableStyles style)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(numColumns);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(bIncludeHeaders);
	argsList.add(bDrawGrid);
	argsList.add(iconWidth);
	argsList.add(iconHeight);
	argsList.add(style);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addTableControlGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addTableControlGFCWithHelp(std::string szName, int numColumns, int iX, int iY, int iWidth, int iHeight, bool bIncludeHeaders, bool bDrawGrid, int iconWidth, int iconHeight, enum TableStyles style, std::wstring szHelpText)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(numColumns);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(bIncludeHeaders);
	argsList.add(bDrawGrid);
	argsList.add(iconWidth);
	argsList.add(iconHeight);
	argsList.add(style);
	argsList.add(szHelpText.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addTableControlGFCWithHelp", argsList.makeFunctionArgs());
}


void GUI_python_interface::addTableHeaderGFC(std::string szAttachTo, std::wstring szText, int iCol, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szText.c_str());
	argsList.add(iCol);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addTableHeaderGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::addUnitGraphicGFC(std::string szName, UnitTypes eUnit, ProfessionTypes eProfession, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, float fxRotation, float fzRotation, float fScale, bool bShowBackground)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(eUnit);
	argsList.add(eProfession);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(fxRotation);
	argsList.add(fzRotation);
	argsList.add(fScale);
	argsList.add(bShowBackground);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "addUnitGraphicGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::appendListBoxString(std::string szAttachTo, std::wstring item, enum WidgetTypes eType, int iData1, int iData2, int iJustify)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(item.c_str());
	argsList.add(eType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(iJustify);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "appendListBoxString", argsList.makeFunctionArgs());
}


void GUI_python_interface::appendListBoxStringNoUpdate(std::string szAttachTo, std::wstring item, enum WidgetTypes eType, int iData1, int iData2, int iJustify)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(item.c_str());
	argsList.add(eType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(iJustify);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "appendListBoxStringNoUpdate", argsList.makeFunctionArgs());
}


void GUI_python_interface::appendMultiListButton(std::string szAttachTo, std::string szTexture, int listId, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bOption)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szTexture.c_str());
	argsList.add(listId);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(bOption);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "appendMultiListButton", argsList.makeFunctionArgs());
}


int GUI_python_interface::appendTableRow(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	long result = 0;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "appendTableRow", argsList.makeFunctionArgs(), &result);

	return result;
}


void GUI_python_interface::attachButtonGFC(std::string szAttachTo, std::string szName, std::wstring szText, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(szText.c_str());
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachButtonGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::attachCheckBoxGFC(std::string szAttachTo, std::string szName, std::string szTexture, std::string szHiliteTexture, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(szTexture.c_str());
	argsList.add(szHiliteTexture.c_str());
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachCheckBoxGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::attachControlToTableCell(std::string szControlName, std::string szTableName, int iRow, int iColumn)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szControlName.c_str());
	argsList.add(szTableName.c_str());
	argsList.add(iRow);
	argsList.add(iColumn);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachControlToTableCell", argsList.makeFunctionArgs());
}


void GUI_python_interface::attachDropDownBoxGFC(std::string szAttachTo, std::string szName, bool bExpand)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(bExpand);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachDropDownBoxGFC", argsList.makeFunctionArgs());
}


void GUI_python_interface::attachImageButton(std::string szAttachTo, std::string szName, std::string szTexture, enum GenericButtonSizes eSize, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bOption)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(szTexture.c_str());
	argsList.add(eSize);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(bOption);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachImageButton", argsList.makeFunctionArgs());
}


void GUI_python_interface::attachLabel(std::string szAttachTo, std::string szName, std::wstring szText)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(szText.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachLabel", argsList.makeFunctionArgs());
}


void GUI_python_interface::attachListBoxGFC(std::string szAttachTo, std::string szName, std::wstring helpText, enum TableStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(helpText.c_str());
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachListBoxGFC", argsList.makeFunctionArgs());
}

void GUI_python_interface::attachMultiListControlGFC(std::string szAttachTo, std::string szName, std::wstring helpText, int numLists, int defaultWidth, int defaultHeight, enum TableStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(helpText.c_str());
	argsList.add(numLists);
	argsList.add(defaultWidth);
	argsList.add(defaultHeight);
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachMultiListControlGFC", argsList.makeFunctionArgs());
}

void GUI_python_interface::attachMultilineText(std::string szAttachTo, std::string szName, std::wstring szText, enum WidgetTypes eType, int iData1, int iData2, int iJustify)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(szText.c_str());
	argsList.add(eType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(iJustify);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachMultilineText", argsList.makeFunctionArgs());
}

void GUI_python_interface::attachMultilineTextAt(std::string szAttachTo, std::string szName, std::wstring szText, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eType, int iData1, int iData2, int iJustify)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(szText.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(iJustify);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachMultilineTextAt", argsList.makeFunctionArgs());
}

void GUI_python_interface::attachPanel(std::string szAttachTo, std::string szName, std::wstring title, std::wstring helpText, bool bVerticalLayout, bool bScrollable, enum PanelStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(title.c_str());
	argsList.add(helpText.c_str());
	argsList.add(bVerticalLayout);
	argsList.add(bScrollable);
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachPanel", argsList.makeFunctionArgs());
}

void GUI_python_interface::attachPanelAt(std::string szAttachTo, std::string szName, std::wstring title, std::wstring helpText, bool bVerticalLayout, bool bScrollable, enum PanelStyles eStyle, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(title.c_str());
	argsList.add(helpText.c_str());
	argsList.add(bVerticalLayout);
	argsList.add(bScrollable);
	argsList.add(eStyle);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachPanelAt", argsList.makeFunctionArgs());
}

void GUI_python_interface::attachSeparator(std::string szAttachTo, std::string szName, bool bVertical)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(bVertical);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachSeparator", argsList.makeFunctionArgs());
}

void GUI_python_interface::attachSlider(std::string szAttachTo, std::string szName, int iX, int iY, int iWidth, int iHeight, int iDefault, int iMin, int iMax, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bIsVertical)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(iDefault);
	argsList.add(iMin);
	argsList.add(iMax);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(bIsVertical);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachSlider", argsList.makeFunctionArgs());
}

void GUI_python_interface::attachTableControlGFC(std::string szAttachTo, std::string szName, int numColumns, bool bIncludeHeaders, bool bDrawGrid, int iconWidth, int iconHeight, enum TableStyles style)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(numColumns);
	argsList.add(bIncludeHeaders);
	argsList.add(bDrawGrid);
	argsList.add(iconWidth);
	argsList.add(iconHeight);
	argsList.add(style);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachTableControlGFC", argsList.makeFunctionArgs());
}

void GUI_python_interface::attachTextGFC(std::string szAttachTo, std::string szName, std::wstring text, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(szName.c_str());
	argsList.add(text.c_str());
	argsList.add(eFont);
	argsList.add(eType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "attachTextGFC", argsList.makeFunctionArgs());
}

void GUI_python_interface::bringMinimapToFront()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "bringMinimapToFront", argsList.makeFunctionArgs());
}

int GUI_python_interface::centerX(int iX)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(iX);

	long result = 0;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "centerX", argsList.makeFunctionArgs(), &result);
	return result;
}

int GUI_python_interface::centerY(int iY)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(iY);

	long result = 0;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "centerY", argsList.makeFunctionArgs(), &result);
	return result;
}

void GUI_python_interface::changeDDSGFC(std::string szName, std::string szTexture)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szTexture.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "changeDDSGFC", argsList.makeFunctionArgs());
}

void GUI_python_interface::changeDrawControl(std::string szName, std::string szTexture)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szTexture.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "changeDrawControl", argsList.makeFunctionArgs());
}

void GUI_python_interface::changeImageButton(std::string szName, std::string szTexture)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szTexture.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "changeImageButton", argsList.makeFunctionArgs());
}

void GUI_python_interface::clearListBoxGFC(std::string szListBoxName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szListBoxName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "clearListBoxGFC", argsList.makeFunctionArgs());
}

void GUI_python_interface::clearMultiList(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "clearMultiList", argsList.makeFunctionArgs());
}

void GUI_python_interface::commitTableRow(std::string szAttachTo)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "commitTableRow", argsList.makeFunctionArgs());
}

void GUI_python_interface::deleteWidget(std::string pszName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(pszName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "deleteWidget", argsList.makeFunctionArgs());
}

void GUI_python_interface::disableMultiListButton(std::string szName, int iListId, int iIndexId, std::string szTexture)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iListId);
	argsList.add(iIndexId);
	argsList.add(szTexture.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "disableMultiListButton", argsList.makeFunctionArgs());
}

void GUI_python_interface::enable(std::string szName, bool bEnable)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(bEnable);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "enable", argsList.makeFunctionArgs());
}

void GUI_python_interface::enableGridlines(std::string szName, bool bVertical, bool bHorizontal)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(bVertical);
	argsList.add(bHorizontal);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "enableGridlines", argsList.makeFunctionArgs());
}

void GUI_python_interface::enableMultiListPulse(std::string szName, bool bEnable, int listId, int iIndexId)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(bEnable);
	argsList.add(listId);
	argsList.add(iIndexId);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "enableMultiListPulse", argsList.makeFunctionArgs());
}

void GUI_python_interface::enableSelect(std::string szControlName, bool bEnable)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szControlName.c_str());
	argsList.add(bEnable);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "enableSelect", argsList.makeFunctionArgs());
}

void GUI_python_interface::enableSort()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "enableSort", argsList.makeFunctionArgs());
}

void GUI_python_interface::enableWorldSounds(bool bEnable)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(bEnable);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "enableWorldSounds", argsList.makeFunctionArgs());
}

bool GUI_python_interface::getCheckBoxState(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getCheckBoxState", argsList.makeFunctionArgs(), &result);
	return result;
}

int GUI_python_interface::getCurrentTime()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getCurrentTime", argsList.makeFunctionArgs(), &result);
	return result;
}

std::wstring GUI_python_interface::getEditBoxString(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	CvWString result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "wstring GUI_python_interface::getEditBoxString", argsList.makeFunctionArgs(), &result);
	return result;
}

int GUI_python_interface::getPullDownData(std::string szName, int iIndex)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iIndex);

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getPullDownData", argsList.makeFunctionArgs(), &result);
	return result;
}

int GUI_python_interface::getPullDownType(std::string szName, int iIndex)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iIndex);

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getPullDownType", argsList.makeFunctionArgs(), &result);
	return result;
}

int GUI_python_interface::getPythonFileID()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getPythonFileID", argsList.makeFunctionArgs(), &result);
	return result;
}

bool GUI_python_interface::getRenderFrozenWorld()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getRenderFrozenWorld", argsList.makeFunctionArgs(), &result);
	return result;
}

bool GUI_python_interface::getRenderInterfaceOnly()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getRenderInterfaceOnly", argsList.makeFunctionArgs(), &result);
	return result;
}

int GUI_python_interface::getScreenGroup()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getScreenGroup", argsList.makeFunctionArgs(), &result);
	return result;
}

int GUI_python_interface::getSelectedPullDownID(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getSelectedPullDownID", argsList.makeFunctionArgs(), &result);
	return result;
}

int GUI_python_interface::getTableNumColumns(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getTableNumColumns", argsList.makeFunctionArgs(), &result);
	return result;
}

int GUI_python_interface::getTableNumRows(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getTableNumRows", argsList.makeFunctionArgs(), &result);
	return result;
}

void GUI_python_interface::getTableText(std::string szName, unsigned int iColumn, unsigned int iRow)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iColumn);
	argsList.add(iRow);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getTableText", argsList.makeFunctionArgs());
}

int GUI_python_interface::getX(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	long result = 0;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getX", argsList.makeFunctionArgs(), &result);
	return result;
}

int GUI_python_interface::getXResolution()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	long result = 0;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getXResolution", argsList.makeFunctionArgs(), &result);
	return result;
}

int GUI_python_interface::getY(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	long result = 0;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getY", argsList.makeFunctionArgs(), &result);
	return result;
}

int GUI_python_interface::getYResolution()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	long result = 0;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "getYResolution", argsList.makeFunctionArgs(), &result);
	return result;
}

void GUI_python_interface::hide(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "hide", argsList.makeFunctionArgs());
}

void GUI_python_interface::hideEndTurn(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "hideEndTurn", argsList.makeFunctionArgs());
}

void GUI_python_interface::hideList(int iID)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(iID);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "hideList", argsList.makeFunctionArgs());
}

void GUI_python_interface::hideScreen()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "hideScreen", argsList.makeFunctionArgs());
}

void GUI_python_interface::initMinimap(int iLeft, int iRight, int iTop, int iBottom, float fZ, bool bCircular)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(iLeft);
	argsList.add(iRight);
	argsList.add(iTop);
	argsList.add(iBottom);
	argsList.add(fZ);
	argsList.add(bCircular);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "initMinimap", argsList.makeFunctionArgs());
}

bool GUI_python_interface::isActive()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "isActive", argsList.makeFunctionArgs(), &result);
	return result;
}

bool GUI_python_interface::isAlwaysShown()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "isAlwaysShown", argsList.makeFunctionArgs(), &result);
	return result;
}

bool GUI_python_interface::isPersistent()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "isPersistent", argsList.makeFunctionArgs(), &result);
	return result;
}

bool GUI_python_interface::isRequiredForcedRedraw()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "isRequiredForcedRedraw", argsList.makeFunctionArgs(), &result);
	return result;
}

bool GUI_python_interface::isRowSelected(std::string szName, int iRow)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iRow);

	long result;
	gDLL->getPythonIFace()->callFunction(PY_MODULE, "isRowSelected", argsList.makeFunctionArgs(), &result);
	return result;
}

void GUI_python_interface::leaderheadKeyInput(std::string  szName, int key)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(key);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "leaderheadKeyInput", argsList.makeFunctionArgs());
}

void GUI_python_interface::markMinimapTexturePlotDirty(int iPlotX, int iPlotY)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(iPlotX);
	argsList.add(iPlotY);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "markMinimapTexturePlotDirty", argsList.makeFunctionArgs());
}

void GUI_python_interface::markRenderTexturesDirty()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "markRenderTexturesDirty", argsList.makeFunctionArgs());
}

void GUI_python_interface::minimapClearAllFlashingTiles()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "minimapClearAllFlashingTiles", argsList.makeFunctionArgs());
}

void GUI_python_interface::minimapClearLayer(int eMinimapLayer)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(eMinimapLayer);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "minimapClearLayer", argsList.makeFunctionArgs());
}

void GUI_python_interface::minimapFlashPlot(int iX, int iY, int eColor, float fSeconds)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(eColor);
	argsList.add(fSeconds);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "minimapFlashPlot", argsList.makeFunctionArgs());
}

void GUI_python_interface::modifyLabel(std::string szName, std::wstring szText, unsigned int uiFlags)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szText.c_str());
	argsList.add(uiFlags);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "modifyLabel", argsList.makeFunctionArgs());
}

void GUI_python_interface::modifyString(std::string szName, std::wstring szText, unsigned int uiFlags)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szText.c_str());
	argsList.add(uiFlags);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "modifyString", argsList.makeFunctionArgs());
}

void GUI_python_interface::moveBackward(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "moveBackward", argsList.makeFunctionArgs());
}

void GUI_python_interface::moveForward(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "moveForward", argsList.makeFunctionArgs());
}

void GUI_python_interface::moveItem(std::string szName, float fX, float fY, float fZ)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(fX);
	argsList.add(fY);
	argsList.add(fZ);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "moveItem", argsList.makeFunctionArgs());
}

void GUI_python_interface::moveToBack(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "moveToBack", argsList.makeFunctionArgs());
}

void GUI_python_interface::moveToFront(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "moveToFront", argsList.makeFunctionArgs());
}

void GUI_python_interface::overlayButtonGFC(std::string szName, const char* szTexture)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szTexture);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "overlayButtonGFC", argsList.makeFunctionArgs());
}

void GUI_python_interface::performLeaderheadAction(std::string  szName, int eAction)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(eAction);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "performLeaderheadAction", argsList.makeFunctionArgs());
}

void GUI_python_interface::placeMinimap(int iLeft, int iRight, int iTop, int iBottom)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(iLeft);
	argsList.add(iRight);
	argsList.add(iTop);
	argsList.add(iBottom);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "placeMinimap", argsList.makeFunctionArgs());
}

void GUI_python_interface::playMovie(std::string szMovieName, float fX, float fY, float fWidth, float fHeight, float fZ)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szMovieName.c_str());
	argsList.add(fX);
	argsList.add(fY);
	argsList.add(fWidth);
	argsList.add(fHeight);
	argsList.add(fZ);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "playMovie", argsList.makeFunctionArgs());
}

void GUI_python_interface::prependListBoxString(std::string szAttachTo, std::wstring item, enum WidgetTypes eType, int iData1, int iData2, int iJustify)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());
	argsList.add(item.c_str());
	argsList.add(eType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(iJustify);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "prependListBoxString", argsList.makeFunctionArgs());
}

void GUI_python_interface::registerHideList(boost::python::list& szNames, int iSize, int iID)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(gDLL->getPythonIFace()->makePythonObject(&szNames));
	argsList.add(iSize);
	argsList.add(iID);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "registerHideList", argsList.makeFunctionArgs());
}

void GUI_python_interface::removeLineGFC(std::string szDrawCtrlName, std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szDrawCtrlName.c_str());
	argsList.add(szName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "removeLineGFC", argsList.makeFunctionArgs());
}

void GUI_python_interface::renderMinimapWorldTexture()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "renderMinimapWorldTexture", argsList.makeFunctionArgs());
}

void GUI_python_interface::selectMultiList(std::string szName, int iListID)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iListID);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "selectMultiList", argsList.makeFunctionArgs());
}

void GUI_python_interface::selectRow(std::string szName, int iRow, bool bSelected)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iRow);
	argsList.add(bSelected);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "selectRow", argsList.makeFunctionArgs());
}

void GUI_python_interface::setActivation(std::string szName, enum ActivationTypes activation)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(activation);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setActivation", argsList.makeFunctionArgs());
}

void GUI_python_interface::setAlwaysShown(bool bAlwaysShown)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(bAlwaysShown);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setAlwaysShown", argsList.makeFunctionArgs());
}

void GUI_python_interface::setBarPercentage(std::string szName, int iBar, float fPercent)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iBar);
	argsList.add(fPercent);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setBarPercentage", argsList.makeFunctionArgs());
}

void GUI_python_interface::setButtonGFC(std::string szName, std::wstring szText, std::string szTexture, int iX, int iY, int imageWidth, int imageHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szText.c_str());
	argsList.add(szTexture.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(imageWidth);
	argsList.add(imageHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(eStyle);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setButtonGFC", argsList.makeFunctionArgs());
}

void GUI_python_interface::setCloseOnEscape(bool bCloseOnEscape)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(bCloseOnEscape);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setCloseOnEscape", argsList.makeFunctionArgs());
}

void GUI_python_interface::setDimensions(int iX, int iY, int iWidth, int iHeight)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setDimensions", argsList.makeFunctionArgs());
}

void GUI_python_interface::setDying(bool bDying)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(bDying);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setDying", argsList.makeFunctionArgs());
}

void GUI_python_interface::setEditBoxMaxCharCount(std::string szName, int maxCharCount, int preferredCharCount)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(maxCharCount);
	argsList.add(preferredCharCount);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setEditBoxMaxCharCount", argsList.makeFunctionArgs());
}

void GUI_python_interface::setEditBoxString(std::string szName, std::wstring szString)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szString.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setEditBoxString", argsList.makeFunctionArgs());
}

void GUI_python_interface::setEditBoxTextColor(std::string szName, NiColorA kColor)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(gDLL->getPythonIFace()->makePythonObject(&kColor));

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setEditBoxTextColor", argsList.makeFunctionArgs());
}

void GUI_python_interface::setEndTurnState(std::string szName, std::wstring szText)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szText.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setEndTurnState", argsList.makeFunctionArgs());
}

void GUI_python_interface::setExitText(std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szText.c_str());
	argsList.add(uiFlags);
	argsList.add(fX);
	argsList.add(fY);
	argsList.add(fZ);
	argsList.add(eFont);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setExitText", argsList.makeFunctionArgs());
}

void GUI_python_interface::setFocus(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setFocus", argsList.makeFunctionArgs());
}

void GUI_python_interface::setForcedRedraw(bool bRequiresForcedRedraw)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(bRequiresForcedRedraw);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setForcedRedraw", argsList.makeFunctionArgs());
}

void GUI_python_interface::setHelpLabel(std::string szName, std::string szAtttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, std::wstring szHelpText)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szAtttachTo.c_str());
	argsList.add(szText.c_str());
	argsList.add(uiFlags);
	argsList.add(fX);
	argsList.add(fY);
	argsList.add(fZ);
	argsList.add(eFont);
	argsList.add(szHelpText.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setHelpLabel", argsList.makeFunctionArgs());
}

void GUI_python_interface::setHelpTextArea(float fWidth, enum FontTypes eFont, float fX, float fY, float fZ, bool bFloating, std::string szArtFile, bool bExpandRight, bool bExpandDown, unsigned int uiFlags, int iMinWidth)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(fWidth);
	argsList.add(eFont);
	argsList.add(fX);
	argsList.add(fY);
	argsList.add(fZ);
	argsList.add(bFloating);
	argsList.add(szArtFile.c_str());
	argsList.add(bExpandRight);
	argsList.add(bExpandDown);
	argsList.add(uiFlags);
	argsList.add(iMinWidth);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setHelpTextArea", argsList.makeFunctionArgs());
}

void GUI_python_interface::setHelpTextString(std::wstring szString)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szString.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setHelpTextString", argsList.makeFunctionArgs());
}

void GUI_python_interface::setHitMargins(std::string szName, int iA, int iB)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iA);
	argsList.add(iB);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setHitMargins", argsList.makeFunctionArgs());
}

void GUI_python_interface::setHitTest(std::string szName, enum hitTestTypes hitTest)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(hitTest);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setHitTest", argsList.makeFunctionArgs());
}

void GUI_python_interface::setImageButton(std::string szName, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szTexture.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setImageButton", argsList.makeFunctionArgs());
}

void GUI_python_interface::setImageButtonAt(std::string szName, std::string szAttachTo, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szAttachTo.c_str());
	argsList.add(szTexture.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setImageButtonAt", argsList.makeFunctionArgs());
}

void GUI_python_interface::setImageShape(std::string szName, enum ImageShapes eShape, int iParameter)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(eShape);
	argsList.add(iParameter);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setImageShape", argsList.makeFunctionArgs());
}

void GUI_python_interface::setLabel(std::string szName, std::string szAtttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szAtttachTo.c_str());
	argsList.add(szText.c_str());
	argsList.add(uiFlags);
	argsList.add(fX);
	argsList.add(fY);
	argsList.add(fZ);
	argsList.add(eFont);
	argsList.add(eType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setLabel", argsList.makeFunctionArgs());
}

void GUI_python_interface::setLabelAt(std::string szName, std::string szAttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szAttachTo.c_str());
	argsList.add(szText.c_str());
	argsList.add(uiFlags);
	argsList.add(fX);
	argsList.add(fY);
	argsList.add(fZ);
	argsList.add(eFont);
	argsList.add(eType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setLabelAt", argsList.makeFunctionArgs());
}

void GUI_python_interface::setLeaderheadMood(std::string  szName, int eAttitude)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(eAttitude);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setLeaderheadMood", argsList.makeFunctionArgs());
}

void GUI_python_interface::setListBoxStringGFC(std::string szName, int item, std::wstring szText, enum WidgetTypes eType, int iData1, int iData2, int iJustify)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(item);
	argsList.add(szText.c_str());
	argsList.add(eType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(iJustify);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setListBoxStringGFC", argsList.makeFunctionArgs());
}

void GUI_python_interface::setMainInterface(bool bMain)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(bMain);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setMainInterface", argsList.makeFunctionArgs());
}

void GUI_python_interface::setMinimapColor(enum MinimapModeTypes eMinimapMode, int iX, int iY, int iColor, float fAlpha)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(eMinimapMode);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iColor);
	argsList.add(fAlpha);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setMinimapColor", argsList.makeFunctionArgs());
}

void GUI_python_interface::setMinimapMap(const CyReplayInfo* pReplayInfo, int iLeft, int iRight, int iTop, int iBottom, float fZ, bool bCircular)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(gDLL->getPythonIFace()->makePythonObject(pReplayInfo));
	argsList.add(iLeft);
	argsList.add(iRight);
	argsList.add(iTop);
	argsList.add(iBottom);
	argsList.add(fZ);
	argsList.add(bCircular);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setMinimapMap", argsList.makeFunctionArgs());
}

void GUI_python_interface::setMinimapMode(enum MinimapModeTypes eMode)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(eMode);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setMinimapMode", argsList.makeFunctionArgs());
}

void GUI_python_interface::setMinimapSectionOverride(float left, float bottom, float right, float top)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(left);
	argsList.add(bottom);
	argsList.add(right);
	argsList.add(top);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setMinimapSectionOverride", argsList.makeFunctionArgs());
}

void GUI_python_interface::setPanelColor(std::string szName, int iRed, int iGreen, int iBlue)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iRed);
	argsList.add(iGreen);
	argsList.add(iBlue);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setPanelColor", argsList.makeFunctionArgs());
}

void GUI_python_interface::setPanelSize(std::string szName, int iX, int iY, int iWidth, int iHeight)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setPanelSize", argsList.makeFunctionArgs());
}

void GUI_python_interface::setPersistent(bool bPersistent)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(bPersistent);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setPersistent", argsList.makeFunctionArgs());
}

void GUI_python_interface::setPlacement(char* szName, int iX, int iY, int iWidth, int iHeight)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName);
	argsList.add(iX);
	argsList.add(iY);
	argsList.add(iWidth);
	argsList.add(iHeight);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setPlacement", argsList.makeFunctionArgs());
}

void GUI_python_interface::setRenderFrozenWorld(bool val)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(val);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setRenderFrozenWorld", argsList.makeFunctionArgs());
}

void GUI_python_interface::setRenderInterfaceOnly(bool val)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(val);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setRenderInterfaceOnly", argsList.makeFunctionArgs());
}

void GUI_python_interface::setScreenGroup(int iGroup)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(iGroup);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setScreenGroup", argsList.makeFunctionArgs());
}

void GUI_python_interface::setSelectedListBoxStringGFC(std::string szName, int item)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(item);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setSelectedListBoxStringGFC", argsList.makeFunctionArgs());
}

void GUI_python_interface::setShowFor()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setShowFor", argsList.makeFunctionArgs());
}

void GUI_python_interface::setSound(std::string pszSound)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(pszSound.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setSound", argsList.makeFunctionArgs());
}

void GUI_python_interface::setSoundId(int iSoundId)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(iSoundId);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setSoundId", argsList.makeFunctionArgs());
}

void GUI_python_interface::setStackedBarColors(std::string szName, int iBar, ColorTypes eColor)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iBar);
	argsList.add(eColor);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setStackedBarColors", argsList.makeFunctionArgs());
}

void GUI_python_interface::setStackedBarColorsAlpha(std::string szName, int iBar, ColorTypes eColor, float fAlpha)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iBar);
	argsList.add(eColor);
	argsList.add(fAlpha);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setStackedBarColorsAlpha", argsList.makeFunctionArgs());
}

void GUI_python_interface::setStackedBarColorsRGB(std::string szName, int iBar, int iRed, int iGreen, int iBlue, float fAlpha)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iBar);
	argsList.add(iRed);
	argsList.add(iGreen);
	argsList.add(iBlue);
	argsList.add(fAlpha);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setStackedBarColorsRGB", argsList.makeFunctionArgs());
}

void GUI_python_interface::setState(std::string szName, bool /*FxCheckBox::CheckBoxState*/ eState)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(eState);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setState", argsList.makeFunctionArgs());
}

void GUI_python_interface::setStyle(std::string szName, std::string szStyle)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szStyle.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setStyle", argsList.makeFunctionArgs());
}

void GUI_python_interface::setTableColumnHeader(std::string szName, int iColumn, std::wstring header, int iWidth)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iColumn);
	argsList.add(header.c_str());
	argsList.add(iWidth);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setTableColumnHeader", argsList.makeFunctionArgs());
}

void GUI_python_interface::setTableColumnRightJustify(std::string szName, int iCol)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iCol);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setTableColumnRightJustify", argsList.makeFunctionArgs());
}

void GUI_python_interface::setTableDate(std::string szName, int iColumn, int iRow, std::wstring text, std::string szIcon, enum WidgetTypes eWidgetType, int iData1, int iData2, int iJustify)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iColumn);
	argsList.add(iRow);
	argsList.add(text.c_str());
	argsList.add(szIcon.c_str());
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(iJustify);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setTableDate", argsList.makeFunctionArgs());
}

void GUI_python_interface::setTableInt()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setTableInt", argsList.makeFunctionArgs());
}

void GUI_python_interface::setTableNumRows(std::string szName, unsigned int numRows)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(numRows);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setTableNumRows", argsList.makeFunctionArgs());
}

void GUI_python_interface::setTableRowHeight(std::string szName, int iRow, int iHeight)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iRow);
	argsList.add(iHeight);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setTableRowHeight", argsList.makeFunctionArgs());
}

void GUI_python_interface::setTableText(std::string szName, int iColumn, int iRow, std::wstring text, std::string szIcon, enum WidgetTypes eWidgetType, int iData1, int iData2, int iJustify)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iColumn);
	argsList.add(iRow);
	argsList.add(text.c_str());
	argsList.add(szIcon.c_str());
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(iJustify);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setTableText", argsList.makeFunctionArgs());
}

void GUI_python_interface::setTableTextKey(std::string szName, int iColumn, std::wstring szKey, int iRowTest, std::wstring text, enum WidgetTypes eWidgetType, int iData1, int iData2, int iJustify, int iNumRows)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iColumn);
	argsList.add(szKey.c_str());
	argsList.add(iRowTest);
	argsList.add(text.c_str());
	argsList.add(eWidgetType);
	argsList.add(iData1);
	argsList.add(iData2);
	argsList.add(iJustify);
	argsList.add(iNumRows);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setTableTextKey", argsList.makeFunctionArgs());
}

void GUI_python_interface::setText(std::string szName, std::string szAtttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szAtttachTo.c_str());
	argsList.add(szText.c_str());
	argsList.add(uiFlags);
	argsList.add(fX);
	argsList.add(fY);
	argsList.add(fZ);
	argsList.add(eFont);
	argsList.add(eType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setText", argsList.makeFunctionArgs());
}

void GUI_python_interface::setTextAt(std::string szName, std::string szAttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(szAttachTo.c_str());
	argsList.add(szText.c_str());
	argsList.add(uiFlags);
	argsList.add(fX);
	argsList.add(fY);
	argsList.add(fZ);
	argsList.add(eFont);
	argsList.add(eType);
	argsList.add(iData1);
	argsList.add(iData2);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setTextAt", argsList.makeFunctionArgs());
}

void GUI_python_interface::setToolTipAlignment(std::string szName, enum ToolTipAlignTypes alignment)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(alignment);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setToolTipAlignment", argsList.makeFunctionArgs());
}

void GUI_python_interface::setViewMin(std::string szName, int iWidth, int iHeight)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iWidth);
	argsList.add(iHeight);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "setViewMin", argsList.makeFunctionArgs());
}

void GUI_python_interface::show(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "show", argsList.makeFunctionArgs());
}

void GUI_python_interface::showEndTurn(std::string szName)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "showEndTurn", argsList.makeFunctionArgs());
}

void GUI_python_interface::showList(int iID)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(iID);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "showList", argsList.makeFunctionArgs());
}

void GUI_python_interface::showScreen(enum PopupStates bState, bool bPassInput)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(bState);
	argsList.add(bPassInput);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "showScreen", argsList.makeFunctionArgs());
}

void GUI_python_interface::showWindowBackground(bool bShow)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(bShow);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "showWindowBackground", argsList.makeFunctionArgs());
}

void GUI_python_interface::updateAppropriateCitySelection(std::string szName, int iNumRows)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szName.c_str());
	argsList.add(iNumRows);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "updateAppropriateCitySelection", argsList.makeFunctionArgs());
}

void GUI_python_interface::updateListBox(std::string szAttachTo)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(szAttachTo.c_str());

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "updateListBox", argsList.makeFunctionArgs());
}

void GUI_python_interface::updateMinimap(float fTime)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(fTime);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "updateMinimap", argsList.makeFunctionArgs());
}

void GUI_python_interface::updateMinimapColorFromMap(enum MinimapModeTypes eMode, enum PlayerTypes ePlayer, float fAlpha)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(eMode);
	argsList.add(ePlayer);
	argsList.add(fAlpha);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "updateMinimapColorFromMap", argsList.makeFunctionArgs());
}

void GUI_python_interface::updateMinimapSection(bool bWholeMap)
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);
	argsList.add(bWholeMap);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "updateMinimapSection", argsList.makeFunctionArgs());
}

void GUI_python_interface::updateMinimapVisibility()
{
	CyArgsList argsList;

	argsList.add(m_ScreenName.c_str());
	argsList.add(m_iScreenEnum);

	gDLL->getPythonIFace()->callFunction(PY_MODULE, "updateMinimapVisibility", argsList.makeFunctionArgs());
}
