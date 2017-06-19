#include "ShadowPass.h"
#include <gtc/matrix_transform.inl>
#include <iostream>
#include <gtc/type_ptr.inl>
#include "GeometryPass.h"
#include "Meshes.h"

ShadowPass::ShadowPass(float viewportWidth, float viewportHeight, float shadowMapScale, float blurMapScale)
	: m_shadowFBO(0)
	, m_blurFBO(0)
	, m_shadowMap(0)
	, m_shadowMapColorId(0)
	, m_blurColorTexId(0)
	, m_renderWidth(viewportWidth)
	, m_renderHeight(viewportHeight)
	, m_shadowMapScale(shadowMapScale)
	, m_blurMapScale(blurMapScale)
	, m_lightProjectionMatrix(glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 100.0f))
{
	SetupShadowFramebuffers();
	SetupBlurFramebuffers();

	m_fullScreenQuadVAO = meshes::GenerateQuad();
}

void ShadowPass::LoadShaders(AssetManager& assetManager)
{
	ShaderProgram& shadowPrg = assetManager.AddShaderSet("ShadowShader");
	shadowPrg.AddShaders(GL_VERTEX_SHADER, "VSMShadows.vert", GL_FRAGMENT_SHADER, "VSMShadows.frag");
	shadowPrg.Link();

	m_shadowShaderId = shadowPrg.m_id;

	ShaderProgram& blurPrg = assetManager.AddShaderSet("BlurShader");
	blurPrg.AddShaders(GL_VERTEX_SHADER, "GBlur7x1.vert", GL_FRAGMENT_SHADER, "GBlur7x1.frag");
	blurPrg.Link();

	m_blurShaderId = blurPrg.m_id;
}

void ShadowPass::Render(glm::vec3 lightPosition, glm::vec3 lightDirection, GLuint VAO, uint32_t count, const glm::mat4& modelmat)
{
	// Render from light view into the shadow map
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);
	glViewport(0, 0, m_renderWidth * m_shadowMapScale, m_renderHeight * m_shadowMapScale);

		//Calculate lightSpaceMatrix
		glm::mat4 lightViewMat = glm::lookAt(lightPosition,
			glm::vec3(0, 0, 0.0f),
			glm::vec3(0, 1, 0));

		glm::mat4 lightSpaceMat = m_lightProjectionMatrix * lightViewMat;
		// Draw all objects and render sillhout into shadow map
		glCullFace(GL_FRONT);

		glUseProgram(m_shadowShaderId);
		GLuint uniformLoc = glGetUniformLocation(m_shadowShaderId, "lightSpaceMatrix");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMat));
		uniformLoc = glGetUniformLocation(m_shadowShaderId, "model");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(modelmat));

		glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, count);
		glBindVertexArray(0);

	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowPass::ResetShadowMap()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);
		glViewport(0, 0, m_renderWidth * m_shadowMapScale, m_renderHeight * m_shadowMapScale);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void ShadowPass::BlurShadowMap()
{
	// *** GAUSSIAN - BLUR ***
	// Blur the shadow map
	glDisable(GL_DEPTH_TEST);
	glUseProgram(m_blurShaderId);

	glViewport(0, 0, m_renderWidth * m_shadowMapScale, m_renderHeight * m_shadowMapScale);

	// Blur horizontally and render to blurTexture
	glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO);

	// Assign texture to blur
	GLuint texLoc = glGetUniformLocation(m_blurShaderId, "toBlur");
	glUniform1i(texLoc, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_shadowMapColorId);

	GLuint uniformLoc = glGetUniformLocation(m_blurShaderId, "blurScale");
	glUniform2fv(uniformLoc, 1, glm::value_ptr(glm::vec2(1.0f / m_renderWidth * m_shadowMapScale * m_blurMapScale, 0)));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw fullscreen quad
	glBindVertexArray(m_fullScreenQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Blur vertically and render to shadowMapTex
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);

	texLoc = glGetUniformLocation(m_blurShaderId, "toBlur");
	glUniform1i(texLoc, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_blurColorTexId);

	uniformLoc = glGetUniformLocation(m_blurShaderId, "blurScale");
	glUniform2fv(uniformLoc, 1, glm::value_ptr(glm::vec2(0, 1.0f / m_renderHeight * m_shadowMapScale * m_blurMapScale)));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw fullscreen quad
	glBindVertexArray(m_fullScreenQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void ShadowPass::SetBlurFactor(float blur)
{
	m_blurMapScale = blur;
}

glm::mat4 ShadowPass::GetLightSpaceMatrix(glm::vec3 lightPosition)
{
	//Calculate lightSpaceMatrix
	glm::mat4 lightViewMat = glm::lookAt(lightPosition,
		glm::vec3(0, 0, 0.0f),
		glm::vec3(0, 1, 0));

	glm::mat4 lightSpaceMat = m_lightProjectionMatrix * lightViewMat;

	return lightSpaceMat;
}

void ShadowPass::SetupShadowFramebuffers(void)
{
	GLenum FBOstate;

	// Create depth texture component
	glGenTextures(1, &m_shadowMap);
	glBindTexture(GL_TEXTURE_2D, m_shadowMap);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_renderWidth * m_shadowMapScale,
		m_renderHeight * m_shadowMapScale, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create color texture component
	glGenTextures(1, &m_shadowMapColorId);
	glBindTexture(GL_TEXTURE_2D, m_shadowMapColorId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, m_renderWidth * m_shadowMapScale,
		m_renderHeight * m_shadowMapScale, 0, GL_RG, GL_FLOAT, nullptr);
	//glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create Shadow FBO
	glGenFramebuffers(1, &m_shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);

	// attach texture to FBO depth attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_shadowMapColorId, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// check the FBOs state
	FBOstate = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (FBOstate != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[ERROR::FBO::VSM] - Creating shadowmap FBO failed, cannot be used" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowPass::SetupBlurFramebuffers(void)
{
	GLenum FBOstate;

	// Create depth texture component (later maybe need a color texture component too)
	glGenTextures(1, &m_blurColorTexId);
	glBindTexture(GL_TEXTURE_2D, m_blurColorTexId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, m_renderWidth * m_shadowMapScale,
		m_renderHeight * m_shadowMapScale, 0, GL_RG, GL_FLOAT, nullptr);

	// Create Shadow FBO
	glGenFramebuffers(1, &m_blurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO);

	// attach texture to FBO depth attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurColorTexId, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// check the FBOs state
	FBOstate = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (FBOstate != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[ERROR::FBO::VSM] - Creating blurmap FBO failed, cannot be used" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
