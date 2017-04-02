#include "ShaderProgram.h"
#include <iostream>
#include <filesystem>
#include "FileReader.h"

void ShaderProgram::AddShaders(GLenum type, std::string fileName) {
	m_shaders.push_back(CompileShader(type, fileName));
}

void ShaderProgram::Link(void) {
	m_id = glCreateProgram();

	GLint isLinked = 0;
	for (const Shader& shader : m_shaders) {
		glAttachShader(m_id, shader.id);
	}

	glLinkProgram(m_id);

	glGetProgramiv(m_id, GL_LINK_STATUS, &isLinked);

	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(m_id);

		for (const Shader& shader : m_shaders) {
			glDeleteShader(shader.id);
		}

		std::cout << "!!!!!!!!!!!!!!!" << std::endl;
		std::cout << "ERROR::SHADER::LINK => See shader info log for more information!" << std::endl;
		std::cout << static_cast<const char*>(&infoLog[0]) << std::endl;
		std::cout << "!!!!!!!!!!!!!!!" << std::endl;
	} else {
		std::cout << "Linked without error" << std::endl;
	}
}

void ShaderProgram::Reload(void) {
	glDeleteProgram(m_id);
	std::cout << "Reload triggered" << std::endl;

	for (size_t i = 0; i < m_shaders.size(); ++i) {
		Shader new_shader = CompileShader(m_shaders[i].type, m_shaders[i].fileName);
		m_shaders[i].isDirty = false;
		if (new_shader.id > 0) {
			m_shaders[i] = new_shader;
		}
	}

	Link();
}

Shader ShaderProgram::CompileShader(GLenum type, std::string fileName) {
	namespace fs = std::experimental::filesystem;
	
	Shader result;
	result.fileName = fileName;
	result.type = type;

	// TODO: Load shader sourc
	FileReader fileReader;
	fs::path shaderFile = (fs::current_path() += SHADER_DIR) /= fileName;
	std::cout << "Compiling : " << shaderFile << std::endl;
	fileReader.ReadFile(shaderFile.generic_string());
	std::string fileContent = fileReader.GetFileContent();
	const GLchar* shaderSource = fileContent.c_str();

	result.id = glCreateShader(type);
	glShaderSource(result.id, 1, &shaderSource, nullptr);
	glCompileShader(result.id);

	GLint compiled = GL_FALSE;
	glGetShaderiv(result.id, GL_COMPILE_STATUS, static_cast<int*>(&compiled));

	if (compiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(result.id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(result.id, maxLength, &maxLength, &infoLog[0]);

		glDeleteShader(result.id);

		std::cout << "!!!!!!!!!!!!!!!" << std::endl;
		std::cout << "ERROR::SHADER::COMPILATION => See shader info log for more information!" << std::endl;
		std::cout << static_cast<const char*>(&infoLog[0]) << std::endl;
		std::cout << "!!!!!!!!!!!!!!!" << std::endl;

		result.id = -1;
	} else {
		std::cout << "Compiled Shader wihtout error" << std::endl;
	}

	return result;
}
