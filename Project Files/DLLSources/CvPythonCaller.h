#pragma once

class Python
{
	friend class CvGlobals;
public:
	Python();
	static Python& call();

	void openXMLEditor();

private:
	CvDLLPythonIFaceBase& IF();
	CvDLLPythonIFaceBase* m_Python;
};
