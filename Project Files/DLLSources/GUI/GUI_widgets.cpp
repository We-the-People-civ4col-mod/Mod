#include "../CvGameCoreDLL.h"

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
	, m_eWidgetType(WIDGET_GENERAL)
	, m_iData1(-1)
	, m_iData2(-1)
	, m_name(name)
	, m_bShown(false)
	, m_bDrawn(false)
{
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

void GUI_Widget_base::moveBackward()
{
	m_interface.moveBackward(name());
}

void GUI_Widget_base::moveForward()
{
	m_interface.moveForward(name());
}

void GUI_Widget_base::moveToBack()
{
	m_interface.moveToBack(name());
}

void GUI_Widget_base::moveToFront()
{
	m_interface.moveToFront(name());
}

void GUI_Widget_base::setFocus()
{
	m_interface.setFocus(name());
}

WidgetTypes GUI_Widget_base::getWidgetType() const
{
	return m_eWidgetType;
}

int GUI_Widget_base::getData1() const
{
	return m_iData1;
}

int GUI_Widget_base::getData2() const
{
	return m_iData2;
}

const char* GUI_Widget_base::name() const
{
	return m_name.c_str();
}

bool GUI_Widget_base::neverDrawn() const
{
	return !m_bDrawn;
}

GUI_Widget_Location::GUI_Widget_Location(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_base(name, interface)
	, m_iX(-1)
	, m_iY(-1)
	, m_iWidth(0)
	, m_iHeight(0)
	, m_pParent(parent)
{
}

int GUI_Widget_Location::getX() const
{
	return m_iX;
}

int GUI_Widget_Location::getY() const
{
	return m_iY;
}

int GUI_Widget_Location::getWidth() const
{
	return m_iWidth;
}

int GUI_Widget_Location::getHeight() const
{
	return m_iHeight;
}

const GUI_Panel* GUI_Widget_Location::getParent() const
{
	return m_pParent;
}

void GUI_Widget_Location::setX(int iX)
{
	FAssert(neverDrawn());
	m_iX = iX;
}

void GUI_Widget_Location::setY(int iY)
{
	FAssert(neverDrawn());
	m_iY = iY;
}

void GUI_Widget_Location::setWidth(int iWidth)
{
	FAssert(neverDrawn());
	m_iWidth = iWidth;
}

void GUI_Widget_Location::setHeight(int iHeight)
{
	FAssert(neverDrawn());
	m_iHeight = iHeight;
}

void GUI_Widget_Location::setParent(GUI_Panel* parent)
{
	FAssert(neverDrawn());
	m_pParent = parent;
}


GUI_Widget_3D::GUI_Widget_3D(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
, m_iX(-1)
, m_iY(-1)
, m_iWidth(0)
, m_iHeight(0)
{
}

int GUI_Widget_3D::getX() const
{
	return m_iX;
}

int GUI_Widget_3D::getY() const
{
	return m_iY;
}

int GUI_Widget_3D::getWidth() const
{
	return m_iWidth;
}

int GUI_Widget_3D::getHeight() const
{
	return m_iHeight;
}

void GUI_Widget_3D::setX(int iX)
{
	FAssert(neverDrawn());
	m_iX = iX;
}

void GUI_Widget_3D::setY(int iY)
{
	FAssert(neverDrawn());
	m_iY = iY;
}

void GUI_Widget_3D::setWidth(int iWidth)
{
	FAssert(neverDrawn());
	m_iWidth = iWidth;
}

void GUI_Widget_3D::setHeight(int iHeight)
{
	FAssert(neverDrawn());
	m_iHeight = iHeight;
}

GUI_Widget_3D_Rotate::GUI_Widget_3D_Rotate(std::string name, GUI_python_interface& interface) : GUI_Widget_3D(name, interface)
	, m_fxRotation(20)
	, m_fzRotation(-30)
	, m_fScale(1)
	, m_bShowBackground(true)
{
}

float GUI_Widget_3D_Rotate::getX_Rotation() const
{
	return m_fxRotation;
}

float GUI_Widget_3D_Rotate::getZ_Rotation() const
{
	return m_fzRotation;
}

float GUI_Widget_3D_Rotate::getScale() const
{
	return m_fScale;
}

bool GUI_Widget_3D_Rotate::getShowBackground() const
{
	return m_bShowBackground;
}


void GUI_Widget_3D_Rotate::setX_Rotation(float fValue)

{
	FAssert(neverDrawn());
	m_fxRotation = fValue;
}
void GUI_Widget_3D_Rotate::setZ_Rotation(float fValue)
{
	FAssert(neverDrawn());
	m_fzRotation = fValue;
}

void GUI_Widget_3D_Rotate::setScale(float fValue)
{
	FAssert(neverDrawn());
	m_fScale = fValue;
}

void GUI_Widget_3D_Rotate::setShowBackground(bool bValue)
{
	FAssert(neverDrawn());
	m_bShowBackground = bValue;
}


GUI_Panel::GUI_Panel(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
{

}

void GUI_Panel::draw() const
{
#if 0
	void addPanel(std::string szName, std::wstring title, std::wstring helpText, bool bVerticalLayout, bool bScrollable, int iX, int iY, int iWidth, int iHeight, enum PanelStyles eStyle, enum WidgetTypes eWidgetType, int iData1, int iData2);
	void addScrollPanel(std::string szName, std::wstring title, int iX, int iY, int iWidth, int iHeight, enum PanelStyles eStyle, bool bTechTreeStyle, enum WidgetTypes eWidgetType, int iData1, int iData2);
	void attachPanel(std::string szAttachTo, std::string szName, std::wstring title, std::wstring helpText, bool bVerticalLayout, bool bScrollable, enum PanelStyles eStyle);

	void attachPanelAt(std::string szAttachTo, std::string szName, std::wstring title, std::wstring helpText, bool bVerticalLayout, bool bScrollable, enum PanelStyles eStyle, int iX, int iY, int iWidth, int iHeight,
		, int iData1, int iData2);
	void setPanelColor(std::string szName, int iRed, int iGreen, int iBlue);

	void setPanelSize(std::string szName, int iX, int iY, int iWidth, int iHeight);

	void setStyle(std::string szName, std::string szStyle);

#endif
}


GUI_Table::GUI_Table(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
{
}

void GUI_Table::draw() const
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

	void enableSort(); // name() argument used in BTS?

	void enableSelect(std::string szControlName, bool bEnable);

	void setStyle(std::string szName, std::string szStyle);

#endif
}

GUI_Multilist::GUI_Multilist(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
{
}

void GUI_Multilist::draw() const
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

GUI_MultiLineText::GUI_MultiLineText(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
{
}

void GUI_MultiLineText::draw() const
{

#if 0
	// iJustify can be the first 3 in JustificationTypes

	void addMultilineText(std::string szName, std::wstring szText, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eType, int iData1, int iData2, int iJustify);
	void attachMultilineText(std::string szAttachTo, std::string szName, std::wstring szText, enum WidgetTypes eType, int iData1, int iData2, int iJustify);

	void attachMultilineTextAt(std::string szAttachTo, std::string szName, std::wstring szText, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eType, int iData1, int iData2, int iJustify);


#endif
}

GUI_Text::GUI_Text(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Text::draw() const
{

#if 0
	// uiFlags can be the first 3 in JustificationTypes

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

void GUI_Label::draw() const
{

#if 0
	// uiFlags can be the first 3 in JustificationTypes

	void attachLabel(std::string szAttachTo, std::string szName, std::wstring szText);
	void modifyLabel(std::string szName, std::wstring szText, unsigned int uiFlags);
	void setHelpLabel(std::string szName, std::string szAtttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, std::wstring szHelpText);
	void setLabel(std::string szName, std::string szAtttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2);

	void setLabelAt(std::string szName, std::string szAttachTo, std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont, enum WidgetTypes eType, int iData1, int iData2);


#endif
}

GUI_Checkbox::GUI_Checkbox(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
{
}

void GUI_Checkbox::draw() const
{

#if 0
	void addCheckBoxGFC(std::string szName, std::string szTexture, std::string szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);

	void addCheckBoxGFCAt(std::string szName, std::string szTexture, std::string szHiliteTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);
	void attachCheckBoxGFC(std::string szAttachTo, std::string szName, std::string szTexture, std::string szHiliteTexture, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum ButtonStyles eStyle);
	bool getCheckBoxState(std::string szName);
	void setState(std::string szName, bool /*FxCheckBox::CheckBoxState*/ eState);


#endif
}

GUI_Editbox::GUI_Editbox(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
{
}

void GUI_Editbox::draw() const
{

#if 0
	void addEditBoxGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2, enum FontTypes eFont);
	std::wstring getEditBoxString(std::string szName);
	void setEditBoxMaxCharCount(std::string szName, int maxCharCount, int preferredCharCount);

	void setEditBoxString(std::string szName, std::wstring szString);

	void setEditBoxTextColor(std::string szName, NiColorA kColor);


#endif
}

GUI_Listbox::GUI_Listbox(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
{
}

void GUI_Listbox::draw() const
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



GUI_Dropdownbox::GUI_Dropdownbox(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
	, m_iX(0)
	, m_iY(0)
	, m_iWidth(0)
	, m_eFontType(TITLE_FONT)
	, m_iNumIndexes(0)
{
}

void GUI_Dropdownbox::draw() const
{
	m_interface.addDropDownBoxGFC(name(), m_iX, m_iY, m_iWidth, m_eWidgetType, m_iData1, m_iData2, m_eFontType);
#if 0
	// NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED
	void attachDropDownBoxGFC(std::string szAttachTo, std::string szName, bool bExpand); // unused in vanilla
#endif
}

void GUI_Dropdownbox::add(std::wstring szString, int iType, int iData, bool bSelected)
{
	m_interface.addPullDownString(name(), szString, iType, iData, bSelected);
	++m_iNumIndexes;
}

int GUI_Dropdownbox::getSelected() const
{
	return m_interface.getSelectedPullDownID(name());
}

int GUI_Dropdownbox::getData(int iIndex) const
{
	if (iIndex < 0 || iIndex >= m_iNumIndexes)
	{
		iIndex = getSelected();
	}
	return m_interface.getPullDownData(name(), iIndex);
}

int GUI_Dropdownbox::getType(int iIndex) const
{
	if (iIndex < 0 || iIndex >= m_iNumIndexes)
	{
		iIndex = getSelected();
	}
	return m_interface.getPullDownType(name(), iIndex);
}

int GUI_Dropdownbox::getNumIndexes() const
{
	return m_iNumIndexes;
}

void GUI_Dropdownbox::setX(int iX)
{
	FAssert(neverDrawn());
	m_iX = iX;
}

void GUI_Dropdownbox::setY(int iY)
{
	FAssert(neverDrawn());
	m_iY = iY;
}

void GUI_Dropdownbox::setWidth(int iWidth)
{
	FAssert(neverDrawn());
	m_iWidth = iWidth;
}

void GUI_Dropdownbox::setFont(FontTypes eFont)
{
	FAssert(neverDrawn());
	m_eFontType = eFont;
}

GUI_Bar::GUI_Bar(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
	, m_iNumBars(1)
{
}

void GUI_Bar::draw() const
{
	if (getParent() != NULL)
	{
		m_interface.addStackedBarGFCAt(name(), getParent()->name(), getX(), getY(), getWidth(), getHeight(), m_iNumBars, m_eWidgetType, m_iData1, m_iData2);
	}
	else
	{
		m_interface.addStackedBarGFC(name(), getX(), getY(), getWidth(), getHeight(), m_iNumBars, m_eWidgetType, m_iData1, m_iData2);
	}
}

void GUI_Bar::setNumBars(int iNumBars)
{
	FAssert(neverDrawn());
	FAssert(iNumBars >= 1);
	m_iNumBars = iNumBars;
}

int GUI_Bar::getNumBars() const
{
	return m_iNumBars;
}

void GUI_Bar::setColor(ColorTypes eColor, int iBar)
{
	FAssert(!neverDrawn());
	FAssert(iBar >= 0 && iBar < m_iNumBars);

	m_interface.setStackedBarColors(name(), iBar, eColor);
}

void GUI_Bar::setColor(ColorTypes eColor, float fAlpha, int iBar)
{
	FAssert(!neverDrawn());
	FAssert(iBar >= 0 && iBar < m_iNumBars);

	m_interface.setStackedBarColorsAlpha(name(), iBar, eColor, fAlpha);
}

void GUI_Bar::setColor(int iRed, int iGreen, int iBlue, float fAlpha, int iBar)
{
	FAssert(!neverDrawn());
	FAssert(iBar >= 0 && iBar < m_iNumBars);

	m_interface.setStackedBarColorsRGB(name(), iBar, iRed, iGreen, iBlue, fAlpha);
}

void GUI_Bar::setPercentage(float fPercentage, int iBar)
{
	FAssert(!neverDrawn());
	FAssert(iBar >= 0 && iBar < m_iNumBars);

	m_interface.setBarPercentage(name(), iBar, fPercentage);
}

GUI_Slider::GUI_Slider(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
	, m_iMin(0)
	, m_iMax(100)
	, m_iStartValue(50)
{
}

void GUI_Slider::draw() const
{
	m_interface.addSlider(name(), getX(), getY(), getWidth(), getHeight(), m_iStartValue, m_iMin, m_iMax, m_eWidgetType, m_iData1, m_iData2);
#if 0
	void attachSlider(std::string szAttachTo, std::string szName, int iX, int iY, int iWidth, int iHeight, int iDefault, int iMin, int iMax, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bIsVertical = false);
	void addSlider(std::string szName, int iX, int iY, int iWidth, int iHeight, int iDefault, int iMin, int iMax, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bIsVertical = false);


#endif
}

void GUI_Slider::setMin(int iMin)
{
	FAssert(neverDrawn());
	m_iMin = iMin;
}

void GUI_Slider::setMax(int iMax)
{
	FAssert(neverDrawn());
	m_iMax = iMax;
}

void GUI_Slider::setStartValue(int iStartValue)
{
	FAssert(neverDrawn());
	m_iStartValue = iStartValue;
}


GUI_Button::GUI_Button(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
	, m_eStyle(BUTTON_STYLE_STANDARD)
{
}

void GUI_Button::draw() const
{
	m_interface.setButtonGFC(name(), m_szText, m_szTexture, getX(), getY(), getWidth(), getHeight(), m_eWidgetType, m_iData1, m_iData2, m_eStyle);
}

void GUI_Button::setStyle(enum ButtonStyles eStyle)
{
	m_eStyle = eStyle;
	if (!neverDrawn())
	{
		m_interface.setStyle(name(), eStyle);
	}
}

void GUI_Button::setText(const wchar* szText)
{
	FAssert(neverDrawn());
	m_szText = szText;
}

void GUI_Button::setTexture(const char* szTexture)
{
	FAssert(neverDrawn());
	m_szTexture = szTexture;
}

GUI_ImageButton::GUI_ImageButton(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
{
}

void GUI_ImageButton::draw() const
{
	if (getParent() == NULL)
	{
		m_interface.setImageButton(name(), m_szTexture.c_str(), getX(), getY(), getWidth(), getHeight(), m_eWidgetType, m_iData1, m_iData2);
	}
	else
	{
		m_interface.setImageButtonAt(getParent()->name(), name(), m_szTexture.c_str(), getX(), getY(), getWidth(), getHeight(), m_eWidgetType, m_iData1, m_iData2);
	}


#if 0
	void attachImageButton(std::string szAttachTo, std::string szName, std::string szTexture, enum GenericButtonSizes eSize, enum WidgetTypes eWidgetType, int iData1, int iData2, bool bOption);
#endif
}

void GUI_ImageButton::changeTexture(const char* szTexture)
{
	m_szTexture = szTexture;
	if (!neverDrawn())
	{
		m_interface.changeImageButton(name(), szTexture);
	}
}

void GUI_ImageButton::setOverlay(const char* szOverlayTexture)
{
	FAssert(!neverDrawn());
	m_interface.overlayButtonGFC(name(), szOverlayTexture);
}

GUI_DragableButton::GUI_DragableButton(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
{
}

void GUI_DragableButton::draw() const
{
	if (getParent() == NULL)
	{
		m_interface.addDragableButton(name(), m_szTexture.c_str(), m_szHiliteTexture.c_str(), getX(), getY(), getWidth(), getHeight(), m_eWidgetType, m_iData1, m_iData2, m_eStyle);
	}
	else
	{
		m_interface.addDragableButtonAt(getParent()->name(), name(), m_szTexture.c_str(), m_szHiliteTexture.c_str(), getX(), getY(), getWidth(), getHeight(), m_eWidgetType, m_iData1, m_iData2, m_eStyle);

	}
}

void GUI_DragableButton::setTexture(const char* szTexture)
{
	FAssert(neverDrawn());
	m_szTexture = szTexture;
}

void GUI_DragableButton::setHiliteTexture(const char* szHiliteTexture)
{
	FAssert(neverDrawn());
	m_szHiliteTexture = szHiliteTexture;
}

void GUI_DragableButton::setStyle(enum ButtonStyles eStyle)
{
	FAssert(neverDrawn());
	m_eStyle = eStyle;
}


GUI_DDSGFC::GUI_DDSGFC(std::string name, GUI_python_interface& interface, GUI_Panel* parent, bool bOption) : GUI_Widget_Location(name, interface, parent)
	, m_bOption(bOption)
{
}

void GUI_DDSGFC::draw() const
{
	FAssert(m_szTexture.length() > 0);
	if (getParent() != NULL)
	{
		m_interface.addDDSGFCAt(name(), getParent()->name(), m_szTexture, getX(), getY(), getWidth(), getHeight(), m_eWidgetType, m_iData1, m_iData2, m_bOption);
	}
	else
	{
		m_interface.addDDSGFC(name(), m_szTexture, getX(), getY(), getWidth(), getHeight(), m_eWidgetType, m_iData1, m_iData2);
	}
}

void GUI_DDSGFC::changeTexture(const char* szTexture)
{
	m_szTexture = szTexture;
	if (!neverDrawn())
	{
		m_interface.changeDDSGFC(name(), szTexture);
	}
}

void GUI_DDSGFC::changeTexture(const CvAssetInfoBase* pAssetInfo)
{
	changeTexture(pAssetInfo->getPath());
}


GUI_BonusCFG::GUI_BonusCFG(std::string name, GUI_python_interface& interface, BonusTypes eBonus) : GUI_Widget_3D_Rotate(name, interface)
	, m_eBonus(eBonus)
{
}

void GUI_BonusCFG::draw() const
{
	m_interface.addBonusGraphicGFC(name(), m_eBonus, getX(), getY(), getWidth(), getHeight(), m_eWidgetType, m_iData1, m_iData2, getX_Rotation(), getZ_Rotation(), getScale(), getShowBackground());
}


GUI_BuildingCFG::GUI_BuildingCFG(std::string name, GUI_python_interface& interface, BuildingTypes eBuilding) : GUI_Widget_3D_Rotate(name, interface)
	, m_eBuilding(eBuilding)
{
}

void GUI_BuildingCFG::draw() const
{
	m_interface.addBuildingGraphicGFC(name(), m_eBuilding, getX(), getY(), getWidth(), getHeight(), m_eWidgetType, m_iData1, m_iData2, getX_Rotation(), getZ_Rotation(), getScale(), getShowBackground());
}


GUI_FlagCFG::GUI_FlagCFG(std::string name, GUI_python_interface& interface) : GUI_Widget_3D(name, interface)
{
}

void GUI_FlagCFG::draw() const
{

#if 0
	// unused in vanilla
	// BTS vanilla only uses it once
	// we don't have the WIDGET_FLAG DLL code

	# This will update the flag widget for SP hotseat and dbeugging
		def updateFlag(self) :

		if (CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_HIDE_ALL and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_MINIMAP_ONLY and CyInterface().getShowInterface() != InterfaceVisibility.INTERFACE_ADVANCED_START) :
			screen = CyGInterfaceScreen("MainInterface", CvScreenEnums.MAIN_INTERFACE)
			xResolution = screen.getXResolution()
			yResolution = screen.getYResolution()
			screen.addFlagWidgetGFC("CivilizationFlag", xResolution - 288, yResolution - 138, 68, 250, gc.getGame().getActivePlayer(), WidgetTypes.WIDGET_FLAG, gc.getGame().getActivePlayer(), -1)

	void addFlagWidgetGFC(std::string szName, int iX, int iY, int iWidth, int iHeight, int iOwner, enum WidgetTypes eWidgetType, int iData1, int iData2);


#endif
}


GUI_ImprovementCFG::GUI_ImprovementCFG(std::string name, GUI_python_interface& interface, ImprovementTypes eImprovement) : GUI_Widget_3D_Rotate(name, interface)
	, m_eImprovement(eImprovement)
{
}

void GUI_ImprovementCFG::draw() const
{
	m_interface.addImprovementGraphicGFC(name(), m_eImprovement, getX(), getY(), getWidth(), getHeight(), m_eWidgetType, m_iData1, m_iData2, getX_Rotation(), getZ_Rotation(), getScale(), getShowBackground());
}

void GUI_ImprovementCFG::setImprovement(ImprovementTypes eImprovement)
{
	m_eImprovement = eImprovement;
}

GUI_LeaderCFG::GUI_LeaderCFG(std::string name, GUI_python_interface& interface, enum LeaderHeadTypes eLeader) : GUI_Widget_3D(name, interface)
	, m_eLeader(eLeader)
	, m_eAttitude(ATTITUDE_FRIENDLY)
{
	FAssert(eLeader >= 0 && eLeader < GC.getNumLeaderHeadInfos());
	FAssert(m_eAttitude > NO_ATTITUDE && m_eAttitude < NUM_ATTITUDE_TYPES);

	if (eLeader < 0 || eLeader >= GC.getNumLeaderHeadInfos())
	{
		m_eLeader = (LeaderHeadTypes)0;
	}
}

void GUI_LeaderCFG::draw() const
{
	m_interface.addLeaderheadGFC(name(), m_eLeader, m_eAttitude, getX(), getY(), getWidth(), getHeight(), m_eWidgetType, m_iData1, m_iData2);
}

void GUI_LeaderCFG::setAttitude(AttitudeTypes eAttitude)
{
	FAssert(eAttitude > NO_ATTITUDE && eAttitude < NUM_ATTITUDE_TYPES);
	m_eAttitude = eAttitude;
	if (neverDrawn())
	{
		return;
	}
	m_interface.setLeaderheadMood(name(), eAttitude);
}

void GUI_LeaderCFG::performAction(LeaderheadAction eAction)
{
	FAssert(eAction > NO_LEADERANIM && eAction < NUM_LEADERANIM_TYPES);
	FAssert(!neverDrawn());
	m_interface.performLeaderheadAction(name(), eAction);
}

void GUI_LeaderCFG::performKeyInput(int iKey)
{
	FAssert(!neverDrawn());
	m_interface.leaderheadKeyInput(name(), iKey);
}


GUI_PlotCFG::GUI_PlotCFG(std::string name, GUI_python_interface& interface, const CvPlot& Plot) : GUI_Widget_3D(name, interface)
	, m_Plot(Plot)
	, m_iDistance(350)
	, m_bRenderUnits(false)
{
}

void GUI_PlotCFG::draw() const
{
	m_interface.addPlotGraphicGFC(name(), getX(), getY(), getWidth(), getHeight(), m_Plot, m_iDistance, m_bRenderUnits,  m_eWidgetType, m_iData1, m_iData2);
}

void GUI_PlotCFG::setDistance(int iDistance)
{
	FAssert(neverDrawn());
	m_iDistance = iDistance;
}

void GUI_PlotCFG::setRenderUnits(bool bRenderUnits)
{
	FAssert(neverDrawn());
	m_bRenderUnits = bRenderUnits;
}


GUI_UnitCFG::GUI_UnitCFG(std::string name, GUI_python_interface& interface, UnitTypes eUnit) : GUI_Widget_3D_Rotate(name, interface)
	, m_eUnit(eUnit)
	, m_eProfession(NO_PROFESSION)
{
}

void GUI_UnitCFG::draw() const
{
	m_interface.addUnitGraphicGFC(name(), m_eUnit, m_eProfession, getX(), getY(), getWidth(), getHeight(), m_eWidgetType, m_iData1, m_iData2, getX_Rotation(), getZ_Rotation(), getScale(), getShowBackground());
}

void GUI_UnitCFG::setProfession(ProfessionTypes eProfession)
{
	FAssert(neverDrawn());
	m_eProfession = eProfession;
}

GUI_Line::GUI_Line(std::string name, GUI_python_interface& interface) : GUI_Widget_base(name, interface)
{
}

void GUI_Line::draw() const
{

#if 0
	void addLineGFC(std::string szDrawCtrlName, std::string szName, int iStartX, int iStartY, int iEndX, int iEndY, ColorTypes eColor);
	void removeLineGFC(std::string szDrawCtrlName, std::string szName);


#endif
}

GUI_DrawControl::GUI_DrawControl(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
{
}

void GUI_DrawControl::draw() const
{

#if 0
	void addDrawControl(std::string szName, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);

	void addDrawControlAt(std::string szName, std::string szAttachTo, std::string szTexture, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);
	void changeDrawControl(std::string szName, std::string szTexture);


#endif
}

GUI_Minimap::GUI_Minimap(std::string name, GUI_python_interface& interface, GUI_Panel* parent) : GUI_Widget_Location(name, interface, parent)
{
}

void GUI_Minimap::draw() const
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



#if 0

// remove functions as they are being used. This way it's easier to keep track of what can be accessed through classes

void addMovieModelWidgetGFC(std::string szName, std::string szFile, int iX, int iY, int iWidth, int iHeight, enum WidgetTypes eWidgetType, int iData1, int iData2);

void attachButtonGFC(std::string szAttachTo, std::string szName, std::wstring szText, enum WidgetTypes eWidgetType, int iData1, int iData2);

void attachSeparator(std::string szAttachTo, std::string szName, bool bVertical);

int centerX(int iX);

int centerY(int iY);

void enable(std::string szName, bool bEnable);

void enableGridlines(std::string szName, bool bVertical, bool bHorizontal); // unused? table?

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

void markRenderTexturesDirty();

void modifyString(std::string szName, std::wstring szText, unsigned int uiFlags);

void moveItem(std::string szName, float fX, float fY, float fZ);

void playMovie(std::string szMovieName, float fX, float fY, float fWidth, float fHeight, float fZ);

void registerHideList(boost::python::list& szNames, int iSize, int iID);

void setActivation(std::string szName, enum ActivationTypes activation);

void setAlwaysShown(bool bAlwaysShown);

void setCloseOnEscape(bool bCloseOnEscape);

void setDimensions(int iX, int iY, int iWidth, int iHeight);

void setDying(bool bDying); // only used by BTS in victory screen when clicking spaceship

void setEndTurnState(std::string szName, std::wstring szText);

void setExitText(std::wstring szText, unsigned int uiFlags, float fX, float fY, float fZ, enum FontTypes eFont); // unused?

void setForcedRedraw(bool bRequiresForcedRedraw);

// what are the int arguments here?
void setHitMargins(std::string szName, int iA, int iB);

void setHitTest(std::string szName, enum hitTestTypes hitTest);

void setImageShape(std::string szName, enum ImageShapes eShape, int iParameter);

void setMainInterface(bool bMain);

void setPersistent(bool bPersistent);

void setPlacement(char* szName, int iX, int iY, int iWidth, int iHeight);

void setRenderFrozenWorld(bool val);

void setRenderInterfaceOnly(bool val);

void setScreenGroup(int iGroup);

void setShowFor();

void setSound(std::string pszSound);

void setSoundId(int iSoundId);

void setToolTipAlignment(std::string szName, enum ToolTipAlignTypes alignment);

void setViewMin(std::string szName, int iWidth, int iHeight);

void showEndTurn(std::string szName);

void showList(int iID);

void showScreen(enum PopupStates bState, bool bPassInput);

void showWindowBackground(bool bShow);

#endif
