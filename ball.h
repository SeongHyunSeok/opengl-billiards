#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "buffer.h"

class Ball
{
	float radius;
	glm::vec3 color;
	BUFFER ball_buffer;
	int num_Triangle = 0;
	glm::vec2 velocity = glm::vec2(0.f, 0.f);
	float mass;
public:
	Ball(const glm::vec3, const float, const float,const glm::vec3);

	~Ball();


	void initBuffer(GLuint);

	void Draw(GLuint);

	void setVel(glm::vec2);

	glm::vec3 getPos() const;

	float getRad() const;

	float getMass() const;

	float getvelsize() const;

	glm::vec2 getVel() const;
	
	void move(float delta_time);

	glm::vec3 position;
};
