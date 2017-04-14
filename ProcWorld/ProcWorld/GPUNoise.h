#pragma once
#include <cstdint>
#include "ShaderProgram.h"

class AssetManager;

class GPUNoise {
public:
	 
	void GenerateNoiseTextures(uint32_t textureDimension, AssetManager& assetManager);
	void GeneratePerlinNoise(void);

private:
	
	void SetupNoiseShader(AssetManager& assetManager);

public:	
	// Cubic noise: width = height = depth
	uint32_t m_noiseDimension;
	uint32_t m_noiseFBO;
	uint32_t m_noiseTextures[4];
	// ShaderProgram ref
	uint32_t m_noiseShader;
};
