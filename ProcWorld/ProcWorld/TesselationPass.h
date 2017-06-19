#pragma once
#include <GL/glew.h>
#include <matrix.hpp>
#include "AssetManager.h"

class TesselationPass
{
public:
	TesselationPass(glm::vec3 cubePosition, glm::vec3 cubeScale);

	void SetupResources(AssetManager& assetManager);
	void RenderBox(const glm::mat4& view, const glm::mat4& projection);

private:
	GLuint m_cubeVAO;
	GLuint m_cubeDisplacementMap;
	GLuint m_standardShaderPrg;
	GLuint m_tessDisplacementPrg;
	glm::mat4 m_modelMat;
};
