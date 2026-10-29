#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <gl/glew.h>

struct BUFFER
{
	GLuint vao = 0;
	GLuint vbo[2] = { 0, 0 };
	GLuint EBO = 0;
};
