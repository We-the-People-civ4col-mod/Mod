#pragma once

class CvPopup;

class PopupHandler
{
public:
	enum WindowStyle
	{
		// styles are in Resource\Themes\Civ4\Civ4Theme_Window.thm
		// most of them are untested
		Window_Standard_Style,
		Window_StandardResize_Style,
		Window_StandardNoResize_Style,

		Window_Standard_TitleBar_Normal_Style,
		Window_Standard_TitleBar_Maximized_Style,
		Window_Standard_TitleBar_Minimized_Style,
		Window_Standard_TitleBar_Collapsed_Style,
		Window_Standard_TitleBar_SysBorderCollapsed_Style,

		Window_StandardNoTitleBar_Style,
		Window_StandardNoTitleBarNoResize_Style,
		Window_NoTitleBar_Style,
		Window_NoTitleBarResize_Style,

		Window_Dialog_TitleBar_Maximized_Style,
		Window_Dialog_TitleBar_Collapsed_Style,
		
		Window_Tool_TitleBar_Normal_Style,
		Window_Tool_TitleBar_Maximized_Style,
		Window_Tool_TitleBar_Minimized_Style,
		Window_Tool_TitleBar_Collapsed_Style,
		Window_ToolResize_Style,
		
		Window_Alt_Style,
		Window_AltNoResize_Style,

		Window_Standard_OuterBorder_Normal_Style,
		Window_NoTitleBar_OuterBorder_Normal_Style,
		Window_Standard_OuterBorder_Min_Style,
		Window_Standard_OuterBorder_Max_Style,
		Window_Standard_OuterBorder_Collapse_Style,

		Window_Standard_InFrameBorder_Normal_Style,
		Window_StandardResize_InFrameBorder_Normal_Style,
		Window_NoTitleBar_InFrameBorder_Normal_Style,
		Window_NoTitleBarResize_InFrameBorder_Normal_Style,

		Window_Tool_OuterBorder_Normal_Style,
		Window_Tool_OuterBorder_Min_Style,
		Window_Tool_OuterBorder_Max_Style,
		Window_Tool_OuterBorder_Collapse_Style,
		Window_Tool_InFrameBorder_Normal_Style,
		Window_ToolResize_InFrameBorder_Normal_Style,
	};

	class GenericButton
	{
	public:
		GenericButton(CvPopup*);

		void create();

		GenericButton& text(CvWString);
		GenericButton& icon(const char*);
		GenericButton& id(int);
		GenericButton& widget(WidgetTypes);
		GenericButton& data1(int);
		GenericButton& data2(int);
		GenericButton& option(bool);
		GenericButton& controlLayout(PopupControlLayout);
		GenericButton& textJustifcation(JustificationTypes);

	private:
		CvPopup* m_popup;

		CvWString m_szText;
		const char* m_szIcon;
		int m_iButtonId;
		WidgetTypes m_eWidgetType;
		int m_iData1;
		int m_iData2;
		bool m_bOption;
		PopupControlLayout m_ctrlLayout;
		JustificationTypes m_textJustifcation;
	};

	class EditBox
	{
	public:
		EditBox(CvPopup*);

		void create();

		EditBox& defaultString(CvWString);
		EditBox& widget(WidgetTypes);
		EditBox& helpString(CvWString);
		EditBox& group(int);
		EditBox& controlLayout(PopupControlLayout);
		EditBox& charWidth(unsigned int);
		EditBox& charCount(unsigned int);
		EditBox& enable(bool);
		
	private:
		CvPopup* m_popup;
		CvWString m_szDefaultString;
		WidgetTypes m_eWidgetType;
		CvWString m_szHelpText;
		int m_iGroup;
		PopupControlLayout m_ctrlLayout;
		unsigned int m_preferredCharWidth;
		unsigned int m_maxCharCount;
		bool m_bEnable;
	};

	class Checkbox
	{
	public:
		Checkbox(CvPopup*, int ID, int iGroup);

		void setText(CvWString szText, CvWString szHelpText = "");
		void setState(bool bChecked);

	private:
		CvPopup* m_popup;
		int m_iID;
		int m_iGroup;
	};

	class CheckBoxes
	{
	public:
		CheckBoxes(CvPopup*, int iNumBoxes);
		void create();

		CheckBoxes& group(int);
		CheckBoxes& widget(WidgetTypes);
		CheckBoxes& controlLayout(PopupControlLayout);

		Checkbox getButton(int ID);

	private:
		CvPopup* m_popup;
		bool m_bCreated;

		int m_iNumButtons;
		int m_iGroup;
		WidgetTypes m_eWidgetType;
		PopupControlLayout m_ctrlLayout;
	};

	class RadioButton
	{
	public:
		RadioButton(CvPopup*, int ID, int iGroup);

		void setText(CvWString szText, CvWString szHelpText = "");

	private:
		CvPopup* m_popup;
		int m_iID;
		int m_iGroup;
	};

	class RadioButtons
	{
	public:
		RadioButtons(CvPopup*, int iNumButtons);
		void create();

		RadioButtons& group(int);
		RadioButtons& widget(WidgetTypes);
		RadioButtons& controlLayout(PopupControlLayout);

		RadioButton getButton(int ID);

	private:
		CvPopup* m_popup;
		bool m_bCreated;

		int m_iNumButtons;
		int m_iGroup;
		WidgetTypes m_eWidgetType;
		PopupControlLayout m_ctrlLayout;
	};

	class SpinBox
	{
	public:
		SpinBox(CvPopup*);
		void create();

		SpinBox& index(int);
		SpinBox& help(CvWString);
		SpinBox& setDefault(int);
		SpinBox& increment(int);
		SpinBox& max(int);
		SpinBox& min(int);

	private:
		CvPopup* m_popup;
		int m_iIndex;
		CvWString m_szHelpText;
		int m_iDefault;
		int m_iIncrement;
		int m_iMax;
		int m_iMin;
	};


	PopupHandler(CvPopup*);

	// flags for layout are unknown
	// functions not present in vanilla
	// HLayout only uses flag = 0 and VLayout is unused even in vanilla
	void startVLayout(uint iFlags);
	void startHLayout(uint iFlags);
	void endLayout();

	void setHeaderString(CvWString szText, JustificationTypes uiFlags = DLL_FONT_CENTER_JUSTIFY);
	void setBodyString(CvWString szText, JustificationTypes uiFlags = DLL_FONT_LEFT_JUSTIFY, char* szName = NULL, CvWString szHelpText = "");

	void addSeparator(int iSpace = 0);
		
	void setPopupType(PopupEventTypes ePopupType, LPCTSTR szArtFileName = NULL);
	
	void setStyle(WindowStyle);

	void addDDS(const char* szIconFilename, int iWidth = 0, int iHeight = 0, CvWString szHelpText = "");

	GenericButton createGenericButton();

	EditBox createEditBox();

	CheckBoxes createCheckBoxes(int iNumBoxes);

	RadioButtons createRadioButtons(int iNumButtons);
	
	SpinBox createSpinBox();

	void launch();

	void setAsCancelled();
	bool isDying();

	PopupHandler& enableOKButton(bool bEnable);
	PopupHandler& setState(PopupStates eState);
	PopupHandler& setNumPixelScroll(int iCount);

protected:
	const char* getStyle(WindowStyle) const;

	CvPopup* m_popup;

	bool m_bOkButton;
	PopupStates m_eState;
	int m_iNumPixelScroll;
};
