#ifndef _OGL_TRIANGLE_H_
#define _OGL_TRIANGLE_H_
#include <GL/glew.h>
#include <mat4x4.hpp>
#include <vector>

class AssetManager;
class Camera;
class Triangle;

class OGLTriangle
{
public:
	OGLTriangle(Triangle& tri);

	void Draw(glm::vec3 color, Camera& m_cam, AssetManager& assetManager);

	std::vector<glm::vec3> mVertices;

private:


	GLuint mVBO, mVAO;
	glm::mat4 mModelMatrix;
};

#endif // _OGL_TRIANGLE_H_