#include "AssetManager.h"
#include <filesystem>
#include <iostream>

AssetManager::AssetManager() {
	m_listening = true;
	m_shaderWatcher = std::thread(&AssetManager::ShaderWatcher, this);
}

ShaderProgram& AssetManager::AddShaderSet(std::string programName) {
	auto it = m_programs.insert(std::make_pair(programName, ShaderProgram()));
	return it.first->second;
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
