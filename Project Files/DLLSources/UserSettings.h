#pragma once
#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

class UserSettings : private boost::noncopyable
{
public:
	UserSettings();

	int getColonyRadius() const;
	int getDebugMaxGameFont() const;

protected:
	void read();
	void write();

	int m_iColonyRadius;
	int m_iDebugMaxGameFont;
};

#endif
