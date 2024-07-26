#pragma once

class ActionTypes
{
	friend class CvXMLLoadUtility;
public:
	enum types
	{
		NONE = -1,
	};

	static const types& NUM;

	ActionTypes();
	ActionTypes(types type);
	types value() const;

	bool isValid() const;
	bool next();

	const CvActionInfo& info() const;

	static ActionTypes createFromInt(int iIndex);

protected:
	types data;
	static types m_num;
	static void initLength();
};
