#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "buffer.h"

class Floor
{
	glm::vec3 position;
	float width, depth; //가로, 세로 
	glm::vec3 color;
	BUFFER floor_buffer;
	float y_degree = 0;
	float z_offset = 0;

public:
	Floor(const glm::vec3, const float, const float, const glm::vec3);

	~Floor();

	glm::vec3 getPos();

	void initBuffer(GLuint);

	void Draw(GLuint);

	void set_state(glm::vec3, float, float);

	void change_col(float);
};