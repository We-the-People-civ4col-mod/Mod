#include "../CvGameCoreDLL.h"
#include "KeyboardKeyTypes.h"


KeyboardKeyTypes::KeyboardKeyTypes()
	: data(NONE)
{

}KeyboardKeyTypes::KeyboardKeyTypes(types eKey)
	: data(eKey)
{
}

KeyboardKeyTypes::types KeyboardKeyTypes::value() const
{
	return data;
}

void KeyboardKeyTypes::assignFromString(const char* szKey)
{
	data = NONE;

	if (strcmp(szKey, "") == 0 || strcmp(szKey, "NONE") == 0)
	{
		return;
	}

	// use of magic numbers from vanilla. This won't change as it is hardcoded in the exe
	for (int i = 1; i <= 108; ++i)
	{
		if (strcmp(szKey, getKeyAtIndex(i)) == 0)
		{
			data = static_cast<types>(i);
			return;
		}
	}
	FAssertMsg(false, CvString::format("Key %s is not a valid KB_ key", szKey).c_str());
}

const char* KeyboardKeyTypes::getString() const
{
	return getKeyAtIndex(data);
}

CvWString KeyboardKeyTypes::getReadableText() const
{
	switch (data)
	{
	case KB_ESCAPE      : return gDLL->getText("TXT_KEY_KEYBOARD_ESCAPE");
	case KB_0           : return "0";
	case KB_1           : return "1";
	case KB_2           : return "2";
	case KB_3           : return "3";
	case KB_4           : return "4";
	case KB_5           : return "5";
	case KB_6           : return "6";
	case KB_7           : return "7";
	case KB_8           : return "8";
	case KB_9           : return "9";
	case KB_MINUS       : return "-";
	case KB_A           : return "A";
	case KB_B           : return "B";
	case KB_C           : return "C";
	case KB_D           : return "D";
	case KB_E           : return "E";
	case KB_F           : return "F";
	case KB_G           : return "G";
	case KB_H           : return "H";
	case KB_I           : return "I";
	case KB_J           : return "J";
	case KB_K           : return "K";
	case KB_L           : return "L";
	case KB_M           : return "M";
	case KB_N           : return "N";
	case KB_O           : return "O";
	case KB_P           : return "P";
	case KB_Q           : return "Q";
	case KB_R           : return "R";
	case KB_S           : return "S";
	case KB_T           : return "T";
	case KB_U           : return "U";
	case KB_V           : return "V";
	case KB_W           : return "W";
	case KB_X           : return "X";
	case KB_Y           : return "Y";
	case KB_Z           : return "Z";
	case KB_EQUALS      : return "=";
	case KB_BACKSPACE   : return gDLL->getText("TXT_KEY_KEYBOARD_BACKSPACE");
	case KB_TAB         : return "TAB";
	case KB_LBRACKET    : return "[";
	case KB_RBRACKET    : return "]";
	case KB_RETURN      : return gDLL->getText("TXT_KEY_KEYBOARD_ENTER");
	case KB_LCONTROL    : return gDLL->getText("TXT_KEY_KEYBOARD_LEFT_CONTROL_KEY");
	case KB_SEMICOLON   : return ";";
	case KB_APOSTROPHE  : return "'";
	case KB_GRAVE       : return "`";
	case KB_LSHIFT      : return gDLL->getText("TXT_KEY_KEYBOARD_LEFT_SHIFT_KEY");
	case KB_BACKSLASH   : return "\\";
	case KB_COMMA       : return ",";
	case KB_PERIOD      : return ".";
	case KB_SLASH       : return "/";
	case KB_RSHIFT      : return gDLL->getText("TXT_KEY_KEYBOARD_RIGHT_SHIFT_KEY");
	case KB_NUMPADSTAR  : return gDLL->getText("TXT_KEY_KEYBOARD_NUM_PAD_STAR");
	case KB_LALT        : return gDLL->getText("TXT_KEY_KEYBOARD_LEFT_ALT_KEY");
	case KB_SPACE       : return gDLL->getText("TXT_KEY_KEYBOARD_SPACE_KEY");
	case KB_CAPSLOCK    : return gDLL->getText("TXT_KEY_KEYBOARD_CAPS_LOCK");
	case KB_F1          : return "F1";
	case KB_F2          : return "F2";
	case KB_F3          : return "F3";
	case KB_F4          : return "F4";
	case KB_F5          : return "F5";
	case KB_F6          : return "F6";
	case KB_F7          : return "F7";
	case KB_F8          : return "F8";
	case KB_F9          : return "F9";
	case KB_F10         : return "F10";
	case KB_F11         : return "F11";
	case KB_F12         : return "F12";
	case KB_NUMLOCK     : return gDLL->getText("TXT_KEY_KEYBOARD_NUM_LOCK");
	case KB_SCROLL      : return gDLL->getText("TXT_KEY_KEYBOARD_SCROLL_KEY");
	case KB_NUMPAD0     : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_NUMBER", 0);
	case KB_NUMPAD1     : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_NUMBER", 1);
	case KB_NUMPAD2     : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_NUMBER", 2);
	case KB_NUMPAD3     : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_NUMBER", 3);
	case KB_NUMPAD4     : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_NUMBER", 4);
	case KB_NUMPAD5     : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_NUMBER", 5);
	case KB_NUMPAD6     : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_NUMBER", 6);
	case KB_NUMPAD7     : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_NUMBER", 7);
	case KB_NUMPAD8     : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_NUMBER", 8);
	case KB_NUMPAD9     : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_NUMBER", 9);
	case KB_NUMPADMINUS : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_MINUS");
	case KB_NUMPADPLUS  : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_PLUS");
	case KB_NUMPADPERIOD: return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_PERIOD");
	case KB_NUMPADEQUALS: return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_EQUALS");
	case KB_AT          : return "@";
	case KB_UNDERLINE   : return "_";
	case KB_COLON       : return ":";
	case KB_NUMPADENTER : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_ENTER_KEY");
	case KB_RCONTROL    : return gDLL->getText("TXT_KEY_KEYBOARD_RIGHT_CONTROL_KEY");
	case KB_VOLUMEDOWN  : return gDLL->getText("TXT_KEY_KEYBOARD_VOLUME_DOWN");
	case KB_VOLUMEUP    : return gDLL->getText("TXT_KEY_KEYBOARD_VOLUME_UP");
	case KB_NUMPADCOMMA : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_COMMA");
	case KB_NUMPADSLASH : return gDLL->getText("TXT_KEY_KEYBOARD_NUMPAD_SLASH");
	case KB_SYSRQ       : return gDLL->getText("TXT_KEY_KEYBOARD_SYSRQ");
	case KB_RALT        : return gDLL->getText("TXT_KEY_KEYBOARD_RIGHT_ALT_KEY");
	case KB_PAUSE       : return gDLL->getText("TXT_KEY_KEYBOARD_PAUSE_KEY");
	case KB_HOME        : return gDLL->getText("TXT_KEY_KEYBOARD_HOME_KEY");
	case KB_UP          : return gDLL->getText("TXT_KEY_KEYBOARD_UP_ARROW");
	case KB_PGUP        : return gDLL->getText("TXT_KEY_KEYBOARD_PAGE_UP");
	case KB_LEFT        : return gDLL->getText("TXT_KEY_KEYBOARD_LEFT_ARROW");
	case KB_RIGHT       : return gDLL->getText("TXT_KEY_KEYBOARD_RIGHT_ARROW");
	case KB_END         : return gDLL->getText("TXT_KEY_KEYBOARD_END_KEY");
	case KB_DOWN        : return gDLL->getText("TXT_KEY_KEYBOARD_DOWN_ARROW");
	case KB_PGDN        : return gDLL->getText("TXT_KEY_KEYBOARD_PAGE_DOWN");
	case KB_INSERT      : return gDLL->getText("TXT_KEY_KEYBOARD_INSERT_KEY");
	case KB_DELETE      : return gDLL->getText("TXT_KEY_KEYBOARD_DELETE_KEY");
	default: return L"";
	};
}

const char* KeyboardKeyTypes::getKeyAtIndex(int iIndex)
{
	switch (iIndex)
	{
	case KB_ESCAPE      : return "KB_ESCAPE";
	case KB_0           : return "KB_0";
	case KB_1           : return "KB_1";
	case KB_2           : return "KB_2";
	case KB_3           : return "KB_3";
	case KB_4           : return "KB_4";
	case KB_5           : return "KB_5";
	case KB_6           : return "KB_6";
	case KB_7           : return "KB_7";
	case KB_8           : return "KB_8";
	case KB_9           : return "KB_9";
	case KB_MINUS       : return "KB_MINUS";
	case KB_A           : return "KB_A";
	case KB_B           : return "KB_B";
	case KB_C           : return "KB_C";
	case KB_D           : return "KB_D";
	case KB_E           : return "KB_E";
	case KB_F           : return "KB_F";
	case KB_G           : return "KB_G";
	case KB_H           : return "KB_H";
	case KB_I           : return "KB_I";
	case KB_J           : return "KB_J";
	case KB_K           : return "KB_K";
	case KB_L           : return "KB_L";
	case KB_M           : return "KB_M";
	case KB_N           : return "KB_N";
	case KB_O           : return "KB_O";
	case KB_P           : return "KB_P";
	case KB_Q           : return "KB_Q";
	case KB_R           : return "KB_R";
	case KB_S           : return "KB_S";
	case KB_T           : return "KB_T";
	case KB_U           : return "KB_U";
	case KB_V           : return "KB_V";
	case KB_W           : return "KB_W";
	case KB_X           : return "KB_X";
	case KB_Y           : return "KB_Y";
	case KB_Z           : return "KB_Z";
	case KB_EQUALS      : return "KB_EQUALS";
	case KB_BACKSPACE   : return "KB_BACKSPACE";
	case KB_TAB         : return "KB_TAB";
	case KB_LBRACKET    : return "KB_LBRACKET";
	case KB_RBRACKET    : return "KB_RBRACKET";
	case KB_RETURN      : return "KB_RETURN";
	case KB_LCONTROL    : return "KB_LCONTROL";
	case KB_SEMICOLON   : return "KB_SEMICOLON";
	case KB_APOSTROPHE  : return "KB_APOSTROPHE";
	case KB_GRAVE       : return "KB_GRAVE";
	case KB_LSHIFT      : return "KB_LSHIFT";
	case KB_BACKSLASH   : return "KB_BACKSLASH";
	case KB_COMMA       : return "KB_COMMA";
	case KB_PERIOD      : return "KB_PERIOD";
	case KB_SLASH       : return "KB_SLASH";
	case KB_RSHIFT      : return "KB_RSHIFT";
	case KB_NUMPADSTAR  : return "KB_NUMPADSTAR";
	case KB_LALT        : return "KB_LALT";
	case KB_SPACE       : return "KB_SPACE";
	case KB_CAPSLOCK    : return "KB_CAPSLOCK";
	case KB_F1          : return "KB_F1";
	case KB_F2          : return "KB_F2";
	case KB_F3          : return "KB_F3";
	case KB_F4          : return "KB_F4";
	case KB_F5          : return "KB_F5";
	case KB_F6          : return "KB_F6";
	case KB_F7          : return "KB_F7";
	case KB_F8          : return "KB_F8";
	case KB_F9          : return "KB_F9";
	case KB_F10         : return "KB_F10";
	case KB_F11         : return "KB_F11";
	case KB_F12         : return "KB_F12";
	case KB_NUMLOCK     : return "KB_NUMLOCK";
	case KB_SCROLL      : return "KB_SCROLL";
	case KB_NUMPAD0     : return "KB_NUMPAD0";
	case KB_NUMPAD1     : return "KB_NUMPAD1";
	case KB_NUMPAD2     : return "KB_NUMPAD2";
	case KB_NUMPAD3     : return "KB_NUMPAD3";
	case KB_NUMPAD4     : return "KB_NUMPAD4";
	case KB_NUMPAD5     : return "KB_NUMPAD5";
	case KB_NUMPAD6     : return "KB_NUMPAD6";
	case KB_NUMPAD7     : return "KB_NUMPAD7";
	case KB_NUMPAD8     : return "KB_NUMPAD8";
	case KB_NUMPAD9     : return "KB_NUMPAD9";
	case KB_NUMPADMINUS : return "KB_NUMPADMINUS";
	case KB_NUMPADPLUS  : return "KB_NUMPADPLUS";
	case KB_NUMPADPERIOD: return "KB_NUMPADPERIOD";
	case KB_NUMPADEQUALS: return "KB_NUMPADEQUALS";
	case KB_AT          : return "KB_AT";
	case KB_UNDERLINE   : return "KB_UNDERLINE";
	case KB_COLON       : return "KB_COLON";
	case KB_NUMPADENTER : return "KB_NUMPADENTER";
	case KB_RCONTROL    : return "KB_RCONTROL";
	case KB_VOLUMEDOWN  : return "KB_VOLUMEDOWN";
	case KB_VOLUMEUP    : return "KB_VOLUMEUP";
	case KB_NUMPADCOMMA : return "KB_NUMPADCOMMA";
	case KB_NUMPADSLASH : return "KB_NUMPADSLASH";
	case KB_SYSRQ       : return "KB_SYSRQ";
	case KB_RALT        : return "KB_RALT";
	case KB_PAUSE       : return "KB_PAUSE";
	case KB_HOME        : return "KB_HOME";
	case KB_UP          : return "KB_UP";
	case KB_PGUP        : return "KB_PGUP";
	case KB_LEFT        : return "KB_LEFT";
	case KB_RIGHT       : return "KB_RIGHT";
	case KB_END         : return "KB_END";
	case KB_DOWN        : return "KB_DOWN";
	case KB_PGDN        : return "KB_PGDN";
	case KB_INSERT      : return "KB_INSERT";
	case KB_DELETE      : return "KB_DELETE";
	default: return "";
	};
}

bool KeyboardKeyTypes::operator==(KeyboardKeyTypes::types rhs) const
{
	return data == rhs;
}

bool KeyboardKeyTypes::operator!=(KeyboardKeyTypes::types rhs) const
{
	return data != rhs;
}
