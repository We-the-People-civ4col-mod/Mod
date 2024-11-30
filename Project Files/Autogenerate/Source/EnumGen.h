#pragma once

#include <vector>
#include "OutputHandler.h"
#include "LineString.h"

class EnumGen
{
public:
	class InfoSource
	{
	public:
		enum var
		{
			GC,
			INFO,
			ENUMMAP,
		};
		InfoSource();
		void assign(const char* newType);
		var getVar() const;
	private:
		var value;
	};

	class HardcodingClass
	{
	public:
		enum var
		{
			TYPE_STANDARD,
			TYPE_HARDCODED,
			TYPE_DYNAMIC,
			TYPE_NO_INFO,
			TYPE_NO_FILE,
			TYPE_PLAYER,
		};
		HardcodingClass();
		HardcodingClass(var);

		bool isAlwaysHardcoded() const;
		bool hasInfo() const;

		var getVar() const;

		void assign(const char* newType);

		HardcodingClass& operator = (const var rhs);
		HardcodingClass& operator = (const HardcodingClass rhs);

		bool operator == (const var rhs) const;
		bool operator == (const HardcodingClass rhs) const;
	protected:
		var value;
	};

	EnumGen(const char* name, int ilength, const char* num = NULL);
	EnumGen(class Element);

	static const std::vector<EnumGen>& getVector();
	static const EnumGen* getEntry(std::string name);

	const HardcodingClass type() const;

	const char* name() const;
	const char* num() const;
	int length() const;

	void generateFiles();

	static const std::string& getOverrideFile();
	static void createInitFunction();

private:
	void scanSourceFiles();


	void writeFile();

	void writeCPPStart();
	void writeCPP();
	void writeDefinesStart();
	void writeDefinesEnd();
	void writeEnum();
	
	void func_constructor();
	void func_constructor_type();
	void func_constructor_types();
	void func_fromInt();
	void func_toInt();
	void func_conversion_operator_value();
	void func_conversion_operator_enum();
	void func_conversion_operator_types();
	void func_range_static();
	void func_range();

	void func_info_static();
	void func_info();

	void func_getString_static();
	void func_getString();
	void func_assignFromString();
	void func_next();

	void func_setupAuto();
	void func_setupCustom();


	bool hasOverride_PrintIfNot(const char*);
	bool hasOverride_PrintIfNot(const char*, const char*);
	bool hasOverride_PrintIfNot(const char*, const char*, const char*);
	bool hasOverride_PrintIfNot(const char*, const char*, const char*, const char*);
	bool hasOverride_PrintIfNot(const char*, const char*, const char*, const char*, const char*);
	bool hasOverride_PrintIfNot(const char*, const char*, const char*, const char*, const char*, const char*);
	bool hasOverride_PrintIfNot(const char*, const char*, const char*, const char*, const char*, const char*, const char*);
	bool hasOverride_PrintIfNot(const char*, const char*, const char*, const char*, const char*, const char*, const char*, const char*);

	std::vector<std::string> m_types;
	LineString m_name;
	LineString m_nameType;
	LineString m_num;
	LineString m_InfoClass;
	LineString m_GetInfo;
	int m_iLength;
	HardcodingClass m_type;
	InfoSource m_InfoSource;
	bool m_bSecondRun;
	bool m_bHasOverrideFunctions;
	bool m_bHasNamedValues;

	OutputHandler file_cpp;
	OutputHandler file_header;

	std::vector<std::string> m_used_enum_values;
};
