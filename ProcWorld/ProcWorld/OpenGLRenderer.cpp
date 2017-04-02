#include "OpenGLRenderer.h"
#include <gtc/type_ptr.hpp>

GLuint OpenGLRenderer::currentShaderProgram = 0;

OpenGLRenderer::OpenGLRenderer()
{

}

void OpenGLRenderer::ViewPort(GLsizei x, GLsizei y, GLsizei width, GLsizei height)
{
	glViewport(x, y, width, height);
}

void OpenGLRenderer::CreateVertexBuffer(GLuint& VBO)
{
	glGenBuffers(1, &VBO);
}

void OpenGLRenderer::BindVertexBuffer(GLuint& VBO)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void OpenGLRenderer::LoadVertexBufferData(GLenum bufferType, size_t size, void* data, GLenum drawType)
{
	glBufferData(GL_ARRAY_BUFFER, size, data, drawType);
}

void OpenGLRenderer::CreateVertexArray(GLuint& VAO)
{
	glGenVertexArrays(1, &VAO);
}

void OpenGLRenderer::BindVertexArray(GLuint& VAO)
{
	glBindVertexArray(VAO);
}

void OpenGLRenderer::LinkVertexAttribute(GLuint attributeLocation, GLuint attributeSize,
	GLenum dataType, GLuint normalize, GLuint stride, GLvoid* positionOffset)
{
	glVertexAttribPointer(attributeLocation, attributeSize, dataType, normalize, stride, positionOffset);
}

void OpenGLRenderer::EnableVertexAttribute(GLuint attributeLocation)
{
	glEnableVertexAttribArray(attributeLocation);
}

void OpenGLRenderer::CreateElementBuffer(GLuint& EBO)
{
	glGenBuffers(1, &EBO);
}

void OpenGLRenderer::BindElementBuffer(GLuint& EBO)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

void OpenGLRenderer::LoadElementBufferData(GLenum bufferType, size_t size, void* data, GLenum drawType)
{
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, drawType);
}

void OpenGLRenderer::CreateFrameBuffer(GLuint& FBO)
{
	glGenFramebuffers(1, &FBO);
}

void OpenGLRenderer::BindFrameBuffer(GLuint& FBO)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void OpenGLRenderer::UnbindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderer::CreateVertexShader(GLuint& shaderId)
{
	shaderId = glCreateShader(GL_VERTEX_SHADER);
}

void OpenGLRenderer::CreateFragmentShader(GLuint& shaderId)
{
	shaderId = glCreateShader(GL_FRAGMENT_SHADER);
}

void OpenGLRenderer::CreateGeometryShader(GLuint& shaderId)
{
	shaderId = glCreateShader(GL_GEOMETRY_SHADER);
}

void OpenGLRenderer::AddShaderSource(GLuint shaderId, GLuint count, const GLchar* source)
{
	glShaderSource(shaderId, count, &source, nullptr);
}

void OpenGLRenderer::CompileShader(GLuint& shaderId)
{
	glCompileShader(shaderId);
}

void OpenGLRenderer::CreateShaderProgram(GLuint& programId)
{
	programId = glCreateProgram();
}

void OpenGLRenderer::AttachShader(GLuint& programId, GLuint& shaderId)
{
	glAttachShader(programId, shaderId);
}

void OpenGLRenderer::LinkProgram(GLuint& programId)
{
	glLinkProgram(programId);
}

void OpenGLRenderer::UseShader(GLuint programId)
{
	glUseProgram(programId);
	currentShaderProgram = programId;
}

void OpenGLRenderer::SetUniformFloat(GLuint shaderId, const char* attributeName, float value)
{
	GLuint attrLocation = glGetUniformLocation(shaderId, attributeName);
	glUniform1f(attrLocation, value);
}

void OpenGLRenderer::SetUniformBool(GLuint shaderId, const char* attributeName, bool value)
{
	GLuint attrLocation = glGetUniformLocation(shaderId, attributeName);
	glUniform1i(attrLocation, value);
}

void OpenGLRenderer::SetUniformVector2(GLuint shaderId, const char* attributeName, glm::vec2 vector)
{
	GLint attrLocation = glGetUniformLocation(shaderId, attributeName);
	glUniform2f(attrLocation, vector.x, vector.y);
}

void OpenGLRenderer::SetUniformVector3(GLuint shaderId, const char* attributeName, glm::vec3 vector)
{
	GLint attrLocation = glGetUniformLocation(shaderId, attributeName);
	glUniform3f(attrLocation, vector.x, vector.y, vector.z);
}

void OpenGLRenderer::SetUniformMatrix4fv(GLuint shaderId, const char* attributeName, glm::mat4 matrix)
{
	GLint attrLocation = glGetUniformLocation(shaderId, attributeName);
	glUniformMatrix4fv(attrLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLRenderer::DetachShader(GLuint& programId, GLuint& shaderId)
{
	glDetachShader(programId, shaderId);
}

void OpenGLRenderer::DeleteShader(GLuint& shaderId)
{
	glDeleteShader(shaderId);
}

void OpenGLRenderer::DeleteShaderProgram(GLuint& programId)
{
	glDeleteProgram(programId);
}

void OpenGLRenderer::CreateTexture2D(GLuint& textureId)
{
	glGenTextures(1, &textureId);
}

void OpenGLRenderer::BindTexture2D(GLuint& textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void OpenGLRenderer::UnbindTexture2D()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLRenderer::LoadTextureImage2D(GLint mipMapLevel, GLint format, GLsizei width, GLsizei height, GLint border, GLenum sourceFormat, GLenum sourceType, const void* pixels)
{
	glTexImage2D(GL_TEXTURE_2D, mipMapLevel, format, width, height,
		border, sourceFormat, sourceType, pixels);
}

void OpenGLRenderer::ClampParams2D(GLenum parameterName, GLint value)
{
	glTexParameteri(GL_TEXTURE_2D, parameterName, value);
}

void OpenGLRenderer::BorderColor2D(GLenum parameterName, const GLfloat* value)
{
	glTexParameterfv(GL_TEXTURE_2D, parameterName, value);
}

void OpenGLRenderer::MipMapFilters(GLenum minFilter, GLenum maxFilter)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
}

void OpenGLRenderer::GenerateMipMaps2D()
{
	glGenerateMipmap(GL_TEXTURE_2D);
}

void OpenGLRenderer::Draw(GLenum primitiveType, GLuint startPos, GLuint count)
{
	glDrawArrays(primitiveType, startPos, count);
}

void OpenGLRenderer::DrawIndexed(GLenum primitiveType, GLsizei count, GLenum type, const void* indices)
{
	glDrawElements(primitiveType, count, type, indices);
}

void OpenGLRenderer::RenderModel(GLuint& VAO, GLuint shaderProgram, const void* data)
{
	UseShader(shaderProgram);
	BindVertexArray(VAO);
	DrawIndexed(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
	UnbindVertexArray();
}

void OpenGLRenderer::Clear(GLbitfield mask)
{
	glClear(mask);
}

void OpenGLRenderer::ClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void OpenGLRenderer::EnableDepthTesting()
{
	glEnable(GL_DEPTH_TEST);
}

void OpenGLRenderer::DisableDepthTesting()
{
	glDisable(GL_DEPTH_TEST);
}

OpenGLRenderer::~OpenGLRenderer()
{

}

