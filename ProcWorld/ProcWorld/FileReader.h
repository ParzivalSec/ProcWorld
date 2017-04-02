#pragma once
#include <string>

class FileReader
{
public:
	bool ReadFile(std::string fileName);
	std::string GetFileContent(void) const;

	bool IsValid(void) const;

private:
	std::string m_fileContent;
	bool		m_isValid;
};
