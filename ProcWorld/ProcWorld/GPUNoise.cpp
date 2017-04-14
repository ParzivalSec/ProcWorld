#include "GPUNoise.h"

#include "AssetManager.h"
#include "OpenGLRenderer.h"
#include <cstdio>

void GPUNoise::GenerateNoiseTextures(uint32_t textureDimension, AssetManager& assetManager)
{
	m_noiseDimension = textureDimension;

	glGenTextures(4, m_noiseTextures);

	for (size_t i = 0; i < 4; ++i) 
	{
		glBindTexture(GL_TEXTURE_3D, m_noiseTextures[i]);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, m_noiseDimension, m_noiseDimension, m_noiseDimension, 0, GL_RED, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}

	// Setup the shader
	SetupNoiseShader(assetManager);

	glGenFramebuffers(1, &m_noiseFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_noiseFBO);
	glBindTexture(GL_TEXTURE_3D, m_noiseTextures[0]);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_noiseTextures[0], 0);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	printf("++ Noise FBO: ");
	if (status == GL_FRAMEBUFFER_COMPLETE) {
		printf("FRAMEBUFFE OK!\n");
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
}

void GPUNoise::GeneratePerlinNoise(void) 
{
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


	OpenGLRenderer::BindFrameBuffer(m_noiseFBO);
	OpenGLRenderer::ViewPort(0, 0, m_noiseDimension, m_noiseDimension);

	OpenGLRenderer::UseShader(m_noiseShader);

	OpenGLRenderer::SetUniformFloat(m_noiseShader, "time", 0.001f);
	GLuint attrLocation = glGetUniformLocation(m_noiseShader, "octaves");
	glUniform1i(attrLocation, 1);
	OpenGLRenderer::SetUniformFloat(m_noiseShader, "lacunarity", 2.0f);

	OpenGLRenderer::BindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 30, m_noiseDimension);
	OpenGLRenderer::UnbindVertexArray();

	OpenGLRenderer::UnbindFrameBuffer();
}

void GPUNoise::SetupNoiseShader(AssetManager& assetManager) 
{
	ShaderProgram& m_cubes_prg = assetManager.AddShaderSet("Perlin_Noise_Texture");
	m_cubes_prg.AddShaders(GL_VERTEX_SHADER, "noise_texture.vert", 
						   GL_GEOMETRY_SHADER, "noise_texture.geom",
						   GL_FRAGMENT_SHADER, "noise_texture.frag");

	m_noiseShader = m_cubes_prg.m_id;
	m_cubes_prg.Link();
}
