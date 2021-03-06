﻿#pragma once

#include "DensityPass.h"

class AssetManager;

class GeometryPass {
public:
	void SetupResources(AssetManager& assetManager);
	void GenerateGeometry(DensityPass& densityPass);

private:

	void GenerateFakePointsBuffer(void);
	void GenerateSliceBuffers(void);
	void GenerateLUTBuffers(void);

	void SetupMarchinCubeShader(AssetManager& assetManager);

	void GenerateYPosBuffer(void);

public:
	uint32_t m_shaderProgram;
	// Fake points data
	GLuint m_fakePointsVAO;	
	GLuint m_fakePointsVBO;
	// Slice geometry
	uint32_t m_sliceTBOs[16];
	uint32_t m_vericesPerSlice[16];
	uint32_t m_sliceVAOs[16];
	// Y Pos
	uint32_t m_wsYposition;
	uint32_t m_wsYpositionAbove;
	// Get query of drawn primitves
	uint32_t m_query;
	// lut buffers
	uint32_t m_cubeToPolyUBO;
	uint32_t m_triangleUBO;
	// fake point data
	float	 m_fakePointsUVs[18050];
	float	 m_wsYPositionData[256];
	float	 m_wsYPositionAboveData[256];

};
