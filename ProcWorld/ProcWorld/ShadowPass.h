#pragma once
#include <GL/glew.h>
#include <glm.hpp>

#include "AssetManager.h"

class GeometryPass;

class ShadowPass
{
public:
	ShadowPass(float viewportWidth, float viewportHeight, float shadowMapScale, float blurMapScale);

	void LoadShaders(AssetManager& assetManager);
	void Render(glm::vec3 lightPosition, glm::vec3 lightDirection, GLuint VAO, uint32_t count, const glm::mat4& modelmat);
	void ResetShadowMap(void);
	void BlurShadowMap(void);

	GLuint GetShadowMapTextureID(void) const { return m_shadowMapColorId; }
	glm::mat4 GetLightSpaceMatrix(glm::vec3 lightPosition);

private:

	void SetupShadowFramebuffers(void);
	void SetupBlurFramebuffers(void);

	GLuint m_shadowShaderId, m_blurShaderId;

	GLuint m_shadowFBO, m_blurFBO;
	GLuint m_shadowMap, m_shadowMapColorId, m_blurColorTexId;
	GLuint m_fullScreenQuadVAO;
	float m_renderWidth, m_renderHeight;
	float m_shadowMapScale, m_blurMapScale;
	glm::mat4 m_lightProjectionMatrix;
};
