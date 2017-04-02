#include "FileReader.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool FileReader::ReadFile(std::string fileName)
{
	std::ifstream inputFile;
	inputFile.exceptions(std::ifstream::badbit);

	try
	{
		inputFile.open(fileName);
		std::stringstream fileContent;
		fileContent << inputFile.rdbuf();
		// close file handlers
		inputFile.close();
		// convert stringstream to strings
		m_fileContent = fileContent.str();
		m_isValid = true;
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::FILEREADER::FILL_NOT_SUCCESSFULLY_READ" << std::endl;
		m_isValid = false;
	}

	return m_isValid;
}

std::string FileReader::GetFileContent(void) const
{
	if (m_isValid)
	{
		return m_fileContent;
	}

	return "";
}

bool FileReader::IsValid(void) const
{
	return m_isValid;
}
