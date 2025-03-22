#include <string>

class GUI_python_interface;

struct GUI_Position
{
	GUI_Position();
	
	int x;
	int y;
	int width;
	int height;

	class GUI_Widget_base* parent;
};

class GUI_Widget_base
{
public:
	GUI_Widget_base(std::string name, GUI_python_interface& interface);
	virtual ~GUI_Widget_base();
	
	void show();
	void hide();
	void moveBackward();

	void moveForward();
	void moveToBack();

	void moveToFront();
	void setFocus();

	virtual void draw() const = 0;

	const char* name() const;

	enum WidgetTypes getWidgetType() const;
	int getData1() const;
	int getData2() const;

protected:
	bool neverDrawn() const;

	GUI_python_interface& m_interface;

	// might not be available for all child classes, but in that case we keep them as defaults
	enum WidgetTypes m_eWidgetType;
	int m_iData1;
	int m_iData2;

private:
	std::string m_name;

	bool m_bShown;
	bool m_bDrawn;
};

class GUI_Widget_Location : public GUI_Widget_base
{
public:
	GUI_Widget_Location(std::string name, GUI_python_interface& interface, class GUI_Panel* parent = NULL);

	int getX() const;
	int getY() const;
	int getWidth() const;
	int getHeight() const;
	const GUI_Panel* getParent() const;

	void setX(int iX);
	void setY(int iY);
	void setWidth(int iWidth);
	void setHeight(int iHeight);

	void setParent(GUI_Panel* parent); // useful?

private:
	int m_iX;
	int m_iY;
	int m_iWidth;
	int m_iHeight;
	class GUI_Panel* m_pParent;
};

class GUI_Widget_3D : public GUI_Widget_base
{
public:
	GUI_Widget_3D(std::string name, GUI_python_interface& interface);

	int getX() const;
	int getY() const;
	int getWidth() const;
	int getHeight() const;

	void setX(int iX);
	void setY(int iY);
	void setWidth(int iWidth);
	void setHeight(int iHeight);

private:
	int m_iX;
	int m_iY;
	int m_iWidth;
	int m_iHeight;
};

class GUI_Widget_3D_Rotate : public GUI_Widget_3D
{
public:
	GUI_Widget_3D_Rotate(std::string name, GUI_python_interface& interface);

	float getX_Rotation() const;
	float getZ_Rotation() const;
	float getScale() const;
	bool getShowBackground() const;

	void setX_Rotation(float fValue);
	void setZ_Rotation(float fValue);
	void setScale(float fValue);
	void setShowBackground(bool bValue);

private:
	float m_fxRotation;
	float m_fzRotation;
	float m_fScale;
	bool m_bShowBackground;
};

class GUI_Panel : public GUI_Widget_Location
{
public:
	GUI_Panel(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

	void setStyle(enum PanelStyles eStyle);
};


class GUI_Table : public GUI_Widget_Location
{
public:
	GUI_Table(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

	void setStyle(enum TableStyles eStyle);
};

class GUI_Multilist : public GUI_Widget_Location
{
public:
	GUI_Multilist(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

};

class GUI_MultiLineText : public GUI_Widget_Location
{
public:
	GUI_MultiLineText(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

};

class GUI_Text : public GUI_Widget_base
{
public:
	GUI_Text(std::string name, GUI_python_interface& interface);

	void draw() const;

};

class GUI_Label : public GUI_Widget_base
{
public:
	GUI_Label(std::string name, GUI_python_interface& interface);

	void draw() const;

};


class GUI_Checkbox : public GUI_Widget_Location
{
public:
	GUI_Checkbox(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

};


class GUI_Editbox : public GUI_Widget_Location
{
public:
	GUI_Editbox(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

};


class GUI_Listbox : public GUI_Widget_Location
{
public:
	GUI_Listbox(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

};


class GUI_Dropdownbox : public GUI_Widget_base
{
public:
	GUI_Dropdownbox(std::string name, GUI_python_interface& interface);

	void draw() const;

	void add(std::wstring szString, int iType, int iData, bool bSelected = false);

	int getSelected() const;
	int getData(int iIndex = -1) const;
	int getType(int iIndex = -1) const;

	int getNumIndexes() const;

	void setX(int iX);
	void setY(int iY);
	void setWidth(int iWidth);
	void setFont(FontTypes eFont);

private:
	int m_iX;
	int m_iY;
	int m_iWidth;
	enum FontTypes m_eFontType;
	int m_iNumIndexes;

};


class GUI_Bar : public GUI_Widget_Location
{
public:
	GUI_Bar(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

	void setNumBars(int iNumBars);
	int getNumBars() const;

	void setColor(enum ColorTypes eColor, int iBar = 0);
	void setColor(enum ColorTypes eColor, float fAlpha, int iBar = 0);
	void setColor(int iRed, int iGreen, int iBlue, float fAlpha, int iBar = 0);

	void setPercentage(float fPercentage, int iBar = 0);

private:
	int m_iNumBars;
};


class GUI_Slider : public GUI_Widget_Location
{
public:
	GUI_Slider(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

	void setMin(int iMin);
	void setMax(int iMax);
	void setStartValue(int iStartValue);
private:
	int m_iMin;
	int m_iMax;
	int m_iStartValue;
};


class GUI_Button : public GUI_Widget_Location
{
public:
	GUI_Button(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

	void setStyle(enum ButtonStyles eStyle);
	void setText(const wchar* szText);
	void setTexture(const char* szTexture); // often or always "" in vanilla

private:
	enum ButtonStyles m_eStyle;
	std::wstring m_szText;
	std::string m_szTexture;
};


class GUI_ImageButton : public GUI_Widget_Location
{
public:
	GUI_ImageButton(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

	void changeTexture(const char* szTexture);
	void setOverlay(const char* szOverlayTexture);

private:
	std::string m_szTexture;
};


class GUI_DragableButton : public GUI_Widget_Location
{
public:
	GUI_DragableButton(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;
	void setTexture(const char* szTexture);
	void setHiliteTexture(const char* szHiliteTexture); // always "" in vanilla

	void setStyle(enum ButtonStyles eStyle);

private:
	enum ButtonStyles m_eStyle;
	std::string m_szTexture;
	std::string m_szHiliteTexture;

};


class GUI_DDSGFC : public GUI_Widget_Location
{
public:
	GUI_DDSGFC(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL, bool bOption = false);

	void draw() const;
	void changeTexture(const char* szTexture);
	void changeTexture(const class CvAssetInfoBase* pAssetInfo);

private:
	std::string m_szTexture;
	bool m_bOption;
};


class GUI_BonusCFG : public GUI_Widget_3D_Rotate
{
public:
	GUI_BonusCFG(std::string name, GUI_python_interface& interface, enum BonusTypes eBonus);

	void draw() const;

private:
	enum BonusTypes m_eBonus;
};


class GUI_BuildingCFG : public GUI_Widget_3D_Rotate
{
public:
	GUI_BuildingCFG(std::string name, GUI_python_interface& interface, enum BuildingTypes eBuilding);

	void draw() const;

private:
	enum BuildingTypes m_eBuilding;
};


class GUI_FlagCFG : public GUI_Widget_3D
{
public:
	GUI_FlagCFG(std::string name, GUI_python_interface& interface);

	void draw() const;

};


class GUI_ImprovementCFG : public GUI_Widget_3D_Rotate
{
public:
	GUI_ImprovementCFG(std::string name, GUI_python_interface& interface, enum ImprovementTypes eImprovement);

	void draw() const;
	void setImprovement(enum ImprovementTypes eImprovement);

private:
	enum ImprovementTypes m_eImprovement;
};


class GUI_LeaderCFG : public GUI_Widget_3D
{
public:
	GUI_LeaderCFG(std::string name, GUI_python_interface& interface, enum LeaderHeadTypes eLeader);

	void draw() const;

	void setAttitude(enum AttitudeTypes eAttitude);
	void performAction(enum LeaderheadAction eAction);
	void performKeyInput(int iKey);

private:
	enum LeaderHeadTypes m_eLeader;
	enum AttitudeTypes m_eAttitude;
};


class GUI_PlotCFG : public GUI_Widget_3D
{
public:
	GUI_PlotCFG(std::string name, GUI_python_interface& interface, const class CvPlot& Plot);

	void draw() const;

	void setDistance(int iDistance);
	void setRenderUnits(bool bRenderUnits);

private:
	const class CvPlot& m_Plot;
	int m_iDistance;
	bool m_bRenderUnits;
};


class GUI_UnitCFG : public GUI_Widget_3D_Rotate
{
public:
	GUI_UnitCFG(std::string name, GUI_python_interface& interface, enum UnitTypes eUnit);

	void draw() const;
	void setProfession(enum ProfessionTypes eProfession);

private:
	enum UnitTypes m_eUnit;
	enum ProfessionTypes m_eProfession;
};

class GUI_Line : public GUI_Widget_base
{
public:
	GUI_Line(std::string name, GUI_python_interface& interface);

	void draw() const;

};


class GUI_DrawControl : public GUI_Widget_Location
{
public:
	GUI_DrawControl(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

};


class GUI_Minimap : public GUI_Widget_Location
{
public:
	GUI_Minimap(std::string name, GUI_python_interface& interface, GUI_Panel* parent = NULL);

	void draw() const;

};
