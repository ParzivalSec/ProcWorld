#pragma once

#include <string>
#include <GL/glew.h>

struct Shader {
	Shader() : timeStamp(INT64_MAX), isDirty(false) {}

	GLenum type;
	std::string fileName;
	int id;

	int64_t timeStamp;
	bool isDirty;
};
