#pragma once
#include "Particle.h"
#include <GL/glew.h>
#include <mat4x2.hpp>

class AssetManager;

namespace Particles
{
	class System
	{
	public:
		System(glm::vec3 emitterPosition, float emitterTTL);

		void SetupRenderShader(AssetManager& assetManager);
		void SetupUpdateShader(AssetManager& assetManager);

		void Tick(float deltaTime);
		void Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);

	private:

		void SetupBuffers(void);

		Particle m_emitter;
		GLuint m_VBO[2], m_VAO[2];
		GLuint m_renderShader, m_updateShader;
		GLuint m_query;

		GLuint m_particleCount;

		//// Smoke parameters
		float m_smokeFrequency;
		float m_smokeAccumulator;
		float m_smokeTTL;

		//// Rocket parameters
		float m_rocketFrequencyMin;
		float m_rocketFrequencyMax;
		float m_rocketAccumulator;
		float m_rocketTTL;
		int m_rocketAmount;

		//// Firework parameters
		int m_fireworkAmount;
		float m_fireworkTTL;

		int m_currentBuffer;
	};
}
