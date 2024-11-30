#pragma once

#include "OutputHandler.h"

class EnumLengthSetupGen
{
public:
	EnumLengthSetupGen();

private:
	void setup(const class EnumGen&);

	void setDeclaration(const char* type, OutputHandler& file);
	void testLength(const char* type, OutputHandler& file);
	void setLength(const char* type, OutputHandler& file);


	OutputHandler file_include;
	OutputHandler file_always_declare;
	OutputHandler file_dynamic_declare;
	OutputHandler file_run_always;
	OutputHandler file_run_hardcoded;
	OutputHandler file_run_dynamic;
};