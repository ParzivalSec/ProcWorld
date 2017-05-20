#pragma once
#include <detail/type_vec3.hpp>

namespace Particles
{
	struct Particle
	{
		Particle(glm::vec3 pos, float TTL, int type)
			: position(pos)
			, velocity({0, 0, 0})
			, lifeTime(TTL)
			, type(type)
		{}

		glm::vec3 position;
		glm::vec3 velocity;
		float lifeTime;
		int type;
	};
}
