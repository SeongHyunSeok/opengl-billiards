#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "buffer.h"

class Box
{
	glm::vec3 position;
	float width, depth, height; //가로, 세로, 높이 
	glm::vec3 color;
	BUFFER box_buffer;
	float y_degree;
	

public:
	Box(const glm::vec3, const float, const float, const float, const float, const glm::vec3);

	~Box();

	glm::vec3 getPos();

	void initBuffer(GLuint);

	void Draw(GLuint);

	void set_state(glm::vec3 position, float y_degree);


	float getY_Deg();

	float getW();

	float getD();

};
