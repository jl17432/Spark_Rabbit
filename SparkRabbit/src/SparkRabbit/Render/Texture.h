#pragma once

#include <glad/glad.h>

namespace SparkRabbit {
	GLuint loadBMP_custom(const char* imagepath);
	GLuint loadDDS(const char* imagepath);
}