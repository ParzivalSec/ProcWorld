#include "AssetManager.h"
#include <filesystem>
#include <iostream>
#include "Soil.h"

AssetManager::AssetManager() {
	m_listening = true;
	m_shaderWatcher = std::thread(&AssetManager::ShaderWatcher, this);
}

ShaderProgram& AssetManager::AddShaderSet(std::string programName) {
	auto it = m_programs.insert(std::make_pair(programName, ShaderProgram()));
	return it.first->second;
}

Texture& AssetManager::LoadTexture(std::string fileName, std::string assetName)
{
	if (m_textures.find(assetName) == m_textures.end())
	{
		std::string texturePath(fileName);
		texturePath = "./data/textures/" + texturePath;

		Texture texture;
		glGenTextures(1, &texture.textureID);
		glBindTexture(GL_TEXTURE_2D, texture.textureID);
		int width, height;
		unsigned char* image = SOIL_load_image(texturePath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_textures.insert({ assetName, texture });
	}

	return m_textures.at(assetName);
}

Texture* AssetManager::GetTextureByName(std::string textureName)
{
	if (m_textures.find(textureName) == m_textures.end())
	{
		return &m_textures.at(textureName);
	}
	return nullptr;
}

ShaderProgram* AssetManager::GetShaderByName(std::string shaderName)
{
	if (m_programs.find(shaderName) != m_programs.end())
	{
		return &m_programs.at(shaderName);
	}

	return nullptr;
}

void AssetManager::ShaderWatcher() {
	namespace fs = std::experimental::filesystem;
	using namespace std::chrono_literals;

	while (m_listening) {
		for (auto& program : m_programs) {
			for (auto& shader : program.second.m_shaders) {
				fs::path shaderPath = (fs::current_path() += SHADER_DIR) /= shader.fileName ;
				auto time = fs::last_write_time(shaderPath).time_since_epoch().count();
				auto last_time = shader.timeStamp;

				std::lock_guard<std::mutex> shaderLock(m_shaderGuard);
				if (last_time != 0 && last_time < time) {
					std::cout << "Content changed ( " << shaderPath << " )" << std::endl;
					shader.isDirty = true;
				}

				shader.timeStamp = time;
			}
		}

		// Just run this code once every second
		std::this_thread::sleep_for(1s);
	}
}

void AssetManager::ReloadData() {
	std::lock_guard<std::mutex> shaderLock(m_shaderGuard);
	for (auto& program : m_programs) {
		bool needsReload = false;
		for (auto& shader : program.second.m_shaders) {
			if (shader.isDirty) {
				needsReload = true;
				break;
			}
		}

		if (needsReload) {
			std::cout << "Reloading program [ " << program.first << " ]" << std::endl;
			program.second.Reload();
		}
	}
}

AssetManager::~AssetManager() {
	m_listening = false;
	m_shaderWatcher.join();
}
