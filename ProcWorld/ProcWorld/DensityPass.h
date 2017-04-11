#pragma once
#include <GL/glew.h>
#include "Texture3D.h"
#include "AssetManager.h"

class DensityPass {
public:
	DensityPass(size_t width, size_t height, size_t depth);

	void CreateDensityTexture(void);
	void FillDensityTexture(const ShaderProgram& shader);

	Texture3D m_densityTexture;
	GLuint m_densityFBO;
};
