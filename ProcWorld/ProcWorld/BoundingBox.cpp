#include "BoundingBox.h"
#include "OpenGLRenderer.h"
#include "AssetManager.h"

// Cube 1x1x1, centered on origin
GLfloat vertices[] = {
	-0.5, -0.5, -0.5,
	0.5, -0.5, -0.5,
	0.5,  0.5, -0.5,
	-0.5,  0.5, -0.5,
	-0.5, -0.5,  0.5,
	0.5, -0.5,  0.5,
	0.5,  0.5,  0.5,
	-0.5,  0.5,  0.5,
};

GLushort elements[] = {
	0, 1, 2, 3,
	4, 5, 6, 7,
	0, 4, 1, 5, 2, 6, 3, 7
};

BoundingBox::BoundingBox()
	: mVBO(0)
	, mVAO(0)
	, mEBO(0)
	, mMin(0, 0, 0)
	, mMax(0, 0, 0)
	, mColor(glm::vec3(0.0f, 1.0f, 0.0f))
{
	SetupRenderRessources();
}

BoundingBox::BoundingBox(glm::vec3 minP, glm::vec3 maxP)
	: mVBO(0)
	, mVAO(0)
	, mEBO(0)
	, mMin(minP)
	, mMax(maxP)
	, mColor(glm::vec3(0.0f, 1.0f, 0.0f))
{
	SetupRenderRessources();
}

void BoundingBox::SetupRenderRessources()
{
	OpenGLRenderer::CreateVertexBuffer(mVBO);
	OpenGLRenderer::CreateElementBuffer(mEBO);
	OpenGLRenderer::CreateVertexArray(mVAO);

	OpenGLRenderer::BindVertexArray(mVAO);
	OpenGLRenderer::BindVertexBuffer(mVBO);
	OpenGLRenderer::LoadVertexBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	OpenGLRenderer::BindElementBuffer(mEBO);
	OpenGLRenderer::LoadElementBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	OpenGLRenderer::LinkVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	OpenGLRenderer::EnableVertexAttribute(0);
	OpenGLRenderer::UnbindVertexArray();
}

void BoundingBox::RecalculateBounds()
{
	glm::vec3 size = glm::vec3(mMax.x - mMin.x, mMax.y - mMin.y, mMax.z - mMin.z);
	glm::vec3 center = glm::vec3((mMin.x + mMax.x) / 2, (mMin.y + mMax.y) / 2, (mMin.z + mMax.z) / 2);
	mModelMatrix = glm::translate(glm::mat4(1), center)* glm::scale(glm::mat4(1), size);
}

void BoundingBox::Draw(AssetManager& assetManager)
{
	GLuint shader = assetManager.GetShaderByName("default")->m_id;

	OpenGLRenderer::UseShader(shader);
	OpenGLRenderer::SetUniformMatrix4fv(shader, "model", mModelMatrix);
	OpenGLRenderer::SetUniformVector3(shader, "fragColor", mColor);

	OpenGLRenderer::BindVertexArray(mVAO);
	//OpenGLRenderer::BindElementBuffer(mEBO);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
	OpenGLRenderer::UnbindVertexArray();
}
