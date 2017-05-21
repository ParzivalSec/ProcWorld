#pragma once
#include <map>
#include "ShaderProgram.h"
#include <mutex>

struct Texture
{
	GLuint textureID;
	std::string textureName;
	std::string fileName;
};

class AssetManager {
public:
	AssetManager();

	ShaderProgram& AddShaderSet(std::string programName);
	Texture& LoadTexture(std::string fileName, std::string assetName);
	Texture* GetTextureByName(std::string textureName);

	ShaderProgram* GetShaderByName(std::string shaderName);

	void ShaderWatcher(void);
	void ReloadData(void);

	~AssetManager();
private:
	bool m_listening;
	std::mutex m_shaderGuard;
	std::thread m_shaderWatcher;
	std::map<std::string, ShaderProgram> m_programs;
	std::map<std::string, Texture> m_textures;
};
