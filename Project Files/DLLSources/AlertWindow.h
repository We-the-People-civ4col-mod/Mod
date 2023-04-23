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

		enumTypes getVar() { return var; }
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
		enumTypes getVar() { return var; }
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
		enumTypes getVar() { return var; }
	protected:
		enumTypes var;
	};

	AlertWindow();
	returnTypes openWindow();

	void setDefaultButton(int iButton);
	void setButtonLayout(Buttons eButtons);
	void setIcon(iconTypes eIcon);

	CvString header;
	CvString message;

	template <typename T1>
	void setMessageArguments(T1 a1)
	{
		lookupKeys();
		message = CvString::format(message, a1);
	}
	template <typename T1, typename T2>
	void setMessageArguments(T1 a1, T2 a2)
	{
		lookupKeys();
		message = CvString::format(message, a1, a2);
	}
	template <typename T1, typename T2, typename T3>
	void setMessageArguments(T1 a1, T2 a2, T3 a3)
	{
		lookupKeys();
		message = CvString::format(message, a1, a2, a3);
	}
	template <typename T1, typename T2, typename T3, typename T4>
	void setMessageArguments(T1 a1, T2 a2, T3 a3, T4 a4)
	{
		lookupKeys();
		message = CvString::format(message, a1, a2, a3, a4);
	}
	template <typename T1, typename T2, typename T3, typename T4, typename T5>
	void setMessageArguments(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
	{
		lookupKeys();
		message = CvString::format(message, a1, a2, a3, a4, a5);
	}

protected:
	void lookupKeys();
	bool m_bKeysLookedup;
	int m_iDefaultButton;
	iconTypes m_Icon;
	Buttons m_Buttons;
};
