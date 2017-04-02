#pragma once
#include <map>
#include "ShaderProgram.h"
#include <mutex>

class AssetManager {
public:
	AssetManager();

	ShaderProgram& AddShaderSet(std::string programName);

	void ShaderWatcher(void);
	void ReloadData(void);

	~AssetManager();
private:
	bool m_listening;
	std::mutex m_shaderGuard;
	std::thread m_shaderWatcher;
	std::map<std::string, ShaderProgram> m_programs;
};
