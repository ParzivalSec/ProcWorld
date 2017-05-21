#ifndef _RAY_H_
#define _RAY_H_
#include <detail/type_vec3.hpp>

class Camera;

class Ray
{
public:
	Ray(glm::vec3 origin, glm::vec3 direction);

	void Draw(Camera& m_cam);

	glm::vec3 mOrigin;
	glm::vec3 mDir;
	glm::vec3 mInvDir;
	int mSign[3];
};

#endif // _RAY_H_
