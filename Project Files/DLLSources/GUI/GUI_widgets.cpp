#include "GUI_widgets.h"
#include "GUI_python_interface.h"

GUI_Position::GUI_Position()
	: x(0)
	, y(0)
	, width(0)
	, height(0)
	, parent(NULL)
{
}

GUI_Widget_base::GUI_Widget_base(std::string name, GUI_python_interface& interface)
	: m_interface(interface)
	, m_name(name)
	, m_bShown(false)
	, m_bDrawn(false)
{
#if 0
	void moveBackward(std::string szName);

	void moveForward(std::string szName);
	void moveToBack(std::string szName);

	void moveToFront(std::string szName);
	void setFocus(std::string szName);
#endif
}

GUI_Widget_base::~GUI_Widget_base()
{
	if (m_bDrawn)
	{
		m_interface.deleteWidget(name());
	}
}

void GUI_Widget_base::show()
{
	if (m_bShown)
	{
		return;
	}
	if (!m_bDrawn)
	{
		m_bDrawn = true;
		draw();
	}
	else
	{
		m_interface.show(name());
	}
	m_bShown = true;
}

void GUI_Widget_base::hide()
{
	if (!m_bShown)
	{
		return;
	}

	m_interface.hide(name());
	m_bShown = false;
}


const std::string GUI_Widget_base::name()
{
	return m_name;
}

GUI_Panel::GUI_Panel(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{

}

void GUI_Panel::draw()
{
#if 0
	void addPanel(std::string szName, std::wstring title, std::wstring helpText, bool bVerticalLayout, bool bScrollable, int iX, int iY, int iWidth, int iHeight, enum PanelStyles eStyle, enum WidgetTypes eWidgetType, int iData1, int iData2);
	void addScrollPanel(std::string szName, std::wstring title, int iX, int iY, int iWidth, int iHeight, enum PanelStyles eStyle, bool bTechTreeStyle, enum WidgetTypes eWidgetType, int iData1, int iData2);
	void attachPanel(std::string szAttachTo, std::string szName, std::wstring title, std::wstring helpText, bool bVerticalLayout, bool bScrollable, enum PanelStyles eStyle);

	void attachPanelAt(std::string szAttachTo, std::string szName, std::wstring title, std::wstring helpText, bool bVerticalLayout, bool bScrollable, enum PanelStyles eStyle, int iX, int iY, int iWidth, int iHeight,
		, int iData1, int iData2);
	void setPanelColor(std::string szName, int iRed, int iGreen, int iBlue);

	void setPanelSize(std::string szName, int iX, int iY, int iWidth, int iHeight);


#endif
}


GUI_ImageButton::GUI_ImageButton(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_ImageButton::draw()
{

#if 0
	void attachImageButton(std::string szAttachTo, std::string szName, std::string szTexture, enum GenericButtonSizes eSize, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bOption);
	void changeImageButton(std::string szName, std::string szTexture);
	void setImageButton(std::string szName, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);

	void setImageButtonAt(std::string szName, std::string szAttachTo, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);


	void addDragableButton(const char* szName, const char* szTexture, const char* szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);

	void addDragableButtonAt(const char* szAttachTo, const char* szName, const char* szTexture, const char* szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);

	void setButtonGFC(std::string szName, std::wstring szText, std::string szTexture, int iX, int iY, int imageWidth, int imageHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);

#endif
}

GUI_Minimap::GUI_Minimap(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Minimap::draw()
{

#if 0
	void bringMinimapToFront();
	void initMinimap(int iLeft, int iRight, int iTop, int iBottom, float fZ, bool bCircular);
	void markMinimapTexturePlotDirty(int iPlotX, int iPlotY);
	void minimapClearAllFlashingTiles();

	void minimapClearLayer(int eMinimapLayer);

	void minimapFlashPlot(int iX, int iY, int eColor, float fSeconds);
	void placeMinimap(int iLeft, int iRight, int iTop, int iBottom);
	void renderMinimapWorldTexture();
	void setMinimapColor(enum MinimapModeTypes eMinimapMode, int iX, int iY, int iColor, float fAlpha);

	void setMinimapMap(const CyReplayInfo * pReplayInfo, int iLeft, int iRight, int iTop, int iBottom, float fZ, bool bCircular);

	void setMinimapMode(enum MinimapModeTypes eMode);
	void setMinimapSectionOverride(float left, float bottom, float right, float top);
	void updateMinimap(float fTime);

	void updateMinimapColorFromMap(enum MinimapModeTypes eMode, enum PlayerTypes ePlayer, float fAlpha);

	void updateMinimapSection(bool bWholeMap);

	void updateMinimapVisibility();

#endif
}

GUI_Table::GUI_Table(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Table::draw()
{

#if 0
	void addItemToTableGFC(std::string szAttachTo, std::wstring szText, enum WidgetTypes eWidgetType, int iData1, int iData2);
	void addSimpleTableControlGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, enum TableStyles eStyle);
	void addTableControlGFC(std::string szName, int numColumns, int iX, int iY, int iWidth, int iHeight, bool bIncludeHeaders, bool bDrawGrid, int iconWidth, int iconHeight, enum TableStyles style);

	void addTableControlGFCWithHelp(std::string szName, int numColumns, int iX, int iY, int iWidth, int iHeight, bool bIncludeHeaders, bool bDrawGrid, int iconWidth, int iconHeight, enum TableStyles style, std::wstring szHelpText);

	void addTableHeaderGFC(std::string szAttachTo, std::wstring szText, int iCol, enum WidgetTypes eWidgetType, int iData1, int iData2);
	int appendTableRow(std::string szName);
	void attachControlToTableCell(std::string szControlName, std::string szTableName, int iRow, int iColumn);
	void attachTableControlGFC(std::string szAttachTo, std::string szName, int numColumns, bool bIncludeHeaders, bool bDrawGrid, int iconWidth, int iconHeight, enum TableStyles style);
	void commitTableRow(std::string szAttachTo);
	int getTableNumColumns(std::string szName);

	int getTableNumRows(std::string szName);

	void getTableText(std::string szName, unsigned int iColumn, unsigned int iRow);
	void setTableColumnHeader(std::string szName, int iColumn, std::wstring header, int iWidth);

	void setTableColumnRightJustify(std::string szName, int iCol);

	void setTableDate(std::string szName, int iColumn, int iRow, std::wstring text, std::string szIcon, enum WidgetTypes eWidgetType, int iData1, int iData2, int iJustify);

	void setTableInt();

	void setTableNumRows(std::string szName, unsigned int numRows);

	void setTableRowHeight(std::string szName, int iRow, int iHeight);

	void setTableText(std::string szName, int iColumn, int iRow, std::wstring text, std::string szIcon, enum WidgetTypes eWidgetType, int iData1, int iData2, int iJustify);

	void setTableTextKey(std::string szName, int iColumn, std::wstring szKey, int iRowTest, std::wstring text, enum WidgetTypes eWidgetType, int iData1, int iData2, int iJustify, int iNumRows);

	void selectRow(std::string szName, int iRow, bool bSelected);
	bool isRowSelected(std::string szName, int iRow);

	void updateAppropriateCitySelection(std::string szName, int iNumRows); // vanilla code has an extra argument, which is always 1

#endif
}

GUI_Multilist::GUI_Multilist(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Multilist::draw()
{

#if 0
	void addMultiListControlGFC(std::string szName, std::wstring helpText, int iX, int iY, int iWidth, int iHeight, int numLists, int defaultWidth, int defaultHeight, enum TableStyles eStyle);

	void addMultiListControlGFCAt(std::string szName, std::wstring helpText, int iX, int iY, int iWidth, int iHeight, int numLists, int defaultWidth, int defaultHeight, enum TableStyles eStyle);
	void appendMultiListButton(std::string szAttachTo, std::string szTexture, int listId, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bOption);
	void attachMultiListControlGFC(std::string szAttachTo, std::string szName, std::wstring helpText, int numLists, int defaultWidth, int defaultHeight, enum TableStyles eStyle);
	void clearMultiList(std::string szName);

	void disableMultiListButton(std::string szName, int iListId, int iIndexId, std::string szTexture);
	void enableMultiListPulse(std::string szName, bool bEnable, int listId, int iIndexId);
	void selectMultiList(std::string szName, int iListID);


#endif
}

GUI_MultiLineText::GUI_MultiLineText(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_MultiLineText::draw()
{

#if 0
	void addMultilineText(std::string szName, std::wstring szText, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eType, int iData1, int iData2, int iJustify);
	void attachMultilineText(std::string szAttachTo, std::string szName, std::wstring szText, enum WidgetTypes eType, int iData1, int iData2, int iJustify);

	void attachMultilineTextAt(std::string szAttachTo, std::string szName, std::wstring szText, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eType, int iData1, int iData2, int iJustify);


#endif
}

GUI_Text::GUI_Text(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Text::draw()
{

#if 0
	void attachTextGFC(std::string szAttachTo, std::string szName, std::wstring text, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2);
	void setHelpTextArea(float fWidth, enum FontTypes eFont, float fX, float fY, float fZ, bool bFloating, std::string szArtFile, bool bExpandRight, bool bExpandDown, unsigned int uiFlags, int iMinWidth);

	void setHelpTextString(std::wstring szString);
	void setText(std::string szName, std::string szAtttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2);

	void setTextAt(std::string szName, std::string szAttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2);


#endif
}

GUI_Label::GUI_Label(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Label::draw()
{

#if 0
	void attachLabel(std::string szAttachTo, std::string szName, std::wstring szText);
	void modifyLabel(std::string szName, std::wstring szText, unsigned int uiFlags);
	void setHelpLabel(std::string szName, std::string szAtttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, std::wstring szHelpText);
	void setLabel(std::string szName, std::string szAtttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2);

	void setLabelAt(std::string szName, std::string szAttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2);


#endif
}

GUI_Bar::GUI_Bar(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Bar::draw()
{

#if 0
	void addStackedBarGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, int iNumBars, enum WidgetTypes eWidgetType, int iData1, int iData2);

	void addStackedBarGFCAt(std::string szName, std::string szAttachTo, int iX, int iY, int iWidth, int iHeight, int iNumBars, enum WidgetTypes eWidgetType, int iData1, int iData2);
	void setBarPercentage(std::string szName, int iBar, float fPercent);
	void setStackedBarColors(std::string szName, int iBar, ColorTypes eColor);

	void setStackedBarColorsAlpha(std::string szName, int iBar, ColorTypes eColor, float fAlpha);

	void setStackedBarColorsRGB(std::string szName, int iBar, int iRed, int iGreen, int iBlue, float fAlpha);


#endif
}

GUI_Checkbox::GUI_Checkbox(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Checkbox::draw()
{

#if 0
	void addCheckBoxGFC(std::string szName, std::string szTexture, std::string szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);

	void addCheckBoxGFCAt(std::string szName, std::string szTexture, std::string szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);
	void attachCheckBoxGFC(std::string szAttachTo, std::string szName, std::string szTexture, std::string szHiliteTexture, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);
	bool getCheckBoxState(std::string szName);
	void setState(std::string szName, bool /*FxCheckBox::CheckBoxState*/ eState);


#endif
}

GUI_Listbox::GUI_Listbox(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Listbox::draw()
{

#if 0
	void addListBoxGFC(std::string szName, std::wstring helpText, int iX, int iY, int iWidth, int iHeight, enum TableStyles eStyle);
	void appendListBoxString(std::string szAttachTo, std::wstring item, enum WidgetTypes eType, int iData1, int iData2, int iJustify);

	void appendListBoxStringNoUpdate(std::string szAttachTo, std::wstring item, enum WidgetTypes eType, int iData1, int iData2, int iJustify);
	void attachListBoxGFC(std::string szAttachTo, std::string szName, std::wstring helpText, enum TableStyles eStyle);
	void clearListBoxGFC(std::string szListBoxName);
	void prependListBoxString(std::string szAttachTo, std::wstring item, enum WidgetTypes eType, int iData1, int iData2, int iJustify);
	void setListBoxStringGFC(std::string szName, int item, std::wstring szText, enum WidgetTypes eType, int iData1, int iData2, int iJustify);
	void setSelectedListBoxStringGFC(std::string szName, int item);
	void updateListBox(std::string szAttachTo);

#endif
}

GUI_Editbox::GUI_Editbox(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Editbox::draw()
{

#if 0
	void addEditBoxGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum FontTypes eFont);
	std::wstring getEditBoxString(std::string szName);
	void setEditBoxMaxCharCount(std::string szName, int maxCharCount, int preferredCharCount);

	void setEditBoxString(std::string szName, std::wstring szString);

	void setEditBoxTextColor(std::string szName, NiColorA kColor);


#endif
}


GUI_Dropdownbox::GUI_Dropdownbox(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Dropdownbox::draw()
{

#if 0
	void addDropDownBoxGFC(std::string szName, int iX, int iY, int iWidth, enum WidgetTypes eWidgetType, int iData1, int iData2, enum FontTypes eFontType);
	void attachDropDownBoxGFC(std::string szAttachTo, std::string szName, bool bExpand);
	int getPullDownData(std::string szName, int iIndex);

	int getPullDownType(std::string szName, int iIndex);
	void addPullDownString(std::string szName, std::wstring szString, int iType, int iData, bool bSelected);
	int getSelectedPullDownID(std::string szName);



#endif
}


GUI_DDSGFC::GUI_DDSGFC(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_DDSGFC::draw()
{

#if 0
	void addDDSGFC(std::string szName, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);

	void addDDSGFCAt(std::string szName, std::string szAttachTo, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bOption);
	void changeDDSGFC(std::string szName, std::string szTexture);


#endif
}


GUI_Line::GUI_Line(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Line::draw()
{

#if 0
	void addLineGFC(std::string szDrawCtrlName, std::string szName, int iStartX, int iStartY, int iEndX, int iEndY, ColorTypes eColor);
	void removeLineGFC(std::string szDrawCtrlName, std::string szName);


#endif
}



#if 0

// remove functions as they are being used. This way it's easier to keep track of what can be accessed through classes

void addBonusGraphicGFC(std::string szName, BonusTypes eBonus, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, float fxRotation, float fzRotation, float fScale, bool bShowBackground);

void addBuildingGraphicGFC(std::string szName, BuildingTypes eBuilding, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, float fxRotation, float fzRotation, float fScale, bool bShowBackground);

void addDrawControl(std::string szName, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);

void addDrawControlAt(std::string szName, std::string szAttachTo, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);

void addFlagWidgetGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, int iOwner, enum WidgetTypes eWidgetType, int iData1, int iData2);

void addImprovementGraphicGFC(std::string szName, ImprovementTypes eImprovement, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, float fxRotation, float fzRotation, float fScale, bool bShowBackground);

void addLeaderheadGFC(std::string szName, int eWho, int eInitAttitude, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidget, int iData1, int iData2);

void addMovieModelWidgetGFC(std::string szName, std::string szFile, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);

void addPlotGraphicGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, CyPlot kPlot, int iDistance, bool renderUnits, enum WidgetTypes eWidgetType, int iData1, int iData2);

void addSlider(std::string szName, int iX, int iY, int iWidth, int iHeight, int iDefault, int iMin, int iMax, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bIsVertical = false);

void addUnitGraphicGFC(std::string szName, UnitTypes eUnit, ProfessionTypes eProfession, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, float fxRotation, float fzRotation, float fScale, bool bShowBackground);

void attachButtonGFC(std::string szAttachTo, std::string szName, std::wstring szText, enum WidgetTypes eWidgetType, int iData1, int iData2);

void attachSeparator(std::string szAttachTo, std::string szName, bool bVertical);

void attachSlider(std::string szAttachTo, std::string szName, int iX, int iY, int iWidth, int iHeight, int iDefault, int iMin, int iMax, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bIsVertical = false);

int centerX(int iX);

int centerY(int iY);

void changeDrawControl(std::string szName, std::string szTexture);

void enable(std::string szName, bool bEnable);

void enableGridlines(std::string szName, bool bVertical, bool bHorizontal);

void enableSelect(std::string szControlName, bool bEnable);

void enableSort();

void enableWorldSounds(bool bEnable);

int getCurrentTime();

int getPythonFileID();

bool getRenderFrozenWorld();

bool getRenderInterfaceOnly();

int getScreenGroup();

int getX(std::string szName);

int getXResolution();

int getY(std::string szName);

int getYResolution();

void hideEndTurn(std::string szName);

void hideList(int iID);

void hideScreen();

bool isActive();

bool isAlwaysShown();

bool isPersistent();

bool isRequiredForcedRedraw();

void leaderheadKeyInput(std::string szName, int key);

void markRenderTexturesDirty();

void modifyString(std::string szName, std::wstring szText, unsigned int uiFlags);

void moveItem(std::string szName, float fX, float fY, float fZ);

void overlayButtonGFC(std::string szName, const char* szTexture);

void performLeaderheadAction(std::string  szName, int eAction);

void playMovie(std::string szMovieName, float fX, float fY, float fWidth, float fHeight, float fZ);

void registerHideList(boost::python::list& szNames, int iSize, int iID);

void setActivation(std::string szName, enum ActivationTypes activation);

void setAlwaysShown(bool bAlwaysShown);

void setCloseOnEscape(bool bCloseOnEscape);

void setDimensions(int iX, int iY, int iWidth, int iHeight);

void setDying(bool bDying);

void setEndTurnState(std::string szName, std::wstring szText);

void setExitText(std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont);

void setForcedRedraw(bool bRequiresForcedRedraw);

// what are the int arguments here?
void setHitMargins(std::string szName, int iA, int iB);

void setHitTest(std::string szName, enum hitTestTypes hitTest);

void setImageShape(std::string szName, enum ImageShapes eShape, int iParameter);

void setLeaderheadMood(std::string  szName, int eAttitude);

void setMainInterface(bool bMain);

void setPersistent(bool bPersistent);

void setPlacement(char* szName, int iX, int iY, int iWidth, int iHeight);

void setRenderFrozenWorld(bool val);

void setRenderInterfaceOnly(bool val);

void setScreenGroup(int iGroup);

void setShowFor();

void setSound(std::string pszSound);

void setSoundId(int iSoundId);

void setStyle(std::string szName, std::string szStyle);

void setToolTipAlignment(std::string szName, enum ToolTipAlignTypes alignment);

void setViewMin(std::string szName, int iWidth, int iHeight);

void showEndTurn(std::string szName);

void showList(int iID);

void showScreen(enum PopupStates bState, bool bPassInput);

void showWindowBackground(bool bShow);

#endif
