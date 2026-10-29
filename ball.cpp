#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "loadobj.h"
#include "ball.h"


Ball::Ball(const glm::vec3 pos, const float rad, const float m,const glm::vec3 col)
	: position(pos), radius(rad), mass(m), color(col)
{
}

Ball::~Ball()
{
}

void Ball::initBuffer(GLuint) {
	static ObjMesh sphere_mesh;
	static bool sphere_load_attempted = false;
	if (!sphere_load_attempted) {
		sphere_load_attempted = true;
		load_obj_normalized("sphere.obj", sphere_mesh);
	}
	if (sphere_mesh.vertices.empty() || sphere_mesh.normals.size() != sphere_mesh.vertices.size()) {
		std::cerr << "Unable to initialize the sphere mesh." << std::endl;
		num_Triangle = 0;
		return;
	}
	num_Triangle = static_cast<int>(sphere_mesh.vertices.size());

	glGenVertexArrays(1, &ball_buffer.vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(ball_buffer.vao); //--- VAO를 바인드하기
	glGenBuffers(2, ball_buffer.vbo); //--- 2개의 VBO를 지정하고 할당하기

	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, ball_buffer.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sphere_mesh.vertices.size() * sizeof(glm::vec3), sphere_mesh.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, ball_buffer.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sphere_mesh.normals.size() * sizeof(glm::vec3), sphere_mesh.normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(1);
}

void Ball::Draw(GLuint shader_program) {
	glm::mat4 transform(1.f);
	transform = glm::translate(transform, position);
	transform = glm::scale(transform, glm::vec3(radius, radius, radius));
	GLint location = glGetUniformLocation(shader_program, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));

	int objColorLocation = glGetUniformLocation(shader_program, "objectColor");
	glUniform3f(objColorLocation, color.x, color.y, color.z);


	glBindVertexArray(ball_buffer.vao);
	glDrawArrays(GL_TRIANGLES, 0, num_Triangle);
}


void Ball::setVel(glm::vec2 v) {
	velocity = v;
}

glm::vec3 Ball::getPos() const {
	return position;
}

float Ball::getRad() const {
	return radius;
}

float Ball::getMass() const {
	return mass;
}

glm::vec2 Ball::getVel() const {
	return velocity;
}

float Ball::getvelsize() const {
	return sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y));
}

void Ball::move(float delta_time) {
	position += glm::vec3(velocity.x, 0.f, velocity.y) * delta_time;
}
