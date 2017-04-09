#include "GeometryPass.h"

#include <GL/glew.h>

#include "AssetManager.h"
#include "mc_cases_lut.h"

// TODO: Get rid of magic numbers and make them settable (but is okey for first assignment)
void GeometryPass::SetupResources(AssetManager& assetManager) 
{
	glGenVertexArrays(1, &m_fakePointsVAO);
	glBindVertexArray(m_fakePointsVAO);

	// Create the buffer with the fake vertex uv data
	{
		glGenBuffers(1, &m_fakePointsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_fakePointsVBO);

		float u = 0.0f, v = 0.0f;
		float step = 1.0f / 96.0f;
		for (size_t i = 0; i < 18432; i += 2)
		{
			m_fakePointsUVs[i] = u;
			m_fakePointsUVs[i + 1] = v;

			u += step;

			if (i % 95 == 0 && i > 0.0f)
			{
				v += step;
				u = 0.0f;
			}
		}

		glBufferData(GL_ARRAY_BUFFER, 18432 * sizeof(float), m_fakePointsUVs, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	}
	
	// Create the 16 buffers for the slices
	{
		glGenBuffers(16, m_sliceTBOs);

		// Resize the transform feedback buffers to hold the vertex datat to generate
		for (size_t i = 0; i < 16; ++i)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_sliceTBOs[i]);
			// TODO: Determine how much data I really need to size the VBOs (definitely not 43700)
			glBufferData(GL_ARRAY_BUFFER, 43700 * 3, nullptr, GL_STATIC_READ);
		}
	}
	
	// Create and link the special shader program with transform feedback support
	{
		ShaderProgram& m_cubes_prg = assetManager.AddShaderSet("Marching_Cubes");
		m_cubes_prg.AddShaders(GL_VERTEX_SHADER, "build_geometry.vert", GL_GEOMETRY_SHADER, "build_geometry.geom");

		// TRANSFORM FEEDBACK settings
		const GLchar* feedbackVaryings[] = { "position" };
		glTransformFeedbackVaryings(m_shaderProgram, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

		m_cubes_prg.Link();

		m_shaderProgram = m_cubes_prg.m_id;
	}

	// Create the lut uniform buffer objects
	{
		glGenBuffers(1, &m_cubeToPolyUBO);
		// Allocate storage for the UBO
		glBindBuffer(GL_UNIFORM_BUFFER, m_cubeToPolyUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uint32_t) * 256, lut::case_to_poly, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &m_triangleUBO);
		// Allocate storage for the UBO
		glBindBuffer(GL_UNIFORM_BUFFER, m_triangleUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(int32_t) * 5120, lut::case_to_tri, GL_DYNAMIC_DRAW);
	}

	// Create buffer for world space Y data
	{
		glGenBuffers(1, &m_wsYposition);
		glBindBuffer(GL_UNIFORM_BUFFER, m_wsYposition);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 256, nullptr, GL_STATIC_DRAW);
	}
}

void GeometryPass::GenerateGeometry(void) 
{
	glUseProgram(m_shaderProgram);

	for (size_t i = 0; i < 16; ++i) 
	{
		for (size_t y = 0; y < 16; ++y) 
		{
			m_wsYPositionData[y] = (y * 10) + (i * 160);
		}

		// Set uniforms for shader draw

			
		glEnable(GL_RASTERIZER_DISCARD);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_sliceTBOs[i]);
		
		glBeginTransformFeedback(GL_TRIANGLES);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 18432, 16);
		glEndTransformFeedback();
		glFlush();
	}
}
