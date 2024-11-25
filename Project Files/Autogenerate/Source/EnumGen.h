#pragma once

#include <vector>

class EnumGen
{
public:
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
		};
		HardcodingClass();
		HardcodingClass(var);

		bool isAlwaysHardcoded() const;
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

	void writeFile();

private:
	void writeFile(bool bHardcoded);

	void writeCPP();
	void writeDefinesStart(class OutputHandler&, bool bHardcoded);
	void writeDefinesEnd(class OutputHandler&, bool bHardcoded);
	void writeEnum(class OutputHandler&, bool bHardcoded);
	

	std::vector<std::string> m_types;
	std::string m_name;
	std::string m_num;
	int m_iLength;
	HardcodingClass m_type;
};
