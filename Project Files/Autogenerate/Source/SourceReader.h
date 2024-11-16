#pragma once

#include <vector>
#include <string>

class SourceFileContainer
{
	friend class SourceFileList;
public:
	const std::string& getText() const;

private:
	SourceFileContainer(std::string fileWName);
	void setup();

	std::string m_filename;
	std::string m_fileAsString;
};

class SourceFileList
{
public:
	SourceFileList();

	static const SourceFileList& getInstance();

	const std::vector<SourceFileContainer>& getFiles() const;

private:
	void readDir(std::string path);

	std::vector<SourceFileContainer> m_files;
};
