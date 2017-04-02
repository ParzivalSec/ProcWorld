#pragma once
#include <vector>

#include "Shader.h"

static const std::string SHADER_DIR = "/data/shader/";

class ShaderProgram {
public:
	template <typename ...Shaders>
	void AddShaders(GLenum type, std::string fileName, Shaders... shaders);
	void AddShaders(GLenum type, std::string fileName);

	void Link(void);
	void Reload(void);

	GLuint m_id;
	std::vector<Shader> m_shaders;
private:
	Shader CompileShader(GLenum type, std::string fileName);
};

template <typename ... Shaders>
void ShaderProgram::AddShaders(GLenum type, std::string fileName, Shaders... shaders) {
	AddShaders(type, fileName);
	AddShaders(shaders...);
}