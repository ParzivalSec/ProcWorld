#include "TesselationPass.h"
#include "Box.h"

#include <gtc/matrix_transform.inl>
#include "OpenGLRenderer.h"

TesselationPass::TesselationPass(glm::vec3 cubePosition, glm::vec3 cubeScale)
	: m_cubeVAO(0)
	, m_modelMat(glm::mat4(1.0f))
{
	m_modelMat = glm::translate(m_modelMat, cubePosition);
	m_modelMat = glm::scale(m_modelMat, cubeScale);

	m_cubeVAO = Box::CreateCubeVAO();
}

void TesselationPass::SetupResources(AssetManager& assetManager)
{
	ShaderProgram* shader =  assetManager.GetShaderByName("StandardShader");
	assert(shader != nullptr);

	m_standardShaderPrg = shader->m_id;

	// Load tesselation displacement shader without PN-AEN or Dominant-UVs
	ShaderProgram& tessDisplacement = assetManager.AddShaderSet("TesselationDisplacementShader");
	tessDisplacement.AddShaders(GL_VERTEX_SHADER, "Tesselation/Tesselation_DM.vert",
		GL_TESS_CONTROL_SHADER, "Tesselation/Tesselation_DM.tcs",
		GL_TESS_EVALUATION_SHADER, "Tesselation/Tesselation_DM.tes",
		GL_FRAGMENT_SHADER, "StandardShader.frag");

	tessDisplacement.Link();

	m_tessDisplacementPrg = tessDisplacement.m_id;

	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	printf("Max supported patch vertices %d\n", MaxPatchVertices);
	glPatchParameteri(GL_PATCH_VERTICES, 3);

	m_cubeDisplacementMap = assetManager.GetTextureByName("cube_height")->textureID;
}


void TesselationPass::RenderBox(const glm::mat4& view, const glm::mat4& projection)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glUseProgram(m_tessDisplacementPrg);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		OpenGLRenderer::SetUniformMatrix4fv(m_tessDisplacementPrg, "view", view);
		OpenGLRenderer::SetUniformMatrix4fv(m_tessDisplacementPrg, "projection", projection);
		OpenGLRenderer::SetUniformMatrix4fv(m_tessDisplacementPrg, "model", m_modelMat);

		GLuint attrLocation = glGetUniformLocation(m_tessDisplacementPrg, "displacementFactor");
		glUniform1f(attrLocation, 2.0f);

		attrLocation = glGetUniformLocation(m_tessDisplacementPrg, "displacementMap");
		glUniform1i(attrLocation, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_cubeDisplacementMap);

		//Box::DrawCube(m_cubeVAO);
		glBindVertexArray(m_cubeVAO);
			glDrawElements(GL_PATCHES, 36, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
