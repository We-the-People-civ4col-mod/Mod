#pragma once

// Class to handle showing message windows
//
// The goal is to make a human readable interface to open one

class AlertWindow
{
public:
	struct returnTypes
	{
		friend class AlertWindow;
		enum enumTypes
		{
			clickedAbout = IDABORT,
			clickedCancel = IDCANCEL,
			clickedContinue = IDCONTINUE,
			clickedIgnore = IDIGNORE,
			clickedNo = IDNO,
			clickedOK = IDOK,
			clickedRetry = IDRETRY,
			clickedTryAgain = IDTRYAGAIN,
			clickedYes = IDYES,
		};
		returnTypes() : var(clickedOK) {}
		returnTypes(enumTypes newVar) : var(newVar) {}

		operator enumTypes() const { return var; }
	protected:
		returnTypes(int iValue) : var((enumTypes)iValue) {}
		enumTypes var;

	};

	struct iconTypes
	{
		enum enumTypes
		{
			IconNone,
			IconError,
			IconWarning,
			IconQuestion,
			IconInformation,
		};
		iconTypes() : var(IconNone) {}
		iconTypes(enumTypes eType) : var(eType) {}
		bool operator == (const enumTypes rhs)
		{
			return var == rhs;
		}
		operator enumTypes() const { return var; }
	protected:
		enumTypes var;
	};

	struct Buttons
	{
		enum enumTypes
		{
			BtnAboutRetryIgnore,
			BtnCancelTryAgainContinue,
			BtnHelp,
			BtnOK,
			BtnCancel,
			BtnRetryCancel,
			BtnYesNo,
			BtnYesNoCancel,
		};
		Buttons() : var(BtnOK) {}
		Buttons(enumTypes eType) : var(eType) {}
		operator enumTypes() const { return var; }
	protected:
		enumTypes var;
	};

	AlertWindow();
	returnTypes openWindow();

	void setDefaultButton(int iButton);
	void setButtonLayout(Buttons eButtons);
	void setIcon(iconTypes eIcon);

	void addArgument(const char* argument);
	void setMessageArguments(int i1, int i2 = 0, int i3 = 0, int i4 = 0, int i5 = 0);

	void lookupKeys();

	CvString header;
	CvString message;

protected:
	bool m_bKeysLookedup;
	int m_iDefaultButton;
	iconTypes m_Icon;
	Buttons m_Buttons;
};
