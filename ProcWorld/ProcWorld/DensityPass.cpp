#include "DensityPass.h"
#include "OpenGLRenderer.h"

DensityPass::DensityPass(size_t width, size_t height, size_t depth) 
	: m_densityTexture({0, width, height, depth})
	, m_densityFBO(0)
{}

void DensityPass::CreateDensityTexture(void) 
{
	glGenTextures(1, &m_densityTexture.textureID);
	glBindTexture(GL_TEXTURE_3D, m_densityTexture.textureID);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, m_densityTexture.width, m_densityTexture.height, m_densityTexture.depth, 0, GL_RED, GL_HALF_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

void DensityPass::FillDensityTexture(const ShaderProgram& shader, uint32_t noise_texture)
{
	glGenFramebuffers(1, &m_densityFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_densityFBO);
	// Bind the whole 3D texture to the framebuffer, decide what slice to render via Instanced Drawing
	// and gl_Layer in the GS
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_densityTexture.textureID, 0);
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status == GL_FRAMEBUFFER_COMPLETE) {
		// printf("FRAMEBUFFE OK!\n");
	}
	else if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) 
	{
		printf("!! FRAMEBUFFE ATTACHMENT INVALID !!\n");
	}
	else if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)
	{
		printf("!! FRAMEBUFFE DIMENSIONS INVALID !!\n");
	}
	else if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
	{
		printf("!! FRAMEBUFFE MISSING ATTACHMENT !!\n");
	}
	else if (status == GL_FRAMEBUFFER_UNSUPPORTED)
	{
		printf("!! FRAMEBUFFE UNSUPPORTED !!\n");
	}

	// INFO: Instanced draw a full-screen quad 256 times (one 2D texture per volume slice)
	GLfloat full_screen_quad[30] = 
	{
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	};
	GLuint VBO, VAO;

	OpenGLRenderer::CreateVertexArray(VAO);
	OpenGLRenderer::CreateVertexBuffer(VBO);

	OpenGLRenderer::BindVertexArray(VAO);
	OpenGLRenderer::BindVertexBuffer(VBO);
	OpenGLRenderer::LoadVertexBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), &full_screen_quad, GL_STATIC_DRAW);

	OpenGLRenderer::EnableVertexAttribute(0);
	OpenGLRenderer::LinkVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);

	OpenGLRenderer::EnableVertexAttribute(1);
	OpenGLRenderer::LinkVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
	OpenGLRenderer::UnbindVertexArray();


	OpenGLRenderer::BindFrameBuffer(m_densityFBO);
	OpenGLRenderer::ViewPort(0, 0, 96, 96);
	//OpenGLRenderer::Clear(GL_COLOR_BUFFER_BIT);
		
	OpenGLRenderer::UseShader(shader.m_id);

	GLint perlin_noise_loc = glGetUniformLocation(shader.m_id, "perlin_noise");
	glUniform1i(perlin_noise_loc, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, noise_texture);

	OpenGLRenderer::BindVertexArray(VAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 30, 256);
	OpenGLRenderer::UnbindVertexArray();

	OpenGLRenderer::UnbindFrameBuffer();
}
