#include "OGLTriangle.h"
#include "OpenGLRenderer.h"
#include "KDTreeController.h"
#include <vector>
#include "Camera.h"
#include <gtc/type_ptr.hpp>
#include "AssetManager.h"

OGLTriangle::OGLTriangle(Triangle& tri)
	: mModelMatrix(glm::mat4(1))
{
	mVertices.push_back(tri.A);
	mVertices.push_back(tri.B);
	mVertices.push_back(tri.C);

	OpenGLRenderer::CreateVertexBuffer(mVBO);
	OpenGLRenderer::CreateVertexArray(mVAO);

	OpenGLRenderer::BindVertexArray(mVAO);
	OpenGLRenderer::BindVertexBuffer(mVBO);
	OpenGLRenderer::LoadVertexBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(mVertices[0]), &mVertices[0], GL_STATIC_DRAW);

	OpenGLRenderer::LinkVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	OpenGLRenderer::EnableVertexAttribute(0);
	OpenGLRenderer::UnbindVertexArray();
}

void OGLTriangle::Draw(glm::vec3 color, Camera& m_cam, AssetManager& assetManager)
{
	GLuint shader = assetManager.GetShaderByName("default")->m_id;

	OpenGLRenderer::UseShader(shader);
	OpenGLRenderer::SetUniformMatrix4fv(shader, "view", m_cam.GetViewMat());
	OpenGLRenderer::SetUniformMatrix4fv(shader, "projection", m_cam.GetProjectionMat());
	OpenGLRenderer::SetUniformMatrix4fv(shader, "model", mModelMatrix);
	OpenGLRenderer::SetUniformVector3(shader, "fragColor", color);

	OpenGLRenderer::BindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	OpenGLRenderer::UnbindVertexArray();
}
