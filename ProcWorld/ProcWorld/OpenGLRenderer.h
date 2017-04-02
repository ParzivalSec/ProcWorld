#ifndef _OPENGL_RENDERER_H_
#define _OPENGL_RENDERER_H_
#include <GL/glew.h>
#include <mat4x2.hpp>

class OpenGLRenderer
{
public:
	OpenGLRenderer();

	// General
	static void ViewPort(GLsizei x, GLsizei y, GLsizei width, GLsizei height);

	// VBO actions
	static void CreateVertexBuffer(GLuint& VBO);
	static void BindVertexBuffer(GLuint& VBO);
	static void LoadVertexBufferData(GLenum bufferType, size_t size, void* data, GLenum drawType);

	// VAO actions
	static void CreateVertexArray(GLuint& VAO);
	static void BindVertexArray(GLuint& VAO);
	static void UnbindVertexArray() { glBindVertexArray(0); }

	// Vertex attribute pointers
	static void LinkVertexAttribute(GLuint attributeLocation, GLuint attributeSize,
		GLenum dataType, GLuint normalize, GLuint stride,
		GLvoid* positionOffset);
	static void EnableVertexAttribute(GLuint attributeLocation);

	// EBO actions
	static void CreateElementBuffer(GLuint& EBO);
	static void BindElementBuffer(GLuint& EBO);
	static void LoadElementBufferData(GLenum bufferType, size_t size, void* data, GLenum drawType);

	// FrameBuffer actions
	static void CreateFrameBuffer(GLuint& FBO);
	static void BindFrameBuffer(GLuint& FBO);
	static void UnbindFrameBuffer(void);

	// Shader actions
	static void CreateVertexShader(GLuint& shaderId);
	static void CreateFragmentShader(GLuint& shaderId);
	static void CreateGeometryShader(GLuint& shaderId);

	static void AddShaderSource(GLuint shaderId, GLuint count, const GLchar* source);
	static void CompileShader(GLuint& shaderId);

	static void CreateShaderProgram(GLuint& programId);
	static void AttachShader(GLuint& programId, GLuint& shaderId);
	static void LinkProgram(GLuint& programId);

	static void UseShader(GLuint programId);

	static void SetUniformMatrix4fv(GLuint shaderId, const char* attributeName, glm::mat4 matrix);
	static void SetUniformFloat(GLuint shaderId, const char* attributeName, float value);
	static void SetUniformBool(GLuint shaderId, const char* attributeName, bool value);
	static void SetUniformVector2(GLuint shaderId, const char* attributeName, glm::vec2 vector);
	static void SetUniformVector3(GLuint shaderId, const char* attributeName, glm::vec3 vector);

	static void DetachShader(GLuint& programId, GLuint& shaderId);
	static void DeleteShader(GLuint& shaderId);

	static void DeleteShaderProgram(GLuint& programId);

	// Texture actions
	static void CreateTexture2D(GLuint& textureId);
	static void BindTexture2D(GLuint& textureId);
	static void UnbindTexture2D(void);

	static void LoadTextureImage2D(GLint mipMapLevel, GLint format,
		GLsizei width, GLsizei height, GLint border, GLenum sourceFormat,
		GLenum sourceType, const void* pixels);
	static void ClampParams2D(GLenum parameterName, GLint value);
	static void BorderColor2D(GLenum parameterName, const GLfloat* value);
	static void MipMapFilters(GLenum parameterName, GLenum minFilter);
	static void GenerateMipMaps2D(void);

	// Draw actions
	static void Draw(GLenum primitiveType, GLuint startPos, GLuint count);
	static void DrawIndexed(GLenum primitiveType, GLsizei count, GLenum type, const void* indices);
	static void RenderModel(GLuint& VAO, GLuint shaderProgram, const void* data);
	//
	static void Clear(GLbitfield mask);
	static void ClearColor(float r, float g, float b, float a);

	// Depth testing
	static void EnableDepthTesting();
	static void DisableDepthTesting();

	~OpenGLRenderer();

	static GLuint currentShaderProgram;
};

#endif // _OPENGL_RENDERER_H_
