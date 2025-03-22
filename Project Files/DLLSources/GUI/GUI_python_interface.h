#include <string>

class NiColorA;
class CyReplayInfo;

namespace boost
{
	namespace python
	{
		class list;
	}
}


class GUI_python_interface
{
	public:
		GUI_python_interface(std::string ScreenName, int iScreenEnum);

		void addBonusGraphicGFC(std::string szName, enum BonusTypes eBonus, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, float fxRotation, float fzRotation, float fScale, bool bShowBackground);

		void addBuildingGraphicGFC(std::string szName, enum BuildingTypes eBuilding, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, float fxRotation, float fzRotation, float fScale, bool bShowBackground);

		void addCheckBoxGFC(std::string szName, std::string szTexture, std::string szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);

		void addCheckBoxGFCAt(std::string szName, std::string szTexture, std::string szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);

		void addDDSGFC(std::string szName, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void addDDSGFCAt(std::string szName, std::string szAttachTo, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bOption);

		void addDragableButton(const char* szName, const char* szTexture, const char* szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);

		void addDragableButtonAt(const char* szAttachTo, const char* szName, const char* szTexture, const char* szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);

		void addDrawControl(std::string szName, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void addDrawControlAt(std::string szName, std::string szAttachTo, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void addDropDownBoxGFC(std::string szName, int iX, int iY, int iWidth, enum WidgetTypes eWidgetType, int iData1, int iData2, enum FontTypes eFontType);

		void addEditBoxGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum FontTypes eFont);

		void addFlagWidgetGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, int iOwner, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void addImprovementGraphicGFC(std::string szName, enum ImprovementTypes eImprovement, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, float fxRotation, float fzRotation, float fScale, bool bShowBackground);

		void addItemToTableGFC(std::string szAttachTo, std::wstring szText, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void addLeaderheadGFC(std::string szName, int eWho, int eInitAttitude, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidget, int iData1, int iData2);

		void addLineGFC(std::string szDrawCtrlName, std::string szName, int iStartX, int iStartY, int iEndX, int iEndY, enum ColorTypes eColor);

		void addListBoxGFC(std::string szName, std::wstring helpText, int iX, int iY, int iWidth, int iHeight, enum TableStyles eStyle);

		void addMovieModelWidgetGFC(std::string szName, std::string szFile, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void addMultiListControlGFC(std::string szName, std::wstring helpText, int iX, int iY, int iWidth, int iHeight, int numLists, int defaultWidth, int defaultHeight, enum TableStyles eStyle);

		void addMultiListControlGFCAt(std::string szName, std::wstring helpText, int iX, int iY, int iWidth, int iHeight, int numLists, int defaultWidth, int defaultHeight, enum TableStyles eStyle);

		void addMultilineText(std::string szName, std::wstring szText, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eType, int iData1, int iData2, int iJustify);

		void addPanel(std::string szName, std::wstring title, std::wstring helpText, bool bVerticalLayout, bool bScrollable, int iX, int iY, int iWidth, int iHeight, enum PanelStyles eStyle, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void addPlotGraphicGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, const class CvPlot& kPlot, int iDistance, bool renderUnits, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void addPullDownString(std::string szName, std::wstring szString, int iType, int iData, bool bSelected);

		void addScrollPanel(std::string szName, std::wstring title, int iX, int iY, int iWidth, int iHeight, enum PanelStyles eStyle, bool bTechTreeStyle, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void addSimpleTableControlGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, enum TableStyles eStyle);

		void addSlider(std::string szName, int iX, int iY, int iWidth, int iHeight, int iDefault, int iMin, int iMax, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bIsVertical = false);

		void addStackedBarGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, int iNumBars, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void addStackedBarGFCAt(std::string szName, std::string szAttachTo, int iX, int iY, int iWidth, int iHeight, int iNumBars, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void addTableControlGFC(std::string szName, int numColumns, int iX, int iY, int iWidth, int iHeight, bool bIncludeHeaders, bool bDrawGrid, int iconWidth, int iconHeight, enum TableStyles style);

		void addTableControlGFCWithHelp(std::string szName, int numColumns, int iX, int iY, int iWidth, int iHeight, bool bIncludeHeaders, bool bDrawGrid, int iconWidth, int iconHeight, enum TableStyles style, std::wstring szHelpText);

		void addTableHeaderGFC(std::string szAttachTo, std::wstring szText, int iCol, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void addUnitGraphicGFC(std::string szName, enum UnitTypes eUnit, enum ProfessionTypes eProfession, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, float fxRotation, float fzRotation, float fScale, bool bShowBackground); 

		void appendListBoxString(std::string szAttachTo, std::wstring item, enum WidgetTypes eType, int iData1, int iData2, int iJustify);

		void appendListBoxStringNoUpdate(std::string szAttachTo, std::wstring item, enum WidgetTypes eType, int iData1, int iData2, int iJustify);

		void appendMultiListButton(std::string szAttachTo, std::string szTexture, int listId, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bOption);

		int appendTableRow(std::string szName);

		void attachButtonGFC(std::string szAttachTo, std::string szName, std::wstring szText, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void attachCheckBoxGFC(std::string szAttachTo, std::string szName, std::string szTexture, std::string szHiliteTexture, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);

		void attachControlToTableCell(std::string szControlName, std::string szTableName, int iRow, int iColumn);

		void attachDropDownBoxGFC(std::string szAttachTo, std::string szName, bool bExpand);

		void attachImageButton(std::string szAttachTo, std::string szName, std::string szTexture, enum GenericButtonSizes eSize, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bOption);

		void attachLabel(std::string szAttachTo, std::string szName, std::wstring szText);

		void attachListBoxGFC(std::string szAttachTo, std::string szName, std::wstring helpText, enum TableStyles eStyle);

		void attachMultiListControlGFC(std::string szAttachTo, std::string szName, std::wstring helpText, int numLists, int defaultWidth, int defaultHeight, enum TableStyles eStyle);

		void attachMultilineText(std::string szAttachTo, std::string szName, std::wstring szText, enum WidgetTypes eType, int iData1, int iData2, int iJustify);
 
		void attachMultilineTextAt(std::string szAttachTo, std::string szName, std::wstring szText, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eType, int iData1, int iData2, int iJustify);

		void attachPanel(std::string szAttachTo, std::string szName, std::wstring title, std::wstring helpText, bool bVerticalLayout, bool bScrollable, enum PanelStyles eStyle);

		void attachPanelAt(std::string szAttachTo, std::string szName, std::wstring title, std::wstring helpText, bool bVerticalLayout, bool bScrollable, enum PanelStyles eStyle, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);

		void attachSeparator(std::string szAttachTo, std::string szName, bool bVertical);

		void attachSlider(std::string szAttachTo, std::string szName, int iX, int iY, int iWidth, int iHeight, int iDefault, int iMin, int iMax, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bIsVertical = false);

		void attachTableControlGFC(std::string szAttachTo, std::string szName, int numColumns, bool bIncludeHeaders, bool bDrawGrid, int iconWidth, int iconHeight, enum TableStyles style);

		void attachTextGFC(std::string szAttachTo, std::string szName, std::wstring text, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2);

		void bringMinimapToFront();

		int centerX(int iX);

		int centerY(int iY);

		void changeDDSGFC(std::string szName, std::string szTexture);

		void changeDrawControl(std::string szName, std::string szTexture);

		void changeImageButton(std::string szName, std::string szTexture);

		void clearListBoxGFC(std::string szListBoxName);

		void clearMultiList(std::string szName);

		void commitTableRow(std::string szAttachTo);

		void deleteWidget(std::string pszName);

		void disableMultiListButton(std::string szName, int iListId, int iIndexId, std::string szTexture);

		void enable(std::string szName, bool bEnable);

		void enableGridlines(std::string szName, bool bVertical, bool bHorizontal);

		void enableMultiListPulse(std::string szName, bool bEnable, int listId, int iIndexId);

		void enableSelect(std::string szControlName, bool bEnable);

		void enableSort();
  
		void enableWorldSounds(bool bEnable);
  
		bool getCheckBoxState(std::string szName);

		int getCurrentTime();

		std::wstring getEditBoxString(std::string szName);

		int getPullDownData(std::string szName, int iIndex);

		int getPullDownType(std::string szName, int iIndex);

		int getPythonFileID();

		bool getRenderFrozenWorld();

		bool getRenderInterfaceOnly();
  
		int getScreenGroup();

		int getSelectedPullDownID(std::string szName);

		int getTableNumColumns(std::string szName);
 
		int getTableNumRows(std::string szName);

		void getTableText(std::string szName, unsigned int iColumn, unsigned int iRow);
 
		int getX(std::string szName);

		int getXResolution();

		int getY(std::string szName);

		int getYResolution();
 
		void hide(std::string szName);

		void hideEndTurn(std::string szName);

		void hideList(int iID);
 
		void hideScreen();
 
		void initMinimap(int iLeft, int iRight, int iTop, int iBottom, float fZ, bool bCircular);
 
		bool isActive();

		bool isAlwaysShown();

		bool isPersistent();

		bool isRequiredForcedRedraw();
 
		bool isRowSelected(std::string szName, int iRow);
 
		void leaderheadKeyInput(std::string szName, int key);
 
		void markMinimapTexturePlotDirty(int iPlotX, int iPlotY);

		void markRenderTexturesDirty();

		void minimapClearAllFlashingTiles();

		void minimapClearLayer(int eMinimapLayer);

		void minimapFlashPlot(int iX, int iY, int eColor, float fSeconds);

		void modifyLabel(std::string szName, std::wstring szText, unsigned int uiFlags);

		void modifyString(std::string szName, std::wstring szText, unsigned int uiFlags);

		void moveBackward(std::string szName);

		void moveForward(std::string szName);

		void moveItem(std::string szName, float fX, float fY, float fZ);

		void moveToBack(std::string szName);

		void moveToFront(std::string szName);
 
		void overlayButtonGFC(std::string szName, const char* szTexture);

		void performLeaderheadAction(std::string  szName, int eAction);

		void placeMinimap(int iLeft, int iRight, int iTop, int iBottom);
 
		void playMovie(std::string szMovieName, float fX, float fY, float fWidth, float fHeight, float fZ);

		void prependListBoxString(std::string szAttachTo, std::wstring item, enum WidgetTypes eType, int iData1, int iData2, int iJustify);

		void registerHideList(boost::python::list& szNames, int iSize, int iID);

		void removeLineGFC(std::string szDrawCtrlName, std::string szName);
 
		void renderMinimapWorldTexture();

		void selectMultiList(std::string szName, int iListID);

		void selectRow(std::string szName, int iRow, bool bSelected);
 
		void setActivation(std::string szName, enum ActivationTypes activation);

		void setAlwaysShown(bool bAlwaysShown);

		void setBarPercentage(std::string szName, int iBar, float fPercent);

		void setButtonGFC(std::string szName, std::wstring szText, std::string szTexture, int iX, int iY, int imageWidth, int imageHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);

		void setCloseOnEscape(bool bCloseOnEscape);

		void setDimensions(int iX, int iY, int iWidth, int iHeight);

		void setDying(bool bDying);

		void setEditBoxMaxCharCount(std::string szName, int maxCharCount, int preferredCharCount);

		void setEditBoxString(std::string szName, std::wstring szString);
 
		void setEditBoxTextColor(std::string szName, NiColorA kColor);

		void setEndTurnState(std::string szName, std::wstring szText);

		void setExitText(std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont);

		void setFocus(std::string szName);

		void setForcedRedraw(bool bRequiresForcedRedraw);

		void setHelpLabel(std::string szName, std::string szAtttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, std::wstring szHelpText);

		void setHelpTextArea(float fWidth, enum FontTypes eFont, float fX, float fY, float fZ, bool bFloating, std::string szArtFile, bool bExpandRight, bool bExpandDown, unsigned int uiFlags, int iMinWidth);
 
		void setHelpTextString(std::wstring szString);
		
		// what are the int arguments here?
		void setHitMargins(std::string szName, int iA, int iB);
 
		void setHitTest(std::string szName, enum hitTestTypes hitTest);
 
		void setImageButton(std::string szName, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);
 
		void setImageButtonAt(std::string szName, std::string szAttachTo, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);
 
		void setImageShape(std::string szName, enum ImageShapes eShape, int iParameter);
 
		void setLabel(std::string szName, std::string szAtttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2);
 
		void setLabelAt(std::string szName, std::string szAttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2);

		void setLeaderheadMood(std::string  szName, int eAttitude);

		void setListBoxStringGFC(std::string szName, int item, std::wstring szText, enum WidgetTypes eType, int iData1, int iData2, int iJustify);
 
		void setMainInterface(bool bMain);

		void setMinimapColor(enum MinimapModeTypes eMinimapMode, int iX, int iY, int iColor, float fAlpha);

		void setMinimapMap(const CyReplayInfo* pReplayInfo, int iLeft, int iRight, int iTop, int iBottom, float fZ, bool bCircular);

		void setMinimapMode(enum MinimapModeTypes eMode);
 
		void setMinimapSectionOverride(float left, float bottom, float right, float top);
 
		void setPanelColor(std::string szName, int iRed, int iGreen, int iBlue);

		void setPanelSize(std::string szName, int iX, int iY, int iWidth, int iHeight);

		void setPersistent(bool bPersistent);

		void setPlacement(char* szName, int iX, int iY, int iWidth, int iHeight);

		void setRenderFrozenWorld(bool val);
    
		void setRenderInterfaceOnly(bool val);
   
		void setScreenGroup(int iGroup);
    
		void setSelectedListBoxStringGFC(std::string szName, int item);

		void setShowFor();
   
		void setSound(std::string pszSound);
   
		void setSoundId(int iSoundId);
   
		void setStackedBarColors(std::string szName, int iBar, ColorTypes eColor);
     
		void setStackedBarColorsAlpha(std::string szName, int iBar, ColorTypes eColor, float fAlpha);
    
		void setStackedBarColorsRGB(std::string szName, int iBar, int iRed, int iGreen, int iBlue, float fAlpha);
    
		void setState(std::string szName, bool /*FxCheckBox::CheckBoxState*/ eState);
 
		void setStyle(std::string szName, std::string szStyle);
		void setStyle(std::string szName, enum ButtonStyles eStyle);
		void setStyle(std::string szName, enum PanelStyles eStyle);
		void setStyle(std::string szName, enum TableStyles eStyle);
     
		void setTableColumnHeader(std::string szName, int iColumn, std::wstring header, int iWidth);
   
		void setTableColumnRightJustify(std::string szName, int iCol);
    
		void setTableDate(std::string szName, int iColumn, int iRow, std::wstring text, std::string szIcon, enum WidgetTypes eWidgetType, int iData1, int iData2, int iJustify);

		void setTableInt();
  
		void setTableNumRows(std::string szName, unsigned int numRows);
    
		void setTableRowHeight(std::string szName, int iRow, int iHeight);
     
		void setTableText(std::string szName, int iColumn, int iRow, std::wstring text, std::string szIcon, enum WidgetTypes eWidgetType, int iData1, int iData2, int iJustify);
    
		void setTableTextKey(std::string szName, int iColumn, std::wstring szKey, int iRowTest, std::wstring text, enum WidgetTypes eWidgetType, int iData1, int iData2, int iJustify, int iNumRows);
    
		void setText(std::string szName, std::string szAtttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2);
     
		void setTextAt(std::string szName, std::string szAttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2);
 
		void setToolTipAlignment(std::string szName, enum ToolTipAlignTypes alignment);
   
		void setViewMin(std::string szName, int iWidth, int iHeight);
    
		void show(std::string szName);

		void showEndTurn(std::string szName);
    
		void showList(int iID);
  
		void showScreen(enum PopupStates bState, bool bPassInput);
   
		void showWindowBackground(bool bShow);
     
		void updateAppropriateCitySelection(std::string szName, int iNumRows);
    
		void updateListBox(std::string szAttachTo);
   
		void updateMinimap(float fTime);
 
		void updateMinimapColorFromMap(enum MinimapModeTypes eMode, enum PlayerTypes ePlayer, float fAlpha);
   
		void updateMinimapSection(bool bWholeMap);
     
		void updateMinimapVisibility();

    private:
		const std::string m_ScreenName;
		const int m_iScreenEnum;
};
