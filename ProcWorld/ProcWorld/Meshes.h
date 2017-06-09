#pragma once
#include <GL/glew.h>

namespace meshes
{
	static float quadVertices[] = {
		// Front-face
		// Pos             //Tex       // Norm       
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Top-left
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Top-right
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Bottom-right

		1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Bottom-right
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, //Bottom-left
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Top-left
	};

	inline GLuint GenerateQuad()
	{
		GLuint VAO, VBO;

		// Create VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Create VBO and copy the vertex data to it
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(float), &quadVertices, GL_STATIC_DRAW);

		// Enable attribs
		// Position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
		// Texcoords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		// Normal
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

		glBindVertexArray(0);

		return VAO;
	}

}
