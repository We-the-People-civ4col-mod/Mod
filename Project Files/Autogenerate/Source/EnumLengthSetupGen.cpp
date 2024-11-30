#include "EnumLengthSetupGen.h"
#include "EnumGen.h"

EnumLengthSetupGen::EnumLengthSetupGen()
{
	file_include.printLine("#include \"../EnumLengthSet.h\"");

	file_run_always.changeIndent(1);
	file_run_hardcoded.changeIndent(1);
	file_run_dynamic.changeIndent(1);

	int i = 0;

	const std::vector<EnumGen>& vec = EnumGen::getVector();
	//for (std::vector<EnumGen>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	const unsigned int iMax = vec.size();
	
	for (unsigned int i = 0; i < iMax; ++i)
	{
		//setup(*it);
		//++i;
		setup(vec[i]);
	}

	file_include.printLine();
	file_include.printLine(file_always_declare.getText());
	file_include.printLineNoIndent("#ifndef HARDCODE_XML_VALUES");
	file_include.printLineNoIndent(file_dynamic_declare.getText());
	file_include.printLineNoIndent("#endif");
	file_include.printLine();
	file_include.printLine("void EnumLengthSet::setup()");
	file_include.addStartBracket();
	file_include.printLineNoIndent(file_run_always.getText());
	file_include.printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
	file_include.printLineNoIndent(file_run_hardcoded.getText());
	file_include.printLineNoIndent("#else");
	file_include.printLineNoIndent(file_run_dynamic.getText());
	file_include.printLineNoIndent("#endif");
	file_include.addEndBracket();
	file_include.printLine();

	file_include.saveFile("EnumLengthSet.cpp");
}

void EnumLengthSetupGen::setup(const class EnumGen& info)
{
	if (info.type() == EnumGen::HardcodingClass::TYPE_NO_FILE)
	{
		return;
	}
	const char* type = info.name();

	file_include.printLine("#include \"AUTO_", type, "Type.h\"");

	switch (info.type().getVar())
	{
	case EnumGen::HardcodingClass::TYPE_STANDARD:
		setDeclaration(type, file_dynamic_declare);
		setLength(type, file_run_dynamic);
		testLength(type, file_run_hardcoded);
		break;
	case EnumGen::HardcodingClass::TYPE_DYNAMIC:
		setDeclaration(type, file_always_declare);
		setLength(type, file_run_always);
		break;
	case EnumGen::HardcodingClass::TYPE_HARDCODED:
	case EnumGen::HardcodingClass::TYPE_NO_INFO:
		testLength(type, file_run_always);
		break;
	};
}

void EnumLengthSetupGen::setDeclaration(const char* type, OutputHandler& file)
{
	file.printLine(type, "Type::types ", type, "_LOCAL_NUM;");
	file.printLine("const ", type, "Type::types& ", type, "Type::NUM_ENTRIES = ", type, "_LOCAL_NUM;");
}

void EnumLengthSetupGen::testLength(const char* type, OutputHandler& file)
{
	// void verifyTypeLength(const char* type, int iLength) const;

	file.printLine("verifyTypeLength(\"", type, "Types\", ", type, "Type::NUM_ENTRIES);");
}

void EnumLengthSetupGen::setLength(const char* type, OutputHandler& file)
{
	// unsigned int getTypeLength(const char* type) const;
	file.printLine(type, "_LOCAL_NUM = static_cast<", type, "Type::types>(getTypeLength(\"", type, "Types\"));");
}
