#pragma once
#include <cstdint>

class AssetManager;

class GeometryPass {
public:
	void SetupResources(AssetManager& assetManager);
	void GenerateGeometry(void);


private:
	uint32_t m_shaderProgram;
	// Fake points data
	uint32_t m_fakePointsVAO;
	uint32_t m_fakePointsVBO;
	// Slice geometry
	uint32_t m_sliceTBOs[16];
	uint32_t m_wsYposition;
	// lut buffers
	uint32_t m_cubeToPolyUBO;
	uint32_t m_triangleUBO;
	// fake point data
	float	 m_fakePointsUVs[18432];
	float	 m_wsYPositionData[256];

};
