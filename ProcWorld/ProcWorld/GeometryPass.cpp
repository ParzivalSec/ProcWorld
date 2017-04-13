#include "GeometryPass.h"

#include <GL/glew.h>

#include "AssetManager.h"
#include "mc_cases_lut.h"
#include "OpenGLRenderer.h"
#include <iostream>

// TODO: Get rid of magic numbers and make them settable (but is okey for first assignment)
void GeometryPass::SetupResources(AssetManager& assetManager) 
{
	GenerateFakePointsBuffer();
	GenerateSliceBuffers();

	SetupMarchinCubeShader(assetManager);

	// TODO: Add LUT uniforms for GS
	GenerateLUTBuffers();

	GenerateYPosBuffer();
}

void GeometryPass::GenerateGeometry(DensityPass& densityPass)
{
	glEnable(GL_RASTERIZER_DISCARD);

	OpenGLRenderer::UseShader(m_shaderProgram);

	GLint density_vol_loc = glGetUniformLocation(m_shaderProgram, "density_texture");
	glUniform1i(density_vol_loc, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, densityPass.m_densityTexture.textureID);

	uint32_t lut_poly_index = glGetUniformBlockIndex(m_shaderProgram, "lut_poly");
	uint32_t lut_tri_index = glGetUniformBlockIndex(m_shaderProgram, "lut_tri");
	uint32_t y_pos = glGetUniformBlockIndex(m_shaderProgram, "yPos");
	uint32_t y_pos_above = glGetUniformBlockIndex(m_shaderProgram, "yPosAbove");

	glUniformBlockBinding(m_shaderProgram, lut_poly_index, 0);
	glUniformBlockBinding(m_shaderProgram, lut_tri_index, 1);
	glUniformBlockBinding(m_shaderProgram, y_pos, 2);
	glUniformBlockBinding(m_shaderProgram, y_pos_above, 3);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_cubeToPolyUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_triangleUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_wsYposition);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, m_wsYpositionAbove);

	float yStep = (1.0f * (256.0f / 96.0f)) / 256.0f;
	for (size_t i = 0; i < 16; ++i) 
	{
		for (size_t y = 0; y < 16; ++y) 
		{
			// TODO: Fix this strange thing with the Y-Values
			// m_wsYPositionData[y] = (i * 16 * yStep) + (y * yStep);
			m_wsYPositionData[y] = (i * 16 * (1.0f / 256.0f)) + y * (1.0f / 256.0f);
			// m_wsYPositionAboveData[y] = (i * 16 * yStep) + ((y + 1) * yStep);
		}

		glBindBuffer(GL_UNIFORM_BUFFER, m_wsYposition);
		glBufferData(GL_UNIFORM_BUFFER, 256 * sizeof(float), &m_wsYPositionData, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_UNIFORM_BUFFER, m_wsYpositionAbove);
		//glBufferSubData(GL_UNIFORM_BUFFER, 0, 256 * sizeof(float), &m_wsYPositionAboveData);
		glBufferData(GL_UNIFORM_BUFFER, 256 * sizeof(float), &m_wsYPositionAboveData, GL_DYNAMIC_DRAW);

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_sliceTBOs[i]);

		OpenGLRenderer::BindVertexArray(m_fakePointsVAO);
			// Begin: Transform Feedback
			glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, m_query);
			glBeginTransformFeedback(GL_TRIANGLES);
				glDrawArraysInstanced(GL_POINTS, 0, 9025, 16);
			// End: Tranform Feedback
			glEndTransformFeedback();
			glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
		OpenGLRenderer::UnbindVertexArray();
		glGetQueryObjectuiv(m_query, GL_QUERY_RESULT, &m_vericesPerSlice[i]);
		printf("Primites genereated: %d \n", m_vericesPerSlice[i]);
	}

	glFlush();
	glDisable(GL_RASTERIZER_DISCARD);
}

void GeometryPass::GenerateFakePointsBuffer(void) 
{
	float u = -1.0f, v = -1.0f;
	float step = 2.0f / 95.0f;
	for (size_t i = 0; i < 18050; i += 2)
	{
		m_fakePointsUVs[i] = u;
		m_fakePointsUVs[i + 1] = v;

		u += step;

		if (i % 188 == 0 && i > 0.0f)
		{
			v += step;
			u = -1.0f;
		}
	}

	glGenVertexArrays(1, &m_fakePointsVAO);
	glGenBuffers(1, &m_fakePointsVBO);

	glBindVertexArray(m_fakePointsVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_fakePointsVBO);
		glBufferData(GL_ARRAY_BUFFER, 18050 * sizeof(float), &m_fakePointsUVs, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glBindVertexArray(0);
}

void GeometryPass::GenerateSliceBuffers(void) 
{
	glGenQueries(1, &m_query);
	glGenBuffers(16, m_sliceTBOs);

	// Resize the transform feedback buffers to hold the vertex datat to generate
	for (size_t i = 0; i < 16; ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_sliceTBOs[i]);
		// TODO: Determine how much data I really need to size the VBOs (definitely not 43700)
		glBufferData(GL_ARRAY_BUFFER, 288800 * 3 * sizeof(float), nullptr, GL_STATIC_READ);
	}
}

void GeometryPass::GenerateLUTBuffers(void) 
{
	// NUM POLY LUT
	glGenBuffers(1, &m_cubeToPolyUBO);
	// Allocate storage for the UBO
	glBindBuffer(GL_UNIFORM_BUFFER, m_cubeToPolyUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uint32_t) * 256, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, m_cubeToPolyUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 256 * sizeof(uint32_t), &lut::case_to_poly);

	// TRIANGLE LUT
	glGenBuffers(1, &m_triangleUBO);
	// Allocate storage for the UBO
	glBindBuffer(GL_UNIFORM_BUFFER, m_triangleUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(int32_t) * 4096, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, m_triangleUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(int32_t) * 4096, &lut::case_to_tri);
}

void GeometryPass::SetupMarchinCubeShader(AssetManager& assetManager) 
{
	ShaderProgram& m_cubes_prg = assetManager.AddShaderSet("Marching_Cubes");
	m_cubes_prg.AddShaders(GL_VERTEX_SHADER, "visualize_3d_tex.vert", GL_GEOMETRY_SHADER, "visualize_3d_tex.geom");

	m_shaderProgram = m_cubes_prg.m_id;

	const GLchar* feedbackVaryings[] = { "position", "color"};
	glTransformFeedbackVaryings(m_shaderProgram, 2, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

	m_cubes_prg.Link();
}

void GeometryPass::GenerateYPosBuffer(void) 
{
	glGenBuffers(1, &m_wsYposition);
	glBindBuffer(GL_UNIFORM_BUFFER, m_wsYposition);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 256, nullptr, GL_STATIC_DRAW); 

	glGenBuffers(1, &m_wsYpositionAbove);
	glBindBuffer(GL_UNIFORM_BUFFER, m_wsYpositionAbove);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 256, nullptr, GL_STATIC_DRAW);
}
