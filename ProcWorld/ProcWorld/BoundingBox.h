#ifndef _BOUNDING_BOX_H_
#define _BOUNDING_BOX_H_
#include <GL/glew.h>
#include <detail/type_vec3.hpp>
#include <gtx/transform.hpp>
#include <gtc/matrix_transform.hpp>

class AssetManager;

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(glm::vec3 minP, glm::vec3 maxP);

	void SetupRenderRessources(void);

	void RecalculateBounds(void);

	void SetMin(glm::vec3 minP) { mMin = minP; }
	void SetMax(glm::vec3 maxP) { mMax = maxP; }

	const glm::vec3& GetMin(void) const { return mMin; }
	const glm::vec3& GetMax(void) const { return mMax; }

	void Draw(AssetManager& assetManager);

	glm::vec3 mColor;

private:
	GLuint mVBO, mVAO, mEBO;
	glm::vec3 mMin, mMax;
	glm::mat4 mModelMatrix;
};

#endif // _BOUNDING_BOX_OBJ_H_
