#pragma once

class CityPlotType
{
	friend class CvXMLLoadUtility;
public:
	enum types
	{
		NONE = -1,
#ifdef DEBUG
		CITYPLOT_0,
		CITYPLOT_1,
		CITYPLOT_2,
		CITYPLOT_3,
		CITYPLOT_4,
		CITYPLOT_5,
		CITYPLOT_6,
		CITYPLOT_7,
		CITYPLOT_8,
		CITYPLOT_9,
		CITYPLOT_10,
		CITYPLOT_11,
		CITYPLOT_12,
		CITYPLOT_13,
		CITYPLOT_14,
		CITYPLOT_15,
		CITYPLOT_16,
		CITYPLOT_17,
		CITYPLOT_18,
		CITYPLOT_19,
		CITYPLOT_20,
		CITYPLOT_21,
		CITYPLOT_22,
		CITYPLOT_23,
		CITYPLOT_24,
#endif
		NUM_ENTRIES = 25,
	};
	
	CityPlotType();
	CityPlotType(types);
	CityPlotType(enum CityPlotTypes);

	void assignFromInt(int iNewValue);
	int toInt() const;
	const types value() const;
	operator const types() const;
	operator const CityPlotTypes() const;


	static bool isInRange(types eValue);
	bool isInRange() const;

	bool next();

private:
	static void setupAuto();
	static void setupCustom();

	types m_Value;
};
