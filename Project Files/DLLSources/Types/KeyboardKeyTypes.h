#pragma once

class KeyboardKeyTypes
{
	friend class CvXMLLoadUtility;
public:
	enum types
	{

		// match values from FInputDevice for exe compatibility
		NONE = -1,

		KB_ESCAPE = 1,
		KB_0,
		KB_1,
		KB_2,
		KB_3,
		KB_4,
		KB_5,
		KB_6,
		KB_7,
		KB_8,
		KB_9,
		KB_MINUS,	    /* - on main keyboard */
		KB_A,
		KB_B,
		KB_C,
		KB_D,
		KB_E,
		KB_F,
		KB_G,
		KB_H,
		KB_I,
		KB_J,
		KB_K,
		KB_L,
		KB_M,
		KB_N,
		KB_O,
		KB_P,
		KB_Q,
		KB_R,
		KB_S,
		KB_T,
		KB_U,
		KB_V,
		KB_W,
		KB_X,
		KB_Y,
		KB_Z,
		KB_EQUALS,
		KB_BACKSPACE,
		KB_TAB,
		KB_LBRACKET,
		KB_RBRACKET,
		KB_RETURN,		/* Enter on main keyboard */
		KB_LCONTROL,
		KB_SEMICOLON,
		KB_APOSTROPHE,
		KB_GRAVE,		/* accent grave */
		KB_LSHIFT,
		KB_BACKSLASH,
		KB_COMMA,
		KB_PERIOD,
		KB_SLASH,
		KB_RSHIFT,
		KB_NUMPADSTAR,
		KB_LALT,
		KB_SPACE,
		KB_CAPSLOCK,
		KB_F1,
		KB_F2,
		KB_F3,
		KB_F4,
		KB_F5,
		KB_F6,
		KB_F7,
		KB_F8,
		KB_F9,
		KB_F10,
		KB_F11,
		KB_F12,
		KB_NUMLOCK,
		KB_SCROLL,
		KB_NUMPAD0,
		KB_NUMPAD1,
		KB_NUMPAD2,
		KB_NUMPAD3,
		KB_NUMPAD4,
		KB_NUMPAD5,
		KB_NUMPAD6,
		KB_NUMPAD7,
		KB_NUMPAD8,
		KB_NUMPAD9,
		KB_NUMPADMINUS,
		KB_NUMPADPLUS,
		KB_NUMPADPERIOD,
		KB_NUMPADEQUALS,
		KB_AT,
		KB_UNDERLINE,
		KB_COLON,
		KB_NUMPADENTER,
		KB_RCONTROL,
		KB_VOLUMEDOWN,
		KB_VOLUMEUP,
		KB_NUMPADCOMMA,
		KB_NUMPADSLASH,
		KB_SYSRQ,
		KB_RALT,
		KB_PAUSE,
		KB_HOME,
		KB_UP,
		KB_PGUP,
		KB_LEFT,
		KB_RIGHT,
		KB_END,
		KB_DOWN,
		KB_PGDN,
		KB_INSERT,
		KB_DELETE,
	};


	KeyboardKeyTypes();
	KeyboardKeyTypes(types);
	types value() const;

	void assignFromString(const char* szKey);
	const char* getString() const;

	CvWString getReadableText() const;

	static const char* getKeyAtIndex(int iIndex);

	bool operator==(types rhs) const;
	bool operator!=(types rhs) const;

protected:
	types data;
};
